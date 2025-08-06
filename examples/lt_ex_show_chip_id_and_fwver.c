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

#include "inttypes.h"
#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_examples.h"
#include "libtropic_logging.h"
#include "string.h"

static void print_header_boot_v1_0_1(uint8_t *data, bank_id_t bank_id)
{
    struct header_boot_v1_t *p_h = (struct header_boot_v1_t *)data;
    switch (bank_id) {
        case FW_BANK_FW1:
            LT_LOG("    Firmware bank 1 header:");
            break;
        case FW_BANK_FW2:
            LT_LOG("    Firmware bank 2 header:");
            break;
        case FW_BANK_SPECT1:
            LT_LOG("    SPECT bank 1 header:");
            break;
        case FW_BANK_SPECT2:
            LT_LOG("    SPECT bank 2 header:");
            break;
        default:
            LT_LOG_ERROR("Unknown bank ID: %d", bank_id);
            return;
    }
    LT_LOG("      Type:                           %02X%02X%02X%02X", p_h->type[3], p_h->type[2], p_h->type[1],
           p_h->type[0]);
    LT_LOG("      Version:                        %02X%02X%02X%02X", p_h->version[3], p_h->version[2], p_h->version[1],
           p_h->version[0]);
    LT_LOG("      Size:                           %02X%02X%02X%02X", p_h->size[3], p_h->size[2], p_h->size[1],
           p_h->size[0]);
    LT_LOG("      Git hash:                       %02X%02X%02X%02X", p_h->git_hash[3], p_h->git_hash[2],
           p_h->git_hash[1], p_h->git_hash[0]);
    LT_LOG("      FW hash:                        %02X%02X%02X%02X", p_h->hash[3], p_h->hash[2], p_h->hash[1],
           p_h->hash[0]);
}

static void print_all_headers_v1(lt_handle_t *h)
{
    LT_LOG("  Reading firmware headers in all banks...");
    uint8_t header[LT_L2_GET_INFO_FW_HEADER_SIZE] = {0};

    // Read header from FW_BANK_FW1
    memset(header, 0, sizeof(header));
    lt_ret_t ret = lt_get_info_fw_bank(h, FW_BANK_FW1, header, sizeof(header));
    if (ret == LT_OK) {
        print_header_boot_v1_0_1(header, FW_BANK_FW1);
    }
    else {
        LT_LOG_ERROR("Failed to get FW bank 1 header, ret=%s", lt_ret_verbose(ret));
    }

    // Read header from FW_BANK_FW2
    memset(header, 0, sizeof(header));
    ret = lt_get_info_fw_bank(h, FW_BANK_FW2, header, sizeof(header));
    if (ret == LT_OK) {
        print_header_boot_v1_0_1(header, FW_BANK_FW2);
    }
    else {
        LT_LOG_ERROR("Failed to get FW bank 2 header, ret=%s", lt_ret_verbose(ret));
    }

    // Read header from FW_BANK_SPECT1
    memset(header, 0, sizeof(header));
    ret = lt_get_info_fw_bank(h, FW_BANK_SPECT1, header, sizeof(header));
    if (ret == LT_OK) {
        print_header_boot_v1_0_1(header, FW_BANK_SPECT1);
    }
    else {
        LT_LOG_ERROR("Failed to get SPECT bank 1 header, ret=%s", lt_ret_verbose(ret));
    }

    // Read header from FW_BANK_SPECT2
    memset(header, 0, sizeof(header));
    ret = lt_get_info_fw_bank(h, FW_BANK_SPECT2, header, sizeof(header));
    if (ret == LT_OK) {
        print_header_boot_v1_0_1(header, FW_BANK_SPECT2);
    }
    else {
        LT_LOG_ERROR("Failed to get SPECT bank 2 header, ret=%s", lt_ret_verbose(ret));
    }
}

// This function prints the header in the new format used in bootloader version 2.0.1
static void print_header_v2(void *data, bank_id_t bank_id)
{
    struct header_boot_v2_t *p_h = (struct header_boot_v2_t *)data;
    switch (bank_id) {
        case FW_BANK_FW1:
            LT_LOG("    Firmware bank 1 header:");
            break;
        case FW_BANK_FW2:
            LT_LOG("    Firmware bank 2 header:");
            break;
        case FW_BANK_SPECT1:
            LT_LOG("    SPECT bank 1 header:");
            break;
        case FW_BANK_SPECT2:
            LT_LOG("    SPECT bank 2 header:");
            break;
        default:
            LT_LOG_ERROR("Unknown bank ID: %d", bank_id);
            return;
    }
    LT_LOG("      Type:                           %04X", p_h->type);
    LT_LOG("      Padding:                        %02X", p_h->padding);
    LT_LOG("      FW header version:              %02X", p_h->header_version);
    LT_LOG("      Version:                        %08" PRIX32, p_h->ver);
    LT_LOG("      Size:                           %08" PRIX32, p_h->size);
    LT_LOG("      Git hash:                       %08" PRIX32, p_h->git_hash);
    // Hash str has 32B
    char hash_str[32 * 2 + 1] = {0};
    for (int i = 0; i < 32; i++) {
        snprintf(hash_str + i * 2, sizeof(hash_str) - i * 2, "%02X", p_h->hash[i]);
    }
    LT_LOG("      Hash:                           %s", hash_str);
    LT_LOG("      Pair version:                   %08" PRIX32, p_h->pair_version);
}

static void print_all_fw_headers_v2(lt_handle_t *h)
{
    LT_LOG("  Reading firmware headers in all banks...");
    uint8_t header[LT_L2_GET_INFO_FW_HEADER_SIZE] = {0};

    // Read header from FW_BANK_FW1
    lt_ret_t ret = lt_get_info_fw_bank(h, FW_BANK_FW1, header, sizeof(header));
    if (ret == LT_OK) {
        print_header_v2(header, FW_BANK_FW1);
    }
    else {
        LT_LOG_ERROR("Failed to get FW bank 1 header, ret=%s", lt_ret_verbose(ret));
    }

    // Read header from FW_BANK_FW2
    memset(header, 0, sizeof(header));
    ret = lt_get_info_fw_bank(h, FW_BANK_FW2, header, sizeof(header));
    if (ret == LT_OK) {
        print_header_v2(header, FW_BANK_FW2);
    }
    else {
        LT_LOG_ERROR("Failed to get FW bank 2 header, ret=%s", lt_ret_verbose(ret));
    }

    // Read header from FW_BANK_SPECT1
    memset(header, 0, sizeof(header));
    ret = lt_get_info_fw_bank(h, FW_BANK_SPECT1, header, sizeof(header));
    if (ret == LT_OK) {
        print_header_v2(header, FW_BANK_SPECT1);
    }
    else {
        LT_LOG_ERROR("Failed to get SPECT bank 1 header, ret=%s", lt_ret_verbose(ret));
    }

    // Read header from FW_BANK_SPECT2
    memset(header, 0, sizeof(header));
    ret = lt_get_info_fw_bank(h, FW_BANK_SPECT2, header, sizeof(header));
    if (ret == LT_OK) {
        print_header_v2(header, FW_BANK_SPECT2);
    }
    else {
        LT_LOG_ERROR("Failed to get SPECT bank 2 header, ret=%s", lt_ret_verbose(ret));
    }
}

int lt_ex_show_chip_id_and_fwver(void)
{
    LT_LOG("\t=======================================================================");
    LT_LOG("\t=====  TROPIC01 explore chip                                        ===");
    LT_LOG("\t=======================================================================");

    lt_handle_t h = {0};
#if LT_SEPARATE_L3_BUFF
    uint8_t l3_buffer[L3_PACKET_MAX_SIZE] __attribute__((aligned(16))) = {0};
    h.l3.buff = l3_buffer;
    h.l3.buff_len = sizeof(l3_buffer);
#endif
    // This variable is reused on more places in this example to store different firmware versions
    uint8_t fw_ver[LT_L2_GET_INFO_RISCV_FW_SIZE] = {0};

    lt_ret_t ret = lt_init(&h);
    if (ret != LT_OK) {
        LT_LOG_ERROR("Failed to initialize handle, ret=%s", lt_ret_verbose(ret));
        return -1;
    }

    // First we check versions of both updateable firmwares. For this we need to be in APPLICATION mode.
    // If there are valid firmwares, chip will execute them on boot. In any case we will try to reboot into application,
    // in case chip would be in maintenance mode (executing bootloader)
    LT_LOG("  Rebooting into APPLICATION mode to check firmware versions...");
    ret = lt_reboot(&h, LT_MODE_APP);

    if (h.l2.mode == LT_MODE_APP) {
        // App runs so we can see what firmwares are running
        // Getting RISCV app firmware version
        ret = lt_get_info_riscv_fw_ver(&h, fw_ver, LT_L2_GET_INFO_RISCV_FW_SIZE);
        if (ret == LT_OK) {
            LT_LOG("  Chip is executing RISCV application firmware version: %d.%d.%d    (+ .%d)", fw_ver[3], fw_ver[2],
                   fw_ver[1], fw_ver[0]);
        }
        else {
            LT_LOG_ERROR("Failed to get RISCV firmware version, ret=%s", lt_ret_verbose(ret));
        }

        // Getting SPECT firmware version
        ret = lt_get_info_spect_fw_ver(&h, fw_ver, LT_L2_GET_INFO_SPECT_FW_SIZE);
        if (ret == LT_OK) {
            LT_LOG("  Chip is executing SPECT firmware version:             %d.%d.%d    (+ .%d)", fw_ver[3], fw_ver[2],
                   fw_ver[1], fw_ver[0]);
        }
        else {
            LT_LOG_ERROR("Failed to get SPECT firmware version, ret=%s", lt_ret_verbose(ret));
        }
    }
    else {
        LT_LOG("     ERROR device couldn't get into APP mode, APP and SPECT firmwares in fw banks are not valid");
    }

    LT_LOG_LINE();

    LT_LOG("  Rebooting into MAINTENANCE mode to check bootloader version and fw bank headers...");
    ret = lt_reboot(&h, LT_MODE_MAINTENANCE);
    if (ret != LT_OK) {
        LT_LOG_ERROR("Failed to reboot into MAINTENANCE mode, ret=%s", lt_ret_verbose(ret));
        lt_deinit(&h);
        return -1;
    }

    if (h.l2.mode == LT_MODE_MAINTENANCE) {
        ret = lt_get_info_riscv_fw_ver(&h, fw_ver, LT_L2_GET_INFO_RISCV_FW_SIZE);
        if (ret != LT_OK) {
            LT_LOG_ERROR("Failed to get bootloader version, ret=%s", lt_ret_verbose(ret));
            lt_deinit(&h);
            return -1;
        }

        // Checking if bootloader version is 1.0.1
        if (((fw_ver[3] & 0x7f) == 1) && (fw_ver[2] == 0) && (fw_ver[1] == 1) && (fw_ver[0] == 0)) {
            LT_LOG("  Bootloader version:                 %d.%d.%d    (+ .%d)", fw_ver[3] & 0x7f, fw_ver[2], fw_ver[1],
                   fw_ver[0]);

            print_all_headers_v1(&h);
        }
        else {
            // Checking if bootloader version is 2.0.1
            if (((fw_ver[3] & 0x7f) == 2) && (fw_ver[2] == 0) && (fw_ver[1] == 1) && (fw_ver[0] == 0)) {
                LT_LOG("  Bootloader version:                 %d.%d.%d    (+ .%d)", fw_ver[3] & 0x7f, fw_ver[2],
                       fw_ver[1], fw_ver[0]);
                print_all_fw_headers_v2(&h);
            }
            else {
                LT_LOG_ERROR("Unknown bootloader version %d.%d.%d.%d", fw_ver[3] & 0x7f, fw_ver[2], fw_ver[1],
                             fw_ver[0]);
            }
            LT_LOG_LINE();
        }
    }
    else {
        LT_LOG("     ERROR device couldn't get into MAINTENANCE mode");
        lt_deinit(&h);
        return -1;
    }

    LT_LOG_LINE();

    LT_LOG("Reading Chip ID:");

    struct lt_chip_id_t chip_id = {0};

    lt_get_info_chip_id(&h, &chip_id);
    lt_print_chip_id(&chip_id, printf);

    return 0;
}
