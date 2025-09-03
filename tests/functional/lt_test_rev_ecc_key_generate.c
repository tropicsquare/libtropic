/**
 * @file lt_test_rev_ecc_key_generate.c
 * @brief Test ECC_Key_Generate command, along with ECC_Key_Read and ECC_Key_Erase.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <inttypes.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "string.h"

// Shared with cleanup function
lt_handle_t *g_h;

static lt_ret_t lt_test_rev_ecc_key_generate_cleanup(void)
{
    lt_ret_t ret;
    uint8_t read_pub_key[64];
    lt_ecc_curve_type_t curve;
    lt_ecc_key_origin_t origin;

    LT_LOG_INFO("Starting secure session with slot %d", (int)PAIRING_KEY_SLOT_INDEX_0);
    ret = lt_verify_chip_and_start_secure_session(g_h, sh0priv, sh0pub, PAIRING_KEY_SLOT_INDEX_0);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to establish secure session.");
        return ret;
    }

    LT_LOG_INFO("Erasing all ECC key slots");
    for (uint8_t i = ECC_SLOT_0; i <= ECC_SLOT_31; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Erasing slot #%" PRIu8, i);
        ret = lt_ecc_key_erase(g_h, i);
        if (LT_OK != ret) {
            LT_LOG_ERROR("Failed to erase slot.");
            return ret;
        }

        LT_LOG_INFO("Reading slot #%" PRIu8 " (should fail)", i);
        ret = lt_ecc_key_read(g_h, i, read_pub_key, &curve, &origin);
        if (LT_L3_ECC_INVALID_KEY != ret) {
            LT_LOG_ERROR("Return value is not LT_L3_ECC_INVALID_KEY.");
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

void lt_test_rev_ecc_key_generate(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_rev_ecc_key_generate()");
    LT_LOG_INFO("----------------------------------------------");

    // Making the handle accessible to the cleanup function.
    g_h = h;

    uint8_t read_pub_key[64];
    lt_ecc_curve_type_t curve;
    lt_ecc_key_origin_t origin;

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    LT_LOG_INFO("Starting Secure Session with key %d", (int)PAIRING_KEY_SLOT_INDEX_0);
    LT_TEST_ASSERT(LT_OK, lt_verify_chip_and_start_secure_session(h, sh0priv, sh0pub, PAIRING_KEY_SLOT_INDEX_0));
    LT_LOG_LINE();

    lt_test_cleanup_function = &lt_test_rev_ecc_key_generate_cleanup;

    LT_LOG_INFO("Testing ECC_Key_Generate using P256 curve...");
    for (uint8_t i = ECC_SLOT_0; i <= ECC_SLOT_31; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Testing ECC key slot #%" PRIu8 "...", i);

        LT_LOG_INFO("Checking if slot is empty...");
        LT_TEST_ASSERT(LT_L3_ECC_INVALID_KEY, lt_ecc_key_read(h, i, read_pub_key, &curve, &origin));

        LT_LOG_INFO("Generating private key using P256 curve...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_key_generate(h, i, CURVE_P256));

        LT_LOG_INFO("Generating private key using P256 curve again (should fail)...");
        LT_TEST_ASSERT(LT_L3_FAIL, lt_ecc_key_generate(h, i, CURVE_P256));

        LT_LOG_INFO("Generating private key using Ed25519 curve (should fail)...");
        LT_TEST_ASSERT(LT_L3_FAIL, lt_ecc_key_generate(h, i, CURVE_ED25519));

        LT_LOG_INFO("Reading the generated public key...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_key_read(h, i, read_pub_key, &curve, &origin));

        LT_LOG_INFO("Checking curve type of the read key...");
        LT_TEST_ASSERT(1, (curve == CURVE_P256));

        LT_LOG_INFO("Checking origin of the read key...");
        LT_TEST_ASSERT(1, (origin == CURVE_GENERATED));

        LT_LOG_INFO("Erasing the slot...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_key_erase(h, i));
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Testing ECC_Key_Generate using Ed25519 curve...");
    for (uint8_t i = ECC_SLOT_0; i <= ECC_SLOT_31; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Testing ECC key slot #%" PRIu8 "...", i);

        LT_LOG_INFO("Checking if slot is empty...");
        LT_TEST_ASSERT(LT_L3_ECC_INVALID_KEY, lt_ecc_key_read(h, i, read_pub_key, &curve, &origin));

        LT_LOG_INFO("Generating private key using Ed25519 curve...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_key_generate(h, i, CURVE_ED25519));

        LT_LOG_INFO("Generating private key using Ed25519 curve again (should fail)...");
        LT_TEST_ASSERT(LT_L3_FAIL, lt_ecc_key_generate(h, i, CURVE_ED25519));

        LT_LOG_INFO("Generating private key using P256 curve (should fail)...");
        LT_TEST_ASSERT(LT_L3_FAIL, lt_ecc_key_generate(h, i, CURVE_P256));

        LT_LOG_INFO("Reading the generated public key...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_key_read(h, i, read_pub_key, &curve, &origin));

        LT_LOG_INFO("Checking curve type of the read key...");
        LT_TEST_ASSERT(1, (curve == CURVE_ED25519));

        LT_LOG_INFO("Checking origin of the read key...");
        LT_TEST_ASSERT(1, (origin == CURVE_GENERATED));

        LT_LOG_INFO("Erasing the slot...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_key_erase(h, i));

        LT_LOG_INFO("Trying to read the erased slot (should fail)...");
        LT_TEST_ASSERT(LT_L3_ECC_INVALID_KEY, lt_ecc_key_read(h, i, read_pub_key, &curve, &origin));
    }
    LT_LOG_LINE();

    // Cleanup not needed anymore
    lt_test_cleanup_function = NULL;

    LT_LOG_INFO("Aborting Secure Session");
    LT_TEST_ASSERT(LT_OK, lt_session_abort(h));

    LT_LOG_INFO("Deinitializing handle");
    LT_TEST_ASSERT(LT_OK, lt_deinit(h));
}