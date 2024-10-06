
/**
 * @file test_lt_l2_input_params.c
 * @author Tropic Square s.r.o.
 * 
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "unity.h"

#include "libtropic_common.h"
#include "lt_l2_frame_check.h"
#include "lt_l2.h"

#include "mock_lt_l1.h"
#include "mock_lt_crc16.h"

void setUp(void)
{
}

void tearDown(void)
{
}

//----------------------------------------------------------------------
// Test if function returns expected error on non valid input parameter
void test_lt_l2_transfer___NULL_h()
{
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_l2_transfer(NULL));
}

//----------------------------------------------------------------------
// Test if function returns expected error on non valid input parameter
void test_lt_l2_encrypted_cmd___NULL_h()
{
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_l2_encrypted_cmd(NULL));
}
