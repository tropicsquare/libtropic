/**
 * @file test_libtropic__lt_session_abort.c
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
void test__invalid_handle()
{
    uint8_t spect_fw_ver[LT_L2_GET_INFO_SPECT_FW_SIZE];
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_session_abort(NULL));
}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- EXECUTION ------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

// Test if function propagates l2 error if l2 transfer fails
void test__lt_l2_transfer_fail()
{
    lt_handle_t h = {0};
    h.session     = SESSION_ON;

    lt_ret_t rets[] = {LT_L1_SPI_ERROR, LT_L1_CHIP_BUSY, LT_L1_DATA_LEN_ERROR, LT_L1_CHIP_STARTUP_MODE, LT_L1_CHIP_ALARM_MODE, LT_PARAM_ERR};

    for(int i=0; i<(sizeof(rets)/sizeof(rets[0])); i++) {
        lt_l2_transfer_ExpectAndReturn(&h, rets[i]);
        TEST_ASSERT_EQUAL(rets[i], lt_session_abort(&h));
    }
}

//---------------------------------------------------------------------------------------------------------//

uint16_t inject_rsp_len;
lt_ret_t callback__lt_l2_transfer(lt_handle_t *h, int cmock_num_calls)
{
    struct lt_l2_encrypted_session_abt_rsp_t* p_l2_rsp = (struct lt_l2_encrypted_session_abt_rsp_t*)&h->l2_buff;
    p_l2_rsp->rsp_len = inject_rsp_len;

    return LT_OK;
}

// Test if function fails on size mismatch in l2 response
void test__resp_size_mismatch()
{
    lt_handle_t h = {0};
    h.session     = SESSION_ON;

    inject_rsp_len = 0+1;
    lt_l2_transfer_StubWithCallback(callback__lt_l2_transfer);
    TEST_ASSERT_EQUAL(LT_FAIL, lt_session_abort(&h));

}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_OK if all went correctly
void test__correct()
{
    lt_handle_t h = {0};
    h.session     = SESSION_ON;

    inject_rsp_len = 0;
    lt_l2_transfer_StubWithCallback(callback__lt_l2_transfer);
    TEST_ASSERT_EQUAL(LT_OK, lt_session_abort(&h));
}
