/**
 * @file lt_mbedtls_v4_sha256.c
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md file in the root directory of this source tree.
 */

#include <inttypes.h>
#include <stdint.h>
#include <string.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"
#include "psa/crypto.h"
#pragma GCC diagnostic pop
#include "libtropic_common.h"
#include "libtropic_logging.h"
#include "libtropic_mbedtls_v4.h"
#include "lt_sha256.h"

lt_ret_t lt_sha256_init(void *ctx)
{
    lt_ctx_mbedtls_v4_t *_ctx = (lt_ctx_mbedtls_v4_t *)ctx;

    // Initialize the hash operation
    _ctx->sha256_ctx = psa_hash_operation_init();
    return LT_OK;
}

lt_ret_t lt_sha256_start(void *ctx)
{
    lt_ctx_mbedtls_v4_t *_ctx = (lt_ctx_mbedtls_v4_t *)ctx;
    psa_status_t status;

    // Set up the hash operation for SHA-256
    status = psa_hash_setup(&_ctx->sha256_ctx, PSA_ALG_SHA_256);
    if (status != PSA_SUCCESS) {
        LT_LOG_ERROR("SHA-256 setup failed, status=%" PRId32 " (psa_status_t)", status);
        psa_hash_abort(&_ctx->sha256_ctx);
        return LT_CRYPTO_ERR;
    }

    return LT_OK;
}

lt_ret_t lt_sha256_update(void *ctx, const uint8_t *input, const size_t input_len)
{
    lt_ctx_mbedtls_v4_t *_ctx = (lt_ctx_mbedtls_v4_t *)ctx;
    psa_status_t status;

    // Update the hash with input data
    status = psa_hash_update(&_ctx->sha256_ctx, input, input_len);
    if (status != PSA_SUCCESS) {
        LT_LOG_ERROR("SHA-256 update failed, status=%" PRId32 " (psa_status_t)", status);
        psa_hash_abort(&_ctx->sha256_ctx);
        return LT_CRYPTO_ERR;
    }

    return LT_OK;
}

lt_ret_t lt_sha256_finish(void *ctx, uint8_t *output)
{
    lt_ctx_mbedtls_v4_t *_ctx = (lt_ctx_mbedtls_v4_t *)ctx;
    psa_status_t status;
    size_t hash_length;

    // Finalize the hash and get the digest
    status = psa_hash_finish(&_ctx->sha256_ctx, output, PSA_HASH_LENGTH(PSA_ALG_SHA_256), &hash_length);
    if (status != PSA_SUCCESS) {
        LT_LOG_ERROR("SHA-256 finish failed, status=%" PRId32 " (psa_status_t)", status);
        psa_hash_abort(&_ctx->sha256_ctx);
        return LT_CRYPTO_ERR;
    }

    return LT_OK;
}
