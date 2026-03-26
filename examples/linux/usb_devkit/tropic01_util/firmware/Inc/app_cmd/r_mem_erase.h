#ifndef R_MEM_ERASE_H
#define R_MEM_ERASE_H

#include "usb_devkit_messages.pb.h"

void r_mem_erase(const RMemEraseCmd *cmd, AppResp *resp);

#endif  // R_MEM_ERASE_H