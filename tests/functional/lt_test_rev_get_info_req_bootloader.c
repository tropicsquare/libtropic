/**
 * @file lt_test_rev_get_info_req_bootloader.c
 * @brief Test Get_Info_Req command in Maintenance mode with all possible OBJECT_ID values.
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <inttypes.h>
#include <string.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "lt_test_common.h"

/** @brief Length of the buffers for certificates. */
#define CERTS_BUF_LEN 700
/** @brief Size of the buffer for printing the 32B hash from lt_header_boot_v2_t. */
#define BOOTLOADER_V2_0_1_HASH_PRINT_BUFF_SIZE (32 * 2 + 1)

lt_handle_t *g_h;

static void print_fw_header_bootloader_v1_0_1(uint8_t *header, uint16_t header_size)
{
    LT_TEST_ASSERT(1, (header_size == TR01_L2_GET_INFO_FW_HEADER_SIZE_BOOT_V1));

    struct lt_header_boot_v1_t *p_h = (struct lt_header_boot_v1_t *)header;

    LT_LOG_INFO("Type:     0x%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8, p_h->type[3], p_h->type[2], p_h->type[1],
                p_h->type[0]);
    LT_LOG_INFO("Version:  0x%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8, p_h->version[3], p_h->version[2],
                p_h->version[1], p_h->version[0]);
    LT_LOG_INFO("Size:     0x%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8, p_h->size[3], p_h->size[2], p_h->size[1],
                p_h->size[0]);
    LT_LOG_INFO("Git hash: 0x%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8, p_h->git_hash[3], p_h->git_hash[2],
                p_h->git_hash[1], p_h->git_hash[0]);
    LT_LOG_INFO("FW hash:  0x%02" PRIX8 "%02" PRIX8 "%02" PRIX8 "%02" PRIX8, p_h->hash[3], p_h->hash[2], p_h->hash[1],
                p_h->hash[0]);
}

static void print_fw_header_bootloader_v2_0_1(uint8_t *header, uint16_t header_size)
{
    if (header_size == TR01_L2_GET_INFO_FW_HEADER_SIZE_BOOT_V2_EMPTY_BANK) {
        LT_LOG_INFO("FW bank is empty, nothing to print.");
        return;
    }

    LT_TEST_ASSERT(1, (header_size == TR01_L2_GET_INFO_FW_HEADER_SIZE_BOOT_V2));

    struct lt_header_boot_v2_t *p_h = (struct lt_header_boot_v2_t *)header;
    char hash_str[BOOTLOADER_V2_0_1_HASH_PRINT_BUFF_SIZE];

    LT_LOG_INFO("Calling lt_print_bytes()...");
    LT_TEST_ASSERT(LT_OK,
                   lt_print_bytes(p_h->hash, sizeof(p_h->hash), hash_str, BOOTLOADER_V2_0_1_HASH_PRINT_BUFF_SIZE));

    LT_LOG_INFO("Type:              0x%04" PRIX16, p_h->type);
    LT_LOG_INFO("Padding:           0x%02" PRIX8, p_h->padding);
    LT_LOG_INFO("FW header version: 0x%02" PRIX8, p_h->header_version);
    LT_LOG_INFO("Version:           0x%08" PRIX32, p_h->ver);
    LT_LOG_INFO("Size:              0x%08" PRIX32, p_h->size);
    LT_LOG_INFO("Git hash:          0x%08" PRIX32, p_h->git_hash);
    LT_LOG_INFO("Hash:              \"%s\"", hash_str);
    LT_LOG_INFO("Pair version:      0x%08" PRIX32, p_h->pair_version);
}

static void read_fw_banks_bootloader_v1_0_1(void)
{
    uint8_t fw_header[TR01_L2_GET_INFO_FW_HEADER_SIZE];
    uint16_t read_header_size;

    LT_LOG_INFO("Reading FW bank %d...", (int)TR01_FW_BANK_FW1);
    LT_TEST_ASSERT(LT_OK, lt_get_info_fw_bank(g_h, TR01_FW_BANK_FW1, fw_header, sizeof(fw_header), &read_header_size));
    print_fw_header_bootloader_v1_0_1(fw_header, read_header_size);
    LT_LOG_INFO();

    LT_LOG_INFO("Reading FW bank %d...", (int)TR01_FW_BANK_FW2);
    LT_TEST_ASSERT(LT_OK, lt_get_info_fw_bank(g_h, TR01_FW_BANK_FW2, fw_header, sizeof(fw_header), &read_header_size));
    print_fw_header_bootloader_v1_0_1(fw_header, read_header_size);
    LT_LOG_INFO();

    LT_LOG_INFO("Reading SPECT bank %d...", (int)TR01_FW_BANK_SPECT1);
    LT_TEST_ASSERT(LT_OK,
                   lt_get_info_fw_bank(g_h, TR01_FW_BANK_SPECT1, fw_header, sizeof(fw_header), &read_header_size));
    print_fw_header_bootloader_v1_0_1(fw_header, read_header_size);
    LT_LOG_INFO();

    LT_LOG_INFO("Reading SPECT bank %d...", (int)TR01_FW_BANK_SPECT2);
    LT_TEST_ASSERT(LT_OK,
                   lt_get_info_fw_bank(g_h, TR01_FW_BANK_SPECT2, fw_header, sizeof(fw_header), &read_header_size));
    print_fw_header_bootloader_v1_0_1(fw_header, read_header_size);
    LT_LOG_INFO();
}

static void read_fw_banks_bootloader_v2_0_1(void)
{
    uint8_t fw_header[TR01_L2_GET_INFO_FW_HEADER_SIZE];
    uint16_t read_header_size;

    LT_LOG_INFO("Reading FW bank %d...", (int)TR01_FW_BANK_FW1);
    LT_TEST_ASSERT(LT_OK, lt_get_info_fw_bank(g_h, TR01_FW_BANK_FW1, fw_header, sizeof(fw_header), &read_header_size));
    print_fw_header_bootloader_v2_0_1(fw_header, read_header_size);
    LT_LOG_INFO();

    LT_LOG_INFO("Reading FW bank %d...", (int)TR01_FW_BANK_FW2);
    LT_TEST_ASSERT(LT_OK, lt_get_info_fw_bank(g_h, TR01_FW_BANK_FW2, fw_header, sizeof(fw_header), &read_header_size));
    print_fw_header_bootloader_v2_0_1(fw_header, read_header_size);
    LT_LOG_INFO();

    LT_LOG_INFO("Reading SPECT bank %d...", (int)TR01_FW_BANK_SPECT1);
    LT_TEST_ASSERT(LT_OK,
                   lt_get_info_fw_bank(g_h, TR01_FW_BANK_SPECT1, fw_header, sizeof(fw_header), &read_header_size));
    print_fw_header_bootloader_v2_0_1(fw_header, read_header_size);
    LT_LOG_INFO();

    LT_LOG_INFO("Reading SPECT bank %d...", (int)TR01_FW_BANK_SPECT2);
    LT_TEST_ASSERT(LT_OK,
                   lt_get_info_fw_bank(g_h, TR01_FW_BANK_SPECT2, fw_header, sizeof(fw_header), &read_header_size));
    print_fw_header_bootloader_v2_0_1(fw_header, read_header_size);
    LT_LOG_INFO();
}

static lt_ret_t lt_test_rev_get_info_req_bootloader_cleanup(void)
{
    lt_ret_t ret;

    LT_LOG_INFO("Rebooting to the Application mode...");
    ret = lt_reboot(g_h, TR01_REBOOT);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Couldn't reboot to the Application mode!");
        return ret;
    }

    LT_LOG_INFO("Deinitializing handle");
    ret = lt_deinit(g_h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to deinitialize handle.");
        return ret;
    }

    return LT_OK;
}

void lt_test_rev_get_info_req_bootloader(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_rev_get_info_req_bootloader()");
    LT_LOG_INFO("----------------------------------------------");

    g_h = h;
#ifdef ACAB
    uint8_t cert1[CERTS_BUF_LEN] = {0}, cert2[CERTS_BUF_LEN] = {0}, cert3[CERTS_BUF_LEN] = {0},
            cert4[CERTS_BUF_LEN] = {0};
    struct lt_cert_store_t store = {.certs = {cert1, cert2, cert3, cert4},
                                    .buf_len = {CERTS_BUF_LEN, CERTS_BUF_LEN, CERTS_BUF_LEN, CERTS_BUF_LEN}};
#endif
    uint8_t riscv_ver[TR01_L2_GET_INFO_RISCV_FW_SIZE], spect_ver[TR01_L2_GET_INFO_SPECT_FW_SIZE];
    struct lt_chip_id_t chip_id = {0};

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    LT_LOG_INFO("Rebooting into Maintenance mode...");
    LT_TEST_ASSERT(LT_OK, lt_reboot(h, TR01_MAINTENANCE_REBOOT));

    lt_test_cleanup_function = &lt_test_rev_get_info_req_bootloader_cleanup;

#ifdef ACAB
    LT_LOG_INFO("Reading X509 Certificate Store...");
    LT_TEST_ASSERT(LT_OK, lt_get_info_cert_store(h, &store));
    LT_LOG_INFO();

    uint8_t *cert;
    for (int i = 0; i < LT_NUM_CERTIFICATES; i++) {
        cert = store.certs[i];
        LT_LOG_INFO("Certificate number: %d", i);
        LT_LOG_INFO("Checking if size of certificate is not zero");
        LT_TEST_ASSERT(1, (store.cert_len[i] != 0));
        LT_LOG_INFO("Size in bytes: %" PRIu16, store.cert_len[i]);

        for (int j = 0; j < store.cert_len[i]; j += 16)
            LT_LOG_INFO("%02" PRIx8 "%02" PRIx8 "%02" PRIx8 "%02" PRIx8 "%02" PRIx8 "%02" PRIx8 "%02" PRIx8 "%02" PRIx8
                        "%02" PRIx8 "%02" PRIx8 "%02" PRIx8 "%02" PRIx8 "%02" PRIx8 "%02" PRIx8 "%02" PRIx8 "%02" PRIx8,
                        cert[j], cert[j + 1], cert[j + 2], cert[j + 3], cert[j + 4], cert[j + 5], cert[j + 6],
                        cert[j + 7], cert[j + 8], cert[j + 9], cert[j + 10], cert[j + 11], cert[j + 12], cert[j + 13],
                        cert[j + 14], cert[j + 15]);
        LT_LOG_INFO();
    }
    LT_LOG_LINE();
#elif ABAB
    LT_LOG_INFO("Bootloader v1.0.1 provided just 512B for device certificate only");
#else
#error "Undefined silicon revision. Please define either ABAB or ACAB."
#endif
    LT_LOG_INFO("Reading Chip ID...");
    LT_TEST_ASSERT(LT_OK, lt_get_info_chip_id(h, &chip_id));
    LT_TEST_ASSERT(LT_OK, lt_print_chip_id(&chip_id, chip_id_printf_wrapper));
    LT_LOG_LINE();

    LT_LOG_INFO("Reading RISC-V bootloader version...");
    LT_TEST_ASSERT(LT_OK, lt_get_info_riscv_fw_ver(h, riscv_ver));
    LT_LOG_INFO("RISC-V Bootloader version: v%" PRIu8 ".%" PRIu8 ".%" PRIu8 "(.%" PRIu8 ")", riscv_ver[3] & 0x7f,
                riscv_ver[2], riscv_ver[1], riscv_ver[0]);
    LT_LOG_LINE();

    LT_LOG_INFO("Reading SPECT bootloader version (should be dummy)...");
    LT_TEST_ASSERT(LT_OK, lt_get_info_spect_fw_ver(h, spect_ver));
    LT_LOG_INFO("SPECT Bootloader version: v%" PRIu8 ".%" PRIu8 ".%" PRIu8 ".%" PRIu8 "", spect_ver[3], spect_ver[2],
                spect_ver[1], spect_ver[0]);
    LT_LOG_INFO("Checking SPECT bootloader version dummy value...");
    LT_TEST_ASSERT(0, memcmp(spect_ver, "\x00\x00\x00\x80", sizeof(spect_ver)));
    LT_LOG_LINE();

    // Bootloader version 1.0.1 (ABAB)
    if (((riscv_ver[3] & 0x7f) == 1) && (riscv_ver[2] == 0) && (riscv_ver[1] == 1) && (riscv_ver[0] == 0)) {
        read_fw_banks_bootloader_v1_0_1();
    }
    // Bootloader version 2.0.1 (ACAB)
    else if (((riscv_ver[3] & 0x7f) == 2) && (riscv_ver[2] == 0) && (riscv_ver[1] == 1) && (riscv_ver[0] == 0)) {
        read_fw_banks_bootloader_v2_0_1();
    }
    else {
        LT_LOG_ERROR("Unknown bootloader version!");
        LT_TEST_ASSERT(1, 0);
    }
    LT_LOG_LINE();

    // Call cleanup function, but don't call it from LT_TEST_ASSERT anymore.
    lt_test_cleanup_function = NULL;
    LT_LOG_INFO("Starting post-test cleanup");
    LT_TEST_ASSERT(LT_OK, lt_test_rev_get_info_req_bootloader_cleanup());
    LT_LOG_INFO("Post-test cleanup was successful");
}
