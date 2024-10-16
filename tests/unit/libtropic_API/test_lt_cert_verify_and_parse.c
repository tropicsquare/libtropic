/**
 * @file test_libtropic__lt_cert_verify_and_parse.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "unity.h"
#include "string.h"
#include "time.h"

#include "libtropic_common.h"
#include "libtropic.h"
#include "lt_l2_api_structs.h"

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

// Test if function returns LT_PARAM_ERROR when invalid handle is passed
void test__invalid_cert()
{
    uint8_t     stpub[32] = {0};
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_cert_verify_and_parse(NULL, LT_L2_GET_INFO_REQ_CERT_SIZE, stpub));
}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_PARAM_ERROR when invalid max_len is passed
void test__invalid_len()
{
    uint8_t dummy_cert[LT_L2_GET_INFO_REQ_CERT_SIZE] = {0};
    uint8_t stpub[32] = {0};

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_cert_verify_and_parse(dummy_cert, LT_L2_GET_INFO_REQ_CERT_SIZE - 1, stpub));
}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_PARAM_ERROR when invalid stpub is passed
void test__invalid_stpub()
{
    uint8_t dummy_cert[LT_L2_GET_INFO_REQ_CERT_SIZE] = {0};

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_cert_verify_and_parse(dummy_cert, LT_L2_GET_INFO_REQ_CERT_SIZE, NULL));
}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- EXECUTION ------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_FAIL when certificate buffer does not contain identificator for stpub key
void test__cert_without_key()
{
    uint8_t dummy_cert[LT_L2_GET_INFO_REQ_CERT_SIZE] = {0};
    uint8_t stpub[32] = {0};

    TEST_ASSERT_EQUAL(LT_FAIL, lt_cert_verify_and_parse(dummy_cert, LT_L2_GET_INFO_REQ_CERT_SIZE, stpub));
}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_OK for some correct inputs
void test__correct()
{
    uint8_t stpub[32] = {0};

    for(int i = 0; i < (512-3-32); i++) {
        // Prepare certificate
        uint8_t dummy_cert[LT_L2_GET_INFO_REQ_CERT_SIZE];
        dummy_cert[i]   = 0x65;
        dummy_cert[i+1] = 0x6e;
        dummy_cert[i+2] = 0x03;
        dummy_cert[i+3] = 0x21;

        TEST_ASSERT_EQUAL(LT_OK, lt_cert_verify_and_parse(dummy_cert, LT_L2_GET_INFO_REQ_CERT_SIZE, stpub));
    }
}
