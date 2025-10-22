/**
 * @file lt_mbedtls_ed25519.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdint.h>

/* MbedTLS 3.x introduced the PSA Crypto API as the primary interface for Ed25519.
 * For compatibility with both MbedTLS 2.x and 3.x, we use the appropriate headers.
 * In MbedTLS 3.x, Ed25519 is typically accessed via PSA Crypto API.
 * In MbedTLS 2.x, it was in the deprecated pk module or not supported directly.
 * 
 * This implementation assumes MbedTLS 3.x with PSA Crypto API support for Ed25519.
 * If using MbedTLS 2.x, alternative implementations or third-party libraries may be needed.
 */

#if defined(MBEDTLS_PSA_CRYPTO_C)
#include "psa/crypto.h"
#endif

#include "libtropic_common.h"
#include "lt_ed25519.h"

lt_ret_t lt_ed25519_sign_verify(const uint8_t *msg, const uint16_t msg_len, const uint8_t *pubkey, const uint8_t *rs)
{
#if defined(MBEDTLS_PSA_CRYPTO_C)
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;

    // Initialize PSA Crypto if not already initialized
    static int psa_initialized = 0;
    if (!psa_initialized) {
        status = psa_crypto_init();
        if (status != PSA_SUCCESS) {
            return LT_CRYPTO_ERR;
        }
        psa_initialized = 1;
    }

    // Set up key attributes for Ed25519 public key
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_VERIFY_MESSAGE);
    psa_set_key_algorithm(&attributes, PSA_ALG_PURE_EDDSA);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_TWISTED_EDWARDS));
    psa_set_key_bits(&attributes, 255);

    // Import the public key
    status = psa_import_key(&attributes, pubkey, 32, &key_id);
    if (status != PSA_SUCCESS) {
        psa_reset_key_attributes(&attributes);
        return LT_CRYPTO_ERR;
    }

    // Verify the signature (rs is the signature, 64 bytes for Ed25519)
    status = psa_verify_message(key_id, PSA_ALG_PURE_EDDSA, msg, msg_len, rs, 64);

    // Clean up
    psa_destroy_key(key_id);
    psa_reset_key_attributes(&attributes);

    if (status != PSA_SUCCESS) {
        return LT_CRYPTO_ERR;
    }
    return LT_OK;
#else
    // Ed25519 not supported without PSA Crypto API
    // This would require using an external library like libsodium or ed25519-donna
    (void)msg;
    (void)msg_len;
    (void)pubkey;
    (void)rs;
    return LT_CRYPTO_ERR;
#endif
}
