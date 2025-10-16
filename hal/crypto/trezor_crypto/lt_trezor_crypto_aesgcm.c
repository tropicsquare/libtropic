/**
 * @file lt_trezor_crypto_aesgcm.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "aes/aes.h"
#include "aes/aesgcm.h"
#include "libtropic_common.h"
#include "lt_aesgcm.h"
#include "lt_crypto_macros.h"

lt_ret_t lt_aesgcm_init_and_key(LT_CRYPTO_AES_GCM_CTX_T *ctx, const uint8_t *key, const uint32_t key_len)
{
    int ret = gcm_init_and_key(key, key_len, ctx);
    if (ret != RETURN_GOOD) {
        return LT_CRYPTO_ERR;
    }
    return LT_OK;
}

lt_ret_t lt_aesgcm_encrypt(LT_CRYPTO_AES_GCM_CTX_T *ctx, const uint8_t *iv, const uint32_t iv_len, const uint8_t *add,
                           const uint32_t add_len, uint8_t *msg, const uint32_t msg_len, uint8_t *tag,
                           const uint32_t tag_len)
{
    int ret = gcm_encrypt_message(iv, iv_len, add, add_len, msg, msg_len, tag, tag_len, ctx);
    if (ret != RETURN_GOOD) {
        return LT_CRYPTO_ERR;
    }

    return LT_OK;
}

lt_ret_t lt_aesgcm_decrypt(LT_CRYPTO_AES_GCM_CTX_T *ctx, const uint8_t *iv, const uint32_t iv_len, const uint8_t *add,
                           const uint32_t add_len, uint8_t *msg, const uint32_t msg_len, const uint8_t *tag,
                           const uint32_t tag_len)
{
    int ret = gcm_decrypt_message(iv, iv_len, add, add_len, msg, msg_len, tag, tag_len, ctx);
    if (ret != RETURN_GOOD) {
        return LT_CRYPTO_ERR;
    }

    return LT_OK;
}

lt_ret_t lt_aesgcm_end(LT_CRYPTO_AES_GCM_CTX_T *ctx)
{
    int ret = gcm_end(ctx);
    if (ret != RETURN_GOOD) {
        return LT_CRYPTO_ERR;
    }
    return LT_OK;
}