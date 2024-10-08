#ifndef LT_CRC16_H
#define LT_CRC16_H

/**
 * @file lt_crc16.h
 * @brief CRC16 functions declarations
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdint.h>

/**
 * @brief
 *
 * @param buf
 * @param size
 * @return uint16_t
 */
uint16_t crc16(const uint8_t *buf, int16_t size);

/**
 * @brief
 *
 * @param req
 */
void add_crc(void *req);

#endif
