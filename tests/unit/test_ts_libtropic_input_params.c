/**
 * @file test_ts_libtropic_input_params.c
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


void setUp(void)
{
    char buffer[100];
    #ifdef RNG_SEED
        srand(RNG_SEED);
    #else
        time_t seed = time(NULL);
        // Using this approach, because in our version of Unity there's no TEST_PRINTF yet.
        // Also, raw printf is worse solution (without additional debug msgs, such as line).
        snprintf(buffer, sizeof(buffer), "Using random seed: %lu\n", seed);
        TEST_MESSAGE(buffer);
        srand(seed);
    #endif
}

void tearDown(void)
{
}

//---------------------------------------------------------------------
// Test if function returns LT_PARAM_ERR on non valid input parameter
void test_lt_init___invalid_handle()
{
    int ret = lt_init(NULL);
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, ret);
}

//---------------------------------------------------------------------
// Test if function returns LT_PARAM_ERR on non valid input parameter
void test_lt_deinit___invalid_handle()
{
    int ret = lt_deinit(NULL);
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, ret);
}

//---------------------------------------------------------------------
// Test if function returns LT_PARAM_ERR on non valid input parameter
void test_lt_handshake___invalid_handle()
{   uint8_t pkey_index  = 1;
    uint8_t shipriv[]   = {0x80,0x02,0xc5,0xa3,0xff,0x46,0xa2,0x09,0x4e,0x4e,0x71,0xf3,0xc8,0xe3,0xdd,0x79,0xec,0x5c,0x1c,0xcd,0xb0,0x40,0xbb,0xcf,0x6f,0x64,0x9d,0x49,0xe9,0x1d,0x9c,0x53};
    uint8_t shipub[]    = {0x83,0xc3,0x36,0x3c,0xff,0x27,0x47,0xb7,0xf7,0xeb,0x19,0x85,0x17,0x63,0x1a,0x71,0x54,0x76,0xb4,0xfe,0x22,0x46,0x01,0x45,0x89,0xc3,0xac,0x11,0x8b,0xb8,0x9e,0x51};
    uint8_t stpub[]     = {0};
    int ret = lt_handshake(NULL, stpub, pkey_index, shipriv, shipub);
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, ret);
}

// Test if function returns LT_PARAM_ERR on non valid input parameter
void test_lt_handshake___invalid_pkey_index()
{
    lt_handle_t handle  = {0};
    uint8_t pkey_index  = 5;
    uint8_t shipriv[]   = {0x80,0x02,0xc5,0xa3,0xff,0x46,0xa2,0x09,0x4e,0x4e,0x71,0xf3,0xc8,0xe3,0xdd,0x79,0xec,0x5c,0x1c,0xcd,0xb0,0x40,0xbb,0xcf,0x6f,0x64,0x9d,0x49,0xe9,0x1d,0x9c,0x53};
    uint8_t shipub[]    = {0x83,0xc3,0x36,0x3c,0xff,0x27,0x47,0xb7,0xf7,0xeb,0x19,0x85,0x17,0x63,0x1a,0x71,0x54,0x76,0xb4,0xfe,0x22,0x46,0x01,0x45,0x89,0xc3,0xac,0x11,0x8b,0xb8,0x9e,0x51};
    uint8_t stpub[]     = {0};

    int ret = lt_handshake(&handle, stpub, pkey_index, shipriv, shipub);
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, ret);
}

// Test if function returns LT_PARAM_ERR on non valid input parameter
void test_lt_handshake___invalid_shipriv()
{
    lt_handle_t handle  = {0};
    uint8_t pkey_index  = 5;
    uint8_t shipub[]    = {0x83,0xc3,0x36,0x3c,0xff,0x27,0x47,0xb7,0xf7,0xeb,0x19,0x85,0x17,0x63,0x1a,0x71,0x54,0x76,0xb4,0xfe,0x22,0x46,0x01,0x45,0x89,0xc3,0xac,0x11,0x8b,0xb8,0x9e,0x51};
    uint8_t stpub[]     = {0};

    int ret = lt_handshake(&handle, stpub, pkey_index, NULL, shipub);
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, ret);
}

// Test if function returns LT_PARAM_ERR on non valid input parameter
void test_lt_handshake___invalid_shipub()
{
    lt_handle_t handle  = {0};
    uint8_t pkey_index  = 5;
    uint8_t shipriv[]   = {0x80,0x02,0xc5,0xa3,0xff,0x46,0xa2,0x09,0x4e,0x4e,0x71,0xf3,0xc8,0xe3,0xdd,0x79,0xec,0x5c,0x1c,0xcd,0xb0,0x40,0xbb,0xcf,0x6f,0x64,0x9d,0x49,0xe9,0x1d,0x9c,0x53};
    uint8_t stpub[]     = {0};

    int ret = lt_handshake(&handle, stpub, pkey_index, shipriv, NULL);
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, ret);
}

//---------------------------------------------------------------------
// void test_lt_ping__invalid_handle()
// {
//     uint8_t msg_out, msg_in;
//     TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ping(NULL, &msg_out, &msg_in, 0));
// }

void test_lt_ping__invalid_msg_out()
{
    lt_handle_t h;
    h.session = SESSION_ON;
    uint8_t msg_in;
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ping(&h, NULL, &msg_in, 0));
}

void test_lt_ping__invalid_msg_in()
{
    lt_handle_t h;
    h.session = SESSION_ON;
    uint8_t msg_out;
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ping(&h, &msg_out, NULL, 0));
}

void test_lt_ping__invalid_len()
{
    lt_handle_t h;
    h.session = SESSION_ON;

    uint8_t msg_out, msg_in;
    uint16_t len;

    for (int i = 0; i < 25; i++) {
        len = PING_LEN_MAX + 1;
        len += rand() % (UINT16_MAX - len);
        TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ping(&h, &msg_out, &msg_in, len));
    }
}
//---------------------------------------------------------------------
// void test_lt_random_get__invalid_handle()
// {
//     TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_random_get(NULL, (uint8_t*)"", 0));
// }

void test_lt_random_get__invalid_buff()
{
    lt_handle_t h;
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_random_get(&h, NULL, 0));
}

void test_lt_random_get__invalid_len()
{
    lt_handle_t h;
    uint16_t len;

    h.session = SESSION_ON;

    for (int i = 0; i < 25; i++) {
        len = RANDOM_VALUE_GET_LEN_MAX + 1;
        len += rand() % (UINT16_MAX - len);
        TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_random_get(&h, (uint8_t*)"", len));
    }
}

//---------------------------------------------------------------------
// void test_lt_ecc_key_generate__invalid_handle()
// {
//     TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_generate(NULL, ECC_SLOT_1, CURVE_ED25519));
//     TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_generate(NULL, ECC_SLOT_1, CURVE_P256));
// }

// void test_lt_ecc_key_generate__invalid_slot()
// {
//     lt_handle_t h;
//     h.session = SESSION_ON;

//     TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_generate(&h, LT_L3_ECC_KEY_GENERATE_SLOT_MIN - 1, CURVE_ED25519));
//     TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_generate(&h, LT_L3_ECC_KEY_GENERATE_SLOT_MAX + 1, CURVE_ED25519));
// }

void test_lt_ecc_key_generate__invalid_curve()
{
    lt_handle_t h;
    h.session = SESSION_ON;

    // Test random values.
    int curve;
    for (int i = 0; i < 25; i++) {
        curve = rand();
        if (curve != CURVE_ED25519 && curve != CURVE_P256) {
            TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_generate(&h, ECC_SLOT_1, curve));
        }
    }

    // Test top and lower bound.
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_generate(&h, ECC_SLOT_1, 0));
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_generate(&h, ECC_SLOT_1, 3));
}

//---------------------------------------------------------------------
// void test_lt_ecc_key_read__invalid_handle()
// {
//     ecc_curve_type_t curve;
//     ecc_key_origin_t origin;
//     uint8_t key[64];

//     TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_read(NULL, ECC_SLOT_1, key, 64, &curve, &origin));
// }

// void test_lt_ecc_key_read__invalid_slot()
// {
//     lt_handle_t h;
//     h.session = SESSION_ON;

//     ecc_curve_type_t curve;
//     ecc_key_origin_t origin;
//     uint8_t key[64];

//     TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_read(&h, LT_L3_ECC_KEY_GENERATE_SLOT_MIN - 1, key, 64, &curve, &origin));
//     TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_read(&h, LT_L3_ECC_KEY_GENERATE_SLOT_MAX + 1, key, 64, &curve, &origin));
// }

void test_lt_ecc_key_read__invalid_key()
{
    lt_handle_t h;
    h.session = SESSION_ON;

    ecc_curve_type_t curve;
    ecc_key_origin_t origin;

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_read(&h, ECC_SLOT_1, NULL, 64, &curve, &origin));
}

void test_lt_ecc_key_read__invalid_key_len()
{
    lt_handle_t h;
    h.session = SESSION_ON;

    ecc_curve_type_t curve;
    ecc_key_origin_t origin;
    uint8_t key[64];

    for (int i = 0; i < 25; i++) {
        TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_read(&h, ECC_SLOT_1, key, rand() % 64, &curve, &origin));
    }
}

void test_lt_ecc_key_read__invalid_curve()
{
    lt_handle_t h;
    h.session = SESSION_ON;

    ecc_key_origin_t origin;
    uint8_t key[64];

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_read(&h, ECC_SLOT_1, key, 64, NULL, &origin));
}

void test_lt_ecc_key_read__invalid_origin()
{
    lt_handle_t h;
    h.session = SESSION_ON;

    ecc_curve_type_t curve;
    uint8_t key[64];

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_read(&h, ECC_SLOT_1, key, 64, &curve, NULL));
}

//---------------------------------------------------------------------
// void test_lt_ecc_eddsa_sign__invalid_handle()
// {
//     uint8_t msg[1];
//     int8_t  rs[64];

//     TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_eddsa_sign(NULL, ECC_SLOT_1, msg, 1, rs, 64));
// }

// void test_lt_ecc_eddsa_sign__invalid_slot()
// {
//     lt_handle_t h;
//     h.session = SESSION_ON;

//     uint8_t msg[1];
//     uint8_t rs[64];

//     TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_eddsa_sign(&h, LT_L3_ECC_KEY_GENERATE_SLOT_MIN - 1, msg, 1, rs, 64));
//     TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_eddsa_sign(&h, LT_L3_ECC_KEY_GENERATE_SLOT_MAX + 1, msg, 1, rs, 64));
// }

void test_lt_ecc_eddsa_sign__invalid_msg()
{
    lt_handle_t h;
    h.session = SESSION_ON;

    uint8_t rs[64];

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_eddsa_sign(&h, ECC_SLOT_1, NULL, 1, rs, 64));
}

void test_lt_ecc_eddsa_sign__invalid_msg_len()
{
    lt_handle_t h;
    h.session = SESSION_ON;

    uint8_t msg[LT_L3_EDDSA_SIGN_MSG_LEN_MAX + 1];
    uint8_t rs[64];

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_eddsa_sign(&h, ECC_SLOT_1, msg, LT_L3_EDDSA_SIGN_MSG_LEN_MAX + 1, rs, 64));
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_eddsa_sign(&h, ECC_SLOT_1, msg, LT_L3_EDDSA_SIGN_MSG_LEN_MIN - 1, rs, 64));
}

void test_lt_ecc_eddsa_sign__invalid_rs()
{
    lt_handle_t h;
    h.session = SESSION_ON;

    uint8_t msg[1];

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_eddsa_sign(&h, ECC_SLOT_1, msg, 1, NULL, 64));
}

void test_lt_ecc_eddsa_sign__invalid_rs_len()
{
    lt_handle_t h;
    h.session = SESSION_ON;

    uint8_t msg[1];
    uint8_t rs[64];

    for (int i = 0; i < 25; i++) {
        TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_eddsa_sign(&h, ECC_SLOT_1, msg, 1, rs, rand() % 64));
    }
}

//---------------------------------------------------------------------
// void test_lt_ecc_ecdsa_sign__invalid_handle()
// {
//     uint8_t msg[1];
//     int8_t  rs[64];

//     TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_ecdsa_sign(NULL, ECC_SLOT_1, msg, 1, rs, 64));
// }

// void test_lt_ecc_ecdsa_sign__invalid_slot()
// {
//     lt_handle_t h;
//     h.session = SESSION_ON;

//     uint8_t msg[1];
//     uint8_t rs[64];

//     TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_ecdsa_sign(&h, LT_L3_ECC_KEY_GENERATE_SLOT_MIN - 1, msg, 1, rs, 64));
//     TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_ecdsa_sign(&h, LT_L3_ECC_KEY_GENERATE_SLOT_MAX + 1, msg, 1, rs, 64));
// }

void test_lt_ecc_ecdsa_sign__invalid_msg()
{
    lt_handle_t h;
    h.session = SESSION_ON;

    uint8_t rs[64];

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_ecdsa_sign(&h, ECC_SLOT_1, NULL, 1, rs, 64));
}

void test_lt_ecc_ecdsa_sign__invalid_msg_len()
{
    lt_handle_t h;
    h.session = SESSION_ON;

    uint8_t msg[LT_L3_ECDSA_SIGN_MSG_LEN_MAX + 1];
    uint8_t rs[64];

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_ecdsa_sign(&h, ECC_SLOT_1, msg, LT_L3_ECDSA_SIGN_MSG_LEN_MAX + 1, rs, 64));
}

void test_lt_ecc_ecdsa_sign__invalid_rs()
{
    lt_handle_t h;
    h.session = SESSION_ON;

    uint8_t msg[1];

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_ecdsa_sign(&h, ECC_SLOT_1, msg, 1, NULL, 64));
}

void test_lt_ecc_ecdsa_sign__invalid_rs_len()
{
    lt_handle_t h;
    h.session = SESSION_ON;

    uint8_t msg[1];
    uint8_t rs[64];

    for (int i = 0; i < 25; i++) {
        TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_ecdsa_sign(&h, ECC_SLOT_1, msg, 1, rs, rand() % 64));
    }
}

//---------------------------------------------------------------------
// void test_lt_ecc_key_erase__invalid_handle()
// {
//     TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_erase(NULL, ECC_SLOT_1));
// }

void test_lt_ecc_key_erase__invalid_slot()
{
    lt_handle_t h;
    h.session = SESSION_ON;

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_erase(&h, LT_L3_ECC_KEY_GENERATE_SLOT_MIN - 1));
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_ecc_key_erase(&h, LT_L3_ECC_KEY_GENERATE_SLOT_MAX + 1));
}

//---------------------------------------------------------------------
// void test_lt_get_info_cert__invalid_handle()
// {
//     uint8_t cert[LT_L2_GET_INFO_REQ_CERT_SIZE];
//     TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_get_info_cert(NULL, cert, LT_L2_GET_INFO_REQ_CERT_SIZE));
// }

// void test_lt_get_info_cert__invalid_cert()
// {
//     lt_handle_t h;
//     h.session = SESSION_ON;

//     TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_get_info_cert(&h, NULL, LT_L2_GET_INFO_REQ_CERT_SIZE));
// }

// void test_lt_get_info_cert__invalid_cert_len()
// {
//     lt_handle_t h;
//     uint8_t     cert[LT_L2_GET_INFO_REQ_CERT_SIZE];

//     h.session = SESSION_ON;

//     for (int i = 0; i < 25; i++) {
//         TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_get_info_cert(&h, cert, rand() % LT_L2_GET_INFO_REQ_CERT_SIZE));
//     }
// }
//---------------------------------------------------------------------
void test_lt_cert_verify_and_parse__invalid_cert()
{
    uint8_t     stpub[] = {0};

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_cert_verify_and_parse(NULL, LT_L2_GET_INFO_REQ_CERT_SIZE, stpub));
}

void test_lt_cert_verify_and_parse__invalid_len()
{
    uint8_t dummy_cert[1024];
    uint8_t stpub[] = {0};

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_cert_verify_and_parse(dummy_cert, LT_L2_GET_INFO_REQ_CERT_SIZE + 1, stpub));
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_cert_verify_and_parse(dummy_cert, LT_L2_GET_INFO_REQ_CERT_SIZE + 500, stpub));
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_cert_verify_and_parse(dummy_cert, LT_L2_GET_INFO_REQ_CERT_SIZE + 1000, stpub));
}

void test_lt_cert_verify_and_parse__invalid_stpub()
{
    uint8_t dummy_cert[LT_L2_GET_INFO_REQ_CERT_SIZE];

    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_cert_verify_and_parse(dummy_cert, LT_L2_GET_INFO_REQ_CERT_SIZE, NULL));
}

//---------------------------------------------------------------------

// Test that all return values can be displayed as a string (for better debug)
void test_lt_ret_verbose()
{
    TEST_ASSERT_EQUAL_STRING("LT_OK", lt_ret_verbose(LT_OK));
    TEST_ASSERT_EQUAL_STRING("LT_FAIL", lt_ret_verbose(LT_FAIL));
    TEST_ASSERT_EQUAL_STRING("LT_PARAM_ERR", lt_ret_verbose(LT_PARAM_ERR));
    TEST_ASSERT_EQUAL_STRING("LT_L1_SPI_ERROR", lt_ret_verbose(LT_L1_SPI_ERROR));
    TEST_ASSERT_EQUAL_STRING("LT_L1_DATA_LEN_ERROR", lt_ret_verbose(LT_L1_DATA_LEN_ERROR));
    TEST_ASSERT_EQUAL_STRING("LT_L1_CHIP_STARTUP_MODE", lt_ret_verbose(LT_L1_CHIP_STARTUP_MODE));
    TEST_ASSERT_EQUAL_STRING("LT_L1_CHIP_ALARM_MODE", lt_ret_verbose(LT_L1_CHIP_ALARM_MODE));
    TEST_ASSERT_EQUAL_STRING("LT_L1_CHIP_BUSY", lt_ret_verbose(LT_L1_CHIP_BUSY));
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
    TEST_ASSERT_EQUAL_STRING("LT_L3_FAIL", lt_ret_verbose(LT_L3_FAIL));
    TEST_ASSERT_EQUAL_STRING("LT_L3_OK", lt_ret_verbose(LT_L3_OK));
    TEST_ASSERT_EQUAL_STRING("LT_L3_UNAUTHORIZED", lt_ret_verbose(LT_L3_UNAUTHORIZED));
    TEST_ASSERT_EQUAL_STRING("LT_L3_INVALID_CMD", lt_ret_verbose(LT_L3_INVALID_CMD));
    TEST_ASSERT_EQUAL_STRING("LT_L3_DATA_LEN_ERROR", lt_ret_verbose(LT_L3_DATA_LEN_ERROR));
    TEST_ASSERT_EQUAL_STRING("LT_HOST_NO_SESSION", lt_ret_verbose(LT_HOST_NO_SESSION));
    TEST_ASSERT_EQUAL_STRING("FATAL ERROR, unknown return value", lt_ret_verbose(99));
}