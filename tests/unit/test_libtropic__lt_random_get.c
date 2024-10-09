/**
 * @file test_libtropic__lt_random_get.c
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

void test_lt_random_get__invalid_handle()
{
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_random_get(NULL, (uint8_t*)"", 0));
}

void test_lt_random_get__invalid_buff()
{
    lt_handle_t h;
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_random_get(&h, NULL, 0));
}

void test_lt_random_get__invalid_len()
{
    lt_handle_t h;
    uint16_t len;

    h.session = SESSION_ON;

    for (int i = 0; i < 25; i++) {
        len = RANDOM_VALUE_GET_LEN_MAX + 1;
        len += rand() % (UINT16_MAX - len);
        TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_random_get(&h, (uint8_t*)"", len));
    }
}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- EXECUTION ------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

void test_lt_random_get__no_session()
{
    uint8_t     buff[200];
    lt_handle_t h =  {0};
    h.session     = 0;

    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, lt_random_get(&h, buff, sizeof(buff)));
}

void test_lt_random_get__l3_fail()
{
    uint8_t     buff[10];
    lt_handle_t h =  {0};
    h.session     = SESSION_ON;

    lt_ret_t rets[] = {LT_L3_FAIL, LT_L3_UNAUTHORIZED, LT_L3_INVALID_CMD, LT_FAIL};
    for (size_t i = 0; i < 4; i++) {
        lt_l3_cmd_ExpectAndReturn(&h, rets[i]);
        TEST_ASSERT_EQUAL(rets[i], lt_random_get(&h, buff, sizeof(buff)));
    }
}

uint16_t lt_random_get_cmd_size_inject_value;

lt_ret_t callback_lt_random_get_lt_l3_cmd(lt_handle_t *h, int __attribute__((unused)) cmock_num_calls)
{
    struct lt_l3_random_value_get_res_t* p_l3_res = (struct lt_l3_random_value_get_res_t*)&h->l3_buff;
    p_l3_res->res_size = lt_random_get_cmd_size_inject_value;

    return LT_OK;
}

void test_lt_random_get__len_mismatch()
{
    const int                buff_max_size = 200;
    uint8_t                  buff[buff_max_size];
    int                      rand_size, rand_len_offset;

    lt_handle_t h = {0};
    h.session     = SESSION_ON;

    for (int i = 0; i < 25; i++) {
        rand_size       = rand() % buff_max_size;
        rand_len_offset = rand() % (buff_max_size - rand_size);
        if (rand_len_offset == 4) {
            rand_len_offset++;
        }

        lt_random_get_cmd_size_inject_value = (uint16_t)rand_size;

        lt_l3_cmd_Stub(callback_lt_random_get_lt_l3_cmd);
        TEST_ASSERT_EQUAL(LT_FAIL, lt_random_get(&h, buff, (uint16_t)(rand_size + rand_len_offset)));
    }
}

void test_lt_random_get__correct()
{
    const int       buff_max_size = 200;
    uint8_t         buff[200];
    int             cmd_size;

    lt_handle_t h =  {0};
    h.session     = SESSION_ON;

    // Making this at least 4 to not underflow in cmd_size - 4.
    cmd_size = (rand() % (buff_max_size - 4)) + 4;

    // No correct value will be set for us as in ping, so injecting again...
    lt_random_get_cmd_size_inject_value = (uint16_t)cmd_size;
    lt_l3_cmd_Stub(callback_lt_random_get_lt_l3_cmd);
    TEST_ASSERT_EQUAL(LT_OK, lt_random_get(&h, buff, (uint16_t)(cmd_size - 4)));
}
