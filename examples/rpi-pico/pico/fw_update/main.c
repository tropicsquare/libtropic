/**
 * @file main.c
 * @brief Example showing how to perform an update of the TROPIC01 firmware using Libtropic on
 * Raspberry Pi Pico.
 * @copyright Copyright (c) 2020-2026 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <inttypes.h>
#include <stdio.h>

#include "fw_CPU.h"
#include "fw_SPECT.h"
#include "hardware/spi.h"
#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_mbedtls_v4.h"
#include "libtropic_port_rpi_pico.h"
#include "pico/stdlib.h"
#include "psa/crypto.h"

// Choose pairing keypair for slot 0 (used only if LT_DISABLE_MAINTENANCE_MODE CMake option is set).
#if LT_USE_SH0_ENG_SAMPLE
#define LT_EX_SH0_PRIV lt_sh0priv_eng_sample
#define LT_EX_SH0_PUB lt_sh0pub_eng_sample
#elif LT_USE_SH0_PROD0
#define LT_EX_SH0_PRIV lt_sh0priv_prod0
#define LT_EX_SH0_PUB lt_sh0pub_prod0
#endif

#if LT_DISABLE_MAINTENANCE_MODE
// Tracks whether the Maintenance Mode had to be enabled before starting the FW update.
bool g_maintenance_mode_was_enabled = false;

lt_ret_t check_and_enable_maintenance_mode(lt_handle_t *lt_handle)
{
    lt_ret_t ret;

    printf("\nChecking if Maintenance Mode is enabled and enabling it in R-Config if needed:\n");

    // Establish Secure Channel Session so we can read I-Config/R-Config.
    printf("  - Starting Secure Session with key slot %d...", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    // Keys are chosen based on the CMake option LT_SH0_KEYS.
    ret = lt_verify_chip_and_start_secure_session(lt_handle, LT_EX_SH0_PRIV, LT_EX_SH0_PUB,
                                                  TR01_PAIRING_KEY_SLOT_INDEX_0);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to start Secure Session with key %d, ret=%s\n",
                (int)TR01_PAIRING_KEY_SLOT_INDEX_0, lt_ret_verbose(ret));
        fprintf(stderr,
                "Check if you use correct SH0 keys! Hint: if you use an engineering sample chip, "
                "compile with "
                "-DLT_SH0_KEYS=eng_sample\n");
        return ret;
    }
    printf("OK\n");

    // Read I-Config and check if Maintenance Mode is enabled.
    uint32_t i_config_cfg_startup;
    printf("  - Reading I-Config[CFG_START_UP]...");
    ret = lt_i_config_read(lt_handle, TR01_CFG_START_UP_ADDR, &i_config_cfg_startup);
    if (ret != LT_OK) {
        fprintf(stderr, "\nFailed to read I-Config[CFG_START_UP], ret=%s\n", lt_ret_verbose(ret));
        return ret;
    }
    printf("OK\n");

    printf("  - Checking if Maintenance Mode is enabled in I-Config[CFG_START_UP]...");
    if (!(i_config_cfg_startup & BOOTLOADER_CO_CFG_START_UP_MAINTENANCE_ENA_MASK)) {
        fprintf(stderr,
                "\nMaintenance Mode is not enabled in I-Config -> FW Update cannot be performed.\n");
        return LT_FAIL;
    }
    printf("OK\n");

    // Read R-Config and check if Maintenance Mode is enabled.
    // The whole R-Config is read in case we need to modify it, as it has to be completely erased
    // before writing again.
    lt_config_t r_config;
    printf("  - Reading R-Config...");
    ret = lt_read_whole_R_config(lt_handle, &r_config);
    if (ret != LT_OK) {
        fprintf(stderr, "\nFailed to read R-Config, ret=%s\n", lt_ret_verbose(ret));
        return ret;
    }
    printf("OK\n");

    printf("  - Read R-Config (acts as a backup):\n");
    for (int i = 0; i < LT_CONFIG_OBJ_CNT; i++) {
        printf("    - %s= 0x%08" PRIx32 "\n", cfg_desc_table[i].desc, r_config.obj[i]);
    }

    printf("  - Checking if Maintenance Mode is enabled in R-Config[CFG_START_UP]...");
    if (!(r_config.obj[TR01_CFG_START_UP_IDX] & BOOTLOADER_CO_CFG_START_UP_MAINTENANCE_ENA_MASK)) {
        printf("Disabled, will enable it\n");
        printf("    - Erasing R-Config...");
        ret = lt_r_config_erase(lt_handle);
        if (ret != LT_OK) {
            fprintf(stderr, "\nFailed to erase R-Config, ret=%s\n", lt_ret_verbose(ret));
            return ret;
        }
        printf("OK\n");

        r_config.obj[TR01_CFG_START_UP_IDX] |= BOOTLOADER_CO_CFG_START_UP_MAINTENANCE_ENA_MASK;
        printf("    - Writing modified R-Config...");
        ret = lt_write_whole_R_config(lt_handle, &r_config);
        if (ret != LT_OK) {
            fprintf(stderr, "\nFailed to write R-Config, ret=%s\n", lt_ret_verbose(ret));
            fprintf(stderr,
                    "WARNING: R-Config is left in an erased state - the state before erasing is "
                    "printed above.\n");
            return ret;
        }
        printf("OK\n");
        g_maintenance_mode_was_enabled = true;

        printf("    - Rebooting TROPIC01 to apply R-Config changes...");
        ret = lt_reboot(lt_handle, TR01_REBOOT);
        if (ret != LT_OK) {
            fprintf(stderr, "\nlt_reboot() failed, ret=%s\n", lt_ret_verbose(ret));
            fprintf(stderr,
                    "WARNING: R-Config (with Maintenance Mode enabled) was written but not applied "
                    "yet. It will be applied on next TROPIC01 reboot/power-cycle.\n");
            return ret;
        }
        printf("OK\n");
    }
    else {
        printf("OK\n");
    }

    return LT_OK;
}

lt_ret_t disable_maintenance_mode(lt_handle_t *lt_handle)
{
    lt_ret_t ret;

    printf("\nDisabling Maintenance Mode in R-Config (reduces the attack surface):\n");

    printf("  - Starting Secure Session with key slot %d...", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    // Keys are chosen based on the CMake option LT_SH0_KEYS.
    ret = lt_verify_chip_and_start_secure_session(lt_handle, LT_EX_SH0_PRIV, LT_EX_SH0_PUB,
                                                  TR01_PAIRING_KEY_SLOT_INDEX_0);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to start Secure Session with key %d, ret=%s\n",
                (int)TR01_PAIRING_KEY_SLOT_INDEX_0, lt_ret_verbose(ret));
        return ret;
    }
    printf("OK\n");

    lt_config_t r_config;
    printf("  - Reading R-Config...");
    ret = lt_read_whole_R_config(lt_handle, &r_config);
    if (ret != LT_OK) {
        fprintf(stderr, "\nFailed to read R-Config, ret=%s\n", lt_ret_verbose(ret));
        return ret;
    }
    printf("OK\n");

    printf("  - Erasing R-Config...");
    ret = lt_r_config_erase(lt_handle);
    if (ret != LT_OK) {
        fprintf(stderr, "\nFailed to erase R-Config, ret=%s\n", lt_ret_verbose(ret));
        return ret;
    }
    printf("OK\n");

    printf("  - Disabling Maintenance Mode in R-Config...");
    r_config.obj[TR01_CFG_START_UP_IDX] &= ~BOOTLOADER_CO_CFG_START_UP_MAINTENANCE_ENA_MASK;
    ret = lt_write_whole_R_config(lt_handle, &r_config);
    if (ret != LT_OK) {
        fprintf(stderr, "\nFailed to write R-Config, ret=%s\n", lt_ret_verbose(ret));
        fprintf(stderr,
                "WARNING: R-Config is left in an erased state - the state before erasing is printed "
                "above.\n");
        return ret;
    }
    printf("OK\n");

    printf("  - Rebooting TROPIC01 to apply R-Config changes...");
    ret = lt_reboot(lt_handle, TR01_REBOOT);
    if (ret != LT_OK) {
        fprintf(stderr, "\nlt_reboot() failed, ret=%s\n", lt_ret_verbose(ret));
        fprintf(stderr,
                "WARNING: R-Config (with Maintenance Mode disabled) was written but not applied yet. "
                "It will be applied on next TROPIC01 reboot/power-cycle.\n");
        return ret;
    }
    printf("OK\n");

    printf("  - Verifying that Maintenance Mode is not accessible...");
    ret = lt_reboot(lt_handle, TR01_MAINTENANCE_REBOOT);
    if (ret == LT_L2_RESP_DISABLED) {
        printf("OK\n");
    }
    else if (ret != LT_OK) {
        fprintf(stderr, "\nlt_reboot() failed, ret=%s\n", lt_ret_verbose(ret));
        return ret;
    }
    else {
        fprintf(stderr, "\nMaintenance reboot succeeded! ret=%s\n", lt_ret_verbose(ret));
        return ret;
    }

    return LT_OK;
}
#endif

lt_ret_t get_fw_versions(lt_handle_t *lt_handle)
{
    uint8_t cpu_fw_ver[TR01_L2_GET_INFO_RISCV_FW_SIZE] = {0};
    uint8_t spect_fw_ver[TR01_L2_GET_INFO_SPECT_FW_SIZE] = {0};

    printf("Reading firmware versions from TROPIC01...");
    lt_ret_t ret = lt_get_info_riscv_fw_ver(lt_handle, cpu_fw_ver);
    if (ret != LT_OK) {
        fprintf(stderr, "\nFailed to get RISC-V FW version, ret=%s\n", lt_ret_verbose(ret));
        return ret;
    }
    ret = lt_get_info_spect_fw_ver(lt_handle, spect_fw_ver);
    if (ret != LT_OK) {
        fprintf(stderr, "\nFailed to get SPECT FW version, ret=%s\n", lt_ret_verbose(ret));
        return ret;
    }
    printf("OK\n");

    printf("TROPIC01 firmware versions:\n");
    printf("  - RISC-V FW version: %d.%d.%d\n", cpu_fw_ver[3], cpu_fw_ver[2], cpu_fw_ver[1]);
    printf("  - SPECT FW version: %d.%d.%d\n", spect_fw_ver[3], spect_fw_ver[2], spect_fw_ver[1]);

    return LT_OK;
}

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

    printf("==========================================\n");
    printf("==== TROPIC01 Firmware Update Example ====\n");
    printf("==========================================\n");

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
    printf("Rebooting TROPIC01...");
    ret = lt_reboot(&lt_handle, TR01_REBOOT);
    if (ret != LT_OK) {
        fprintf(stderr, "\nlt_reboot() failed, ret=%s\n", lt_ret_verbose(ret));
        cleanup(&lt_handle);
    }
    printf("OK\n");

    if (get_fw_versions(&lt_handle) != LT_OK) {
        cleanup(&lt_handle);
    }

    printf("Versions to update to:\n");
    printf("  - RISC-V FW version: %d.%d.%d\n", fw_CPU_ver[3], fw_CPU_ver[2], fw_CPU_ver[1]);
    printf("  - SPECT FW version: %d.%d.%d\n", fw_SPECT_ver[3], fw_SPECT_ver[2], fw_SPECT_ver[1]);

#if LT_DISABLE_MAINTENANCE_MODE
    // We need to make sure we can reboot into Maintenance Mode to perform the FW update.
    if (LT_OK != check_and_enable_maintenance_mode(&lt_handle)) {
        cleanup(&lt_handle);
    }
#endif

    printf("\nUpdating TROPIC01 firmware...");
    // This helper function implements the recommended FW update process.
    ret = lt_do_mutable_fw_update(&lt_handle, fw_CPU, sizeof(fw_CPU), fw_SPECT, sizeof(fw_SPECT));
    if (ret != LT_OK) {
        fprintf(stderr, "\nlt_do_mutable_fw_update() failed, ret=%s\n", lt_ret_verbose(ret));
        fprintf(stderr,
                "Tip: turn logging on to see more information (compile with -DLT_LOG_LVL=Info)\n");
#if LT_DISABLE_MAINTENANCE_MODE
        if (g_maintenance_mode_was_enabled) {
            fprintf(stderr,
                    "WARNING: Due to the error, Maintenance Mode was kept enabled in R-Config!\n");
        }
#endif
        cleanup(&lt_handle);
    }
    printf("OK\n");

    if (get_fw_versions(&lt_handle) != LT_OK) {
        cleanup(&lt_handle);
    }

#if LT_DISABLE_MAINTENANCE_MODE
    // Warning: User shall disable Maintenance Mode only after both FW banks are updated with the
    // latest FW. If only one bank is updated (while the second one contains invalid FW), and
    // Maintenance Mode is disabled, the probability of bricking TROPIC01 increases. In this case, if
    // lt_do_mutable_fw_update() succeeds, it is safe to disable Maintenance Mode.
    if (LT_OK != disable_maintenance_mode(&lt_handle)) {
        cleanup(&lt_handle);
    }
#else
    printf(
        "\nWARNING: We strongly recommend disabling Maintenance Mode in R-Config to reduce the attack "
        "surface.\n");
#endif

    printf("\nDeinitializing handle...");
    ret = lt_deinit(&lt_handle);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to deinitialize handle, ret=%s\n", lt_ret_verbose(ret));
        cleanup(NULL);  // We pass NULL to not deinitialize Libtropic handle.
    }
    printf("OK\n");

    cleanup(NULL);  // We pass NULL to not deinitialize Libtropic handle.

    return 0;  // This should not be reached.
}
