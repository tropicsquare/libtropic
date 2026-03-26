#ifndef R_MEM_READ_H
#define R_MEM_READ_H

#include "usb_devkit_messages.pb.h"

void r_mem_read(const RMemReadCmd *cmd, AppResp *resp);

#endif  // R_MEM_READ_H