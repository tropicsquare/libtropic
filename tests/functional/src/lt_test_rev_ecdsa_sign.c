/**
 * @file lt_test_rev_ecdsa_sign.c
 * @brief Tests ECDSA_Sign command.
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <inttypes.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "lt_l3_api_structs.h"
#include "lt_port_wrap.h"
#include "lt_sha256.h"
#include "lt_test_common.h"
#include "string.h"
#include "uECC.h"

#define MSG_TO_SIGN_LEN_MAX 4096

// Pre-generated key for testing using OpenSSL
uint8_t priv_test_key[]
    = {0x5e, 0xc6, 0xf1, 0xef, 0x96, 0x1f, 0x69, 0xb5, 0xd4, 0x34, 0xe1, 0x50, 0x6e, 0xa1, 0xcc, 0x51,
       0x11, 0x91, 0x94, 0x65, 0x87, 0xcb, 0x36, 0x82, 0x24, 0x07, 0x70, 0x32, 0x10, 0x1d, 0x62, 0xd1};

// Shared with cleanup function
lt_handle_t *g_h;

static lt_ret_t lt_test_rev_ecdsa_sign_cleanup(void)
{
    lt_ret_t ret;
    uint8_t read_pub_key[TR01_CURVE_P256_PUBKEY_LEN];  // The read key can have 32B or 64B, depending on the used curve,
                                                       // and we work with both curves here, so let's use one buffer for
                                                       // both for simplification and assume the size of pubkey on the
                                                       // P256 curve to be safe.
    lt_ecc_curve_type_t curve;
    lt_ecc_key_origin_t origin;

    LT_LOG_INFO("Starting secure session with slot %d", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    ret = lt_verify_chip_and_start_secure_session(g_h, LT_TEST_SH0_PRIV, LT_TEST_SH0_PUB,
                                                  TR01_PAIRING_KEY_SLOT_INDEX_0);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to establish secure session.");
        return ret;
    }

    LT_LOG_INFO("Erasing all ECC key slots");
    for (uint8_t i = TR01_ECC_SLOT_0; i <= TR01_ECC_SLOT_31; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Erasing slot #%" PRIu8, i);
        ret = lt_ecc_key_erase(g_h, i);
        if (LT_OK != ret) {
            LT_LOG_ERROR("Failed to erase slot.");
            return ret;
        }

        LT_LOG_INFO("Reading slot #%" PRIu8 " (should fail)", i);
        ret = lt_ecc_key_read(g_h, i, read_pub_key, sizeof(read_pub_key), &curve, &origin);
        if (LT_L3_INVALID_KEY != ret) {
            LT_LOG_ERROR("Return value is not LT_L3_INVALID_KEY.");
            return ret;
        }
    }

    LT_LOG_INFO("Aborting secure session");
    ret = lt_session_abort(g_h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to abort secure session.");
        return ret;
    }

    LT_LOG_INFO("Deinitializing handle");
    ret = lt_deinit(g_h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to deinitialize handle.");
        return ret;
    }

    return LT_OK;
}

void lt_test_rev_ecdsa_sign(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_rev_ecdsa_sign()");
    LT_LOG_INFO("----------------------------------------------");

    // Making the handle accessible to the cleanup function.
    g_h = h;

    uint8_t read_pub_key[TR01_CURVE_P256_PUBKEY_LEN], msg_to_sign[MSG_TO_SIGN_LEN_MAX],
        rs[TR01_ECDSA_EDDSA_SIGNATURE_LENGTH];
    lt_ecc_curve_type_t curve;
    lt_ecc_key_origin_t origin;
    uint32_t msg_to_sign_len;
    uint8_t msg_hash[LT_SHA256_DIGEST_LENGTH] = {0};

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    LT_LOG_INFO("Starting Secure Session with key %d", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    LT_TEST_ASSERT(LT_OK, lt_verify_chip_and_start_secure_session(h, LT_TEST_SH0_PRIV, LT_TEST_SH0_PUB,
                                                                  TR01_PAIRING_KEY_SLOT_INDEX_0));
    LT_LOG_LINE();

    lt_test_cleanup_function = &lt_test_rev_ecdsa_sign_cleanup;

    LT_LOG_INFO("Test ECDSA_Sign with stored key...");
    for (uint8_t i = TR01_ECC_SLOT_0; i <= TR01_ECC_SLOT_31; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Testing signing with ECC key slot #%" PRIu8 "...", i);

        LT_LOG_INFO("Generating random message length <= %d...", MSG_TO_SIGN_LEN_MAX);
        LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, &msg_to_sign_len, sizeof(msg_to_sign_len)));
        msg_to_sign_len %= MSG_TO_SIGN_LEN_MAX + 1;  // 0-MSG_TO_SIGN_LEN_MAX

        LT_LOG_INFO("Generating random message with length %" PRIu32 " for signing...", msg_to_sign_len);
        LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, msg_to_sign, msg_to_sign_len));

        LT_LOG_INFO("Signing message with empty slot (should fail)...");
        LT_TEST_ASSERT(LT_L3_INVALID_KEY, lt_ecc_ecdsa_sign(h, i, msg_to_sign, msg_to_sign_len, rs));

        LT_LOG_INFO("Storing private key pre-generated using P256 curve...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_key_store(h, i, TR01_CURVE_P256, priv_test_key));

        LT_LOG_INFO("Reading the stored public key...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_key_read(h, i, read_pub_key, sizeof(read_pub_key), &curve, &origin));

        LT_LOG_INFO("Signing message...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_ecdsa_sign(h, i, msg_to_sign, msg_to_sign_len, rs));

        LT_LOG_INFO("Calculating hash of the message before verifying the signature...");
        LT_TEST_ASSERT(LT_OK, lt_sha256_init(h->l3.crypto_ctx));
        LT_TEST_ASSERT(LT_OK, lt_sha256_start(h->l3.crypto_ctx));
        LT_TEST_ASSERT(LT_OK, lt_sha256_update(h->l3.crypto_ctx, msg_to_sign, msg_to_sign_len));
        LT_TEST_ASSERT(LT_OK, lt_sha256_finish(h->l3.crypto_ctx, msg_hash));
        LT_TEST_ASSERT(LT_OK, lt_sha256_deinit(h->l3.crypto_ctx));

        LT_LOG_INFO("Verifying signature...");
        LT_TEST_ASSERT(1, uECC_verify(read_pub_key, msg_hash, sizeof(msg_hash), rs, uECC_secp256r1()));

        LT_LOG_INFO("Erasing the slot...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_key_erase(h, i));

        LT_LOG_INFO("Signing message with erased slot (should fail)...");
        LT_TEST_ASSERT(LT_L3_INVALID_KEY, lt_ecc_ecdsa_sign(h, i, msg_to_sign, msg_to_sign_len, rs));
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Test ECDSA_Sign with generated key...");
    for (uint8_t i = TR01_ECC_SLOT_0; i <= TR01_ECC_SLOT_31; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Testing signing with ECC key slot #%" PRIu8 "...", i);

        LT_LOG_INFO("Generating random message length <= %d...", MSG_TO_SIGN_LEN_MAX);
        LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, &msg_to_sign_len, sizeof(msg_to_sign_len)));
        msg_to_sign_len %= MSG_TO_SIGN_LEN_MAX + 1;  // 0-MSG_TO_SIGN_LEN_MAX

        LT_LOG_INFO("Generating random message with length %" PRIu32 " for signing...", msg_to_sign_len);
        LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, msg_to_sign, msg_to_sign_len));

        LT_LOG_INFO("Signing message with empty slot (should fail)...");
        LT_TEST_ASSERT(LT_L3_INVALID_KEY, lt_ecc_ecdsa_sign(h, i, msg_to_sign, msg_to_sign_len, rs));

        LT_LOG_INFO("Generating private key using P256 curve...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_key_generate(h, i, TR01_CURVE_P256));

        LT_LOG_INFO("Reading the generated public key...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_key_read(h, i, read_pub_key, sizeof(read_pub_key), &curve, &origin));

        LT_LOG_INFO("Signing message...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_ecdsa_sign(h, i, msg_to_sign, msg_to_sign_len, rs));

        LT_LOG_INFO("Calculating hash of the message before verifying the signature...");
        LT_TEST_ASSERT(LT_OK, lt_sha256_init(h->l3.crypto_ctx));
        LT_TEST_ASSERT(LT_OK, lt_sha256_start(h->l3.crypto_ctx));
        LT_TEST_ASSERT(LT_OK, lt_sha256_update(h->l3.crypto_ctx, msg_to_sign, msg_to_sign_len));
        LT_TEST_ASSERT(LT_OK, lt_sha256_finish(h->l3.crypto_ctx, msg_hash));
        LT_TEST_ASSERT(LT_OK, lt_sha256_deinit(h->l3.crypto_ctx));

        LT_LOG_INFO("Verifying signature...");
        LT_TEST_ASSERT(1, uECC_verify(read_pub_key, msg_hash, sizeof(msg_hash), rs, uECC_secp256r1()));

        LT_LOG_INFO("Erasing the slot...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_key_erase(h, i));

        LT_LOG_INFO("Signing message with erased slot (should fail)...");
        LT_TEST_ASSERT(LT_L3_INVALID_KEY, lt_ecc_ecdsa_sign(h, i, msg_to_sign, msg_to_sign_len, rs));
    }
    LT_LOG_LINE();

    // Cleanup not needed anymore, all slots were erased
    lt_test_cleanup_function = NULL;

    LT_LOG_INFO("Aborting Secure Session");
    LT_TEST_ASSERT(LT_OK, lt_session_abort(h));

    LT_LOG_INFO("Deinitializing handle");
    LT_TEST_ASSERT(LT_OK, lt_deinit(h));
}