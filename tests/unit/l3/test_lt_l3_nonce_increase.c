
/**
 * @file test_lt_l3_process.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic_common.h"
#include "lt_l3_process.h"
#include "mock_lt_aesgcm.h"
#include "mock_lt_l2.h"
#include "string.h"
#include "unity.h"

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- SETUP AND TEARDOWN ---------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

void setUp(void) {}

void tearDown(void) {}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- INPUT PARAMETERS   ---------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_PARAM_ERR on invalid handle
void test__handle_invalid()
{
    int ret = lt_l3_nonce_increase(NULL);
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, ret);
}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- EXECUTION ------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

// Test if function correctly increases nonce when executed correctly TODO
void test__nonce_is_increased()
{
    uint8_t nonce[12] = {0};
    int ret = lt_l3_nonce_increase(nonce);
    TEST_ASSERT_EQUAL(1, nonce[0]);
    TEST_ASSERT_EQUAL(LT_OK, ret);
}

// Test if function returns LT_OK when executed correctly
void test__correct()
{
    uint8_t nonce[12] = {0};
    int ret = lt_l3_nonce_increase(nonce);
    TEST_ASSERT_EQUAL(LT_OK, ret);
}
