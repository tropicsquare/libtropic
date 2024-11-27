/**
 * @file lt_crc16.c
 * @brief CRC16 functions definitions
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "lt_crc16.h"

/* Generator polynomial value used */
#define CRC16_POLYNOMIAL      0x8005

/* Used to initialize the crc value */
#define CRC16_INITIAL_VAL     0x0000

/* The final XOR value is xored to the final CRC value before being returned.
This is done after the 'Result reflected' step. */
#define CRC16_FINAL_XOR_VALUE 0x0000


uint16_t crc16_byte(uint8_t data, uint16_t crc)
{
    uint16_t current_byte;
    int i;

    current_byte = data;
    crc ^= current_byte << 8;
    i = 8;
    do {
        if (crc & 0x8000) {
            crc <<= 1;
            crc ^= CRC16_POLYNOMIAL;
        }
        else {
            crc <<= 1;
        }
    } while (--i);

    return(crc);
}

uint16_t crc16(const uint8_t *data, int16_t len)
{
    uint16_t crc = CRC16_INITIAL_VAL;

    while (--len >= 0) {
        crc = crc16_byte(*data++, crc);
    }

    crc ^= CRC16_FINAL_XOR_VALUE;

	return (crc << 8 | crc >> 8);
}

// TODO test boundaries of passed req
void add_crc(void *req)
{
	uint8_t *p = (uint8_t *)req;
	uint16_t len = p[1] + 2;

	uint16_t crc = crc16(p, len);

	p[len] = crc >> 8;
	p[len+1] = crc & 0x00FF;
}
