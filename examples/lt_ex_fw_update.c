/**
 * @file lt_ex_fw_update.c
 * @name Firmware update
 * @brief This code performs firmware update of TROPIC01 chip.
 *
 * How to select the firmware for this example:
 *
 * The `TROPIC01_fw_update_files/` directory contains all officially released
 * and signed firmwares for the TROPIC01 chip, for all silicon revisions.
 *
 * To specify which firmware to use for the update:
 *   1. Open the `CMakeLists.txt` file.
 *   2. Set the `LT_SILICON_REV` variable to your chip's silicon revision (e.g., "ABAB").
 *   3. Set the `LT_CPU_FW_VERSION` and `LT_SPECT_FW_VERSION` to the desired versions.
 *
 * The build system uses these variables to include the correct firmware header files into produced binary containing
 * firmware update example.
 *
 * @note The firmware update process differs based on the silicon revision:
 *       - **ACAB Revision**: Automatically selects an inactive bank for the update.
 *       - **ABAB Revision**: Requires the target bank to be manually erased before writing
 *         the new firmware. This example demonstrates this by calling
 *         `lt_mutable_fw_erase()` before `lt_mutable_fw_update()`.
 *
 *       For the ABAB revision, it may be necessary to erase both sets of firmware banks
 *       (e.g., FW_BANK_FW1 and FW_BANK_FW2) to ensure a successful update, depending on
 *       their current state. This example only updates ABAB banks defined by
 *       `FW_APP_UPDATE_BANK` and `FW_SPECT_UPDATE_BANK`.
 *       We recommend reading the TROPIC01 datasheet for more details.
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
 * - Application (CPU) firmware: `FW_BANK_FW1`
 * - SPECT firmware:             `FW_BANK_SPECT1`
 *
 * @note The TROPIC01 chip contains internal logic to select which bank to boot
 * from after an update. For predictable behavior, particularly with the ABAB
 * silicon revision, it may be necessary to erase all firmware banks before
 * running this example.
 */
#define FW_APP_UPDATE_BANK FW_BANK_FW1
#define FW_SPECT_UPDATE_BANK FW_BANK_SPECT1

int lt_ex_fw_update(lt_handle_t *h)
{
    LT_LOG("\t=======================================================================");
    LT_LOG("\t=====  TROPIC01 FW update                                           ===");
    LT_LOG("\t=======================================================================");

    lt_ret_t ret = LT_FAIL;

    lt_init(h);

    // Reused variable
    uint8_t fw_ver[LT_L2_GET_INFO_RISCV_FW_SIZE] = {0};

    // For firmware update chip must be rebooted into MAINTENANCE mode.
    ret = lt_reboot(h, LT_MODE_MAINTENANCE);
    if (ret != LT_OK) {
        LT_LOG_ERROR("lt_reboot() failed, ret=%s", lt_ret_verbose(ret));
        return -1;
    }

    if (h->l2.mode == LT_MODE_MAINTENANCE) {
        LT_LOG("  Chip is executing bootloader");

#ifdef ABAB  // ABAB silicon revision needs bank to be erased first
        // Erase application firmware bank. Call following function twice with different arguments to erase both banks.
        LT_LOG("lt_mutable_fw_erase()                         %s",
               lt_ret_verbose(lt_mutable_fw_erase(h, FW_APP_UPDATE_BANK)));
#endif
        // Update APP firmware bank
        LT_LOG("lt_mutable_fw_update() APP                    %s",
               lt_ret_verbose(lt_mutable_fw_update(h, fw_CPU, sizeof(fw_CPU), FW_APP_UPDATE_BANK)));
#ifdef ABAB  // ABAB silicon revision needs bank to be erased first
        // Erase SPECT firmware bank. Call following function twice with different arguments to erase both banks.
        LT_LOG("lt_mutable_fw_erase()                         %s",
               lt_ret_verbose(lt_mutable_fw_erase(h, FW_SPECT_UPDATE_BANK)));
#endif
        // Update SPECT firmware bank
        LT_LOG("lt_mutable_fw_update() SPECT                  %s",
               lt_ret_verbose(lt_mutable_fw_update(h, fw_SPECT, sizeof(fw_SPECT), FW_SPECT_UPDATE_BANK)));
    }
    else {
        LT_LOG("     ERROR device couldn't get into MAINTENANCE mode");
        return -1;
    }
    LT_LOG_LINE();

    // To read firmware versions chip must be rebooted into application mode.
    ret = lt_reboot(h, LT_MODE_APP);
    if (ret != LT_OK) {
        LT_LOG_ERROR("lt_reboot() failed, ret=%s", lt_ret_verbose(ret));
        return -1;
    }

    if (h->l2.mode == LT_MODE_APP) {
        ret = lt_get_info_riscv_fw_ver(h, fw_ver, LT_L2_GET_INFO_RISCV_FW_SIZE);
        if (ret == LT_OK) {
            LT_LOG("  Chip is executing RISCV application firmware version: %d.%d.%d    (+ .%d)", fw_ver[3], fw_ver[2],
                   fw_ver[1], fw_ver[0]);
        }
        else {
            LT_LOG_ERROR("Failed to get RISCV firmware version, ret=%s", lt_ret_verbose(ret));
        }

        // Getting SPECT firmware version
        ret = lt_get_info_spect_fw_ver(h, fw_ver, LT_L2_GET_INFO_SPECT_FW_SIZE);
        if (ret == LT_OK) {
            LT_LOG("  Chip is executing SPECT firmware version:             %d.%d.%d    (+ .%d)", fw_ver[3], fw_ver[2],
                   fw_ver[1], fw_ver[0]);
        }
        else {
            LT_LOG_ERROR("Failed to get SPECT firmware version, ret=%s", lt_ret_verbose(ret));
        }
    }
    else {
        LT_LOG(
            "     Device couldn't get into APP mode, APP and SPECT firmwares in fw banks are not valid or banks are "
            "empty");
    }

    return 0;
}
