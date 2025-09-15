/**
 * @file test_libtropic__lt_session_start.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic.h"
#include "libtropic_common.h"
#include "lt_l2_api_structs.h"
#include "mock_lt_aesgcm.h"
#include "mock_lt_asn1_der.h"
#include "mock_lt_ed25519.h"
#include "mock_lt_hkdf.h"
#include "mock_lt_l1.h"
#include "mock_lt_l1_port_wrap.h"
#include "mock_lt_l2.h"
#include "mock_lt_l3.h"
#include "mock_lt_l3_process.h"
#include "mock_lt_random.h"
#include "mock_lt_sha256.h"
#include "mock_lt_x25519.h"
#include "string.h"
#include "time.h"
#include "unity.h"

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

void tearDown(void) {}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- INPUT PARAMETERS   ---------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_PARAM_ERR on non valid handle
void test__invalid_handle()
{
    uint8_t pkey_index = 1;
    uint8_t shipriv[]
        = {0x80, 0x02, 0xc5, 0xa3, 0xff, 0x46, 0xa2, 0x09, 0x4e, 0x4e, 0x71, 0xf3, 0xc8, 0xe3, 0xdd, 0x79,
           0xec, 0x5c, 0x1c, 0xcd, 0xb0, 0x40, 0xbb, 0xcf, 0x6f, 0x64, 0x9d, 0x49, 0xe9, 0x1d, 0x9c, 0x53};
    uint8_t shipub[] = {0x83, 0xc3, 0x36, 0x3c, 0xff, 0x27, 0x47, 0xb7, 0xf7, 0xeb, 0x19, 0x85, 0x17, 0x63, 0x1a, 0x71,
                        0x54, 0x76, 0xb4, 0xfe, 0x22, 0x46, 0x01, 0x45, 0x89, 0xc3, 0xac, 0x11, 0x8b, 0xb8, 0x9e, 0x51};
    uint8_t stpub[] = {0};
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_session_start(NULL, stpub, pkey_index, shipriv, shipub));
}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_PARAM_ERR on non valid pkey_index
void test__invalid_pkey_index()
{
    lt_handle_t handle = {0};
    uint8_t pkey_index = 5;
    uint8_t shipriv[]
        = {0x80, 0x02, 0xc5, 0xa3, 0xff, 0x46, 0xa2, 0x09, 0x4e, 0x4e, 0x71, 0xf3, 0xc8, 0xe3, 0xdd, 0x79,
           0xec, 0x5c, 0x1c, 0xcd, 0xb0, 0x40, 0xbb, 0xcf, 0x6f, 0x64, 0x9d, 0x49, 0xe9, 0x1d, 0x9c, 0x53};
    uint8_t shipub[] = {0x83, 0xc3, 0x36, 0x3c, 0xff, 0x27, 0x47, 0xb7, 0xf7, 0xeb, 0x19, 0x85, 0x17, 0x63, 0x1a, 0x71,
                        0x54, 0x76, 0xb4, 0xfe, 0x22, 0x46, 0x01, 0x45, 0x89, 0xc3, 0xac, 0x11, 0x8b, 0xb8, 0x9e, 0x51};
    uint8_t stpub[] = {0};
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_session_start(&handle, stpub, pkey_index, shipriv, shipub));
}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_PARAM_ERR on non valid shipriv
void test__invalid_shipriv()
{
    lt_handle_t handle = {0};
    uint8_t pkey_index = 5;
    uint8_t shipub[] = {0x83, 0xc3, 0x36, 0x3c, 0xff, 0x27, 0x47, 0xb7, 0xf7, 0xeb, 0x19, 0x85, 0x17, 0x63, 0x1a, 0x71,
                        0x54, 0x76, 0xb4, 0xfe, 0x22, 0x46, 0x01, 0x45, 0x89, 0xc3, 0xac, 0x11, 0x8b, 0xb8, 0x9e, 0x51};
    uint8_t stpub[] = {0};
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_session_start(&handle, stpub, pkey_index, NULL, shipub));
}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_PARAM_ERR on non valid shipub
void test__invalid_shipub()
{
    lt_handle_t handle = {0};
    uint8_t pkey_index = 5;
    uint8_t shipriv[]
        = {0x80, 0x02, 0xc5, 0xa3, 0xff, 0x46, 0xa2, 0x09, 0x4e, 0x4e, 0x71, 0xf3, 0xc8, 0xe3, 0xdd, 0x79,
           0xec, 0x5c, 0x1c, 0xcd, 0xb0, 0x40, 0xbb, 0xcf, 0x6f, 0x64, 0x9d, 0x49, 0xe9, 0x1d, 0x9c, 0x53};
    uint8_t stpub[] = {0};
    TEST_ASSERT_EQUAL(LT_PARAM_ERR, lt_session_start(&handle, stpub, pkey_index, shipriv, NULL));
}

//---------------------------------------------------------------------------------------------------------//
//---------------------------------- EXECUTION ------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------//

/*void mock_all_sha256()
{
    // Static needed, because data here are used after function return for comparison
    // by CMock.
    static lt_crypto_sha256_ctx_t hctx = {0};
    static uint8_t protocol_name[32] =
{'N','o','i','s','e','_','K','K','1','_','2','5','5','1','9','_','A','E','S','G','C','M','_','S','H','A','2','5','6',0x00,0x00,0x00};
    static uint8_t hash[LT_SHA256_DIGEST_LENGTH] = {0};

    // hctx is useful to check, because we need it blank (zeroed)
    lt_sha256_init_Expect(&hctx);
    for (int i = 0; i < 6; i++) {
        lt_sha256_start_Expect(&hctx);
        if (i > 0) {
            lt_sha256_update_Expect(&hctx, hash, 32);
            lt_sha256_update_ExpectAnyArgs(); // not interested in exact pointer addresses
        } else {
            lt_sha256_update_ExpectWithArray(&hctx, LT_SHA256_DIGEST_LENGTH, protocol_name, 32, 32);
        }
        lt_sha256_finish_Expect(&hctx, hash);
    }

    for (int i = 0; i < 3; i++) {
        lt_X25519_ExpectAnyArgs();
        lt_hkdf_ExpectAnyArgs();
    }
    lt_hkdf_ExpectAnyArgs();
}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_FAIL on failed lt_random_bytes()
//void test__random_bytes_error()
//{
//    lt_handle_t h = {0};
//    uint8_t stpub   = 0;
//    uint8_t shipriv = 0;
//    uint8_t shipub  = 0;
//
//    lt_random_bytes_ExpectAnyArgsAndReturn(LT_FAIL);
//    TEST_ASSERT_EQUAL(LT_FAIL, lt_session_start(&h, &stpub, 0, &shipriv, &shipub));
//}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_FAIL on failed lt_l2_transfer()
//void test__l2_transfer_error()
//{
//    lt_handle_t h = {0};
//    uint8_t stpub   = 0;
//    uint8_t shipriv = 0;
//    uint8_t shipub  = 0;
//
//    lt_random_bytes_ExpectAnyArgsAndReturn(LT_OK);
//    lt_X25519_scalarmult_ExpectAnyArgs();
//    lt_l2_transfer_ExpectAndReturn(&h, LT_FAIL);
//
//    TEST_ASSERT_EQUAL(LT_FAIL, lt_session_start(&h, &stpub, 0, &shipriv, &shipub));
//}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_CRYPTO_ERR on failed lt_aesgcm_init_and_key()
//void test__lt_aesgcm_init_error()
//{
//    lt_handle_t h = {0};
//    uint8_t stpub = 0;
//    uint8_t shipriv = 0;
//    uint8_t shipub = 0;
//    pkey_index_t pkey_index = 0;
//
//
//    lt_random_bytes_ExpectAnyArgsAndReturn(LT_OK);
//    lt_X25519_scalarmult_ExpectAnyArgs();
//    lt_l2_transfer_ExpectAndReturn(&h, LT_OK);
//    mock_all_sha256();
//
//    lt_aesgcm_init_and_key_ExpectAnyArgsAndReturn(LT_FAIL);
//
//    TEST_ASSERT_EQUAL(LT_CRYPTO_ERR, lt_session_start(&h, &stpub, pkey_index, &shipriv, &shipub));
//}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_CRYPTO_ERR on failed lt_aesgcm_decrypt()
//void test__lt_aesgcm_decrypt_error()
//{
//    lt_handle_t h = {0};
//    uint8_t stpub = 0;
//    uint8_t shipriv = 0;
//    uint8_t shipub = 0;
//    pkey_index_t pkey_index = 0;
//
//    lt_random_bytes_ExpectAnyArgsAndReturn(LT_OK);
//    lt_X25519_scalarmult_ExpectAnyArgs();
//    lt_l2_transfer_ExpectAndReturn(&h, LT_OK);
//    mock_all_sha256();
//    lt_aesgcm_init_and_key_ExpectAnyArgsAndReturn(LT_OK);
//    lt_aesgcm_decrypt_ExpectAnyArgsAndReturn(LT_FAIL);
//
//    TEST_ASSERT_EQUAL(LT_CRYPTO_ERR, lt_session_start(&h, &stpub, pkey_index, &shipriv, &shipub));
//}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_CRYPTO_ERR on second failed lt_aesgcm_init_and_key()
//void test__lt_aesgcm_2nd_init_error()
//{
//    lt_handle_t h = {0};
//    uint8_t stpub = 0;
//    uint8_t shipriv = 0;
//    uint8_t shipub = 0;
//    pkey_index_t pkey_index = 0;
//
//    lt_random_bytes_ExpectAnyArgsAndReturn(LT_OK);
//    lt_X25519_scalarmult_ExpectAnyArgs();
//    lt_l2_transfer_ExpectAndReturn(&h, LT_OK);
//    mock_all_sha256();
//    lt_aesgcm_init_and_key_ExpectAnyArgsAndReturn(LT_OK);
//    lt_aesgcm_decrypt_ExpectAnyArgsAndReturn(LT_OK);
//    lt_aesgcm_init_and_key_ExpectAnyArgsAndReturn(LT_FAIL);
//
//    TEST_ASSERT_EQUAL(LT_CRYPTO_ERR, lt_session_start(&h, &stpub, pkey_index, &shipriv, &shipub));
//}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_CRYPTO_ERR on third failed lt_aesgcm_init_and_key()
//void test__lt_aesgcm_3rd_init_error()
//{
//    lt_handle_t h = {0};
//    uint8_t stpub = 0;
//    uint8_t shipriv = 0;
//    uint8_t shipub = 0;
//    pkey_index_t pkey_index = 0;
//
//    lt_random_bytes_ExpectAnyArgsAndReturn(LT_OK);
//    lt_X25519_scalarmult_ExpectAnyArgs();
//    lt_l2_transfer_ExpectAndReturn(&h, LT_OK);
//    mock_all_sha256();
//    lt_aesgcm_init_and_key_ExpectAnyArgsAndReturn(LT_OK);
//    lt_aesgcm_decrypt_ExpectAnyArgsAndReturn(LT_OK);
//    lt_aesgcm_init_and_key_ExpectAnyArgsAndReturn(LT_OK);
//    lt_aesgcm_init_and_key_ExpectAnyArgsAndReturn(LT_FAIL);
//
//    TEST_ASSERT_EQUAL(LT_CRYPTO_ERR, lt_session_start(&h, &stpub, pkey_index, &shipriv, &shipub));
//}

//---------------------------------------------------------------------------------------------------------//

// Test if function returns LT_OK if executed correctly
//void test__correct()
//{
//    lt_handle_t h = {0};
//    uint8_t stpub = 0;
//    uint8_t shipriv = 0;
//    uint8_t shipub = 0;
//    pkey_index_t pkey_index = 0;
//
//    lt_random_bytes_ExpectAnyArgsAndReturn(LT_OK);
//    lt_X25519_scalarmult_ExpectAnyArgs();
//    lt_l2_transfer_ExpectAndReturn(&h, LT_OK);
//    mock_all_sha256();
//    lt_aesgcm_init_and_key_ExpectAnyArgsAndReturn(LT_OK);
//    lt_aesgcm_decrypt_ExpectAnyArgsAndReturn(LT_OK);
//    lt_aesgcm_init_and_key_ExpectAnyArgsAndReturn(LT_OK);
//    lt_aesgcm_init_and_key_ExpectAnyArgsAndReturn(LT_OK);
//
//    TEST_ASSERT_EQUAL(LT_OK, lt_session_start(&h, &stpub, pkey_index, &shipriv, &shipub));
//}
*/