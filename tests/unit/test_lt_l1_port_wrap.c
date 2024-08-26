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
// Check that function lt_l1_init() returns LT_FAIL when lt_port_init() function returns LT_FAIL
void test_lt_l1_init___fail_during_lt_port_init__LT_FAIL()
{
    lt_handle_t h = {0};

    lt_port_init_ExpectAndReturn(&h, LT_FAIL);

    lt_ret_t ret = lt_l1_init(&h);
    TEST_ASSERT_EQUAL(LT_FAIL, ret);
}

// Check that function lt_l1_init() returns LT_L1_SPI_ERROR when lt_port_init() function returns LT_L1_SPI_ERROR
void test_lt_l1_init___fail_during_lt_port_init__LT_L1_SPI_ERROR()
{
    lt_handle_t h = {0};

    lt_port_init_ExpectAndReturn(&h, LT_L1_SPI_ERROR);

    lt_ret_t ret = lt_l1_init(&h);
    TEST_ASSERT_EQUAL(LT_L1_SPI_ERROR, ret);
}

// Check that function lt_l1_init() returns LT_OK when lt_port_init() function returns LT_OK
void test_lt_l1_init___fail_during_lt_port_init__LT_OK()
{
    lt_handle_t h = {0};

    lt_port_init_ExpectAndReturn(&h, LT_OK);

    lt_ret_t ret = lt_l1_init(&h);
    TEST_ASSERT_EQUAL(LT_OK, ret);
}


