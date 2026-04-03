#include "frame_protocol.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "app_cmd/app_cmd_common.h"
#include "main.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "raw_cmd/raw_cmd_common.h"
#include "tusb.h"
#include "usb_devkit_messages.pb.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define USB_READ_TIMEOUT_MS 50

static usb_devkit_state_t usb_devkit_state = READ_MAGIC_BYTE_1;

// tinyUSB calls this function when the USB bus is reset or unplugged.
void tud_umount_cb(void) { usb_devkit_state = READ_MAGIC_BYTE_1; }

// tinyUSB calls this function when the USB is plugged and recognized.
void tud_mount_cb(void) { usb_devkit_state = READ_MAGIC_BYTE_1; }

/**
 * @brief Read up to `to_read` bytes from the TROPIC01 Util USB CDC RX FIFO into `buff`.
 *
 * This helper is non-blocking: when no bytes are currently available, it
 * returns 0. When data is available, it reads at most `to_read` bytes.
 *
 * @param[out] buff Destination buffer for received bytes.
 * @param[in] to_read Maximum number of bytes to read in this call.
 * @return Number of bytes actually read.
 */
static size_t usb_read_chunk(uint8_t *buff, size_t to_read)
{
    uint32_t avail_bytes = tud_cdc_n_available(TR01_UTIL_CDC_ITF);
    if (avail_bytes == 0 || to_read == 0) {
        return 0;
    }

    size_t chunk_size = MIN(to_read, (size_t)avail_bytes);
    return (size_t)tud_cdc_n_read(TR01_UTIL_CDC_ITF, buff, chunk_size);
}

/**
 * @brief Write up to `to_write` bytes to the TROPIC01 Util USB CDC TX FIFO from `buff`.
 *
 * This helper is non-blocking: when there is no room in the TX FIFO, it
 * calls tud_cdc_write_flush() and returns 0. When room is available, it
 * writes at most `to_write` bytes.
 *
 * @param[in] buff Source buffer with bytes to write.
 * @param[in] to_write Number of bytes to write in this call.
 * @return Number of bytes actually written.
 */
static size_t usb_write_chunk(const uint8_t *buff, size_t to_write)
{
    uint32_t free_bytes = tud_cdc_n_write_available(TR01_UTIL_CDC_ITF);
    if (free_bytes == 0 || to_write == 0) {
        tud_cdc_n_write_flush(TR01_UTIL_CDC_ITF);
        return 0;
    }

    size_t chunk_size = MIN(to_write, (size_t)free_bytes);
    return (size_t)tud_cdc_n_write(TR01_UTIL_CDC_ITF, buff, chunk_size);
}

void usb_devkit_main_loop(void)
{
    // Variables for the DATA_LEN field in the frame.
    uint8_t in_data_len_bytes[FRAME_DATA_LEN_SIZE];  // Incoming DATA_LEN bytes.
    size_t in_data_len_bytes_read = 0;               // Number of incoming DATA_LEN bytes read so far.
    size_t in_data_len = 0;                          // Parsed incoming DATA_LEN.
    // Variables for DATA field in the frame.
    uint8_t in_data[UsbDevkitCmd_size];     // Incoming DATA bytes.
    size_t in_data_read = 0;                // Number of incoming DATA bytes read so far.
    uint8_t out_frame[OUT_FRAME_MAX_SIZE];  // Outgoing frame bytes.
    size_t out_frame_len = 0;               // Actual number of outgoing frame bytes.
    size_t out_frame_written = 0;           // Number of outgoing frame bytes written so far.
    // Variables for CRC field in the frame.
    uint8_t in_crc[FRAME_CRC_SIZE];  // Incoming CRC bytes.
    size_t in_crc_read = 0;          // Number of incoming CRC bytes read so far.
    while (1) {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */

        // TinyUSB task.
        tud_task();

        switch (usb_devkit_state) {
            case READ_MAGIC_BYTE_1: {
                uint8_t magic_byte_1;
                if (tud_cdc_n_read(TR01_UTIL_CDC_ITF, &magic_byte_1, 1) == 1 &&
                    magic_byte_1 == FRAME_MAGIC_BYTE_1) {
                    usb_devkit_state = READ_MAGIC_BYTE_2;
                }
                break;
            }

            case READ_MAGIC_BYTE_2: {
                uint8_t magic_byte_2;
                if (tud_cdc_n_read(TR01_UTIL_CDC_ITF, &magic_byte_2, 1) == 1) {
                    if (magic_byte_2 == FRAME_MAGIC_BYTE_2) {
                        in_data_len_bytes_read = 0;
                        usb_devkit_state = READ_DATA_LEN;
                    }
                    else {
                        usb_devkit_state = READ_MAGIC_BYTE_1;
                    }
                }
                break;
            }

            case READ_DATA_LEN: {
                uint8_t *read_ptr = in_data_len_bytes + in_data_len_bytes_read;
                size_t to_read = FRAME_DATA_LEN_SIZE - in_data_len_bytes_read;

                in_data_len_bytes_read += usb_read_chunk(read_ptr, to_read);
                if (in_data_len_bytes_read == FRAME_DATA_LEN_SIZE) {
                    // DATA_LEN comes as big-endian.
                    in_data_len = ((size_t)in_data_len_bytes[0] << 8) | (size_t)in_data_len_bytes[1];

                    if (in_data_len == 0 || in_data_len > UsbDevkitCmd_size) {
                        // 1. Construct error response.
                        UsbDevkitResp resp = UsbDevkitResp_init_zero;
                        resp.which_type = UsbDevkitResp_error_tag;
                        resp.type.error.res_code = ERROR_RESP_CODE_BAD_DATA_LEN;
                        if (!construct_resp(&resp, out_frame, sizeof(out_frame), &out_frame_len)) {
                            Error_Handler();
                        }
                        // 2. Prepare for writing.
                        out_frame_written = 0;
                        usb_devkit_state = WRITE_DATA;
                    }
                    else {
                        in_data_read = 0;
                        usb_devkit_state = READ_DATA;
                    }
                }
                break;
            }

            case READ_DATA: {
                uint8_t *read_ptr = in_data + in_data_read;
                size_t to_read = in_data_len - in_data_read;

                in_data_read += usb_read_chunk(read_ptr, to_read);
                if (in_data_read == in_data_len) {
                    in_crc_read = 0;
                    usb_devkit_state = READ_VERIFY_CRC;
                }
                break;
            }

            case READ_VERIFY_CRC: {
                uint8_t *read_ptr = in_crc + in_crc_read;
                size_t to_read = FRAME_CRC_SIZE - in_crc_read;

                in_crc_read += usb_read_chunk(read_ptr, to_read);
                if (in_crc_read == FRAME_CRC_SIZE) {
                    // CRC comes as big-endian.
                    uint16_t in_crc_parsed = ((uint16_t)in_crc[0] << 8) | (uint16_t)in_crc[1];
                    // 1. Calculate CRC for DATA_LEN.
                    uint32_t frame_data_len_word = 0;
                    memcpy(&frame_data_len_word, in_data_len_bytes, FRAME_DATA_LEN_SIZE);
                    HAL_CRC_Calculate(&hcrc, &frame_data_len_word, (uint32_t)FRAME_DATA_LEN_SIZE);
                    // 2. Calculate CRC for DATA.
                    uint32_t in_crc_hw = HAL_CRC_Accumulate(&hcrc, (uint32_t *)(void *)in_data,
                                                            (uint32_t)in_data_len);
                    // 3. Compare CRCs.
                    if (in_crc_parsed == (uint16_t)(in_crc_hw & 0xFFFF)) {
                        usb_devkit_state = PROCESS_DATA;
                    }
                    else {
                        // 1. Construct response.
                        UsbDevkitResp resp = UsbDevkitResp_init_zero;
                        resp.which_type = UsbDevkitResp_error_tag;
                        resp.type.error.res_code = ERROR_RESP_CODE_BAD_CRC;
                        if (!construct_resp(&resp, out_frame, sizeof(out_frame), &out_frame_len)) {
                            Error_Handler();
                        }
                        // 2. Prepare for writing.
                        out_frame_written = 0;
                        usb_devkit_state = WRITE_DATA;
                    }
                }
                break;
            }

            case PROCESS_DATA: {
                // 1. Process incoming DATA and construct response.
                if (!process_data(in_data, in_data_len, out_frame, sizeof(out_frame),
                                  &out_frame_len)) {
                    Error_Handler();
                }

                // 2. Prepare for writing.
                out_frame_written = 0;
                usb_devkit_state = WRITE_DATA;
                break;
            }

            case WRITE_DATA: {
                uint8_t *write_ptr = out_frame + out_frame_written;
                size_t to_write = out_frame_len - out_frame_written;

                out_frame_written += usb_write_chunk(write_ptr, to_write);
                if (out_frame_written == out_frame_len) {
                    tud_cdc_n_write_flush(TR01_UTIL_CDC_ITF);
                    usb_devkit_state = READ_MAGIC_BYTE_1;
                }
                break;
            }
        }
    }
}

bool process_data(const uint8_t *data, size_t data_len, uint8_t *frame_buff, size_t frame_buff_size,
                  size_t *frame_buff_len)
{
    UsbDevkitCmd cmd = UsbDevkitCmd_init_zero;
    UsbDevkitResp resp = UsbDevkitResp_init_zero;

    // 1. Decode command and process it.
    pb_istream_t pb_in = pb_istream_from_buffer(data, data_len);
    if (!pb_decode(&pb_in, UsbDevkitCmd_fields, &cmd)) {
        resp.which_type = UsbDevkitResp_error_tag;
        resp.type.error.res_code = ERROR_RESP_CODE_PB_DECODE;
    }
    else {
        switch (cmd.which_type) {
            case UsbDevkitCmd_raw_tag:
                process_raw_cmd(&cmd, &resp);
                break;

            case UsbDevkitCmd_app_tag:
                process_app_cmd(&cmd, &resp);
                break;

            default:
                resp.which_type = UsbDevkitResp_error_tag;
                resp.type.error.res_code = ERROR_RESP_CODE_UNKNOWN_CMD;
                break;
        }
    }

    // 2. Construct response.
    return construct_resp(&resp, frame_buff, frame_buff_size, frame_buff_len);
}

bool construct_resp(const UsbDevkitResp *resp, uint8_t *frame_buff, size_t frame_buff_size,
                    size_t *frame_buff_len)
{
    // 1. Encode response.
    uint8_t pb_data_out[UsbDevkitResp_size];
    pb_ostream_t pb_out = pb_ostream_from_buffer(pb_data_out, sizeof(pb_data_out));
    if (!pb_encode(&pb_out, UsbDevkitResp_fields, resp)) {
        return false;
    }

    // 2. Construct frame.
    if (!construct_frame(pb_data_out, pb_out.bytes_written, frame_buff, frame_buff_size,
                         frame_buff_len)) {
        return false;
    }

    return true;
}

bool construct_frame(const uint8_t *data, size_t data_len, uint8_t *frame_buff, size_t frame_buff_size,
                     size_t *frame_buff_len)
{
    // 1. Check if buffer for the frame is big enough.
    if (FRAME_MAGIC_BYTES + FRAME_DATA_LEN_SIZE + data_len + FRAME_CRC_SIZE > frame_buff_size) {
        return false;
    }

    // 2. Place magic bytes into the frame.
    frame_buff[0] = FRAME_MAGIC_BYTE_2;
    frame_buff[1] = FRAME_MAGIC_BYTE_1;
    // 3. Place DATA_LEN into the frame.
    frame_buff[2] = (data_len >> 8) & 0xFF;
    frame_buff[3] = data_len & 0xFF;
    // 4. Place DATA into the frame.
    memcpy(frame_buff + 4, data, data_len);

    // 5. Calculate CRC16 over DATA_LEN || DATA and append it as big-endian.
    size_t data_pos = FRAME_MAGIC_BYTES + FRAME_DATA_LEN_SIZE;
    uint32_t data_len_word = 0;  // Needed because HAL_CRC_Calculate accepts uint32_t.
    memcpy(&data_len_word, frame_buff + FRAME_MAGIC_BYTES, FRAME_DATA_LEN_SIZE);
    uint32_t out_crc_hw = HAL_CRC_Calculate(&hcrc, &data_len_word, (uint32_t)FRAME_DATA_LEN_SIZE);
    out_crc_hw = HAL_CRC_Accumulate(&hcrc, (uint32_t *)(void *)(frame_buff + data_pos),
                                    (uint32_t)data_len);
    // 6. Get the calculated CRC.
    uint16_t out_crc = (uint16_t)(out_crc_hw & 0xFFFF);
    size_t out_crc_pos = FRAME_MAGIC_BYTES + FRAME_DATA_LEN_SIZE + data_len;
    // 7. Place CRC into the frame.
    frame_buff[out_crc_pos] = (uint8_t)((out_crc >> 8) & 0xFF);
    frame_buff[out_crc_pos + 1] = (uint8_t)(out_crc & 0xFF);

    // 8. Calculate frame length.
    *frame_buff_len = FRAME_MAGIC_BYTES + FRAME_DATA_LEN_SIZE + data_len + FRAME_CRC_SIZE;
    return true;
}