/**
 * @file TROPIC01_hw_wallet.c
 * @brief Test samples according to specification from design department
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "string.h"
#include "inttypes.h"

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"

/**
 * @brief Test routine specification:
 *
 * Note: Pairing keys for all pairing slots are taken from keys.c file
 *
 * Test is divided into more functions (this allows controling execution of the part with 'pairing key write')
 *
 * [1] setup_test_data():
 *         - Prepares known test data: ping_msg, ecc_privkey, ecc_msg and r_mem_data
 *
 * [2] test_test_write_pairing_keys():
 *         - Why? To test if pairing keys are empty and if they can be written
 *         - Should be executed only once
 *         - Checks if pairing keys 1-3 are empty and then write them and check if they are correct
 *         - reboots chip
 *
 * [3] test_test_get_chip_info():
 *         - Why? To see what data were provisioned
 *         - Reads content of all get info fields
 *         - Reads all configuration objects
 *         - Reads firmware headers
 * 
 * [4] test_channels():
 *         - try ping of max length with all session keys
 *
 */

/**
 * @brief Local helper, print bytes as hexadecimal numbers
 *
 * @param data  Bytes to be printed
 * @param len    Length of bytes
 */
static void print_bytes(uint8_t *data, uint16_t len) {
    char buffer[256] = {0};
    for (uint16_t i = 0; i < len; i++) {
        char byte_str[4];
        snprintf(byte_str, sizeof(byte_str), "%02X ", data[i]);
        strncat(buffer, byte_str, sizeof(buffer) - strlen(buffer) - 1);

        // Print the buffer every 32 bytes or at the end of the data
        if ((i + 1) % 32 == 0 || i == len - 1) {
            LT_LOG_INFO("%s", buffer);
            buffer[0] = '\0'; // Clear the buffer for the next line
        }
    }
}

#define R_CONFIG 0
#define I_CONFIG 1

/**
 * @brief Local helper, print out chip config objects
 *
 * @param config_type  R_CONFIG or I_CONFIG
 * @param config     structure containing config objects
 */
static void print_config(uint8_t config_type, struct lt_config_t config) {
    if (config_type == R_CONFIG) {
        LT_LOG("R_CONFIG:");
    } else if (config_type == I_CONFIG) {
        LT_LOG("I_CONFIG:");
    } else {
        LT_LOG("Unknown config type\n");
        return;
    }

    for (int i = 0; i < 27; i++) { // Assuming the config has 27 objects
        LT_LOG("    %s,  %08" PRIX32, get_conf_desc(i), config.obj[i]);
    }
}

/***********************************************************************************/
/*************       START OF TEST FUNCTIONS       *********************************/
/***********************************************************************************/

// Define pairing keys for all pairing slots - used keys from keys.c
// Define Ping message of max length
uint8_t ping_msg[PING_LEN_MAX];
// Define ECC privkey which will be stored in ECC slots
uint8_t ecc_privkey[32] = {0xb1, 0x90, 0x9f, 0xe1, 0xf3, 0x1f, 0xa1, 0x21, 0x75, 0xef, 0x45, 0xb1, 0x42, 0xde, 0x0e, 0xdd, 0xa1, 0xf4, 0x51, 0x01, 0x40, 0xc2, 0xe5, 0x2c, 0xf4, 0x68, 0xac, 0x96, 0xa1, 0x0e, 0xcb, 0x46};
// define ecc message of max length
uint8_t ecc_msg[4096];
// Define content of one r-mem slot which will be stored, read and compared
uint8_t r_mem_data[R_MEM_DATA_SIZE_MAX];

/**
 * @brief Set the test data up
 */
static void setup_test_data()
{
    // Ping message
    for (uint8_t i = 0; i < (PING_LEN_MAX / 32); i++) {
        memcpy(ping_msg+32*i, "This is ping message to be sent", 32);
    }
    // ECC message
    for (uint8_t i = 0; i < (4096 / 32); i++) {
        memcpy(ecc_msg+32*i, "This is ECC message to be signed", 32);
    }
    // R-mem data
    memset(r_mem_data, 0x00, 444);
}

/**
 * @brief  * Establish secure channel
 *         * Do Ping with length of 32B
 *         * Read all pairing keys, 1 2 and 3 should be empty
 *         * Write pairing keys into slot 1, 2 and 3
 *         * Read 1,2 and 3 pairing keys and assert them with original keys, print them out
 *         * Abort session
 *         * Deinit handle
 *
 * @return int
 */
static int test_write_pairing_keys(void)
{
    lt_handle_t h = {0};

    LT_LOG("%s", "Initialize handle");
    LT_ASSERT(LT_OK, lt_init(&h));
    LT_LOG("%s with %d", "verify_chip_and_start_secure_session()", pkey_index_0);
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, pkey_index_0));
    LT_LOG("%s", "lt_ping() ");
    uint8_t in[32] = {0};
    LT_ASSERT(LT_OK, lt_ping(&h, ping_msg, in, 32));
    LT_LOG("Received Ping message: %s", in);
    LT_LOG_LINE();

    // Read all pairing keys, 1 2 and 3 should be empty
    LT_LOG("%s", "Read pairing pubkey 0");
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
    LT_ASSERT(LT_OK, lt_pairing_key_write(&h, sh1pub, pkey_index_1));
    LT_LOG("%s", "Writing pairing key H2");
    LT_ASSERT(LT_OK, lt_pairing_key_write(&h, sh2pub, pkey_index_2));
    LT_LOG("%s", "Writing pairing key H3");
    LT_ASSERT(LT_OK, lt_pairing_key_write(&h, sh3pub, pkey_index_3));
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
    LT_ASSERT(LT_OK,  lt_reboot(&h, LT_L2_STARTUP_ID_REBOOT));

    // Deinit handle
    LT_LOG("%s", "lt_deinit()");
    LT_ASSERT(LT_OK, lt_deinit(&h));

    LT_LOG_LINE();

    return 0;
}

static int test_channels(void)
{
    lt_handle_t h = {0};
    uint8_t in[PING_LEN_MAX] = {0};

    LT_LOG("%s", "Initialize handle");
    LT_ASSERT(LT_OK, lt_init(&h));

    // Ping with SH0
    LT_LOG("%s with %d", "verify_chip_and_start_secure_session()", pkey_index_0);
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, pkey_index_0));
    LT_LOG("%s", "lt_ping() ");
    LT_ASSERT(LT_OK, lt_ping(&h, ping_msg, in, PING_LEN_MAX));
    LT_LOG("Asserting %d B of Ping message", PING_LEN_MAX);
    LT_ASSERT(0, memcmp(in, ping_msg, PING_LEN_MAX));
    LT_LOG_LINE();
    LT_LOG("%s", "lt_session_abort()");
    LT_ASSERT(LT_OK, lt_session_abort(&h));
    memset(in, 0x00, PING_LEN_MAX);

    // Ping with SH1
    LT_LOG("%s with %d", "verify_chip_and_start_secure_session()", pkey_index_1);
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh1priv, sh1pub, pkey_index_1));
    LT_LOG("%s", "lt_ping() ");
    LT_ASSERT(LT_OK, lt_ping(&h, ping_msg, in, PING_LEN_MAX));
    LT_LOG("Asserting %d B of Ping message", PING_LEN_MAX);
    LT_ASSERT(0, memcmp(in, ping_msg, PING_LEN_MAX));
    LT_LOG_LINE();
    LT_LOG("%s", "lt_session_abort()");
    LT_ASSERT(LT_OK, lt_session_abort(&h));
    memset(in, 0x00, PING_LEN_MAX);

    // Ping with SH2
    LT_LOG("%s with %d", "verify_chip_and_start_secure_session()", pkey_index_2);
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh2priv, sh2pub, pkey_index_2));
    LT_LOG("%s", "lt_ping() ");
    LT_ASSERT(LT_OK, lt_ping(&h, ping_msg, in, PING_LEN_MAX));
    LT_LOG("Asserting %d B of Ping message", PING_LEN_MAX);
    LT_ASSERT(0, memcmp(in, ping_msg, PING_LEN_MAX));
    LT_LOG_LINE();
    LT_LOG("%s", "lt_session_abort()");
    LT_ASSERT(LT_OK, lt_session_abort(&h));
    memset(in, 0x00, PING_LEN_MAX);

    // Ping with SH3
    LT_LOG("%s with %d", "verify_chip_and_start_secure_session()", pkey_index_3);
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh3priv, sh3pub, pkey_index_3));
    LT_LOG("%s", "lt_ping() ");
    LT_ASSERT(LT_OK, lt_ping(&h, ping_msg, in, PING_LEN_MAX));
    LT_LOG("Asserting %d B of Ping message", PING_LEN_MAX);
    LT_ASSERT(0, memcmp(in, ping_msg, PING_LEN_MAX));
    LT_LOG_LINE();
    LT_LOG("%s", "lt_session_abort()");
    LT_ASSERT(LT_OK, lt_session_abort(&h));

    // Deinit handle
    LT_LOG("%s", "lt_deinit()");
    LT_ASSERT(LT_OK, lt_deinit(&h));

    return 0;
}


/**
 * @brief  * Establish channel
 *         * Do Ping with length of 32B
 *         * Read R-config and print it out
 *         * Read I-config and print it out
 *         * Read chip ID
 *         * Read cert field and print it out
 *         * Reboot into maintenance, read fw banks info and print it out
 *         * Reboot into application
 *         * Abort session
 *         * Deinit handle
 *
 * @return int
 */
static int test_get_chip_info(void)
{
    lt_handle_t h = {0};

    LT_LOG("%s", "Initialize handle");
    LT_ASSERT(LT_OK, lt_init(&h));
    LT_LOG("%s with %d", "verify_chip_and_start_secure_session()", pkey_index_0);
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, pkey_index_0));
    LT_LOG("%s", "lt_ping() ");
    uint8_t in[32] = {0};
    LT_ASSERT(LT_OK, lt_ping(&h, ping_msg, in, 32));
    LT_LOG("Received Ping message: %s", in);
    LT_LOG_LINE();

    // Read R-config and print it out
    LT_LOG("read_whole_R_config()");
    struct lt_config_t r_config_read;
    LT_ASSERT(LT_OK, read_whole_R_config(&h, &r_config_read));
    LT_LOG("Print R config()");
    print_config(R_CONFIG, r_config_read);
    LT_LOG_LINE();

    // Read I-config and print it out
    LT_LOG("read_whole_I_config()");
    struct lt_config_t i_config_read;
    LT_ASSERT(LT_OK, read_whole_I_config(&h, &i_config_read));
    LT_LOG("Print I config()");
    print_config(I_CONFIG, i_config_read);
    LT_LOG_LINE();

    // Read chip ID
    LT_LOG("lt_get_info_chip_id()");
    uint8_t chip_id[LT_L2_GET_INFO_CHIP_ID_SIZE] = {0};
    LT_ASSERT(LT_OK, lt_get_info_chip_id(&h, chip_id, LT_L2_GET_INFO_CHIP_ID_SIZE));
    LT_LOG("Print chip_id()");
    print_bytes(chip_id, 128);
    LT_LOG_LINE();

    // Read cert field and print it out
    uint8_t cert[3840] = {0};
    LT_ASSERT(LT_OK, lt_get_info_cert(&h, cert, 3840));
    LT_LOG("Print cert()");
    print_bytes(cert, 3840);

    // Reading info from firmware banks:

    // Chip should be in application, so let's try to read fw versions
    uint8_t fw_ver[LT_L2_GET_INFO_RISCV_FW_SIZE] = {0};

    LT_LOG("lt_update_mode()                              %s", lt_ret_verbose(lt_update_mode(&h)));
    if(h.mode == LT_MODE_APP) {
        LT_LOG("  Chip is executing CPU firmware");
        // App runs so we can see what firmwares are running
        LT_LOG("lt_get_info_riscv_fw_ver()                    %s", lt_ret_verbose(lt_get_info_riscv_fw_ver(&h, fw_ver, LT_L2_GET_INFO_RISCV_FW_SIZE)));
        LT_LOG("  riscv_fw_ver: %d.%d.%d    (+ unused %d)", fw_ver[3],fw_ver[2],fw_ver[1],fw_ver[0]);
        LT_LOG("lt_get_info_spect_fw_ver()                    %s", lt_ret_verbose(lt_get_info_spect_fw_ver(&h, fw_ver, LT_L2_GET_INFO_SPECT_FW_SIZE)));
        LT_LOG("  spect_fw_ver: %d.%d.%d    (+ unused %d)", fw_ver[3],fw_ver[2],fw_ver[1],fw_ver[0]);
        // Now reboot into STARTUP
        LT_LOG("lt_reboot() reboot into MAINTENANCE               %s", lt_ret_verbose(lt_reboot(&h, LT_L2_STARTUP_ID_MAINTENANCE)));
    }

    LT_LOG_LINE();
    // Now chip should be in maintenance, update mode and check mode again
    LT_LOG("lt_update_mode()                              %s", lt_ret_verbose(lt_update_mode(&h)));
    if(h.mode == LT_MODE_STARTUP) {
        LT_LOG("  Chip is executing bootloader - MAINTENANCE MODE");
        // Get bootloader version by issuing "Read riscv fw version" request while chip is in maintenance:
        LT_LOG("  lt_get_info_riscv_fw_ver() now returns bootloader version                 %s", lt_ret_verbose(lt_get_info_riscv_fw_ver(&h, fw_ver, LT_L2_GET_INFO_RISCV_FW_SIZE)));
        LT_LOG("  Bootloader version: %d.%d.%d    (+ unused %d)", fw_ver[3] & 0x7f,fw_ver[2],fw_ver[1],fw_ver[0]);
        LT_LOG("  Chip contains following headers:");

        // Print all fw headers
        uint8_t header[LT_L2_GET_INFO_FW_HEADER_SIZE] = {0};
        LT_LOG("    lt_get_info_fw_bank()  FW_BANK_FW1        %s", lt_ret_verbose(lt_get_info_fw_bank(&h, FW_BANK_FW1, header, LT_L2_GET_INFO_FW_HEADER_SIZE)));
        print_bytes(header, 10);
        print_bytes(header+10, 10);

        LT_LOG("    lt_get_info_fw_bank()  FW_BANK_FW2        %s", lt_ret_verbose(lt_get_info_fw_bank(&h, FW_BANK_FW2, header, LT_L2_GET_INFO_FW_HEADER_SIZE)));
        print_bytes(header, 10);
        print_bytes(header+10, 10);

        LT_LOG("    lt_get_info_fw_bank()  FW_BANK_SPECT1     %s", lt_ret_verbose(lt_get_info_fw_bank(&h, FW_BANK_SPECT1, header, LT_L2_GET_INFO_FW_HEADER_SIZE)));
        print_bytes(header, 10);
        print_bytes(header+10, 10);

        LT_LOG("    lt_get_info_fw_bank()  FW_BANK_SPECT2     %s", lt_ret_verbose(lt_get_info_fw_bank(&h, FW_BANK_SPECT2, header, LT_L2_GET_INFO_FW_HEADER_SIZE)));
        print_bytes(header, 10);
        print_bytes(header+10, 10);
    } else {
        LT_LOG("     ERROR device couldn't get into STARTUP mode");
    }
    LT_LOG_LINE();

    // Reboot into application
    LT_LOG("lt_reboot() reboot into APP                          %s", lt_ret_verbose(lt_reboot(&h, LT_L2_STARTUP_ID_REBOOT)));
    LT_LOG("lt_get_info_riscv_fw_ver()                    %s", lt_ret_verbose(lt_get_info_riscv_fw_ver(&h, fw_ver, LT_L2_GET_INFO_RISCV_FW_SIZE)));
    LT_LOG("riscv_fw_ver: %d.%d.%d    (+ unused %d)", fw_ver[3],fw_ver[2],fw_ver[1],fw_ver[0]);
    LT_LOG("lt_get_info_spect_fw_ver()                    %s", lt_ret_verbose(lt_get_info_spect_fw_ver(&h, fw_ver, LT_L2_GET_INFO_SPECT_FW_SIZE)));
    LT_LOG("spect_fw_ver: %d.%d.%d    (+ unused %d)", fw_ver[3],fw_ver[2],fw_ver[1],fw_ver[0]);
    LT_LOG_LINE();

    // Abort session
    LT_LOG("%s", "lt_session_abort()");
    LT_ASSERT(LT_OK, lt_session_abort(&h));
    // Deinit handle
    LT_LOG("%s", "lt_deinit()");
    LT_ASSERT(LT_OK, lt_deinit(&h));

    return 0;
}

/**
 * @brief Main function for test samples 1
 *
 * @return int
 */
int lt_test_samples_1(void)
{
    LT_LOG("");
    LT_LOG("  -------------------------------------------------------------------------------------------------------------");
    LT_LOG("  -------- TROPIC01 test samples 1 ----------------------------------------------------------------------------");


    setup_test_data();

    // WARNING!! Ireversible operations.
    LT_LOG("  -------------------------------------------------------------------------------------------------------------");
    LT_LOG("  -------- [1] TEST WRITE PAIRING KEYS ------------------------------------------------------------------------");
    LT_LOG("  -------------------------------------------------------------------------------------------------------------");
    if (test_write_pairing_keys() == -1) {
        LT_LOG("Error during TEST WRITE PAIRING KEYS");
    }

    LT_LOG("  -------------------------------------------------------------------------------------------------------------");
    LT_LOG("  -------- [2] TEST CHANNELS ----------------------------------------------------------------------------------");
    LT_LOG("  -------------------------------------------------------------------------------------------------------------");
    if (test_channels() == -1) {
        LT_LOG("Error during TEST CHANNELS");
    }

    LT_LOG("  -------------------------------------------------------------------------------------------------------------");
    LT_LOG("  -------- [3] TEST GET CHIP INFO -----------------------------------------------------------------------------");
    LT_LOG("  -------------------------------------------------------------------------------------------------------------");
    if (test_get_chip_info() == -1) {
        LT_LOG("Error during TEST GET CHIP INFO");
    }

    LT_LOG("\t End of execution");

    return 0;
}
