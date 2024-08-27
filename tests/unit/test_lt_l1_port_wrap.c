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
// Check that function lt_l1_init() returns LT_FAIL when lt_port_init() function returns LT_FAIL
void test_lt_l1_init___lt_port_init__LT_FAIL()
{
    lt_handle_t h = {0};

    lt_port_init_ExpectAndReturn(&h, LT_FAIL);

    lt_ret_t ret = lt_l1_init(&h);
    TEST_ASSERT_EQUAL(LT_FAIL, ret);
}

// Check that function lt_l1_init() returns LT_OK when lt_port_init() function returns LT_OK
void test_lt_l1_init____lt_port_init__LT_OK()
{
    lt_handle_t h = {0};

    lt_port_init_ExpectAndReturn(&h, LT_OK);

    lt_ret_t ret = lt_l1_init(&h);
    TEST_ASSERT_EQUAL(LT_OK, ret);
}

// Check that function lt_l1_deinit() returns LT_FAIL when lt_port_deinit() function returns LT_FAIL
void test_lt_l1_deinit___lt_port_deinit__LT_FAIL()
{
    lt_handle_t h = {0};

    lt_port_deinit_ExpectAndReturn(&h, LT_FAIL);

    lt_ret_t ret = lt_l1_deinit(&h);
    TEST_ASSERT_EQUAL(LT_FAIL, ret);
}

void test_lt_l1_deinit___lt_port_deinit__LT_OK()
{
    lt_handle_t h = {0};

    lt_port_deinit_ExpectAndReturn(&h, LT_OK);

    lt_ret_t ret = lt_l1_deinit(&h);
    TEST_ASSERT_EQUAL(LT_OK, ret);
}

//---------------------------------------------------------------------------------------------------------------------
// Check that function lt_l1_spi_csn_low() returns LT_FAIL when lt_port_spi_csn_low() function returns LT_FAIL
void test_lt_l1_spi_csn_low___lt_port_spi_csn_low__LT_FAIL()
{
    lt_handle_t h = {0};

    lt_port_spi_csn_low_ExpectAndReturn(&h, LT_FAIL);

    lt_ret_t ret = lt_l1_spi_csn_low(&h);
    TEST_ASSERT_EQUAL(LT_FAIL, ret);
}

// Check that function lt_l1_spi_csn_low() returns LT_OK when lt_port_spi_csn_low() function returns LT_OK
void test_lt_l1_spi_csn_low___lt_port_spi_csn_low__LT_OK()
{
    lt_handle_t h = {0};

    lt_port_spi_csn_low_ExpectAndReturn(&h, LT_OK);

    lt_ret_t ret = lt_l1_spi_csn_low(&h);
    TEST_ASSERT_EQUAL(LT_OK, ret);
}

//---------------------------------------------------------------------------------------------------------------------
// Check that function lt_l1_spi_csn_high() returns LT_FAIL when lt_port_spi_csn_high() function returns LT_FAIL
void test_lt_l1_spi_csn_high___lt_port_spi_csn_high__LT_FAIL()
{
    lt_handle_t h = {0};

    lt_port_spi_csn_high_ExpectAndReturn(&h, LT_FAIL);

    lt_ret_t ret = lt_l1_spi_csn_high(&h);
    TEST_ASSERT_EQUAL(LT_FAIL, ret);
}

// Check that function lt_l1_spi_csn_high() returns LT_OK when lt_port_spi_csn_high() function returns LT_OK
void test_lt_l1_spi_csn_high___lt_port_spi_csn_high__LT_OK()
{
    lt_handle_t h = {0};

    lt_port_spi_csn_high_ExpectAndReturn(&h, LT_OK);

    lt_ret_t ret = lt_l1_spi_csn_high(&h);
    TEST_ASSERT_EQUAL(LT_OK, ret);
}





//---------------------------------------------------------------------------------------------------------------------
// Check that function lt_l1_spi_transfer() returns LT_L1_DATA_LEN_ERROR when lt_port_spi_transfer() function returns LT_L1_DATA_LEN_ERROR
void test_lt_l1_spi_transfer___lt_port_spi_transfer__LT_L1_DATA_LEN_ERROR()
{
    lt_handle_t h = {0};

    lt_port_spi_transfer_ExpectAndReturn(&h, 1, 1, LT_L1_TIMEOUT_MS_DEFAULT, LT_L1_DATA_LEN_ERROR);

    lt_ret_t ret = lt_l1_spi_transfer(&h, 1, 1, LT_L1_TIMEOUT_MS_DEFAULT);
    TEST_ASSERT_EQUAL(LT_L1_DATA_LEN_ERROR, ret);
}

// Check that function lt_l1_spi_transfer() returns LT_FAIL when lt_port_spi_transfer() function returns LT_FAIL
void test_lt_l1_spi_transfer___lt_port_spi_transfer__LT_FAIL()
{
    lt_handle_t h = {0};

    lt_port_spi_transfer_ExpectAndReturn(&h, 1, 1, LT_L1_TIMEOUT_MS_DEFAULT, LT_FAIL);

    lt_ret_t ret = lt_l1_spi_transfer(&h, 1, 1, LT_L1_TIMEOUT_MS_DEFAULT);
    TEST_ASSERT_EQUAL(LT_FAIL, ret);
}

// Check that function lt_l1_spi_transfer() returns LT_OK when lt_port_spi_transfer() function returns LT_OK
void test_lt_l1_spi_transfer___lt_port_spi_transfer__LT_OK()
{
    lt_handle_t h = {0};

    lt_port_spi_transfer_ExpectAndReturn(&h, 1, 1, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);

    lt_ret_t ret = lt_l1_spi_transfer(&h, 1, 1, LT_L1_TIMEOUT_MS_DEFAULT);
    TEST_ASSERT_EQUAL(LT_OK, ret);
}

//---------------------------------------------------------------------------------------------------------------------
// Check that function lt_l1_delay() returns LT_OK when lt_port_delay() function returns LT_OK
void test_lt_l1_delay___lt_port_delay__LT_OK()
{
    lt_handle_t h = {0};

    lt_port_delay_ExpectAndReturn(&h, LT_L1_TIMEOUT_MS_DEFAULT, LT_OK);

    lt_ret_t ret = lt_l1_delay(&h, LT_L1_TIMEOUT_MS_DEFAULT);
    TEST_ASSERT_EQUAL(LT_OK, ret);
}