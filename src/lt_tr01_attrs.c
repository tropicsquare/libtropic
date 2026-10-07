/**
 * @file lt_tr01_attrs.c
 * @brief Implementation for handling TROPIC01 attributes based on silicon revision and FW versions.
 * @copyright Copyright (c) 2020-2026 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include "lt_tr01_attrs.h"

#include <inttypes.h>
#include <string.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_logging.h"

#define LT_LATEST_RISCV_FW_VER_MAJOR 2
#define LT_LATEST_RISCV_FW_VER_MINOR 2
#define LT_LATEST_RISCV_FW_VER_PATCH 0

/**
 * @brief Determines TROPIC01's silicon revision from its CHIP_ID.
 *
 * @param chip_id      CHIP_ID read from TROPIC01
 * @param silicon_rev  Determined silicon revision
 * @retval             LT_OK Silicon revision determined successfully
 * @retval             LT_SILICON_REV_UNKNOWN Silicon revision is not known to Libtropic
 */
static lt_ret_t parse_silicon_rev(const struct lt_chip_id_t *chip_id,
                                  lt_tr01_silicon_rev_t *silicon_rev)
{
    // ABAB chips use chip id version 0.0.0.1 which does not have the silicon rev field
    bool is_ABAB = (chip_id->chip_id_ver[0] == 0 && chip_id->chip_id_ver[1] == 0 &&
                    chip_id->chip_id_ver[2] == 0 && chip_id->chip_id_ver[3] == 1);

    if (is_ABAB) {
        *silicon_rev = LT_TR01_ABAB;
        return LT_OK;
    }

    // number of bytes needs to be sizeof(chip_id->silicon_rev), as the literal is null-terminated
    if (!memcmp(chip_id->silicon_rev, "ACAB", sizeof(chip_id->silicon_rev))) {
        *silicon_rev = LT_TR01_ACAB;
        return LT_OK;
    }

    if (!memcmp(chip_id->silicon_rev, "BDBB", sizeof(chip_id->silicon_rev))) {
        *silicon_rev = LT_TR01_BDBB;
        return LT_OK;
    }

    LT_LOG_ERROR("Unknown silicon revision: 0x%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8,
                 chip_id->silicon_rev[0], chip_id->silicon_rev[1], chip_id->silicon_rev[2],
                 chip_id->silicon_rev[3]);
    return LT_SILICON_REV_UNKNOWN;
}

lt_ret_t lt_init_tr01_attrs(lt_handle_t *h)
{
#ifdef LT_REDUNDANT_ARG_CHECK
    if (!h) {
        return LT_PARAM_ERR;
    }
#endif

    lt_ret_t ret;
    struct lt_chip_id_t chip_id;
    uint8_t riscv_fw_ver[TR01_L2_GET_INFO_RISCV_FW_SIZE];

    // 1. Set some default dummy values for the attributes
    h->tr01_attrs.r_mem_udata_slot_size_max = 0;

    // 2. Read CHIP_ID and determine the silicon revision
    ret = lt_get_info_chip_id(h, &chip_id);
    if (ret != LT_OK) {
        return ret;
    }

    ret = parse_silicon_rev(&chip_id, &h->tr01_attrs.silicon_rev);
    if (ret != LT_OK) {
        return ret;
    }

    // 3. Read Application FW version
    ret = lt_get_info_riscv_fw_ver(h, riscv_fw_ver);
    if (ret != LT_OK) {
        return ret;
    }

    // 4. Check if the Application FW version is supported by the current version of libtropic
    // TODO: handle FW versions older than 1.0.0
    if (riscv_fw_ver[3] > LT_LATEST_RISCV_FW_VER_MAJOR ||
        (riscv_fw_ver[3] == LT_LATEST_RISCV_FW_VER_MAJOR &&
         riscv_fw_ver[2] > LT_LATEST_RISCV_FW_VER_MINOR) ||
        (riscv_fw_ver[3] == LT_LATEST_RISCV_FW_VER_MAJOR &&
         riscv_fw_ver[2] == LT_LATEST_RISCV_FW_VER_MINOR &&
         riscv_fw_ver[1] > LT_LATEST_RISCV_FW_VER_PATCH)) {
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
