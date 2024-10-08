/**
 * @file test_libtropic__lt_ping.c
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

void test_lt_ping__invalid_handle()
{
    uint8_t msg_out = 0;
    uint8_t msg_in = 0;
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ping(NULL, &msg_out, &msg_in, 0));
}

void test_lt_ping__invalid_msg_out()
{
    lt_handle_t h;
    h.session = SESSION_ON;
    uint8_t msg_in;
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ping(&h, NULL, &msg_in, 0));
}

void test_lt_ping__invalid_msg_in()
{
    lt_handle_t h;
    h.session = SESSION_ON;
    uint8_t msg_out = 0;
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ping(&h, &msg_out, NULL, 0));
}

void test_lt_ping__invalid_len()
{
    lt_handle_t h;
    h.session = SESSION_ON;

    uint8_t msg_out, msg_in;
    uint16_t len;

    for (int i = 0; i < 25; i++) {
        len = PING_LEN_MAX + 1;
        len += rand() % (UINT16_MAX - len);
        TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ping(&h, &msg_out, &msg_in, len));
    }
}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- EXECUTION ------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

void test_lt_ping__no_session()
{
    lt_handle_t h = {0};
    h.session     = 0;

    uint8_t msg_out = 1;
    uint8_t msg_in = 0;

    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, lt_ping(&h, &msg_out, &msg_in, 1));
}

void test_lt_ping__l3_fail()
{
    lt_handle_t h =  {0};
    h.session     = SESSION_ON;

    uint8_t msg_out, msg_in;

    lt_ret_t rets[] = {LT_L3_FAIL, LT_L3_UNAUTHORIZED, LT_L3_INVALID_CMD, LT_FAIL};
    for (size_t i = 0; i < (sizeof(rets)/sizeof(rets[0])); i++) {
        lt_l3_cmd_ExpectAndReturn(&h, rets[i]);
        TEST_ASSERT_EQUAL(rets[i], lt_ping(&h, &msg_out, &msg_in, 0));
    }
}

uint16_t lt_ping_cmd_size_inject_value;

lt_ret_t callback_lt_ping_lt_l3_cmd(lt_handle_t *h, int __attribute__((unused)) cmock_num_calls)
{
    struct lt_l3_ping_res_t* p_l3_res = (struct lt_l3_ping_res_t*)&h->l3_buff;
    p_l3_res->res_size = lt_ping_cmd_size_inject_value;

    return LT_OK;
}

void test_lt_ping__len_mismatch()
{
    const int msg_max_size = 200;
    uint8_t   msg_out[msg_max_size], msg_in[msg_max_size];
    int       rand_size, rand_len_offset;

    lt_handle_t h =  {0};
    h.session     = SESSION_ON;

    for (int i = 0; i < 25; i++) {
        rand_size       = rand() % msg_max_size;
        rand_len_offset = rand() % (msg_max_size - rand_size);
        if (rand_len_offset == 1) {
            rand_len_offset++;
        }

        lt_ping_cmd_size_inject_value = (uint16_t)rand_size;

        // Packet size for both cmd and res has the same position, so
        // it would be overwritten by "p_l3_cmd->cmd_size = len + 1;",
        // so we need to modify the value inside the callback.
        lt_l3_cmd_StubWithCallback(callback_lt_ping_lt_l3_cmd);
        TEST_ASSERT_EQUAL(LT_FAIL, lt_ping(&h, msg_out, msg_in, (uint16_t)(rand_size + rand_len_offset)));
    }
}

void test_lt_ping__correct()
{
    const int       msg_max_size = 200;
    uint8_t         msg_out[200], msg_in[200];

    lt_handle_t h =  {0};
    h.session     = SESSION_ON;

    // Because packet size position is shared by both cmd and res,
    // it will already be set correctly by "p_l3_cmd->cmd_size = len + 1;".
    lt_l3_cmd_ExpectAndReturn(&h, LT_OK);
    TEST_ASSERT_EQUAL(LT_OK, lt_ping(&h, msg_out, msg_in, (uint16_t)(rand() % msg_max_size)));
}
