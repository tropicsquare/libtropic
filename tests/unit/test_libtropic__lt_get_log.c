/**
 * @file test_libtropic__lt_get_log.c
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


// Test if function returns LT_PARAM_ERR on invalid handle
void test__invalid_handle()
{
    uint8_t msg[GET_LOG_MAX_MSG_LEN] = {0};
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_get_log(NULL, msg, GET_LOG_MAX_MSG_LEN));
}

// Test if function returns LT_PARAM_ERR on invalid log_msg
void test__invalid_log_msg()
{
    lt_handle_t h = {0};
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_get_log(&h, NULL, GET_LOG_MAX_MSG_LEN));
}

// Test if function returns LT_PARAM_ERR on invalid msg_len_max
void test__invalid_msg_len_max()
{
    lt_handle_t h = {0};
    uint8_t msg[GET_LOG_MAX_MSG_LEN+1] = {0};
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_get_log(&h, msg, GET_LOG_MAX_MSG_LEN + 1));
}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- EXECUTION ------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

// Test if function propagates l2 error if l2 transfer fails
void test__lt_l2_transfer_fail()
{
    lt_handle_t h = {0};
    uint8_t msg[GET_LOG_MAX_MSG_LEN] = {0};

    lt_ret_t rets[] = {LT_L1_SPI_ERROR, LT_L1_CHIP_BUSY, LT_L1_DATA_LEN_ERROR, LT_L1_CHIP_STARTUP_MODE, LT_L1_CHIP_ALARM_MODE, LT_PARAM_ERR};

    for(unsigned int i=0; i<(sizeof(rets)/sizeof(rets[0])); i++) {
        lt_l2_transfer_ExpectAndReturn(&h, rets[i]);
        TEST_ASSERT_EQUAL(rets[i], lt_get_log(&h, msg, GET_LOG_MAX_MSG_LEN));
    }
}

//---------------------------------------------------------------------------------------------------------//
uint16_t size_inject_value;
lt_ret_t callback__lt_l2_transfer(lt_handle_t *h, int __attribute__((unused)) cmock_num_calls)
{
    struct lt_l2_get_info_rsp_t* p_l2_rsp = (struct lt_l2_get_info_rsp_t*)&h->l2_buff;
    p_l2_rsp->rsp_len = size_inject_value;

    return LT_OK;
}

// Test if function returns LT_OK if all went correctly
void test__correct()
{
    lt_handle_t h = {0};
    uint8_t msg[GET_LOG_MAX_MSG_LEN] = {0};

    size_inject_value = GET_LOG_MAX_MSG_LEN;
    lt_l2_transfer_StubWithCallback(callback__lt_l2_transfer);
    TEST_ASSERT_EQUAL(LT_OK, lt_get_log(&h, msg, GET_LOG_MAX_MSG_LEN));
}
