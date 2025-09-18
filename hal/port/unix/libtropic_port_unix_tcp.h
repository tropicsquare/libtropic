#ifndef LIBTROPIC_PORT_UNIX_TCP_H
#define LIBTROPIC_PORT_UNIX_TCP_H

/**
 * @file libtropic_port_unix_tcp.h
 * @author Tropic Square s.r.o.
 * @brief Port for communication with the TROPIC01 Model using TCP.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <netinet/in.h>

#include "libtropic_common.h"

#define LT_UNIX_TCP_TAG_AND_LENGTH_SIZE (sizeof(uint8_t) + sizeof(uint16_t))
#define LT_UNIX_TCP_MAX_PAYLOAD_LEN TR01_L2_MAX_FRAME_SIZE
#define LT_UNIX_TCP_MAX_BUFFER_LEN (LT_UNIX_TCP_TAG_AND_LENGTH_SIZE + LT_UNIX_TCP_MAX_PAYLOAD_LEN)

#define LT_UNIX_TCP_TX_ATTEMPTS 3
#define LT_UNIX_TCP_RX_ATTEMPTS 3
#define LT_UNIX_TCP_MAX_RECV_SIZE (LT_UNIX_TCP_MAX_PAYLOAD_LEN + LT_UNIX_TCP_TAG_AND_LENGTH_SIZE)

/** @brief Possible values for `tag` field of `lt_unix_tcp_buffer_t`. */
typedef enum lt_unix_tcp_tag_t {
    LT_UNIX_TCP_TAG_SPI_DRIVE_CSN_LOW = 0x01,
    LT_UNIX_TCP_TAG_SPI_DRIVE_CSN_HIGH = 0x02,
    LT_UNIX_TCP_TAG_SPI_SEND = 0x03,
    LT_UNIX_TCP_TAG_POWER_ON = 0x04,
    LT_UNIX_TCP_TAG_POWER_OFF = 0x05,
    LT_UNIX_TCP_TAG_WAIT = 0x06,
    LT_UNIX_TCP_TAG_RESET_TARGET = 0x10,
    LT_UNIX_TCP_TAG_INVALID = 0xfd,
    LT_UNIX_TCP_TAG_UNSUPPORTED = 0xfe,
} lt_unix_tcp_tag_t;

/** @brief Structure for RX and TX buffers. */
typedef struct lt_unix_tcp_buffer_t {
    union {
        uint8_t buff[LT_UNIX_TCP_MAX_BUFFER_LEN];
        struct {
            uint8_t tag;
            uint16_t len;
            uint8_t payload[LT_UNIX_TCP_MAX_PAYLOAD_LEN];
        } __attribute__((packed, aligned));
    };
} lt_unix_tcp_buffer_t;

/**
 * @brief Device structure for Unix model port (TCP communication).
 *
 * @note Public members are meant to be configured by the developer before passing the handle to
 *       libtropic.
 */
typedef struct lt_dev_unix_tcp_t {
    /** @public @brief Address of the model server. */
    in_addr_t addr;
    /** @public @brief Port of the model server. */
    in_port_t port;
    /** @public @brief Seed for the platform's random number generator. */
    unsigned int rng_seed;

    /** @private @brief Socket file descriptor. */
    int socket_fd;
    /** @private @brief Reception buffer. */
    struct lt_unix_tcp_buffer_t rx_buffer;
    /** @private @brief Emission buffer. */
    struct lt_unix_tcp_buffer_t tx_buffer;
} lt_dev_unix_tcp_t;

#endif  // LIBTROPIC_PORT_UNIX_TCP_H