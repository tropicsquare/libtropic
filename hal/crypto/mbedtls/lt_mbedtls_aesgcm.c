/**
 * @file lt_mbedtls_aesgcm.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "mbedtls/gcm.h"
#include "mbedtls/aes.h"
#include "libtropic_common.h"
#include "lt_aesgcm.h"
#include "lt_crypto_macros.h"

lt_ret_t lt_aesgcm_init_and_key(LT_CRYPTO_AES_GCM_CTX_T *ctx, const uint8_t *key, const uint32_t key_len)
{
    mbedtls_gcm_init(ctx);
    
    int ret = mbedtls_gcm_setkey(ctx, MBEDTLS_CIPHER_ID_AES, key, key_len * 8);
    if (ret != 0) {
        return LT_CRYPTO_ERR;
    }
    return LT_OK;
}

lt_ret_t lt_aesgcm_encrypt(LT_CRYPTO_AES_GCM_CTX_T *ctx, const uint8_t *iv, const uint32_t iv_len, const uint8_t *add,
                           const uint32_t add_len, uint8_t *msg, const uint32_t msg_len, uint8_t *tag,
                           const uint32_t tag_len)
{
    int ret = mbedtls_gcm_crypt_and_tag(ctx, MBEDTLS_GCM_ENCRYPT, msg_len, iv, iv_len, add, add_len, msg, msg, tag_len, tag);
    if (ret != 0) {
        return LT_CRYPTO_ERR;
    }

    return LT_OK;
}

lt_ret_t lt_aesgcm_decrypt(LT_CRYPTO_AES_GCM_CTX_T *ctx, const uint8_t *iv, const uint32_t iv_len, const uint8_t *add,
                           const uint32_t add_len, uint8_t *msg, const uint32_t msg_len, const uint8_t *tag,
                           const uint32_t tag_len)
{
    int ret = mbedtls_gcm_auth_decrypt(ctx, msg_len, iv, iv_len, add, add_len, tag, tag_len, msg, msg);
    if (ret != 0) {
        return LT_CRYPTO_ERR;
    }

    return LT_OK;
}

lt_ret_t lt_aesgcm_end(LT_CRYPTO_AES_GCM_CTX_T *ctx)
{
    mbedtls_gcm_free(ctx);
    return LT_OK;
}
