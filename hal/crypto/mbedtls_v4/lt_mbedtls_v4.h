#ifndef LT_MBEDTLS_V4_H
#define LT_MBEDTLS_V4_H

/**
 * @file lt_mbedtls_v4.h
 * @brief MbedTLS public declarations.
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"
#include "psa/crypto.h"
#pragma GCC diagnostic pop

typedef struct lt_aesgcm_ctx_mbedtls_v4_t {
    /** @private @brief PSA key identifier. */
    psa_key_id_t key_id;
    /** @private @brief Flag indicating if key is set. */
    uint8_t key_set;
} lt_aesgcm_ctx_mbedtls_v4_t;

typedef struct lt_ctx_mbedtls_v4_t {
    /** @private @brief AES-GCM context for encryption. */
    lt_aesgcm_ctx_mbedtls_v4_t aesgcm_encrypt_ctx;
    /** @private @brief AES-GCM context for decryption. */
    lt_aesgcm_ctx_mbedtls_v4_t aesgcm_decrypt_ctx;
    /** @private @brief SHA-256 context. */
    psa_hash_operation_t sha256_ctx;
} lt_ctx_mbedtls_v4_t;

#endif  // LT_MBEDTLS_V4_H