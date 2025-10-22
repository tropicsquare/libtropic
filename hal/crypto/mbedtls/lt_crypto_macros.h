#ifndef LT_CRYPTO_MACROS_H
#define LT_CRYPTO_MACROS_H

#include "psa/crypto.h"

/** @brief Type of AES-GCM context for MbedTLS PSA Crypto. 
 * 
 * For PSA Crypto API, we store the key ID and other state needed
 * for multi-part AES-GCM operations.
 */
typedef struct {
    psa_key_id_t key_id;     /**< PSA key identifier */
    uint8_t key_set;         /**< Flag indicating if key is set */
} LT_CRYPTO_AES_GCM_CTX_T;

/** @brief Type of SHA-256 context for MbedTLS PSA Crypto. */
#define LT_CRYPTO_SHA256_CTX_T psa_hash_operation_t

#endif  // LT_CRYPTO_MACROS_H
