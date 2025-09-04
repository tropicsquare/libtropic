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

#define TCP_TAG_AND_LENGTH_SIZE (sizeof(uint8_t) + sizeof(uint16_t))
#define MAX_PAYLOAD_LEN L2_MAX_FRAME_SIZE
#define MAX_BUFFER_LEN (TCP_TAG_AND_LENGTH_SIZE + MAX_PAYLOAD_LEN)

#define TX_ATTEMPTS 3
#define RX_ATTEMPTS 3
#define MAX_RECV_SIZE (MAX_PAYLOAD_LEN + TCP_TAG_AND_LENGTH_SIZE)

/** @brief Possible values for `tag` field of `unix_tcp_buffer_t`. */
typedef enum unix_tcp_tag_t {
    TAG_E_SPI_DRIVE_CSN_LOW = 0x01,
    TAG_E_SPI_DRIVE_CSN_HIGH = 0x02,
    TAG_E_SPI_SEND = 0x03,
    TAG_E_POWER_ON = 0x04,
    TAG_E_POWER_OFF = 0x05,
    TAG_E_WAIT = 0x06,
    TAG_E_RESET_TARGET = 0x10,
    TAG_E_INVALID = 0xfd,
    TAG_E_UNSUPPORTED = 0xfe,
} unix_tcp_tag_t;

/** @brief Structure for RX and TX buffers. */
typedef struct unix_tcp_buffer_t {
    union {
        uint8_t buff[MAX_BUFFER_LEN];
        struct {
            uint8_t tag;
            uint16_t len;
            uint8_t payload[MAX_PAYLOAD_LEN];
        } __attribute__((packed, aligned));
    };
} unix_tcp_buffer_t;

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
    struct unix_tcp_buffer_t rx_buffer;
    /** @private @brief Emission buffer. */
    struct unix_tcp_buffer_t tx_buffer;
} lt_dev_unix_tcp_t;

#endif  // LIBTROPIC_PORT_UNIX_TCP_H