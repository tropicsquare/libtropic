/**
 * @file lt_test_rev_ecc_key_store.c
 * @brief Test ECC_Key_Store command, along with ECC_Key_Read and ECC_Key_Erase.
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <inttypes.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "lt_test_common.h"
#include "string.h"

// Pre-generated keys for testing using OpenSSL
uint8_t p256_priv_test_key[]
    = {0x7d, 0x35, 0xc5, 0x0a, 0xfe, 0x9b, 0x15, 0xeb, 0x41, 0x16, 0xcb, 0x9b, 0xaa, 0xc2, 0xcb, 0xdd,
       0xbb, 0xdc, 0xb9, 0xb8, 0x77, 0xc7, 0x0f, 0x9e, 0x21, 0x8c, 0x2c, 0xff, 0xaa, 0x8b, 0x6f, 0x72};
uint8_t p256_pub_test_key[]
    = {0x62, 0x4e, 0xeb, 0x9d, 0x01, 0x82, 0x24, 0xdd, 0x1f, 0x2a, 0xbb, 0xdc, 0x0f, 0x8f, 0xca, 0xa3,
       0xc8, 0x9c, 0x2f, 0x9a, 0x46, 0x11, 0x73, 0x8b, 0x5f, 0xcb, 0xc5, 0x5b, 0xdb, 0x51, 0x93, 0xd7,
       0x2f, 0x2e, 0x48, 0x56, 0x1b, 0x97, 0x51, 0x16, 0xc4, 0x26, 0x6e, 0x50, 0x64, 0x30, 0xbc, 0x40,
       0xbf, 0x11, 0xb5, 0xc7, 0x51, 0x8c, 0xac, 0x64, 0xb2, 0x4c, 0xc3, 0x8b, 0x80, 0x4d, 0xa5, 0x1b};
uint8_t ed25519_priv_test_key[]
    = {0x73, 0x5b, 0x09, 0xb9, 0x5f, 0x4e, 0x17, 0x83, 0x4f, 0xa0, 0x7e, 0x93, 0x14, 0xa8, 0x7b, 0xa8,
       0x86, 0x36, 0x00, 0x30, 0x7f, 0x90, 0xf2, 0x3d, 0x52, 0x4c, 0xac, 0x15, 0x5f, 0x94, 0x44, 0xe0};
uint8_t ed25519_pub_test_key[]
    = {0xde, 0x86, 0x1d, 0xac, 0xc2, 0x36, 0x4a, 0xe0, 0x5f, 0xb4, 0xef, 0x3c, 0xfc, 0xc1, 0xb2, 0x41,
       0xab, 0x51, 0xdb, 0xc6, 0x38, 0xfc, 0x84, 0xb2, 0x5f, 0x04, 0xe6, 0x58, 0x5a, 0xd5, 0x3c, 0xcd};

// Invalid key can be checked only for P256
uint8_t p256_invalid_priv_test_key[]
    = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

// Shared with cleanup function
lt_handle_t *g_h;

static lt_ret_t lt_test_rev_ecc_key_store_cleanup(void)
{
    lt_ret_t ret;
    uint8_t read_pub_key[TR01_CURVE_P256_PUBKEY_LEN];  // The read key can have 32B or 64B, depending on the used curve,
                                                       // but we don't know what is stored in the slot, so to be safe,
                                                       // let's assume the size of pubkey on the P256 curve.
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

void lt_test_rev_ecc_key_store(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_rev_ecc_key_store()");
    LT_LOG_INFO("----------------------------------------------");

    // Making the handle accessible to the cleanup function.
    g_h = h;

    uint8_t read_pub_key[TR01_CURVE_P256_PUBKEY_LEN];  // The read key can have 32B or 64B, depending on the used curve,
                                                       // and we work with both curves here, so let's use one buffer for
                                                       // both for simplification and assume the size of pubkey on the
                                                       // P256 curve to be safe.
    lt_ecc_curve_type_t curve;
    lt_ecc_key_origin_t origin;

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    LT_LOG_INFO("Starting Secure Session with key %d", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    LT_TEST_ASSERT(LT_OK, lt_verify_chip_and_start_secure_session(h, LT_TEST_SH0_PRIV, LT_TEST_SH0_PUB,
                                                                  TR01_PAIRING_KEY_SLOT_INDEX_0));
    LT_LOG_LINE();

    lt_test_cleanup_function = &lt_test_rev_ecc_key_store_cleanup;

    LT_LOG_INFO("Testing ECC_Key_Store using P256 curve...");
    for (uint8_t i = TR01_ECC_SLOT_0; i <= TR01_ECC_SLOT_31; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Testing ECC key slot #%" PRIu8 "...", i);

        LT_LOG_INFO("Checking if slot is empty...");
        LT_TEST_ASSERT(LT_L3_INVALID_KEY, lt_ecc_key_read(h, i, read_pub_key, sizeof(read_pub_key), &curve, &origin));

        LT_LOG_INFO("Storing invalid private key (should fail)...");
        LT_TEST_ASSERT(LT_L3_FAIL, lt_ecc_key_store(h, i, TR01_CURVE_P256, p256_invalid_priv_test_key));

        LT_LOG_INFO("Storing private key pre-generated using P256 curve...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_key_store(h, i, TR01_CURVE_P256, p256_priv_test_key));

        LT_LOG_INFO("Storing private key pre-generated using P256 curve again (should fail)...");
        LT_TEST_ASSERT(LT_L3_FAIL, lt_ecc_key_store(h, i, TR01_CURVE_P256, p256_priv_test_key));

        LT_LOG_INFO("Storing private key pre-generated using Ed25519 curve (should fail)...");
        LT_TEST_ASSERT(LT_L3_FAIL, lt_ecc_key_store(h, i, TR01_CURVE_ED25519, ed25519_priv_test_key));

        LT_LOG_INFO("Reading the stored public key...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_key_read(h, i, read_pub_key, sizeof(read_pub_key), &curve, &origin));

        LT_LOG_INFO("Checking curve type of the read key...");
        LT_TEST_ASSERT(1, (curve == TR01_CURVE_P256));

        LT_LOG_INFO("Checking origin of the read key...");
        LT_TEST_ASSERT(1, (origin == TR01_CURVE_STORED));

        LT_LOG_INFO("Comparing the public key to the pre-generated one...");
        LT_TEST_ASSERT(0, memcmp(p256_pub_test_key, read_pub_key, sizeof(p256_pub_test_key)));

        LT_LOG_INFO("Erasing the slot...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_key_erase(h, i));
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Testing ECC_Key_Store using Ed25519 curve...");
    for (uint8_t i = TR01_ECC_SLOT_0; i <= TR01_ECC_SLOT_31; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Testing ECC key slot #%" PRIu8 "...", i);

        LT_LOG_INFO("Checking if slot is empty...");
        LT_TEST_ASSERT(LT_L3_INVALID_KEY, lt_ecc_key_read(h, i, read_pub_key, sizeof(read_pub_key), &curve, &origin));

        LT_LOG_INFO("Storing private key pre-generated using Ed25519 curve...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_key_store(h, i, TR01_CURVE_ED25519, ed25519_priv_test_key));

        LT_LOG_INFO("Storing private key pre-generated using Ed25519 curve again (should fail)...");
        LT_TEST_ASSERT(LT_L3_FAIL, lt_ecc_key_store(h, i, TR01_CURVE_ED25519, ed25519_priv_test_key));

        LT_LOG_INFO("Storing private key pre-generated using P256 curve (should fail)...");
        LT_TEST_ASSERT(LT_L3_FAIL, lt_ecc_key_store(h, i, TR01_CURVE_P256, p256_priv_test_key));

        LT_LOG_INFO("Reading the stored public key...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_key_read(h, i, read_pub_key, sizeof(read_pub_key), &curve, &origin));

        LT_LOG_INFO("Checking curve type of the read key...");
        LT_TEST_ASSERT(1, (curve == TR01_CURVE_ED25519));

        LT_LOG_INFO("Checking origin of the read key...");
        LT_TEST_ASSERT(1, (origin == TR01_CURVE_STORED));

        LT_LOG_INFO("Comparing the public key to the pre-generated one...");
        LT_TEST_ASSERT(0, memcmp(ed25519_pub_test_key, read_pub_key, sizeof(ed25519_pub_test_key)));

        LT_LOG_INFO("Erasing the slot...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_key_erase(h, i));

        LT_LOG_INFO("Trying to read the erased slot (should fail)...");
        LT_TEST_ASSERT(LT_L3_INVALID_KEY, lt_ecc_key_read(h, i, read_pub_key, sizeof(read_pub_key), &curve, &origin));
    }
    LT_LOG_LINE();

    // Cleanup not needed anymore, all slots were erased
    lt_test_cleanup_function = NULL;

    LT_LOG_INFO("Aborting Secure Session");
    LT_TEST_ASSERT(LT_OK, lt_session_abort(h));

    LT_LOG_INFO("Deinitializing handle");
    LT_TEST_ASSERT(LT_OK, lt_deinit(h));
}