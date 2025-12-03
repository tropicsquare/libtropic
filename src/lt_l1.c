/**
 * @file lt_l1.c
 * @brief Layer 1 functions definitions
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */
#include "lt_l1.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "libtropic_common.h"
#include "libtropic_logging.h"
#include "libtropic_macros.h"
#include "lt_port_wrap.h"

#ifdef LT_PRINT_SPI_DATA
#include "stdio.h"
#define LT_L1_SPI_DIR_MISO 0
#define LT_L1_SPI_DIR_MOSI 1
static void print_hex_chunks(const uint8_t *data, uint8_t len, uint8_t dir)
{
    if ((!data) || (len == 0)) {
        return;
    }
    printf("%s", dir ? "  >>  TX: " : "  <<  RX: ");
    for (size_t i = 0; i < len; i++) {
        printf("%02" PRIX8 " ", data[i]);
        if ((i + 1) % 32 == 0) {
            printf("\n          ");
        }
    }
    printf("\n");
}
#endif

lt_ret_t lt_l1_read(lt_l2_state_t *s2, const uint32_t max_len, const uint32_t timeout_ms)
{
#ifdef LT_REDUNDANT_ARG_CHECK
    if (!s2) {
        return LT_PARAM_ERR;
    }
    if ((timeout_ms < LT_L1_TIMEOUT_MS_MIN) | (timeout_ms > LT_L1_TIMEOUT_MS_MAX)) {
        return LT_PARAM_ERR;
    }
    if ((max_len < TR01_L1_LEN_MIN) | (max_len > TR01_L1_LEN_MAX)) {
        return LT_PARAM_ERR;
    }
#else
    LT_UNUSED(max_len);
#endif

    lt_ret_t ret;
    int max_tries = LT_L1_READ_MAX_TRIES;

    while (max_tries > 0) {
        max_tries--;

        s2->buff[0] = TR01_L1_GET_RESPONSE_REQ_ID;

        // Try to read CHIP_STATUS byte
        ret = lt_l1_spi_csn_low(s2);
        if (ret != LT_OK) {
            return ret;
        }

        ret = lt_l1_spi_transfer(s2, 0, 1, timeout_ms);
        if (ret != LT_OK) {
            lt_ret_t ret_unused = lt_l1_spi_csn_high(s2);
            LT_UNUSED(ret_unused);  // We don't care about it, we return ret from SPI transfer anyway.
            return ret;
        }

        // Check ALARM bit of CHIP_STATUS byte
        if (s2->buff[0] & TR01_L1_CHIP_MODE_ALARM_bit) {
            lt_ret_t ret_unused = lt_l1_spi_csn_high(s2);
            LT_UNUSED(ret_unused);  // We don't care about it, we return LT_L1_CHIP_ALARM_MODE anyway.

            LT_LOG_DEBUG("CHIP_STATUS: 0x%02" PRIX8, s2->buff[0]);
            return LT_L1_CHIP_ALARM_MODE;
        }

        // Proceed further in case CHIP_STATUS contains READY bit, signalizing that chip is ready to receive request
        if (s2->buff[0] & TR01_L1_CHIP_MODE_READY_bit) {
            // receive STATUS byte and length byte
            ret = lt_l1_spi_transfer(s2, 1, 2, timeout_ms);
            if (ret != LT_OK) {  // offset 1
                lt_ret_t ret_unused = lt_l1_spi_csn_high(s2);
                LT_UNUSED(ret_unused);  // We don't care about it, we return ret from SPI transfer anyway.
                return ret;
            }

            // 0xFF received in second byte means that chip has no response to send.
            if (s2->buff[1] == 0xff) {
                ret = lt_l1_spi_csn_high(s2);
                if (ret != LT_OK) {
                    return ret;
                }
                ret = lt_l1_delay(s2, LT_L1_READ_RETRY_DELAY);
                if (ret != LT_OK) {
                    return ret;
                }
                continue;
            }

            // Take length information and add 2B for crc bytes
            uint16_t length = s2->buff[2] + 2;
            if (length > (TR01_L1_LEN_MAX - 2)) {
                lt_ret_t ret_unused = lt_l1_spi_csn_high(s2);
                LT_UNUSED(ret_unused);  // We don't care about it, we return LT_L1_DATA_LEN_ERROR anyway.
                return LT_L1_DATA_LEN_ERROR;
            }
            // Receive the rest of incomming bytes, including crc
            ret = lt_l1_spi_transfer(s2, 3, length, timeout_ms);
            if (ret != LT_OK) {  // offset 3
                lt_ret_t ret_unused = lt_l1_spi_csn_high(s2);
                LT_UNUSED(ret_unused);  // We don't care about it, we return ret from SPI transfer anyway.
                return ret;
            }
            ret = lt_l1_spi_csn_high(s2);
            if (ret != LT_OK) {
                return ret;
            }
#ifdef LT_PRINT_SPI_DATA
            print_hex_chunks(s2->buff, s2->buff[2] + 5, LT_L1_SPI_DIR_MISO);
#endif
            return LT_OK;

            // Chip status does not contain any special mode bit and also is not ready,
            // try it again (until max_tries runs out)
        }
        else {
            ret = lt_l1_spi_csn_high(s2);
            if (ret != LT_OK) {
                return ret;
            }
            if (s2->buff[0] & TR01_L1_CHIP_MODE_STARTUP_bit) {
                // INT pin is not implemented in Start-up Mode
                // So we wait a bit before we poll again for CHIP_STATUS
                ret = lt_l1_delay(s2, LT_L1_READ_RETRY_DELAY);
                if (ret != LT_OK) {
                    return ret;
                }
            }
            else {
#if LT_USE_INT_PIN
                // Wait for rising edge on the INT pin, which signalizes that L2 Response frame is ready to be received
                ret = lt_l1_delay_on_int(s2, LT_L1_TIMEOUT_MS_MAX);
                if (ret != LT_OK) {
                    return ret;
                }
#else
                // INT pin not used, delay for some time
                ret = lt_l1_delay(s2, LT_L1_READ_RETRY_DELAY);
                if (ret != LT_OK) {
                    return ret;
                }
#endif
            }
        }
    }

    return LT_L1_CHIP_BUSY;
}

lt_ret_t lt_l1_write(lt_l2_state_t *s2, const uint16_t len, const uint32_t timeout_ms)
{
#ifdef LT_REDUNDANT_ARG_CHECK
    if (!s2) {
        return LT_PARAM_ERR;
    }
    if ((timeout_ms < LT_L1_TIMEOUT_MS_MIN) | (timeout_ms > LT_L1_TIMEOUT_MS_MAX)) {
        return LT_PARAM_ERR;
    }
    if ((len < TR01_L1_LEN_MIN) | (len > TR01_L1_LEN_MAX)) {
        return LT_PARAM_ERR;
    }
#endif

    lt_ret_t ret;

    ret = lt_l1_spi_csn_low(s2);
    if (ret != LT_OK) {
        return ret;
    }
#ifdef LT_PRINT_SPI_DATA
    print_hex_chunks(s2->buff, len, LT_L1_SPI_DIR_MOSI);
#endif
    ret = lt_l1_spi_transfer(s2, 0, len, timeout_ms);
    if (ret != LT_OK) {
        lt_ret_t ret_unused = lt_l1_spi_csn_high(s2);
        LT_UNUSED(ret_unused);  // We don't care about it, we return ret from SPI transfer anyway.
        return ret;
    }

    ret = lt_l1_spi_csn_high(s2);
    if (ret != LT_OK) {
        return ret;
    }

    return LT_OK;
}
