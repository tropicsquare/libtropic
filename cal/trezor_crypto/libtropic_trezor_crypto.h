#ifndef LT_TREZOR_CRYPTO_H
#define LT_TREZOR_CRYPTO_H

/**
 * @file lt_trezor_crypto.h
 * @brief Trezor crypto public declarations.
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md file in the root directory of this source tree.
 */

#include "aes/aesgcm.h"
#include "hasher.h"

/**
 * @brief Context structure for Trezor crypto.
 *
 */
typedef struct lt_ctx_trezor_crypto_t {
    /** @private @brief AES-GCM context for encryption. */
    gcm_ctx aesgcm_encrypt_ctx;
    /** @private @brief AES-GCM context for decryption. */
    gcm_ctx aesgcm_decrypt_ctx;
    /** @private @brief SHA-256 context. */
    Hasher sha256_ctx;
} lt_ctx_trezor_crypto_t;

#endif  // LT_TREZOR_CRYPTO_H