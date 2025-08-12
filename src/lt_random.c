/**
 * @file lt_random.c
 * @brief Definitions of functions related to random number generator
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdlib.h>

#include "libtropic_common.h"
#include "libtropic_port.h"

lt_ret_t lt_random_bytes(lt_l2_state_t *s2, void *buff, size_t count)
{
#ifdef LIBT_DEBUG
    if (!buff || !s2) {
        return LT_PARAM_ERR;
    }
#endif
    return lt_port_random_bytes(s2, buff, count);
}
