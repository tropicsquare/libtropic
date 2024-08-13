/**
* @file ts_random.c
* @brief Functions related to random number generator
* @author Tropic Square s.r.o.
*/

#include "libtropic_common.h"
#include "libtropic_port.h"


ts_ret_t ts_random_bytes(uint32_t *buff, uint16_t len)
{
#ifdef LIBT_DEBUG
    if(!buff) {
        return TS_PARAM_ERR;
    }
#endif
    return ts_port_random_bytes(buff, len);
}
