/**
 * @file lt_test_rev_ecdsa_sign.c
 * @brief Tests ECDSA_Sign command.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <inttypes.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "libtropic_port.h"
#include "lt_l3_api_structs.h"
#include "string.h"

#define MSG_TO_SIGN_LEN_MAX 4096

// Pre-generated key for testing using OpenSSL
uint8_t priv_test_key[]
    = {0x5e, 0xc6, 0xf1, 0xef, 0x96, 0x1f, 0x69, 0xb5, 0xd4, 0x34, 0xe1, 0x50, 0x6e, 0xa1, 0xcc, 0x51,
       0x11, 0x91, 0x94, 0x65, 0x87, 0xcb, 0x36, 0x82, 0x24, 0x07, 0x70, 0x32, 0x10, 0x1d, 0x62, 0xd1};

// Shared with cleanup function
lt_handle_t *lt_h;

lt_ret_t lt_test_rev_ecdsa_sign_cleanup(void)
{
    lt_ret_t ret;
    uint8_t read_pub_key[64];
    lt_ecc_curve_type_t curve;
    ecc_key_origin_t origin;

    LT_LOG_INFO("Starting secure session with slot %d", (int)PAIRING_KEY_SLOT_INDEX_0);
    ret = verify_chip_and_start_secure_session(lt_h, sh0priv, sh0pub, PAIRING_KEY_SLOT_INDEX_0);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to establish secure session.");
        return ret;
    }

    LT_LOG_INFO("Erasing all ECC key slots");
    for (uint8_t i = ECC_SLOT_0; i <= ECC_SLOT_31; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Erasing slot #%" PRIu8, i);
        ret = lt_ecc_key_erase(lt_h, i);
        if (LT_OK != ret) {
            LT_LOG_ERROR("Failed to erase slot.");
            return ret;
        }

        LT_LOG_INFO("Reading slot #%" PRIu8 " (should fail)", i);
        ret = lt_ecc_key_read(lt_h, i, read_pub_key, &curve, &origin);
        if (LT_L3_ECC_INVALID_KEY != ret) {
            LT_LOG_ERROR("Return value is not LT_L3_ECC_INVALID_KEY.");
            return ret;
        }
    }

    LT_LOG_INFO("Aborting secure session");
    ret = lt_session_abort(lt_h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to abort secure session.");
        return ret;
    }

    LT_LOG_INFO("Deinitializing handle");
    ret = lt_deinit(lt_h);
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

    lt_h = h;

    uint8_t read_pub_key[64], msg_to_sign[MSG_TO_SIGN_LEN_MAX], rs[64];
    lt_ecc_curve_type_t curve;
    ecc_key_origin_t origin;
    uint32_t random_data[MSG_TO_SIGN_LEN_MAX / sizeof(uint32_t)], random_data_size;

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    LT_LOG_INFO("Starting Secure Session with key %d", (int)PAIRING_KEY_SLOT_INDEX_0);
    LT_TEST_ASSERT(LT_OK, verify_chip_and_start_secure_session(h, sh0priv, sh0pub, PAIRING_KEY_SLOT_INDEX_0));
    LT_LOG_LINE();

    lt_test_cleanup_function = &lt_test_rev_ecdsa_sign_cleanup;

    LT_LOG_INFO("Test ECDSA_Sign with stored key...");
    for (uint8_t i = ECC_SLOT_0; i <= ECC_SLOT_31; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Testing signing with ECC key slot #%" PRIu8 "...", i);

        LT_LOG_INFO("Generating random data length <= %d...", MSG_TO_SIGN_LEN_MAX);
        LT_TEST_ASSERT(LT_OK, lt_port_random_bytes(&random_data_size, 1));
        random_data_size %= MSG_TO_SIGN_LEN_MAX + 1;  // 0-MSG_TO_SIGN_LEN_MAX

        LT_LOG_INFO("Generating random message with length %" PRIu32 " for signing...", random_data_size);
        LT_TEST_ASSERT(LT_OK, lt_port_random_bytes(random_data, sizeof(random_data) / sizeof(uint32_t)));
        memcpy(msg_to_sign, random_data, random_data_size);

        LT_LOG_INFO("Signing message with empty slot (should fail)...");
        LT_TEST_ASSERT(LT_L3_ECC_INVALID_KEY, lt_ecc_ecdsa_sign(h, i, msg_to_sign, random_data_size, rs));

        LT_LOG_INFO("Storing private key pre-generated using P256 curve...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_key_store(h, i, CURVE_P256, priv_test_key));

        LT_LOG_INFO("Reading the stored public key...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_key_read(h, i, read_pub_key, &curve, &origin));

        LT_LOG_INFO("Signing message...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_ecdsa_sign(h, i, msg_to_sign, random_data_size, rs));

        LT_LOG_INFO("Verifying signature...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_ecdsa_sig_verify(msg_to_sign, random_data_size, read_pub_key, rs));

        LT_LOG_INFO("Erasing the slot...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_key_erase(h, i));

        LT_LOG_INFO("Signing message with erased slot (should fail)...");
        LT_TEST_ASSERT(LT_L3_ECC_INVALID_KEY, lt_ecc_ecdsa_sign(h, i, msg_to_sign, random_data_size, rs));
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Test ECDSA_Sign with generated key...");
    for (uint8_t i = ECC_SLOT_0; i <= ECC_SLOT_31; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Testing signing with ECC key slot #%" PRIu8 "...", i);

        LT_LOG_INFO("Generating random data length <= %d...", MSG_TO_SIGN_LEN_MAX);
        LT_TEST_ASSERT(LT_OK, lt_port_random_bytes(&random_data_size, 1));
        random_data_size %= MSG_TO_SIGN_LEN_MAX + 1;  // 0-MSG_TO_SIGN_LEN_MAX

        LT_LOG_INFO("Generating random message with length %" PRIu32 " for signing...", random_data_size);
        LT_TEST_ASSERT(LT_OK, lt_port_random_bytes(random_data, sizeof(random_data) / sizeof(uint32_t)));
        memcpy(msg_to_sign, random_data, random_data_size);

        LT_LOG_INFO("Signing message with empty slot (should fail)...");
        LT_TEST_ASSERT(LT_L3_ECC_INVALID_KEY, lt_ecc_ecdsa_sign(h, i, msg_to_sign, random_data_size, rs));

        LT_LOG_INFO("Generating private key using P256 curve...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_key_generate(h, i, CURVE_P256));

        LT_LOG_INFO("Reading the generated public key...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_key_read(h, i, read_pub_key, &curve, &origin));

        LT_LOG_INFO("Signing message...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_ecdsa_sign(h, i, msg_to_sign, random_data_size, rs));

        LT_LOG_INFO("Verifying signature...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_ecdsa_sig_verify(msg_to_sign, random_data_size, read_pub_key, rs));

        LT_LOG_INFO("Erasing the slot...");
        LT_TEST_ASSERT(LT_OK, lt_ecc_key_erase(h, i));

        LT_LOG_INFO("Signing message with erased slot (should fail)...");
        LT_TEST_ASSERT(LT_L3_ECC_INVALID_KEY, lt_ecc_ecdsa_sign(h, i, msg_to_sign, random_data_size, rs));
    }
    LT_LOG_LINE();

    // Cleanup not needed anymore, all slots were erased
    lt_test_cleanup_function = NULL;

    LT_LOG_INFO("Aborting Secure Session");
    LT_TEST_ASSERT(LT_OK, lt_session_abort(h));

    LT_LOG_INFO("Deinitializing handle");
    LT_TEST_ASSERT(LT_OK, lt_deinit(h));
}