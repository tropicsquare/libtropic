/**
 * @file lt_test_ire_pairing_key_slots.c
 * @brief Test Pairing_Key_Read, Pairing_Key_Write and Pairing_Key_Invalidate on all slots.
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

/** @brief Size of the print buffer. */
#define PRINT_BUFF_SIZE 65

void lt_test_ire_pairing_key_slots(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_ire_pairing_key_slots()");
    LT_LOG_INFO("----------------------------------------------");

    uint8_t *pub_keys[] = {sh0pub, sh1pub, sh2pub, sh3pub};
    uint8_t *priv_keys[] = {sh0priv, sh1priv, sh2priv, sh3priv};
    uint8_t read_key[32] = {0};
    uint8_t zeros[32] = {0};
    char print_buff[PRINT_BUFF_SIZE];

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    LT_LOG_INFO("Starting Secure Session with key %d", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    LT_TEST_ASSERT(LT_OK, lt_verify_chip_and_start_secure_session(h, sh0priv, sh0pub, TR01_PAIRING_KEY_SLOT_INDEX_0));
    LT_LOG_LINE();

    // Read pairing keys (1,2,3 should be empty)
    LT_LOG_INFO("Reading pairing key slot %d...", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    LT_TEST_ASSERT(LT_OK, lt_pairing_key_read(h, read_key, TR01_PAIRING_KEY_SLOT_INDEX_0));
    LT_TEST_ASSERT(LT_OK, lt_print_bytes(read_key, 32, print_buff, PRINT_BUFF_SIZE));
    LT_LOG_INFO("%s", print_buff);
    LT_LOG_INFO();
    for (uint8_t i = TR01_PAIRING_KEY_SLOT_INDEX_1; i <= TR01_PAIRING_KEY_SLOT_INDEX_3; i++) {
        LT_LOG_INFO("Reading pairing key slot %" PRIu8 " (should fail)...", i);
        LT_TEST_ASSERT(LT_L3_PAIRING_KEY_EMPTY, lt_pairing_key_read(h, read_key, i));
        LT_LOG_INFO();
    }
    LT_LOG_LINE();

    // Write pairing keys into slot 1,2,3
    for (uint8_t i = TR01_PAIRING_KEY_SLOT_INDEX_1; i <= TR01_PAIRING_KEY_SLOT_INDEX_3; i++) {
        LT_LOG_INFO("Writing to pairing key slot %" PRIu8 "...", i);
        LT_TEST_ASSERT(LT_OK, lt_print_bytes(pub_keys[i], 32, print_buff, PRINT_BUFF_SIZE));
        LT_LOG_INFO("%s", print_buff);
        LT_TEST_ASSERT(LT_OK, lt_pairing_key_write(h, pub_keys[i], i));
        LT_LOG_INFO();
    }
    LT_LOG_LINE();

    // Read all pairing keys and check value
    for (uint8_t i = TR01_PAIRING_KEY_SLOT_INDEX_0; i <= TR01_PAIRING_KEY_SLOT_INDEX_3; i++) {
        LT_LOG_INFO("Reading pairing key slot %" PRIu8 "...", i);
        LT_TEST_ASSERT(LT_OK, lt_pairing_key_read(h, read_key, i));
        LT_TEST_ASSERT(LT_OK, lt_print_bytes(read_key, 32, print_buff, PRINT_BUFF_SIZE));
        LT_LOG_INFO("%s", print_buff);

        LT_LOG_INFO("Comparing contents of written and read key...");
        LT_TEST_ASSERT(0, memcmp(pub_keys[i], read_key, 32));
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
    LT_TEST_ASSERT(LT_OK, lt_verify_chip_and_start_secure_session(h, sh0priv, sh0pub, TR01_PAIRING_KEY_SLOT_INDEX_0));
    LT_LOG_LINE();

    // Write pairing key slots again (should fail)
    for (uint8_t i = TR01_PAIRING_KEY_SLOT_INDEX_0; i <= TR01_PAIRING_KEY_SLOT_INDEX_3; i++) {
        LT_LOG_INFO("Writing to pairing key slot %" PRIu8 " (should fail)...", i);
        LT_TEST_ASSERT(LT_L3_FAIL, lt_pairing_key_write(h, zeros, i));

        LT_LOG_INFO("Reading pairing key slot %" PRIu8 "...", i);
        LT_TEST_ASSERT(LT_OK, lt_pairing_key_read(h, read_key, i));
        LT_TEST_ASSERT(LT_OK, lt_print_bytes(read_key, 32, print_buff, PRINT_BUFF_SIZE));
        LT_LOG_INFO("%s", print_buff);

        LT_LOG_INFO("Comparing contents of expected key and read key...");
        LT_TEST_ASSERT(0, memcmp(pub_keys[i], read_key, 32));
        LT_LOG_INFO();
    }
    LT_LOG_LINE();

    // Invalidate all slots, try reading and writing them
    for (uint8_t i = TR01_PAIRING_KEY_SLOT_INDEX_0; i <= TR01_PAIRING_KEY_SLOT_INDEX_3; i++) {
        LT_LOG_INFO("Invalidating pairing key slot %" PRIu8 "...", i);
        LT_TEST_ASSERT(LT_OK, lt_pairing_key_invalidate(h, i));

        LT_LOG_INFO("Reading pairing key slot %" PRIu8 " (should fail)...", i);
        LT_TEST_ASSERT(LT_L3_PAIRING_KEY_INVALID, lt_pairing_key_read(h, read_key, i));

        LT_LOG_INFO("Writing to pairing key slot %" PRIu8 " (should fail)...", i);
        LT_TEST_ASSERT(LT_L3_FAIL, lt_pairing_key_write(h, zeros, i));
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Aborting Secure Session");
    LT_TEST_ASSERT(LT_OK, lt_session_abort(h));

    LT_LOG_INFO("Deinitializing handle");
    LT_TEST_ASSERT(LT_OK, lt_deinit(h));
}
