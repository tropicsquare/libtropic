#ifndef SET_AUTO_CS_MODE_H
#define SET_AUTO_CS_MODE_H

#include <stdbool.h>

#include "usb_devkit_messages.pb.h"

void set_auto_cs_mode(const SetAutoCsModeCmd *cmd, SetAutoCsModeResp *resp, bool *auto_cs_mode);

#endif  // SET_AUTO_CS_MODE_H