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

//sleep
#include <unistd.h>

#include "libtropic_common.h"
#include "libtropic_port.h"


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



int spiOpen(unsigned spiChan, unsigned spiBaud, unsigned spiFlags)
{
   int i,fd;
   char  spiMode;
   char  spiBits  = 8;
   char dev[32];

   spiMode  = spiFlags & 3;
   spiBits  = 8;

   sprintf(dev, "/dev/spidev0.%d", spiChan);

   if ((fd = open(dev, O_RDWR)) < 0)
   {
      return -1;
   }

   if (ioctl(fd, SPI_IOC_WR_MODE, &spiMode) < 0)
   {
      close(fd);
      return -2;
   }

   if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &spiBits) < 0)
   {
      close(fd);
      return -3;
   }

   if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &spiBaud) < 0)
   {
      close(fd);
      return -4;
   }

   return fd;
}

int spiClose(int fd)
{
   return close(fd);
}

#define MAX_SPI_BUFSIZ 8192

char RXBuf[MAX_SPI_BUFSIZ];
char TXBuf[MAX_SPI_BUFSIZ];

#define LOOPS 10000
#define SPEED 1000000
#define BYTES 3

int bytes = BYTES;
int speed = SPEED;
int loops = LOOPS;

int spiXfer(int fd, unsigned speed, char *txBuf, char *rxBuf, unsigned count, unsigned char last_transfer)
{
   int err;
   struct spi_ioc_transfer spi;

   memset(&spi, 0, sizeof(spi));

   spi.tx_buf        = (unsigned long)txBuf;
   spi.rx_buf        = (unsigned long)rxBuf;
   spi.len           = count;
   spi.speed_hz      = speed;
   spi.delay_usecs   = 0;
   spi.bits_per_word = 8;
   spi.cs_change     = last_transfer;

   err = ioctl(fd, SPI_IOC_MESSAGE(1), &spi);

   return err;
}

int fd;

//--------------------------------------------------------------------------


lt_ret_t lt_port_init(lt_handle_t *h)
{
    UNUSED(h);
    memset(h, 0, sizeof(lt_handle_t));


    fd = spiOpen(0, 400000, 0);

    return LT_OK;
}



lt_ret_t lt_port_deinit(lt_handle_t *h)
{
    UNUSED(h);

	spiClose(fd);

    return LT_OK;
}

lt_ret_t lt_port_spi_csn_low (lt_handle_t *h)
{
    UNUSED(h);
    LOG_OUT("-- Driving Chip Select to Low.\n");

    return LT_OK;
}

lt_ret_t lt_port_spi_csn_high (lt_handle_t *h)
{
    UNUSED(h);
    LOG_OUT("-- Driving Chip Select to High.\n");

    return LT_OK;
}

lt_ret_t lt_port_spi_transfer (lt_handle_t *h, uint8_t offset, uint16_t tx_data_length, uint32_t timeout)
{
    UNUSED(h);
    UNUSED(timeout);


    int ret = spiXfer(fd, 700000, h->l2_buff + offset, h->l2_buff + offset, tx_data_length, h->last_transfer);

    return LT_OK;
}





lt_ret_t lt_port_delay (lt_handle_t *h, uint32_t wait_time_usecs)
{
    UNUSED(h);
    LOG_OUT("-- Waiting for the target.\n");

    usleep(wait_time_usecs);		//uS (unsigned int)

    return LT_OK;
}

lt_ret_t lt_port_random_bytes(uint32_t *buff, uint16_t len) {

    for(int i=0; i<len; i++) {
        buff[i] = 0xabcdabcd;
    }

    return LT_OK;
}
