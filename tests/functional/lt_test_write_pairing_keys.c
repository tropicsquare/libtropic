/**
 * @file lt_test_write_pairing_keys.c
 * @brief Test function which writes pairing keys 1 2 and 3 into TROPIC01
 * @details: Pairing keys for pairing slots 1, 2 and 3 are written int TROPIC01
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
 * @param len    Length of bytes
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

/**
 * @brief
 *
 * @return int
 */
int lt_test_write_pairing_keys(void)
{
    LT_LOG(
        "  "
        "------------------------------------------------------------------------------------------------------------"
        "-");
    LT_LOG(
        "  -------- lt_test_write_pairing_keys() "
        "-----------------------------------------------------------------------");
    LT_LOG(
        "  "
        "------------------------------------------------------------------------------------------------------------"
        "-");

    lt_handle_t h = {0};
#if LT_SEPARATE_L3_BUFF
    uint8_t l3_buffer[L3_FRAME_MAX_SIZE] __attribute__((aligned(16))) = {0};
    h.l3.buff = l3_buffer;
    h.l3.buff_len = sizeof(l3_buffer);
#endif

    LT_LOG("%s", "Initialize handle");
    LT_ASSERT(LT_OK, lt_init(&h));
    LT_LOG("%s with key H%d", "verify_chip_and_start_secure_session()", PAIRING_KEY_SLOT_INDEX_0);
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, PAIRING_KEY_SLOT_INDEX_0));
    LT_LOG("%s", "lt_ping() ");
    uint8_t ping_msg[33] = {'>', 'A', 'h', 'o', 'y', ' ', 'A', 'h', 'o', 'y', ' ', 'A', 'h', 'o', 'y', ' ', 'A',
                            'h', 'o', 'y', ' ', 'A', 'h', 'o', 'y', ' ', 'A', 'h', 'o', 'y', '!', '<', '\0'};
    uint8_t in[33] = {0};
    LT_ASSERT(LT_OK, lt_ping(&h, ping_msg, in, 33));
    LT_LOG("Received Ping message: %s", in);
    LT_LOG_LINE();

    // Read all pairing keys, 1 2 and 3 should be empty
    LT_LOG("%s", "Read pairing key H0");
    uint8_t public_pairing_key[32] = {0};
    LT_ASSERT(LT_OK, lt_pairing_key_read(&h, public_pairing_key, PAIRING_KEY_SLOT_INDEX_0));
    print_bytes(public_pairing_key, 32);
    LT_LOG("%s", "Read pairing pubkey 1, asserting LT_L3_PAIRING_KEY_EMPTY because key should be empty");
    LT_ASSERT(LT_L3_PAIRING_KEY_EMPTY, lt_pairing_key_read(&h, public_pairing_key, PAIRING_KEY_SLOT_INDEX_1));
    LT_LOG("%s", "Read pairing pubkey 2, asserting LT_L3_PAIRING_KEY_EMPTY because key should be empty");
    LT_ASSERT(LT_L3_PAIRING_KEY_EMPTY, lt_pairing_key_read(&h, public_pairing_key, PAIRING_KEY_SLOT_INDEX_2));
    LT_LOG("%s", "Read pairing pubkey 3, asserting LT_L3_PAIRING_KEY_EMPTY because key should be empty");
    LT_ASSERT(LT_L3_PAIRING_KEY_EMPTY, lt_pairing_key_read(&h, public_pairing_key, PAIRING_KEY_SLOT_INDEX_3));
    LT_LOG_LINE();

    // Write pairing keys into slot 1, 2 and 3
    LT_LOG("%s", "Writing pairing key H1");
    LT_ASSERT(LT_OK, lt_pairing_key_write(&h, sh1pub, PAIRING_KEY_SLOT_INDEX_1));
    LT_LOG("%s", "Writing pairing key H2");
    LT_ASSERT(LT_OK, lt_pairing_key_write(&h, sh2pub, PAIRING_KEY_SLOT_INDEX_2));
    LT_LOG("%s", "Writing pairing key H3");
    LT_ASSERT(LT_OK, lt_pairing_key_write(&h, sh3pub, PAIRING_KEY_SLOT_INDEX_3));
    LT_LOG_LINE();

    // Read 1,2 and 3 pairing keys and print them out
    LT_LOG("%s", "Reading pairing key H0 - should proceed");
    uint8_t readed_pubkey[32] = {0};
    LT_ASSERT(LT_OK, lt_pairing_key_read(&h, readed_pubkey, PAIRING_KEY_SLOT_INDEX_0));
    LT_LOG("%s", "Compare content of readed key with uploaded key, print pubkey");
    LT_ASSERT(0, memcmp(sh0pub, readed_pubkey, 32));
    print_bytes(readed_pubkey, 32);

    LT_LOG("%s", "Reading pairing key H1 - should proceed");
    LT_ASSERT(LT_OK, lt_pairing_key_read(&h, readed_pubkey, PAIRING_KEY_SLOT_INDEX_1));
    LT_LOG("%s", "Compare content of readed key with uploaded key, print pubkey");
    LT_ASSERT(0, memcmp(sh1pub, readed_pubkey, 32));
    print_bytes(readed_pubkey, 32);

    LT_LOG("%s", "Reading pairing key H2 - should proceed");
    LT_ASSERT(LT_OK, lt_pairing_key_read(&h, readed_pubkey, PAIRING_KEY_SLOT_INDEX_2));
    LT_LOG("%s", "Compare content of readed key with uploaded key, print pubkey");
    LT_ASSERT(0, memcmp(sh2pub, readed_pubkey, 32));
    print_bytes(readed_pubkey, 32);

    LT_LOG("%s", "Reading pairing key H3 - should proceed");
    LT_ASSERT(LT_OK, lt_pairing_key_read(&h, readed_pubkey, PAIRING_KEY_SLOT_INDEX_3));
    LT_LOG("%s", "Compare content of readed key with uploaded key, print pubkey");
    LT_ASSERT(0, memcmp(sh3pub, readed_pubkey, 32));
    print_bytes(readed_pubkey, 32);

    LT_LOG("%s", "lt_reboot() reboot into Application");
    LT_ASSERT(LT_OK, lt_reboot(&h, LT_MODE_APP));

    // Deinit handle
    LT_LOG("%s", "lt_deinit()");
    LT_ASSERT(LT_OK, lt_deinit(&h));

    return 0;
}
