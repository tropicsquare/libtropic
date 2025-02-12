/**
 * @file lt_port_unix.c
 * @author Tropic Square s.r.o.
 * @brief  C TCP client for communicating with the TROPIC01 Model
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <time.h>

//sleep
#include <unistd.h>

#include "libtropic_common.h"
#include "libtropic_port.h"
#include <pigpio.h> 

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

int spi_handle = 0;
#define GPIO_CS 14

lt_ret_t lt_port_init(lt_handle_t *h)
{
    UNUSED(h);
    memset(h, 0, sizeof(lt_handle_t));

    // Pseudo RNG init
    srand (time(NULL));

    int status = gpioInitialise();
    LOG_OUT("gpioInitialise(): %d\r\n", status);

    int spi_handle = spiOpen(0, 500000, SPI_MODE_0);
    LOG_OUT("spi open: %d\r\n", spi_handle);

    gpioSetMode(GPIO_CS, PI_OUTPUT);

    return LT_OK;
}

lt_ret_t lt_port_deinit(lt_handle_t *h)
{
    UNUSED(h);

    int ret = spiClose(spi_handle);
    LOG_OUT("spi close: %d\r\n", ret);

    gpioTerminate();

    return LT_OK;
}

lt_ret_t lt_port_spi_csn_low (lt_handle_t *h)
{
    UNUSED(h);
    LOG_OUT("-- Driving Chip Select to Low.\n");

    gpioWrite(GPIO_CS, PI_LOW);

    return LT_OK;
}

lt_ret_t lt_port_spi_csn_high (lt_handle_t *h)
{
    UNUSED(h);
    LOG_OUT("-- Driving Chip Select to High.\n");

    gpioWrite(GPIO_CS, PI_HIGH);
    return LT_OK;
}

lt_ret_t lt_port_spi_transfer (lt_handle_t *h, uint8_t offset, uint16_t tx_data_length, uint32_t timeout)
{
    UNUSED(h);
    UNUSED(timeout);

    int ret = spiXfer(spi_handle, h->l2_buff + offset, h->l2_buff + offset, tx_data_length);

    return LT_OK;
}

lt_ret_t lt_port_delay (lt_handle_t *h, uint32_t wait_time_msecs)
{
    UNUSED(h);
    LOG_OUT("-- Waiting for the target.\n");

    usleep(wait_time_msecs*1000);		//uS (unsigned int)

    return LT_OK;
}

lt_ret_t lt_port_random_bytes(uint32_t *buff, uint16_t len) {

    for(int i=0; i<len; i++) {
        buff[i] = (uint16_t)rand();
    }

    return LT_OK;
}
