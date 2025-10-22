/**
 * @file lt_mbedtls_x25519.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdint.h>
#include <string.h>

#include "psa/crypto.h"
#include "libtropic_common.h"
#include "lt_x25519.h"

/* PSA Crypto initialization state */
static uint8_t psa_crypto_initialized = 0;

/* Initialize PSA Crypto library if not already done */
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

lt_ret_t lt_X25519(const uint8_t *privkey, const uint8_t *pubkey, uint8_t *secret)
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    size_t secret_length;

    /* Ensure PSA Crypto is initialized */
    if (ensure_psa_crypto_init() != LT_OK) {
        return LT_CRYPTO_ERR;
    }

    /* Set up key attributes for X25519 private key */
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DERIVE);
    psa_set_key_algorithm(&attributes, PSA_ALG_ECDH);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_MONTGOMERY));
    psa_set_key_bits(&attributes, 255);

    /* Import the private key (32 bytes) */
    status = psa_import_key(&attributes, privkey, 32, &key_id);
    psa_reset_key_attributes(&attributes);

    if (status != PSA_SUCCESS) {
        return LT_CRYPTO_ERR;
    }

    /* Perform X25519 key agreement to compute shared secret */
    status = psa_raw_key_agreement(PSA_ALG_ECDH, key_id, pubkey, 32, secret, 32, &secret_length);

    /* Clean up */
    psa_destroy_key(key_id);

    if (status != PSA_SUCCESS || secret_length != 32) {
        return LT_CRYPTO_ERR;
    }

    return LT_OK;
}

lt_ret_t lt_X25519_scalarmult(const uint8_t *sk, uint8_t *pk)
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    size_t pubkey_length;

    /* Ensure PSA Crypto is initialized */
    if (ensure_psa_crypto_init() != LT_OK) {
        return LT_CRYPTO_ERR;
    }

    /* Set up key attributes for X25519 private key */
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_EXPORT);
    psa_set_key_algorithm(&attributes, PSA_ALG_ECDH);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_MONTGOMERY));
    psa_set_key_bits(&attributes, 255);

    /* Import the private key (32 bytes) */
    status = psa_import_key(&attributes, sk, 32, &key_id);
    psa_reset_key_attributes(&attributes);

    if (status != PSA_SUCCESS) {
        return LT_CRYPTO_ERR;
    }

    /* Export the public key (scalar multiplication with base point) */
    status = psa_export_public_key(key_id, pk, 32, &pubkey_length);

    /* Clean up */
    psa_destroy_key(key_id);

    if (status != PSA_SUCCESS || pubkey_length != 32) {
        return LT_CRYPTO_ERR;
    }

    return LT_OK;
}
