/**
 * @file test_ts_crc.c
 * @author Tropic Square s.r.o.
 * 
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "unity.h"
#include "lt_crc16.h"

void setUp(void)
{
}

void tearDown(void)
{
}

//---------------------------------------------------------------------------------------------------------------------
// Test if crc function calculate known crc bytes
void test_crc16___correct()
{
    uint8_t data[6] = {0x01, 0x02, 0x01, 0x01, 0, 0};
    uint16_t crc = crc16(data, 4);

    TEST_ASSERT_EQUAL_HEX16(0x2e12, crc);
}

//---------------------------------------------------------------------------------------------------------------------
// Prepare (uint8_t*)req as this {0x01, 0x02, 0x01, 0x01, 0x00, 0x00};
// And test if function fills crc bytes instead of last two null bytes
void test_add_crc___correct()
{
    uint8_t req[] = {0x01, 0x02, 0x01, 0x01, 0x00, 0x00};
    add_crc(&req);

    uint8_t expected[] = {0x01, 0x02, 0x01, 0x01, 0x2e, 0x12};
    TEST_ASSERT_EQUAL_INT8_ARRAY(expected, (uint8_t *)&req, 6);
}
