/**
 * @file lt_trezor_crypto_sha256.c
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdint.h>
#include <string.h>

#include "hasher.h"
#include "libtropic_common.h"
#include "lt_sha256.h"
#include "lt_trezor_crypto.h"

lt_ret_t lt_sha256_init(void *ctx)
{
    lt_ctx_trezor_crypto_t *_ctx = (lt_ctx_trezor_crypto_t *)ctx;

    memset(&_ctx->sha256_ctx, 0, sizeof(_ctx->sha256_ctx));
    return LT_OK;
}

lt_ret_t lt_sha256_start(void *ctx)
{
    lt_ctx_trezor_crypto_t *_ctx = (lt_ctx_trezor_crypto_t *)ctx;

    hasher_InitParam(&_ctx->sha256_ctx, HASHER_SHA2, NULL, 0);
    return LT_OK;
}

lt_ret_t lt_sha256_update(void *ctx, const uint8_t *input, const size_t input_len)
{
    lt_ctx_trezor_crypto_t *_ctx = (lt_ctx_trezor_crypto_t *)ctx;

    hasher_Update(&_ctx->sha256_ctx, input, input_len);
    return LT_OK;
}

lt_ret_t lt_sha256_finish(void *ctx, uint8_t *output)
{
    lt_ctx_trezor_crypto_t *_ctx = (lt_ctx_trezor_crypto_t *)ctx;

    hasher_Final(&_ctx->sha256_ctx, output);
    return LT_OK;
}