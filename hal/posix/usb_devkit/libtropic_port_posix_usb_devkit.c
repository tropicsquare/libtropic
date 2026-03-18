/**
 * @file libtropic_port_posix_usb_devkit.c
 * @copyright Copyright (c) 2020-2026 Tropic Square s.r.o.
 * @brief Port for communication with TROPIC01 USB DevKit.
 *
 * The TROPIC01 USB DevKit uses a special protocol to translate UART communication to SPI. This port
 * implements the protocol. More info about the USB DevKit here:
 * https://github.com/tropicsquare/devboards/tree/main/TROPIC01_USB_Devkit
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include "libtropic_port_posix_usb_devkit.h"

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "libtropic_common.h"
#include "libtropic_logging.h"
#include "libtropic_macros.h"
#include "libtropic_port.h"
#include "lt_crc16.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "usb_devkit_messages.pb.h"

#if LT_USE_INT_PIN
#error "Interrupt PIN not supported in the USB DevKit port!"
#endif

#define LT_USB_DT_NEW_FW_FRAME_MAGIC_BYTE_1 0xAA
#define LT_USB_DT_NEW_FW_FRAME_MAGIC_BYTE_2 0x55
#define LT_USB_DT_NEW_FW_FRAME_MAGIC_BYTES 2
#define LT_USB_DT_NEW_FW_FRAME_DATA_LEN_SIZE 2
#define LT_USB_DT_NEW_FW_FRAME_DATA_MAX_SIZE 4093
#define LT_USB_DT_NEW_FW_FRAME_CRC_SIZE 2
#define LT_USB_DT_NEW_FW_OUT_FRAME_MAX_SIZE                                                          \
    (LT_USB_DT_NEW_FW_FRAME_MAGIC_BYTES + LT_USB_DT_NEW_FW_FRAME_DATA_LEN_SIZE + UsbDevkitCmd_size + \
     LT_USB_DT_NEW_FW_FRAME_CRC_SIZE)

/**
 * @brief Macro used for identifying which FW running on the USB DevKit (legacy FW or not).
 *
 * @note We send magic bytes (used by newer FW), two 0x00 bytes (DATA_LEN in the newer FW) and '\n'
 * (used by legacy FW). This sequence should trigger "ERROR: unknown command" in the legacy FW and
 * ErrorResp with ERROR_RESP_CODE_BAD_DATA_LEN in the newer FW.
 */
#define LT_USB_DT_FW_ID_MSG \
    {LT_USB_DT_NEW_FW_FRAME_MAGIC_BYTE_1, LT_USB_DT_NEW_FW_FRAME_MAGIC_BYTE_2, 0x00, 0x00, '\n'}

// getentropy() has a limit of random bytes it can generate in one call. The POSIX.1-2024 standard
// requires GETENTROPY_MAX to be defined in limits.h, but because this standard is quite new, we will
// define the macro here in case the current limits.h does not define it yet. The value 256 is safe to
// use because it was always the minimum value.
#ifndef GETENTROPY_MAX
#define GETENTROPY_MAX 256
#endif

// Maximum number of consecutive EINTR retries for read()/write() operations.
#define LT_PORT_EINTR_RETRY_MAX 10U

/**
 * @brief Writes data to a serial port (specified by fd).
 * @note Returns after all bytes have been written or if an error occurred (EINTR is tolerated).
 *
 * @param[in] fd      File descriptor of the port to write to.
 * @param[in] buffer  Pointer to the buffer containing the data to be written.
 * @param[in] size    Size of the data in bytes to be written from the buffer.
 *
 * @return Returns true on success, false on error.
 */
static bool write_port(int fd, const uint8_t *buffer, size_t size)
{
    size_t written_total = 0;
    size_t eintr_retries = 0;  // Used for tracking retries upon receiving EINTR.

    while (written_total < size) {
        ssize_t written_bytes = write(fd, buffer + written_total, size - written_total);
        if (written_bytes < 0) {
            if (errno == EINTR) {
                if (++eintr_retries > LT_PORT_EINTR_RETRY_MAX) {
                    LT_LOG_ERROR("write() returned EINTR too many times (max=%u).",
                                 LT_PORT_EINTR_RETRY_MAX);
                    return false;
                }
                LT_LOG_INFO("write() interrupted by a signal, will try again.");
                continue;
            }
            LT_LOG_ERROR("write() failed: errno=%d (%s).", errno, strerror(errno));
            return false;
        }

        eintr_retries = 0;

        if (written_bytes == 0) {
            LT_LOG_ERROR("Failed to write to port (write() returned 0).");
            return false;
        }

        written_total += (size_t)written_bytes;
    }

    return true;
}

/**
 * @brief Reads data from a serial port (specified by fd).
 * @note Returns after all bytes have been read, after a timeout or if an error occurred (EINTR is
 * tolerated).
 *
 * @param[in]  fd      The file descriptor to read from.
 * @param[out] buffer  Pointer to the buffer where the read data will be stored.
 * @param[in]  size    Number of bytes to read into the buffer.
 *
 * @return Returns true on success, false on error.
 */
static bool read_port(int fd, uint8_t *buffer, size_t size)
{
    size_t received = 0;
    size_t eintr_retries = 0;  // Used for tracking retries upon receiving EINTR.

    while (received < size) {
        ssize_t read_bytes = read(fd, buffer + received, size - received);
        if (read_bytes < 0) {
            if (errno == EINTR) {
                if (++eintr_retries > LT_PORT_EINTR_RETRY_MAX) {
                    LT_LOG_ERROR("read() returned EINTR too many times (max=%u).",
                                 LT_PORT_EINTR_RETRY_MAX);
                    return false;
                }
                LT_LOG_INFO("read() interrupted by a signal, will try again.");
                continue;
            }
            LT_LOG_ERROR("read() failed: errno=%d (%s).", errno, strerror(errno));
            return false;
        }

        eintr_retries = 0;

        if (read_bytes == 0) {
            LT_LOG_ERROR("Failed to read from port (read() returned 0): timeout or EOF.");
            return false;
        }

        received += (size_t)read_bytes;
    }

    return true;
}

/**
 * @brief Construct the frame with a command to send to TROPIC01 USB DevKit with the new FW.
 *
 * @param[in]  data             DATA field in the frame.
 * @param[in]  data_len         DATA_LEN field in the frame.
 * @param[out] frame_buff       Constructed frame.
 * @param[in]  frame_buff_size  Size of `frame_buff`.
 * @param[out] frame_buff_len   Constructed frame length (number of bytes written to `frame_buff`).
 * @retval     true             Frame constructed successfully.
 * @retval     false            Frame could not be constructed, `frame_buff` is too small.
 */
static bool construct_frame(const uint8_t *data, size_t data_len, uint8_t *frame_buff,
                            size_t frame_buff_size, size_t *frame_buff_len)
{
    if (LT_USB_DT_NEW_FW_FRAME_MAGIC_BYTES + LT_USB_DT_NEW_FW_FRAME_DATA_LEN_SIZE + data_len +
            LT_USB_DT_NEW_FW_FRAME_CRC_SIZE >
        frame_buff_size) {
        return false;
    }
    // 1. Place magic bytes into the frame.
    frame_buff[0] = LT_USB_DT_NEW_FW_FRAME_MAGIC_BYTE_1;
    frame_buff[1] = LT_USB_DT_NEW_FW_FRAME_MAGIC_BYTE_2;
    // 2. Place DATA_LEN into the frame.
    frame_buff[2] = (data_len >> 8) & 0xFF;
    frame_buff[3] = data_len & 0xFF;
    // 3. Place DATA into the frame.
    memcpy(frame_buff + 4, data, data_len);

    // 4. Calculate CRC16 over DATA_LEN || DATA and append it as big-endian.
    uint16_t crc = crc16(frame_buff + LT_USB_DT_NEW_FW_FRAME_MAGIC_BYTES,
                         (int16_t)(LT_USB_DT_NEW_FW_FRAME_DATA_LEN_SIZE + data_len));
    // 5. Place CRC into the frame.
    size_t crc_pos = LT_USB_DT_NEW_FW_FRAME_MAGIC_BYTES + LT_USB_DT_NEW_FW_FRAME_DATA_LEN_SIZE +
                     data_len;
    frame_buff[crc_pos] = (uint8_t)(crc & 0xFF);
    frame_buff[crc_pos + 1] = (uint8_t)((crc >> 8) & 0xFF);

    // 6. Calculate frame length.
    *frame_buff_len = LT_USB_DT_NEW_FW_FRAME_MAGIC_BYTES + LT_USB_DT_NEW_FW_FRAME_DATA_LEN_SIZE +
                      data_len + LT_USB_DT_NEW_FW_FRAME_CRC_SIZE;
    return true;
}

static bool send_usb_devkit_cmd(int fd, const UsbDevkitCmd *cmd)
{
    // 1. Encode command as protobuf payload.
    uint8_t pb_data[UsbDevkitCmd_size];
    pb_ostream_t pb_ostream = pb_ostream_from_buffer(pb_data, sizeof(pb_data));
    if (!pb_encode(&pb_ostream, UsbDevkitCmd_fields, cmd)) {
        LT_LOG_ERROR("Failed to encode UsbDevkitCmd: %s", PB_GET_ERROR(&pb_ostream));
        return false;
    }

    // 2. Construct frame.
    uint8_t frame[LT_USB_DT_NEW_FW_OUT_FRAME_MAX_SIZE];
    size_t frame_len = 0;
    if (!construct_frame(pb_data, pb_ostream.bytes_written, frame, sizeof(frame), &frame_len)) {
        LT_LOG_ERROR("Failed to construct frame.");
        return false;
    }
    // 3. Send frame.
    if (!write_port(fd, frame, frame_len)) {
        LT_LOG_ERROR("Failed to send UsbDevkitCmd.");
        return false;
    }

    return true;
}

static bool recv_usb_devkit_resp(int fd, UsbDevkitResp *resp)
{
    // 1. Read magic bytes.
    uint8_t magic_bytes[LT_USB_DT_NEW_FW_FRAME_MAGIC_BYTES];
    if (!read_port(fd, magic_bytes, sizeof(magic_bytes))) {
        LT_LOG_ERROR("Failed to read magic bytes.");
        return false;
    }
    // 2. Check magic bytes.
    if (magic_bytes[0] != LT_USB_DT_NEW_FW_FRAME_MAGIC_BYTE_2 ||
        magic_bytes[1] != LT_USB_DT_NEW_FW_FRAME_MAGIC_BYTE_1) {
        LT_LOG_ERROR("Received unexpected magic bytes: 0x%02" PRIx8 ", 0x%02" PRIx8, magic_bytes[0],
                     magic_bytes[1]);
        return false;
    }

    // 3. Read DATA_LEN.
    // Using one buffer for DATA_LEN and DATA for easier CRC calculation.
    uint8_t data_len_and_data_bytes[LT_USB_DT_NEW_FW_FRAME_DATA_LEN_SIZE + UsbDevkitResp_size];
    if (!read_port(fd, data_len_and_data_bytes, LT_USB_DT_NEW_FW_FRAME_DATA_LEN_SIZE)) {
        LT_LOG_ERROR("Failed to read DATA_LEN bytes.");
        return false;
    }
    // DATA_LEN comes as big-endian.
    size_t data_len = 0;
    data_len = ((size_t)data_len_and_data_bytes[0] << 8) | (size_t)data_len_and_data_bytes[1];
    if (data_len == 0 || data_len > UsbDevkitResp_size) {
        LT_LOG_ERROR("Unexpected DATA_LEN=%zu.", data_len);
        return false;
    }

    // 4. Read DATA.
    if (!read_port(fd, data_len_and_data_bytes + LT_USB_DT_NEW_FW_FRAME_DATA_LEN_SIZE, data_len)) {
        LT_LOG_ERROR("Failed to read DATA.");
        return false;
    }

    // 5. Read CRC.
    uint16_t crc;
    if (!read_port(fd, (uint8_t *)&crc, sizeof(crc))) {
        LT_LOG_ERROR("Failed to read CRC.");
        return false;
    }
    // 6. Verify CRC.
    uint16_t crc_calc = crc16(data_len_and_data_bytes,
                              (int16_t)(LT_USB_DT_NEW_FW_FRAME_DATA_LEN_SIZE + data_len));

    if (crc != crc_calc) {
        LT_LOG_ERROR("CRC mismatch in USB DevKit response (received=0x%04" PRIx16
                     ", calculated=0x%04" PRIx16 ").",
                     crc, crc_calc);
        return false;
    }

    // 7. Decode DATA.
    pb_istream_t pb_istream = pb_istream_from_buffer(
        data_len_and_data_bytes + LT_USB_DT_NEW_FW_FRAME_DATA_LEN_SIZE, data_len);
    if (!pb_decode(&pb_istream, UsbDevkitResp_fields, resp)) {
        LT_LOG_ERROR("Failed to decode UsbDevkitResp: %s", PB_GET_ERROR(&pb_istream));
        return false;
    }

    return true;
}

lt_ret_t lt_port_init(lt_l2_state_t *s2)
{
    lt_dev_posix_usb_devkit_t *device = (lt_dev_posix_usb_devkit_t *)s2->device;

    device->legacy_fw = false;

    // Initialize the serial port.
    device->fd = open(device->dev_path, O_RDWR | O_NOCTTY);
    if (device->fd == -1) {
        LT_LOG_ERROR("Error opening serial at \"%s\".", device->dev_path);
        return LT_FAIL;
    }

    // Flush away any bytes previously read or written.
    int result = tcflush(device->fd, TCIOFLUSH);
    if (result) {
        // just a warning, not a fatal error
        LT_LOG_WARN("tcflush failed, result=%d", result);
    }

    // Get the current configuration of the serial port.
    struct termios options;
    result = tcgetattr(device->fd, &options);
    if (result) {
        LT_LOG_ERROR("tcgetattr failed, result=%d", result);
        close(device->fd);
        return LT_FAIL;
    }

    // Turn off any options that might interfere with our ability to send and
    // receive raw binary bytes.
    options.c_iflag &= ~(INLCR | IGNCR | ICRNL | IXON | IXOFF);
    options.c_oflag &= ~(ONLCR | OCRNL);
    options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

    // Set up timeouts: Calls to read() will return as soon as there is
    // at least one byte available or when 100 ms has passed.
    options.c_cc[VTIME] = 1;
    options.c_cc[VMIN] = 0;

    // This code only supports certain standard baud rates. Supporting
    // non-standard baud rates should be possible but takes more work.
    switch (device->baud_rate) {
        case 4800:
            cfsetospeed(&options, B4800);
            break;
        case 9600:
            cfsetospeed(&options, B9600);
            break;
        case 19200:
            cfsetospeed(&options, B19200);
            break;
        case 38400:
            cfsetospeed(&options, B38400);
            break;
        case 115200:
            cfsetospeed(&options, B115200);
            break;
        default:
            LT_LOG_WARN("Baud rate %" PRIu32 " is not supported, using 9600.\n", device->baud_rate);
            cfsetospeed(&options, B9600);
            break;
    }
    cfsetispeed(&options, cfgetospeed(&options));

    result = tcsetattr(device->fd, TCSANOW, &options);
    if (result) {
        LT_LOG_ERROR("tcsetattr failed, result=%d", result);
        close(device->fd);
        return LT_FAIL;
    }

    // Check what FW is USB DevKit running:
    // 1. Send the identification message.
    uint8_t fw_id_msg[] = LT_USB_DT_FW_ID_MSG;
    if (!write_port(device->fd, fw_id_msg, sizeof(fw_id_msg))) {
        LT_LOG_INFO("Failed to send a message to identify USB DevKit FW.");
        close(device->fd);
        return LT_FAIL;
    }
    // 2. Receive response.
    // We read only 2 bytes, as that should be enough to indentify the FW.
    uint8_t fw_id_msg_resp[2];
    if (!read_port(device->fd, fw_id_msg_resp, sizeof(fw_id_msg_resp))) {
        LT_LOG_ERROR("Failed to read USB DevKit FW identification response.");
        close(device->fd);
        return LT_FAIL;
    }
    // 3. Check the response.
    if (fw_id_msg_resp[0] == LT_USB_DT_NEW_FW_FRAME_MAGIC_BYTE_2 &&
        fw_id_msg_resp[1] == LT_USB_DT_NEW_FW_FRAME_MAGIC_BYTE_1) {
        device->legacy_fw = false;
    }
    else if (fw_id_msg_resp[0] == 'E' && fw_id_msg_resp[1] == 'R') {
        device->legacy_fw = true;
    }
    else {
        LT_LOG_ERROR("Could not identify USB DevKit FW.");
        close(device->fd);
        return LT_FAIL;
    }

    // 4. Drop the rest of the response.
    // Both USB DevKit FWs send more than 2 bytes -> we drop the remaining unread bytes, so we start
    // clean.
    if (0 != tcflush(device->fd, TCIFLUSH)) {
        LT_LOG_ERROR("tcflush() failed: errno=%d (%s).", errno, strerror(errno));
        close(device->fd);
        return LT_FAIL;
    }

    if (!device->legacy_fw) {
        // 1. Prepare command for disabling auto CS mode.
        UsbDevkitCmd cmd = UsbDevkitCmd_init_zero;
        cmd.which_type = UsbDevkitCmd_raw_tag;
        cmd.type.raw.which_type = RawCmd_set_auto_cs_mode_tag;
        cmd.type.raw.type.set_auto_cs_mode.on = false;

        if (!send_usb_devkit_cmd(device->fd, &cmd)) {
            close(device->fd);
            return LT_FAIL;
        }

        // 2. Get response.
        UsbDevkitResp resp = UsbDevkitResp_init_zero;
        if (!recv_usb_devkit_resp(device->fd, &resp)) {
            close(device->fd);
            return LT_FAIL;
        }

        // 3. Check response.
        if (resp.which_type != UsbDevkitResp_raw_tag) {
            LT_LOG_ERROR("Received unexpected UsbDevkitResp tag=%d.", resp.which_type);
            close(device->fd);
            return LT_FAIL;
        }
        if (resp.type.raw.result_code != RAW_RESP_RESULT_CODE_OK) {
            LT_LOG_ERROR("Auto CS mode was not disabled, result_code=%d.", resp.type.raw.result_code);
            close(device->fd);
            return LT_FAIL;
        }
    }

    return LT_OK;
}

lt_ret_t lt_port_deinit(lt_l2_state_t *s2)
{
    lt_dev_posix_usb_devkit_t *device = (lt_dev_posix_usb_devkit_t *)s2->device;

    device->legacy_fw = false;

    if (close(device->fd)) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_port_delay(lt_l2_state_t *s2, uint32_t ms)
{
    LT_UNUSED(s2);
    int ret = usleep(ms * 1000);
    if (ret != 0) {
        LT_LOG_ERROR("usleep() failed: %s (%d)", strerror(errno), ret);
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_port_random_bytes(lt_l2_state_t *s2, void *buff, size_t count)
{
    LT_UNUSED(s2);

    uint8_t *buff_ptr = buff;
    size_t bytes_left = count;
    size_t current_cnt;

    // Number of bytes getentropy() can generate is limited to GETENTROPY_MAX,
    // so generate random data in chunks.
    while (bytes_left) {
        current_cnt = bytes_left > GETENTROPY_MAX ? GETENTROPY_MAX : bytes_left;

        if (0 != getentropy(buff_ptr, current_cnt)) {
            LT_LOG_ERROR("lt_port_random_bytes: getentropy() failed (%s)!", strerror(errno));
            return LT_FAIL;
        }

        buff_ptr += current_cnt;
        bytes_left -= current_cnt;
    }

    return LT_OK;
}

lt_ret_t lt_port_spi_csn_low(lt_l2_state_t *s2)
{
    lt_dev_posix_usb_dongle_t *device = (lt_dev_posix_usb_dongle_t *)s2->device;

    // In legacy USB DevKit FW, CS LOW is handled automatically when SPI transfer is executed.
    if (!device->legacy_fw) {
        // 1. Prepare command for driving CS low.
        UsbDevkitCmd cmd = UsbDevkitCmd_init_zero;
        cmd.which_type = UsbDevkitCmd_raw_tag;
        cmd.type.raw.which_type = RawCmd_set_cs_tag;
        cmd.type.raw.type.set_cs.high = false;

        if (!send_usb_devkit_cmd(device->fd, &cmd)) {
            return LT_FAIL;
        }

        // 2. Get response.
        UsbDevkitResp resp = UsbDevkitResp_init_zero;
        if (!recv_usb_devkit_resp(device->fd, &resp)) {
            return LT_FAIL;
        }

        // 3. Check response.
        if (resp.which_type != UsbDevkitResp_raw_tag) {
            LT_LOG_ERROR("Received unexpected UsbDevkitResp tag=%d.", resp.which_type);
            return LT_FAIL;
        }
        if (resp.type.raw.result_code != RAW_RESP_RESULT_CODE_OK) {
            LT_LOG_ERROR("CS was not driven low, result_code=%d.", resp.type.raw.result_code);
            return LT_FAIL;
        }
    }

    return LT_OK;
}

lt_ret_t lt_port_spi_csn_high(lt_l2_state_t *s2)
{
    lt_dev_posix_usb_devkit_t *device = (lt_dev_posix_usb_devkit_t *)s2->device;

    if (device->legacy_fw) {
        uint8_t cs_high[] = "CS=0\n";  // Yes, CS=0 really means that CSN is low
        if (!write_port(device->fd, cs_high, 5)) {
            LT_LOG_ERROR("Failed to send CS=0 command.");
            return LT_L1_SPI_ERROR;
        }

        uint8_t buff[4];
        if (!read_port(device->fd, buff, 4)) {
            LT_LOG_ERROR("Failed to read response for CS=0 command.");
            return LT_L1_SPI_ERROR;
        }

        if (memcmp(buff, "OK\r\n", 4) != 0) {
            LT_LOG_ERROR("USB DevKit did not ACK CS=0 command.");
            return LT_L1_SPI_ERROR;
        }
    }
    else {
        // 1. Prepare command for driving CS low.
        UsbDevkitCmd cmd = UsbDevkitCmd_init_zero;
        cmd.which_type = UsbDevkitCmd_raw_tag;
        cmd.type.raw.which_type = RawCmd_set_cs_tag;
        cmd.type.raw.type.set_cs.high = true;

        if (!send_usb_devkit_cmd(device->fd, &cmd)) {
            return LT_FAIL;
        }

        // 2. Get response.
        UsbDevkitResp resp = UsbDevkitResp_init_zero;
        if (!recv_usb_devkit_resp(device->fd, &resp)) {
            return LT_FAIL;
        }

        // 3. Check response.
        if (resp.which_type != UsbDevkitResp_raw_tag) {
            LT_LOG_ERROR("Received unexpected UsbDevkitResp tag=%d.", resp.which_type);
            return LT_FAIL;
        }
        if (resp.type.raw.result_code != RAW_RESP_RESULT_CODE_OK) {
            LT_LOG_ERROR("CS was not driven high, result_code=%d.", resp.type.raw.result_code);
            return LT_FAIL;
        }
    }

    return LT_OK;
}

lt_ret_t lt_port_spi_transfer(lt_l2_state_t *s2, uint8_t offset, uint16_t tx_data_length,
                              uint32_t timeout_ms)
{
    lt_dev_posix_usb_devkit_t *device = (lt_dev_posix_usb_devkit_t *)s2->device;

    if (offset + tx_data_length > TR01_L1_LEN_MAX) {
        LT_LOG_ERROR("Invalid data length!");
        return LT_L1_DATA_LEN_ERROR;
    }

    if (device->legacy_fw) {
        LT_UNUSED(timeout_ms);
        // Bytes from handle which are about to be sent are encoded as chars and stored to
        // buffered_chars.
        uint8_t buffered_chars[LT_USB_DEVKIT_SPI_TRANSFER_BUFF_SIZE_MAX] = {0};
        for (int i = 0; i < tx_data_length; i++) {
            sprintf((char *)(buffered_chars + i * 2), "%02" PRIX8, s2->buff[i + offset]);
        }

        // Control characters to keep CS LOW (they are expected by USB DevKit, see the top of this file
        // for more information).
        buffered_chars[tx_data_length * 2] = 'x';
        buffered_chars[tx_data_length * 2 + 1] = '\n';

        if (!write_port(device->fd, buffered_chars, (tx_data_length * 2) + 2)) {
            LT_LOG_ERROR("Failed to write SPI payload.");
            return LT_L1_SPI_ERROR;
        }

        lt_port_delay(s2, LT_USB_DEVKIT_READ_WRITE_DELAY);

        if (!read_port(device->fd, buffered_chars, (2 * tx_data_length) + 2)) {
            LT_LOG_ERROR("Failed to read SPI payload.");
            return LT_L1_SPI_ERROR;
        }

        for (size_t count = 0; count < tx_data_length; count++) {
            sscanf((char *)&buffered_chars[count * 2], "%02" SCNx8, &s2->buff[count + offset]);
        }
    }
    else {
        // 1. Prepare command for driving CS low.
        UsbDevkitCmd cmd = UsbDevkitCmd_init_zero;
        cmd.which_type = UsbDevkitCmd_raw_tag;
        cmd.type.raw.which_type = RawCmd_send_spi_data_tag;
        memcpy(cmd.type.raw.type.send_spi_data.tx_data.bytes, s2->buff + offset, tx_data_length);
        cmd.type.raw.type.send_spi_data.tx_data.size = tx_data_length;
        cmd.type.raw.type.send_spi_data.timeout_ms = timeout_ms;

        // 2. Send command.
        if (!send_usb_devkit_cmd(device->fd, &cmd)) {
            return LT_FAIL;
        }
        // 3. Get response.
        UsbDevkitResp resp = UsbDevkitResp_init_zero;
        if (!recv_usb_devkit_resp(device->fd, &resp)) {
            return LT_FAIL;
        }

        // 4. Check response.
        if (resp.which_type != UsbDevkitResp_raw_tag) {
            LT_LOG_ERROR("Received unexpected UsbDevkitResp tag=%d.", resp.which_type);
            return LT_FAIL;
        }
        if (resp.type.raw.result_code != RAW_RESP_RESULT_CODE_OK) {
            LT_LOG_ERROR("RawRespResultCode is not OK, result_code=%d.", resp.type.raw.result_code);
            return LT_FAIL;
        }
        if (resp.type.raw.which_type != RawResp_send_spi_data_tag) {
            LT_LOG_ERROR("Received unexpected RawResp tag=%d.", resp.type.raw.which_type);
            return LT_FAIL;
        }
        // 4. Get the received SPI payload.
        memcpy(s2->buff + offset, resp.type.raw.type.send_spi_data.rx_data.bytes, tx_data_length);
    }

    return LT_OK;
}

int lt_port_log(const char *format, ...)
{
    va_list args;
    int ret;

    va_start(args, format);
    ret = vfprintf(stderr, format, args);
    fflush(stderr);
    va_end(args);

    return ret;
}