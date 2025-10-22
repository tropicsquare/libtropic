#ifndef LT_CRYPTO_MACROS_H
#define LT_CRYPTO_MACROS_H

#include "mbedtls/gcm.h"
#include "mbedtls/sha256.h"

/** @brief Type of AES-GCM context for MbedTLS crypto. */
#define LT_CRYPTO_AES_GCM_CTX_T mbedtls_gcm_context

/** @brief Type of SHA-256 context for MbedTLS crypto. */
#define LT_CRYPTO_SHA256_CTX_T mbedtls_sha256_context

#endif  // LT_CRYPTO_MACROS_H
