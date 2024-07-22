#include "ts_common.h"
#include "stdint.h"

#include <sys/random.h>
#include <stdlib.h>

ts_ret_t ts_random_bytes(uint8_t *buff, uint16_t len) {

    getrandom(buff, len, 0);
    return TS_OK;
}

