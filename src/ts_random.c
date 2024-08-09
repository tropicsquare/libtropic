/**
* @file   ts_random.c
* @brief  API for providing random numbers from host platform RNG. Weak function, which is meant to be defined in platform HAL file.
* @author Tropic Square s.r.o.
*/

#include "stdint.h"
#include "libtropic_common.h"
//
//#if ((TS_HAL_UNIX == 0) && (TS_HAL_STM32_SPI == 0) && (TS_HAL_STM32_UART == 0) && (TEST == 0)) // && other platforms here
//#pragma message("Provide own implementation for:    ts_random_bytes()")
//
//__attribute__((weak)) ts_ret_t ts_random_bytes(uint8_t *buff, uint16_t len) {
//    UNUSED(buff);
//    UNUSED(len);
//    return TS_FAIL;
//}

//#endif
