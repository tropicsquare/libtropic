/**
 * @file lt_random.c
 * @brief Definitions of functions related to random number generator
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "lt_random.h"

#include <stdint.h>

#include "libtropic_common.h"
#include "libtropic_port.h"

lt_ret_t lt_random_bytes(lt_l2_state_t *s2, void *buff, size_t count)
{
#ifdef LT_REDUNDANT_ARG_CHECK
    if (!buff || !s2) {
        return LT_PARAM_ERR;
    }
#endif
    return lt_port_random_bytes(s2, buff, count);
}
