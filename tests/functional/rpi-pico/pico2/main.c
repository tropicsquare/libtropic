/**
 * @file main.c
 * @brief Common entrypoint for running functional tests against Raspberry Pi Pico 2.
 * @copyright Copyright (c) 2020-2026 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <stdio.h>

#include "hardware/spi.h"
#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "libtropic_port_rpi_pico.h"
#include "lt_test_common.h"
#include "pico/stdlib.h"

#if LT_USE_MBEDTLS_V4
#include "libtropic_mbedtls_v4.h"
#include "psa/crypto.h"
#define CRYPTO_CTX_TYPE lt_ctx_mbedtls_v4_t
#elif LT_USE_WOLFCRYPT
#include "libtropic_wolfcrypt.h"
#include "wolfssl/wolfcrypt/error-crypt.h"
#include "wolfssl/wolfcrypt/wc_port.h"
#define CRYPTO_CTX_TYPE lt_ctx_wolfcrypt_t
#endif

// Cleanup function (pass NULL as Libtropic handle to not deinitialize it).
static void cleanup(void)
{
#if LT_USE_MBEDTLS_V4
    mbedtls_psa_crypto_free();
#elif LT_USE_WOLFCRYPT
    int ret = wolfCrypt_Cleanup();
    if (ret != 0) {
        LT_LOG_ERROR("WolfCrypt cleanup failed, ret=%d (%s)", ret, wc_GetErrorString(ret));
    }
#endif

    // Stay alive so the board is available via USB.
    while (1) {
        tight_loop_contents();
    }
}

int main(void)
{
    stdio_init_all();

    // Disable buffering.
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    // Loop until the USB CDC serial connection is actively opened by your PC.
    // This prevents loosing output from this example after flashing and before opening the serial.
    while (!stdio_usb_connected()) {
        sleep_ms(10);
    }

    // CFP initialization.
#if LT_USE_MBEDTLS_V4
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        LT_LOG_ERROR("PSA Crypto initialization failed, status=%d (psa_status_t)", status);
        cleanup();
    }
#elif LT_USE_WOLFCRYPT
    ret = wolfCrypt_Init();
    if (ret != 0) {
        LT_LOG_ERROR("WolfCrypt initialization failed, ret=%d (%s)", ret, wc_GetErrorString(ret));
        cleanup();
    }
#endif

    // Handle initialization.
    lt_handle_t lt_handle = {0};
#if LT_SEPARATE_L3_BUFF
    uint8_t l3_buffer[LT_SIZE_OF_L3_BUFF] __attribute__((aligned(16))) = {0};
    lt_handle.l3.buff = l3_buffer;
    lt_handle.l3.buff_len = sizeof(l3_buffer);
#endif

    // Device mappings.
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

    // CAL context (selectable)
    CRYPTO_CTX_TYPE crypto_ctx;
    lt_handle.l3.crypto_ctx = &crypto_ctx;

    // Test code (correct test function is selected automatically per binary).
    // __lt_handle__ identifier is used by the test registry.
    lt_handle_t *__lt_handle__ = &lt_handle;
#include "lt_test_registry.c.inc"

    LT_FINISH_TEST();

    cleanup();

    return 0;  // This should not be reached.
}