/**
 * @file test_libtropic__lt_ecc_ecdsa_sign.c
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

void test_lt_ecc_ecdsa_sign__invalid_handle()
{
    uint8_t msg[1] = {0};
    uint8_t  rs[64] = {0};

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_ecdsa_sign(NULL, ECC_SLOT_1, msg, 1, rs, 64));
}

void test_lt_ecc_ecdsa_sign__invalid_slot()
{
    lt_handle_t h;
    h.session = SESSION_ON;

    uint8_t msg[1] = {0};
    uint8_t rs[64] = {0};

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_ecdsa_sign(&h, LT_L3_ECC_KEY_GENERATE_SLOT_MIN - 1, msg, 1, rs, 64));
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_ecdsa_sign(&h, LT_L3_ECC_KEY_GENERATE_SLOT_MAX + 1, msg, 1, rs, 64));
}

void test_lt_ecc_ecdsa_sign__invalid_msg()
{
    lt_handle_t h;
    h.session = SESSION_ON;

    uint8_t rs[64] = {0};

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_ecdsa_sign(&h, ECC_SLOT_1, NULL, 1, rs, 64));
}

void test_lt_ecc_ecdsa_sign__invalid_msg_len()
{
    lt_handle_t h;
    h.session = SESSION_ON;

    uint8_t msg[LT_L3_ECDSA_SIGN_MSG_LEN_MAX + 1] = {0};
    uint8_t rs[64] = {0};

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_ecdsa_sign(&h, ECC_SLOT_1, msg, LT_L3_ECDSA_SIGN_MSG_LEN_MAX + 1, rs, 64));
}

void test_lt_ecc_ecdsa_sign__invalid_rs()
{
    lt_handle_t h;
    h.session = SESSION_ON;

    uint8_t msg[1] = {0};

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_ecdsa_sign(&h, ECC_SLOT_1, msg, 1, NULL, 64));
}

void test_lt_ecc_ecdsa_sign__invalid_rs_len()
{
    lt_handle_t h;
    h.session = SESSION_ON;

    uint8_t msg[1] = {0};
    uint8_t rs[64] = {0};

    for (int i = 0; i < 25; i++) {
        TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_ecdsa_sign(&h, ECC_SLOT_1, msg, 1, rs, rand() % 64));
    }
}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- EXECUTION ------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

void test_lt_ecc_ecdsa_sign__no_session()
{
    lt_handle_t h = {0};
    h.session     = 0;

    uint8_t     msg[10] = {0};
    uint8_t     rs[64] = {0};

    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, lt_ecc_ecdsa_sign(&h, ECC_SLOT_1, msg, sizeof(msg), rs, sizeof(rs)));
}

void test_lt_ecc_ecdsa_sign__l3_fail()
{
    lt_handle_t h = {0};
    h.session     = SESSION_ON;

    uint8_t                msg[10] = {0};
    uint8_t                rs[64]  = {0};
    lt_crypto_sha256_ctx_t hctx         = {0};
    uint8_t                msg_hash[32] = {0};

    lt_ret_t rets[] = {LT_L3_FAIL, LT_L3_UNAUTHORIZED, LT_L3_INVALID_CMD, LT_FAIL};
    for (size_t i = 0; i < (sizeof(rets)/sizeof(rets[0])); i++) {
        lt_sha256_init_Expect(&hctx);
        lt_sha256_start_Expect(&hctx);
        lt_sha256_update_Expect(&hctx, msg, sizeof(msg));
        lt_sha256_finish_Expect(&hctx, msg_hash);
        lt_l3_cmd_ExpectAndReturn(&h, rets[i]);
        TEST_ASSERT_EQUAL(rets[i], lt_ecc_ecdsa_sign(&h, ECC_SLOT_1, msg, sizeof(msg), rs, sizeof(rs)));
    }
}

lt_ret_t callback_lt_ecc_ecdsa_sign_lt_l3_cmd(lt_handle_t *h, int cmock_num_calls)
{
    struct lt_l3_eddsa_sign_res_t* p_l3_res = (struct lt_l3_eddsa_sign_res_t*)&h->l3_buff;

    p_l3_res->res_size = 0x50;

    return LT_OK;
}

void test_lt_ecc_ecdsa_sign__correct()
{
    lt_handle_t h = {0};
    h.session     = SESSION_ON;

    uint8_t                msg[10] = {0};
    uint8_t                rs[64]  = {0};
    lt_crypto_sha256_ctx_t hctx         = {0};
    uint8_t                msg_hash[32] = {0};

    lt_sha256_init_Expect(&hctx);
    lt_sha256_start_Expect(&hctx);
    lt_sha256_update_Expect(&hctx, msg, sizeof(msg));
    lt_sha256_finish_Expect(&hctx, msg_hash);
    lt_l3_cmd_Stub(callback_lt_ecc_ecdsa_sign_lt_l3_cmd);
    TEST_ASSERT_EQUAL(LT_OK, lt_ecc_ecdsa_sign(&h, ECC_SLOT_1, msg, sizeof(msg), rs, sizeof(rs)));
}
