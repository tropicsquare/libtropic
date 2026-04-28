#ifndef PIN_VERIFY_H
#define PIN_VERIFY_H

#include "usb_devkit_messages.pb.h"

void pin_verify(const PinVerifyCmd *cmd, AppResp *resp);

#endif  // PIN_VERIFY_H