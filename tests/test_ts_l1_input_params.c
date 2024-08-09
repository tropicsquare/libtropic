
#include "unity.h"

#include "libtropic_common.h"

#include "ts_l1.h"
#include "mock_support_ts_l1.h"
#include "mock_ts_l2.h"


#define SOME_UNUSED_DEFAULT_BYTE 0xfe

void setUp(void)
{
}

void tearDown(void)
{
}


void test_ts_l1_read___NULL_h()
{

    TEST_ASSERT_EQUAL(TS_PARAM_ERR, ts_l1_read(NULL, TS_L1_LEN_MAX, TS_L1_TIMEOUT_MS_DEFAULT));
}

void test_ts_l1_read___invalid_max_len_smaller()
{
    ts_handle_t h = {0};

    TEST_ASSERT_EQUAL(TS_PARAM_ERR, ts_l1_read(&h, TS_L1_LEN_MIN-1, TS_L1_TIMEOUT_MS_DEFAULT));
}

void test_ts_l1_read___invalid_max_len_bigger()
{
    ts_handle_t h = {0};

    TEST_ASSERT_EQUAL(TS_PARAM_ERR, ts_l1_read(&h, TS_L1_LEN_MAX+1, TS_L1_TIMEOUT_MS_DEFAULT));
}

void test_ts_l1_read___invalid_timeout_smaller()
{
    ts_handle_t h = {0};

    TEST_ASSERT_EQUAL(TS_PARAM_ERR, ts_l1_read(&h, TS_L1_LEN_MAX, TS_L1_TIMEOUT_MS_MIN-1));
}

void test_ts_l1_read___invalid_timeout_bigger()
{
    ts_handle_t h = {0};

    TEST_ASSERT_EQUAL(TS_PARAM_ERR, ts_l1_read(&h, TS_L1_LEN_MAX, TS_L1_TIMEOUT_MS_MAX+1));
}

//---------------------------------------------------------------------------------------------------------------------
void test_ts_l1_write___NULL_h()
{
    TEST_ASSERT_EQUAL(TS_PARAM_ERR, ts_l1_write(NULL, TS_L1_LEN_MAX, TS_L1_TIMEOUT_MS_DEFAULT));
}

void test_ts_l1_write___invalid_len_smaller()
{
    ts_handle_t h = {0};

    TEST_ASSERT_EQUAL(TS_PARAM_ERR, ts_l1_write(&h, TS_L1_LEN_MIN-1, TS_L1_TIMEOUT_MS_DEFAULT));
}

void test_ts_l1_write___invalid_len_bigger()
{
    ts_handle_t h = {0};

    TEST_ASSERT_EQUAL(TS_PARAM_ERR, ts_l1_write(&h, TS_L1_LEN_MAX+1, TS_L1_TIMEOUT_MS_DEFAULT));
}

void test_ts_l1_write___invalid_timeout_smaller()
{
    ts_handle_t h = {0};

    TEST_ASSERT_EQUAL(TS_PARAM_ERR, ts_l1_write(&h, TS_L1_LEN_MAX, TS_L1_TIMEOUT_MS_MIN-1));
}

void test_ts_l1_write___invalid_timeout_bigger()
{
    ts_handle_t h = {0};

    TEST_ASSERT_EQUAL(TS_PARAM_ERR, ts_l1_write(&h, TS_L1_LEN_MAX, TS_L1_TIMEOUT_MS_MAX+1));
}
