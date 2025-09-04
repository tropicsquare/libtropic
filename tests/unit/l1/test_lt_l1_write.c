/**
 * @file test_lt_l1.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic_common.h"
#include "lt_l1.h"
#include "mock_lt_l1_port_wrap.h"
#include "unity.h"

#define SOME_UNUSED_DEFAULT_BYTE 0xfe
#define L2_BUFF_WHEN_NOT_LT_L1_DATA_LEN_ERROR 0

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- SETUP AND TEARDOWN ---------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

void setUp(void) {}

void tearDown(void) {}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- INPUT PARAMETERS   ---------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_PARAM_ERR on non valid handle
void test__invalid_handle()
{
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_l1_write(NULL, TR01_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT));
}

// Test if function returns LT_PARAM_ERR on non valid len
// void test__invalid_len()
//{
//    lt_handle_t h = {0};
//    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_l1_write(&h, TR01_L1_LEN_MIN-1, LT_L1_TIMEOUT_MS_DEFAULT));
//    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_l1_write(&h, TR01_L1_LEN_MAX+1, LT_L1_TIMEOUT_MS_DEFAULT));
//}
//
//// Test if function returns LT_PARAM_ERR on non valid timeout
// void test__invalid_timeout()
//{
//     lt_handle_t h = {0};
//     TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_l1_write(&h, TR01_L1_LEN_MAX, LT_L1_TIMEOUT_MS_MIN-1));
//     TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_l1_write(&h, TR01_L1_LEN_MAX, LT_L1_TIMEOUT_MS_MAX+1));
// }
//
////---------------------------------------------------------------------------------------------------------//
////---------------------------------- EXECUTION ------------------------------------------------------------//
////---------------------------------------------------------------------------------------------------------//
//
//// Test LT_L1_SPI_ERROR return value during write
// void test__LT_L1_SPI_ERROR()
//{
//     lt_handle_t h = {0};
//
//     lt_l1_spi_csn_low_ExpectAndReturn(&h, LT_OK);
//     lt_l1_spi_transfer_ExpectAndReturn(&h, 0, 1, LT_L1_TIMEOUT_MS_DEFAULT, LT_FAIL);
//     lt_l1_spi_csn_high_ExpectAndReturn(&h, LT_OK);
//     TEST_ASSERT_EQUAL(LT_L1_SPI_ERROR, lt_l1_write(&h, 1, LT_L1_TIMEOUT_MS_DEFAULT));
// }
//
//// Test LT_OK return value
// void test__correct()
//{
//     lt_handle_t h = {0};
//
//     lt_l1_spi_csn_low_ExpectAndReturn(&h, LT_OK);
//     lt_l1_spi_transfer_ExpectAndReturn(&h, 0, 1, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);
//     lt_l1_spi_csn_high_ExpectAndReturn(&h, LT_OK);
//
//     TEST_ASSERT_EQUAL(LT_OK, lt_l1_write(&h, 1, LT_L1_TIMEOUT_MS_DEFAULT));
// }
//