/**
 * @file lt_ex_show_chip_id_and_fwver.c
 * @name Show chip ID and firmware versions
 * @brief This example shows how to read TROPIC01's chip ID and firmware versions
 * @note We recommend reading TROPIC01's datasheet before diving into this example!
 *
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <inttypes.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_examples.h"
#include "libtropic_logging.h"
#include "string.h"

int lt_ex_show_chip_id_and_fwver(lt_handle_t *h)
{
    LT_LOG_INFO("=============================================================");
    LT_LOG_INFO("==== TROPIC01 show chip ID and firmware versions example ====");
    LT_LOG_INFO("=============================================================");

    // This variable is reused on more places in this example to store different firmware versions
    uint8_t fw_ver[TR01_L2_GET_INFO_RISCV_FW_SIZE] = {0};

    lt_ret_t ret = lt_init(h);
    if (ret != LT_OK) {
        LT_LOG_ERROR("Failed to initialize handle, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }

    // First we check versions of both updateable firmwares. For this we need to be in APPLICATION mode.
    // If there are valid firmwares, chip will execute them on boot. In any case we will try to reboot into application,
    // in case chip would be in maintenance mode (executing bootloader)
    LT_LOG_INFO("Rebooting into APPLICATION mode to check FW versions");
    ret = lt_reboot(h, TR01_REBOOT);
    if (ret != LT_OK) {
        LT_LOG_ERROR("lt_reboot() failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }

    if (h->l2.mode == LT_TR01_APP_MODE) {
        // App runs so we can see what firmwares are running
        // Getting RISCV app firmware version
        LT_LOG_INFO("Reading RISC-V FW version");
        ret = lt_get_info_riscv_fw_ver(h, fw_ver);
        if (ret == LT_OK) {
            LT_LOG_INFO("Chip is executing RISC-V application FW version: %02" PRIX8 ".%02" PRIX8 ".%02" PRIX8
                        "    (+ .%02" PRIX8 ")",
                        fw_ver[3], fw_ver[2], fw_ver[1], fw_ver[0]);
        }
        else {
            LT_LOG_ERROR("Failed to get RISC-V FW version, ret=%s", lt_ret_verbose(ret));
            lt_deinit(h);
            return -1;
        }

        LT_LOG_INFO("Reading SPECT FW version");
        ret = lt_get_info_spect_fw_ver(h, fw_ver);
        if (ret == LT_OK) {
            LT_LOG_INFO("Chip is executing SPECT firmware version: %02" PRIX8 ".%02" PRIX8 ".%02" PRIX8
                        "    (+ .%02" PRIX8 ")",
                        fw_ver[3], fw_ver[2], fw_ver[1], fw_ver[0]);
        }
        else {
            LT_LOG_ERROR("Failed to get SPECT firmware version, ret=%s", lt_ret_verbose(ret));
            lt_deinit(h);
            return -1;
        }
    }
    else {
        LT_LOG_ERROR("Chip couldn't get into APP mode, APP and SPECT firmwares in fw banks are not valid");
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Rebooting into MAINTENANCE mode to check bootloader version and fw bank headers");
    ret = lt_reboot(h, TR01_MAINTENANCE_REBOOT);
    if (ret != LT_OK) {
        LT_LOG_ERROR("Failed to reboot into MAINTENANCE mode, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }

    if (h->l2.mode == LT_TR01_MAINTENANCE_MODE) {
        LT_LOG_INFO("Reading RISC-V FW version (during maintenance chip actually returns bootloader version):");
        ret = lt_get_info_riscv_fw_ver(h, fw_ver);
        if (ret != LT_OK) {
            LT_LOG_ERROR("Failed to get RISC-V FW version, ret=%s", lt_ret_verbose(ret));
            lt_deinit(h);
            return -1;
        }

        LT_LOG_INFO("Bootloader version: %02" PRIX8 ".%02" PRIX8 ".%02" PRIX8 "    (+ .%02" PRIX8 ")", fw_ver[3] & 0x7f,
                    fw_ver[2], fw_ver[1], fw_ver[0]);
        LT_LOG_LINE();

        LT_LOG_INFO("Reading and printing headers of all 4 FW banks:");
        ret = lt_print_fw_header(h, TR01_FW_BANK_FW1, printf);
        if (ret != LT_OK) {
            LT_LOG_ERROR("Failed to print TR01_FW_BANK_FW1 header, ret=%s", lt_ret_verbose(ret));
            lt_deinit(h);
            return -1;
        }
        ret = lt_print_fw_header(h, TR01_FW_BANK_FW2, printf);
        if (ret != LT_OK) {
            LT_LOG_ERROR("Failed to print TR01_FW_BANK_FW2 header, ret=%s", lt_ret_verbose(ret));
            lt_deinit(h);
            return -1;
        }
        ret = lt_print_fw_header(h, TR01_FW_BANK_SPECT1, printf);
        if (ret != LT_OK) {
            LT_LOG_ERROR("Failed to print TR01_FW_BANK_SPECT1 header, ret=%s", lt_ret_verbose(ret));
            lt_deinit(h);
            return -1;
        }
        ret = lt_print_fw_header(h, TR01_FW_BANK_SPECT2, printf);
        if (ret != LT_OK) {
            LT_LOG_ERROR("Failed to print TR01_FW_BANK_SPECT2 header, ret=%s", lt_ret_verbose(ret));
            lt_deinit(h);
            return -1;
        }
    }
    else {
        LT_LOG_ERROR("Chip couldn't get into MAINTENANCE mode");
        lt_deinit(h);
        return -1;
    }
    LT_LOG_LINE();

    struct lt_chip_id_t chip_id = {0};

    LT_LOG_INFO("Reading Chip ID:");
    ret = lt_get_info_chip_id(h, &chip_id);
    if (ret != LT_OK) {
        LT_LOG_ERROR("Failed to get chip ID, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }

    ret = lt_print_chip_id(&chip_id, printf);
    if (ret != LT_OK) {
        LT_LOG_ERROR("Failed to print chip ID, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Rebooting back into APPLICATION mode");
    ret = lt_reboot(h, TR01_REBOOT);
    if (ret != LT_OK) {
        LT_LOG_ERROR("lt_reboot() failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }

    LT_LOG_INFO("Deinitializing handle");
    ret = lt_deinit(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to deinitialize handle, ret=%s", lt_ret_verbose(ret));
        return -1;
    }

    return 0;
}
