
#ifndef LT_RANDOM_H
#define LT_RANDOM_H

/**
 * @file lt_random.h
 * @brief Declarations of functions related to random number generator
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic_common.h"

/**
 * @brief Get random bytes in a form of 32bit numbers. This is wrapper for platform defined function.
 *
 * @param buff        Buffer to be filled
 * @param len         number of 32bit numbers
 * @return lt_ret_t
 */
lt_ret_t lt_random_bytes(uint32_t *buff, uint16_t len);

#endif
