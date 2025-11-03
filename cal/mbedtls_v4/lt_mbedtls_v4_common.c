/**
 * @file lt_mbedtls_v4_common.c
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdbool.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"
#include "psa/crypto.h"
#pragma GCC diagnostic pop
#include "libtropic_logging.h"
#include "libtropic_mbedtls_v4.h"
#include "lt_aesgcm.h"
#include "lt_crypto_common.h"
#include "lt_mbedtls_v4_common.h"

bool lt_mbedtls_psa_crypto_initialized = false;

lt_ret_t lt_crypto_ctx_init(void *ctx)
{
    lt_ctx_mbedtls_v4_t *_ctx = (lt_ctx_mbedtls_v4_t *)ctx;

    _ctx->aesgcm_encrypt_ctx.key_set = 0;
    _ctx->aesgcm_decrypt_ctx.key_set = 0;

    if (!lt_mbedtls_psa_crypto_initialized) {
        psa_status_t status = psa_crypto_init();
        if (status != PSA_SUCCESS) {
            LT_LOG_ERROR("PSA Crypto initialization failed, status=%d (psa_status_t)", status);
            return LT_CRYPTO_ERR;
        }
        lt_mbedtls_psa_crypto_initialized = true;
    }

    return LT_OK;
}

lt_ret_t lt_crypto_ctx_deinit(void *ctx)
{
    lt_ret_t ret1 = lt_aesgcm_encrypt_deinit(ctx);
    lt_ret_t ret2 = lt_aesgcm_decrypt_deinit(ctx);

    if (ret1 != LT_OK) {
        return ret1;
    }
    if (ret2 != LT_OK) {
        return ret2;
    }

    return LT_OK;
}