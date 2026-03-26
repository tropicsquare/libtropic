#ifndef SEND_SPI_DATA_H
#define SEND_SPI_DATA_H

#include <stdbool.h>

#include "usb_devkit_messages.pb.h"

void send_spi_data(const SendSpiDataCmd *cmd, SendSpiDataResp *resp, bool auto_cs_mode);

#endif  // SEND_SPI_DATA_H