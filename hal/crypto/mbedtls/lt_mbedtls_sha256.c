/**
 * @file lt_mbedtls_sha256.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdint.h>
#include <string.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"
#include "psa/crypto.h"
#pragma GCC diagnostic pop
#include "libtropic_common.h"
#include "lt_crypto_macros.h"
#include "lt_sha256.h"

// PSA Crypto initialization state
static uint8_t psa_crypto_initialized = 0;

// Initialize PSA Crypto library if not already done
static lt_ret_t ensure_psa_crypto_init(void)
{
    if (!psa_crypto_initialized) {
        psa_status_t status = psa_crypto_init();
        if (status != PSA_SUCCESS) {
            return LT_CRYPTO_ERR;
        }
        psa_crypto_initialized = 1;
    }
    return LT_OK;
}

lt_ret_t lt_sha256_init(LT_CRYPTO_SHA256_CTX_T *ctx)
{
    // Ensure PSA Crypto is initialized
    if (ensure_psa_crypto_init() != LT_OK) {
        return LT_CRYPTO_ERR;
    }

    // Initialize the hash operation
    *ctx = psa_hash_operation_init();
    return LT_OK;
}

lt_ret_t lt_sha256_start(LT_CRYPTO_SHA256_CTX_T *ctx)
{
    psa_status_t status;

    // Set up the hash operation for SHA-256
    status = psa_hash_setup(ctx, PSA_ALG_SHA_256);
    if (status != PSA_SUCCESS) {
        return LT_CRYPTO_ERR;
    }

    return LT_OK;
}

lt_ret_t lt_sha256_update(LT_CRYPTO_SHA256_CTX_T *ctx, const uint8_t *input, const size_t input_len)
{
    psa_status_t status;

    // Update the hash with input data
    status = psa_hash_update(ctx, input, input_len);
    if (status != PSA_SUCCESS) {
        return LT_CRYPTO_ERR;
    }

    return LT_OK;
}

lt_ret_t lt_sha256_finish(LT_CRYPTO_SHA256_CTX_T *ctx, uint8_t *output)
{
    psa_status_t status;
    size_t hash_length;

    // Finalize the hash and get the digest
    status = psa_hash_finish(ctx, output, 32, &hash_length);
    if (status != PSA_SUCCESS) {
        return LT_CRYPTO_ERR;
    }

    return LT_OK;
}
