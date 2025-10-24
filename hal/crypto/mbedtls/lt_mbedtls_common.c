#include <stdbool.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"
#include "psa/crypto.h"
#pragma GCC diagnostic pop
#include "lt_mbedtls_common.h"
#include "libtropic_logging.h"

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