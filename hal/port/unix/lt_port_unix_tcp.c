/**
 * @file lt_port_unix.c
 * @author Tropic Square s.r.o.
 * @brief  C TCP client for communicating with the TROPIC01 Model
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */


#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "libtropic_common.h"
#include "libtropic_port.h"

#define TCP_ADDR "127.0.0.1"
#define TCP_PORT 28992

//#define NDEBUG
#ifdef NDEBUG
#    define LOG_OUT(f_, ...) printf("[TCP] "f_, ##__VA_ARGS__)
#    define LOG_ERR(f_, ...) fprintf(stderr, "ERROR: " f_, ##__VA_ARGS__)
#    define LOG_U8_ARRAY(arr, len)                                             \
        for (int i = 0; i < len; i++)                                          \
        {                                                                      \
            printf("%02x ", arr[i]);                                          \
        }                                                                      \
        printf("\r\n");
#else
#    define LOG_OUT(...)
#    define LOG_ERR(...)
#    define LOG_U8_ARRAY(...)
#endif


#define TCP_TAG_AND_LENGTH_SIZE  (sizeof(uint8_t) + sizeof(uint16_t))
#define MAX_PAYLOAD_LEN L2_MAX_FRAME_SIZE
#define MAX_BUFFER_LEN  (TCP_TAG_AND_LENGTH_SIZE + MAX_PAYLOAD_LEN)

#define TX_ATTEMPTS 3
#define RX_ATTEMPTS 3
#define MAX_RECV_SIZE (MAX_PAYLOAD_LEN + TCP_TAG_AND_LENGTH_SIZE)

typedef struct
{
    union
    {
        uint8_t BUFFER[MAX_BUFFER_LEN];
        struct __attribute__((__packed__, __aligned__))
        {
            uint8_t  TAG;
            uint16_t LENGTH;
            uint8_t  PAYLOAD[MAX_PAYLOAD_LEN];
        };
    };
} buffer_s;

// Possible values for TAG field
typedef enum
{
    TAG_E_SPI_DRIVE_CSN_LOW  = 0x01,
    TAG_E_SPI_DRIVE_CSN_HIGH = 0x02,
    TAG_E_SPI_SEND           = 0x03,
    TAG_E_POWER_ON           = 0x04,
    TAG_E_POWER_OFF          = 0x05,
    TAG_E_WAIT               = 0x06,
    TAG_E_RESET_TARGET       = 0x10,
    TAG_E_INVALID            = 0xfd,
    TAG_E_UNSUPPORTED        = 0xfe,
} tag_e;

// reception buffer
static buffer_s rx_buffer;
// emission buffer
static buffer_s tx_buffer;
// server socket
static int socket_fd = -1;

static int lt_connect_to_server ()
{
    struct sockaddr_in server;

    // Create socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        LOG_ERR("Could not create socket: %s (%d).\n", strerror(errno), errno);
        return LT_FAIL;
    }
    LOG_OUT("Socket created.\n");

    // Server information
    memset(&server, 0, sizeof(server));
    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = inet_addr(TCP_ADDR);
    server.sin_port        = htons(TCP_PORT);

    // Connect to the server
    LOG_OUT("Connecting to %s:%d.\n", TCP_ADDR, TCP_PORT);
    if (connect(socket_fd, (struct sockaddr *)(&server), sizeof(server)) < 0)
    {
        LOG_ERR("Could not connect: %s (%d).\n", strerror(errno), errno);
        return 1;
    }
    LOG_OUT("Connected to the server.\n");

    return 0;
}

static int lt_send_all (int socket, uint8_t *buffer, size_t length)
{
    int nb_bytes_sent;
    int nb_bytes_sent_total = 0;
    int nb_bytes_to_send = length;
    uint8_t *ptr = buffer;

    // attempt several times to send the data
    for (int i = 0; i < TX_ATTEMPTS; i++) {
        LOG_OUT("Attempting to send data: attempt #%d.\n", i);
        nb_bytes_sent = send(socket, ptr, nb_bytes_to_send, 0);
        if (nb_bytes_sent <= 0) {
            LOG_ERR("Send failed: %s (%d).\n", strerror(errno), errno);
            return 1;
        }

        nb_bytes_to_send -= nb_bytes_sent;
        if (nb_bytes_to_send == 0)
        {
            LOG_OUT("All %ld bytes sent successfully.\n", length);
            return 0;
        }

        ptr += nb_bytes_sent;
        nb_bytes_sent_total += nb_bytes_sent;
    }

    // could not send all the data after several attempts
    LOG_ERR("%d bytes sent instead of expected %lu ", nb_bytes_sent_total, length);
    LOG_ERR("after %d attempts.\n", TX_ATTEMPTS);

    return 1;
}

static int lt_communicate (int *tx_payload_length_ptr, int *rx_payload_length_ptr)
{
    int status;
    int nb_bytes_received;
    int nb_bytes_received_total = 0;
    int nb_bytes_to_receive = TCP_TAG_AND_LENGTH_SIZE;
    uint8_t * rx_ptr = rx_buffer.BUFFER;
    // number of bytes to send
    int nb_bytes_to_send = TCP_TAG_AND_LENGTH_SIZE;

    if (tx_payload_length_ptr != NULL) {
        nb_bytes_to_send += *tx_payload_length_ptr;
    }

    // update payload length field
    tx_buffer.LENGTH = nb_bytes_to_send - TCP_TAG_AND_LENGTH_SIZE;

    // send data
    LOG_U8_ARRAY(tx_buffer.BUFFER, nb_bytes_to_send);

    status = lt_send_all(socket_fd, tx_buffer.BUFFER, nb_bytes_to_send);
    if (status != 0)
    {
        return status;
    }

    // receive data
    LOG_OUT("- Receiving data from target.\n");
    nb_bytes_received = recv(socket_fd, rx_buffer.BUFFER, MAX_RECV_SIZE, 0);

    if (nb_bytes_received < 0)
    {
        LOG_ERR("Receive failed: %s (%d).\n", strerror(errno), errno);
        return 1;
    }

    else if (nb_bytes_received < nb_bytes_to_receive)
    {
        LOG_ERR("At least %d bytes are expected: %d.\n", nb_bytes_to_receive,
                nb_bytes_received);
        return 1;
    }

    LOG_OUT("Length field: %d.\n", rx_buffer.LENGTH);
    nb_bytes_to_receive += rx_buffer.LENGTH;
    nb_bytes_received_total += nb_bytes_received;
    LOG_OUT("Received %d bytes out of %d expected.\n", nb_bytes_received_total, nb_bytes_to_receive);

    if (nb_bytes_received_total < nb_bytes_to_receive)
    {
        rx_ptr += nb_bytes_received;

        for (int i = 0; i < RX_ATTEMPTS; i++)
        {
            LOG_OUT("Attempting to receive remaining bytes: attempt #%d.\n", i);
            nb_bytes_received = recv(socket_fd, rx_buffer.BUFFER, MAX_RECV_SIZE, 0);

            if (nb_bytes_received < 0)
            {
                LOG_ERR("Receive failed: %s (%d).\n", strerror(errno), errno);
                return 1;
            }

            nb_bytes_received_total += nb_bytes_received;
            if (nb_bytes_received_total >= nb_bytes_to_receive)
            {
                LOG_OUT("Received %d bytes in total.\n", nb_bytes_received_total);
                break;
            }
            rx_ptr += nb_bytes_received;
        }
    }

    if (nb_bytes_received_total != nb_bytes_to_receive)
    {
        LOG_ERR("Received %d bytes in total instead of %d.\n",
                nb_bytes_received_total, nb_bytes_to_receive);
        return 1;
    }

    // server does not know the sent tag
    if (rx_buffer.TAG == TAG_E_INVALID)
    {
        LOG_ERR("Tag %d is not known by the server.\n", tx_buffer.TAG);
        return 1;
    }

    // server does not know what to do with the sent tag
    else if (rx_buffer.TAG == TAG_E_UNSUPPORTED)
    {
        LOG_ERR("Tag %d is not supported by the server.\n", tx_buffer.TAG);
        return 1;
    }

    // RX tag and TX tag should be identical
    else if (rx_buffer.TAG != tx_buffer.TAG)
    {
        LOG_ERR("Expected tag %d, received %d.\n", rx_buffer.TAG,
                tx_buffer.TAG);
        return 1;
    }

    LOG_OUT("Rx tag and tx tag match: %d.\n", rx_buffer.TAG);
    if (rx_payload_length_ptr != NULL)
    {
        *rx_payload_length_ptr = nb_bytes_received_total - TCP_TAG_AND_LENGTH_SIZE;
    }

    return 0;
}

//static int lt_tropic01_power_on(void)
//{
//    LOG_OUT("-- Powering on the TROPIC01 chip.\n");
//    tx_buffer.TAG = TAG_E_POWER_ON;
//    return lt_communicate(NULL, NULL);
//}
//
//static int lt_tropic01_power_off(void)
//{
//    LOG_OUT("-- Powering off the TROPIC01 chip.\n");
//    tx_buffer.TAG = TAG_E_POWER_OFF;
//    return lt_communicate(NULL, NULL);
//}
//static int lt_reset_target(void)
//{
//    LOG_OUT("-- Resetting TROPIC01 target.\n");
//    tx_buffer.TAG = TAG_E_RESET_TARGET;
//
//    return lt_communicate(NULL, NULL);
//}

static int server_connect(void)
{
    bzero(tx_buffer.BUFFER, MAX_BUFFER_LEN);
    bzero(rx_buffer.BUFFER, MAX_BUFFER_LEN);

    int ret = lt_connect_to_server();
    if (ret != 0)
    {
        return ret;
    }
    return 0;
}

static int server_disconnect(void)
{
    LOG_OUT("-- Server disconnect\n");
    return close(socket_fd);
}

lt_ret_t lt_port_init(lt_handle_t *h)
{
    UNUSED(h);
    memset(h, 0, sizeof(lt_handle_t));

    int ret = server_connect();
    if(ret != 0) {
        return LT_FAIL;
    }

    //lt_reset_target();

    return LT_OK;
}

lt_ret_t lt_port_deinit(lt_handle_t *h)
{
    UNUSED(h);
    if(server_disconnect() != 0) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_port_spi_csn_low (lt_handle_t *h)
{
    UNUSED(h);
    LOG_OUT("-- Driving Chip Select to Low.\n");
    tx_buffer.TAG = TAG_E_SPI_DRIVE_CSN_LOW;
    return lt_communicate(NULL, NULL);
}

lt_ret_t lt_port_spi_csn_high (lt_handle_t *h)
{
    UNUSED(h);
    LOG_OUT("-- Driving Chip Select to High.\n");
    tx_buffer.TAG = TAG_E_SPI_DRIVE_CSN_HIGH;
    return lt_communicate(NULL, NULL);
}

lt_ret_t lt_port_spi_transfer (lt_handle_t *h, uint8_t offset, uint16_t tx_data_length, uint32_t timeout)
{
    UNUSED(h);
    UNUSED(timeout);

    if (offset + tx_data_length > LT_L1_LEN_MAX) {
        return LT_L1_DATA_LEN_ERROR;
    }

    LOG_OUT("-- Sending data through SPI bus.\n");

    int tx_payload_length = tx_data_length;
    int rx_payload_length;

    tx_buffer.TAG    = TAG_E_SPI_SEND;
    tx_buffer.LENGTH = (uint16_t)tx_payload_length;

    // copy tx_data to tx payload
    memcpy(&tx_buffer.PAYLOAD, h->l2_buff, tx_payload_length);

    int status = lt_communicate(&tx_payload_length, &rx_payload_length);
    if (status != 0)
    {
        return LT_FAIL;
    }

    memcpy(h->l2_buff + offset, &rx_buffer.PAYLOAD, rx_payload_length);

    return LT_OK;
}

lt_ret_t lt_port_delay (lt_handle_t *h, uint32_t wait_time_usecs)
{
    UNUSED(h);
    LOG_OUT("-- Waiting for the target.\n");

    tx_buffer.TAG                     = TAG_E_WAIT;
    int payload_length                = sizeof(uint32_t);
    //*(uint32_t *)(&tx_buffer.PAYLOAD) = wait_time_usecs;
    rx_buffer.PAYLOAD[0] = wait_time_usecs  & 0x000000ff;
    rx_buffer.PAYLOAD[1] = (wait_time_usecs & 0x0000ff00) >> 8;
    rx_buffer.PAYLOAD[2] = (wait_time_usecs & 0x00ff0000) >> 16;
    rx_buffer.PAYLOAD[3] = (wait_time_usecs & 0xff000000) >> 24;

    return lt_communicate(&payload_length, NULL);
}

lt_ret_t lt_port_random_bytes(uint32_t *buff, uint16_t len) {

    for(int i=0; i<len; i++) {
        buff[i] = 0xabcdabcd;
    }

    return LT_OK;
}
