#ifndef GET_GPO_H
#define GET_GPO_H

#include "usb_devkit_messages.pb.h"

void get_gpo(const GetGpoCmd *cmd, GetGpoResp *resp);

#endif  // GET_GPO_H