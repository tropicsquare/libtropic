/**
 * @file lt_mbedtls_v4_hmac_sha256.c
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdint.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"
#include "psa/crypto.h"
#pragma GCC diagnostic pop
#include "libtropic_common.h"
#include "libtropic_logging.h"
#include "lt_hmac_sha256.h"

lt_ret_t lt_hmac_sha256(const uint8_t *key, const uint32_t key_len, const uint8_t *input, const uint32_t input_len,
                        uint8_t *output)
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    size_t mac_length;

    // Set up key attributes for HMAC
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_MESSAGE);
    psa_set_key_algorithm(&attributes, PSA_ALG_HMAC(PSA_ALG_SHA_256));
    psa_set_key_type(&attributes, PSA_KEY_TYPE_HMAC);

    // Import the key
    status = psa_import_key(&attributes, key, key_len, &key_id);
    psa_reset_key_attributes(&attributes);

    if (status != PSA_SUCCESS) {
        LT_LOG_ERROR("Couldn't import HMAC key, status=%d (psa_status_t)", status);
        return LT_CRYPTO_ERR;
    }

    // Compute HMAC-SHA256
    status = psa_mac_compute(key_id, PSA_ALG_HMAC(PSA_ALG_SHA_256), input, input_len, output,
                             PSA_HASH_LENGTH(PSA_ALG_SHA_256), &mac_length);

    // Clean up
    psa_status_t destroy_key_status = psa_destroy_key(key_id);

    if (status != PSA_SUCCESS) {
        LT_LOG_ERROR("HMAC-SHA256 computation failed, status=%d (psa_status_t)", status);
        return LT_CRYPTO_ERR;
    }

    if (destroy_key_status != PSA_SUCCESS) {
        LT_LOG_ERROR("Couldn't destroy HMAC key, status=%d (psa_status_t)", destroy_key_status);
        return LT_CRYPTO_ERR;
    }

    return LT_OK;
}
