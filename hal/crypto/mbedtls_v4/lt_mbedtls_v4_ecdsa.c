/**
 * @file lt_mbedtls_v4_ecdsa.c
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
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
#include "libtropic_logging.h"
#include "lt_ecdsa.h"
#include "lt_mbedtls_v4_common.h"

lt_ret_t lt_ecdsa_sign_verify(const uint8_t *msg, const uint32_t msg_len, const uint8_t *pubkey, const uint8_t *rs)
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    uint8_t hash[32];
    size_t hash_length;

    // Add correct prefix to public key (uncompressed format).
    // A Weierstrass public key is represented as:
    //  - prefix 0x04
    //  - X coordinate (32 bytes)
    //  - Y coordinate (32 bytes)
    uint8_t pubkey_with_prefix[65];
    pubkey_with_prefix[0] = 0x04;
    memcpy(&pubkey_with_prefix[1], pubkey, 64);

    // Ensure PSA Crypto is initialized
    if (lt_mbedtls_ensure_psa_crypto_init() != LT_OK) {
        LT_LOG_ERROR("PSA Crypto is not initialized!");
        return LT_CRYPTO_ERR;
    }

    // Compute SHA-256 hash of the message
    status = psa_hash_compute(PSA_ALG_SHA_256, msg, msg_len, hash, sizeof(hash), &hash_length);
    if (status != PSA_SUCCESS) {
        LT_LOG_ERROR("Couldn't compute SHA-256, status=%d (psa_status_t)", status);
        return LT_CRYPTO_ERR;
    }

    // Set up key attributes for ECDSA public key on SECP256R1
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&attributes, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
    psa_set_key_type(&attributes, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&attributes, 256);

    // Import the public key with prefix
    status = psa_import_key(&attributes, pubkey_with_prefix, sizeof(pubkey_with_prefix), &key_id);
    psa_reset_key_attributes(&attributes);

    if (status != PSA_SUCCESS) {
        LT_LOG_ERROR("Couldn't import public key, status=%d (psa_status_t)", status);
        return LT_CRYPTO_ERR;
    }

    // Verify the ECDSA signature (64 bytes: R and S)
    status = psa_verify_hash(key_id, PSA_ALG_ECDSA(PSA_ALG_SHA_256), hash, hash_length, rs, 64);

    // Clean up
    psa_destroy_key(key_id);

    if (status != PSA_SUCCESS) {
        LT_LOG_ERROR("Signature verification failed, status=%d (psa_status_t)", status);
        return LT_CRYPTO_ERR;
    }

    return LT_OK;
}
