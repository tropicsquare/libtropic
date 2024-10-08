/**
 * @file lt_random.c
 * @brief Definitions of functions related to random number generator
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic_common.h"
#include "libtropic_port.h"

lt_ret_t lt_random_bytes(uint32_t *buff, uint16_t len)
{
#ifdef LIBT_DEBUG
    if(!buff) {
        return LT_PARAM_ERR;
    }
#endif
    return lt_port_random_bytes(buff, len);
}
