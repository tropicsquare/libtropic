
#include "unity.h"
#include "string.h"

#include "libtropic_common.h"
#include "libtropic.h"
#include "lt_l2_api_structs.h"

#include "mock_lt_random.h"
#include "mock_lt_l1.h"
#include "mock_lt_l2.h"
#include "mock_lt_l3.h"
#include "mock_lt_x25519.h"
#include "mock_lt_ed25519.h"
#include "mock_lt_hkdf.h"
#include "mock_lt_sha256.h"
#include "mock_lt_aesgcm.h"


void setUp(void)
{
    
}

void tearDown(void)
{
}

// Test if function returns LT_FAIL when l1 init failed
void test_lt_init___error_during_lt_l1_init()
{
    lt_handle_t h = {0};

    lt_l1_init_ExpectAndReturn(&h, LT_FAIL);

    lt_ret_t ret = lt_init(&h);
    TEST_ASSERT_EQUAL(LT_FAIL, ret);
}

// Test if function returns LT_OK when all went correctly
void test_lt_init___correct()
{
    lt_handle_t h = {0};

    lt_l1_init_ExpectAndReturn(&h, LT_OK);

    lt_ret_t ret = lt_init(&h);
    TEST_ASSERT_EQUAL(LT_OK, ret);
}

//---------------------------------------------------------------------

// Test if function returns LT_FAIL when l1 deinit failed
void test_lt_deinit___error_during_lt_l1_deinit()
{
    lt_handle_t h = {0};

    lt_l1_deinit_ExpectAndReturn(&h, LT_FAIL);

    lt_ret_t ret = lt_deinit(&h);
    TEST_ASSERT_EQUAL(LT_FAIL, ret);
}

// Test if function returns LT_OK when all went correctly
void test_lt_deinit___correct()
{
    lt_handle_t h = {0};

    lt_l1_deinit_ExpectAndReturn(&h, LT_OK);

    lt_ret_t ret = lt_deinit(&h);
    TEST_ASSERT_EQUAL(LT_OK, ret);
}

//---------------------------------------------------------------------

