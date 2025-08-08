/**
 * @file lt_ex_fw_update.c
 * @name Firmware update
 * @brief This code performs firmware update of TROPIC01 chip. Use defines at the beginning of this file to specify
 * firmware version and slot.
 *
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


// In TROPIC01_fw_update_files/ there are all officially released and signed firmwares for TROPIC01 chip, for all
// silicon revisions. Header files containing the firmware update bytes have all the same name, switching happens in
// CMakeFiles.txt, where it can be defined which firmware will be used use based on path to this header.
// Alternatively, firmware update files are there also in binary form, which can be used on devices with filesystem, but
// this is covered by different example.
#include "fw_CPU.h"
#include "fw_SPECT.h"

/** @brief Define firmware banks to be used during update
 * App fw bank FW_BANK_FW1 will be erased an firmware will be installed there.
 * SPECT fw bank FW_BANK_SPECT1 will be erased an firmware will be installed there.
 *
 * Please note that chip has own application logic which from bank is used to boot.
 * For correct behaviour it might be necessary to erase all banks prior running this example.
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

#ifdef ABAB // ABAB silicon revision needs bank to be erased first
        // Erase application firmware bank
        LT_LOG("lt_mutable_fw_erase()                         %s",
               lt_ret_verbose(lt_mutable_fw_erase(h, FW_APP_UPDATE_BANK)));
#endif
        // Update APP firmware bank
        LT_LOG("lt_mutable_fw_update() APP                    %s",
               lt_ret_verbose(lt_mutable_fw_update(h, fw_CPU, sizeof(fw_CPU), FW_APP_UPDATE_BANK)));
#ifdef ABAB // ABAB silicon revision needs bank to be erased first
        // Erase SPECT firmware bank
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
