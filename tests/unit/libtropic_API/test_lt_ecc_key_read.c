/**
 * @file test_libtropic__lt_ecc_key_read.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "unity.h"
#include "string.h"
#include "time.h"

#include "libtropic_common.h"
#include "libtropic.h"
#include "lt_l3_api_structs.h"

#include "mock_lt_random.h"
#include "mock_lt_l1_port_wrap.h"
#include "mock_lt_l1.h"
#include "mock_lt_l2.h"
#include "mock_lt_l3.h"
#include "mock_lt_x25519.h"
#include "mock_lt_ed25519.h"
#include "mock_lt_hkdf.h"
#include "mock_lt_sha256.h"
#include "mock_lt_aesgcm.h"

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

void tearDown(void)
{
}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- INPUT PARAMETERS   ---------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_PARAM_ERR on invalid handle
void test_lt_ecc_key_read__invalid_handle()
{
    lt_ecc_curve_type_t curve = 0;
    ecc_key_origin_t origin = 0;
    uint8_t key[64];
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_read(NULL, ECC_SLOT_1, key, 64, &curve, &origin));
}

// Test if function returns LT_PARAM_ERR on invalid slot
void test_lt_ecc_key_read__invalid_slot()
{
    lt_handle_t h = {0};
    h.session = SESSION_ON;
    lt_ecc_curve_type_t curve = 0;
    ecc_key_origin_t origin = 0;
    uint8_t key[64];
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_read(&h, ECC_SLOT_0 - 1, key, 64, &curve, &origin));
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_read(&h, ECC_SLOT_31 + 1, key, 64, &curve, &origin));
}

// Test if function returns LT_PARAM_ERR on invalid key
void test_lt_ecc_key_read__invalid_key()
{
    lt_handle_t h = {0};
    h.session = SESSION_ON;
    lt_ecc_curve_type_t curve = 0;
    ecc_key_origin_t origin = 0;
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_read(&h, ECC_SLOT_1, NULL, 64, &curve, &origin));
}

// Test if function returns LT_PARAM_ERR on invalid key_len
void test_lt_ecc_key_read__invalid_key_len()
{
    lt_handle_t h = {0};
    h.session = SESSION_ON;
    lt_ecc_curve_type_t curve = 0;
    ecc_key_origin_t origin = 0;
    uint8_t key[64] = {0};
    for (int i = 0; i < 25; i++) {
        TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_read(&h, ECC_SLOT_1, key, rand() % 64, &curve, &origin));
    }
}

// Test if function returns LT_PARAM_ERR on invalid curve
void test_lt_ecc_key_read__invalid_curve()
{
    lt_handle_t h = {0};
    h.session = SESSION_ON;
    ecc_key_origin_t origin = 0;
    uint8_t key[64] ={0};
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_read(&h, ECC_SLOT_1, key, 64, NULL, &origin));
}

// Test if function returns LT_PARAM_ERR on invalid origin
void test_lt_ecc_key_read__invalid_origin()
{
    lt_handle_t h = {0};
    h.session = SESSION_ON;
    lt_ecc_curve_type_t curve = 0;
    uint8_t key[64] = {0};
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_read(&h, ECC_SLOT_1, key, 64, &curve, NULL));
}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- EXECUTION ------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_HOST_NO_SESSION when handle's variable 'session' is not set to 'SESSION_ON'
void test_lt_ecc_key_read__no_session()
{
    lt_handle_t h =  {0};
    uint8_t key[64] = {0};
    lt_ecc_curve_type_t curve = 0;
    ecc_key_origin_t origin = 0;

    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, lt_ecc_key_read(&h, ECC_SLOT_1, key, sizeof(key), &curve, &origin));
}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_FAIL when lt_l3() fails
void test_lt_ecc_key_read__l3_fail()
{
    lt_handle_t h =  {0};
    h.session     = SESSION_ON;
    uint8_t          key[64] = {0};
    lt_ecc_curve_type_t curve = 0;
    ecc_key_origin_t origin = 0;

    lt_ret_t rets[] = {LT_L3_FAIL, LT_L3_UNAUTHORIZED, LT_L3_INVALID_CMD, LT_FAIL};
    for (size_t i = 0; i < (sizeof(rets)/sizeof(rets[0])); i++) {
        lt_l3_cmd_ExpectAndReturn(&h, rets[i]);
        TEST_ASSERT_EQUAL(rets[i], lt_ecc_key_read(&h, ECC_SLOT_1, key, sizeof(key), &curve, &origin));
    }
}

//---------------------------------------------------------------------------------------------------------//

uint16_t size_inject_value;
lt_ecc_curve_type_t curve_inject_valuet_value;

lt_ret_t callback_lt_ecc_key_read_lt_l3_cmd(lt_handle_t *h, int __attribute__((unused)) cmock_num_calls)
{
    struct lt_l3_ecc_key_read_res_t* p_l3_res = (struct lt_l3_ecc_key_read_res_t*)&h->l3_buff;
    p_l3_res->res_size = size_inject_value;
    p_l3_res->curve       = curve_inject_valuet_value;

    return LT_OK;
}

// Test if function returns LT_FAIL if res_size field in result structure contains unexpected size
void test_lt_ecc_key_read__ed25519_size_mismatch()
{
    lt_handle_t h =  {0};
    h.session     = SESSION_ON;
    uint8_t          key[64] = {0};
    lt_ecc_curve_type_t curve = 0;
    ecc_key_origin_t origin = 0;

    lt_l3_cmd_Stub(callback_lt_ecc_key_read_lt_l3_cmd);

    for (int i = 0; i < 25; i++) {
        size_inject_value = (uint16_t)(rand() % L3_PACKET_MAX_SIZE);

        if (size_inject_value != 48) { // skip correct value
            curve_inject_valuet_value       = CURVE_ED25519;
            TEST_ASSERT_EQUAL(LT_FAIL, lt_ecc_key_read(&h, ECC_SLOT_1, key, sizeof(key), &curve, &origin));
        }

        if (size_inject_value != 80) { // skip correct value
            curve_inject_valuet_value       = CURVE_P256;
            TEST_ASSERT_EQUAL(LT_FAIL, lt_ecc_key_read(&h, ECC_SLOT_1, key, sizeof(key), &curve, &origin));
        }
    }
}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_OK when executed correctly
void test_lt_ecc_key_read__correct()
{
    lt_handle_t h =  {0};
    h.session     = SESSION_ON;
    uint8_t          key[64] = {0};
    lt_ecc_curve_type_t curve = 0;
    ecc_key_origin_t origin = 0;

    lt_l3_cmd_Stub(callback_lt_ecc_key_read_lt_l3_cmd);

    size_inject_value = 48;
    curve_inject_valuet_value       = CURVE_ED25519;
    TEST_ASSERT_EQUAL(LT_OK, lt_ecc_key_read(&h, ECC_SLOT_1, key, sizeof(key), &curve, &origin));

    size_inject_value = 80;
    curve_inject_valuet_value       = CURVE_P256;
    TEST_ASSERT_EQUAL(LT_OK, lt_ecc_key_read(&h, ECC_SLOT_1, key, sizeof(key), &curve, &origin));
}
