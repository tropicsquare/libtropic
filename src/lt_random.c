/**
 * @file lt_random.c
 * @brief Definitions of functions related to random number generator
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include "lt_random.h"

#include <stdint.h>

#include "libtropic_common.h"
#include "libtropic_port.h"

lt_ret_t lt_random_bytes(lt_handle_t *h, void *buff, size_t count)
{
#ifdef LT_REDUNDANT_ARG_CHECK
    if (!buff || !h) {
        return LT_PARAM_ERR;
    }
#endif
    return lt_port_random_bytes(&h->l2, buff, count);
}
