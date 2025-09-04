/**
 * @file libtropic_port_unix_tcp.c
 * @author Tropic Square s.r.o.
 * @brief Port for communication with the TROPIC01 Model using TCP.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic_port_unix_tcp.h"

#include <arpa/inet.h>
#include <errno.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "libtropic_common.h"
#include "libtropic_logging.h"
#include "libtropic_macros.h"
#include "libtropic_port.h"

static lt_ret_t connect_to_server(lt_dev_unix_tcp_t *dev)
{
    struct sockaddr_in server;

    // Create socket
    dev->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (dev->socket_fd < 0) {
        LT_LOG_ERROR("Could not create socket: %s (%d).", strerror(errno), errno);
        return LT_FAIL;
    }
    LT_LOG_DEBUG("Socket created.");

    // Server information
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = dev->addr;
    server.sin_port = htons(dev->port);

    // Connect to the server
    LT_LOG_DEBUG("Connecting to %s:%d.", inet_ntoa(server.sin_addr), dev->port);
    if (connect(dev->socket_fd, (struct sockaddr *)(&server), sizeof(server)) < 0) {
        LT_LOG_ERROR("Could not connect: %s (%d).", strerror(errno), errno);
        return LT_FAIL;
    }
    LT_LOG_DEBUG("Connected to the server.");

    return LT_OK;
}

static lt_ret_t send_all(int socket, uint8_t *buffer, size_t length)
{
    int nb_bytes_sent;
    int nb_bytes_sent_total = 0;
    int nb_bytes_to_send = length;
    uint8_t *ptr = buffer;

    // attempt several times to send the data
    for (int i = 0; i < TX_ATTEMPTS; i++) {
        LT_LOG_DEBUG("Attempting to send data: attempt #%d.", i);
        nb_bytes_sent = send(socket, ptr, nb_bytes_to_send, 0);
        if (nb_bytes_sent <= 0) {
            LT_LOG_ERROR("Send failed: %s (%d).", strerror(errno), errno);
            return LT_FAIL;
        }

        nb_bytes_to_send -= nb_bytes_sent;
        if (nb_bytes_to_send == 0) {
            LT_LOG_DEBUG("All %zu bytes sent successfully.", length);
            return LT_OK;
        }

        ptr += nb_bytes_sent;
        nb_bytes_sent_total += nb_bytes_sent;
    }

    // could not send all the data after several attempts
    LT_LOG_ERROR("%d bytes sent instead of expected %zu ", nb_bytes_sent_total, length);
    LT_LOG_ERROR("after %d attempts.", TX_ATTEMPTS);

    return LT_FAIL;
}

static lt_ret_t communicate(lt_dev_unix_tcp_t *dev, int *tx_payload_length_ptr, int *rx_payload_length_ptr)
{
    lt_ret_t ret;
    int nb_bytes_received;
    int nb_bytes_received_total = 0;
    int nb_bytes_to_receive = TCP_TAG_AND_LENGTH_SIZE;
    uint8_t *rx_ptr = dev->rx_buffer.buff;
    // number of bytes to send
    int nb_bytes_to_send = TCP_TAG_AND_LENGTH_SIZE;

    if (tx_payload_length_ptr != NULL) {
        nb_bytes_to_send += *tx_payload_length_ptr;
    }

    // update payload length field
    dev->tx_buffer.len = nb_bytes_to_send - TCP_TAG_AND_LENGTH_SIZE;

    ret = send_all(dev->socket_fd, dev->tx_buffer.buff, nb_bytes_to_send);
    if (ret != LT_OK) {
        return ret;
    }

    // receive data
    LT_LOG_DEBUG("- Receiving data from target.");
    nb_bytes_received = recv(dev->socket_fd, dev->rx_buffer.buff, MAX_RECV_SIZE, 0);

    if (nb_bytes_received < 0) {
        LT_LOG_ERROR("Receive failed: %s (%d).", strerror(errno), errno);
        return LT_FAIL;
    }
    else if (nb_bytes_received < nb_bytes_to_receive) {
        LT_LOG_ERROR("At least %d bytes are expected: %d.", nb_bytes_to_receive, nb_bytes_received);
        return LT_FAIL;
    }

    LT_LOG_DEBUG("Length field: %" PRIu16 ".", dev->rx_buffer.len);
    nb_bytes_to_receive += dev->rx_buffer.len;
    nb_bytes_received_total += nb_bytes_received;
    LT_LOG_DEBUG("Received %d bytes out of %d expected.", nb_bytes_received_total, nb_bytes_to_receive);

    if (nb_bytes_received_total < nb_bytes_to_receive) {
        rx_ptr += nb_bytes_received;

        for (int i = 0; i < RX_ATTEMPTS; i++) {
            LT_LOG_DEBUG("Attempting to receive remaining bytes: attempt #%d.", i);
            nb_bytes_received = recv(dev->socket_fd, dev->rx_buffer.buff, MAX_RECV_SIZE, 0);

            if (nb_bytes_received < 0) {
                LT_LOG_ERROR("Receive failed: %s (%d).", strerror(errno), errno);
                return LT_FAIL;
            }

            nb_bytes_received_total += nb_bytes_received;
            if (nb_bytes_received_total >= nb_bytes_to_receive) {
                LT_LOG_DEBUG("Received %d bytes in total.", nb_bytes_received_total);
                break;
            }
            rx_ptr += nb_bytes_received;
        }
    }

    if (nb_bytes_received_total != nb_bytes_to_receive) {
        LT_LOG_ERROR("Received %d bytes in total instead of %d.", nb_bytes_received_total, nb_bytes_to_receive);
        return LT_FAIL;
    }

    // server does not know the sent tag
    if ((unix_tcp_tag_t)dev->rx_buffer.tag == TAG_E_INVALID) {
        LT_LOG_ERROR("Tag %" PRIu8 " is not known by the server.", dev->tx_buffer.tag);
        return LT_FAIL;
    }
    // server does not know what to do with the sent tag
    else if ((unix_tcp_tag_t)dev->rx_buffer.tag == TAG_E_UNSUPPORTED) {
        LT_LOG_ERROR("Tag %" PRIu8 " is not supported by the server.", dev->tx_buffer.tag);
        return LT_FAIL;
    }
    // RX tag and TX tag should be identical
    else if (dev->rx_buffer.tag != dev->tx_buffer.tag) {
        LT_LOG_ERROR("Expected tag %" PRIu8 ", received %" PRIu8 ".", dev->rx_buffer.tag, dev->tx_buffer.tag);
        return LT_FAIL;
    }

    LT_LOG_DEBUG("Rx tag and tx tag match: %" PRIu8 ".", dev->rx_buffer.tag);
    if (rx_payload_length_ptr != NULL) {
        *rx_payload_length_ptr = nb_bytes_received_total - TCP_TAG_AND_LENGTH_SIZE;
    }

    return LT_OK;
}

static lt_ret_t server_connect(lt_dev_unix_tcp_t *dev)
{
    bzero(dev->tx_buffer.buff, MAX_BUFFER_LEN);
    bzero(dev->rx_buffer.buff, MAX_BUFFER_LEN);

    lt_ret_t ret = connect_to_server(dev);
    if (ret != LT_OK) {
        return ret;
    }

    return LT_OK;
}

static lt_ret_t server_disconnect(int socket_fd)
{
    LT_LOG_DEBUG("-- Server disconnect");
    if (close(socket_fd)) {
        LT_LOG_ERROR("close() failed: %s (%d)", strerror(errno), errno);
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_port_init(lt_l2_state_t *s2)
{
    lt_dev_unix_tcp_t *dev = (lt_dev_unix_tcp_t *)(s2->device);

    lt_ret_t ret = server_connect(dev);
    if (ret != LT_OK) {
        return ret;
    }

    srand(dev->rng_seed);

    return LT_OK;
}

lt_ret_t lt_port_deinit(lt_l2_state_t *s2)
{
    lt_dev_unix_tcp_t *dev = (lt_dev_unix_tcp_t *)(s2->device);
    lt_ret_t ret = server_disconnect(dev->socket_fd);
    if (ret != LT_OK) {
        return ret;
    }

    return LT_OK;
}

lt_ret_t lt_port_spi_csn_low(lt_l2_state_t *s2)
{
    lt_dev_unix_tcp_t *dev = (lt_dev_unix_tcp_t *)(s2->device);
    LT_LOG_DEBUG("-- Driving Chip Select to Low.");
    dev->tx_buffer.tag = TAG_E_SPI_DRIVE_CSN_LOW;
    return communicate(dev, NULL, NULL);
}

lt_ret_t lt_port_spi_csn_high(lt_l2_state_t *s2)
{
    lt_dev_unix_tcp_t *dev = (lt_dev_unix_tcp_t *)(s2->device);
    LT_LOG_DEBUG("-- Driving Chip Select to High.");
    dev->tx_buffer.tag = TAG_E_SPI_DRIVE_CSN_HIGH;
    return communicate(dev, NULL, NULL);
}

lt_ret_t lt_port_spi_transfer(lt_l2_state_t *s2, uint8_t offset, uint16_t tx_data_length, uint32_t timeout_ms)
{
    UNUSED(timeout_ms);
    lt_dev_unix_tcp_t *dev = (lt_dev_unix_tcp_t *)(s2->device);
    lt_ret_t ret;

    if (offset + tx_data_length > LT_L1_LEN_MAX) {
        return LT_L1_DATA_LEN_ERROR;
    }

    LT_LOG_DEBUG("-- Sending data through SPI bus.");

    int tx_payload_length = tx_data_length;
    int rx_payload_length;

    dev->tx_buffer.tag = TAG_E_SPI_SEND;
    dev->tx_buffer.len = (uint16_t)tx_payload_length;

    // copy tx_data to tx payload
    memcpy(&dev->tx_buffer.payload, s2->buff, tx_payload_length);

    ret = communicate(dev, &tx_payload_length, &rx_payload_length);
    if (ret != LT_OK) {
        return LT_FAIL;
    }

    memcpy(s2->buff + offset, &dev->rx_buffer.payload, rx_payload_length);

    return LT_OK;
}

lt_ret_t lt_port_delay(lt_l2_state_t *s2, uint32_t ms)
{
    lt_dev_unix_tcp_t *dev = (lt_dev_unix_tcp_t *)(s2->device);
    LT_LOG_DEBUG("-- Waiting for the target.");

    dev->tx_buffer.tag = TAG_E_WAIT;
    int payload_length = sizeof(uint32_t);
    //*(uint32_t *)(&tx_buffer.PAYLOAD) = wait_time_usecs;
    dev->rx_buffer.payload[0] = ms & 0x000000ff;
    dev->rx_buffer.payload[1] = (ms & 0x0000ff00) >> 8;
    dev->rx_buffer.payload[2] = (ms & 0x00ff0000) >> 16;
    dev->rx_buffer.payload[3] = (ms & 0xff000000) >> 24;

    return communicate(dev, &payload_length, NULL);
}

lt_ret_t lt_port_random_bytes(lt_l2_state_t *s2, void *buff, size_t count)
{
    UNUSED(s2);

    uint8_t *buff_ptr = buff;
    for (size_t i = 0; i < count; i++) {
        // Number from rand() is guaranteed to have at least 15 bits valid
        buff_ptr[i] = (uint8_t)(rand() & 0xFF);
    }

    return LT_OK;
}
