
/**
 * @file test_lt_l3.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "unity.h"
#include "string.h"

#include "libtropic_common.h"
#include "lt_l3.h"

#include "mock_lt_l2.h"
#include "mock_lt_aesgcm.h"

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- SETUP AND TEARDOWN ---------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

void setUp(void)
{
}

void tearDown(void)
{
}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- INPUT PARAMETERS   ---------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_PARAM_ERR on non valid input parameter
void test__invalid_handle()
{
    lt_ret_t ret = lt_l3_nonce_init(NULL);
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, ret);
}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- EXECUTION ------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

// Test correct nonce initialization
void test_lt_l3_nonce_init___correct()
{
    lt_handle_t handle = {0};
    memset(handle.encryption_IV, 0xff, 12);
    memset(handle.decryption_IV, 0xff, 12);

    uint8_t expected_1[12] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_1, handle.encryption_IV, 12);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_1, handle.decryption_IV, 12);

    int ret = lt_l3_nonce_init(&handle);
    TEST_ASSERT_EQUAL(LT_OK, ret);

    uint8_t expected_2[12] = {0};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_2, handle.encryption_IV, 12);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_2, handle.decryption_IV, 12);
}
