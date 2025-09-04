/**
 * @file test_libtropic__lt_pairing_key_write.c
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
    uint8_t pubkey[32] = {0};
    uint8_t slot = 0;
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_pairing_key_write(NULL, pubkey, slot));
}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_PARAM_ERR on invalid pubkey
void test__invalid_pubkey()
{
    lt_handle_t h = {0};
    uint8_t slot = 0;
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_pairing_key_write(&h, NULL, slot));
}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_PARAM_ERR on invalid slot
void test__invalid_slot()
{
    lt_handle_t h = {0};
    uint8_t pubkey[32] = {0};
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_pairing_key_write(&h, pubkey, TR01_PAIRING_KEY_SLOT_INDEX_3 + 1));
}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- EXECUTION ------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_HOST_NO_SESSION when handle's variable 'session' is not set to SESSION_ON
void test__no_session()
{
    lt_handle_t h =  {0};
    uint8_t pubkey[32] = {0};
    uint8_t slot = 0;

    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, lt_pairing_key_write(&h, pubkey, slot));
}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_FAIL when lt_l3() fails
void test__l3_cmd_fail()
{
    lt_handle_t h =  {0};
    h.session = SESSION_ON;
    uint8_t pubkey[32] = {0};
    uint8_t slot = 0;

    lt_l3_cmd_ExpectAndReturn(&h, LT_FAIL);
    TEST_ASSERT_EQUAL(LT_FAIL, lt_pairing_key_write(&h, pubkey, slot));
}

//---------------------------------------------------------------------------------------------------------//

uint16_t size_inject_value;
lt_ret_t callback__lt_l3_cmd(lt_handle_t *h, int __attribute__((unused)) cmock_num_calls)
{
    struct lt_l3_pairing_key_write_res_t* p_l3_res = (struct lt_l3_pairing_key_write_res_t*)&h->l3_buff;
    p_l3_res->res_size = size_inject_value;

    return LT_OK;
}

// Test if function returns LT_FAIL if res_size field in result structure contains unexpected size
void test__len_mismatch()
{
    lt_handle_t h =  {0};
    h.session = SESSION_ON;
    uint8_t pubkey[32] = {0};
    uint8_t slot = 0;

    size_inject_value = 1+1;
    lt_l3_cmd_Stub(callback__lt_l3_cmd);

    TEST_ASSERT_EQUAL(LT_FAIL, lt_pairing_key_write(&h, pubkey, slot));
}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_OK when executed correctly
void test__correct()
{
    lt_handle_t h =  {0};
    h.session = SESSION_ON;
    uint8_t pubkey[32] = {0};
    uint8_t slot = 0;

    size_inject_value = 1;
    lt_l3_cmd_Stub(callback__lt_l3_cmd);

    TEST_ASSERT_EQUAL(LT_OK, lt_pairing_key_write(&h, pubkey, slot));
}
*/