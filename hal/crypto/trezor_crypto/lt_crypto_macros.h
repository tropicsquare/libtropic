#ifndef LT_CRYPTO_MACROS_H
#define LT_CRYPTO_MACROS_H

#include "aes/aesgcm.h"
#include "hasher.h"

/** @brief Type of AES-GCM context for Trezor crypto. */
#define LT_CRYPTO_AES_GCM_CTX_T gcm_ctx

/** @brief Type of SHA-256 context for Trezor crypto. */
#define LT_CRYPTO_SHA256_CTX_T Hasher

#endif  // LT_CRYPTO_MACROS_H