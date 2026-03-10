#include <stdint.h>
#include <string.h>

#include "main.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_time.h"
#include "mbedtls/platform_util.h"
#include "stm32u5xx_hal.h"

mbedtls_ms_time_t mbedtls_ms_time(void)
{
    /* Use STM32 HAL millisecond tick. HAL_GetTick() returns uint32_t ms since startup.
       See Page 48 of UM1725 Rev 8. */
    return (mbedtls_ms_time_t)HAL_GetTick();
}

int mbedtls_platform_get_entropy(psa_driver_get_entropy_flags_t flags, size_t *estimate_bits,
                                 unsigned char *output, size_t output_size)
{
    /* We don't implement any flags. */
    if (flags != 0) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    HAL_StatusTypeDef hal_status = HAL_OK;
    uint32_t random_data;
    size_t bytes_left = output_size;

    while (bytes_left) {
        hal_status = HAL_RNG_GenerateRandomNumber(&hrng, &random_data);
        if (hal_status != HAL_OK) {
            mbedtls_platform_zeroize(&random_data, sizeof(random_data));
            return PSA_ERROR_INSUFFICIENT_ENTROPY;
        }

        size_t cpy_cnt = bytes_left < sizeof(random_data) ? bytes_left : sizeof(random_data);
        memcpy(output, &random_data, cpy_cnt);
        bytes_left -= cpy_cnt;
        output += cpy_cnt;
    }

    *estimate_bits = 8 * output_size;

    mbedtls_platform_zeroize(&random_data, sizeof(random_data));
    return 0;
}