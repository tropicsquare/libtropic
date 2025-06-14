/**
 * @file test_lt_l2.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic_common.h"
#include "lt_l2.h"
#include "lt_l2_api_structs.h"
#include "mock_lt_crc16.h"
#include "mock_lt_l1.h"
#include "mock_lt_l2_frame_check.h"
#include "unity.h"

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- SETUP AND TEARDOWN ---------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

void setUp(void) {}

void tearDown(void) {}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- INPUT PARAMETERS   ---------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//
/*
// Test if function returns expected error on non valid input parameter
void test_lt_l2_transfer___NULL_h()
{
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_l2_transfer(NULL));
}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- EXECUTION ------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

void test_lt_l2_transfer__fail_during_l1_write()
{
    lt_handle_t h = {0};

    add_crc_Expect(h.l2_buff);
    lt_l1_write_ExpectAndReturn(&h, 4, LT_L1_TIMEOUT_MS_DEFAULT, LT_L1_SPI_ERROR);
    TEST_ASSERT_EQUAL(LT_L1_SPI_ERROR, lt_l2_transfer(&h));
}

void test_lt_l2_transfer__fail_during_l1_read()
{
    lt_handle_t h = {0};

    add_crc_Expect(h.l2_buff);
    lt_l1_write_ExpectAndReturn(&h, 4, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l1_read_ExpectAndReturn(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT, LT_L1_SPI_ERROR);
    TEST_ASSERT_EQUAL(LT_L1_SPI_ERROR, lt_l2_transfer(&h));
}

void test_lt_l2_transfer__fail_during_frame_check()
{
    lt_handle_t h = {0};

    add_crc_Expect(h.l2_buff);
    lt_l1_write_ExpectAndReturn(&h, 4, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l1_read_ExpectAndReturn(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l2_frame_check_ExpectAndReturn(h.l2_buff, LT_L2_IN_CRC_ERR);

    TEST_ASSERT_EQUAL(LT_L2_IN_CRC_ERR, lt_l2_transfer(&h));
}

void test_lt_l2_transfer__correct()
{
    lt_handle_t h = {0};

    add_crc_Expect(h.l2_buff);
    lt_l1_write_ExpectAndReturn(&h, 4, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l1_read_ExpectAndReturn(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
    lt_l2_frame_check_ExpectAndReturn(h.l2_buff, LT_OK);

    TEST_ASSERT_EQUAL(LT_OK, lt_l2_transfer(&h));
}
*/