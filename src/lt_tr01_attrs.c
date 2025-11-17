
#include "lt_tr01_attrs.h"

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_logging.h"

#define LT_LATEST_RISCV_FW_VER_MAJOR 2
#define LT_LATEST_RISCV_FW_VER_MINOR 0
#define LT_LATEST_RISCV_FW_VER_PATCH 0

lt_ret_t lt_init_tr01_attrs(lt_handle_t *h)
{
    lt_ret_t ret;
    uint8_t riscv_fw_ver[TR01_L2_GET_INFO_RISCV_FW_SIZE];

    // 1. Set some default dummy values for the attributes
    h->tr01_attrs.r_mem_udata_slot_size_max = 0;

    // 2. Check current mode
    ret = lt_update_mode(h);
    if (ret != LT_OK) {
        return ret;
    }

    switch (h->l2.mode) {
        case LT_TR01_APP_MODE:
            break;

        case LT_TR01_MAINTENANCE_MODE:
            // Reboot into App mode
            ret = lt_reboot(h, TR01_REBOOT);
            if (ret != LT_OK) {
                return ret;
            }

            // Rebooting into App mode might fail if there is an invalid App FW.
            if (h->l2.mode != LT_TR01_APP_MODE) {
                LT_LOG_WARN("There is not a valid Application FW in the chip.");
                return LT_OK;
            }
            break;

        default:
            LT_LOG_ERROR("Unknown CHIP_STATUS.");
            return LT_FAIL;
    }

    // 3. Read App version
    ret = lt_get_info_riscv_fw_ver(h, riscv_fw_ver);
    if (ret != LT_OK) {
        return ret;
    }

    // 4. Check if the App FW version is supported by the current version of libtropic
    // TODO: handle FW versions older than 1.0.0
    if (riscv_fw_ver[3] > LT_LATEST_RISCV_FW_VER_MAJOR || riscv_fw_ver[2] > LT_LATEST_RISCV_FW_VER_MINOR
        || riscv_fw_ver[1] > LT_LATEST_RISCV_FW_VER_PATCH) {
        return LT_APP_FW_TOO_NEW;
    }

    // 5. Initialize the TROPIC01 attributes structure
    // this is the most crucial part - has to be efficient and logically correct
    if (riscv_fw_ver[3] < 2) {
        h->tr01_attrs.r_mem_udata_slot_size_max = 444;
    }
    else {
        h->tr01_attrs.r_mem_udata_slot_size_max = 475;
    }

    return LT_OK;
}