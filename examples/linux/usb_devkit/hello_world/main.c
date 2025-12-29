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

#include "libtropic_mbedtls_v4.h"
#include "psa/crypto.h"

/** @brief Message to send with Ping L3 command. */
#define PING_MSG "This is Hello World message from TROPIC01!!"
/** @brief Size of the Ping message, including '\0'. */
#define PING_MSG_SIZE 44

int main(void)
{
    printf("======================================\n");
    printf("==== TROPIC01 Hello World Example ====\n");
    printf("======================================\n");

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

    printf("Starting Secure Session with key slot %d...", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    ret = lt_verify_chip_and_start_secure_session(&lt_handle, LT_EX_SH0_PRIV, LT_EX_SH0_PUB, TR01_PAIRING_KEY_SLOT_INDEX_0);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to start Secure Session with key %d, ret=%s\n", (int)TR01_PAIRING_KEY_SLOT_INDEX_0,
                     lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("OK\n");

    uint8_t recv_buf[PING_MSG_SIZE];
    printf("Sending Ping command...\n");
    printf("\t--> Message sent to TROPIC01: '%s'\n", PING_MSG);
    ret = lt_ping(&lt_handle, (const uint8_t *)PING_MSG, recv_buf, PING_MSG_SIZE);
    if (LT_OK != ret) {
        fprintf(stderr, "Ping command failed, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(&lt_handle);
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("\t<-- Message received from TROPIC01: '%s'\n", recv_buf);

    printf("Aborting Secure Session...");
    ret = lt_session_abort(&lt_handle);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to abort Secure Session, ret=%s\n", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("OK\n");

    printf("Deinitializing handle...");
    ret = lt_deinit(&lt_handle);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to deinitialize handle, ret=%s\n", lt_ret_verbose(ret));
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