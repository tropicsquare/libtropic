/**
 * @file test_lt_l1_port_wrap.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "unity.h"

#include "libtropic_common.h"
#include "lt_l1_port_wrap.h"
#include "lt_l1.h"

#include "mock_libtropic_port.h"

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

// Test if function returns LT_PARAM_ERR on invalid handle
void test__invalid_handle()
{
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_l1_deinit(NULL));
}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- EXECUTION ------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

// Check that function lt_l1_init() returns LT_FAIL when lt_port_init() function returns LT_FAIL
void test__lt_port_init_LT_FAIL()
{
    lt_handle_t h = {0};

    lt_port_deinit_ExpectAndReturn(&h, LT_FAIL);
    lt_ret_t ret = lt_l1_deinit(&h);
    TEST_ASSERT_EQUAL(LT_FAIL, ret);
}

// Check that function lt_l1_init() returns LT_OK when lt_port_init() function returns LT_OK
void test__correct()
{
    lt_handle_t h = {0};

    lt_port_deinit_ExpectAndReturn(&h, LT_OK);
    lt_ret_t ret = lt_l1_deinit(&h);
    TEST_ASSERT_EQUAL(LT_OK, ret);
}
