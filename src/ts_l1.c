/**
* @file
* @author Tropic Square s.r.o.
* @version 0.1
*
*
* @brief DESCRIPTION
*
*/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "libtropic_common.h"
#include "libtropic_platform.h"
#include "ts_l1.h"

ts_ret_t ts_l1_init(ts_handle_t *h)
{
    return ts_l1_platform_init(h);
}

ts_ret_t ts_l1_deinit(ts_handle_t *h)
{
    return ts_l1_platform_deinit(h);
}

ts_ret_t ts_l1_read(ts_handle_t *h, const uint32_t max_len, const uint32_t timeout)
{
    if (!h) {
        return TS_PARAM_ERR;
    }
    if ((timeout<TS_L1_TIMEOUT_MS_MIN) | (timeout > TS_L1_TIMEOUT_MS_MAX)) {
        return TS_PARAM_ERR;
    }
    if ((max_len < TS_L1_LEN_MIN) | (max_len > TS_L1_LEN_MAX)) {
        return TS_PARAM_ERR;
    }

    int max_tries = TS_L1_READ_MAX_TRIES;

    while(max_tries > 0) {

        max_tries--;

        h->l2_buff[0] = GET_INFO_REQ_ID;

        // Try to read CHIP_STATUS
        ts_l1_spi_csn_low(h);
        if (ts_l1_spi_transfer(h, 0, 1, timeout) != TS_OK)
        {
            ts_l1_spi_csn_high(h);
            return TS_L1_SPI_ERROR;
        }

        // Check ALARM bit of CHIP_STATUS
        if (h->l2_buff[0] & CHIP_MODE_ALARM_bit) {
            ts_l1_spi_csn_high(h);
            return TS_L1_CHIP_ALARM_MODE;
        }
        // Check STARTUP bit of CHIP_STATUS
        else if (h->l2_buff[0] & CHIP_MODE_STARTUP_bit) {
            ts_l1_spi_csn_high(h);
            return TS_L1_CHIP_STARTUP_MODE;
        }
        // Check READY bit of CHIP_STATUS
        else if (h->l2_buff[0] & CHIP_MODE_READY_bit) {
            // receive STATUS byte and length byte
            if (ts_l1_spi_transfer(h, 1, 2, timeout) != TS_OK) //offset 1
            {
                ts_l1_spi_csn_high(h);
                return TS_L1_SPI_ERROR;
            }
            // TODO Better to read two bytes, not one. Then length.
            // This function as it is now will return chip in alarm mode when
            // spi wires are not connected (and therefore reading 0xff), but I want to get chip busy instead.
            // Hotfix for fpga no resp handling:
            if (h->l2_buff[1] == 0xff) {
                ts_l1_spi_csn_high(h);
                ts_l1_delay(h, TS_L1_READ_RETRY_DELAY);
                continue;
            }

            // Take length information and add 2B for crc bytes
            uint16_t length = h->l2_buff[2] + 2;
            if(length > (TS_L1_LEN_MAX - 2)) {
                ts_l1_spi_csn_high(h);
                return TS_L1_DATA_LEN_ERROR;
            }
            // Receive the rest of incomming bytes, including crc
            if (ts_l1_spi_transfer(h, 3, length, timeout) != TS_OK) // offset 3
            {
                ts_l1_spi_csn_high(h);
                return TS_L1_SPI_ERROR;
            }
            ts_l1_spi_csn_high(h);
            return TS_OK;

        // Chip status does not contain any special mode bit and also is not ready,
        // try it again (until max_tries runs out)
        } else {
            ts_l1_spi_csn_high(h);
            ts_l1_delay(h, TS_L1_READ_RETRY_DELAY);
        }

    }

    return TS_L1_CHIP_BUSY;
}

ts_ret_t ts_l1_write(ts_handle_t *h, const uint16_t len, const uint32_t timeout)
{
    if (!h) {
        return TS_PARAM_ERR;
    }
    if ((timeout<TS_L1_TIMEOUT_MS_MIN) | (timeout > TS_L1_TIMEOUT_MS_MAX)) {
        return TS_PARAM_ERR;
    }
    if ((len < TS_L1_LEN_MIN) | (len > TS_L1_LEN_MAX)) {
        return TS_PARAM_ERR;
    }

    ts_l1_spi_csn_low(h);

    if (ts_l1_spi_transfer(h, 0, len, timeout) != TS_OK)
    {
        ts_l1_spi_csn_high(h);
        return TS_L1_SPI_ERROR;
    }

    ts_l1_spi_csn_high(h);

    return TS_OK;
}
