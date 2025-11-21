/**
 * @file lt_tr01_attrs.c
 * @brief Implementation for handling TROPIC01 attributes based on FW versions.
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "lt_tr01_attrs.h"

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

    lt_ret_t ret;
    lt_tr01_status_t tr01_status;
    uint8_t riscv_fw_ver[TR01_L2_GET_INFO_RISCV_FW_SIZE];

    // 1. Set some default dummy values for the attributes
    h->tr01_attrs.r_mem_udata_slot_size_max = 0;

    // 2. Get current TROPIC01's status
    ret = lt_get_tr01_status(h, &tr01_status);
    if (ret != LT_OK) {
        return ret;
    }

    // 3. Reboot into Idle Mode if TROPIC01's status is not READY.
    // When TROPIC01 is in Idle Mode, we can be sure that it is executing the Application FW.
    if (tr01_status != TR01_READY) {
        ret = lt_reboot(h, TR01_REBOOT);
        if (ret != LT_OK) {
            return ret;
        }
    }

    // 4. Read Application FW version
    ret = lt_get_info_riscv_fw_ver(h, riscv_fw_ver);
    if (ret != LT_OK) {
        return ret;
    }

    // 5. Check if the Application FW version is supported by the current version of libtropic
    // TODO: handle FW versions older than 1.0.0
    if (riscv_fw_ver[3] > LT_LATEST_RISCV_FW_VER_MAJOR
        || (riscv_fw_ver[3] == LT_LATEST_RISCV_FW_VER_MAJOR && riscv_fw_ver[2] > LT_LATEST_RISCV_FW_VER_MINOR)
        || (riscv_fw_ver[3] == LT_LATEST_RISCV_FW_VER_MAJOR && riscv_fw_ver[2] == LT_LATEST_RISCV_FW_VER_MINOR
            && riscv_fw_ver[1] > LT_LATEST_RISCV_FW_VER_PATCH)) {
        return LT_APP_FW_TOO_NEW;
    }

    // 6. Initialize the TROPIC01 attributes structure
    // this is the most crucial part - has to be efficient and logically correct
    if (riscv_fw_ver[3] < 2) {
        h->tr01_attrs.r_mem_udata_slot_size_max = 444;
    }
    else {
        h->tr01_attrs.r_mem_udata_slot_size_max = 475;
    }

    return LT_OK;
}