/**
 * @file test_lt_l2_frame_check.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic_common.h"
#include "lt_l2_frame_check.h"
#include "mock_lt_crc16.h"
#include "mock_lt_l1.h"
#include "unity.h"

// Unknown byte, rubbish
#define INVALID_BYTE 0xfe

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- SETUP AND TEARDOWN ---------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

void setUp(void) {}

void tearDown(void) {}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- INPUT PARAMETERS   ---------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

// Test if function returns expected error on non valid input parameter
void test_lt_l2_frame_check___NULL_frame() { TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_l2_frame_check(NULL)); }

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- EXECUTION ------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

static uint8_t test_data[] = {
    0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x2e, 0x4e};

// Check function's return value on correct frame
void test_lt_l2_frame_check___CHIP_STATUS_READY__LT_OK()
{
    test_data[0] = TR01_L1_CHIP_MODE_READY_bit;
    test_data[1] = TR01_L2_STATUS_RESULT_OK;
    crc16_IgnoreAndReturn(0x2e4e);
    TEST_ASSERT_EQUAL(LT_OK, lt_l2_frame_check(test_data));
}

/* Check what function returns when frame looks valid (from first two bytes), but frame's CRC check fails.
This may happen when payload is altered during transmission */
void test_lt_l2_frame_check___CHIP_STATUS_READY__LT_CRC_ERR()
{
    test_data[0] = TR01_L1_CHIP_MODE_READY_bit;
    test_data[1] = TR01_L2_STATUS_REQUEST_OK;
    crc16_IgnoreAndReturn(0xdead);
    TEST_ASSERT_EQUAL(LT_L2_IN_CRC_ERR, lt_l2_frame_check(test_data));
}

// Test various other return values
void test_lt_l2_frame_check___LT_L2_REQ_CONT()
{
    test_data[0] = TR01_L1_CHIP_MODE_READY_bit;
    test_data[1] = TR01_L2_STATUS_REQUEST_CONT;
    TEST_ASSERT_EQUAL(LT_L2_REQ_CONT, lt_l2_frame_check(test_data));
}

void test_lt_l2_frame_check___LT_L2_RES_CONT()
{
    test_data[0] = TR01_L1_CHIP_MODE_READY_bit;
    test_data[1] = TR01_L2_STATUS_RESULT_CONT;
    TEST_ASSERT_EQUAL(LT_L2_RES_CONT, lt_l2_frame_check(test_data));
}

void test_lt_l2_frame_check___LT_L2_HSK_ERR()
{
    test_data[0] = TR01_L1_CHIP_MODE_READY_bit;
    test_data[1] = TR01_L2_STATUS_HSK_ERR;
    TEST_ASSERT_EQUAL(LT_L2_HSK_ERR, lt_l2_frame_check(test_data));
}

void test_lt_l2_frame_check___LT_L2_NO_SESSION()
{
    test_data[0] = TR01_L1_CHIP_MODE_READY_bit;
    test_data[1] = TR01_L2_STATUS_NO_SESSION;
    TEST_ASSERT_EQUAL(LT_L2_NO_SESSION, lt_l2_frame_check(test_data));
}

void test_lt_l2_frame_check___LT_L2_TAG_ERR()
{
    test_data[0] = TR01_L1_CHIP_MODE_READY_bit;
    test_data[1] = TR01_L2_STATUS_TAG_ERR;
    TEST_ASSERT_EQUAL(LT_L2_TAG_ERR, lt_l2_frame_check(test_data));
}

void test_lt_l2_frame_check___LT_L2_CRC_ERR()
{
    test_data[0] = TR01_L1_CHIP_MODE_READY_bit;
    test_data[1] = TR01_L2_STATUS_CRC_ERR;
    TEST_ASSERT_EQUAL(LT_L2_CRC_ERR, lt_l2_frame_check(test_data));
}

void test_lt_l2_frame_check___LT_L2_GEN_ERR()
{
    test_data[0] = TR01_L1_CHIP_MODE_READY_bit;
    test_data[1] = TR01_L2_STATUS_GEN_ERR;
    TEST_ASSERT_EQUAL(LT_L2_GEN_ERR, lt_l2_frame_check(test_data));
}

void test_lt_l2_frame_check___LT_L2_NO_RESP()
{
    test_data[0] = TR01_L1_CHIP_MODE_READY_bit;
    test_data[1] = TR01_L2_STATUS_NO_RESP;
    TEST_ASSERT_EQUAL(LT_L2_NO_RESP, lt_l2_frame_check(test_data));
}

void test_lt_l2_frame_check___LT_L2_UNKNOWN_REQ()
{
    test_data[0] = TR01_L1_CHIP_MODE_READY_bit;
    test_data[1] = TR01_L2_STATUS_UNKNOWN_ERR;
    TEST_ASSERT_EQUAL(LT_L2_UNKNOWN_REQ, lt_l2_frame_check(test_data));
}

// Test default behaviour when second byte is not recognized by parser
void test_lt_l2_frame_check___LT_FAIL()
{
    test_data[0] = TR01_L1_CHIP_MODE_READY_bit;
    test_data[1] = INVALID_BYTE;
    TEST_ASSERT_EQUAL(LT_L2_STATUS_NOT_RECOGNIZED, lt_l2_frame_check(test_data));
}
