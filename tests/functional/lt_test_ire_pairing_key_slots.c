/**
 * @file lt_test_ire_pairing_key_slots.c
 * @brief Test Pairing_Key_Read, Pairing_Key_Write and Pairing_Key_Invalidate on all slots.
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

/** @brief Size of the print buffer. */
#define PRINT_BUFF_SIZE 65

/** @brief X25519 private key to execute a Secure Channel Handshake on Pairing Key slot 1. */
const uint8_t sh1priv[]
    = {0x58, 0xc4, 0x81, 0x88, 0xf8, 0xb1, 0xcb, 0xd4, 0x19, 0x00, 0x2e, 0x9c, 0x8d, 0xf8, 0xce, 0xea,
       0xf3, 0xa9, 0x11, 0xde, 0xb6, 0x6b, 0xc8, 0x87, 0xae, 0xe7, 0x88, 0x10, 0xfb, 0x48, 0xb6, 0x74};

/** @brief X25519 public key to execute a Secure Channel Handshake on Pairing Key slot 1. */
const uint8_t sh1pub[]
    = {0xe1, 0xdc, 0xf9, 0xc3, 0x46, 0xbc, 0xf2, 0xe7, 0x8b, 0xa8, 0xf0, 0x27, 0xd8, 0x0a, 0x8a, 0x33,
       0xcc, 0xf3, 0xe9, 0xdf, 0x6b, 0xdf, 0x65, 0xa2, 0xc1, 0xae, 0xc4, 0xd9, 0x21, 0xe1, 0x8d, 0x51};

/** @brief X25519 private key to execute a Secure Channel Handshake on Pairing Key slot 2. */
const uint8_t sh2priv[]
    = {0x00, 0x40, 0x5e, 0x19, 0x46, 0x75, 0xab, 0xe1, 0x5f, 0x0b, 0x57, 0xf2, 0x5b, 0x12, 0x86, 0x62,
       0xab, 0xb0, 0xe9, 0xc6, 0xa7, 0xc3, 0xca, 0xdf, 0x1c, 0xb1, 0xd2, 0xb7, 0xf8, 0xcf, 0x35, 0x47};

/** @brief X25519 public key to execute a Secure Channel Handshake on Pairing Key slot 2. */
const uint8_t sh2pub[]
    = {0x66, 0xb9, 0x92, 0x5a, 0x85, 0x66, 0xe8, 0x09, 0x5c, 0x56, 0x80, 0xfb, 0x22, 0xd4, 0xb8, 0x4b,
       0xf8, 0xe3, 0x12, 0xb2, 0x7c, 0x4b, 0xac, 0xce, 0x26, 0x3c, 0x78, 0x39, 0x6d, 0x4c, 0x16, 0x6c};

/** @brief X25519 private key to execute a Secure Channel Handshake on Pairing Key slot 3. */
const uint8_t sh3priv[]
    = {0xb0, 0x90, 0x9f, 0xe1, 0xf3, 0x1f, 0xa1, 0x21, 0x75, 0xef, 0x45, 0xb1, 0x42, 0xde, 0x0e, 0xdd,
       0xa1, 0xf4, 0x51, 0x01, 0x40, 0xc2, 0xe5, 0x2c, 0xf4, 0x68, 0xac, 0x96, 0xa1, 0x0e, 0xcb, 0x46};

/** @brief X25519 public key to execute a Secure Channel Handshake on Pairing Key slot 3. */
const uint8_t sh3pub[]
    = {0x22, 0x57, 0xa8, 0x2f, 0x85, 0x8f, 0x13, 0x32, 0xfa, 0x0f, 0xf6, 0x0c, 0x76, 0x29, 0x42, 0x70,
       0xa9, 0x58, 0x9d, 0xfd, 0x47, 0xa5, 0x23, 0x78, 0x18, 0x4d, 0x2d, 0x38, 0xf0, 0xa7, 0xc4, 0x01};

void lt_test_ire_pairing_key_slots(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_ire_pairing_key_slots()");
    LT_LOG_INFO("----------------------------------------------");

    const uint8_t *pub_keys[] = {LT_TEST_SH0_PUB, sh1pub, sh2pub, sh3pub};
    const uint8_t *priv_keys[] = {LT_TEST_SH0_PRIV, sh1priv, sh2priv, sh3priv};
    uint8_t read_key[TR01_SHIPUB_LEN] = {0};
    uint8_t zeros[TR01_SHIPUB_LEN] = {0};
    char print_buff[PRINT_BUFF_SIZE];

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    LT_LOG_INFO("Starting Secure Session with key %d", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    LT_TEST_ASSERT(LT_OK, lt_verify_chip_and_start_secure_session(h, LT_TEST_SH0_PRIV, LT_TEST_SH0_PUB,
                                                                  TR01_PAIRING_KEY_SLOT_INDEX_0));
    LT_LOG_LINE();

    // Read pairing keys (1,2,3 should be empty)
    LT_LOG_INFO("Reading pairing key slot %d...", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    LT_TEST_ASSERT(LT_OK, lt_pairing_key_read(h, read_key, TR01_PAIRING_KEY_SLOT_INDEX_0));
    LT_TEST_ASSERT(LT_OK, lt_print_bytes(read_key, sizeof(read_key), print_buff, PRINT_BUFF_SIZE));
    LT_LOG_INFO("%s", print_buff);
    LT_LOG_INFO();
    for (uint8_t i = TR01_PAIRING_KEY_SLOT_INDEX_1; i <= TR01_PAIRING_KEY_SLOT_INDEX_3; i++) {
        LT_LOG_INFO("Reading pairing key slot %" PRIu8 " (should fail)...", i);
        LT_TEST_ASSERT(LT_L3_SLOT_EMPTY, lt_pairing_key_read(h, read_key, i));
        LT_LOG_INFO();
    }
    LT_LOG_LINE();

    // Write pairing keys into slot 1,2,3
    for (uint8_t i = TR01_PAIRING_KEY_SLOT_INDEX_1; i <= TR01_PAIRING_KEY_SLOT_INDEX_3; i++) {
        LT_LOG_INFO("Writing to pairing key slot %" PRIu8 "...", i);
        LT_TEST_ASSERT(LT_OK, lt_print_bytes(pub_keys[i], TR01_SHIPUB_LEN, print_buff, PRINT_BUFF_SIZE));
        LT_LOG_INFO("%s", print_buff);
        LT_TEST_ASSERT(LT_OK, lt_pairing_key_write(h, pub_keys[i], i));
        LT_LOG_INFO();
    }
    LT_LOG_LINE();

    // Read all pairing keys and check value
    for (uint8_t i = TR01_PAIRING_KEY_SLOT_INDEX_0; i <= TR01_PAIRING_KEY_SLOT_INDEX_3; i++) {
        LT_LOG_INFO("Reading pairing key slot %" PRIu8 "...", i);
        LT_TEST_ASSERT(LT_OK, lt_pairing_key_read(h, read_key, i));
        LT_TEST_ASSERT(LT_OK, lt_print_bytes(read_key, sizeof(read_key), print_buff, PRINT_BUFF_SIZE));
        LT_LOG_INFO("%s", print_buff);

        LT_LOG_INFO("Comparing contents of written and read key...");
        LT_TEST_ASSERT(0, memcmp(pub_keys[i], read_key, TR01_SHIPUB_LEN));
        LT_LOG_INFO();
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Aborting Secure Session with slot %d...", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    LT_TEST_ASSERT(LT_OK, lt_session_abort(h));
    LT_LOG_LINE();

    // Test secure session with slots 1,2,3
    for (uint8_t i = TR01_PAIRING_KEY_SLOT_INDEX_1; i <= TR01_PAIRING_KEY_SLOT_INDEX_3; i++) {
        LT_LOG_INFO("Starting Secure Session with key %" PRIu8 "...", i);
        LT_TEST_ASSERT(LT_OK, lt_verify_chip_and_start_secure_session(h, priv_keys[i], pub_keys[i], i));

        LT_LOG_INFO("Aborting Secure Session with slot %" PRIu8 "...", i);
        LT_TEST_ASSERT(LT_OK, lt_session_abort(h));
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Starting Secure Session with key %d...", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    LT_TEST_ASSERT(LT_OK, lt_verify_chip_and_start_secure_session(h, LT_TEST_SH0_PRIV, LT_TEST_SH0_PUB,
                                                                  TR01_PAIRING_KEY_SLOT_INDEX_0));
    LT_LOG_LINE();

    // Write pairing key slots again (should fail)
    for (uint8_t i = TR01_PAIRING_KEY_SLOT_INDEX_0; i <= TR01_PAIRING_KEY_SLOT_INDEX_3; i++) {
        LT_LOG_INFO("Writing to pairing key slot %" PRIu8 " (should fail)...", i);
        LT_TEST_ASSERT(LT_L3_FAIL, lt_pairing_key_write(h, zeros, i));

        LT_LOG_INFO("Reading pairing key slot %" PRIu8 "...", i);
        LT_TEST_ASSERT(LT_OK, lt_pairing_key_read(h, read_key, i));
        LT_TEST_ASSERT(LT_OK, lt_print_bytes(read_key, sizeof(read_key), print_buff, PRINT_BUFF_SIZE));
        LT_LOG_INFO("%s", print_buff);

        LT_LOG_INFO("Comparing contents of expected key and read key...");
        LT_TEST_ASSERT(0, memcmp(pub_keys[i], read_key, sizeof(read_key)));
        LT_LOG_INFO();
    }
    LT_LOG_LINE();

    // Invalidate all slots, try reading and writing them
    for (uint8_t i = TR01_PAIRING_KEY_SLOT_INDEX_0; i <= TR01_PAIRING_KEY_SLOT_INDEX_3; i++) {
        LT_LOG_INFO("Invalidating pairing key slot %" PRIu8 "...", i);
        LT_TEST_ASSERT(LT_OK, lt_pairing_key_invalidate(h, i));

        LT_LOG_INFO("Reading pairing key slot %" PRIu8 " (should fail)...", i);
        LT_TEST_ASSERT(LT_L3_SLOT_INVALID, lt_pairing_key_read(h, read_key, i));

        LT_LOG_INFO("Writing to pairing key slot %" PRIu8 " (should fail)...", i);
        LT_TEST_ASSERT(LT_L3_FAIL, lt_pairing_key_write(h, zeros, i));
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Aborting Secure Session");
    LT_TEST_ASSERT(LT_OK, lt_session_abort(h));

    LT_LOG_INFO("Deinitializing handle");
    LT_TEST_ASSERT(LT_OK, lt_deinit(h));
}
