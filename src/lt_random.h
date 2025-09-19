
#ifndef LT_RANDOM_H
#define LT_RANDOM_H

/**
 * @file lt_random.h
 * @brief Declarations of functions related to random number generator
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdlib.h>

#include "libtropic_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get random bytes in a form of 32bit numbers. This is wrapper for platform defined function.
 *
 * @param h           Device's handle
 * @param buff        Buffer to be filled
 * @param count       Number of random bytes
 * @return lt_ret_t
 */
lt_ret_t lt_random_bytes(lt_handle_t *h, void *buff, size_t count) __attribute__((warn_unused_result));

#ifdef __cplusplus
}
#endif

#endif // LT_RANDOM_H
