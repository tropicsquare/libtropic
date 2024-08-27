#include "unity.h"

#include "libtropic_common.h"
#include "lt_l1_port_wrap.h"
#include "lt_l1.h"

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

//---------------------------------------------------------------------------------------------------------------------
// Test if function returns LT_PARAM_ERR on non valid input parameter
void test_lt_l1_spi_csn_low___NULL_h()
{
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_l1_spi_csn_low(NULL));
}

//---------------------------------------------------------------------------------------------------------------------
// Test if function returns LT_PARAM_ERR on non valid input parameter
void test_lt_l1_spi_csn_high___NULL_h()
{
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_l1_spi_csn_high(NULL));
}

//---------------------------------------------------------------------------------------------------------------------
// Test if function returns LT_PARAM_ERR on non valid input parameter
void test_lt_l1_spi_transfer___NULL_h()
{
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_l1_spi_transfer(NULL, 1, 1, LT_L1_TIMEOUT_MS_DEFAULT));
}

//---------------------------------------------------------------------------------------------------------------------
// Test if function returns LT_PARAM_ERR on non valid input parameter
void test_lt_l1_delay___NULL_h()
{
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_l1_delay(NULL, LT_L1_TIMEOUT_MS_DEFAULT));
}