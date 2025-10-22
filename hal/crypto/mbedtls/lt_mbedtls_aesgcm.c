/**
 * @file lt_mbedtls_aesgcm.c
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"
#include "psa/crypto.h"
#pragma GCC diagnostic pop
#include "libtropic_common.h"
#include "lt_aesgcm.h"
#include "lt_crypto_macros.h"

// PSA Crypto initialization state
static uint8_t psa_crypto_initialized = 0;

// Initialize PSA Crypto library if not already done
static lt_ret_t ensure_psa_crypto_init(void)
{
    if (!psa_crypto_initialized) {
        psa_status_t status = psa_crypto_init();
        if (status != PSA_SUCCESS) {
            return LT_CRYPTO_ERR;
        }
        psa_crypto_initialized = 1;
    }
    return LT_OK;
}

lt_ret_t lt_aesgcm_init_and_key(LT_CRYPTO_AES_GCM_CTX_T *ctx, const uint8_t *key, const uint32_t key_len)
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

    // Ensure PSA Crypto is initialized
    if (ensure_psa_crypto_init() != LT_OK) {
        return LT_CRYPTO_ERR;
    }

    // Initialize context
    memset(ctx, 0, sizeof(LT_CRYPTO_AES_GCM_CTX_T));

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_GCM);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, key_len * 8);

    // Import the key
    status = psa_import_key(&attributes, key, key_len, &ctx->key_id);
    psa_reset_key_attributes(&attributes);

    if (status != PSA_SUCCESS) {
        return LT_CRYPTO_ERR;
    }

    ctx->key_set = 1;
    return LT_OK;
}

lt_ret_t lt_aesgcm_encrypt(LT_CRYPTO_AES_GCM_CTX_T *ctx, const uint8_t *iv, const uint32_t iv_len, const uint8_t *add,
                           const uint32_t add_len, uint8_t *msg, const uint32_t msg_len, uint8_t *tag,
                           const uint32_t tag_len)
{
    psa_status_t status;
    size_t output_length;

    if (!ctx->key_set) {
        return LT_CRYPTO_ERR;
    }

    // PSA AEAD encrypt operation
    status = psa_aead_encrypt(ctx->key_id, PSA_ALG_GCM,
                              iv, iv_len,
                              add, add_len,
                              msg, msg_len,
                              msg, msg_len + tag_len, &output_length);

    if (status != PSA_SUCCESS) {
        return LT_CRYPTO_ERR;
    }

    // The tag is appended after the ciphertext in PSA output
    // Copy tag to separate buffer if needed
    if (output_length >= tag_len) {
        memcpy(tag, msg + msg_len, tag_len);
    } else {
        return LT_CRYPTO_ERR;
    }

    return LT_OK;
}

lt_ret_t lt_aesgcm_decrypt(LT_CRYPTO_AES_GCM_CTX_T *ctx, const uint8_t *iv, const uint32_t iv_len, const uint8_t *add,
                           const uint32_t add_len, uint8_t *msg, const uint32_t msg_len, const uint8_t *tag,
                           const uint32_t tag_len)
{
    psa_status_t status;
    size_t output_length;
    uint8_t *ciphertext_with_tag;
    size_t ciphertext_with_tag_len = msg_len + tag_len;

    if (!ctx->key_set) {
        return LT_CRYPTO_ERR;
    }

    // PSA expects ciphertext and tag concatenated
    ciphertext_with_tag = (uint8_t *)malloc(ciphertext_with_tag_len);
    if (ciphertext_with_tag == NULL) {
        return LT_CRYPTO_ERR;
    }

    memcpy(ciphertext_with_tag, msg, msg_len);
    memcpy(ciphertext_with_tag + msg_len, tag, tag_len);

    // PSA AEAD decrypt operation
    status = psa_aead_decrypt(ctx->key_id, PSA_ALG_GCM,
                              iv, iv_len,
                              add, add_len,
                              ciphertext_with_tag, ciphertext_with_tag_len,
                              msg, msg_len, &output_length);

    free(ciphertext_with_tag);

    if (status != PSA_SUCCESS) {
        return LT_CRYPTO_ERR;
    }

    return LT_OK;
}

lt_ret_t lt_aesgcm_end(LT_CRYPTO_AES_GCM_CTX_T *ctx)
{
    if (ctx->key_set) {
        psa_destroy_key(ctx->key_id);
        ctx->key_set = 0;
    }
    return LT_OK;
}
