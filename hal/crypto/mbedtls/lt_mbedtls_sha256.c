/**
 * @file lt_mbedtls_sha256.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdint.h>
#include <string.h>

#include "mbedtls/sha256.h"
#include "libtropic_common.h"
#include "lt_crypto_macros.h"
#include "lt_sha256.h"

lt_ret_t lt_sha256_init(LT_CRYPTO_SHA256_CTX_T *ctx)
{
    mbedtls_sha256_init(ctx);
    return LT_OK;
}

lt_ret_t lt_sha256_start(LT_CRYPTO_SHA256_CTX_T *ctx)
{
    int ret = mbedtls_sha256_starts(ctx, 0); // 0 for SHA-256 (not SHA-224)
    if (ret != 0) {
        return LT_CRYPTO_ERR;
    }
    return LT_OK;
}

lt_ret_t lt_sha256_update(LT_CRYPTO_SHA256_CTX_T *ctx, const uint8_t *input, const size_t input_len)
{
    int ret = mbedtls_sha256_update(ctx, input, input_len);
    if (ret != 0) {
        return LT_CRYPTO_ERR;
    }
    return LT_OK;
}

lt_ret_t lt_sha256_finish(LT_CRYPTO_SHA256_CTX_T *ctx, uint8_t *output)
{
    int ret = mbedtls_sha256_finish(ctx, output);
    if (ret != 0) {
        return LT_CRYPTO_ERR;
    }
    return LT_OK;
}
