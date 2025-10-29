/**
 * @file lt_trezor_crypto_aesgcm.c
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
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
                           const uint32_t add_len, const uint8_t *plaintext, const uint32_t plaintext_len,
                           uint8_t *ciphertext, const uint32_t ciphertext_len)
{
    if (plaintext_len != ciphertext_len - TR01_L3_TAG_SIZE) {
        return LT_PARAM_ERR;
    }

    // Copy plaintext into ciphertext, as Trezor's gcm_encrypt_message() works in-place
    memcpy(ciphertext, plaintext, plaintext_len);

    int ret = gcm_encrypt_message(iv, iv_len, add, add_len, ciphertext, plaintext_len, ciphertext + plaintext_len,
                                  TR01_L3_TAG_SIZE, ctx);
    if (ret != RETURN_GOOD) {
        return LT_CRYPTO_ERR;
    }

    return LT_OK;
}

lt_ret_t lt_aesgcm_decrypt(LT_CRYPTO_AES_GCM_CTX_T *ctx, const uint8_t *iv, const uint32_t iv_len, const uint8_t *add,
                           const uint32_t add_len, const uint8_t *ciphertext, const uint32_t ciphertext_len,
                           uint8_t *plaintext, const uint32_t plaintext_len)
{
    if (ciphertext_len < plaintext_len || plaintext_len != ciphertext_len - TR01_L3_TAG_SIZE) {
        return LT_PARAM_ERR;
    }

    // Copy ciphertext into plaintext, as Trezor's gcm_decrypt_message() works in-place
    memcpy(plaintext, ciphertext, plaintext_len);

    int ret = gcm_decrypt_message(iv, iv_len, add, add_len, plaintext, plaintext_len, ciphertext + plaintext_len,
                                  TR01_L3_TAG_SIZE, ctx);
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