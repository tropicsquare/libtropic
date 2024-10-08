/**
 * @file test_libtropic__lt_pairing_key_invalidate.c
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

void test_lt_pairing_key_invalidate__invalid_handle()
{
    uint8_t slot = 0;
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_pairing_key_invalidate(NULL, slot));
}

void test_lt_pairing_key_invalidate__invalid_slot()
{
    lt_handle_t h;
    h.session = SESSION_ON;

    uint8_t slot = 0;
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_pairing_key_invalidate(&h, 4));
}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- EXECUTION ------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

void test_lt_pairing_key_invalidate__no_session()
{
    lt_handle_t h =  {0};

    uint8_t pubkey[32] = {0};
    uint8_t slot = 0;

    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, lt_pairing_key_invalidate(&h, slot));
}

void test_lt_pairing_key_invalidate__l3_cmd_fail()
{
    lt_handle_t h =  {0};
    h.session = SESSION_ON;
    uint8_t pubkey[32] = {0};
    uint8_t slot = 0;
    lt_l3_cmd_ExpectAndReturn(&h, LT_FAIL);

    TEST_ASSERT_EQUAL(LT_FAIL, lt_pairing_key_invalidate(&h, slot));
}


lt_ret_t callback_lt_pairing_key_invalidate(lt_handle_t *h, int __attribute__((unused)) cmock_num_calls)
{
    struct lt_l3_pairing_key_read_res_t* p_l3_res = (struct lt_l3_pairing_key_read_res_t*)&h->l3_buff;
    p_l3_res->res_size = 100;

    return LT_OK;
}

void test_lt_pairing_key_invalidate__len_mismatch()
{
    lt_handle_t h =  {0};
    h.session = SESSION_ON;
    uint8_t pubkey[32] = {0};
    uint8_t slot = 0;
    lt_l3_cmd_Stub(callback_lt_pairing_key_invalidate);

    TEST_ASSERT_EQUAL(LT_FAIL, lt_pairing_key_invalidate(&h, slot));
}

lt_ret_t callback2_lt_pairing_key_invalidate(lt_handle_t *h, int __attribute__((unused)) cmock_num_calls)
{
    struct lt_l3_pairing_key_read_res_t* p_l3_res = (struct lt_l3_pairing_key_read_res_t*)&h->l3_buff;
    p_l3_res->res_size = 1;

    return LT_OK;
}

void test_lt_pairing_key_invalidate__correct()
{
    lt_handle_t h =  {0};
    h.session = SESSION_ON;
    uint8_t pubkey[32] = {0};
    uint8_t slot = 0;
    lt_l3_cmd_Stub(callback2_lt_pairing_key_invalidate);

    TEST_ASSERT_EQUAL(LT_OK, lt_pairing_key_invalidate(&h, slot));
}
