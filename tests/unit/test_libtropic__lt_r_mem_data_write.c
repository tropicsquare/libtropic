/**
 * @file test_libtropic__lt_r_mem_data_write.c
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

void test_lt_r_mem_data_write__invalid_handle()
{
    lt_handle_t h;
    uint16_t udata_slot;
    uint8_t udata[100];
    uint16_t size;

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_r_mem_data_write(NULL, udata_slot, udata, size));
}

void test_lt_r_mem_data_write__invalid_udata_slot()
{
    lt_handle_t h;
    uint16_t udata_slot;
    uint8_t udata[100];
    uint16_t size;

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_r_mem_data_write(&h, 512, udata, size));
}

void test_lt_r_mem_data_write__invalid_udata()
{
    lt_handle_t h;
    uint16_t udata_slot;
    uint8_t udata[100];
    uint16_t size;

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_r_mem_data_write(&h, 0, NULL, size));
}

void test_lt_r_mem_data_write__invalid_size()
{
    lt_handle_t h;
    uint16_t udata_slot;
    uint8_t udata[100];
    uint16_t size;

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_r_mem_data_write(&h, udata_slot, udata, 445));
}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- EXECUTION ------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

void test_lt_r_mem_data_write__no_session()
{
    lt_handle_t h = {0};
    uint16_t udata_slot;
    uint8_t udata[100];
    uint16_t size;

    TEST_ASSERT_EQUAL(LT_HOST_NO_SESSION, lt_r_mem_data_write(&h, udata_slot, udata, size));
}

