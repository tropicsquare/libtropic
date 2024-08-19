
#include "unity.h"

#include "libtropic_common.h"

#include "lt_l1.h"
#include "mock_libtropic_port.h"
#include "mock_lt_l2.h"


#define SOME_UNUSED_DEFAULT_BYTE 0xfe

void setUp(void)
{
}

void tearDown(void)
{
}

//---------------------------------------------------------------------------------------------------------------------
// Test if function returns LT_PARAM_ERR on non valid input parameter
void test_lt_l1_read___NULL_h()
{

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_l1_read(NULL, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT));
}

// Test if function returns LT_PARAM_ERR on non valid input parameter
void test_lt_l1_read___invalid_max_len_smaller()
{
    lt_handle_t h = {0};

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_l1_read(&h, LT_L1_LEN_MIN-1, LT_L1_TIMEOUT_MS_DEFAULT));
}

// Test if function returns LT_PARAM_ERR on non valid input parameter
void test_lt_l1_read___invalid_max_len_bigger()
{
    lt_handle_t h = {0};

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_l1_read(&h, LT_L1_LEN_MAX+1, LT_L1_TIMEOUT_MS_DEFAULT));
}

// Test if function returns LT_PARAM_ERR on non valid input parameter
void test_lt_l1_read___invalid_timeout_smaller()
{
    lt_handle_t h = {0};

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_l1_read(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_MIN-1));
}

// Test if function returns LT_PARAM_ERR on non valid input parameter
void test_lt_l1_read___invalid_timeout_bigger()
{
    lt_handle_t h = {0};

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_l1_read(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_MAX+1));
}

//---------------------------------------------------------------------------------------------------------------------
// Test if function returns LT_PARAM_ERR on non valid input parameter
void test_lt_l1_write___NULL_h()
{
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_l1_write(NULL, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_DEFAULT));
}

// Test if function returns LT_PARAM_ERR on non valid input parameter
void test_lt_l1_write___invalid_len_smaller()
{
    lt_handle_t h = {0};

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_l1_write(&h, LT_L1_LEN_MIN-1, LT_L1_TIMEOUT_MS_DEFAULT));
}

// Test if function returns LT_PARAM_ERR on non valid input parameter
void test_lt_l1_write___invalid_len_bigger()
{
    lt_handle_t h = {0};

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_l1_write(&h, LT_L1_LEN_MAX+1, LT_L1_TIMEOUT_MS_DEFAULT));
}

// Test if function returns LT_PARAM_ERR on non valid input parameter
void test_lt_l1_write___invalid_timeout_smaller()
{
    lt_handle_t h = {0};

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_l1_write(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_MIN-1));
}

// Test if function returns LT_PARAM_ERR on non valid input parameter
void test_lt_l1_write___invalid_timeout_bigger()
{
    lt_handle_t h = {0};

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_l1_write(&h, LT_L1_LEN_MAX, LT_L1_TIMEOUT_MS_MAX+1));
}
