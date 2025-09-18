/**
 * @file lt_crc16.c
 * @brief CRC16 functions definitions
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "lt_crc16.h"

/* Generator polynomial value used */
#define LT_CRC16_POLYNOMIAL 0x8005

/* Used to initialize the crc value */
#define LT_CRC16_INITIAL_VAL 0x0000

/* The final XOR value is xored to the final CRC value before being returned.
This is done after the 'Result reflected' step. */
#define LT_CRC16_FINAL_XOR_VALUE 0x0000

static uint16_t crc16_byte(uint8_t data, uint16_t crc)
{
    uint16_t current_byte;
    int i;

    current_byte = data;
    crc ^= current_byte << 8;
    i = 8; // Iterate over every bit in a byte.
    do {
        if (crc & 0x8000) { // Highest bit set -> carry -> add generator polynomial of finite field used in CRC calulation.
            crc <<= 1;
            crc ^= LT_CRC16_POLYNOMIAL;
        }
        else {
            crc <<= 1;
        }
    } while (--i);

    return (crc);
}

uint16_t crc16(const uint8_t *data, int16_t len)
{
    uint16_t crc = LT_CRC16_INITIAL_VAL;

    while (--len >= 0) {
        crc = crc16_byte(*data++, crc);
    }

    crc ^= LT_CRC16_FINAL_XOR_VALUE;

    return (crc << 8 | crc >> 8);
}

void add_crc(void *req)
{
    uint8_t *p = (uint8_t *)req;
    // CRC is calculated from REQ_DATA (its size is in p[1]), REQ_ID (+ 1 byte) and REQ_LEN (+ 1 byte),
    // hence total length of data to calculate the CRC from is p[1] + 2.
    // We use p[1], as the offset of the REQ_LEN is 1.
    uint16_t len = p[1] + 2;

    uint16_t crc = crc16(p, len);

    p[len] = crc >> 8;
    p[len + 1] = crc & 0x00FF;
}
