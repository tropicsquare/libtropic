
#include "unity.h"
#include "string.h"

#include "libtropic_common.h"
#include "libtropic.h"
#include "lt_l2_api_structs.h"

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

void mock_all_sha256()
{
    // Static needed, because data here are used after function return for comparison 
    // by CMock.
    static lt_crypto_sha256_ctx_t hctx = {0};
    static uint8_t protocol_name[32] = {'N','o','i','s','e','_','K','K','1','_','2','5','5','1','9','_','A','E','S','G','C','M','_','S','H','A','2','5','6',0x00,0x00,0x00};
    static uint8_t hash[SHA256_DIGEST_LENGTH] = {0};

    // hctx is useful to check, because we need it blank (zeroed)
    lt_sha256_init_Expect(&hctx);
    for (int i = 0; i < 6; i++) {
        lt_sha256_start_Expect(&hctx);
        if (i > 0) {
            lt_sha256_update_Expect(&hctx, hash, 32);
            lt_sha256_update_ExpectAnyArgs(); // not interested in exact pointer addresses
        } else {
            lt_sha256_update_ExpectWithArray(&hctx, SHA256_DIGEST_LENGTH, protocol_name, 32, 32);
        }
        lt_sha256_finish_Expect(&hctx, hash);
    }

    for (int i = 0; i < 3; i++) {
        lt_X25519_ExpectAnyArgs();
        lt_hkdf_ExpectAnyArgs();
    }
    lt_hkdf_ExpectAnyArgs();
}

void test_lt_handshake__random_bytes_error()
{
    lt_handle_t h       = {0};
    uint8_t     stpub   = 0;
    uint8_t     shipriv = 0; 
    uint8_t     shipub  = 0;

    lt_random_bytes_ExpectAnyArgsAndReturn(LT_FAIL);
    TEST_ASSERT_EQUAL(lt_handshake(&h, &stpub, 0, &shipriv, &shipub), LT_FAIL);
}

void test_lt_handshake__l2_transfer_error()
{
    lt_handle_t h = {0};
    uint8_t     stpub   = 0;
    uint8_t     shipriv = 0; 
    uint8_t     shipub  = 0;

    lt_random_bytes_ExpectAnyArgsAndReturn(LT_OK);
    lt_X25519_scalarmult_ExpectAnyArgs();
    lt_l2_transfer_ExpectAndReturn(&h, LT_FAIL);

    TEST_ASSERT_EQUAL(lt_handshake(&h, &stpub, 0, &shipriv, &shipub), LT_FAIL);
}

void test_lt_handshake__lt_aesgcm_init_error()
{
    lt_handle_t  h          = {0};
    uint8_t      stpub      = 0;
    uint8_t      shipriv    = 0; 
    uint8_t      shipub     = 0;
    pkey_index_t pkey_index = 0;

    
    lt_random_bytes_ExpectAnyArgsAndReturn(LT_OK);
    lt_X25519_scalarmult_ExpectAnyArgs();
    lt_l2_transfer_ExpectAndReturn(&h, LT_OK);
    mock_all_sha256();

    lt_aesgcm_init_and_key_ExpectAnyArgsAndReturn(LT_FAIL);

    TEST_ASSERT_EQUAL(lt_handshake(&h, &stpub, 0, &shipriv, &shipub), LT_CRYPTO_ERR);
}

void test_lt_handshake__lt_aesgcm_decrypt_error()
{
    lt_handle_t  h          = {0};
    uint8_t      stpub      = 0;
    uint8_t      shipriv    = 0; 
    uint8_t      shipub     = 0;
    pkey_index_t pkey_index = 0;

    
    lt_random_bytes_ExpectAnyArgsAndReturn(LT_OK);
    lt_X25519_scalarmult_ExpectAnyArgs();
    lt_l2_transfer_ExpectAndReturn(&h, LT_OK);
    mock_all_sha256();

    lt_aesgcm_init_and_key_ExpectAnyArgsAndReturn(LT_OK);
    lt_aesgcm_decrypt_ExpectAnyArgsAndReturn(LT_FAIL);

    TEST_ASSERT_EQUAL(lt_handshake(&h, &stpub, 0, &shipriv, &shipub), LT_CRYPTO_ERR);
}

void test_lt_handshake__lt_aesgcm_2nd_init_error()
{
    lt_handle_t  h          = {0};
    uint8_t      stpub      = 0;
    uint8_t      shipriv    = 0; 
    uint8_t      shipub     = 0;
    pkey_index_t pkey_index = 0;

    
    lt_random_bytes_ExpectAnyArgsAndReturn(LT_OK);
    lt_X25519_scalarmult_ExpectAnyArgs();
    lt_l2_transfer_ExpectAndReturn(&h, LT_OK);
    mock_all_sha256();

    lt_aesgcm_init_and_key_ExpectAnyArgsAndReturn(LT_OK);
    lt_aesgcm_decrypt_ExpectAnyArgsAndReturn(LT_OK);
    lt_aesgcm_init_and_key_ExpectAnyArgsAndReturn(LT_FAIL);

    TEST_ASSERT_EQUAL(lt_handshake(&h, &stpub, 0, &shipriv, &shipub), LT_CRYPTO_ERR);
}

void test_lt_handshake__lt_aesgcm_3rd_init_error()
{
    lt_handle_t  h          = {0};
    uint8_t      stpub      = 0;
    uint8_t      shipriv    = 0; 
    uint8_t      shipub     = 0;
    pkey_index_t pkey_index = 0;

    
    lt_random_bytes_ExpectAnyArgsAndReturn(LT_OK);
    lt_X25519_scalarmult_ExpectAnyArgs();
    lt_l2_transfer_ExpectAndReturn(&h, LT_OK);
    mock_all_sha256();

    lt_aesgcm_init_and_key_ExpectAnyArgsAndReturn(LT_OK);
    lt_aesgcm_decrypt_ExpectAnyArgsAndReturn(LT_OK);
    lt_aesgcm_init_and_key_ExpectAnyArgsAndReturn(LT_OK);
    lt_aesgcm_init_and_key_ExpectAnyArgsAndReturn(LT_FAIL);

    TEST_ASSERT_EQUAL(lt_handshake(&h, &stpub, 0, &shipriv, &shipub), LT_CRYPTO_ERR);
}

void test_lt_handshake__correct()
{
    lt_handle_t  h          = {0};
    uint8_t      stpub      = 0;
    uint8_t      shipriv    = 0; 
    uint8_t      shipub     = 0;
    pkey_index_t pkey_index = 0;

    
    lt_random_bytes_ExpectAnyArgsAndReturn(LT_OK);
    lt_X25519_scalarmult_ExpectAnyArgs();
    lt_l2_transfer_ExpectAndReturn(&h, LT_OK);
    mock_all_sha256();

    lt_aesgcm_init_and_key_ExpectAnyArgsAndReturn(LT_OK);
    lt_aesgcm_decrypt_ExpectAnyArgsAndReturn(LT_OK);
    lt_aesgcm_init_and_key_ExpectAnyArgsAndReturn(LT_OK);
    lt_aesgcm_init_and_key_ExpectAnyArgsAndReturn(LT_OK);

    TEST_ASSERT_EQUAL(lt_handshake(&h, &stpub, 0, &shipriv, &shipub), LT_OK);
}