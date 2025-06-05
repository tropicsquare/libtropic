/**
 * @file test_libtropic__lt_get_info.c
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
#include "mock_lt_l3_transfer.h"
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
    uint8_t cert[LT_L2_GET_INFO_REQ_CERT_SIZE];
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_get_info_cert(NULL, cert, LT_L2_GET_INFO_REQ_CERT_SIZE));
}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_PARAM_ERROR when invalid cert is passed
void test__invalid_cert()
{
    lt_handle_t h = {0};

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_get_info_cert(&h, NULL, LT_L2_GET_INFO_REQ_CERT_SIZE));
}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_PARAM_ERROR when invalid max_len is passed
void test__invalid_max_len()
{
    lt_handle_t h = {0};
    uint8_t cert[LT_L2_GET_INFO_REQ_CERT_SIZE];

    for (int i = 0; i < 25; i++) {
        TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_get_info_cert(&h, cert, rand() % LT_L2_GET_INFO_REQ_CERT_SIZE));
    }
}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- EXECUTION ------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

lt_ret_t callback_1__lt_l2_transfer(lt_handle_t *h, int __attribute__((unused)) cmock_num_calls)
{
    struct lt_l2_get_info_rsp_t* p_l2_rsp = (struct lt_l2_get_info_rsp_t*)&h->l2_buff;

    p_l2_rsp->rsp_len = 128;

    switch(cmock_num_calls) {
        case 0:
            return LT_L1_SPI_ERROR;

        case 1:
            return LT_OK;
        case 2:
            return LT_L1_SPI_ERROR;

        case 3:
            return LT_OK;
        case 4:
            return LT_OK;
        case 5:
            return LT_L1_SPI_ERROR;

        case 6:
            return LT_OK;
        case 7:
            return LT_OK;
        case 8:
            return LT_OK;
        case 9:
            return LT_L1_SPI_ERROR;

        default:
            return 100;
    }
}

// Test if function propagates l2 error if l2 transfer fails
void test__l2_fail()
{
    lt_handle_t h = {0};
    h.session     = SESSION_ON;
    uint8_t cert[LT_L2_GET_INFO_REQ_CERT_SIZE];

    lt_ret_t rets[] = {LT_L1_SPI_ERROR, LT_L1_CHIP_BUSY, LT_L1_DATA_LEN_ERROR, LT_L1_CHIP_STARTUP_MODE, LT_L1_CHIP_ALARM_MODE, LT_PARAM_ERR};
    for(unsigned int i=0; i<(sizeof(rets)/sizeof(rets[0])); i++) {
        lt_l2_send_ExpectAndReturn(&h, LT_OK);
        lt_l2_receive_ExpectAndReturn(&h, rets[i]);
        TEST_ASSERT_EQUAL(rets[i], lt_get_info_cert(&h, cert, sizeof(cert)));
    }

    lt_l2_send_ExpectAndReturn(&h, LT_OK);
    lt_l2_receive_StubWithCallback(callback_1__lt_l2_transfer);
    TEST_ASSERT_EQUAL(LT_L1_SPI_ERROR, lt_get_info_cert(&h, cert, sizeof(cert)));

    lt_l2_send_ExpectAndReturn(&h, LT_OK);
    lt_l2_receive_StubWithCallback(callback_1__lt_l2_transfer);
    lt_l2_send_ExpectAndReturn(&h, LT_OK);
    lt_l2_receive_StubWithCallback(callback_1__lt_l2_transfer);
    TEST_ASSERT_EQUAL(LT_L1_SPI_ERROR, lt_get_info_cert(&h, cert, sizeof(cert)));

    lt_l2_send_ExpectAndReturn(&h, LT_OK);
    lt_l2_receive_StubWithCallback(callback_1__lt_l2_transfer);
    lt_l2_send_ExpectAndReturn(&h, LT_OK);
    lt_l2_receive_StubWithCallback(callback_1__lt_l2_transfer);
    lt_l2_send_ExpectAndReturn(&h, LT_OK);
    lt_l2_receive_StubWithCallback(callback_1__lt_l2_transfer);
    TEST_ASSERT_EQUAL(LT_L1_SPI_ERROR, lt_get_info_cert(&h, cert, sizeof(cert)));

    lt_l2_send_ExpectAndReturn(&h, LT_OK);
    lt_l2_receive_StubWithCallback(callback_1__lt_l2_transfer);
    lt_l2_send_ExpectAndReturn(&h, LT_OK);
    lt_l2_receive_StubWithCallback(callback_1__lt_l2_transfer);
    lt_l2_send_ExpectAndReturn(&h, LT_OK);
    lt_l2_receive_StubWithCallback(callback_1__lt_l2_transfer);
    lt_l2_send_ExpectAndReturn(&h, LT_OK);
    lt_l2_receive_StubWithCallback(callback_1__lt_l2_transfer);
    TEST_ASSERT_EQUAL(LT_L1_SPI_ERROR, lt_get_info_cert(&h, cert, sizeof(cert)));
}

//---------------------------------------------------------------------------------------------------------//

// Test if function fails on size mismatch in l2 response
lt_ret_t callback_2__lt_l2_transfer(lt_handle_t *h, int __attribute__((unused)) cmock_num_calls)
{
    struct lt_l2_get_info_rsp_t* p_l2_rsp = (struct lt_l2_get_info_rsp_t*)&h->l2_buff;
    p_l2_rsp->rsp_len = (LT_L2_GET_INFO_REQ_CERT_SIZE/30) + 1;

    return LT_OK;
}

void test__resp_size_mismatch()
{
    lt_handle_t h = {0};
    h.session = SESSION_ON;
    uint8_t cert[LT_L2_GET_INFO_REQ_CERT_SIZE];

    lt_l2_send_ExpectAndReturn(&h, LT_OK);
    lt_l2_receive_StubWithCallback(callback_2__lt_l2_transfer);
    TEST_ASSERT_EQUAL(LT_FAIL, lt_get_info_cert(&h, cert, sizeof(cert)));

}

//---------------------------------------------------------------------------------------------------------//

lt_ret_t callback_3__lt_l2_transfer(lt_handle_t *h, int cmock_num_calls)
{
    struct lt_l2_get_info_rsp_t* p_l2_rsp = (struct lt_l2_get_info_rsp_t*)&h->l2_buff;
    p_l2_rsp->rsp_len = 128;

    switch(cmock_num_calls) {
        case 0:
            return LT_OK;
        case 1:
            return LT_OK;
        case 2:
            return LT_OK;
        case 3:
            return LT_OK;
        case 4:
            return LT_OK;
        case 5:
            return LT_OK;
        case 6:
            return LT_OK;
        case 7:
            return LT_OK;
        case 8:
            return LT_OK;
        case 9:
            return LT_OK;
        case 10:
            return LT_OK;
        case 11:
            return LT_OK;
        case 12:
            return LT_OK;
        case 13:
            return LT_OK;
        case 14:
            return LT_OK;
        case 15:
            return LT_OK;
        case 16:
            return LT_OK;
        case 17:
            return LT_OK;
        case 18:
            return LT_OK;
        case 19:
            return LT_OK;
        case 20:
            return LT_OK;
        case 21:
            return LT_OK;
        case 22:
            return LT_OK;
        case 23:
            return LT_OK;
        case 24:
            return LT_OK;
        case 25:
            return LT_OK;
        case 26:
            return LT_OK;
        case 27:
            return LT_OK;
        case 28:
            return LT_OK;
        case 29:
            return LT_OK;
        default:
            return 100;
    }
}

// Test if function returns LT_OK if all went correctly
void test__l2_correct()
{
    lt_handle_t h = {0};
    h.session = SESSION_ON;
    uint8_t cert[LT_L2_GET_INFO_REQ_CERT_SIZE];

    for (int i = 0; i < (LT_L2_GET_INFO_REQ_CERT_SIZE/128); i++) {
        lt_l2_send_ExpectAndReturn(&h, LT_OK);
        lt_l2_receive_StubWithCallback(callback_3__lt_l2_transfer);
    }
    TEST_ASSERT_EQUAL(LT_OK, lt_get_info_cert(&h, cert, sizeof(cert)));
}
