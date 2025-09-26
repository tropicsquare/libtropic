/**
 * @file lt_ex_fw_update.c
 * @name Firmware update
 * @brief This code performs firmware update of TROPIC01 chip, works on both ABAB and ACAB silicon revisions.
 *
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <inttypes.h>

#include "fw_CPU.h"
#include "fw_SPECT.h"
#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_examples.h"
#include "libtropic_logging.h"
#include "string.h"

/**
 * @brief Example how to update TROPIC01 firmware. Process is described in detail in 'ODN_TR01_app_007_fw_update.pdf'
 * Application Note.
 *
 * It is recommended to update both Application and SPECT firmware banks with the same firmware.
 *
 * @param h Pointer to lt_handle_t structure
 * @returns 0 on success, -1 otherwise
 */

int lt_ex_fw_update(lt_handle_t *h)
{
    LT_LOG_INFO("====================================");
    LT_LOG_INFO("==== TROPIC01 FW update Example ====");
    LT_LOG_INFO("====================================");

    lt_ret_t ret;

    LT_LOG_INFO("Initializing handle");
    ret = lt_init(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to initialize handle, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }

    // The chip must be rebooted into MAINTENANCE mode to perform a firmware update.
    LT_LOG_LINE();
    LT_LOG_INFO("Updating TR01_FW_BANK_FW1 and TR01_FW_BANK_SPECT1");
    LT_LOG_INFO("Rebooting into Maintenance mode");
    ret = lt_reboot(h, TR01_MODE_MAINTENANCE);
    if (ret != LT_OK) {
        LT_LOG_ERROR("lt_reboot() failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }

    if (h->l2.mode == TR01_MODE_MAINTENANCE) {
        LT_LOG_INFO("Chip is in maintenance mode, executing bootloader");
        LT_LOG_INFO("Updating RISC-V FW");
        ret = lt_do_mutable_fw_update(h, fw_CPU, sizeof(fw_CPU), TR01_FW_BANK_FW1);
        if (ret != LT_OK) {
            LT_LOG_ERROR("RISC-V FW update failed, ret=%s", lt_ret_verbose(ret));
            lt_deinit(h);
            return -1;
        }
        LT_LOG_INFO("OK");

        LT_LOG_INFO("Updating SPECT FW");
        ret = lt_do_mutable_fw_update(h, fw_SPECT, sizeof(fw_SPECT), TR01_FW_BANK_SPECT1);
        if (ret != LT_OK) {
            LT_LOG_ERROR("SPECT FW update failed, ret=%s", lt_ret_verbose(ret));
            lt_deinit(h);
            return -1;
        }
        LT_LOG_INFO("OK");
    }
    else {
        LT_LOG_ERROR("Chip couldn't get into MAINTENANCE mode");
        lt_deinit(h);
        return -1;
    }
    LT_LOG_LINE();

    // The chip must be rebooted into MAINTENANCE mode to perform a firmware update.
    LT_LOG_INFO("Updating TR01_FW_BANK_FW2 and TR01_FW_BANK_SPECT2");
    LT_LOG_INFO("Rebooting into Maintenance mode");
    ret = lt_reboot(h, TR01_MODE_MAINTENANCE);
    if (ret != LT_OK) {
        LT_LOG_ERROR("lt_reboot() failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }

    if (h->l2.mode == TR01_MODE_MAINTENANCE) {
        LT_LOG_INFO("Chip is in maintenance mode, executing bootloader");
        LT_LOG_INFO("Updating RISC-V FW");
        ret = lt_do_mutable_fw_update(h, fw_CPU, sizeof(fw_CPU), TR01_FW_BANK_FW2);
        if (ret != LT_OK) {
            LT_LOG_ERROR("RISC-V FW update failed, ret=%s", lt_ret_verbose(ret));
            lt_deinit(h);
            return -1;
        }
        else {
            LT_LOG_INFO("OK");
        }

        LT_LOG_INFO("Updating SPECT FW");
        ret = lt_do_mutable_fw_update(h, fw_SPECT, sizeof(fw_SPECT), TR01_FW_BANK_SPECT2);
        if (ret != LT_OK) {
            LT_LOG_ERROR("SPECT FW update failed, ret=%s", lt_ret_verbose(ret));
            lt_deinit(h);
            return -1;
        }
        else {
            LT_LOG_INFO("OK");
        }
    }
    else {
        LT_LOG_ERROR("Chip couldn't get into MAINTENANCE mode");
        lt_deinit(h);
        return -1;
    }

    LT_LOG_LINE();
    LT_LOG("Successfully updated all 4 FW banks");
    LT_LOG_LINE();

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
    LT_LOG_LINE();

    LT_LOG_INFO("Rebooting into Application mode");
    ret = lt_reboot(h, TR01_MODE_APP);
    if (ret != LT_OK) {
        LT_LOG_ERROR("lt_reboot() failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }

    if (h->l2.mode == TR01_MODE_APP) {
        LT_LOG_INFO("Chip is executing firmwares of following versions:");
        LT_LOG_INFO("Reading RISC-V FW version");
        // This variable is reused on more places in this block to store different firmware versions
        uint8_t fw_ver[TR01_L2_GET_INFO_RISCV_FW_SIZE] = {0};

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
        lt_deinit(h);
        return -1;
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Deinitializing handle");
    ret = lt_deinit(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to deinitialize handle, ret=%s", lt_ret_verbose(ret));
        return -1;
    }

    return 0;
}
