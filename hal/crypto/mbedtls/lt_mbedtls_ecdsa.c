/**
 * @file lt_mbedtls_ecdsa.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdint.h>
#include <string.h>

#include "psa/crypto.h"
#include "libtropic_common.h"
#include "lt_ecdsa.h"

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

lt_ret_t lt_ecdsa_sign_verify(const uint8_t *msg, const uint32_t msg_len, const uint8_t *pubkey, const uint8_t *rs)
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    uint8_t hash[32];
    size_t hash_length;

    /* Ensure PSA Crypto is initialized */
    if (ensure_psa_crypto_init() != LT_OK) {
        return LT_CRYPTO_ERR;
    }

    /* Compute SHA-256 hash of the message */
    status = psa_hash_compute(PSA_ALG_SHA_256, msg, msg_len, hash, sizeof(hash), &hash_length);
    if (status != PSA_SUCCESS) {
        return LT_CRYPTO_ERR;
    }

    /* Set up key attributes for ECDSA public key on SECP256R1 */
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&attributes, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
    psa_set_key_type(&attributes, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&attributes, 256);

    /* Import the public key (64 bytes: X and Y coordinates) */
    status = psa_import_key(&attributes, pubkey, 64, &key_id);
    psa_reset_key_attributes(&attributes);

    if (status != PSA_SUCCESS) {
        return LT_CRYPTO_ERR;
    }

    /* Verify the ECDSA signature (64 bytes: R and S) */
    status = psa_verify_hash(key_id, PSA_ALG_ECDSA(PSA_ALG_SHA_256), hash, hash_length, rs, 64);

    /* Clean up */
    psa_destroy_key(key_id);

    if (status != PSA_SUCCESS) {
        return LT_CRYPTO_ERR;
    }

    return LT_OK;
}
