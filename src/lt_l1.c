/**
 * @file lt_l1.c
 * @brief Layer 1 functions definitions
 * @copyright Copyright (c) 2020-2026 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */
#include "lt_l1.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "libtropic_common.h"
#include "libtropic_logging.h"
#include "libtropic_macros.h"
#include "lt_port_wrap.h"

#ifdef LT_PRINT_SPI_DATA
#include "libtropic_port.h"
enum lt_spi_dir_t { LT_L1_SPI_DIR_MISO, LT_L1_SPI_DIR_MOSI };
static void print_spi_data_hex(const uint8_t *data, const size_t len, const enum lt_spi_dir_t dir)
{
    if ((!data) || (len == 0)) {
        LT_LOG_ERROR("Can't print SPI data, invalid argument(s)!");
        return;
    }

    lt_port_log("SPI     %s", dir == LT_L1_SPI_DIR_MOSI ? ">> TX  " : "<< RX  ");
    for (size_t i = 0; i < len; i++) {
        lt_port_log("%02" PRIX8 " ", data[i]);
        if ((i + 1) % 32 == 0) {
            lt_port_log("\n               ");
        }
    }

    lt_port_log("\n");
}
#endif

lt_ret_t lt_l1_read(lt_l2_state_t *s2, const uint32_t max_len, const uint32_t timeout_ms)
{
#ifdef LT_REDUNDANT_ARG_CHECK
    if (!s2) {
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
            LT_LOG_DEBUG("CHIP_STATUS: 0x%02" PRIX8, s2->buff[0]);

#ifdef LT_RETRIEVE_ALARM_LOG
            ret_unused = lt_l1_retrieve_alarm_log(s2, timeout_ms);
#endif

            LT_UNUSED(ret_unused);  // We don't care about it, we return LT_L1_CHIP_ALARM_MODE anyway.
            return LT_L1_CHIP_ALARM_MODE;
        }

        // Proceed further in case CHIP_STATUS contains READY bit, signalizing that chip is ready to
        // receive request
        if (s2->buff[0] & TR01_L1_CHIP_MODE_READY_bit) {
            // receive STATUS byte and length byte
            ret = lt_l1_spi_transfer(s2, 1, 2, timeout_ms);
            if (ret != LT_OK) {  // offset 1
                lt_ret_t ret_unused = lt_l1_spi_csn_high(s2);
                LT_UNUSED(
                    ret_unused);  // We don't care about it, we return ret from SPI transfer anyway.
                return ret;
            }

            // 0xFF received in second byte means that chip has no response to send.
            if (s2->buff[1] == 0xff) {
                ret = lt_l1_spi_csn_high(s2);
                if (ret != LT_OK) {
                    return ret;
                }
                ret = lt_l1_delay(s2, LT_L1_READ_RETRY_DELAY_MS);
                if (ret != LT_OK) {
                    return ret;
                }
                continue;
            }

            // Take length information and add length of CRC
            uint16_t length = s2->buff[2] + TR01_L2_REQ_RSP_CRC_SIZE;
            // First 3 bytes were already read, so the maximum valid read length is MAX_DATA + CRC
            if (length > (TR01_L2_CHUNK_MAX_DATA_SIZE + TR01_L2_REQ_RSP_CRC_SIZE)) {
                lt_ret_t ret_unused = lt_l1_spi_csn_high(s2);
                LT_UNUSED(
                    ret_unused);  // We don't care about it, we return LT_L1_DATA_LEN_ERROR anyway.
                return LT_L1_DATA_LEN_ERROR;
            }
            // Receive the rest of incomming bytes, including crc
            ret = lt_l1_spi_transfer(s2, 3, length, timeout_ms);
            if (ret != LT_OK) {  // offset 3
                lt_ret_t ret_unused = lt_l1_spi_csn_high(s2);
                LT_UNUSED(
                    ret_unused);  // We don't care about it, we return ret from SPI transfer anyway.
                return ret;
            }
            ret = lt_l1_spi_csn_high(s2);
            if (ret != LT_OK) {
                return ret;
            }
#ifdef LT_PRINT_SPI_DATA
            print_spi_data_hex(s2->buff, s2->buff[2] + 5, LT_L1_SPI_DIR_MISO);
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
                ret = lt_l1_delay(s2, LT_L1_READ_RETRY_DELAY_MS);
                if (ret != LT_OK) {
                    return ret;
                }
            }
            else {
#if LT_USE_INT_PIN
                // Wait for rising edge on the INT pin, which signalizes that L2 Response frame is
                // ready to be received
                ret = lt_l1_delay_on_int(s2, LT_L1_INT_TIMEOUT_MS);
                if (ret != LT_OK) {
                    return ret;
                }
#else
                // INT pin not used, delay for some time
                ret = lt_l1_delay(s2, LT_L1_READ_RETRY_DELAY_MS);
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
    print_spi_data_hex(s2->buff, len, LT_L1_SPI_DIR_MOSI);
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

lt_ret_t lt_l1_retrieve_alarm_log(lt_l2_state_t *s2, const uint32_t timeout_ms)
{
    LT_LOG_DEBUG("Retrieving alarm log from TROPIC01...");

    // Transfer full L2 frame to get the alarm log

    memset(s2->buff, 0, sizeof(s2->buff));
    s2->buff[0] = TR01_L1_GET_RESPONSE_REQ_ID;

    lt_ret_t ret = lt_l1_spi_csn_low(s2);
    if (ret != LT_OK) {
        LT_LOG_ERROR("Failed to set CSN low while retrieving alarm log.");
        return ret;
    }

    ret = lt_l1_spi_transfer(s2, 0, TR01_L2_MAX_FRAME_SIZE, timeout_ms);
    if (ret != LT_OK) {
        lt_ret_t ret_unused = lt_l1_spi_csn_high(s2);
        LT_UNUSED(ret_unused);  // We don't care about it, we return ret from SPI transfer.
        LT_LOG_ERROR("Failed to transfer SPI data while retrieving alarm log.");
        return ret;
    }

    ret = lt_l1_spi_csn_high(s2);
    if (ret != LT_OK) {
        LT_LOG_ERROR("Failed to set CSN high after retrieving alarm log.");
        return ret;
    }

    // Decode and print the alarm log

    uint8_t log_size = lt_min(s2->buff[TR01_L2_RSP_LEN_OFFSET], TR01_L2_CHUNK_MAX_DATA_SIZE);
    LT_LOG_DEBUG("LOG SIZE: %" PRIu8, log_size);

    LT_LOG_DEBUG("------------ DECODED CPU Log BEGIN ------------");
    for (size_t i = 0; i < log_size;
         i++) {  // log_size is guaranteed to be <= TR01_L2_CHUNK_MAX_DATA_SIZE
        lt_port_log("%c", s2->buff[i + TR01_L2_RSP_DATA_RSP_CRC_OFFSET]);
    }
    lt_port_log("\n");
    LT_LOG_DEBUG("------------- DECODED CPU Log END -------------");

    LT_LOG_DEBUG("------------ RAW CPU Log BEGIN ------------");
    for (size_t i = 0; i < sizeof(s2->buff); i++) {  // Print whole L2 buffer.
        lt_port_log("0x%02x ", s2->buff[i]);
    }
    lt_port_log("\n");
    LT_LOG_DEBUG("------------- RAW CPU Log END -------------");

    return LT_OK;
}
