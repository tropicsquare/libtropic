#ifndef GET_RAND_BYTES_H
#define GET_RAND_BYTES_H

#include "usb_devkit_messages.pb.h"

void get_rand_bytes(const GetRandBytesCmd *cmd, AppResp *resp);

#endif  // GET_RAND_BYTES_H