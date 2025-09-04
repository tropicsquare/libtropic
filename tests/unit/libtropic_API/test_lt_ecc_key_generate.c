/**
 * @file test_libtropic__lt_ecc_key_generate.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic.h"
#include "libtropic_common.h"
#include "lt_l3_api_structs.h"
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

/*// Test if function returns LT_PARAM_ERR on invalid handle
void test_lt_ecc_key_generate__invalid_handle()
{
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_generate(NULL, TR01_ECC_SLOT_1, TR01_CURVE_ED25519));
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_generate(NULL, TR01_ECC_SLOT_1, TR01_CURVE_P256));
}

// Test if function returns LT_PARAM_ERR on invalid slot
void test_lt_ecc_key_generate__invalid_slot()
{
    lt_handle_t h = {0};
    h.session = SESSION_ON;

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_generate(&h, TR01_ECC_SLOT_0 - 1, TR01_CURVE_ED25519));
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_generate(&h, TR01_ECC_SLOT_31 + 1, TR01_CURVE_ED25519));
}

// Test if function returns LT_PARAM_ERR on invalid curve
void test_lt_ecc_key_generate__invalid_curve()
{
    lt_handle_t h = {0};
    h.session = SESSION_ON;
    // Test random values.
    int curve;
    for (int i = 0; i < 25; i++) {
        curve = rand();
        if (curve != TR01_CURVE_ED25519 && curve != TR01_CURVE_P256) {
            TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_generate(&h, TR01_ECC_SLOT_1, curve));
        }
    }
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_generate(&h, TR01_ECC_SLOT_1, TR01_CURVE_P256-1));
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_generate(&h, TR01_ECC_SLOT_1, TR01_CURVE_ED25519+1));
}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- EXECUTION ------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_HOST_NO_SESSION when handle's variable 'session' is not set to 'SESSION_ON'
void test_lt_ecc_key_generate__no_session()
{
    lt_handle_t h = {0};
    h.session     = 0;

    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, lt_ecc_key_generate(&h, TR01_ECC_SLOT_1, TR01_CURVE_ED25519));
    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, lt_ecc_key_generate(&h, TR01_ECC_SLOT_1, TR01_CURVE_P256));
}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_FAIL when lt_out__ecc_key_generate() returns LT_FAIL
void test_lt_ecc_key_generate__out_fnc_fail()
{
    lt_handle_t h =  {0};
    h.session     = SESSION_ON;

    lt_out__ecc_key_generate_ExpectAndReturn(&h, TR01_ECC_SLOT_1, TR01_CURVE_ED25519, LT_FAIL);
    TEST_ASSERT_EQUAL(LT_FAIL, lt_ecc_key_generate(&h, TR01_ECC_SLOT_1, TR01_CURVE_ED25519));
}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_FAIL when lt_l2_encrypted_cmd() returns LT_FAIL
void test_lt_ecc_key_generate__l3_fail()
{
    lt_handle_t h =  {0};
    h.session     = SESSION_ON;

    lt_out__ecc_key_generate_ExpectAndReturn(&h, TR01_ECC_SLOT_1, TR01_CURVE_ED25519, LT_OK);
    lt_l2_send_encrypted_cmd_ExpectAndReturn(&h, LT_FAIL);
    TEST_ASSERT_EQUAL(LT_FAIL, lt_ecc_key_generate(&h, TR01_ECC_SLOT_1, TR01_CURVE_ED25519));
}


// Test if function returns LT_FAIL when lt_in__ecc_key_generate() returns LT_FAIL
//void test_lt_ecc_key_generate__in_fnc_fail()
//{
//    lt_handle_t h =  {0};
//    h.session     = SESSION_ON;
//
//    lt_out__ecc_key_generate_ExpectAndReturn(&h, TR01_ECC_SLOT_1, TR01_CURVE_ED25519, LT_OK);
//    lt_l2_send_encrypted_cmd_ExpectAndReturn(&h, LT_OK);
//    // add l2 recv here
//    lt_in__ecc_key_generate_ExpectAndReturn(&h, LT_FAIL);
//    TEST_ASSERT_EQUAL(LT_FAIL, lt_ecc_key_generate(&h, TR01_ECC_SLOT_1, TR01_CURVE_ED25519));
//}
//

//---------------------------------------------------------------------------------------------------------//

//uint16_t size_inject_value;
//lt_ret_t callback_lt_ecc_key_generate_lt_l3_cmd(lt_handle_t *h, int __attribute__((unused)) cmock_num_calls)
//{
//    struct lt_l3_ecc_key_generate_res_t* p_l3_res = (struct lt_l3_ecc_key_generate_res_t*)&h->l3_buff;
//    p_l3_res->res_size = size_inject_value;
//
//    return LT_OK;
//}
//
//// Test if function returns LT_FAIL if res_size field in result structure contains unexpected size
//void test_lt_ecc_key_generate__res_size_mismatch()
//{
//    lt_handle_t h =  {0};
//    h.session     = SESSION_ON;
//
//    size_inject_value = 0;
//    lt_l3_cmd_Stub(callback_lt_ecc_key_generate_lt_l3_cmd);
//    TEST_ASSERT_EQUAL(LT_FAIL, lt_ecc_key_generate(&h, TR01_ECC_SLOT_1, TR01_CURVE_ED25519));
//
//    size_inject_value = 2;
//    lt_l3_cmd_Stub(callback_lt_ecc_key_generate_lt_l3_cmd);
//    TEST_ASSERT_EQUAL(LT_FAIL, lt_ecc_key_generate(&h, TR01_ECC_SLOT_1, TR01_CURVE_ED25519));
//
//    size_inject_value = (uint16_t)((rand() % (TR01_L3_CYPHERTEXT_MAX_SIZE - 2)) + 2);
//    lt_l3_cmd_Stub(callback_lt_ecc_key_generate_lt_l3_cmd);
//    TEST_ASSERT_EQUAL(LT_FAIL, lt_ecc_key_generate(&h, TR01_ECC_SLOT_1, TR01_CURVE_ED25519));
//}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_OK when executed correctly
//void test_lt_ecc_key_generate__correct()
//{
//    lt_handle_t h = {0};
//    h.session     = SESSION_ON;
//
//    size_inject_value = 1;
//    lt_l3_cmd_Stub(callback_lt_ecc_key_generate_lt_l3_cmd);
//    for (ecc_slot_t slot = TR01_ECC_SLOT_1; slot < TR01_ECC_SLOT_31; slot++) {
//        TEST_ASSERT_EQUAL(LT_OK, lt_ecc_key_generate(&h, slot, TR01_CURVE_ED25519));
//        TEST_ASSERT_EQUAL(LT_OK, lt_ecc_key_generate(&h, slot, TR01_CURVE_P256));
//    }
//}
*/