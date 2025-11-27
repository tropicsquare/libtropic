/**
 * @file libtropic_port_mock.h
 * @brief Test-only mock HAL.
 */
#ifndef LIBTROPIC_PORT_MOCK_H
#define LIBTROPIC_PORT_MOCK_H

#include "libtropic_port.h"

#ifdef __cplusplus
extern "C" {
#endif

// This can be either L2 Response frame or a single CHIP_STATUS byte (in a case of writing REQ_ID=0xAA).
// Basically, anything that can be returned by the chip during a period between CSN low and CSN high on the MISO line.
typedef struct mock_miso_data_t {
    size_t len;
    uint8_t data[TR01_L1_LEN_MAX];
} mock_miso_data_t;

/* Device structure ----------------------------------------------------- */
/* Simple single-frame queue */
/* Simple fixed-size queue of frames to return in FIFO order. */
#define MOCK_QUEUE_DEPTH 16

typedef struct lt_dev_mock_t {
    mock_miso_data_t mock_queue[MOCK_QUEUE_DEPTH];
    size_t mock_queue_head; /* index of next element to pop */
    size_t mock_queue_tail; /* index where to push next element */
    size_t mock_queue_count;

    bool   frame_in_progress;
    size_t frame_bytes_transferred;
} lt_dev_mock_t;

/* Test control API ----------------------------------------------------- */

/**
 * @brief Reset internal mock state (clear queued responses and flags).
 */
lt_ret_t lt_mock_hal_reset(lt_l2_state_t *s2);

/**
 * @brief Enqueue a raw response that the mock will return on the next SPI transfer.
 *
 * The mock copies `len` bytes from `data` into its internal queue. `len` must be <= TR01_L1_LEN_MAX.
 *
 * @param data Pointer to response bytes.
 * @param len Length of response bytes.
 * @return LT_OK on success, LT_FAIL otherwise.
 */
lt_ret_t lt_mock_hal_enqueue_response(lt_l2_state_t *s2, const uint8_t *data, size_t len);

/* The mock also implements the platform port entrypoints declared in `libtropic_port.h`. */

#ifdef __cplusplus
}
#endif

#endif /* LIBTROPIC_PORT_MOCK_H */
