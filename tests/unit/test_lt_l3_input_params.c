
/**
 * @file test_ts_l3_input_params.c
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

void setUp(void)
{
}

void tearDown(void)
{
}

// Test if function returns LT_PARAM_ERR on non valid input parameter
void test_lt_l3_nonce_init___NULL_h()
{
    lt_ret_t ret = lt_l3_nonce_init(NULL);
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, ret);
}

// Test if function returns LT_PARAM_ERR on non valid input parameter
void test_lt_l3_nonce_increase___NULL_h()
{
    lt_ret_t ret = lt_l3_nonce_increase(NULL);
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, ret);
}

// Test if function returns LT_PARAM_ERR on non valid input parameter
void test_lt_l3_cmd___NULL_h()
{
    int ret = lt_l3_cmd(NULL);
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, ret);
}
