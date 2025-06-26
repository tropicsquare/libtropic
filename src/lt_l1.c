/**
 * @file lt_l1.c
 * @brief Layer 1 functions definitions
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */
#include "lt_l1.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "libtropic_common.h"
#include "lt_l1_port_wrap.h"

#ifdef LT_PRINT_SPI_DATA
#include "stdio.h"
#define SPI_DIR_MISO 0
#define SPI_DIR_MOSI 1
void print_hex_chunks(const uint8_t *data, uint8_t len, uint8_t dir) {
    if ((!data) || (len == 0)) {
        return;
    }
    printf("%s", dir ?  "  >>  TX: " : "  <<  RX: ");
    for (size_t i = 0; i < len; i++) {
        printf("%02X ", data[i]);
        if ((i + 1) % 32 == 0) {
            printf("\n          ");
        }
    }
    printf("\n");
}
#endif

lt_ret_t lt_l1_read(lt_l2_state_t *s2, const uint32_t max_len, const uint32_t timeout)
{
#ifdef LIBT_DEBUG
    if (!s2) {
        return LT_PARAM_ERR;
    }
    if ((timeout < LT_L1_TIMEOUT_MS_MIN) | (timeout > LT_L1_TIMEOUT_MS_MAX)) {
        return LT_PARAM_ERR;
    }
    if ((max_len < LT_L1_LEN_MIN) | (max_len > LT_L1_LEN_MAX)) {
        return LT_PARAM_ERR;
    }
#endif

    int max_tries = LT_L1_READ_MAX_TRIES;

    while (max_tries > 0) {
        max_tries--;

        s2->buff[0] = GET_RESPONSE_REQ_ID;

        // Try to read CHIP_STATUS byte
        lt_l1_spi_csn_low(s2);

        if (lt_l1_spi_transfer(s2, 0, 1, timeout) != LT_OK) {
            lt_l1_spi_csn_high(s2);
            return LT_L1_SPI_ERROR;
        }

        // Check ALARM bit of CHIP_STATUS byte
        if (s2->buff[0] & CHIP_MODE_ALARM_bit) {
            lt_l1_spi_csn_high(s2);
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

        // TODO comment
        if (s2->buff[0] & (CHIP_MODE_READY_bit)) {
            // receive STATUS byte and length byte
            if (lt_l1_spi_transfer(s2, 1, 2, timeout) != LT_OK) {  // offset 1
                lt_l1_spi_csn_high(s2);
                return LT_L1_SPI_ERROR;
            }

            // TODO Better to read two bytes, not one. Then length.
            // This function as it is now will return chip in alarm mode when
            // spi wires are not connected (and therefore reading 0xff), but I want to get chip busy instead.
            // Hotfix for fpga no resp handling:
            if (s2->buff[1] == 0xff) {
                lt_l1_spi_csn_high(s2);
                lt_l1_delay(s2, LT_L1_READ_RETRY_DELAY);
                continue;
            }

            // Take length information and add 2B for crc bytes
            uint16_t length = s2->buff[2] + 2;
            if (length > (LT_L1_LEN_MAX - 2)) {
                lt_l1_spi_csn_high(s2);
                return LT_L1_DATA_LEN_ERROR;
            }
            // Receive the rest of incomming bytes, including crc
            if (lt_l1_spi_transfer(s2, 3, length, timeout) != LT_OK) {  // offset 3
                lt_l1_spi_csn_high(s2);
                return LT_L1_SPI_ERROR;
            }
            lt_l1_spi_csn_high(s2);

            print_hex_chunks(s2->buff, s2->buff[2] + 4, SPI_DIR_MISO);

            return LT_OK;

            // Chip status does not contain any special mode bit and also is not ready,
            // try it again (until max_tries runs out)
        }
        else {
            lt_l1_spi_csn_high(s2);
            if (s2->mode == LT_MODE_MAINTENANCE) {
                // Chip is in bootloader mode and INT pin is not implemented in bootloader mode
                // So we wait a bit before we poll again for CHIP_STATUS
                // printf("x\n");
                lt_l1_delay(s2, LT_L1_READ_RETRY_DELAY);
            }
            else {
                // We are in application. IF INT pin is enabled, wait for it to go low
#if LT_USE_INT_PIN
                if (lt_l1_delay_on_int(h, LT_L1_TIMEOUT_MS_MAX) != LT_OK) {
                    return LT_L1_INT_TIMEOUT;
                }
#else
                lt_l1_delay(s2, LT_L1_READ_RETRY_DELAY);
#endif
            }
        }
    }

    return LT_L1_CHIP_BUSY;
}

lt_ret_t lt_l1_write(lt_l2_state_t *s2, const uint16_t len, const uint32_t timeout)
{
#ifdef LIBT_DEBUG
    if (!s2) {
        return LT_PARAM_ERR;
    }
    if ((timeout < LT_L1_TIMEOUT_MS_MIN) | (timeout > LT_L1_TIMEOUT_MS_MAX)) {
        return LT_PARAM_ERR;
    }
    if ((len < LT_L1_LEN_MIN) | (len > LT_L1_LEN_MAX)) {
        return LT_PARAM_ERR;
    }
#endif

    lt_l1_spi_csn_low(s2);

    print_hex_chunks(s2->buff, len, SPI_DIR_MOSI);

    if (lt_l1_spi_transfer(s2, 0, len, timeout) != LT_OK) {
        lt_l1_spi_csn_high(s2);
        return LT_L1_SPI_ERROR;
    }

    lt_l1_spi_csn_high(s2);

    return LT_OK;
}
