/**
 * @file test_libtropic__lt_get_info.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic.h"
#include "libtropic_common.h"
#include "lt_l2_api_structs.h"
#include "mock_lt_aesgcm.h"
#include "mock_lt_asn1_der.h"
#include "mock_lt_ed25519.h"
#include "mock_lt_hkdf.h"
#include "mock_lt_l1.h"
#include "mock_lt_l1_port_wrap.h"
#include "mock_lt_l2.h"
#include "mock_lt_l3.h"
#include "mock_lt_l3_process.h"
#include "mock_lt_random.h"
#include "mock_lt_sha256.h"
#include "mock_lt_x25519.h"
#include "string.h"
#include "time.h"
#include "unity.h"

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- SETUP AND TEARDOWN ---------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

void setUp(void)
{
    char buffer[100] = {0};
#ifdef RNG_SEED
    srand(RNG_SEED);
#else
    time_t seed = time(NULL);
    // Using this approach, because in our version of Unity there's no TEST_PRINTF yet.
    // Also, raw printf is worse solution (without additional debug msgs, such as line).
    snprintf(buffer, sizeof(buffer), "Using random seed: %ld\n", seed);
    TEST_MESSAGE(buffer);
    srand((unsigned int)seed);
#endif
}

void tearDown(void) {}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- INPUT PARAMETERS   ---------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_PARAM_ERROR when invalid handle is passed
void test__invalid_handle()
{
    struct lt_cert_store_t store;
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_get_info_cert_store(NULL, &store));

    struct lt_handle_t h;
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_get_info_cert_store(&h, NULL));
}


//---------------------------------------------------------------------------------------------------------//
//---------------------------------- EXECUTION ------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_PARAM_ERROR when invalid handle is passed
void test__l2_error_handling()
{
    struct lt_cert_store_t store = {0};
    struct lt_handle_t h = {0};

    lt_l2_send_IgnoreAndReturn(LT_PARAM_ERR);
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_get_info_cert_store(&h, &store));

    lt_l2_send_IgnoreAndReturn(LT_OK);
    lt_l2_receive_IgnoreAndReturn(LT_PARAM_ERR);
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_get_info_cert_store(&h, &store));

    lt_l2_send_IgnoreAndReturn(LT_OK);
    lt_l2_receive_IgnoreAndReturn(LT_OK);
    struct lt_l2_get_info_rsp_t* p_l2_resp = (struct lt_l2_get_info_rsp_t*)h.l2.buff;
    p_l2_resp->rsp_len = (uint8_t)260;
    TEST_ASSERT_EQUAL(LT_FAIL, lt_get_info_cert_store(&h, &store));

}