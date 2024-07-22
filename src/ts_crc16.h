#ifndef TS_CRC16_H
#define TS_CRC16_H

/**
* @file ts_crc16.h
* @brief CRC16 functions are defined here
* @author Tropic Square s.r.o.
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
