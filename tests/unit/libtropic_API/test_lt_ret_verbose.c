/**
 * @file test_libtropic__lt_ret_verbose.c
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

// This function parses all known values, otherwise returns error string. No need to test input parameters.

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- EXECUTION ------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

// Test that all return values can be displayed as a string (for better debug)
void test__correct()
{
    TEST_ASSERT_EQUAL_STRING("LT_OK", lt_ret_verbose(LT_OK));
    TEST_ASSERT_EQUAL_STRING("LT_FAIL", lt_ret_verbose(LT_FAIL));
    TEST_ASSERT_EQUAL_STRING("LT_HOST_NO_SESSION", lt_ret_verbose(LT_HOST_NO_SESSION));
    TEST_ASSERT_EQUAL_STRING("LT_PARAM_ERR", lt_ret_verbose(LT_PARAM_ERR));
    TEST_ASSERT_EQUAL_STRING("LT_CRYPTO_ERR", lt_ret_verbose(LT_CRYPTO_ERR));

    TEST_ASSERT_EQUAL_STRING("LT_L1_SPI_ERROR", lt_ret_verbose(LT_L1_SPI_ERROR));
    TEST_ASSERT_EQUAL_STRING("LT_L1_DATA_LEN_ERROR", lt_ret_verbose(LT_L1_DATA_LEN_ERROR));
    TEST_ASSERT_EQUAL_STRING("LT_L1_CHIP_STARTUP_MODE", lt_ret_verbose(LT_L1_CHIP_STARTUP_MODE));
    TEST_ASSERT_EQUAL_STRING("LT_L1_CHIP_ALARM_MODE", lt_ret_verbose(LT_L1_CHIP_ALARM_MODE));
    TEST_ASSERT_EQUAL_STRING("LT_L1_CHIP_BUSY", lt_ret_verbose(LT_L1_CHIP_BUSY));

    TEST_ASSERT_EQUAL_STRING("LT_L3_R_MEM_DATA_WRITE_WRITE_FAIL", lt_ret_verbose(LT_L3_R_MEM_DATA_WRITE_WRITE_FAIL));
    TEST_ASSERT_EQUAL_STRING("LT_L3_R_MEM_DATA_WRITE_SLOT_EXPIRED", lt_ret_verbose(LT_L3_R_MEM_DATA_WRITE_SLOT_EXPIRED));
    TEST_ASSERT_EQUAL_STRING("LT_L3_ECC_INVALID_KEY", lt_ret_verbose(LT_L3_ECC_INVALID_KEY));
    TEST_ASSERT_EQUAL_STRING("LT_L3_MCOUNTER_UPDATE_UPDATE_ERR", lt_ret_verbose(LT_L3_MCOUNTER_UPDATE_UPDATE_ERR));
    TEST_ASSERT_EQUAL_STRING("LT_L3_COUNTER_INVALID", lt_ret_verbose(LT_L3_COUNTER_INVALID));
    TEST_ASSERT_EQUAL_STRING("LT_L3_PAIRING_KEY_EMPTY", lt_ret_verbose(LT_L3_PAIRING_KEY_EMPTY));
    TEST_ASSERT_EQUAL_STRING("LT_L3_PAIRING_KEY_INVALID", lt_ret_verbose(LT_L3_PAIRING_KEY_INVALID));
    TEST_ASSERT_EQUAL_STRING("LT_L3_OK", lt_ret_verbose(LT_L3_OK));
    TEST_ASSERT_EQUAL_STRING("LT_L3_FAIL", lt_ret_verbose(LT_L3_FAIL));
    TEST_ASSERT_EQUAL_STRING("LT_L3_UNAUTHORIZED", lt_ret_verbose(LT_L3_UNAUTHORIZED));
    TEST_ASSERT_EQUAL_STRING("LT_L3_INVALID_CMD", lt_ret_verbose(LT_L3_INVALID_CMD));
    TEST_ASSERT_EQUAL_STRING("LT_L3_DATA_LEN_ERROR", lt_ret_verbose(LT_L3_DATA_LEN_ERROR));

    TEST_ASSERT_EQUAL_STRING("LT_L2_IN_CRC_ERR", lt_ret_verbose(LT_L2_IN_CRC_ERR));
    TEST_ASSERT_EQUAL_STRING("LT_L2_REQ_CONT", lt_ret_verbose(LT_L2_REQ_CONT));
    TEST_ASSERT_EQUAL_STRING("LT_L2_RES_CONT", lt_ret_verbose(LT_L2_RES_CONT));
    TEST_ASSERT_EQUAL_STRING("LT_L2_HSK_ERR", lt_ret_verbose(LT_L2_HSK_ERR));
    TEST_ASSERT_EQUAL_STRING("LT_L2_NO_SESSION", lt_ret_verbose(LT_L2_NO_SESSION));
    TEST_ASSERT_EQUAL_STRING("LT_L2_TAG_ERR", lt_ret_verbose(LT_L2_TAG_ERR));
    TEST_ASSERT_EQUAL_STRING("LT_L2_CRC_ERR", lt_ret_verbose(LT_L2_CRC_ERR));
    TEST_ASSERT_EQUAL_STRING("LT_L2_GEN_ERR", lt_ret_verbose(LT_L2_GEN_ERR));
    TEST_ASSERT_EQUAL_STRING("LT_L2_NO_RESP", lt_ret_verbose(LT_L2_NO_RESP));
    TEST_ASSERT_EQUAL_STRING("LT_L2_UNKNOWN_REQ", lt_ret_verbose(LT_L2_UNKNOWN_REQ));
    TEST_ASSERT_EQUAL_STRING("LT_L2_STATUS_NOT_RECOGNIZED", lt_ret_verbose(LT_L2_STATUS_NOT_RECOGNIZED));
    TEST_ASSERT_EQUAL_STRING("LT_L2_DATA_LEN_ERROR", lt_ret_verbose(LT_L2_DATA_LEN_ERROR));

    TEST_ASSERT_EQUAL_STRING("FATAL ERROR, unknown return value", lt_ret_verbose(99));
}
