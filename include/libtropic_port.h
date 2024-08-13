#ifndef TS_PORT_H
#define TS_PORT_H

/**
* @file ts_l1.h
* @brief Header file with layer 1 interfaces
* @author Tropic Square s.r.o.
*/

#include "libtropic_common.h"

/**
 * @brief Platform agonostic init function, configurable during build. Check libtropic's documentation for more info about platform configuration.
 *
 * @param h           Chip's handle
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_port_init(ts_handle_t *h);

/**
 * @brief Platform agonostic deinit function, configurable during build. Check libtropic's documentation for more info about platform configuration.
 *
 * @param h           Chip's handle
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_port_deinit(ts_handle_t *h);

/**
 * @brief Set chip select pin low
 *
 * @param h           Chip's handle
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_port_spi_csn_low(ts_handle_t *h);

/**
 * @brief Set chip select pin high
 *
 * @param h           Chip's handle
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_port_spi_csn_high(ts_handle_t *h);

/**
 * @brief Do l1 transfer
 *
 * @param h           Chip's handle
 * @param tx_len      The length of data to be transferred
 * @param offset      Offset in handle's internal buffer where incomming bytes should be stored into
 * @param timeout     Timeout
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_port_spi_transfer(ts_handle_t *h, uint8_t offset, uint16_t tx_len, uint32_t timeout);

/**
 * @brief Platform's definition for delay, specifies what host platform should do when libtropic's functions need some delay
 *
 * @param h           Chip's handle
 * @param ms          Time to wait in miliseconds
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_port_delay(ts_handle_t *h, uint32_t ms);

/**
 * @brief Fill buffer with random bytes
 *
 * @param buff        Buffer to be filled
 * @param len         number of 32bit numbers
 * @return ts_ret_t   S_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_port_random_bytes(uint32_t *buff, uint16_t len);

#endif
