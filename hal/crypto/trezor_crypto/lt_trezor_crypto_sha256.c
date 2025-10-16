/**
 * @file lt_trezor_crypto_sha256.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdint.h>
#include <string.h>

#include "hasher.h"
#include "libtropic_common.h"
#include "lt_crypto_macros.h"
#include "lt_sha256.h"

lt_ret_t lt_sha256_init(LT_CRYPTO_SHA256_CTX_T *ctx)
{
    memset(ctx, 0, sizeof(LT_CRYPTO_SHA256_CTX_T));
    return LT_OK;
}

lt_ret_t lt_sha256_start(LT_CRYPTO_SHA256_CTX_T *ctx)
{
    hasher_InitParam(ctx, HASHER_SHA2, NULL, 0);
    return LT_OK;
}

lt_ret_t lt_sha256_update(LT_CRYPTO_SHA256_CTX_T *ctx, const uint8_t *input, const size_t input_len)
{
    hasher_Update(ctx, input, input_len);
    return LT_OK;
}

lt_ret_t lt_sha256_finish(LT_CRYPTO_SHA256_CTX_T *ctx, uint8_t *output)
{
    hasher_Final(ctx, output);
    return LT_OK;
}