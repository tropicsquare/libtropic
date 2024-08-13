
#ifndef TS_RANDOM_H
#define TS_RANDOM_H

/**
* @file ts_random.h
* @brief Functions related to random number generator
* @author Tropic Square s.r.o.
*/

#include "libtropic_common.h"


/**
 * @brief
 *
 * @param buff        Buffer to be filled
 * @param len         number of 32bit numbers
 * @return ts_ret_t
 */
ts_ret_t ts_random_bytes(uint32_t *buff, uint16_t len);

#endif
