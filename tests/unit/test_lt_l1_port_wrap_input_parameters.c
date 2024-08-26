#include "unity.h"

#include "libtropic_common.h"
#include "lt_l1_port_wrap.h"

#include "mock_libtropic_port.h"

void setUp(void)
{
}

void tearDown(void)
{
}
//---------------------------------------------------------------------------------------------------------------------
// Test if function returns LT_PARAM_ERR on non valid input parameter
void test_lt_l1_init___NULL_h()
{
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_l1_init(NULL));
}

//---------------------------------------------------------------------------------------------------------------------
// Test if function returns LT_PARAM_ERR on non valid input parameter
void test_lt_l1_deinit___NULL_h()
{
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_l1_deinit(NULL));
}
