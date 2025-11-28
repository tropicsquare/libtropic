#ifndef LIBTROPIC_PORT_MOCK_H
#define LIBTROPIC_PORT_MOCK_H

/**
 * @file libtropic_port_mock.h
 * @brief Mock HAL implementation (only for testing purposes).
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic_port.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Mocked MISO data structure.
 *
 * @details This can be either L2 Response frame or a single CHIP_STATUS byte (in a case of writing REQ_ID=0xAA).
 * Basically, anything that can be returned by the chip during a period between CSN low and CSN high on the MISO line.
 */
typedef struct mock_miso_data_t {
    size_t len;
    uint8_t data[TR01_L1_LEN_MAX];
} mock_miso_data_t;

/// @brief Depth of the mock response queue.
#define MOCK_QUEUE_DEPTH 16

/**
 * @brief Device structure for Mock HAL port.
 */
typedef struct lt_dev_mock_t {
    /** @private @brief Queue of mocked data. */
    mock_miso_data_t mock_queue[MOCK_QUEUE_DEPTH];
    /** @private @brief Index of next element to pop. */
    size_t mock_queue_head;
    /** @private @brief Index where to push next element. */
    size_t mock_queue_tail;
    /** @private @brief Number of elements currently in the queue. */
    size_t mock_queue_count;

    /** @private @brief Flag indicating if a frame is currently in progress (CSN low). */
    bool frame_in_progress;
    /** @private @brief Number of bytes transferred in the current frame so far. */
    size_t frame_bytes_transferred;
} lt_dev_mock_t;

// Test control API -----------------------------------------------------

/**
 * @brief Reset internal mock state (clear queued responses and flags).
 *
 * @warning This has to be called by the test developer before starting a new test case. It is not called automatically
 * by HAL, as mocked reponses may be needed before/after initializing Libtropic.
 */
lt_ret_t lt_mock_hal_reset(lt_l2_state_t *s2);

/**
 * @brief Enqueue a raw response that the mock will return on the next SPI transfer.
 *
 * @details The mock copies `len` bytes from `data` into its internal queue. `len` must be <= TR01_L1_LEN_MAX.
 *
 * @param data Pointer to response bytes.
 * @param len Length of response bytes.
 * @return LT_OK on success, LT_FAIL otherwise.
 */
lt_ret_t lt_mock_hal_enqueue_response(lt_l2_state_t *s2, const uint8_t *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif  // LIBTROPIC_PORT_MOCK_H
