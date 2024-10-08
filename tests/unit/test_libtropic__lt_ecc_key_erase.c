/**
 * @file test_libtropic__lt_ecc_key_erase.c
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
    char buffer[100];
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

void test_lt_ecc_key_erase__invalid_handle()
{
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_erase(NULL, ECC_SLOT_1));
}

void test_lt_ecc_key_erase__invalid_slot()
{
    lt_handle_t h;
    h.session = SESSION_ON;

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_erase(&h, LT_L3_ECC_KEY_GENERATE_SLOT_MIN - 1));
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_erase(&h, LT_L3_ECC_KEY_GENERATE_SLOT_MAX + 1));
}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- EXECUTION ------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

void test_lt_ecc_key_erase__no_session()
{
    lt_handle_t h = {0};
    h.session     = 0;

    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, lt_ecc_key_erase(&h, ECC_SLOT_1));
}

void test_lt_ecc_key_erase__l3_fail()
{
    lt_handle_t h = {0};
    h.session     = SESSION_ON;

    lt_ret_t rets[] = {LT_L3_FAIL, LT_L3_UNAUTHORIZED, LT_L3_INVALID_CMD, LT_FAIL};
    for (size_t i = 0; i < (sizeof(rets)/sizeof(rets[0])); i++) {
        lt_l3_cmd_ExpectAndReturn(&h, rets[i]);
        TEST_ASSERT_EQUAL(rets[i], lt_ecc_key_erase(&h, ECC_SLOT_1));
    }
}

lt_ret_t callback_lt_ecc_key_erase_lt_l3_cmd(lt_handle_t *h, int cmock_num_calls)
{
    struct lt_l3_eddsa_sign_res_t* p_l3_res = (struct lt_l3_eddsa_sign_res_t*)&h->l3_buff;

    p_l3_res->res_size = 1;

    return LT_OK;
}

void test_lt_ecc_key_erase__correct()
{
    lt_handle_t h = {0};
    h.session     = SESSION_ON;

    lt_l3_cmd_Stub(callback_lt_ecc_key_erase_lt_l3_cmd);
    TEST_ASSERT_EQUAL(LT_OK, lt_ecc_key_erase(&h, ECC_SLOT_1));
}
