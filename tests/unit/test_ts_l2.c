
/**
 * @file test_ts_l2.c
 * @author Tropic Square s.r.o.
 * 
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "unity.h"

#include "libtropic_common.h"
#include "lt_l2.h"
#include "lt_l2_frame_check.h"

#include "mock_lt_l1.h"
#include "mock_lt_crc16.h"


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


// Check function's return value on correct frame
void test_lt_l2_frame_check___CHIP_STATUS_READY__LT_OK()
{
    test_data[0] = CHIP_MODE_READY_bit;
    test_data[1] = L2_STATUS_RESULT_OK;
    crc16_IgnoreAndReturn(0x2e4e);
    TEST_ASSERT_EQUAL(LT_OK, lt_l2_frame_check(test_data));
}

/* Check what function returns when frame looks valid (from first two bytes), but frame's CRC check fails.
This may happen when payload is altered during transmission */
void test_lt_l2_frame_check___CHIP_STATUS_READY__LT_CRC_ERR()
{
    test_data[0] = CHIP_MODE_READY_bit;
    test_data[1] = L2_STATUS_REQUEST_OK;
    crc16_IgnoreAndReturn(0xdead);
    TEST_ASSERT_EQUAL(LT_L2_IN_CRC_ERR, lt_l2_frame_check(test_data));
}

// Test various other return values
void test_lt_l2_frame_check___LT_L2_REQ_CONT()
{
    test_data[0] = CHIP_MODE_READY_bit;
    test_data[1] = L2_STATUS_REQUEST_CONT;
    TEST_ASSERT_EQUAL(LT_L2_REQ_CONT, lt_l2_frame_check(test_data));
}

void test_lt_l2_frame_check___LT_L2_RES_CONT()
{
    test_data[0] = CHIP_MODE_READY_bit;
    test_data[1] = L2_STATUS_RESULT_CONT;
    TEST_ASSERT_EQUAL(LT_L2_RES_CONT, lt_l2_frame_check(test_data));
}

void test_lt_l2_frame_check___LT_L2_HSK_ERR()
{
    test_data[0] = CHIP_MODE_READY_bit;
    test_data[1] = L2_STATUS_HSK_ERR;
    TEST_ASSERT_EQUAL(LT_L2_HSK_ERR, lt_l2_frame_check(test_data));
}

void test_lt_l2_frame_check___LT_L2_NO_SESSION()
{
    test_data[0] = CHIP_MODE_READY_bit;
    test_data[1] = L2_STATUS_NO_SESSION;
    TEST_ASSERT_EQUAL(LT_L2_NO_SESSION, lt_l2_frame_check(test_data));
}

void test_lt_l2_frame_check___LT_L2_TAG_ERR()
{
    test_data[0] = CHIP_MODE_READY_bit;
    test_data[1] = L2_STATUS_TAG_ERR;
    TEST_ASSERT_EQUAL(LT_L2_TAG_ERR, lt_l2_frame_check(test_data));
}

void test_lt_l2_frame_check___LT_L2_CRC_ERR()
{
    test_data[0] = CHIP_MODE_READY_bit;
    test_data[1] = L2_STATUS_CRC_ERR;
    TEST_ASSERT_EQUAL(LT_L2_CRC_ERR, lt_l2_frame_check(test_data));
}

void test_lt_l2_frame_check___LT_L2_GEN_ERR()
{
    test_data[0] = CHIP_MODE_READY_bit;
    test_data[1] = L2_STATUS_GEN_ERR;
    TEST_ASSERT_EQUAL(LT_L2_GEN_ERR, lt_l2_frame_check(test_data));
}

void test_lt_l2_frame_check___LT_L2_NO_RESP()
{
    test_data[0] = CHIP_MODE_READY_bit;
    test_data[1] = L2_STATUS_NO_RESP;
    TEST_ASSERT_EQUAL(LT_L2_NO_RESP, lt_l2_frame_check(test_data));
}

void test_lt_l2_frame_check___LT_L2_UNKNOWN_REQ()
{
    test_data[0] = CHIP_MODE_READY_bit;
    test_data[1] = L2_STATUS_UNKNOWN_ERR;
    TEST_ASSERT_EQUAL(LT_L2_UNKNOWN_REQ, lt_l2_frame_check(test_data));
}

// Test default behaviour when second byte is not recognized by parser
void test_lt_l2_frame_check___LT_FAIL()
{
    test_data[0] = CHIP_MODE_READY_bit;
    test_data[1] = INVALID_BYTE;
    TEST_ASSERT_EQUAL(LT_L2_STATUS_NOT_RECOGNIZED, lt_l2_frame_check(test_data));
}

void test_lt_l2_transfer__fail_during_l1_write()
{
    lt_handle_t h = {0};
    
    add_crc_Ignore();
    lt_l1_write_ExpectAndReturn(&h, 4, LT_L1_TIMEOUT_MS_DEFAULT, LT_L1_SPI_ERROR);
    TEST_ASSERT_EQUAL(LT_L1_SPI_ERROR, lt_l2_transfer(&h));
}

void test_lt_l2_transfer__fail_during_l1_read()
{
    lt_handle_t h = {0};

    add_crc_Ignore();
    lt_l1_write_ExpectAndReturn(&h, 4, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l1_read_ExpectAndReturn(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT, LT_L1_SPI_ERROR);
    TEST_ASSERT_EQUAL(LT_L1_SPI_ERROR, lt_l2_transfer(&h));
}

// TODO: move lt_l2_frame_check to another compilation unit?
// void test_lt_l2_transfer__fail_during_frame_check()
// {
//     lt_handle_t h = {0};
//     // This is to force lt_l2_frame_check to return LT_OK.
//     // Reason: functions from the same compilation unit are unmockable.
//     // TODO this won't work
//     h.l2_buff[1] = L2_STATUS_REQUEST_OK;

//     add_crc_Ignore();
//     lt_l1_write_ExpectAndReturn(&h, L2_STATUS_REQUEST_OK + 4, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
//     lt_l1_read_ExpectAndReturn(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);

//     TEST_ASSERT_EQUAL(LT_OK, lt_l2_transfer(&h));
// }

//--------------------------------------------------------------------------------------------------

void test_lt_l2_encrypted_cmd__l1_write_fail()
{
    lt_handle_t h = {0};
    int chunk_len = L3_RES_SIZE_SIZE + L3_TAG_SIZE;

    add_crc_Ignore();
    lt_l1_write_ExpectAndReturn(&h, chunk_len + 4, LT_L1_TIMEOUT_MS_DEFAULT, LT_L1_SPI_ERROR);

    TEST_ASSERT_EQUAL(LT_L1_SPI_ERROR, lt_l2_encrypted_cmd(&h));
}

void test_lt_l2_encrypted_cmd__l1_read_fail()
{
    lt_handle_t h = {0};
    int chunk_len = L3_RES_SIZE_SIZE + L3_TAG_SIZE;

    add_crc_Ignore();
    lt_l1_write_ExpectAndReturn(&h, chunk_len + 4, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l1_read_ExpectAndReturn(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT, LT_L1_SPI_ERROR);

    TEST_ASSERT_EQUAL(LT_L1_SPI_ERROR, lt_l2_encrypted_cmd(&h));
}

// TODO: frame check
