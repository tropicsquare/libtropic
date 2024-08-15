
#include "unity.h"

#include "libtropic_common.h"
#include "ts_l2.h"

#include "mock_ts_l1.h"
#include "mock_ts_crc16.h"

void setUp(void)
{
}

void tearDown(void)
{
}

//----------------------------------------------------------------------
// Test if function returns expected error on non valid input parameter
void test_ts_l2_frame_check___NULL_frame()
{
    TEST_ASSERT_EQUAL(TS_PARAM_ERR, ts_l2_frame_check(NULL));
}

//----------------------------------------------------------------------
// Test if function returns expected error on non valid input parameter
void test_ts_l2_transfer___NULL_h()
{
    TEST_ASSERT_EQUAL(TS_PARAM_ERR, ts_l2_transfer(NULL));
}
