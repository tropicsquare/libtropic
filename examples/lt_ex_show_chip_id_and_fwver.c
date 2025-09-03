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

static void print_header_boot_v1_0_1(uint8_t *data, lt_bank_id_t bank_id)
{
    struct lt_header_boot_v1_t *p_h = (struct lt_header_boot_v1_t *)data;
    switch (bank_id) {
        case FW_BANK_FW1:
            LT_LOG_INFO("    Firmware bank 1 header:");
            break;
        case FW_BANK_FW2:
            LT_LOG_INFO("    Firmware bank 2 header:");
            break;
        case FW_BANK_SPECT1:
            LT_LOG_INFO("    SPECT bank 1 header:");
            break;
        case FW_BANK_SPECT2:
            LT_LOG_INFO("    SPECT bank 2 header:");
            break;
        default:
            LT_LOG_ERROR("    Unknown bank ID: %d", (int)bank_id);
            return;
    }
    LT_LOG_INFO("      Type:      %02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8, p_h->type[3], p_h->type[2],
                p_h->type[1], p_h->type[0]);
    LT_LOG_INFO("      Version:   %02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8, p_h->version[3], p_h->version[2],
                p_h->version[1], p_h->version[0]);
    LT_LOG_INFO("      Size:      %02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8, p_h->size[3], p_h->size[2],
                p_h->size[1], p_h->size[0]);
    LT_LOG_INFO("      Git hash:  %02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8, p_h->git_hash[3], p_h->git_hash[2],
                p_h->git_hash[1], p_h->git_hash[0]);
    LT_LOG_INFO("      FW hash:   %02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8, p_h->hash[3], p_h->hash[2],
                p_h->hash[1], p_h->hash[0]);
}

static void print_all_headers_v1(lt_handle_t *h)
{
    uint8_t header[LT_L2_GET_INFO_FW_HEADER_SIZE] = {0};

    // Read header from FW_BANK_FW1
    LT_LOG_INFO("Reading firmware headers in bank %d", (int)FW_BANK_FW1);
    lt_ret_t ret = lt_get_info_fw_bank(h, FW_BANK_FW1, header, sizeof(header));
    if (ret == LT_OK) {
        print_header_boot_v1_0_1(header, FW_BANK_FW1);
    }
    else {
        LT_LOG_ERROR("Failed to get FW bank %d header, ret=%s", (int)FW_BANK_FW1, lt_ret_verbose(ret));
        return;
    }

    // Read header from FW_BANK_FW2
    LT_LOG_INFO("Reading firmware headers in bank %d", (int)FW_BANK_FW2);
    memset(header, 0, sizeof(header));
    ret = lt_get_info_fw_bank(h, FW_BANK_FW2, header, sizeof(header));
    if (ret == LT_OK) {
        print_header_boot_v1_0_1(header, FW_BANK_FW2);
    }
    else {
        LT_LOG_ERROR("Failed to get FW bank %d header, ret=%s", (int)FW_BANK_FW2, lt_ret_verbose(ret));
        return;
    }

    // Read header from FW_BANK_SPECT1
    LT_LOG_INFO("Reading SPECT headers in bank %d", (int)FW_BANK_SPECT1);
    memset(header, 0, sizeof(header));
    ret = lt_get_info_fw_bank(h, FW_BANK_SPECT1, header, sizeof(header));
    if (ret == LT_OK) {
        print_header_boot_v1_0_1(header, FW_BANK_SPECT1);
    }
    else {
        LT_LOG_ERROR("Failed to get SPECT bank %d header, ret=%s", (int)FW_BANK_SPECT1, lt_ret_verbose(ret));
        return;
    }

    // Read header from FW_BANK_SPECT2
    LT_LOG_INFO("Reading SPECT headers in bank %d", (int)FW_BANK_SPECT2);
    memset(header, 0, sizeof(header));
    ret = lt_get_info_fw_bank(h, FW_BANK_SPECT2, header, sizeof(header));
    if (ret == LT_OK) {
        print_header_boot_v1_0_1(header, FW_BANK_SPECT2);
    }
    else {
        LT_LOG_ERROR("Failed to get SPECT bank %d header, ret=%s", (int)FW_BANK_SPECT2, lt_ret_verbose(ret));
        return;
    }
}

// This function prints the header in the new format used in bootloader version 2.0.1
static void print_header_boot_v2_0_1(uint8_t *data, lt_bank_id_t bank_id)
{
    struct header_boot_v2_t *p_h = (struct header_boot_v2_t *)data;
    switch (bank_id) {
        case FW_BANK_FW1:
            LT_LOG_INFO("    Firmware bank 1 header:");
            break;
        case FW_BANK_FW2:
            LT_LOG_INFO("    Firmware bank 2 header:");
            break;
        case FW_BANK_SPECT1:
            LT_LOG_INFO("    SPECT bank 1 header:");
            break;
        case FW_BANK_SPECT2:
            LT_LOG_INFO("    SPECT bank 2 header:");
            break;
        default:
            LT_LOG_ERROR("    Unknown bank ID: %d", (int)bank_id);
            return;
    }
    LT_LOG_INFO("      Type:               %04" PRIX16, p_h->type);
    LT_LOG_INFO("      Padding:            %02" PRIX8, p_h->padding);
    LT_LOG_INFO("      FW header version:  %02" PRIX8, p_h->header_version);
    LT_LOG_INFO("      Version:            %08" PRIX32, p_h->ver);
    LT_LOG_INFO("      Size:               %08" PRIX32, p_h->size);
    LT_LOG_INFO("      Git hash:           %08" PRIX32, p_h->git_hash);
    // Hash str has 32B
    char hash_str[32 * 2 + 1] = {0};
    for (int i = 0; i < 32; i++) {
        snprintf(hash_str + i * 2, sizeof(hash_str) - i * 2, "%02" PRIX8 "", p_h->hash[i]);
    }
    LT_LOG_INFO("      Hash:          %s", hash_str);
    LT_LOG_INFO("      Pair version:  %08" PRIX32, p_h->pair_version);
}

static void print_all_fw_headers_v2(lt_handle_t *h)
{
    uint8_t header[LT_L2_GET_INFO_FW_HEADER_SIZE] = {0};

    // Read header from FW_BANK_FW1
    LT_LOG_INFO("Reading firmware headers in bank %d", (int)FW_BANK_FW1);
    lt_ret_t ret = lt_get_info_fw_bank(h, FW_BANK_FW1, header, sizeof(header));
    if (ret == LT_OK) {
        print_header_boot_v2_0_1(header, FW_BANK_FW1);
    }
    else {
        LT_LOG_ERROR("Failed to get FW bank %d header, ret=%s", (int)FW_BANK_FW1, lt_ret_verbose(ret));
        return;
    }

    // Read header from FW_BANK_FW2
    LT_LOG_INFO("Reading firmware headers in bank %d", (int)FW_BANK_FW2);
    memset(header, 0, sizeof(header));
    ret = lt_get_info_fw_bank(h, FW_BANK_FW2, header, sizeof(header));
    if (ret == LT_OK) {
        print_header_boot_v2_0_1(header, FW_BANK_FW2);
    }
    else {
        LT_LOG_ERROR("Failed to get FW bank %d header, ret=%s", (int)FW_BANK_FW2, lt_ret_verbose(ret));
        return;
    }

    // Read header from FW_BANK_SPECT1
    LT_LOG_INFO("Reading SPECT headers in bank %d", (int)FW_BANK_SPECT1);
    memset(header, 0, sizeof(header));
    ret = lt_get_info_fw_bank(h, FW_BANK_SPECT1, header, sizeof(header));
    if (ret == LT_OK) {
        print_header_boot_v2_0_1(header, FW_BANK_SPECT1);
    }
    else {
        LT_LOG_ERROR("Failed to get SPECT bank %d header, ret=%s", (int)FW_BANK_SPECT1, lt_ret_verbose(ret));
        return;
    }

    // Read header from FW_BANK_SPECT2
    LT_LOG_INFO("Reading SPECT headers in bank %d", (int)FW_BANK_SPECT2);
    memset(header, 0, sizeof(header));
    ret = lt_get_info_fw_bank(h, FW_BANK_SPECT2, header, sizeof(header));
    if (ret == LT_OK) {
        print_header_boot_v2_0_1(header, FW_BANK_SPECT2);
    }
    else {
        LT_LOG_ERROR("Failed to get SPECT bank %d header, ret=%s", (int)FW_BANK_SPECT2, lt_ret_verbose(ret));
        return;
    }
}

int lt_ex_show_chip_id_and_fwver(lt_handle_t *h)
{
    LT_LOG_INFO("=============================================================");
    LT_LOG_INFO("==== TROPIC01 show chip ID and firmware versions example ====");
    LT_LOG_INFO("=============================================================");

    // This variable is reused on more places in this example to store different firmware versions
    uint8_t fw_ver[LT_L2_GET_INFO_RISCV_FW_SIZE] = {0};

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
    ret = lt_reboot(h, LT_MODE_APP);
    if (ret != LT_OK) {
        LT_LOG_ERROR("lt_reboot() failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }

    if (h->l2.mode == LT_MODE_APP) {
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
    ret = lt_reboot(h, LT_MODE_MAINTENANCE);
    if (ret != LT_OK) {
        LT_LOG_ERROR("Failed to reboot into MAINTENANCE mode, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }

    if (h->l2.mode == LT_MODE_MAINTENANCE) {
        LT_LOG_INFO("Reading RISC-V FW version for bootloader version");
        ret = lt_get_info_riscv_fw_ver(h, fw_ver);
        if (ret != LT_OK) {
            LT_LOG_ERROR("Failed to get RISC-V FW version, ret=%s", lt_ret_verbose(ret));
            lt_deinit(h);
            return -1;
        }

        // Checking if bootloader version is 1.0.1
        if (((fw_ver[3] & 0x7f) == 1) && (fw_ver[2] == 0) && (fw_ver[1] == 1) && (fw_ver[0] == 0)) {
            LT_LOG_INFO("Bootloader version: %02" PRIX8 ".%02" PRIX8 ".%02" PRIX8 "    (+ .%02" PRIX8 ")",
                        fw_ver[3] & 0x7f, fw_ver[2], fw_ver[1], fw_ver[0]);

            print_all_headers_v1(h);
        }
        else {
            // Checking if bootloader version is 2.0.1
            if (((fw_ver[3] & 0x7f) == 2) && (fw_ver[2] == 0) && (fw_ver[1] == 1) && (fw_ver[0] == 0)) {
                LT_LOG_INFO("Bootloader version: %02" PRIX8 ".%02" PRIX8 ".%02" PRIX8 "    (+ .%02" PRIX8 ")",
                            fw_ver[3] & 0x7f, fw_ver[2], fw_ver[1], fw_ver[0]);
                print_all_fw_headers_v2(h);
            }
            else {
                LT_LOG_ERROR("Unknown bootloader version: %02" PRIX8 ".%02" PRIX8 ".%02" PRIX8 ".%02" PRIX8,
                             fw_ver[3] & 0x7f, fw_ver[2], fw_ver[1], fw_ver[0]);
                lt_deinit(h);
                return -1;
            }
            LT_LOG_LINE();
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

    LT_LOG_INFO("Deinitializing handle");
    ret = lt_deinit(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to deinitialize handle, ret=%s", lt_ret_verbose(ret));
        return -1;
    }

    return 0;
}
