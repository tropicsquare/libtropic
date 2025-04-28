/**
 * @file TROPIC01_hw_wallet.c
 * @brief Example usage of TROPIC01 chip in a generic *hardware wallet* project.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "string.h"
#include "inttypes.h"

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_examples.h"
// Include here a particular firmware (in a form of header file)
#include "fw_CPU_0_1_2.h"
#include "fw_CPU_0_2_0.h"
#include "fw_CPU_0_2_0_plus.h"

/**
 * @name Firmware update
 * @note We recommend reading TROPIC01's datasheet before diving into this example!
 * @par
 */
#define BUFF_SIZE 196

// Buffer for storing string with bytes
static char bytes_buffer[BUFF_SIZE];
static char* print_bytes(uint8_t *data, uint16_t len) {
    if((len > BUFF_SIZE) || (!data)) {
        memcpy(bytes_buffer, "error_str_decoding", 20);
        return bytes_buffer;
    }
    bytes_buffer[0] = '\0';
    for (uint16_t i = 0; i < len; i++) {
        char byte_str[4];
        snprintf(byte_str, sizeof(byte_str), "%02X", data[i]);
        // Check if appending the byte would exceed the buffer size
        if (strlen(bytes_buffer) + strlen(byte_str) + 1 > sizeof(bytes_buffer)) {
            break; // Stop if the buffer is full
        }
        strncat(bytes_buffer, byte_str, sizeof(bytes_buffer) - strlen(bytes_buffer) - 1);
    }

    return bytes_buffer;
}

static void print_headers(lt_handle_t *h)
{
        LT_LOG("  Chip contains following headers:");
        uint8_t header[LT_L2_GET_INFO_FW_HEADER_SIZE] = {0};
        LT_LOG("    lt_get_info_fw_bank()  FW_BANK_FW1        %s", lt_ret_verbose(lt_get_info_fw_bank(h, FW_BANK_FW1, header, LT_L2_GET_INFO_FW_HEADER_SIZE)));
        LT_LOG("    Header:                                   %s", print_bytes(header, 10));
        LT_LOG("                                              %s", print_bytes(header+10, 10));

        LT_LOG("    lt_get_info_fw_bank()  FW_BANK_FW2        %s", lt_ret_verbose(lt_get_info_fw_bank(h, FW_BANK_FW2, header, LT_L2_GET_INFO_FW_HEADER_SIZE)));
        LT_LOG("    Header:                                   %s", print_bytes(header, 10));
        LT_LOG("                                              %s", print_bytes(header+10, 10));

        LT_LOG("    lt_get_info_fw_bank()  FW_BANK_SPECT1     %s", lt_ret_verbose(lt_get_info_fw_bank(h, FW_BANK_SPECT1, header, LT_L2_GET_INFO_FW_HEADER_SIZE)));
        LT_LOG("    Header:                                   %s", print_bytes(header, 10));
        LT_LOG("                                              %s", print_bytes(header+10, 10));

        LT_LOG("    lt_get_info_fw_bank()  FW_BANK_SPECT2     %s", lt_ret_verbose(lt_get_info_fw_bank(h, FW_BANK_SPECT2, header, LT_L2_GET_INFO_FW_HEADER_SIZE)));
        LT_LOG("    Header:                                   %s", print_bytes(header, 10));
        LT_LOG("                                              %s", print_bytes(header+10, 10));
}

void lt_ex_fw_update(void)
{
    LT_LOG("\t=======================================================================");
    LT_LOG("\t=====  TROPIC01 FW update                                           ===");
    LT_LOG("\t=======================================================================");

    lt_handle_t h = {0};
    lt_ret_t ret = LT_FAIL;

    lt_init(&h);

    // Reused variable
    uint8_t fw_ver[LT_L2_GET_INFO_RISCV_FW_SIZE] = {0};

    // First check in which mode chip operates, bootloader or application
    LT_LOG("lt_update_mode()                              %s", lt_ret_verbose(lt_update_mode(&h)));
    if(h.mode == LT_MODE_APP) {
        LT_LOG("  Chip is executing application firmware");
        // App runs so we can see what firmwares are running
        // RISCV app firmware version
        ret = lt_get_info_riscv_fw_ver(&h, fw_ver, LT_L2_GET_INFO_RISCV_FW_SIZE);
        if(ret != LT_OK) {
            LT_LOG("     lt_get_info_riscv_fw_ver()               %s", lt_ret_verbose(ret));
        } else {
            LT_LOG("lt_get_info_riscv_fw_ver()                    %s", lt_ret_verbose(ret));
            LT_LOG("  riscv_fw_ver: %d.%d.%d    (+ unused %d)", fw_ver[3],fw_ver[2],fw_ver[1],fw_ver[0]);
        }

        // SPECT firmware version
        ret = lt_get_info_spect_fw_ver(&h, fw_ver, LT_L2_GET_INFO_SPECT_FW_SIZE);
        if(ret != LT_OK) {
            LT_LOG("     lt_get_info_spect_fw_ver()               %s", lt_ret_verbose(ret));
        } else {
            LT_LOG("lt_get_info_spect_fw_ver()                    %s", lt_ret_verbose(ret));
            LT_LOG("  spect_fw_ver: %d.%d.%d    (+ unused %d)", fw_ver[3],fw_ver[2],fw_ver[1],fw_ver[0]);
        }
        // Now reboot into STARTUP (bootloader)
        LT_LOG("lt_reboot() reboot into STARTUP               %s", lt_ret_verbose(lt_reboot(&h, LT_L2_STARTUP_ID_MAINTENANCE)));
    }

    // Check again mode
    LT_LOG_LINE();
    LT_LOG("lt_update_mode()                              %s", lt_ret_verbose(lt_update_mode(&h)));
    if(h.mode == LT_MODE_STARTUP) {
        LT_LOG("  Chip is executing bootloader");
        // Chip must be in startup mode now.
        // Get bootloader version by issuing "Read riscv fw version" request while chip is in maintenance:
        LT_LOG("  lt_get_info_riscv_fw_ver()                  %s", lt_ret_verbose(lt_get_info_riscv_fw_ver(&h, fw_ver, LT_L2_GET_INFO_RISCV_FW_SIZE)));
        LT_LOG("  Bootloader version: %d.%d.%d    (+ unused %d)", fw_ver[3] & 0x7f,fw_ver[2],fw_ver[1],fw_ver[0]);

        print_headers(&h);

        if(1) {
            // Erase firmware bank
            LT_LOG("lt_mutable_fw_erase()                    %s", lt_ret_verbose(lt_mutable_fw_erase(&h, FW_BANK_FW2)));
            // Update firmware bank
            LT_LOG("lt_mutable_fw_update()                   %s", lt_ret_verbose(lt_mutable_fw_update(&h, fw_CPU_0_2_0_plus, fw_CPU_0_2_0_plus_len, FW_BANK_FW1)));

            print_headers(&h);

        } else {
            LT_LOG("Update disabled");
        }

    } else {
        LT_LOG("     ERROR device couldn't get into STARTUP mode");
        return;
    }
    LT_LOG_LINE();
    LT_LOG("lt_reboot() reboot                            %s", lt_ret_verbose(lt_reboot(&h, LT_L2_STARTUP_ID_REBOOT)));
    LT_LOG("lt_get_info_riscv_fw_ver()                    %s", lt_ret_verbose(lt_get_info_riscv_fw_ver(&h, fw_ver, LT_L2_GET_INFO_RISCV_FW_SIZE)));
    LT_LOG("riscv_fw_ver: %d.%d.%d    (+ unused %d)", fw_ver[3],fw_ver[2],fw_ver[1],fw_ver[0]);
    LT_LOG("lt_get_info_spect_fw_ver()                    %s", lt_ret_verbose(lt_get_info_spect_fw_ver(&h, fw_ver, LT_L2_GET_INFO_SPECT_FW_SIZE)));
    LT_LOG("spect_fw_ver: %d.%d.%d    (+ unused %d)", fw_ver[3],fw_ver[2],fw_ver[1],fw_ver[0]);

    return;
}
