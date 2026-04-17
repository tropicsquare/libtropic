/**
 * @file main.c
 * @brief Example showing how to perform an update of the TROPIC01 firmware using Libtropic and
 * ESP32-DevKitC-V4.
 * @copyright Copyright (c) 2020-2026 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "fw_CPU.h"
#include "fw_SPECT.h"
#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_mbedtls_v4.h"
#include "libtropic_port_esp_idf.h"
#include "psa/crypto.h"

#define TAG "fw_update"

lt_ret_t get_fw_versions(lt_handle_t *lt_handle)
{
    uint8_t cpu_fw_ver[TR01_L2_GET_INFO_RISCV_FW_SIZE] = {0};
    uint8_t spect_fw_ver[TR01_L2_GET_INFO_SPECT_FW_SIZE] = {0};

    ESP_LOGI(TAG, "Reading firmware versions from TROPIC01...");
    lt_ret_t ret = lt_get_info_riscv_fw_ver(lt_handle, cpu_fw_ver);
    if (ret != LT_OK) {
        ESP_LOGE(TAG, "Failed to get RISC-V FW version, ret=%s", lt_ret_verbose(ret));
        return ret;
    }
    ret = lt_get_info_spect_fw_ver(lt_handle, spect_fw_ver);
    if (ret != LT_OK) {
        ESP_LOGE(TAG, "Failed to get SPECT FW version, ret=%s", lt_ret_verbose(ret));
        return ret;
    }
    ESP_LOGI(TAG, "OK");

    ESP_LOGI(TAG, "TROPIC01 firmware versions:");
    ESP_LOGI(TAG, "  - RISC-V FW version: %d.%d.%d", cpu_fw_ver[3], cpu_fw_ver[2], cpu_fw_ver[1]);
    ESP_LOGI(TAG, "  - SPECT FW version: %d.%d.%d", spect_fw_ver[3], spect_fw_ver[2], spect_fw_ver[1]);

    return LT_OK;
}

void app_main(void)
{
    ESP_LOGI(TAG, "==========================================");
    ESP_LOGI(TAG, "==== TROPIC01 Firmware Update Example ====");
    ESP_LOGI(TAG, "==========================================");

    // Cryptographic function provider initialization.
    //
    // In production, this would typically be done only once,
    // usually at the start of the application or before
    // the first use of cryptographic functions but no later than
    // the first occurrence of any Libtropic function
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        ESP_LOGE(TAG, "PSA Crypto initialization failed, status=%d (psa_status_t)", status);
        return;
    }

    // Libtropic handle.
    //
    // It is declared here (on stack) for
    // simplicity. In production, you put it on heap if needed.
    lt_handle_t lt_handle = {0};

    // Device structure.
    //
    // Used default pins for a typical ESP32 DevKit; adjust if you use different wiring.
    lt_dev_esp_idf_t device = {0};
    device.spi_host_id = VSPI_HOST;
    device.spi_cs_gpio_pin = GPIO_NUM_5;
    device.spi_miso_pin = GPIO_NUM_19;
    device.spi_mosi_pin = GPIO_NUM_23;
    device.spi_clk_pin = GPIO_NUM_18;
    device.spi_clk_hz = 5000000; /* 5 MHz */
#if LT_USE_INT_PIN
    device.int_gpio_pin = GPIO_NUM_32;
#endif
    lt_handle.l2.device = &device;

#if LT_USE_INT_PIN
    // This function has to be called only once.
    // The call is needed because the Libtropic ESP-IDF HAL uses GPIO interrupts.
    esp_err_t esp_ret = gpio_install_isr_service(0);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "gpio_install_isr_service() failed: %s", esp_err_to_name(esp_ret));
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return;
    }
#endif

    // Crypto abstraction layer (CAL) context.
    lt_ctx_mbedtls_v4_t crypto_ctx;
    lt_handle.l3.crypto_ctx = &crypto_ctx;

    ESP_LOGI(TAG, "Initializing handle...");
    lt_ret_t ret = lt_init(&lt_handle);
    if (LT_OK != ret) {
        ESP_LOGE(TAG, "Failed to initialize handle, ret=%s", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return;
    }
    ESP_LOGI(TAG, "OK");

    // First, we check versions of both updateable firmwares. To do that, we need TROPIC01 to **not**
    // be in the Start-up Mode. If there are valid firmwares, TROPIC01 will begin to execute them
    // automatically on boot.
    ESP_LOGI(TAG, "Rebooting TROPIC01...");
    ret = lt_reboot(&lt_handle, TR01_REBOOT);
    if (ret != LT_OK) {
        ESP_LOGE(TAG, "Reboot failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return;
    }
    ESP_LOGI(TAG, "OK");

    if (get_fw_versions(&lt_handle) != LT_OK) {
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return;
    }
    ESP_LOGI(TAG, "OK");
    ESP_LOGI(TAG, "");

    ESP_LOGI(TAG, "Starting firmware update...");

    // The chip must be in Start-up Mode to be able to perform a firmware update.
    ESP_LOGI(TAG, "- Sending maintenance reboot request...");
    ret = lt_reboot(&lt_handle, TR01_MAINTENANCE_REBOOT);
    if (ret != LT_OK) {
        ESP_LOGE(TAG, "Maintenance reboot failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return;
    }
    ESP_LOGI(TAG, "OK");

    ESP_LOGI(TAG, "- Updating TR01_FW_BANK_FW1 and TR01_FW_BANK_SPECT1");
    ESP_LOGI(TAG, "  - Updating RISC-V FW...");
    ret = lt_do_mutable_fw_update(&lt_handle, fw_CPU, sizeof(fw_CPU), TR01_FW_BANK_FW1);
    if (ret != LT_OK) {
        ESP_LOGE(TAG, "RISC-V FW update failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return;
    }
    ESP_LOGI(TAG, "OK");

    ESP_LOGI(TAG, "  - Updating SPECT FW...");
    ret = lt_do_mutable_fw_update(&lt_handle, fw_SPECT, sizeof(fw_SPECT), TR01_FW_BANK_SPECT1);
    if (ret != LT_OK) {
        ESP_LOGE(TAG, "SPECT FW update failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return;
    }
    ESP_LOGI(TAG, "OK");

    // Reboot into Maintenance Mode. This step is crucial if we want to update both FW bank pairs.
    // If the reboot is not done, then in the case of ACAB silicon revision, the second FW bank pair
    // will not be updated, which increases the possibility of a downgrade attack.
    ESP_LOGI(TAG, "- Rebooting TROPIC01 into Maintenance Mode...");
    ret = lt_reboot(&lt_handle, TR01_MAINTENANCE_REBOOT);
    if (ret != LT_OK) {
        ESP_LOGE(TAG, "lt_reboot() failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return;
    }
    ESP_LOGI(TAG, "OK");

    ESP_LOGI(TAG, "- Updating TR01_FW_BANK_FW2 and TR01_FW_BANK_SPECT2");
    ESP_LOGI(TAG, "  - Updating RISC-V FW...");
    ret = lt_do_mutable_fw_update(&lt_handle, fw_CPU, sizeof(fw_CPU), TR01_FW_BANK_FW2);
    if (ret != LT_OK) {
        ESP_LOGE(TAG, "RISC-V FW update failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return;
    }
    ESP_LOGI(TAG, "OK");

    ESP_LOGI(TAG, "  - Updating SPECT FW...");
    ret = lt_do_mutable_fw_update(&lt_handle, fw_SPECT, sizeof(fw_SPECT), TR01_FW_BANK_SPECT2);
    if (ret != LT_OK) {
        ESP_LOGE(TAG, "SPECT FW update failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return;
    }
    ESP_LOGI(TAG, "OK");
    ESP_LOGI(TAG, "Successfully updated all 4 FW banks.");
    ESP_LOGI(TAG, "");

    ESP_LOGI(TAG, "Sending reboot request...");
    ret = lt_reboot(&lt_handle, TR01_REBOOT);
    if (ret != LT_OK) {
        ESP_LOGE(TAG, "lt_reboot() failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return;
    }
    ESP_LOGI(TAG, "OK");
    ESP_LOGI(TAG, "TROPIC01 is executing Application FW now");
    ESP_LOGI(TAG, "");

    if (get_fw_versions(&lt_handle) != LT_OK) {
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return;
    }

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "Deinitializing handle...");
    ret = lt_deinit(&lt_handle);
    if (LT_OK != ret) {
        ESP_LOGE(TAG, "Failed to deinitialize handle, ret=%s", lt_ret_verbose(ret));
        mbedtls_psa_crypto_free();
        return;
    }
    ESP_LOGI(TAG, "OK");

    // Cryptographic function provider deinitialization.
    //
    // In production, this would be done only once, typically
    // during termination of the application.
    mbedtls_psa_crypto_free();
}
