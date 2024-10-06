/**
 * @file test_lt_l2_frame_check_input_params.c
 * @author Tropic Square s.r.o.
 * 
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "unity.h"

#include "libtropic_common.h"
#include "lt_crc16.h"
#include "lt_l2_frame_check.h"

void setUp(void)
{
}

void tearDown(void)
{
}

//----------------------------------------------------------------------
// Test if function returns expected error on non valid input parameter
void test_lt_l2_frame_check___NULL_frame()
{
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_l2_frame_check(NULL));
}