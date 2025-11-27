/**
 * @file libtropic_port_mock.c
 * @brief Mock HAL implementation.
 *
 * This file provides a mock implementation of the platform port API
 * used by Libtropic. It is intended for tests only.
 */

#include "libtropic_port_mock.h"

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <inttypes.h>

#include "libtropic_common.h"
#include "libtropic_logging.h"
#include "lt_l1.h"

/* Mock test control API ----------------------------------------------------- */

lt_ret_t lt_mock_hal_reset(lt_l2_state_t *s2)
{
    if (!s2) {
        return LT_FAIL;
    }

    lt_dev_mock_t *dev = (lt_dev_mock_t *)s2->device;

    dev->mock_queue_head = 0;
    dev->mock_queue_tail = 0;
    dev->mock_queue_count = 0;

    dev->frame_in_progress = false;
    dev->frame_bytes_transferred = 0;
}

lt_ret_t lt_mock_hal_enqueue_response(lt_l2_state_t *s2, const uint8_t *data, size_t len)
{  
    if (!s2 || !data || len == 0 || len > TR01_L1_LEN_MAX) {
        return LT_FAIL;
    }

    lt_dev_mock_t *dev = (lt_dev_mock_t *)s2->device;

    if (dev->mock_queue_count >= MOCK_QUEUE_DEPTH) {
        LT_LOG_ERROR("Mock HAL: response queue full, cannot enqueue more responses!");
        return LT_FAIL;
    }

    if (len > TR01_L1_LEN_MAX) {
        LT_LOG_ERROR("Mock HAL: response too long to enqueue!");
        return LT_FAIL;
    }

    // Copy provided data into next slot.
    mock_miso_data_t *r = &dev->mock_queue[dev->mock_queue_tail];
    // Fill with zeroes first, as transfer may exceed data queued by user (typically on writing).
    memset(r->data, 0, TR01_L1_LEN_MAX);
    memcpy(r->data, data, len);
    r->len = len;

    // Advance tail.
    dev->mock_queue_tail = (dev->mock_queue_tail + 1) % MOCK_QUEUE_DEPTH;
    dev->mock_queue_count++;

    return LT_OK;
}

/* Platform API implementation ------------------------------------------------ */

lt_ret_t lt_port_init(lt_l2_state_t *s2)
{
    LT_UNUSED(s2);
    // Reset cannot be performed here, as mocked data has to be enqueued before init
    // (as we transfer data during init to get FW version).
    return LT_OK;
}

lt_ret_t lt_port_deinit(lt_l2_state_t *s2)
{
    LT_UNUSED(s2);
    return LT_OK;
}

lt_ret_t lt_port_spi_csn_low(lt_l2_state_t *s2)
{
    lt_dev_mock_t *dev = (lt_dev_mock_t *)(s2->device);

    if (dev->frame_in_progress) {
        LT_LOG_ERROR("Mock HAL: SPI CSN Low called while frame already in progress!");
        LT_ASSERT(0, 1); // Force test failure.
    }

    dev->frame_in_progress = true;
    dev->frame_bytes_transferred = 0;

    return LT_OK;
}

lt_ret_t lt_port_spi_csn_high(lt_l2_state_t *s2)
{
    lt_dev_mock_t *dev = (lt_dev_mock_t *)(s2->device);

    if (!dev->frame_in_progress) {
        LT_LOG_ERROR("Mock HAL: SPI CSN High called while no frame in progress!");
        LT_ASSERT(0, 1); // Force test failure
    }

    // End of transaction (frame), pop the response.
    if (dev->mock_queue_count == 0) {
        // This could happen only if no reponse was enqueued and Libtropic
        // sets CSN low and high without any SPI transfer in between (implementation mistake).
        LT_LOG_ERROR("Mock HAL: no response queued at the end of transaction!");
        LT_ASSERT(0, 1); // Force test failure
    }

    dev->mock_queue_head = (dev->mock_queue_head + 1) % MOCK_QUEUE_DEPTH;
    dev->mock_queue_count--;
    dev->frame_in_progress = false;

    return LT_OK;
}

lt_ret_t lt_port_spi_transfer(lt_l2_state_t *s2, uint8_t offset, uint16_t tx_len, uint32_t timeout_ms)
{
    (void)timeout_ms;
    if (!s2) {
        return LT_PARAM_ERR;
    }

    lt_dev_mock_t *dev = (lt_dev_mock_t *)(s2->device);

    if (!dev->frame_in_progress) {
        LT_LOG_ERROR("Mock HAL: SPI Transfer called while no frame in progress!");
        LT_ASSERT(0, 1); // Force test failure
    }

    if (dev->mock_queue_count == 0) {
        LT_LOG_ERROR("Mock HAL: no response queued!");
        LT_ASSERT(0, 1); // Force test failure
    }

    // Peek next response.
    mock_miso_data_t *r = &dev->mock_queue[dev->mock_queue_head];

    // Offset to the internal buffer + tx_len must not exceed the buffer size.
    if (tx_len + offset > TR01_L1_LEN_MAX) {
        LT_LOG_ERROR("Mock HAL: SPI Transfer exceeds L1 buffer size!");
        return LT_L1_DATA_LEN_ERROR;
    }

    // If reading more bytes than available in the mocked response, log. Normally, this is OK:
    // this happens when writing, as on the MOSI there is whole L2 Request and on MISO there is just CHIP_STATUS byte.
    // During reading, this should not happen, as the lt_l1_read always reads up to the length of the response frame. It can
    // happen only if the queued frame is invalid (shorter than minimum, not containing length byte or CRC etc).
    if (tx_len > r->len - dev->frame_bytes_transferred) {
        LT_LOG_DEBUG("Mock HAL: SPI Transfer length exceeds mocked response length.");
    }

    memcpy(s2->buff + offset, r->data + dev->frame_bytes_transferred, tx_len);
    dev->frame_bytes_transferred += tx_len;

    LT_LOG_DEBUG("Mock HAL queue position: head=%zu, tail=%zu, count=%zu", dev->mock_queue_head, dev->mock_queue_tail, dev->mock_queue_count);
    for(size_t i = 0; i < tx_len; i++) {
        LT_LOG_DEBUG("Mock HAL: SPI Transfer: buff[%d] = 0x%02" PRIX8, offset + i, s2->buff[offset + i]);
    }

    return LT_OK;
}

lt_ret_t lt_port_delay(lt_l2_state_t *s2, uint32_t ms)
{
    LT_UNUSED(s2);

    if (ms == 0) {
        return LT_OK;
    }
    
    usleep((useconds_t)ms * 1000U);
    
    return LT_OK;
}

lt_ret_t lt_port_random_bytes(lt_l2_state_t *s2, void *buff, size_t count)
{
    LT_UNUSED(s2);
    if (!buff) {
        return LT_PARAM_ERR;
    }
    uint8_t *ptr = (uint8_t *)buff;
    for (size_t i = 0; i < count; i++) {
        ptr[i] = (uint8_t)(rand() & 0xFF);
    }
    return LT_OK;
}
