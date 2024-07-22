#include "ts_common.h"
#include "stdint.h"

//#include <sys/random.h>
//#include <stdlib.h>
// TODO FAKE
ts_ret_t ts_random_bytes(uint8_t *buff, uint16_t len) {

    for(int i=0; i<len; i++) {
        buff[i] = 0xab;
    }
    
    return TS_OK;
}

