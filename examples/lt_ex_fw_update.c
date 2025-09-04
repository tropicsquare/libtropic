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
 * @brief Defines the target firmware banks for this update example.
 *
 * This example is configured to update the following banks:
 * - Application (CPU) firmware: `TR01_FW_BANK_FW1`
 * - SPECT firmware:             `TR01_FW_BANK_SPECT1`
 *
 * @note Used only during ABAB update, ACAB update ignores firmware banks
 */
#define FW_APP_UPDATE_BANK TR01_FW_BANK_FW1
#define FW_SPECT_UPDATE_BANK TR01_FW_BANK_SPECT1

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

    // Reused variable
    uint8_t fw_ver[TR01_L2_GET_INFO_RISCV_FW_SIZE] = {0};

    // For firmware update chip must be rebooted into MAINTENANCE mode.
    LT_LOG_INFO("Rebooting into Maintenance mode");
    ret = lt_reboot(h, TR01_MODE_MAINTENANCE);
    if (ret != LT_OK) {
        LT_LOG_ERROR("lt_reboot() failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }

    if (h->l2.mode == TR01_MODE_MAINTENANCE) {
        LT_LOG_INFO("Chip is executing bootloader");

        LT_LOG_INFO("Updating RISC-V FW");
        ret = lt_do_mutable_fw_update(h, fw_CPU, sizeof(fw_CPU), FW_APP_UPDATE_BANK);
        if (ret != LT_OK) {
            LT_LOG_ERROR("RISC-V FW update failed, ret=%s", lt_ret_verbose(ret));
            lt_deinit(h);
            return -1;
        }

        LT_LOG_INFO("Updating SPECT FW");
        ret = lt_do_mutable_fw_update(h, fw_SPECT, sizeof(fw_SPECT), FW_SPECT_UPDATE_BANK);
        if (ret != LT_OK) {
            LT_LOG_ERROR("SPECT FW update failed, ret=%s", lt_ret_verbose(ret));
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

    // To read firmware versions chip must be rebooted into application mode.
    LT_LOG_INFO("Rebooting into Application mode");
    ret = lt_reboot(h, TR01_MODE_APP);
    if (ret != LT_OK) {
        LT_LOG_ERROR("lt_reboot() failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }

    if (h->l2.mode == TR01_MODE_APP) {
        LT_LOG_INFO("Reading RISC-V FW version");
        ret = lt_get_info_riscv_fw_ver(h, fw_ver);
        if (ret == LT_OK) {
            LT_LOG_INFO("Chip is executing RISC-V application FW version: %" PRIu8 ".%" PRIu8 ".%" PRIu8
                        "    (+ .%" PRIu8 ")",
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
            LT_LOG_INFO("Chip is executing SPECT FW version: %" PRIu8 ".%" PRIu8 ".%" PRIu8 "    (+ .%" PRIu8 ")",
                        fw_ver[3], fw_ver[2], fw_ver[1], fw_ver[0]);
        }
        else {
            LT_LOG_ERROR("Failed to get SPECT FW version, ret=%s", lt_ret_verbose(ret));
            lt_deinit(h);
            return -1;
        }
    }
    else {
        LT_LOG_ERROR(
            "Device couldn't get into APP mode, APP and SPECT firmwares in fw banks are not valid or banks are empty");
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
