/**
 * @file test_libtropic__lt_ecc_eddsa_sig_verify.c
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

/*// Test if function returns LT_PARAM_ERR on invalid msg
void test__invalid_msg()
{
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_eddsa_sig_verify(NULL, LT_L3_EDDSA_SIGN_CMD_MSG_LEN_MIN, (uint8_t*)"",
(uint8_t*)""));
}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_PARAM_ERR on invalid msg_len
void test__invalid_msg_len()
{
    uint8_t msg[TR01_L3_EDDSA_SIGN_CMD_MSG_LEN_MAX + 1];

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_eddsa_sig_verify((uint8_t*)"", 0, (uint8_t*)"", (uint8_t*)""));
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_eddsa_sig_verify(msg, sizeof(msg), (uint8_t*)"", (uint8_t*)""));
}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_PARAM_ERR on invalid pubkey
void test__invalid_pubkey()
{
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_eddsa_sig_verify((uint8_t*)"", 1, NULL, (uint8_t*)""));
}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_PARAM_ERR on invalid rs
void test__invalid_rs()
{
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_eddsa_sig_verify((uint8_t*)"", 1, (uint8_t*)"", NULL));
}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- EXECUTION ------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_FAIL when lt_ed25519_sign_open() returns 1
void test__fail()
{
    uint8_t msg[10]= {0};
    uint8_t pubkey[64]= {0};
    uint8_t rs[64] = {0};

    lt_ed25519_sign_open_ExpectAndReturn(msg, sizeof(msg), pubkey, rs, 1);
    TEST_ASSERT_EQUAL(LT_FAIL, lt_ecc_eddsa_sig_verify(msg, sizeof(msg), pubkey, rs));
}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_OK when execution is correct
void test__correct()
{
    uint8_t msg[10]    = {0};
    uint8_t pubkey[64] = {0};
    uint8_t rs[64]     = {0};

    lt_ed25519_sign_open_ExpectAndReturn(msg, sizeof(msg), pubkey, rs, 0);
    TEST_ASSERT_EQUAL(LT_OK, lt_ecc_eddsa_sig_verify(msg, sizeof(msg), pubkey, rs));
}
*/