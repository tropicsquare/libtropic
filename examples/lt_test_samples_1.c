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
#include "libtropic_examples.h"

/**
 * @brief Test routine specification:
 *
 * Test data:
 *
 *  (1) Define pairing keys for all pairing slots
 *  (2) Define Ping message of max length
 *  (3) Define ECC privkey which will be stored in ECC slots
 *  (4) Define ECC message of max length
 *  (5) Define content of one r-mem slot which will be stored, read and compared
 *
 *
 * Pre test routine: read chip data and set pairing keys:
 *
 *     Session with H0:
 *         * Establish channel
 *         * Do Ping with length of 32B
 *         * Read all pairing keys, 1 2 and 3 should be empty
 *         * Write pairing keys into slot 1, 2 and 3
 *         * Read 1,2 and 3 pairing keys and print them out
 *         * Read R-config and print it out
 *         * Read I-config and print it out
 *         * Read chip ID
 *         * Read cert field and print it out
 *         * Reboot into maintenance, read fw banks info and print it out
 *         * Reboot into application
 *         * Abort session
 *         * Deinit handle
 *
 * Now run the same tests for all 4 pairing keys:
 *
 *         * Establish channel
 *         * Do Ping with max length
 *
 *         * R-mem tests:
 *             For all r-memory slots:
 *                 read slot and check if it is empty
 *                 write 444B of test data into slot
 *                 read test data and compare if they are correct
 *                 erase slot
 *                 check if it is empty
 *
 *         * ECC tests ECDSA:
 *             For all ECC slots:
 *                 store test key
 *                 read pubkey
 *                 ecdsa sign message
 *                 printout signature, because I cannot verify P256 signature
 *                 erase key
 *                 generate key
 *                 read pubkey
 *                 ecdsa sign message
 *                 printout signature, because I cannot verify P256 signature
 *                 erase key
 *
 *  *         * ECC tests EdDSA:
 *             For all slots:
 *                 store test key
 *                 read pubkey
 *                 ecdsa sign message
 *                 verify signature
 *                 erase key
 *                 generate key
 *                 read pubkey
 *                 ecdsa sign message
 *                 verify signature
 *                 erase key
 *
 *         * Abort session
 *         * Deinit handle
 */

// Define pairing keys for all pairing slots - used keys from keys.c
// Define Ping message of max length
uint8_t ping_msg[PING_LEN_MAX];
// Define ECC privkey which will be stored in ECC slots
uint8_t ecc_privkey[32] = {0xb1, 0x90, 0x9f, 0xe1, 0xf3, 0x1f, 0xa1, 0x21, 0x75, 0xef, 0x45, 0xb1, 0x42, 0xde, 0x0e, 0xdd, 0xa1, 0xf4, 0x51, 0x01, 0x40, 0xc2, 0xe5, 0x2c, 0xf4, 0x68, 0xac, 0x96, 0xa1, 0x0e, 0xcb, 0x46};
// define ecc message of max length
uint8_t ecc_msg[4096];
// Define content of one r-mem slot which will be stored, read and compared
uint8_t r_mem_data[R_MEM_DATA_SIZE_MAX];

void init_test_data()
{
    for (uint8_t i = 0; i < (PING_LEN_MAX / 32); i++)
    {
        memcpy(ping_msg, "This is ping message to be sent.", 32);
    }
    for (uint8_t i = 0; i < (4096 / 32); i++)
    {
        memcpy(ping_msg, "This is ECC message to be signed", 32);
    }

    memset(r_mem_data, 0x00, 444);
}

void print_bytes(uint8_t *data, uint16_t len) {
    ;
}

#define R_CONFIG 0
#define I_CONFIG 1

void print_config(uint8_t config_type, struct lt_config_t config) {
    if (config_type == R_CONFIG) {
        LT_LOG("R_CONFIG:");
    } else {
        LT_LOG("I_CONFIG:");
    }
    ;
}



void print_headers(lt_handle_t *h)
{
        LT_LOG("  Chip contains following headers:");
        uint8_t header[LT_L2_GET_INFO_FW_HEADER_SIZE] = {0};
        LT_LOG("    lt_get_info_fw_bank()  FW_BANK_FW1        %s", lt_ret_verbose(lt_get_info_fw_bank(h, FW_BANK_FW1, header, LT_L2_GET_INFO_FW_HEADER_SIZE)));
        LT_LOG("    Header:                                   %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", header[0], header[1], header[2], header[3],
                                                                                                                     header[4], header[5], header[6], header[7],
                                                                                                                 header[8], header[9]);
        LT_LOG("                                              %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", header[10], header[11], header[12], header[13],
                                                                                                                 header[14], header[15], header[16], header[17],
                                                                                                                 header[18], header[19]);

        LT_LOG("    lt_get_info_fw_bank()  FW_BANK_FW2        %s", lt_ret_verbose(lt_get_info_fw_bank(h, FW_BANK_FW2, header, LT_L2_GET_INFO_FW_HEADER_SIZE)));
        LT_LOG("    Header:                                   %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", header[0], header[1], header[2], header[3],
                                                                                                                 header[4], header[5], header[6], header[7],
                                                                                                                 header[8], header[9]);
        LT_LOG("                                              %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", header[10], header[11], header[12], header[13],
                                                                                                                 header[14], header[15], header[16], header[17],
                                                                                                                 header[18], header[19]);

        LT_LOG("    lt_get_info_fw_bank()  FW_BANK_SPECT1     %s", lt_ret_verbose(lt_get_info_fw_bank(h, FW_BANK_SPECT1, header, LT_L2_GET_INFO_FW_HEADER_SIZE)));
        LT_LOG("    Header:                                   %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", header[0], header[1], header[2], header[3],
                                                                                                                 header[4], header[5], header[6], header[7],
                                                                                                                 header[8], header[9]);
        LT_LOG("                                              %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", header[10], header[11], header[12], header[13],
                                                                                                                 header[14], header[15], header[16], header[17],
                                                                                                                 header[18], header[19]);

        LT_LOG("    lt_get_info_fw_bank()  FW_BANK_SPECT2     %s", lt_ret_verbose(lt_get_info_fw_bank(h, FW_BANK_SPECT2, header, LT_L2_GET_INFO_FW_HEADER_SIZE)));
        LT_LOG("    Header:                                   %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", header[0], header[1], header[2], header[3],
                                                                                                                 header[4], header[5], header[6], header[7],
                                                                                                                 header[8], header[9]);
        LT_LOG("                                              %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", header[10], header[11], header[12], header[13],
                                                                                                                  header[14], header[15], header[16], header[17],
                                                                                                                 header[18], header[19]);
}

void print_cert(uint8_t *data, uint16_t len) {
    ;
}

/**
 * @brief Session with H0 pairing keys
 *
 * @param h           Device's handle
 * @return            0 if success, otherwise -1
 */
static int session_H0(void)
{
    lt_handle_t h = {0};

    lt_init(&h);

    LT_LOG("%s", "Establish session with H0");
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, pkey_index_0));

    uint8_t in[32] = {0};
    uint8_t out[32] = {0};
    memcpy(out, "This is ping message to be sent", 31);
    // Do Ping with length of 32B
    LT_LOG("%s", "lt_ping() ");
    LT_ASSERT(LT_OK, lt_ping(&h, out, in, 43));
    LT_LOG("\t\tMessage: %s", in);

    // Read all pairing keys, 1 2 and 3 should be empty
    uint8_t public_pairing_key[32] = {0};
    LT_ASSERT(LT_OK, lt_pairing_key_read(&h, public_pairing_key, PAIRING_KEY_SLOT_INDEX_0));
    print_bytes(public_pairing_key, 32);
    LT_ASSERT(LT_L3_PAIRING_KEY_EMPTY, lt_pairing_key_read(&h, public_pairing_key, PAIRING_KEY_SLOT_INDEX_1));
    LT_ASSERT(LT_L3_PAIRING_KEY_EMPTY, lt_pairing_key_read(&h, public_pairing_key, PAIRING_KEY_SLOT_INDEX_2));
    LT_ASSERT(LT_L3_PAIRING_KEY_EMPTY, lt_pairing_key_read(&h, public_pairing_key, PAIRING_KEY_SLOT_INDEX_3));

    // Write pairing keys into slot 1, 2 and 3
    LT_LOG("%s", "Writing pairing key H1");
    LT_ASSERT(LT_OK, lt_pairing_key_write(&h, sh1pub, pkey_index_1));
    LT_LOG("%s", "Writing pairing key H2");
    LT_ASSERT(LT_OK, lt_pairing_key_write(&h, sh2pub, pkey_index_2));
    LT_LOG("%s", "Writing pairing key H3");
    LT_ASSERT(LT_OK, lt_pairing_key_write(&h, sh3pub, pkey_index_3));

    // Read 1,2 and 3 pairing keys and print them out
    LT_LOG("%s", "Reading pairing key H0");
    LT_ASSERT(LT_OK, lt_pairing_key_read(&h, public_pairing_key, PAIRING_KEY_SLOT_INDEX_0));
    print_bytes(public_pairing_key, 32);
    LT_LOG("%s", "Reading pairing key H1");
    LT_ASSERT(LT_OK, lt_pairing_key_read(&h, public_pairing_key, PAIRING_KEY_SLOT_INDEX_1));
    print_bytes(public_pairing_key, 32);
    LT_LOG("%s", "Reading pairing key H2");
    LT_ASSERT(LT_OK, lt_pairing_key_read(&h, public_pairing_key, PAIRING_KEY_SLOT_INDEX_2));
    print_bytes(public_pairing_key, 32);
    LT_LOG("%s", "Reading pairing key H3");
    LT_ASSERT(LT_OK, lt_pairing_key_read(&h, public_pairing_key, PAIRING_KEY_SLOT_INDEX_3));
    print_bytes(public_pairing_key, 32);

    // Read R-config and print it out
    LT_LOG("read_whole_R_config()");
    struct lt_config_t r_config_read;
    LT_ASSERT(LT_OK, read_whole_R_config(&h, &r_config_read));
    // Print r config
    for (int i=0; i<27;i++) {
        LT_LOG("    %s,  %08" PRIX32, get_conf_desc(i), r_config_read.obj[i]);
    }
    print_config(R_CONFIG, r_config_read);

    // Read I-config and print it out
    LT_LOG("read_whole_I_config()");
    struct lt_config_t i_config_read;
    LT_ASSERT(LT_OK, read_whole_I_config(&h, &i_config_read));
    // Print r config
    for (int i=0; i<27;i++) {
        LT_LOG("    %s,  %08" PRIX32, get_conf_desc(i), i_config_read.obj[i]);
    }
    print_config(I_CONFIG, i_config_read);

    // Read chip ID
    uint8_t chip_id[LT_L2_GET_INFO_CHIP_ID_SIZE] = {0};
    LT_ASSERT(LT_OK, lt_get_info_chip_id(&h, chip_id, LT_L2_GET_INFO_CHIP_ID_SIZE));
    print_bytes(chip_id, 128);


    // Read cert field and print it out
    uint8_t cert[LT_L2_GET_INFO_CHIP_ID_SIZE] = {0};
    LT_ASSERT(LT_OK, lt_get_info_cert(&h, cert, 3840));
    print_cert(cert, 3840);

    // Reboot into maintenance, read fw banks info and print it out
    // Reused variable
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
        LT_LOG("lt_reboot() reboot into STARTUP               %s", lt_ret_verbose(lt_reboot(&h, LT_L2_STARTUP_ID_MAINTENANCE)));
    }

    // Check again mode
    LT_LOG("---------------------------------------------------------------------------");
    LT_LOG("lt_update_mode()                              %s", lt_ret_verbose(lt_update_mode(&h)));
    if(h.mode == LT_MODE_STARTUP) {
        LT_LOG("  Chip is executing bootloader");
        // Chip must be in startup mode now.
        // Get bootloader version by issuing "Read riscv fw version" request while chip is in maintenance:
        LT_LOG("  lt_get_info_riscv_fw_ver()                  %s", lt_ret_verbose(lt_get_info_riscv_fw_ver(&h, fw_ver, LT_L2_GET_INFO_RISCV_FW_SIZE)));
        LT_LOG("  Bootloader version: %d.%d.%d    (+ unused %d)", fw_ver[3] & 0x7f,fw_ver[2],fw_ver[1],fw_ver[0]);

        print_headers(&h);

    } else {
        LT_LOG("     ERROR device couldn't get into STARTUP mode");
    }
    LT_LOG("---------------------------------------------------------------------------");

    // Reboot into application
    LT_LOG("lt_reboot() reboot                            %s", lt_ret_verbose(lt_reboot(&h, LT_L2_STARTUP_ID_REBOOT)));
    LT_LOG("lt_get_info_riscv_fw_ver()                    %s", lt_ret_verbose(lt_get_info_riscv_fw_ver(&h, fw_ver, LT_L2_GET_INFO_RISCV_FW_SIZE)));
    LT_LOG("riscv_fw_ver: %d.%d.%d    (+ unused %d)", fw_ver[3],fw_ver[2],fw_ver[1],fw_ver[0]);
    LT_LOG("lt_get_info_spect_fw_ver()                    %s", lt_ret_verbose(lt_get_info_spect_fw_ver(&h, fw_ver, LT_L2_GET_INFO_SPECT_FW_SIZE)));
    LT_LOG("spect_fw_ver: %d.%d.%d    (+ unused %d)", fw_ver[3],fw_ver[2],fw_ver[1],fw_ver[0]);
    // Abort session

    LT_LOG("%s", "Closing session H0");
    LT_ASSERT(LT_OK, lt_session_abort(&h));

    // Deinit handle
    lt_deinit(&h);

    return 0;
}

int lt_test_samples_1(void)
{
    LT_LOG("");
    LT_LOG("\t=======================================================================");
    LT_LOG("\t=====  TROPIC01 test samples 1                                      ===");
    LT_LOG("\t=======================================================================");

    LT_LOG_LINE();
    LT_LOG("\t Session with H0 keys:");
    if (session_H0() == -1)
    {
        LT_LOG("Error during session_H0()");
    }

    LT_LOG_LINE();

    LT_LOG("\t End of execution");

    return 0;
}
