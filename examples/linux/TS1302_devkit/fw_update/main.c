/**
 * @file main.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <string.h>
#include <stdio.h>
#include <time.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_examples.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "libtropic_port_posix_usb_dongle.h"
#include "fw_CPU.h"
#include "fw_SPECT.h"

#include "libtropic_mbedtls_v4.h"
#include "psa/crypto.h"

int get_fw_versions(lt_handle_t *lt_handle) {
    
    uint8_t cpu_fw_ver[TR01_L2_GET_INFO_RISCV_FW_SIZE] = {0};
    uint8_t spect_fw_ver[TR01_L2_GET_INFO_SPECT_FW_SIZE] = {0};

    printf("Reading firmware versions from TROPIC01...");
    lt_ret_t ret = lt_get_info_riscv_fw_ver(lt_handle, cpu_fw_ver);
    if (ret != LT_OK) {
        fprintf(stderr, "\nFailed to get RISC-V FW version, ret=%s", lt_ret_verbose(ret));
        return LT_FAIL;
    }
    ret = lt_get_info_spect_fw_ver(lt_handle, spect_fw_ver);
    if (ret != LT_OK) {
        fprintf(stderr, "\nFailed to get SPECT FW version, ret=%s", lt_ret_verbose(ret));
        return LT_FAIL;
    }
    printf("OK\n");

    printf("TROPIC01 firmware versions:\n");
    printf("  - RISC-V FW version: %d.%d.%d\n", cpu_fw_ver[3], cpu_fw_ver[2], cpu_fw_ver[1]);
    printf("  - SPECT FW version: %d.%d.%d\n", spect_fw_ver[3], spect_fw_ver[2], spect_fw_ver[1]);

    return LT_OK;
}

int main(void)
{
    printf("==========================================\n");
    printf("==== TROPIC01 Firmware Update Example ====\n");
    printf("==========================================\n");
    
    // Cryptographic function provider initialization.
    //                                                           
    // In production, this would typically be done only once,    
    // usually at the start of the application or before         
    // the first use of cryptographic functions but no later than
    // the first occurrence of any Libtropic function            
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        fprintf(stderr, "PSA Crypto initialization failed, status=%d (psa_status_t)\n", status);
        return -1;
    }
                              
    // Libtropic handle.
    //
    // It is declared here (on stack) for        
    // simplicity. In production, you put it on heap if needed.
    lt_handle_t lt_handle = {0};
#if LT_SEPARATE_L3_BUFF
    uint8_t l3_buffer[LT_SIZE_OF_L3_BUFF] __attribute__((aligned(16))) = {0};
    lt_handle.l3.buff = l3_buffer;
    lt_handle.l3.buff_len = sizeof(l3_buffer);
#endif
         
    // Device structure.
    //
    // Modify this according to your environment. Default values
    // are compatible with RPi and our RPi shield.              
    lt_dev_posix_usb_dongle_t device = {0};
    strcpy(device.dev_path, LT_USB_DEVKIT_PATH); // LT_USB_DEVKIT_PATH is defined in CMakeLists.txt. Pass -DLT_USB_DEVKIT_PATH=<path> to cmake if you want to change it.
    device.baud_rate = 115200;
    lt_handle.l2.device = &device;


    // Crypto abstraction layer (CAL) context.
    lt_ctx_mbedtls_v4_t crypto_ctx;
    lt_handle.l3.crypto_ctx = &crypto_ctx;

    printf("Initializing handle...");
    lt_ret_t ret = lt_init(&lt_handle);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to initialize handle, ret=%s\n", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("OK\n");

    // First, we check versions of both updateable firmwares. To do that, we need TROPIC01 to **not** be in the Start-up
    // Mode. If there are valid firmwares, TROPIC01 will begin to execute them automatically on boot.
    printf("Rebooting TROPIC01...");
    ret = lt_reboot(&lt_handle, TR01_REBOOT);
    if (ret != LT_OK) {
        fprintf(stderr, "\nlt_reboot() failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("OK\n");

    if(get_fw_versions(&lt_handle) != LT_OK) {
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }

    printf("Versions to update to:\n");
    printf("  - RISC-V FW version: %d.%d.%d\n", 6, 6, 6);
    printf("  - SPECT FW version: %d.%d.%d\n", 6, 6, 6);

    printf("Proceed with update? [y/N]: ");
    char user_input = getchar();
    if (user_input != 'y' && user_input != 'Y') {
        printf("\nUpdate cancelled by user.\n");
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return 0;
    }
    printf("\nStarting firmware update...\n");


    // The chip must be in Start-up Mode to be able to perform a firmware update.
    printf("- Sending maintenance reboot request...");
    ret = lt_reboot(&lt_handle, TR01_MAINTENANCE_REBOOT);
    if (ret != LT_OK) {
        fprintf(stderr, "\nlt_reboot() failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("OK\n");

    printf("- Updating TR01_FW_BANK_FW1 and TR01_FW_BANK_SPECT1\n");
    printf("  - Updating RISC-V FW...");
    ret = lt_do_mutable_fw_update(&lt_handle, fw_CPU, sizeof(fw_CPU), TR01_FW_BANK_FW1);
    if (ret != LT_OK) {
        fprintf(stderr, "\nRISC-V FW update failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("OK\n");

    printf("  - Updating SPECT FW...");
    ret = lt_do_mutable_fw_update(&lt_handle, fw_SPECT, sizeof(fw_SPECT), TR01_FW_BANK_SPECT1);
    if (ret != LT_OK) {
        fprintf(stderr, "\nSPECT FW update failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("OK\n");

    printf("- Updating TR01_FW_BANK_FW2 and TR01_FW_BANK_SPECT2\n");
    printf("  - Updating RISC-V FW...");
    ret = lt_do_mutable_fw_update(&lt_handle, fw_CPU, sizeof(fw_CPU), TR01_FW_BANK_FW2);
    if (ret != LT_OK) {
        fprintf(stderr, "\nRISC-V FW update failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("OK\n");

    printf("  - Updating SPECT FW...");
    ret = lt_do_mutable_fw_update(&lt_handle, fw_SPECT, sizeof(fw_SPECT), TR01_FW_BANK_SPECT2);
    if (ret != LT_OK) {
        fprintf(stderr, "\nSPECT FW update failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("OK\n");
    printf("Successfully updated all 4 FW banks.\n\n");

    printf("Sending reboot request...");
    ret = lt_reboot(&lt_handle, TR01_REBOOT);
    if (ret != LT_OK) {
        fprintf(stderr, "\nlt_reboot() failed, ret=%s", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("OK!\nTROPIC01 is executing Application FW now\n");

    if(get_fw_versions(&lt_handle) != LT_OK) {
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }

    printf("Deinitializing handle...");
    ret = lt_deinit(&lt_handle);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to deinitialize handle, ret=%s\n", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("OK\n");
    
    // Cryptographic function provider deinitialization.
    //                                                           
    // In production, this would be done only once, typically     
    // during termination of the application.                     
    mbedtls_psa_crypto_free();

    return 0;
}