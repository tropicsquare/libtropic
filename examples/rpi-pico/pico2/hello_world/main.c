/**
 * @file main.c
 * @brief Simple "Hello, World!" example of using Libtropic with Raspberry Pi Pico 2.
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

// @brief Message to send with Ping L3 command.
#define PING_MSG "This is Hello World message from TROPIC01!!"
// Size of the Ping message, including '\0'.
#define PING_MSG_SIZE 44

// Choose pairing keypair for slot 0.
#if LT_USE_SH0_ENG_SAMPLE
#define LT_EX_SH0_PRIV lt_sh0priv_eng_sample
#define LT_EX_SH0_PUB lt_sh0pub_eng_sample
#elif LT_USE_SH0_PROD0
#define LT_EX_SH0_PRIV lt_sh0priv_prod0
#define LT_EX_SH0_PUB lt_sh0pub_prod0
#endif

/**
 * @brief Cleanup function.
 *
 * @param lt_handle  Handle for communication with TROPIC01 (pass NULL to not call lt_deinit())
 */
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

    printf("======================================\n");
    printf("==== TROPIC01 Hello World Example ====\n");
    printf("======================================\n");

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

    // We need to ensure we are not in the Startup Mode, as L3 commands are available only in the
    // Application Firmware.
    printf("Sending reboot request...");
    ret = lt_reboot(&lt_handle, TR01_REBOOT);
    if (ret != LT_OK) {
        fprintf(stderr, "\nlt_reboot() failed, ret=%s\n", lt_ret_verbose(ret));
        cleanup(&lt_handle);
    }
    printf("OK\n");

    printf("Starting Secure Session with key slot %d...", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    // Keys are chosen based on the CMake option LT_SH0_KEYS.
    ret = lt_verify_chip_and_start_secure_session(&lt_handle, LT_EX_SH0_PRIV, LT_EX_SH0_PUB,
                                                  TR01_PAIRING_KEY_SLOT_INDEX_0);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to start Secure Session with key %d, ret=%s\n",
                (int)TR01_PAIRING_KEY_SLOT_INDEX_0, lt_ret_verbose(ret));
        fprintf(stderr,
                "Check if you use correct SH0 keys! Hint: if you use an engineering sample chip, "
                "compile with "
                "-DLT_SH0_KEYS=eng_sample\n");
        cleanup(&lt_handle);
    }
    printf("OK\n");

    uint8_t recv_buf[PING_MSG_SIZE];
    printf("Sending Ping command...\n");
    printf("\t--> Message sent to TROPIC01: '%s'\n", PING_MSG);
    ret = lt_ping(&lt_handle, (const uint8_t *)PING_MSG, recv_buf, PING_MSG_SIZE);
    if (LT_OK != ret) {
        fprintf(stderr, "Ping command failed, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(&lt_handle);
        cleanup(&lt_handle);
    }
    printf("\t<-- Message received from TROPIC01: '%s'\n", recv_buf);

    printf("Aborting Secure Session...");
    ret = lt_session_abort(&lt_handle);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to abort Secure Session, ret=%s\n", lt_ret_verbose(ret));
        cleanup(&lt_handle);
    }
    printf("OK\n");

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
