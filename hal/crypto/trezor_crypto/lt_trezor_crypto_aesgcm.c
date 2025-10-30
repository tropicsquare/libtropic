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
#include "lt_trezor_crypto.h"

/**
 * @brief Initializes Trezor crypto AES-GCM context.
 *
 * @param ctx      AES-GCM context structure (Trezor crypto specific)
 * @param key      Key to initialize with
 * @param key_len  Length of the key
 * @return         LT_OK if success, otherwise returns other error code.
 */
static lt_ret_t lt_aesgcm_init(gcm_ctx *ctx, const uint8_t *key, const uint32_t key_len)
{
    int ret = gcm_init_and_key(key, key_len, ctx);
    if (ret != RETURN_GOOD) {
        return LT_CRYPTO_ERR;
    }
    return LT_OK;
}

/**
 * @brief Deinitializes Trezor crypto AES-GCM context.
 *
 * @param ctx  AES-GCM context structure (Trezor crypto specific)
 * @return     LT_OK if success, otherwise returns other error code.
 */
static lt_ret_t lt_aesgcm_deinit(gcm_ctx *ctx)
{
    int ret = gcm_end(ctx);
    if (ret != RETURN_GOOD) {
        return LT_CRYPTO_ERR;
    }
    return LT_OK;
}

lt_ret_t lt_aesgcm_encrypt_init(void *ctx, const uint8_t *key, const uint32_t key_len)
{
    lt_ctx_trezor_crypto_t *_ctx = (lt_ctx_trezor_crypto_t *)ctx;

    return lt_aesgcm_init(&_ctx->aesgcm_encrypt_ctx, key, key_len);
}

lt_ret_t lt_aesgcm_decrypt_init(void *ctx, const uint8_t *key, const uint32_t key_len)
{
    lt_ctx_trezor_crypto_t *_ctx = (lt_ctx_trezor_crypto_t *)ctx;

    return lt_aesgcm_init(&_ctx->aesgcm_decrypt_ctx, key, key_len);
}

lt_ret_t lt_aesgcm_encrypt(void *ctx, const uint8_t *iv, const uint32_t iv_len, const uint8_t *add,
                           const uint32_t add_len, const uint8_t *plaintext, const uint32_t plaintext_len,
                           uint8_t *ciphertext, const uint32_t ciphertext_len)
{
    lt_ctx_trezor_crypto_t *_ctx = (lt_ctx_trezor_crypto_t *)ctx;

    if (plaintext_len != ciphertext_len - TR01_L3_TAG_SIZE) {
        return LT_PARAM_ERR;
    }

    // Copy plaintext into ciphertext, as Trezor's gcm_encrypt_message() works in-place
    memcpy(ciphertext, plaintext, plaintext_len);

    int ret = gcm_encrypt_message(iv, iv_len, add, add_len, ciphertext, plaintext_len, ciphertext + plaintext_len,
                                  TR01_L3_TAG_SIZE, &_ctx->aesgcm_encrypt_ctx);
    if (ret != RETURN_GOOD) {
        return LT_CRYPTO_ERR;
    }

    return LT_OK;
}

lt_ret_t lt_aesgcm_decrypt(void *ctx, const uint8_t *iv, const uint32_t iv_len, const uint8_t *add,
                           const uint32_t add_len, const uint8_t *ciphertext, const uint32_t ciphertext_len,
                           uint8_t *plaintext, const uint32_t plaintext_len)
{
    lt_ctx_trezor_crypto_t *_ctx = (lt_ctx_trezor_crypto_t *)ctx;

    if (plaintext_len != ciphertext_len - TR01_L3_TAG_SIZE) {
        return LT_PARAM_ERR;
    }

    // Copy ciphertext into plaintext, as Trezor's gcm_decrypt_message() works in-place
    memcpy(plaintext, ciphertext, plaintext_len);

    int ret = gcm_decrypt_message(iv, iv_len, add, add_len, plaintext, plaintext_len, ciphertext + plaintext_len,
                                  TR01_L3_TAG_SIZE, &_ctx->aesgcm_decrypt_ctx);
    if (ret != RETURN_GOOD) {
        return LT_CRYPTO_ERR;
    }

    return LT_OK;
}

lt_ret_t lt_aesgcm_encrypt_deinit(void *ctx)
{
    lt_ctx_trezor_crypto_t *_ctx = (lt_ctx_trezor_crypto_t *)ctx;

    return lt_aesgcm_deinit(&_ctx->aesgcm_encrypt_ctx);
}

lt_ret_t lt_aesgcm_decrypt_deinit(void *ctx)
{
    lt_ctx_trezor_crypto_t *_ctx = (lt_ctx_trezor_crypto_t *)ctx;

    return lt_aesgcm_deinit(&_ctx->aesgcm_decrypt_ctx);
}