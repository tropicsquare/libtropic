/////////////////////////
// C TCP client for communicating with the TROPIC01Model

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ts_common.h"
#include "ts_l1.h"

#define UNUSED(x) (void)(x)

#ifdef NDEBUG
#    define LOG_OUT(f_, ...) printf(f_, ##__VA_ARGS__)
#    define LOG_ERR(f_, ...) fprintf(stderr, "ERROR: " f_, ##__VA_ARGS__)
#    define LOG_U8_ARRAY(arr, len)                                             \
        for (int i = 0; i < len; i++)                                          \
        {                                                                      \
            LOG_OUT("%02x ", arr[i]);                                          \
        }                                                                      \
        LOG_OUT("\n");
#else
#    define LOG_OUT(...)
#    define LOG_ERR(...)
#    define LOG_U8_ARRAY(...)
#endif

#define TCP_ADDR "127.0.0.1"
#define TCP_PORT 28992

#define UINT8_SIZE  1
#define UINT16_SIZE 2
#define UINT32_SIZE 4
#define UINT64_SIZE 8

#define MIN_BUFFER_LEN  (UINT8_SIZE + UINT16_SIZE)
#define MAX_PAYLOAD_LEN 259
#define MAX_BUFFER_LEN  (MIN_BUFFER_LEN + MAX_PAYLOAD_LEN)

#define TX_ATTEMPTS 3
#define RX_ATTEMPTS 3
#define BUFFER_SIZE 1024

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

static int ts_connect_to_server ()
{
    struct sockaddr_in server;

    // Create socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        LOG_ERR("Could not create socket: %s (%d).\n", strerror(errno), errno);
        return TS_FAIL;
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
        return TS_FAIL;
    }
    LOG_OUT("Connected to the server.\n");

    return TS_OK;
}

static int ts_send_all (int socket, uint8_t * buffer, size_t length)
{
    // check length is strictly positive
    if (length <= 0)
    {
        LOG_ERR("Parameter length is not striclty positive: %lu.\n", length);
        return TS_FAIL;
    }

    int       nb_bytes_sent;
    int       nb_bytes_sent_total = 0;
    int       nb_bytes_to_send    = length;
    uint8_t * ptr                 = buffer;

    // attempt several times to send the data
    for (int i = 0; i < TX_ATTEMPTS; i++)
    {
        LOG_OUT("Attempting to send data: attempt #%d.\n", i);

        nb_bytes_sent = send(socket, ptr, nb_bytes_to_send, 0);
        if (nb_bytes_sent < 1)
        {
            LOG_ERR("Send failed: %s (%d).\n", strerror(errno), errno);
            return TS_FAIL;
        }

        nb_bytes_to_send -= nb_bytes_sent;
        if (nb_bytes_to_send == 0)
        {
            LOG_OUT("All %ld bytes sent successfully.\n", length);
            return TS_OK;
        }

        ptr += nb_bytes_sent;
        nb_bytes_sent_total += nb_bytes_sent;
    }

    // could not send all the data after several attempts
    LOG_ERR("%d bytes sent instead of expected %lu ", nb_bytes_sent_total,
            length);
    LOG_ERR("after %d attempts.\n", TX_ATTEMPTS);
    return TS_OK;
}

static int ts_communicate (int * tx_payload_length_ptr, int * rx_payload_length_ptr)
{

    int       status;
    int       nb_bytes_received;
    int       nb_bytes_received_total = 0;
    int       nb_bytes_to_receive     = MIN_BUFFER_LEN;
    uint8_t * rx_ptr                  = rx_buffer.BUFFER;

    // number of bytes to send
    int nb_bytes_to_send = MIN_BUFFER_LEN;
    if (tx_payload_length_ptr != NULL)
    {
        nb_bytes_to_send += *tx_payload_length_ptr;
    }

    // update payload length field
    tx_buffer.LENGTH = nb_bytes_to_send - MIN_BUFFER_LEN;

    // send data
    LOG_U8_ARRAY(tx_buffer.BUFFER, nb_bytes_to_send);

    status = ts_send_all(socket_fd, tx_buffer.BUFFER, nb_bytes_to_send);
    if (status != TS_OK)
    {
        return status;
    }

    // receive data
    LOG_OUT("- Receiving data from target.\n");
    nb_bytes_received = recv(socket_fd, rx_buffer.BUFFER, BUFFER_SIZE, 0);

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
    LOG_OUT("Received %d bytes out of %d expected.\n", nb_bytes_received_total,
            nb_bytes_to_receive);

    if (nb_bytes_received_total < nb_bytes_to_receive)
    {

        rx_ptr += nb_bytes_received;

        for (int i = 0; i < RX_ATTEMPTS; i++)
        {
            LOG_OUT("Attempting to receive remaining bytes: attempt #%d.\n", i);
            nb_bytes_received =
                recv(socket_fd, rx_buffer.BUFFER, BUFFER_SIZE, 0);

            if (nb_bytes_received < 0)
            {
                LOG_ERR("Receive failed: %s (%d).\n", strerror(errno), errno);
                return TS_FAIL;
            }
            nb_bytes_received_total += nb_bytes_received;
            if (nb_bytes_received_total >= nb_bytes_to_receive)
            {
                LOG_OUT("Received %d bytes in total.\n",
                        nb_bytes_received_total);
                break;
            }
            rx_ptr += nb_bytes_received;
        }
    }
    if (nb_bytes_received_total != nb_bytes_to_receive)
    {
        LOG_ERR("Received %d bytes in total instead of %d.\n",
                nb_bytes_received_total, nb_bytes_to_receive);
        return TS_FAIL;
    }

    // server does not know the sent tag
    if (rx_buffer.TAG == TAG_E_INVALID)
    {
        LOG_ERR("Tag %d is not known by the server.\n", tx_buffer.TAG);
        return TS_FAIL;
    }
    // server does not know what to do with the sent tag
    else if (rx_buffer.TAG == TAG_E_UNSUPPORTED)
    {
        LOG_ERR("Tag %d is not supported by the server.\n", tx_buffer.TAG);
        return TS_FAIL;
    }
    // RX tag and TX tag should be identical
    else if (rx_buffer.TAG != tx_buffer.TAG)
    {
        LOG_ERR("Expected tag %d, received %d.\n", rx_buffer.TAG,
                tx_buffer.TAG);
        return TS_FAIL;
    }

    LOG_OUT("Rx tag and tx tag match: %d.\n", rx_buffer.TAG);
    if (rx_payload_length_ptr != NULL)
    {
        *rx_payload_length_ptr = nb_bytes_received_total - MIN_BUFFER_LEN;
    }
    return TS_OK;
}

static int ts_tropic01_power_on(void)
{
    LOG_OUT("-- Powering on the TROPIC01 chip.\n");
    tx_buffer.TAG = TAG_E_POWER_ON;
    return ts_communicate(NULL, NULL);
}

static int ts_tropic01_power_off(void)
{
    LOG_OUT("-- Powering off the TROPIC01 chip.\n");
    tx_buffer.TAG = TAG_E_POWER_OFF;
    return ts_communicate(NULL, NULL);
}

static int ts_reset_target(void)
{
    LOG_OUT("-- Resetting TROPIC01 target.\n");
    tx_buffer.TAG = TAG_E_RESET_TARGET;
    return ts_communicate(NULL, NULL);
}

static int server_connect(void)
{
    bzero(tx_buffer.BUFFER, MAX_BUFFER_LEN);
    bzero(rx_buffer.BUFFER, MAX_BUFFER_LEN);

    int ret = ts_connect_to_server();
    if (ret != TS_OK)
    {
        return ret;
    }
    return TS_OK;
}

static int server_disconnect(void)
{
    return close(socket_fd);
}


//////////////////////////////////////////////////////////////////////
//                                                                  //
// ts_l1 HAL definitions:                                           //
//                                                                  //
//////////////////////////////////////////////////////////////////////

ts_ret_t ts_l1_init(ts_handle_t *h)
{
    UNUSED(h);
    memset(h, 0, sizeof(ts_handle_t));

    int ret = server_connect();
    if(ret != TS_OK) {
        return TS_FAIL;
    }
    ret = ts_reset_target();
    if(ret != TS_OK) {
        return TS_FAIL;
    }
    return TS_OK;
}

ts_ret_t ts_l1_deinit(ts_handle_t *h)
{
    UNUSED(h);
    if(server_disconnect() != 0) {
        return TS_FAIL;
    }

    return TS_OK;
}

ts_ret_t ts_l1_spi_csn_low (ts_handle_t *h)
{
    UNUSED(h);
    LOG_OUT("-- Driving Chip Select to Low.\n");
    tx_buffer.TAG = TAG_E_SPI_DRIVE_CSN_LOW;
    return ts_communicate(NULL, NULL);
}

ts_ret_t ts_l1_spi_csn_high (ts_handle_t *h)
{
    UNUSED(h);
    LOG_OUT("-- Driving Chip Select to High.\n");
    tx_buffer.TAG = TAG_E_SPI_DRIVE_CSN_HIGH;
    return ts_communicate(NULL, NULL);
}

ts_ret_t ts_l1_spi_transfer (ts_handle_t *h, uint8_t offset, uint16_t tx_data_length, uint32_t timeout)
{
    UNUSED(h);
    UNUSED(timeout);

    if (offset + tx_data_length > TS_L1_LEN_MAX) {
        return TS_L1_DATA_LEN_ERROR;
    }

    LOG_OUT("-- Sending data through SPI bus.\n");
    int status;
    int tx_payload_length = tx_data_length;
    int rx_payload_length;

    tx_buffer.TAG    = TAG_E_SPI_SEND;
    tx_buffer.LENGTH = (uint16_t)tx_payload_length;

    // copy tx_data to tx payload
    for (int i = 0; i < tx_payload_length; i++)
    {
        *(tx_buffer.PAYLOAD + i) = *(h->l2_buff + i);
    }

    status = ts_communicate(&tx_payload_length, &rx_payload_length);
    if (status != TS_OK)
    {
        return status;
    }

    // copy rx payload to rx_data
    for (int i = 0; i < rx_payload_length; i++)
    {
        *(h->l2_buff + i + offset) = *(rx_buffer.PAYLOAD + i);
    }

    return TS_OK;
}

ts_ret_t ts_l1_delay (ts_handle_t *h, uint32_t wait_time_usecs)
{
    UNUSED(h);
    LOG_OUT("-- Waiting for the target.\n");
    tx_buffer.TAG                     = TAG_E_WAIT;
    int payload_length                = UINT32_SIZE;
    *(uint32_t *)(&tx_buffer.PAYLOAD) = wait_time_usecs;
    return ts_communicate(&payload_length, NULL);
}
