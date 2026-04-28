#ifndef R_MEM_WRITE_H
#define R_MEM_WRITE_H

#include "usb_devkit_messages.pb.h"

void r_mem_write(const RMemWriteCmd *cmd, AppResp *resp);

#endif  // R_MEM_WRITE_H