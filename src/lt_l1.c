/**
 * @file lt_l1.c
 * @brief Layer 1 functions definitions
 * @author Tropic Square s.r.o.
 */


#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "libtropic_common.h"
#include "libtropic_port.h"
#include "lt_l1.h"

/**
 * @brief Sets chip select pin low. This is static wrapper for platform defined function.
 *
 * @param h           Chip's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
static lt_ret_t lt_l1_spi_csn_low(lt_handle_t *h);

/**
 * @brief Set chip select pin high. This is static wrapper for platform defined function.
 *
 * @param h           Chip's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
static lt_ret_t lt_l1_spi_csn_high(lt_handle_t *h);

/**
 * @brief Do l1 transfer. This is static wrapper for platform defined function.
 *
 * @param h           Chip's handle
 * @param tx_len      The length of data to be transferred
 * @param offset      Offset in handle's internal buffer where incomming bytes should be stored into
 * @param timeout     Timeout
 * @return            LT_OK if success, otherwise returns other error code.
 */
static lt_ret_t lt_l1_spi_transfer(lt_handle_t *h, uint8_t offset, uint16_t tx_len, uint32_t timeout);

/**
 * @brief Platform's definition for delay, specifies what host
 *        platform should do when libtropic's functions need some delay. 
 *        This is static wrapper for platform defined function.
 *
 * @param h           Chip's handle
 * @param ms          Time to wait in miliseconds
 * @return            LT_OK if success, otherwise returns other error code.
 */
static lt_ret_t lt_l1_delay(lt_handle_t *h, uint32_t ms);

//------------------------------------------------------------------------------------------------------//

static lt_ret_t lt_l1_spi_csn_low(lt_handle_t *h)
{
#ifdef LIBT_DEBUG
    if(!h) {
        return LT_PARAM_ERR;
    }
#endif

    return lt_port_spi_csn_low(h);
}

static lt_ret_t lt_l1_spi_csn_high(lt_handle_t *h)
{
#ifdef LIBT_DEBUG
    if(!h) {
        return LT_PARAM_ERR;
    }
#endif

    return lt_port_spi_csn_high(h);
}

static lt_ret_t lt_l1_spi_transfer(lt_handle_t *h, uint8_t offset, uint16_t tx_len, uint32_t timeout)
{
#ifdef LIBT_DEBUG
    if(!h) {
        return LT_PARAM_ERR;
    }
#endif

    return lt_port_spi_transfer(h, offset, tx_len, timeout);
}

static lt_ret_t lt_l1_delay(lt_handle_t *h, uint32_t ms)
{
#ifdef LIBT_DEBUG
    if(!h) {
        return LT_PARAM_ERR;
    }
#endif

    return lt_port_delay(h, ms);
}

lt_ret_t lt_l1_init(lt_handle_t *h)
{
#ifdef LIBT_DEBUG
    if(!h) {
        return LT_PARAM_ERR;
    }
#endif

    return lt_port_init(h);
}

lt_ret_t lt_l1_deinit(lt_handle_t *h)
{
#ifdef LIBT_DEBUG
    if(!h) {
        return LT_PARAM_ERR;
    }
#endif

    return lt_port_deinit(h);
}

lt_ret_t lt_l1_read(lt_handle_t *h, const uint32_t max_len, const uint32_t timeout)
{
#ifdef LIBT_DEBUG
    if(!h) {
        return LT_PARAM_ERR;
    }
    if ((timeout<LT_L1_TIMEOUT_MS_MIN) | (timeout > LT_L1_TIMEOUT_MS_MAX)) {
        return LT_PARAM_ERR;
    }
    if ((max_len < LT_L1_LEN_MIN) | (max_len > LT_L1_LEN_MAX)) {
        return LT_PARAM_ERR;
    }
#endif
    int max_tries = LT_L1_READ_MAX_TRIES;

    while(max_tries > 0) {

        max_tries--;

        h->l2_buff[0] = GET_INFO_REQ_ID;

        // Try to read CHIP_STATUS
        lt_l1_spi_csn_low(h);
        if (lt_l1_spi_transfer(h, 0, 1, timeout) != LT_OK)
        {
            lt_l1_spi_csn_high(h);
            return LT_L1_SPI_ERROR;
        }

        // Check ALARM bit of CHIP_STATUS
        if (h->l2_buff[0] & CHIP_MODE_ALARM_bit) {
            lt_l1_spi_csn_high(h);
            return LT_L1_CHIP_ALARM_MODE;
        }
        // Check STARTUP bit of CHIP_STATUS
        else if (h->l2_buff[0] & CHIP_MODE_STARTUP_bit) {
            lt_l1_spi_csn_high(h);
            return LT_L1_CHIP_STARTUP_MODE;
        }
        // Check READY bit of CHIP_STATUS
        else if (h->l2_buff[0] & CHIP_MODE_READY_bit) {
            // receive STATUS byte and length byte
            if (lt_l1_spi_transfer(h, 1, 2, timeout) != LT_OK) //offset 1
            {
                lt_l1_spi_csn_high(h);
                return LT_L1_SPI_ERROR;
            }
            // TODO Better to read two bytes, not one. Then length.
            // This function as it is now will return chip in alarm mode when
            // spi wires are not connected (and therefore reading 0xff), but I want to get chip busy instead.
            // Hotfix for fpga no resp handling:
            if (h->l2_buff[1] == 0xff) {
                lt_l1_spi_csn_high(h);
                lt_l1_delay(h, LT_L1_READ_RETRY_DELAY);
                continue;
            }

            // Take length information and add 2B for crc bytes
            uint16_t length = h->l2_buff[2] + 2;
            if(length > (LT_L1_LEN_MAX - 2)) {
                lt_l1_spi_csn_high(h);
                return LT_L1_DATA_LEN_ERROR;
            }
            // Receive the rest of incomming bytes, including crc
            if (lt_l1_spi_transfer(h, 3, length, timeout) != LT_OK) // offset 3
            {
                lt_l1_spi_csn_high(h);
                return LT_L1_SPI_ERROR;
            }
            lt_l1_spi_csn_high(h);
            return LT_OK;

        // Chip status does not contain any special mode bit and also is not ready,
        // try it again (until max_tries runs out)
        } else {
            lt_l1_spi_csn_high(h);
            lt_l1_delay(h, LT_L1_READ_RETRY_DELAY);
        }

    }

    return LT_L1_CHIP_BUSY;
}

lt_ret_t lt_l1_write(lt_handle_t *h, const uint16_t len, const uint32_t timeout)
{
#ifdef LIBT_DEBUG
    if(!h) {
        return LT_PARAM_ERR;
    }
    if ((timeout<LT_L1_TIMEOUT_MS_MIN) | (timeout > LT_L1_TIMEOUT_MS_MAX)) {
        return LT_PARAM_ERR;
    }
    if ((len < LT_L1_LEN_MIN) | (len > LT_L1_LEN_MAX)) {
        return LT_PARAM_ERR;
    }
#endif
    lt_l1_spi_csn_low(h);

    if (lt_l1_spi_transfer(h, 0, len, timeout) != LT_OK)
    {
        lt_l1_spi_csn_high(h);
        return LT_L1_SPI_ERROR;
    }

    lt_l1_spi_csn_high(h);

    return LT_OK;
}