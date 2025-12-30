/**
 * @file lt_test_rev_get_info_req_app.c
 * @brief Test Get_Info_Req command in Application mode with all possible OBJECT_ID values.
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

void lt_test_rev_get_info_req_app(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_rev_get_info_req_app()");
    LT_LOG_INFO("----------------------------------------------");

    uint8_t cert1[CERTS_BUF_LEN] = {0}, cert2[CERTS_BUF_LEN] = {0}, cert3[CERTS_BUF_LEN] = {0},
            cert4[CERTS_BUF_LEN] = {0}, riscv_ver[TR01_L2_GET_INFO_RISCV_FW_SIZE],
            spect_ver[TR01_L2_GET_INFO_SPECT_FW_SIZE];
    struct lt_cert_store_t store = {.certs = {cert1, cert2, cert3, cert4},
                                    .buf_len = {CERTS_BUF_LEN, CERTS_BUF_LEN, CERTS_BUF_LEN, CERTS_BUF_LEN}};
    struct lt_chip_id_t chip_id = {0};

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

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

    LT_LOG_INFO("Reading Chip ID...");
    LT_TEST_ASSERT(LT_OK, lt_get_info_chip_id(h, &chip_id));
    LT_TEST_ASSERT(LT_OK, lt_print_chip_id(&chip_id, chip_id_printf_wrapper));
    LT_LOG_LINE();

    LT_LOG_INFO("Reading RISC-V FW version...");
    LT_TEST_ASSERT(LT_OK, lt_get_info_riscv_fw_ver(h, riscv_ver));
    LT_LOG_INFO("RISC-V FW version: v%" PRIu8 ".%" PRIu8 ".%" PRIu8 ".%" PRIu8 "", riscv_ver[3], riscv_ver[2],
                riscv_ver[1], riscv_ver[0]);
    LT_LOG_LINE();

    LT_LOG_INFO("Reading SPECT FW version...");
    LT_TEST_ASSERT(LT_OK, lt_get_info_spect_fw_ver(h, spect_ver));
    LT_LOG_INFO("SPECT FW version: v%" PRIu8 ".%" PRIu8 ".%" PRIu8 ".%" PRIu8 "", spect_ver[3], spect_ver[2],
                spect_ver[1], spect_ver[0]);
    LT_LOG_INFO("Checking if SPECT FW version is not dummy...");
    LT_TEST_ASSERT(1, (memcmp(spect_ver, "\x00\x00\x00\x80", sizeof(spect_ver)) != 0));
    LT_LOG_LINE();

    LT_LOG_INFO("Deinitializing handle");
    LT_TEST_ASSERT(LT_OK, lt_deinit(h));
}
