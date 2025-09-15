/**
 * @file test_libtropic__lt_random_get.c
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

// Test if function returns LT_PARAM_ERR on invalid handle
/*void test__invalid_handle()
{
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_random_get(NULL, (uint8_t*)"", 0));
}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_PARAM_ERR on invalid buff
void test__invalid_buff()
{
    lt_handle_t h = {0};
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_random_get(&h, NULL, 0));
}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_PARAM_ERR on invalid len
void test__invalid_len()
{
    lt_handle_t h = {0};
    uint16_t len;
    for (int i = 0; i < 25; i++) {
        len = TR01_RANDOM_VALUE_GET_LEN_MAX + 1;
        len += rand() % (UINT16_MAX - len);
        TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_random_get(&h, (uint8_t*)"", len));
    }
}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- EXECUTION ------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_HOST_NO_SESSION when handle's variable 'session' is not set to SESSION_ON
void test__no_session()
{
    uint8_t     buff[200];
    lt_handle_t h =  {0};
    h.session     = 0;

    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, lt_random_get(&h, buff, sizeof(buff)));
}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_FAIL when lt_l3() fails
void test__l3_fail()
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

//---------------------------------------------------------------------------------------------------------//

uint16_t size_inject_value;
lt_ret_t callback__lt_l3_cmd(lt_handle_t *h, int __attribute__((unused)) cmock_num_calls)
{
    struct lt_l3_random_value_get_res_t* p_l3_res = (struct lt_l3_random_value_get_res_t*)&h->l3_buff;
    p_l3_res->res_size = size_inject_value;

    return LT_OK;
}

// Test if function returns LT_FAIL if res_size field in result structure contains unexpected size
void test__len_mismatch()
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

        size_inject_value = (uint16_t)rand_size;

        lt_l3_cmd_Stub(callback__lt_l3_cmd);
        TEST_ASSERT_EQUAL(LT_FAIL, lt_random_get(&h, buff, (uint16_t)(rand_size + rand_len_offset)));
    }
}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_OK when executed correctly
void test__correct()
{
    lt_handle_t h =  {0};
    h.session = SESSION_ON;
    uint8_t buff[TR01_RANDOM_VALUE_GET_LEN_MAX];

    size_inject_value = TR01_RANDOM_VALUE_GET_LEN_MAX + LT_L3_RANDOM_VALUE_GET_RES_SIZE_MIN;
    lt_l3_cmd_Stub(callback__lt_l3_cmd);
    TEST_ASSERT_EQUAL(LT_OK, lt_random_get(&h, buff, TR01_RANDOM_VALUE_GET_LEN_MAX));
}
*/