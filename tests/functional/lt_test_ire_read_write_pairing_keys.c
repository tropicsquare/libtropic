/**
 * @file lt_test_read_write_pairing_keys_irreversible.c
 * @brief Test Pairing_Key_Read and Pairing_Key_Write on all slots.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */
#include "inttypes.h"
#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "string.h"

/**
 * @brief Local helper, print bytes as hexadecimal numbers
 *
 * @param data  Bytes to be printed
 * @param len   Length of bytes
 */
static void print_bytes(uint8_t *data, uint16_t len)
{
    char buffer[256] = {0};
    for (uint16_t i = 0; i < len; i++) {
        char byte_str[4];
        snprintf(byte_str, sizeof(byte_str), "%02X ", data[i]);
        strncat(buffer, byte_str, sizeof(buffer) - strlen(buffer) - 1);

        // Print the buffer every 32 bytes or at the end of the data
        if ((i + 1) % 32 == 0 || i == len - 1) {
            LT_LOG_INFO("%s", buffer);
            buffer[0] = '\0';  // Clear the buffer for the next line
        }
    }
}

void lt_test_ire_read_write_pairing_keys(void)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_read_write_pairing_keys_irreversible()");
    LT_LOG_INFO("----------------------------------------------");

    lt_handle_t h = {0};
#if LT_SEPARATE_L3_BUFF
    uint8_t l3_buffer[L3_FRAME_MAX_SIZE] __attribute__((aligned(16))) = {0};
    h.l3.buff = l3_buffer;
    h.l3.buff_len = sizeof(l3_buffer);
#endif
    uint8_t *pub_keys[] = {sh0pub, sh1pub, sh2pub, sh3pub};
    uint8_t read_key[32] = {0};
    uint8_t zeros[32] = {0};

    LT_LOG_INFO("Initializing handle");
    LT_ASSERT(LT_OK, lt_init(&h));

    LT_LOG_INFO("Starting Secure Session with key %d", PAIRING_KEY_SLOT_INDEX_0);
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, PAIRING_KEY_SLOT_INDEX_0));
    LT_LOG_LINE();

    // Read pairing keys (1,2,3 should be empty)
    LT_LOG_INFO("Reading pairing key slot 0:");
    LT_ASSERT(LT_OK, lt_pairing_key_read(&h, read_key, PAIRING_KEY_SLOT_INDEX_0));
    print_bytes(read_key, 32);
    LT_LOG_INFO();
    for (uint8_t i = PAIRING_KEY_SLOT_INDEX_1; i <= PAIRING_KEY_SLOT_INDEX_3; i++) {
        LT_LOG_INFO("Reading pairing key slot %d, asserting LT_L3_PAIRING_KEY_EMPTY", i);
        LT_ASSERT(LT_L3_PAIRING_KEY_EMPTY, lt_pairing_key_read(&h, read_key, i));
        LT_LOG_INFO();
    }
    LT_LOG_LINE();

    // Write pairing keys into slot 1,2,3
    for (uint8_t i = PAIRING_KEY_SLOT_INDEX_1; i <= PAIRING_KEY_SLOT_INDEX_3; i++) {
        LT_LOG_INFO("Writing to pairing key slot %d:", i);
        print_bytes(pub_keys[i], 32);
        LT_ASSERT(LT_OK, lt_pairing_key_write(&h, pub_keys[i], i));
        LT_LOG_INFO();
    }
    LT_LOG_LINE();

    // Read all pairing keys and check value
    for (uint8_t i = PAIRING_KEY_SLOT_INDEX_0; i <= PAIRING_KEY_SLOT_INDEX_3; i++) {
        LT_LOG_INFO("Reading pairing key slot %d:", i);
        LT_ASSERT(LT_OK, lt_pairing_key_read(&h, read_key, i));
        print_bytes(read_key, 32);
        LT_LOG_INFO("Comparing contents of written and read key");
        LT_ASSERT(0, memcmp(pub_keys[i], read_key, 32));
        LT_LOG_INFO();
    }
    LT_LOG_LINE();

    // Write pairing key slots again (should fail)
    for (uint8_t i = PAIRING_KEY_SLOT_INDEX_0; i <= PAIRING_KEY_SLOT_INDEX_3; i++) {
        LT_LOG_INFO("Writing zeros to pairing key slot %d, asserting LT_L3_FAIL", i);
        LT_ASSERT(LT_L3_FAIL, lt_pairing_key_write(&h, zeros, i));
        LT_LOG_INFO("Reading pairing key slot %d:", i);
        LT_ASSERT(LT_OK, lt_pairing_key_read(&h, read_key, i));
        print_bytes(read_key, 32);
        LT_LOG_INFO("Comparing contents of expected key and read key");
        LT_ASSERT(0, memcmp(pub_keys[i], read_key, 32));
        LT_LOG_INFO();
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Aborting Secure Session");
    LT_ASSERT(LT_OK, lt_session_abort(&h));

    LT_LOG_INFO("Deinitializing handle");
    LT_ASSERT(LT_OK, lt_deinit(&h));
}
