
#include "unity.h"
#include "string.h"

#include "libtropic_common.h"
#include "libtropic.h"

#include "mock_lt_random.h"
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
}

void tearDown(void)
{
}

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


// Test if function returns LT_PARAM_ERR on non valid input parameter
void test_lt_init___invalid_handle()
{
    int ret = lt_init(NULL);
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, ret);
}

// Test if function returns LT_FAIL when l1 init failed
void test_lt_init___error_during_lt_l1_init()
{
    lt_handle_t h = {0};

    lt_l1_init_ExpectAndReturn(&h, LT_FAIL);

    lt_ret_t ret = lt_init(&h);
    TEST_ASSERT_EQUAL(LT_FAIL, ret);
}

// Test if function returns LT_OK when all went correctly
void test_lt_init___correct()
{
    lt_handle_t h = {0};

    lt_l1_init_ExpectAndReturn(&h, LT_OK);

    lt_ret_t ret = lt_init(&h);
    TEST_ASSERT_EQUAL(LT_OK, ret);
}

//---------------------------------------------------------------------
// Test if function returns LT_PARAM_ERR on non valid input parameter
void test_lt_deinit___invalid_handle()
{
    int ret = lt_deinit(NULL);
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, ret);
}

// Test if function returns LT_FAIL when l1 deinit failed
void test_lt_deinit___error_during_lt_l1_deinit()
{
    lt_handle_t h = {0};

    lt_l1_deinit_ExpectAndReturn(&h, LT_FAIL);

    lt_ret_t ret = lt_deinit(&h);
    TEST_ASSERT_EQUAL(LT_FAIL, ret);
}

// Test if function returns LT_OK when all went correctly
void test_lt_deinit___correct()
{
    lt_handle_t h = {0};

    lt_l1_deinit_ExpectAndReturn(&h, LT_OK);

    lt_ret_t ret = lt_deinit(&h);
    TEST_ASSERT_EQUAL(LT_OK, ret);
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
