#ifndef RAW_CMD_COMMON_H
#define RAW_CMD_COMMON_H

#include "usb_devkit_messages.pb.h"

/**
 * @brief Process USB DevKit raw command.
 *
 * @param[in]  cmd   USB DevKit command.
 * @param[out] resp  USB DevKit response.
 */
void process_raw_cmd(const UsbDevkitCmd *cmd, UsbDevkitResp *resp);

#endif  // RAW_CMD_COMMON_H