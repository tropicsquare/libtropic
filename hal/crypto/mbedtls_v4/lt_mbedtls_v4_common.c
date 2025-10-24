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
#include "lt_mbedtls_v4_common.h"

bool lt_mbedtls_psa_crypto_initialized = false;

lt_ret_t lt_mbedtls_ensure_psa_crypto_init(void)
{
    if (!lt_mbedtls_psa_crypto_initialized) {
        psa_status_t status = psa_crypto_init();
        if (status != PSA_SUCCESS) {
            LT_LOG_ERROR("PSA Crypto initialization failed, status=%d (psa_status_t)", status);
            return LT_CRYPTO_ERR;
        }
        lt_mbedtls_psa_crypto_initialized = 1;
    }
    return LT_OK;
}