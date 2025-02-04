/**
 * @file TROPIC01_hw_wallet.c
 * @brief Example usage of TROPIC01 chip in a generic *hardware wallet* project.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "string.h"

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_examples.h"
// Include here a particular firmware (in a form of header file)
#include "fw_CPU_0_1_2.h"

/**
 * @name Firmware update
 * @note We recommend reading TROPIC01's datasheet before diving into this example!
 * @par
 */

#if defined(_MSC_VER) || (defined(__INTEL_COMPILER) && defined(_WIN32))
    #if defined(_M_X64)
        #define BITNESS 64
        #define LONG_SIZE 4
    #else
        #define BITNESS 32
        #define LONG_SIZE 4
    #endif
#elif defined(__clang__) || defined(__INTEL_COMPILER) || defined(__GNUC__)
    #if defined(__x86_64)
        #define BITNESS 64
    #else
       #define BITNESS 32
    #endif
    #if __LONG_MAX__ == 2147483647L
        #define LONG_SIZE 4
    #else
        #define LONG_SIZE 8
    #endif
#endif


void print_headers(lt_handle_t *h)
{
        LOG_OUT("  Chip contains following headers:\r\n");
        uint8_t header[LT_L2_GET_INFO_FW_HEADER_SIZE] = {0};
        LOG_OUT("    lt_get_info_fw_bank()  FW_BANK_FW1        %s\r\n", lt_ret_verbose(lt_get_info_fw_bank(&h, FW_BANK_FW1, header, LT_L2_GET_INFO_FW_HEADER_SIZE)));
        LOG_OUT("    Header:                                   %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\r\n", header[0], header[1], header[2], header[3],
                                                                                                                     header[4], header[5], header[6], header[7],
                                                                                                                 header[8], header[9]);
        LOG_OUT("                                              %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\r\n", header[10], header[11], header[12], header[13],
                                                                                                                 header[14], header[15], header[16], header[17],
                                                                                                                 header[18], header[19]);

        LOG_OUT("    lt_get_info_fw_bank()  FW_BANK_FW2        %s\r\n", lt_ret_verbose(lt_get_info_fw_bank(&h, FW_BANK_FW2, header, LT_L2_GET_INFO_FW_HEADER_SIZE)));
        LOG_OUT("    Header:                                   %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\r\n", header[0], header[1], header[2], header[3],
                                                                                                                 header[4], header[5], header[6], header[7],
                                                                                                                 header[8], header[9]);
        LOG_OUT("                                              %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\r\n", header[10], header[11], header[12], header[13],
                                                                                                                 header[14], header[15], header[16], header[17],
                                                                                                                 header[18], header[19]);

        LOG_OUT("    lt_get_info_fw_bank()  FW_BANK_SPECT1     %s\r\n", lt_ret_verbose(lt_get_info_fw_bank(&h, FW_BANK_SPECT1, header, LT_L2_GET_INFO_FW_HEADER_SIZE)));
        LOG_OUT("    Header:                                   %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\r\n", header[0], header[1], header[2], header[3],
                                                                                                                 header[4], header[5], header[6], header[7],
                                                                                                                 header[8], header[9]);
        LOG_OUT("                                              %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\r\n", header[10], header[11], header[12], header[13],
                                                                                                                 header[14], header[15], header[16], header[17],
                                                                                                                 header[18], header[19]);

        LOG_OUT("    lt_get_info_fw_bank()  FW_BANK_SPECT2     %s\r\n", lt_ret_verbose(lt_get_info_fw_bank(&h, FW_BANK_SPECT2, header, LT_L2_GET_INFO_FW_HEADER_SIZE)));
        LOG_OUT("    Header:                                   %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\r\n", header[0], header[1], header[2], header[3],
                                                                                                                 header[4], header[5], header[6], header[7],
                                                                                                                 header[8], header[9]);
        LOG_OUT("                                              %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\r\n", header[10], header[11], header[12], header[13],
                                                                                                                  header[14], header[15], header[16], header[17],
                                                                                                                 header[18], header[19]);
}

void lt_ex_fw_update(void)
{
    // Reused variable
    uint8_t fw_ver[LT_L2_GET_INFO_RISCV_FW_SIZE] = {0};

    LOG_OUT("\r\n\n\n---------------------------------------------------------------------------\r\n");
    LOG_OUT("------------------------- FW UPDATE ---------------------------------------\r\n");
    LOG_OUT("---------------------------------------------------------------------------\r\n");

    lt_handle_t h = {0};

    lt_init(&h);

    LOG_OUT("lt_update_mode()                              %s\r\n", lt_ret_verbose(lt_update_mode(&h)));
    if(h.mode == LT_MODE_APP) {
        LOG_OUT("  Chip is executing CPU firmware\r\n");
        // App runs so we can see what firmwares are running
        LOG_OUT("lt_get_info_riscv_fw_ver()                    %s\r\n", lt_ret_verbose(lt_get_info_riscv_fw_ver(&h, fw_ver, LT_L2_GET_INFO_RISCV_FW_SIZE)));
        LOG_OUT("  riscv_fw_ver: %d.%d.%d    (+ unused %d)\r\n", fw_ver[3],fw_ver[2],fw_ver[1],fw_ver[0]);
        LOG_OUT("lt_get_info_spect_fw_ver()                    %s\r\n", lt_ret_verbose(lt_get_info_spect_fw_ver(&h, fw_ver, LT_L2_GET_INFO_SPECT_FW_SIZE)));
        LOG_OUT("  spect_fw_ver: %d.%d.%d    (+ unused %d)\r\n", fw_ver[3],fw_ver[2],fw_ver[1],fw_ver[0]);
        // Now reboot into STARTUP
        LOG_OUT("lt_reboot() reboot into STARTUP               %s\r\n", lt_ret_verbose(lt_reboot(&h, LT_L2_STARTUP_ID_MAINTENANCE)));
    }

    // Check again mode
    LOG_OUT("---------------------------------------------------------------------------\r\n");
    LOG_OUT("\r\nlt_update_mode()                              %s\r\n", lt_ret_verbose(lt_update_mode(&h)));
    if(h.mode == LT_MODE_STARTUP) {
        LOG_OUT("  Chip is executing bootloader\r\n");
        // Chip must be in startup mode now.
        // Get bootloader version by issuing "Read riscv fw version" request while chip is in maintenance:
        LOG_OUT("\r\n  lt_get_info_riscv_fw_ver()                  %s\r\n", lt_ret_verbose(lt_get_info_riscv_fw_ver(&h, fw_ver, LT_L2_GET_INFO_RISCV_FW_SIZE)));
        LOG_OUT("  Bootloader version: %d.%d.%d    (+ unused %d)\r\n", fw_ver[3] & 0x7f,fw_ver[2],fw_ver[1],fw_ver[0]);

        print_headers(&h);

        if(0) {
            // Erase firmware bank
            LOG_OUT("lt_mutable_fw_erase()                    %s\r\n", lt_ret_verbose(lt_mutable_fw_erase(&h, FW_BANK_FW1)));
            // Update firmware bank
            LOG_OUT("lt_mutable_fw_update()                   %s\r\n", lt_ret_verbose(lt_mutable_fw_update(&h, fw_CPU_0_1_2, fw_CPU_0_1_2_len, FW_BANK_FW1)));

            print_headers(&h);

        } else {
            LOG_OUT("Update disabled\r\n");
        }

    } else {
        LOG_OUT("     ERROR device couldn't get into STARTUP mode\r\n");
        return;
    }
    LOG_OUT("---------------------------------------------------------------------------\r\n");
    LOG_OUT("\r\nlt_reboot() reboot                            %s\r\n", lt_ret_verbose(lt_reboot(&h, LT_L2_STARTUP_ID_REBOOT)));
    LOG_OUT("lt_get_info_riscv_fw_ver()                    %s\r\n", lt_ret_verbose(lt_get_info_riscv_fw_ver(&h, fw_ver, LT_L2_GET_INFO_RISCV_FW_SIZE)));
    LOG_OUT("riscv_fw_ver: %d.%d.%d    (+ unused %d)\r\n", fw_ver[3],fw_ver[2],fw_ver[1],fw_ver[0]);
    LOG_OUT("lt_get_info_spect_fw_ver()                    %s\r\n", lt_ret_verbose(lt_get_info_spect_fw_ver(&h, fw_ver, LT_L2_GET_INFO_SPECT_FW_SIZE)));
    LOG_OUT("spect_fw_ver: %d.%d.%d    (+ unused %d)\r\n", fw_ver[3],fw_ver[2],fw_ver[1],fw_ver[0]);

    return;
}
