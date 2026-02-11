/**
 * @file lt_tr01_attrs.c
 * @brief Implementation for handling TROPIC01 attributes based on FW versions.
 * @copyright Copyright (c) 2020-2026 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include "lt_tr01_attrs.h"

#include <stdbool.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_logging.h"

#define LT_LATEST_RISCV_FW_VER_MAJOR 2
#define LT_LATEST_RISCV_FW_VER_MINOR 0
#define LT_LATEST_RISCV_FW_VER_PATCH 0

lt_ret_t lt_init_tr01_attrs(lt_handle_t *h)
{
#ifdef LT_REDUNDANT_ARG_CHECK
    if (!h) {
        return LT_PARAM_ERR;
    }
#endif

    lt_ret_t ret = LT_OK;
    lt_tr01_mode_t tr01_mode;
    uint8_t riscv_fw_ver[TR01_L2_GET_INFO_RISCV_FW_SIZE];
    bool reboot_to_startup_mode = false;  // Used to track if we need to reboot back to Start-up Mode
                                          // at the end of this function.

    // 1. Set some default dummy values for the attributes
    h->tr01_attrs.r_mem_udata_slot_size_max = 0;

    // 2. Get current TROPIC01's mode
    ret = lt_get_tr01_mode(h, &tr01_mode);
    if (ret != LT_OK) {
        return ret;
    }

    // 3. Reboot if TROPIC01 is not executing Application FW.
    if (tr01_mode != LT_TR01_APPLICATION) {
        ret = lt_reboot(h, TR01_REBOOT);
        if (ret != LT_OK) {
            return ret;
        }

        reboot_to_startup_mode = true;
    }

    // 4. Read Application FW version
    ret = lt_get_info_riscv_fw_ver(h, riscv_fw_ver);
    if (ret != LT_OK) {
        goto exit;
    }

    // 5. Check if the Application FW version is supported by the current version of libtropic
    // TODO: handle FW versions older than 1.0.0
    if (riscv_fw_ver[3] > LT_LATEST_RISCV_FW_VER_MAJOR ||
        (riscv_fw_ver[3] == LT_LATEST_RISCV_FW_VER_MAJOR &&
         riscv_fw_ver[2] > LT_LATEST_RISCV_FW_VER_MINOR) ||
        (riscv_fw_ver[3] == LT_LATEST_RISCV_FW_VER_MAJOR &&
         riscv_fw_ver[2] == LT_LATEST_RISCV_FW_VER_MINOR &&
         riscv_fw_ver[1] > LT_LATEST_RISCV_FW_VER_PATCH)) {
        ret = LT_APP_FW_TOO_NEW;
        goto exit;
    }

    // 6. Initialize the TROPIC01 attributes structure
    // this is the most crucial part - has to be efficient and logically correct
    if (riscv_fw_ver[3] < 2) {
        h->tr01_attrs.r_mem_udata_slot_size_max = 444;
    }
    else {
        h->tr01_attrs.r_mem_udata_slot_size_max = 475;
    }

// 7. Reboot back to Start-up Mode if we had to reboot in step 3.
exit:
    if (reboot_to_startup_mode) {
        lt_ret_t mtnc_reboot_ret = lt_reboot(h, TR01_MAINTENANCE_REBOOT);
        if (mtnc_reboot_ret != LT_OK) {
            LT_LOG_ERROR("Failed to reboot back to Start-up Mode, ret=%s",
                         lt_ret_verbose(mtnc_reboot_ret));
            return (ret == LT_OK) ? mtnc_reboot_ret : ret;  // return the first error that occurred.
        }
    }

    return ret;
}