/**
 * @file lt_l1.c
 * @brief Layer 1 functions definitions
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */
#include "lt_l1.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "libtropic_common.h"
#include "libtropic_macros.h"
#include "lt_l1_port_wrap.h"

#ifdef LT_PRINT_SPI_DATA
#include "stdio.h"
#define SPI_DIR_MISO 0
#define SPI_DIR_MOSI 1
void print_hex_chunks(const uint8_t *data, uint8_t len, uint8_t dir)
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
#ifdef LIBT_DEBUG
    if (!s2) {
        return LT_PARAM_ERR;
    }
    if ((timeout_ms < LT_L1_TIMEOUT_MS_MIN) | (timeout_ms > LT_L1_TIMEOUT_MS_MAX)) {
        return LT_PARAM_ERR;
    }
    if ((max_len < LT_L1_LEN_MIN) | (max_len > LT_L1_LEN_MAX)) {
        return LT_PARAM_ERR;
    }
#else
    UNUSED(max_len);
#endif

    lt_ret_t ret;
    int max_tries = LT_L1_READ_MAX_TRIES;

    while (max_tries > 0) {
        max_tries--;

        s2->buff[0] = GET_RESPONSE_REQ_ID;

        // Try to read CHIP_STATUS byte
        ret = lt_l1_spi_csn_low(s2);
        if (ret != LT_OK) {
            return ret;
        }

        ret = lt_l1_spi_transfer(s2, 0, 1, timeout_ms);
        if (ret != LT_OK) {
            lt_ret_t ret_unused = lt_l1_spi_csn_high(s2);
            UNUSED(ret_unused);  // We don't care about it, we return ret from SPI transfer anyway.
            return ret;
        }

        // Check ALARM bit of CHIP_STATUS byte
        if (s2->buff[0] & CHIP_MODE_ALARM_bit) {
            lt_ret_t ret_unused = lt_l1_spi_csn_high(s2);
            UNUSED(ret_unused);  // We don't care about it, we return LT_L1_CHIP_ALARM_MODE anyway.
            return LT_L1_CHIP_ALARM_MODE;
        }

        // Check and save STARTUP bit of CHIP_STATUS to signalize whether device operates in bootloader or in
        // application
        if (s2->buff[0] & CHIP_MODE_STARTUP_bit) {
            s2->mode = LT_MODE_MAINTENANCE;
        }
        else {
            s2->mode = LT_MODE_APP;
        }

        // Proceed further in case CHIP_STATUS contains READY bit, signalizing that chip is ready to receive request
        if (s2->buff[0] & (CHIP_MODE_READY_bit)) {
            // receive STATUS byte and length byte
            ret = lt_l1_spi_transfer(s2, 1, 2, timeout_ms);
            if (ret != LT_OK) {  // offset 1
                lt_ret_t ret_unused = lt_l1_spi_csn_high(s2);
                UNUSED(ret_unused);  // We don't care about it, we return ret from SPI transfer anyway.
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
            if (length > (LT_L1_LEN_MAX - 2)) {
                lt_ret_t ret_unused = lt_l1_spi_csn_high(s2);
                UNUSED(ret_unused);  // We don't care about it, we return LT_L1_DATA_LEN_ERROR anyway.
                return LT_L1_DATA_LEN_ERROR;
            }
            // Receive the rest of incomming bytes, including crc
            ret = lt_l1_spi_transfer(s2, 3, length, timeout_ms);
            if (ret != LT_OK) {  // offset 3
                lt_ret_t ret_unused = lt_l1_spi_csn_high(s2);
                UNUSED(ret_unused);  // We don't care about it, we return ret from SPI transfer anyway.
                return ret;
            }
            ret = lt_l1_spi_csn_high(s2);
            if (ret != LT_OK) {
                return ret;
            }
#ifdef LT_PRINT_SPI_DATA
            print_hex_chunks(s2->buff, s2->buff[2] + 5, SPI_DIR_MISO);
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
            if (s2->mode == LT_MODE_MAINTENANCE) {
                // Chip is in bootloader mode and INT pin is not implemented in bootloader mode
                // So we wait a bit before we poll again for CHIP_STATUS
                // printf("x\n");
                ret = lt_l1_delay(s2, LT_L1_READ_RETRY_DELAY);
                if (ret != LT_OK) {
                    return ret;
                }
            }
            else {
                // We are in application. IF INT pin is enabled, wait for it to go low
#if LT_USE_INT_PIN
                ret = lt_l1_delay_on_int(h, LT_L1_TIMEOUT_MS_MAX);
                if (ret != LT_OK) {
                    return ret;
                }
#else
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
#ifdef LIBT_DEBUG
    if (!s2) {
        return LT_PARAM_ERR;
    }
    if ((timeout_ms < LT_L1_TIMEOUT_MS_MIN) | (timeout_ms > LT_L1_TIMEOUT_MS_MAX)) {
        return LT_PARAM_ERR;
    }
    if ((len < LT_L1_LEN_MIN) | (len > LT_L1_LEN_MAX)) {
        return LT_PARAM_ERR;
    }
#endif

    lt_ret_t ret;

    ret = lt_l1_spi_csn_low(s2);
    if (ret != LT_OK) {
        return ret;
    }
#ifdef LT_PRINT_SPI_DATA
    print_hex_chunks(s2->buff, len, SPI_DIR_MOSI);
#endif
    ret = lt_l1_spi_transfer(s2, 0, len, timeout_ms);
    if (ret != LT_OK) {
        lt_ret_t ret_unused = lt_l1_spi_csn_high(s2);
        UNUSED(ret_unused);  // We don't care about it, we return ret from SPI transfer anyway.
        return ret;
    }

    ret = lt_l1_spi_csn_high(s2);
    if (ret != LT_OK) {
        return ret;
    }

    return LT_OK;
}
