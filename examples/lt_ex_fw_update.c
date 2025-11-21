/**
 * @file lt_ex_fw_update.c
 * @name Firmware update
 * @brief This code performs firmware update of TROPIC01 chip, works on both ABAB and ACAB silicon revisions.
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
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

int lt_ex_fw_update(lt_handle_t *h)
{
    LT_LOG_INFO("====================================");
    LT_LOG_INFO("==== TROPIC01 FW update Example ====");
    LT_LOG_INFO("====================================");

    lt_ret_t ret;

    LT_LOG_INFO("Initializing handle");
    // Note: It is assumed that the `h.l2.device` and `h.l3.crypto_ctx` members were already
    // initialized. Because these members are pointers, the assigned structures must exist throughout the whole
    // life-cycle of the handle. Refer to the 'Get Started'->'Integrating Libtropic'->'How to Use' Section in the
    // Libtropic documentation for more information.
    ret = lt_init(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to initialize handle, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }

    // The chip must be in Start-up Mode to be able to perform a firmware update.
    LT_LOG_LINE();
    LT_LOG_INFO("1. Doing a Maintenance restart upon which TROPIC01's FW can be updated");
    ret = lt_reboot(h, TR01_MAINTENANCE_REBOOT);
    if (ret != LT_OK) {
        LT_LOG_ERROR("lt_reboot() failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }
    LT_LOG_INFO("OK");

    LT_LOG_LINE();
    LT_LOG_INFO("2. Updating TR01_FW_BANK_FW1 and TR01_FW_BANK_SPECT1");
    LT_LOG_INFO("2.1. Updating RISC-V FW");
    ret = lt_do_mutable_fw_update(h, fw_CPU, sizeof(fw_CPU), TR01_FW_BANK_FW1);
    if (ret != LT_OK) {
        LT_LOG_ERROR("RISC-V FW update failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }
    LT_LOG_INFO("OK");

    LT_LOG_INFO("2.2. Updating SPECT FW");
    ret = lt_do_mutable_fw_update(h, fw_SPECT, sizeof(fw_SPECT), TR01_FW_BANK_SPECT1);
    if (ret != LT_OK) {
        LT_LOG_ERROR("SPECT FW update failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }
    LT_LOG_INFO("OK");

    LT_LOG_LINE();
    LT_LOG_INFO("3. Updating TR01_FW_BANK_FW2 and TR01_FW_BANK_SPECT2");
    LT_LOG_INFO("3.1. Updating RISC-V FW");
    ret = lt_do_mutable_fw_update(h, fw_CPU, sizeof(fw_CPU), TR01_FW_BANK_FW2);
    if (ret != LT_OK) {
        LT_LOG_ERROR("RISC-V FW update failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }
    LT_LOG_INFO("OK");

    LT_LOG_INFO("3.2. Updating SPECT FW");
    ret = lt_do_mutable_fw_update(h, fw_SPECT, sizeof(fw_SPECT), TR01_FW_BANK_SPECT2);
    if (ret != LT_OK) {
        LT_LOG_ERROR("SPECT FW update failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }
    LT_LOG_INFO("OK");

    LT_LOG_LINE();
    LT_LOG("Successfully updated all 4 FW banks:");
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

    LT_LOG_INFO("Doing a normal restart upon which TROPIC01 will begin to execute Application FW");
    ret = lt_reboot(h, TR01_REBOOT);
    if (ret != LT_OK) {
        LT_LOG_ERROR("lt_reboot() failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }
    LT_LOG_INFO("OK");

    LT_LOG_INFO("Reading RISC-V FW version");
    // This variable is reused on more places in this block to store different FW versions
    uint8_t fw_ver[TR01_L2_GET_INFO_RISCV_FW_SIZE] = {0};
    ret = lt_get_info_riscv_fw_ver(h, fw_ver);
    if (ret != LT_OK) {
        LT_LOG_ERROR("Failed to get RISC-V FW version, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }
    LT_LOG_INFO("OK");

    LT_LOG_INFO("Reading SPECT FW version");
    ret = lt_get_info_spect_fw_ver(h, fw_ver);
    if (ret != LT_OK) {
        LT_LOG_ERROR("Failed to get SPECT FW version, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }
    LT_LOG_INFO("OK");

    LT_LOG_LINE();
    LT_LOG_INFO("TROPIC01 is executing FW with the following versions:");
    LT_LOG_INFO("RISC-V FW version: %02" PRIX8 ".%02" PRIX8 ".%02" PRIX8 " (+ .%02" PRIX8 ")", fw_ver[3], fw_ver[2],
                fw_ver[1], fw_ver[0]);
    LT_LOG_INFO("SPECT FW version: %02" PRIX8 ".%02" PRIX8 ".%02" PRIX8 " (+ .%02" PRIX8 ")", fw_ver[3], fw_ver[2],
                fw_ver[1], fw_ver[0]);

    LT_LOG_LINE();
    LT_LOG_INFO("Deinitializing handle");
    ret = lt_deinit(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to deinitialize handle, ret=%s", lt_ret_verbose(ret));
        return -1;
    }

    return 0;
}
