#ifndef TS_RANDOM_H
#define TS_RANDOM_H

/**
* @file   ts_random.h
* @brief  API for providing random numbers from host platform RNG
* @author Tropic Square s.r.o.
*/

#include "stdint.h"
#include "ts_common.h"

/**
 * @brief Get a number of random bytes from the host platform RNG
 *
 * @param buff Buffer to be filled with random bytes
 * @param len Number of random bytes
 * @return ts_ret_t TS_OK if all went OK, or other return value.
 */
ts_ret_t ts_random_bytes(uint8_t *buff, uint16_t len);

#endif
