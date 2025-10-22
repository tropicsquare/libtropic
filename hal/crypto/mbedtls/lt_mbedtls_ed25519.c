/**
 * @file lt_mbedtls_ed25519.c
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
#include "lt_ed25519.h"

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

lt_ret_t lt_ed25519_sign_verify(const uint8_t *msg, const uint16_t msg_len, const uint8_t *pubkey, const uint8_t *rs)
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;

    // Ensure PSA Crypto is initialized
    if (ensure_psa_crypto_init() != LT_OK) {
        return LT_CRYPTO_ERR;
    }

    // Set up key attributes for Ed25519 public key
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_VERIFY_MESSAGE);
    psa_set_key_algorithm(&attributes, PSA_ALG_PURE_EDDSA);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_TWISTED_EDWARDS));
    psa_set_key_bits(&attributes, 255);

    // Import the public key (32 bytes)
    status = psa_import_key(&attributes, pubkey, 32, &key_id);
    psa_reset_key_attributes(&attributes);

    if (status != PSA_SUCCESS) {
        return LT_CRYPTO_ERR;
    }

    // Verify the Ed25519 signature (64 bytes)
    status = psa_verify_message(key_id, PSA_ALG_PURE_EDDSA, msg, msg_len, rs, 64);

    // Clean up
    psa_destroy_key(key_id);

    if (status != PSA_SUCCESS) {
        return LT_CRYPTO_ERR;
    }

    return LT_OK;
}
