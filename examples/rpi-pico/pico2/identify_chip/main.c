/**
 * @file main.c
 * @brief Example of reading information about the TROPIC01 chip and its firmware using Libtropic on
 * Raspberry Pi Pico 2.
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
#include "psa/crypto.h"

// Cleanup function (pass NULL as Libtropic handle to not deinitialize it).
static void cleanup(lt_handle_t *lt_handle)
{
    // Deinitialize Libtropic handle.
    // Must be called only if lt_init() was successful!
    if (lt_handle) {
        lt_deinit(lt_handle);
    }

    // Cryptographic function provider deinitialization.
    //
    // In production, this would be done only once, typically
    // during termination of the application.
    mbedtls_psa_crypto_free();

    // Stay alive so the board is available via USB.
    while (1) {
        tight_loop_contents();
    }
}

int main(void)
{
    stdio_init_all();

    // Loop until the USB CDC serial connection is actively opened by your PC.
    // This prevents loosing output from this example after flashing and before opening the serial.
    while (!stdio_usb_connected()) {
        sleep_ms(10);
    }

    printf("==============================================\n");
    printf("==== TROPIC01 Chip Identification Example ====\n");
    printf("==============================================\n");

    // Cryptographic function provider initialization.
    //
    // In production, this would typically be done only once,
    // usually at the start of the application or before
    // the first use of cryptographic functions but no later than
    // the first occurrence of any Libtropic function.
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        fprintf(stderr, "PSA Crypto initialization failed, status=%d (psa_status_t)\n", status);
        cleanup(NULL);  // We pass NULL to indicate Libtropic handle was not initialized yet.
    }

    // Libtropic handle.
    //
    // It is declared here (on stack) for
    // simplicity. In production, you put it on heap if needed.
    lt_handle_t lt_handle = {0};

    // Device structure.
    //
    // Modify this according to your environment.
    lt_dev_rpi_pico_t device = {0};
    device.spi_instance = spi1;
    device.spi_baudrate = 6250000;
    device.cs_pin = 13;
    device.pin_miso = 12;
    device.pin_mosi = 15;
#ifdef LT_USE_INT_PIN
    device.int_gpio_pin = 22;
#endif
    device.pin_sck = 14;
    lt_handle.l2.device = &device;

    // Crypto abstraction layer (CAL) context.
    lt_ctx_mbedtls_v4_t crypto_ctx = {0};
    lt_handle.l3.crypto_ctx = &crypto_ctx;

    printf("Initializing handle...");
    lt_ret_t ret = lt_init(&lt_handle);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to initialize handle, ret=%s\n", lt_ret_verbose(ret));
        cleanup(NULL);  // We pass NULL to indicate Libtropic handle was not initialized.
    }
    printf("OK\n");

    // First, we check versions of both updateable firmwares. To do that, we need TROPIC01 to **not**
    // be in the Start-up Mode. If there are valid firmwares, TROPIC01 will begin to execute them
    // automatically on boot.
    printf("Sending reboot request...");
    ret = lt_reboot(&lt_handle, TR01_REBOOT);
    if (ret != LT_OK) {
        fprintf(stderr, "\nlt_reboot() failed, ret=%s\n", lt_ret_verbose(ret));
        cleanup(&lt_handle);
    }
    printf("OK\n");

    printf("Reading data from chip...\n");

    uint8_t fw_ver[4];
    ret = lt_get_info_riscv_fw_ver(&lt_handle, fw_ver);
    if (ret != LT_OK) {
        fprintf(stderr, "Failed to get RISC-V FW version, ret=%s\n", lt_ret_verbose(ret));
        cleanup(&lt_handle);
    }
    printf("  RISC-V FW version: %" PRIX8 ".%" PRIX8 ".%" PRIX8 " (.%" PRIX8 ")\n", fw_ver[3],
           fw_ver[2], fw_ver[1], fw_ver[0]);

    ret = lt_get_info_spect_fw_ver(&lt_handle, fw_ver);
    if (ret != LT_OK) {
        fprintf(stderr, "Failed to get SPECT FW version, ret=%s\n", lt_ret_verbose(ret));
        cleanup(&lt_handle);
    }
    printf("  SPECT FW version: %" PRIX8 ".%" PRIX8 ".%" PRIX8 " (.%" PRIX8 ")\n", fw_ver[3],
           fw_ver[2], fw_ver[1], fw_ver[0]);

    // We need to do the maintenance reboot to check bootloader version and FW bank headers in the
    // Startup Mode.
    printf("Sending maintenance reboot request...");
    ret = lt_reboot(&lt_handle, TR01_MAINTENANCE_REBOOT);
    if (ret != LT_OK) {
        fprintf(stderr, "\nlt_reboot() failed, ret=%s\n", lt_ret_verbose(ret));
        cleanup(&lt_handle);
    }
    printf("OK\n");

    printf("Reading data from chip...\n");

    // When TROPIC01 is in Start-up Mode, we can get RISC-V bootloader version the same way as we got
    // RISC-V FW version.
    ret = lt_get_info_riscv_fw_ver(&lt_handle, fw_ver);
    if (ret != LT_OK) {
        fprintf(stderr, "Failed to get RISC-V bootloader version, ret=%s\n", lt_ret_verbose(ret));
        cleanup(&lt_handle);
    }
    printf("  RISC-V bootloader version: %" PRIX8 ".%" PRIX8 ".%" PRIX8 " (.%" PRIX8 ")\n",
           fw_ver[3] & 0x7f, fw_ver[2], fw_ver[1], fw_ver[0]);

    printf("Firmware bank headers:\n");
    ret = lt_print_fw_header(&lt_handle, TR01_FW_BANK_FW1, printf);
    if (ret != LT_OK) {
        fprintf(stderr, "Failed to print TR01_FW_BANK_FW1 header, ret=%s\n", lt_ret_verbose(ret));
        cleanup(&lt_handle);
    }
    ret = lt_print_fw_header(&lt_handle, TR01_FW_BANK_FW2, printf);
    if (ret != LT_OK) {
        fprintf(stderr, "Failed to print TR01_FW_BANK_FW2 header, ret=%s\n", lt_ret_verbose(ret));
        cleanup(&lt_handle);
    }
    ret = lt_print_fw_header(&lt_handle, TR01_FW_BANK_SPECT1, printf);
    if (ret != LT_OK) {
        fprintf(stderr, "Failed to print TR01_FW_BANK_SPECT1 header, ret=%s\n", lt_ret_verbose(ret));
        cleanup(&lt_handle);
    }
    ret = lt_print_fw_header(&lt_handle, TR01_FW_BANK_SPECT2, printf);
    if (ret != LT_OK) {
        fprintf(stderr, "Failed to print TR01_FW_BANK_SPECT2 header, ret=%s\n", lt_ret_verbose(ret));
        cleanup(&lt_handle);
    }

    struct lt_chip_id_t chip_id = {0};

    printf("Chip ID data:\n");
    ret = lt_get_info_chip_id(&lt_handle, &chip_id);
    if (ret != LT_OK) {
        fprintf(stderr, "Failed to get chip ID, ret=%s\n", lt_ret_verbose(ret));
        cleanup(&lt_handle);
    }

    printf("---------------------------------------------------------\n");
    ret = lt_print_chip_id(&chip_id, printf);
    if (ret != LT_OK) {
        fprintf(stderr, "Failed to print chip ID, ret=%s\n", lt_ret_verbose(ret));
        cleanup(&lt_handle);
    }
    printf("---------------------------------------------------------\n");

    printf("Sending reboot request...");
    ret = lt_reboot(&lt_handle, TR01_REBOOT);
    if (ret != LT_OK) {
        fprintf(stderr, "\nlt_reboot() failed, ret=%s\n", lt_ret_verbose(ret));
        cleanup(&lt_handle);
    }
    printf("OK!\n");

    printf("Deinitializing handle...");
    ret = lt_deinit(&lt_handle);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to deinitialize handle, ret=%s\n", lt_ret_verbose(ret));
        cleanup(NULL);  // We pass NULL to not deinitialize Libtropic handle.
    }
    printf("OK\n");

    cleanup(NULL);  // We pass NULL to not deinitialize Libtropic handle.

    return 0;  // This should not be reached.
}
