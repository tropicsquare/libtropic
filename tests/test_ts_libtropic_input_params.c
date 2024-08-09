
#include "unity.h"
#include "string.h"

#include "libtropic_common.h"
#include "libtropic.h"

#include "mock_ts_l1.h"
#include "mock_ts_l2.h"
#include "mock_ts_l3.h"
#include "mock_ts_x25519.h"
#include "mock_ts_ed25519.h"
#include "mock_ts_hkdf.h"
#include "mock_ts_sha256.h"
#include "mock_ts_aesgcm.h"
#include "mock_libtropic_platform.h"



void setUp(void)
{
}

void tearDown(void)
{
}


void test_ts_ret_verbose()
{
    TEST_ASSERT_EQUAL_STRING("TS_OK", ts_ret_verbose(TS_OK));
    TEST_ASSERT_EQUAL_STRING("TS_FAIL", ts_ret_verbose(TS_FAIL));
    TEST_ASSERT_EQUAL_STRING("TS_PARAM_ERR", ts_ret_verbose(TS_PARAM_ERR));
    TEST_ASSERT_EQUAL_STRING("TS_L1_SPI_ERROR", ts_ret_verbose(TS_L1_SPI_ERROR));
    TEST_ASSERT_EQUAL_STRING("TS_L1_DATA_LEN_ERROR", ts_ret_verbose(TS_L1_DATA_LEN_ERROR));
    TEST_ASSERT_EQUAL_STRING("TS_L1_CHIP_STARTUP_MODE", ts_ret_verbose(TS_L1_CHIP_STARTUP_MODE));
    TEST_ASSERT_EQUAL_STRING("TS_L1_CHIP_ALARM_MODE", ts_ret_verbose(TS_L1_CHIP_ALARM_MODE));
    TEST_ASSERT_EQUAL_STRING("TS_L1_CHIP_BUSY", ts_ret_verbose(TS_L1_CHIP_BUSY));
    TEST_ASSERT_EQUAL_STRING("TS_L2_IN_CRC_ERR", ts_ret_verbose(TS_L2_IN_CRC_ERR));
    TEST_ASSERT_EQUAL_STRING("TS_L2_REQ_CONT", ts_ret_verbose(TS_L2_REQ_CONT));
    TEST_ASSERT_EQUAL_STRING("TS_L2_RES_CONT", ts_ret_verbose(TS_L2_RES_CONT));
    TEST_ASSERT_EQUAL_STRING("TS_L2_HSK_ERR", ts_ret_verbose(TS_L2_HSK_ERR));
    TEST_ASSERT_EQUAL_STRING("TS_L2_NO_SESSION", ts_ret_verbose(TS_L2_NO_SESSION));
    TEST_ASSERT_EQUAL_STRING("TS_L2_TAG_ERR", ts_ret_verbose(TS_L2_TAG_ERR));
    TEST_ASSERT_EQUAL_STRING("TS_L2_CRC_ERR", ts_ret_verbose(TS_L2_CRC_ERR));
    TEST_ASSERT_EQUAL_STRING("TS_L2_GEN_ERR", ts_ret_verbose(TS_L2_GEN_ERR));
    TEST_ASSERT_EQUAL_STRING("TS_L2_NO_RESP", ts_ret_verbose(TS_L2_NO_RESP));
    TEST_ASSERT_EQUAL_STRING("TS_L2_UNKNOWN_REQ", ts_ret_verbose(TS_L2_UNKNOWN_REQ));
    TEST_ASSERT_EQUAL_STRING("TS_L3_FAIL", ts_ret_verbose(TS_L3_FAIL));
    TEST_ASSERT_EQUAL_STRING("TS_L3_OK", ts_ret_verbose(TS_L3_OK));
    TEST_ASSERT_EQUAL_STRING("TS_L3_UNAUTHORIZED", ts_ret_verbose(TS_L3_UNAUTHORIZED));
    TEST_ASSERT_EQUAL_STRING("TS_L3_INVALID_CMD", ts_ret_verbose(TS_L3_INVALID_CMD));
    TEST_ASSERT_EQUAL_STRING("TS_HOST_NO_SESSION", ts_ret_verbose(TS_HOST_NO_SESSION));
    TEST_ASSERT_EQUAL_STRING("UNKNOWN return value", ts_ret_verbose(99));
}


// ts_init()
void test_ts_init___invalid_handle()
{
    int ret = ts_init(NULL);
    TEST_ASSERT_EQUAL(TS_PARAM_ERR, ret);
}

void test_ts_init___error_during_ts_l1_init()
{
    ts_handle_t h = {0};

    ts_l1_init_ExpectAndReturn(&h, TS_FAIL);

    ts_ret_t ret = ts_init(&h);
    TEST_ASSERT_EQUAL(TS_FAIL, ret);
}

void test_ts_init___correct()
{
    ts_handle_t h = {0};

    ts_l1_init_ExpectAndReturn(&h, TS_OK);

    ts_ret_t ret = ts_init(&h);
    TEST_ASSERT_EQUAL(TS_OK, ret);
}

// ts_deinit()

void test_ts_deinit___invalid_handle()
{
    int ret = ts_deinit(NULL);
    TEST_ASSERT_EQUAL(TS_PARAM_ERR, ret);
}

void test_ts_deinit___error_during_ts_l1_deinit()
{
    ts_handle_t h = {0};

    ts_l1_deinit_ExpectAndReturn(&h, TS_FAIL);

    ts_ret_t ret = ts_deinit(&h);
    TEST_ASSERT_EQUAL(TS_FAIL, ret);
}

void test_ts_deinit___correct()
{
    ts_handle_t h = {0};

    ts_l1_deinit_ExpectAndReturn(&h, TS_OK);

    ts_ret_t ret = ts_deinit(&h);
    TEST_ASSERT_EQUAL(TS_OK, ret);
}


// ts_handshake()

void test_ts_handshake___invalid_handle()
{   uint8_t pkey_index  = 1;
    uint8_t shipriv[]   = {0x80,0x02,0xc5,0xa3,0xff,0x46,0xa2,0x09,0x4e,0x4e,0x71,0xf3,0xc8,0xe3,0xdd,0x79,0xec,0x5c,0x1c,0xcd,0xb0,0x40,0xbb,0xcf,0x6f,0x64,0x9d,0x49,0xe9,0x1d,0x9c,0x53};
    uint8_t shipub[]    = {0x83,0xc3,0x36,0x3c,0xff,0x27,0x47,0xb7,0xf7,0xeb,0x19,0x85,0x17,0x63,0x1a,0x71,0x54,0x76,0xb4,0xfe,0x22,0x46,0x01,0x45,0x89,0xc3,0xac,0x11,0x8b,0xb8,0x9e,0x51};
    uint8_t stpub[]     = {0};
    int ret = ts_handshake(NULL, stpub, pkey_index, shipriv, shipub);
    TEST_ASSERT_EQUAL(TS_PARAM_ERR, ret);
}

void test_ts_handshake___invalid_pkey_index()
{
    ts_handle_t handle  = {0};
    uint8_t pkey_index  = 5;
    uint8_t shipriv[]   = {0x80,0x02,0xc5,0xa3,0xff,0x46,0xa2,0x09,0x4e,0x4e,0x71,0xf3,0xc8,0xe3,0xdd,0x79,0xec,0x5c,0x1c,0xcd,0xb0,0x40,0xbb,0xcf,0x6f,0x64,0x9d,0x49,0xe9,0x1d,0x9c,0x53};
    uint8_t shipub[]    = {0x83,0xc3,0x36,0x3c,0xff,0x27,0x47,0xb7,0xf7,0xeb,0x19,0x85,0x17,0x63,0x1a,0x71,0x54,0x76,0xb4,0xfe,0x22,0x46,0x01,0x45,0x89,0xc3,0xac,0x11,0x8b,0xb8,0x9e,0x51};
    uint8_t stpub[]     = {0};

    int ret = ts_handshake(&handle, stpub, pkey_index, shipriv, shipub);
    TEST_ASSERT_EQUAL(TS_PARAM_ERR, ret);
}

void test_ts_handshake___invalid_shipriv()
{
    ts_handle_t handle  = {0};
    uint8_t pkey_index  = 5;
    uint8_t shipub[]    = {0x83,0xc3,0x36,0x3c,0xff,0x27,0x47,0xb7,0xf7,0xeb,0x19,0x85,0x17,0x63,0x1a,0x71,0x54,0x76,0xb4,0xfe,0x22,0x46,0x01,0x45,0x89,0xc3,0xac,0x11,0x8b,0xb8,0x9e,0x51};
    uint8_t stpub[]     = {0};

    int ret = ts_handshake(&handle, stpub, pkey_index, NULL, shipub);
    TEST_ASSERT_EQUAL(TS_PARAM_ERR, ret);
}

void test_ts_handshake___invalid_shipub()
{
    ts_handle_t handle  = {0};
    uint8_t pkey_index  = 5;
    uint8_t shipriv[]   = {0x80,0x02,0xc5,0xa3,0xff,0x46,0xa2,0x09,0x4e,0x4e,0x71,0xf3,0xc8,0xe3,0xdd,0x79,0xec,0x5c,0x1c,0xcd,0xb0,0x40,0xbb,0xcf,0x6f,0x64,0x9d,0x49,0xe9,0x1d,0x9c,0x53};
    uint8_t stpub[]     = {0};

    int ret = ts_handshake(&handle, stpub, pkey_index, shipriv, NULL);
    TEST_ASSERT_EQUAL(TS_PARAM_ERR, ret);
}
