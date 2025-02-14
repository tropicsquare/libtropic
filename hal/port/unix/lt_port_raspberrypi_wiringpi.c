/**
 * @file lt_port_raspberrypi_spi.c
 * @author Tropic Square s.r.o.
 * @brief  L1 implementation for raspberrypi. Expects wiringPi library to be
 * installed on Raspberry Pi, for more info about this library check https://github.com/WiringPi/WiringPi/tree/master
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <wiringPi.h>       /// for GPIO control
#include <wiringPiSPI.h>    /// For SPI control

#include "libtropic_common.h"
#include "libtropic_port.h"

// CS is controlled separately
#define GPIO_CS           14
#define SPI_SPEED_HZ 5000000

// File descriptor, used in init and deinit
int fd = 0;

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

lt_ret_t lt_port_delay (lt_handle_t *h, uint32_t wait_time_msecs)
{
    UNUSED(h);
    LOG_OUT("-- Waiting for the target.\n");

    usleep(wait_time_msecs*1000);

    return LT_OK;
}

lt_ret_t lt_port_random_bytes(uint32_t *buff, uint16_t len) {

    for(int i=0; i<len; i++) {
        buff[i] = (uint16_t)rand();
    }

    return LT_OK;
}

lt_ret_t lt_port_init(lt_handle_t *h)
{
    UNUSED(h);
    memset(h, 0, sizeof(lt_handle_t));

    // Pseudo RNG init
    srand(time(NULL));

    // Setup CS pin
    wiringPiSetupGpio();
    pinMode(14, OUTPUT);
    digitalWrite(GPIO_CS, HIGH);

    // Setup SPI, returns fd, error if -1 is returned
    fd = wiringPiSPISetup(0, SPI_SPEED_HZ);
    if(fd < 0) {
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_port_deinit(lt_handle_t *h)
{
    UNUSED(h);

    close(fd);

    return LT_OK;
}

lt_ret_t lt_port_spi_transfer (lt_handle_t *h, uint8_t offset, uint16_t tx_data_length, uint32_t timeout)
{
    UNUSED(timeout);
    LOG_OUT("-- Transfer of %d B\n", tx_data_length);

    int ret = wiringPiSPIxDataRW (0, 0, h->l2_buff + offset, tx_data_length);

    return LT_OK;
}

lt_ret_t lt_port_spi_csn_low (lt_handle_t *h)
{
    UNUSED(h);
    LOG_OUT("-- Driving Chip Select to Low.\n");

    digitalWrite(GPIO_CS, LOW);

    return LT_OK;
}

lt_ret_t lt_port_spi_csn_high (lt_handle_t *h)
{
    UNUSED(h);
    LOG_OUT("-- Driving Chip Select to High.\n");

    digitalWrite(GPIO_CS, HIGH);

    return LT_OK;
}
