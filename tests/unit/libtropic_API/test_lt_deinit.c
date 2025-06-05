/**
 * @file test_libtropic__lt_deinit.c
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
#include "mock_lt_l3_process.h"
#include "mock_lt_l3.h"
#include "mock_lt_x25519.h"
#include "mock_lt_ed25519.h"
#include "mock_lt_hkdf.h"
#include "mock_lt_sha256.h"
#include "mock_lt_aesgcm.h"
#include "mock_lt_asn1_der.h"

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
void test__invalid_handle()
{
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_deinit(NULL));
}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- EXECUTION ------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_FAIL when l1 deinit failed
void test__lt_l1_deinit_fail()
{
    lt_handle_t h = {0};

    lt_l1_deinit_ExpectAndReturn(&h.l2, LT_FAIL);
    TEST_ASSERT_EQUAL(LT_FAIL, lt_deinit(&h));
}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_OK when all went correctly
void test__correct()
{
    lt_handle_t h = {0};

    lt_l1_deinit_ExpectAndReturn(&h.l2, LT_OK);
    TEST_ASSERT_EQUAL(LT_OK, lt_deinit(&h));
}
