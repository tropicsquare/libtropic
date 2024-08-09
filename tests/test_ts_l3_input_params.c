
#include "unity.h"
#include "string.h"

#include "libtropic_common.h"
#include "ts_l3.h"

#include "mock_ts_l2.h"
#include "mock_ts_aesgcm.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_ts_l3_nonce_init___NULL_h()
{
    ts_ret_t ret = ts_l3_nonce_init(NULL);
    TEST_ASSERT_EQUAL(TS_PARAM_ERR, ret);
}

void test_ts_l3_nonce_increase___NULL_h()
{
    ts_ret_t ret = ts_l3_nonce_increase(NULL);
    TEST_ASSERT_EQUAL(TS_PARAM_ERR, ret);
}

void test_ts_l3_cmd___NULL_h()
{
    int ret = ts_l3_cmd(NULL);
    TEST_ASSERT_EQUAL(TS_PARAM_ERR, ret);
}
