#ifndef SET_CS_H
#define SET_CS_H

#include <stdbool.h>

#include "usb_devkit_messages.pb.h"

void set_cs(const SetCsCmd *cmd, SetCsResp *resp, bool auto_cs_mode);

#endif  // SET_CS_H