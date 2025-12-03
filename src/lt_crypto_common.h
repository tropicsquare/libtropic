#ifndef LT_CRYPTO_COMMON_H
#define LT_CRYPTO_COMMON_H

/**
 * @file lt_crypto_common.h
 * @brief Common CAL declarations.
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md file in the root directory of this source tree.
 */

#include "libtropic_common.h"

/**
 * @brief Initializes the crypto context.
 *
 * @param ctx  Context structure
 * @return     LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_crypto_ctx_init(void *ctx) __attribute__((warn_unused_result));

/**
 * @brief Deinitializes the crypto context.
 *
 * @param ctx  Context structure
 * @return     LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_crypto_ctx_deinit(void *ctx) __attribute__((warn_unused_result));

#endif  // LT_CRYPTO_COMMON_H