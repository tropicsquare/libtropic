#ifndef PIN_SET_H
#define PIN_SET_H

#include "usb_devkit_messages.pb.h"

void pin_set(const PinSetCmd *cmd, AppResp *resp);

#endif  // PIN_SET_H