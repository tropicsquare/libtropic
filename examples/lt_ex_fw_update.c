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

void lt_ex_fw_update(void)
{
    LOG_OUT("\r\n\r\nFW update\r\n");

    lt_handle_t h = {0};

    lt_init(&h);

    uint8_t chip_status;
    LOG_OUT("lt_get_chip_status()                      %s\r\n", lt_ret_verbose(lt_get_chip_status(&h, &chip_status)));
    LOG_OUT("chip_status: READY %d, ALARM %d, STARTUP %d\r\n", (chip_status & 1), (chip_status & 2)>>1, (chip_status & 4)>>2);

    uint8_t riscv_fw_ver[LT_L2_GET_INFO_RISCV_FW_SIZE] = {0};
    LOG_OUT("lt_get_info_riscv_fw_ver()                %s\r\n", lt_ret_verbose(lt_get_info_riscv_fw_ver(&h, riscv_fw_ver, LT_L2_GET_INFO_RISCV_FW_SIZE)));
    LOG_OUT("riscv_fw_ver: %d.%d.%d    (+ unused %d)\r\n", riscv_fw_ver[3],riscv_fw_ver[2],riscv_fw_ver[1],riscv_fw_ver[0]);

    uint8_t spect_fw_ver[LT_L2_GET_INFO_SPECT_FW_SIZE] = {0};
    LOG_OUT("lt_get_info_spect_fw_ver()                %s\r\n", lt_ret_verbose(lt_get_info_spect_fw_ver(&h, spect_fw_ver, LT_L2_GET_INFO_SPECT_FW_SIZE)));
    LOG_OUT("spect_fw_ver: %d.%d.%d    (+ unused %d)\r\n", spect_fw_ver[3],spect_fw_ver[2],spect_fw_ver[1],spect_fw_ver[0]);

    LOG_OUT("lt_reboot() reboot into maintenance       %s\r\n", lt_ret_verbose(lt_reboot(&h, LT_L2_STARTUP_ID_MAINTENANCE)));
    LOG_OUT("lt_get_chip_status()                      %s\r\n", lt_ret_verbose(lt_get_chip_status(&h, &chip_status)));
    LOG_OUT("chip_status: READY %d, ALARM %d, STARTUP %d\r\n", (chip_status & 1), (chip_status & 2)>>1, (chip_status & 4)>>2);

    if((chip_status & 4)>>2) {
        // Request works only when chip is in STARTUP mode
        uint8_t header[LT_L2_GET_INFO_FW_HEADER_SIZE] = {0};
        LOG_OUT("lt_get_info_fw_bank()  FW_BANK_FW1        %s\r\n", lt_ret_verbose(lt_get_info_fw_bank(&h, FW_BANK_FW1, header, LT_L2_GET_INFO_FW_HEADER_SIZE)));
        LOG_OUT("Header:                                   %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\r\n", header[0], header[1], header[2], header[3],
                                                                                                                 header[4], header[5], header[6], header[7],
                                                                                                                 header[8], header[9]);
        LOG_OUT("                                          %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\r\n", header[10], header[11], header[12], header[13],
                                                                                                                 header[14], header[15], header[16], header[17],
                                                                                                                 header[18], header[19]);

        LOG_OUT("lt_get_info_fw_bank()  FW_BANK_FW2        %s\r\n", lt_ret_verbose(lt_get_info_fw_bank(&h, FW_BANK_FW2, header, LT_L2_GET_INFO_FW_HEADER_SIZE)));
        LOG_OUT("Header:                                   %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\r\n", header[0], header[1], header[2], header[3],
                                                                                                                 header[4], header[5], header[6], header[7],
                                                                                                                 header[8], header[9]);
        LOG_OUT("                                          %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\r\n", header[10], header[11], header[12], header[13],
                                                                                                                 header[14], header[15], header[16], header[17],
                                                                                                                 header[18], header[19]);

        LOG_OUT("lt_get_info_fw_bank()  FW_BANK_SPECT1     %s\r\n", lt_ret_verbose(lt_get_info_fw_bank(&h, FW_BANK_SPECT1, header, LT_L2_GET_INFO_FW_HEADER_SIZE)));
        LOG_OUT("Header:                                   %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\r\n", header[0], header[1], header[2], header[3],
                                                                                                                 header[4], header[5], header[6], header[7],
                                                                                                                 header[8], header[9]);
        LOG_OUT("                                          %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\r\n", header[10], header[11], header[12], header[13],
                                                                                                                 header[14], header[15], header[16], header[17],
                                                                                                                 header[18], header[19]);

        LOG_OUT("lt_get_info_fw_bank()  FW_BANK_SPECT2     %s\r\n", lt_ret_verbose(lt_get_info_fw_bank(&h, FW_BANK_SPECT2, header, LT_L2_GET_INFO_FW_HEADER_SIZE)));
        LOG_OUT("Header:                                   %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\r\n", header[0], header[1], header[2], header[3],
                                                                                                                 header[4], header[5], header[6], header[7],
                                                                                                                 header[8], header[9]);
        LOG_OUT("                                          %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\r\n", header[10], header[11], header[12], header[13],
                                                                                                                 header[14], header[15], header[16], header[17],
                                                                                                                 header[18], header[19]);

        // Erase firmware bank
        LOG_OUT("lt_mutable_fw_erase()                    %s\r\n", lt_ret_verbose(lt_mutable_fw_erase(&h, FW_BANK_FW1)));
        // Update firmware bank
        LOG_OUT("lt_mutable_fw_update()                   %s\r\n", lt_ret_verbose(lt_mutable_fw_update(&h, fw_CPU_0_1_2, fw_CPU_0_1_2_len, FW_BANK_FW1)));

       LOG_OUT("lt_reboot() reboot                        %s\r\n", lt_ret_verbose(lt_reboot(&h, LT_L2_STARTUP_ID_REBOOT)));
    } else {
        LOG_OUT("Something is wrong, chip_status: READY %d, ALARM %d, STARTUP %d\r\n", (chip_status & 1), (chip_status & 2)>>1, (chip_status & 4)>>2);
        return;
    }

    LOG_OUT("lt_get_info_riscv_fw_ver()                %s\r\n", lt_ret_verbose(lt_get_info_riscv_fw_ver(&h, riscv_fw_ver, LT_L2_GET_INFO_RISCV_FW_SIZE)));
    LOG_OUT("riscv_fw_ver: %d.%d.%d    (+ unused %d)\r\n", riscv_fw_ver[3],riscv_fw_ver[2],riscv_fw_ver[1],riscv_fw_ver[0]);
    LOG_OUT("lt_get_info_spect_fw_ver()                %s\r\n", lt_ret_verbose(lt_get_info_spect_fw_ver(&h, spect_fw_ver, LT_L2_GET_INFO_SPECT_FW_SIZE)));
    LOG_OUT("spect_fw_ver: %d.%d.%d    (+ unused %d)\r\n", spect_fw_ver[3],spect_fw_ver[2],spect_fw_ver[1],spect_fw_ver[0]);

    return;
}

