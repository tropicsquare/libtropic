#ifndef LT_CRC16_H
#define LT_CRC16_H

/**
 * @file lt_crc16.h
 * @brief CRC16 functions declarations
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Calculates CRC16 checksum on a buffer
 *
 * @param buf       Buffer with data
 * @param size      Length if data in buffer
 * @return          CRC16 checksum returned as uint16_t
 */
uint16_t crc16(const uint8_t *buf, int16_t size) __attribute__((warn_unused_result));

/**
 * @brief Takes pointer to filled l2 buffer and adds checksum
 *
 * @note Current implementation rely on that passed data come from l2 functions which always prepare data correctly
 *
 * @param req
 */
void add_crc(void *req);

#ifdef __cplusplus
}
#endif

#endif  // LT_CRC16_H
