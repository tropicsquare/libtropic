
#include "unity.h"

#include "ts_common.h"
#include "ts_l2.h"

#include "mock_ts_l1.h"
#include "mock_ts_crc16.h"


// Unknown byte, rubbish
#define INVALID_BYTE 0xfe

/*
[ ] pridat popis erroru do verbose output funkce
*/

void setUp(void)
{
}

void tearDown(void)
{
}

static uint8_t test_data[] = {0x00,
                           0x00,
                           0x80,
                           0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
                           0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
                           0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
                           0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
                           0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
                           0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
                           0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
                           0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
                           0x2e,0x4e};


/* Test if function returns when first byte contains BUSY bit */

/* Check function's return value on correct frame */
void test_ts_l2_frame_check___CHIP_STATUS_READY__TS_OK()
{
    test_data[0] = CHIP_STATUS_READY;
    test_data[1] = L2_STATUS_RESULT_OK;
    crc16_IgnoreAndReturn(0x2e4e);
    TEST_ASSERT_EQUAL(TS_OK, ts_l2_frame_check(test_data));
}

/* Check what function returns when frame looks valid (from first two bytes), but frame's CRC check fails.
This may happen when payload is altered during transmission */
void test_ts_l2_frame_check___CHIP_STATUS_READY__TS_CRC_ERR()
{
    test_data[0] = CHIP_STATUS_READY;
    test_data[1] = L2_STATUS_REQUEST_OK;
    crc16_IgnoreAndReturn(0xdead);
    TEST_ASSERT_EQUAL(TS_L2_IN_CRC_ERR, ts_l2_frame_check(test_data));
}

/* Check what function returns when frame looks valid (from first two bytes), but frame's CRC check fails.
This may happen when payload is altered during transmission */
void test_ts_l2_frame_check___CHIP_STATUS_READY__TS_CRC_ERR_2()
{
    test_data[0] = CHIP_STATUS_READY;
    test_data[1] = L2_STATUS_RESULT_OK;
    crc16_IgnoreAndReturn(0xdead);
    TEST_ASSERT_EQUAL(TS_L2_IN_CRC_ERR, ts_l2_frame_check(test_data));
}

/* Test various other return values */
void test_ts_l2_frame_check___TS_L2_REQ_CONT()
{
    test_data[0] = CHIP_STATUS_READY;
    test_data[1] = L2_STATUS_REQUEST_CONT;
    TEST_ASSERT_EQUAL(TS_L2_REQ_CONT, ts_l2_frame_check(test_data));
}

void test_ts_l2_frame_check___TS_L2_RES_CONT()
{
    test_data[0] = CHIP_STATUS_READY;
    test_data[1] = L2_STATUS_RESULT_CONT;
    TEST_ASSERT_EQUAL(TS_L2_RES_CONT, ts_l2_frame_check(test_data));
}

void test_ts_l2_frame_check___TS_L2_HSK_ERR()
{
    test_data[0] = CHIP_STATUS_READY;
    test_data[1] = L2_STATUS_HSK_ERR;
    TEST_ASSERT_EQUAL(TS_L2_HSK_ERR, ts_l2_frame_check(test_data));
}

void test_ts_l2_frame_check___TS_L2_NO_SESSION()
{
    test_data[0] = CHIP_STATUS_READY;
    test_data[1] = L2_STATUS_NO_SESSION;
    TEST_ASSERT_EQUAL(TS_L2_NO_SESSION, ts_l2_frame_check(test_data));
}

void test_ts_l2_frame_check___TS_L2_TAG_ERR()
{
    test_data[0] = CHIP_STATUS_READY;
    test_data[1] = L2_STATUS_TAG_ERR;
    TEST_ASSERT_EQUAL(TS_L2_TAG_ERR, ts_l2_frame_check(test_data));
}

void test_ts_l2_frame_check___TS_L2_CRC_ERR()
{
    test_data[0] = CHIP_STATUS_READY;
    test_data[1] = L2_STATUS_CRC_ERR;
    TEST_ASSERT_EQUAL(TS_L2_CRC_ERR, ts_l2_frame_check(test_data));
}

void test_ts_l2_frame_check___TS_L2_GEN_ERR()
{
    test_data[0] = CHIP_STATUS_READY;
    test_data[1] = L2_STATUS_GEN_ERR;
    TEST_ASSERT_EQUAL(TS_L2_GEN_ERR, ts_l2_frame_check(test_data));
}

void test_ts_l2_frame_check___TS_L2_NO_RESP()
{
    test_data[0] = CHIP_STATUS_READY;
    test_data[1] = L2_STATUS_NO_RESP;
    TEST_ASSERT_EQUAL(TS_L2_NO_RESP, ts_l2_frame_check(test_data));
}

void test_ts_l2_frame_check___TS_L2_UNKNOWN_REQ()
{
    test_data[0] = CHIP_STATUS_READY;
    test_data[1] = L2_STATUS_UNKNOWN_ERR;
    TEST_ASSERT_EQUAL(TS_L2_UNKNOWN_REQ, ts_l2_frame_check(test_data));
}

/* Test default behaviour when second byte is not recognized by parser */
void test_ts_l2_frame_check___TS_FAIL()
{
    test_data[0] = CHIP_STATUS_READY;
    test_data[1] = INVALID_BYTE;
    TEST_ASSERT_EQUAL(TS_FAIL, ts_l2_frame_check(test_data));
}
