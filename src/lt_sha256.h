#ifndef LT_SHA256_H
#define LT_SHA256_H

/**
 * @file   lt_sha256.h
 * @brief  SHA256 functions declarations
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stddef.h>
#include <stdint.h>

#include "libtropic_common.h"
#include "lt_crypto_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Length of SHA-256 digest. */
#define LT_SHA256_DIGEST_LENGTH 32

/**
 * @brief Initializes hash context.
 *
 * @param  ctx Hash context
 * @return LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_sha256_init(LT_CRYPTO_SHA256_CTX_T *ctx) __attribute__((warn_unused_result));

/**
 * @brief Starts SHA-256 calculation.
 *
 * @param  ctx
 * @return LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_sha256_start(LT_CRYPTO_SHA256_CTX_T *ctx) __attribute__((warn_unused_result));

/**
 * @brief Adds data to SHA-256 context.
 *
 * @param  ctx        Hash context
 * @param  input      Input data
 * @param  input_len  Length of input data
 * @return LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_sha256_update(LT_CRYPTO_SHA256_CTX_T *ctx, const uint8_t *input, const size_t input_len)
    __attribute__((warn_unused_result));

/**
 * @brief Finishes SHA-256 operation.
 *
 * @param  ctx     Hash context
 * @param  output  Hash digest
 * @return LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_sha256_finish(LT_CRYPTO_SHA256_CTX_T *ctx, uint8_t *output) __attribute__((warn_unused_result));

#ifdef __cplusplus
}
#endif

#endif  // LT_SHA256_H
