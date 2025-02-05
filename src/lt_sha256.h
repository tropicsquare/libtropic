#ifndef LT_SHA256_H
#define LT_SHA256_H

/**
 * @file   lt_sha256.h
 * @brief  SHA256 functions declarations
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdint.h>
#include <stddef.h>

/** Length of sha256 digest */
#define SHA256_DIGEST_LENGTH 32

/** sha256 context structure */
typedef struct  lt_crypto_sha256_ctx
{
#ifdef USE_MBEDTLS
    uint32_t space[32];
#elif LT_USE_TREZOR_CRYPTO
    uint32_t space[256];  // mbedtls is ok with 32. TODO how big is Hasher struct
#endif
}  lt_crypto_sha256_ctx_t;

/**
 * @details This function initializes hash context
 * @param ctx   Hash context
 */
void lt_sha256_init(void* ctx);

/**
 * @details This function starts hash context
 * @param ctx
 */
void lt_sha256_start(void *ctx);

/**
 * @details This function add data to hashing context
 * @param ctx   Hash context
 * @param input Input data
 * @param len   Length of input data
 */
void lt_sha256_update(void *ctx, const uint8_t *input, size_t len);

/**
 * @brief This function finalizes hashing and outputs a digest
 * @param ctx    Hash context
 * @param output Hash digest
 */
void lt_sha256_finish(void * ctx, uint8_t *output);

#endif
