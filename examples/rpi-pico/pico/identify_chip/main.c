/**
 * @file main.c
 * @brief Example of reading information about the TROPIC01 chip and its firmware using Libtropic on
 * Raspberry Pi Pico.
 * @copyright Copyright (c) 2020-2026 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <inttypes.h>
#include <stdio.h>

#include "hardware/spi.h"
#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_mbedtls_v4.h"
#include "libtropic_port_rpi_pico.h"
#include "pico/stdlib.h"

int main(void)
{
    stdio_init_all();
    // Loop until the USB CDC serial connection is actively opened by your PC.
    while (!stdio_usb_connected()) {
        sleep_ms(10);
    }

    printf("==============================================\n");
    printf("==== TROPIC01 Chip Identification Example ====\n");
    printf("==============================================\n");

    lt_handle_t lt_handle = {0};

    // Defaults correspond to SPI0 pins on Pico boards.
    lt_dev_rpi_pico_t device = {.spi_instance = spi0,
                                .spi_baudrate = 5000000,
                                .cs_pin = 13,
                                .pin_miso = 12,
                                .pin_mosi = 15,
                                .pin_sck = 14};
    lt_handle.l2.device = &device;

    lt_ctx_mbedtls_v4_t crypto_ctx = {0};
    lt_handle.l3.crypto_ctx = &crypto_ctx;

    printf("Initializing handle...");
    lt_ret_t ret = lt_init(&lt_handle);
    if (ret != LT_OK) {
        printf("\nFailed to initialize handle, ret=%s\n", lt_ret_verbose(ret));
        return -1;
    }
    printf("OK\n");

    printf("Sending reboot request...");
    ret = lt_reboot(&lt_handle, TR01_REBOOT);
    if (ret != LT_OK) {
        printf("\nlt_reboot() failed, ret=%s\n", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        return -1;
    }
    printf("OK\n");

    printf("Reading data from chip...\n");

    uint8_t fw_ver[4] = {0};
    ret = lt_get_info_riscv_fw_ver(&lt_handle, fw_ver);
    if (ret != LT_OK) {
        printf("Failed to get RISC-V FW version, ret=%s\n", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        return -1;
    }
    printf("  RISC-V FW version: %" PRIX8 ".%" PRIX8 ".%" PRIX8 " (.%" PRIX8 ")\n", fw_ver[3],
           fw_ver[2], fw_ver[1], fw_ver[0]);

    ret = lt_get_info_spect_fw_ver(&lt_handle, fw_ver);
    if (ret != LT_OK) {
        printf("Failed to get SPECT FW version, ret=%s\n", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        return -1;
    }
    printf("  SPECT FW version: %" PRIX8 ".%" PRIX8 ".%" PRIX8 " (.%" PRIX8 ")\n", fw_ver[3],
           fw_ver[2], fw_ver[1], fw_ver[0]);

    printf("Sending maintenance reboot request...");
    ret = lt_reboot(&lt_handle, TR01_MAINTENANCE_REBOOT);
    if (ret != LT_OK) {
        printf("\nlt_reboot() failed, ret=%s\n", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        return -1;
    }
    printf("OK\n");

    printf("Reading data from chip...\n");

    ret = lt_get_info_riscv_fw_ver(&lt_handle, fw_ver);
    if (ret != LT_OK) {
        printf("Failed to get RISC-V bootloader version, ret=%s\n", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        return -1;
    }
    printf("  RISC-V bootloader version: %" PRIX8 ".%" PRIX8 ".%" PRIX8 " (.%" PRIX8 ")\n",
           fw_ver[3] & 0x7f, fw_ver[2], fw_ver[1], fw_ver[0]);

    printf("Firmware bank headers:\n");
    ret = lt_print_fw_header(&lt_handle, TR01_FW_BANK_FW1, printf);
    if (ret != LT_OK) {
        printf("Failed to print TR01_FW_BANK_FW1 header, ret=%s\n", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        return -1;
    }
    ret = lt_print_fw_header(&lt_handle, TR01_FW_BANK_FW2, printf);
    if (ret != LT_OK) {
        printf("Failed to print TR01_FW_BANK_FW2 header, ret=%s\n", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        return -1;
    }
    ret = lt_print_fw_header(&lt_handle, TR01_FW_BANK_SPECT1, printf);
    if (ret != LT_OK) {
        printf("Failed to print TR01_FW_BANK_SPECT1 header, ret=%s\n", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        return -1;
    }
    ret = lt_print_fw_header(&lt_handle, TR01_FW_BANK_SPECT2, printf);
    if (ret != LT_OK) {
        printf("Failed to print TR01_FW_BANK_SPECT2 header, ret=%s\n", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        return -1;
    }

    struct lt_chip_id_t chip_id = {0};

    printf("Chip ID data:\n");
    ret = lt_get_info_chip_id(&lt_handle, &chip_id);
    if (ret != LT_OK) {
        printf("Failed to get chip ID, ret=%s\n", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        return -1;
    }

    printf("---------------------------------------------------------\n");
    ret = lt_print_chip_id(&chip_id, printf);
    if (ret != LT_OK) {
        printf("Failed to print chip ID, ret=%s\n", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        return -1;
    }
    printf("---------------------------------------------------------\n");

    printf("Sending reboot request...");
    ret = lt_reboot(&lt_handle, TR01_REBOOT);
    if (ret != LT_OK) {
        printf("\nlt_reboot() failed, ret=%s\n", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        return -1;
    }
    printf("OK!\n");

    printf("Deinitializing handle...");
    ret = lt_deinit(&lt_handle);
    if (ret != LT_OK) {
        printf("\nFailed to deinitialize handle, ret=%s\n", lt_ret_verbose(ret));
        return -1;
    }
    printf("OK\n");

    return 0;
}
