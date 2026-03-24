#include "usb_devkit_protocol.h"

#include "libtropic.h"
#include "libtropic_common.h"
#include "main.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "usb_devkit_app_commands.h"
#include "usb_devkit_messages.pb.h"

void process_raw_cmd(const UsbDevkitCmd *cmd, UsbDevkitResp *resp)
{
    resp->which_type = UsbDevkitResp_raw_tag;

    switch (cmd->type.raw.which_type) {
        case RawCmd_send_spi_data_tag:
            resp->type.raw.which_type = RawResp_send_spi_data_tag;
            // 1. Drive CS low (only if auto CS mode is on).
            if (auto_cs_mode) {
                HAL_GPIO_WritePin(TR01_CS_GPIO_Port, TR01_CS_Pin, GPIO_PIN_RESET);
            }
            // 2. Do SPI transfer.
            HAL_StatusTypeDef ret = HAL_SPI_TransmitReceive(
                hspi1, cmd->type.raw.type.send_spi_data.tx_data.bytes,
                resp->type.raw.type.send_spi_data.rx_data.bytes,
                cmd->type.raw.type.send_spi_data.tx_data.size,
                cmd->type.raw.type.send_spi_data.timeout_ms);
            // 3. Drive CS high (only if auto CS mode is on).
            if (auto_cs_mode) {
                HAL_GPIO_WritePin(TR01_CS_GPIO_Port, TR01_CS_Pin, GPIO_PIN_SET);
            }
            // 4. Check HAL_SPI_TransmitReceive return value.
            if (ret == HAL_OK) {
                resp->type.raw.type.send_spi_data.rx_data.size =
                    cmd->type.raw.type.send_spi_data.tx_data.size;
            }
            // SendSpiDataRespCode is designed in a way the values are HAL_StatusTypeDef + 1.
            resp->type.raw.type.send_spi_data.res_code = (SendSpiDataRespCode)(ret + 1);
            break;

        case RawCmd_set_auto_cs_mode_tag:
            resp->type.raw.which_type = RawResp_set_auto_cs_mode_tag;
            auto_cs_mode = cmd->type.raw.type.set_auto_cs_mode.on;
            resp->type.raw.type.set_auto_cs_mode.res_code = SET_AUTO_CS_MODE_RESP_CODE_OK;
            break;

        case RawCmd_set_cs_tag:
            resp->type.raw.which_type = RawResp_set_cs_tag;
            // CS can be manually set only if auto CS mode is off.
            if (auto_cs_mode) {
                resp->type.raw.type.set_cs.res_code = SET_CS_RESP_CODE_AUTO_CS_MODE_ON;
            }
            else {
                HAL_GPIO_WritePin(TR01_CS_GPIO_Port, TR01_CS_Pin,
                                  cmd->type.raw.type.set_cs.high ? GPIO_PIN_SET : GPIO_PIN_RESET);
                resp->type.raw.type.set_cs.res_code = SET_CS_RESP_CODE_OK;
            }
            break;

        case RawCmd_set_tr01_pwr_tag:
            resp->type.raw.which_type = RawResp_set_tr01_pwr_tag;
            HAL_GPIO_WritePin(TR01_PWR_GPIO_Port, TR01_PWR_Pin,
                              cmd->type.raw.type.set_tr01_pwr.on ? GPIO_PIN_SET : GPIO_PIN_RESET);
            resp->type.raw.type.set_tr01_pwr.res_code = SET_TR01_PWR_RESP_CODE_OK;
            break;

        case RawCmd_get_gpo_tag:
            resp->type.raw.which_type = RawResp_get_gpo_tag;
            resp->type.raw.type.get_gpo.high = (HAL_GPIO_ReadPin(TR01_GPO_GPIO_Port, TR01_GPO_Pin) ==
                                                GPIO_PIN_SET);
            resp->type.raw.type.get_gpo.res_code = GET_GPO_RESP_CODE_OK;
            break;

        default:
            resp->which_type = UsbDevkitResp_error_tag;
            resp->type.error.res_code = ERROR_RESP_CODE_UNKNOWN_CMD;
            break;
    }
}

void process_app_cmd(const UsbDevkitCmd *cmd, UsbDevkitResp *resp)
{
    resp->which_type = UsbDevkitResp_app_tag;

    switch (cmd->type.app.which_type) {
        case AppCmd_pin_set_tag:
            resp->type.app.which_type = AppResp_pin_set_tag;
            set_pin(&cmd->type.app.type.pin_set, &resp->type.app);
            break;

        case AppCmd_pin_verify_tag:
            resp->type.app.which_type = AppResp_pin_verify_tag;
            verify_pin(&cmd->type.app.type.pin_verify, &resp->type.app);
            break;

            // case AppCmd_r_mem_read_tag:
            //     break;

            // case AppCmd_r_mem_write_tag:
            //     break;

        default:
            resp->which_type = UsbDevkitResp_error_tag;
            resp->type.error.res_code = ERROR_RESP_CODE_UNKNOWN_CMD;
            break;
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