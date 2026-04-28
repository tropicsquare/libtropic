#include "raw_cmd/send_spi_data.h"

#include <stdbool.h>

#include "main.h"
#include "usb_devkit_messages.pb.h"

void send_spi_data(const SendSpiDataCmd *cmd, SendSpiDataResp *resp, bool auto_cs_mode)
{
    // 1. Drive CS low (only if auto CS mode is on).
    if (auto_cs_mode) {
        HAL_GPIO_WritePin(TR01_CS_GPIO_Port, TR01_CS_Pin, GPIO_PIN_RESET);
    }
    // 2. Do SPI transfer.
    HAL_StatusTypeDef ret = HAL_SPI_TransmitReceive(hspi1, cmd->tx_data.bytes, resp->rx_data.bytes,
                                                    cmd->tx_data.size, cmd->timeout_ms);
    // 3. Drive CS high (only if auto CS mode is on).
    if (auto_cs_mode) {
        HAL_GPIO_WritePin(TR01_CS_GPIO_Port, TR01_CS_Pin, GPIO_PIN_SET);
    }
    // 4. Check HAL_SPI_TransmitReceive return value.
    if (ret == HAL_OK) {
        resp->rx_data.size = cmd->tx_data.size;
    }
    // SendSpiDataRespCode is designed in a way the values are HAL_StatusTypeDef + 1.
    resp->res_code = (SendSpiDataRespCode)(ret + 1);
}