#ifndef TS_L1_H
#define TS_L1_H

/**
* @file ts_l1.h
* @brief Testing support for l1 unit tests. Following functions will be mocked up by cmock, because normally they are platform defined.
* @author Tropic Square s.r.o.
*/

#include "libtropic_common.h"

#define CHIP_MODE_READY_mask    0x01
#define CHIP_MODE_ALARM_mask    0x02
#define CHIP_MODE_STARTUP_mask  0x04

#define TS_L1_READ_MAX_TRIES    10

#define TS_L1_TIMEOUT_MS_MIN 5
#define TS_L1_TIMEOUT_MS_DEFAULT 70
#define TS_L1_TIMEOUT_MS_MAX 150
#define TS_L1_DELAY_MS_MAX 500

// Support function definition - definition will be mocked up by cmock
ts_ret_t ts_l1_spi_csn_low(const ts_handle_t *h);

// Support function definition - definition will be mocked up by cmock
ts_ret_t ts_l1_spi_csn_high(const ts_handle_t *h);

// Support function definition - definition will be mocked up by cmock
ts_ret_t ts_l1_spi_transfer(ts_handle_t *h, uint8_t offset, uint16_t tx_len, uint32_t timeout);

// Support function definition - definition will be mocked up by cmock
ts_ret_t ts_l1_platform_init(ts_handle_t *h);

// Support function definition - definition will be mocked up by cmock
ts_ret_t ts_l1_platform_deinit(ts_handle_t *h);

// Support function definition - definition will be mocked up by cmock
ts_ret_t ts_l1_delay(const ts_handle_t *h, const uint32_t ms);

#endif
