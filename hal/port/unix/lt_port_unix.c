/**
 * @file lt_port_unix.c
 * @author Evgeny Beysembaev <evgeny@contentwise.tech>
 * @brief  L1 implementation for Generic SPI and GPIO Linux UAPI
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

//SPI-related includes
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

//GPIO
#include <linux/gpio.h>

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "libtropic_common.h"
#include "libtropic_port.h"

#define LOG_OUT(f_, ...) printf("[SPI UAPI] "f_, ##__VA_ARGS__)
#define LOG_ERR(f_, ...) fprintf(stderr, "ERROR: " f_, ##__VA_ARGS__)
#define LOG_U8_ARRAY(arr, len)                                             \
    for (int i = 0; i < len; i++)                                          \
    {                                                                      \
        printf("%02x ", arr[i]);                                          \
    }                                                                      \
    printf("\r\n");

typedef struct
{
    int fd;
    int gpiofd;
    struct gpio_v2_line_request gpioreq;
    lt_l2_state_t *h;
    uint32_t mode;
} state_s;

static state_s s = {};

lt_ret_t lt_port_init(lt_l2_state_t *h)
{
    uint32_t request_mode;
    uint8_t spiBPW = 8;

    int speed = 100000;
    char spi_dev[] = "/dev/spidev0.0";
    char gpio_dev[] = "/dev/gpiochip0";
    int  gpio_cs_num = 4;

    s.h = h;

    LOG_OUT("Initializing SPI...\n");
    LOG_OUT("SPI speed: %d\n",   speed);
    LOG_OUT("SPI device: %s\n",  spi_dev);
    LOG_OUT("GPIO device: %s\n", gpio_dev);
    LOG_OUT("GPIO CS pin: %d\n", gpio_cs_num);

    s.mode = SPI_MODE_0;
    s.fd = open(spi_dev, O_RDWR);
    if (s.fd < 0)
    {
        LOG_ERR("can't open device");
        return LT_FAIL;
    }

    request_mode = s.mode;
    if (ioctl(s.fd, SPI_IOC_WR_MODE32, &s.mode) < 0)
    {
        LOG_ERR("can't set spi mode");
        return LT_FAIL;
    }
    /* RD is read what mode the device actually is in */
    if (ioctl(s.fd, SPI_IOC_RD_MODE32, &s.mode) < 0)
    {
        LOG_ERR("can't get spi mode");
        return LT_FAIL;
    }
    if (request_mode != s.mode)
        LOG_ERR("WARNING device does not support requested mode 0x%x\n", request_mode);

    if (ioctl (s.fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0)
    {
        LOG_ERR("can't set max speed Hz");
        return LT_FAIL;
    }

    // CS is controlled separately
    s.gpiofd = open(gpio_dev, O_RDWR | O_CLOEXEC);
    if (s.gpiofd < 0)
    {
        LOG_ERR("can't open GPIO device");
        return LT_FAIL;
    }

    struct gpiochip_info info;
    if (ioctl(s.gpiofd, GPIO_GET_CHIPINFO_IOCTL, &info) < 0) {
        LOG_ERR("GPIO_GET_CHIPINFO_IOCTL");
        return LT_FAIL;
    }

    
    printf("[+] info.name = \"%s\"\n", info.name);
    printf("[+] info.label = \"%s\"\n", info.label);
    printf("[+] info.lines = \"%u\"\n", info.lines);
    

    memset(&s.gpioreq, 0, sizeof(s.gpioreq));
    s.gpioreq.offsets[0] = gpio_cs_num;
    s.gpioreq.num_lines = 1;
    s.gpioreq.config.flags = GPIO_V2_LINE_FLAG_OUTPUT;
    s.gpioreq.config.num_attrs = 1;
    s.gpioreq.config.attrs[0].attr.id = GPIO_V2_LINE_ATTR_ID_OUTPUT_VALUES;
    s.gpioreq.config.attrs[0].mask = 1;
    s.gpioreq.config.attrs[0].attr.values = 1; // initial value = 1
    if (ioctl(s.gpiofd, GPIO_V2_GET_LINE_IOCTL, &s.gpioreq) < 0) {
        LOG_ERR("GPIO_V2_GET_LINE_IOCTL\n");
        LOG_ERR("Errno: %s\n", strerror(errno));
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_port_deinit(lt_l2_state_t *h)
{
    UNUSED(h);
    close(s.gpiofd);
    close(s.fd);
    return LT_OK;
}

lt_ret_t lt_port_spi_csn_low (lt_l2_state_t *h)
{
    UNUSED(h);
    struct gpio_v2_line_values values;

    values.mask = 1;
    values.bits = 0;
    if (ioctl(s.gpioreq.fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &values) < 0) {
        LOG_ERR("GPIO_V2_LINE_SET_VALUES_IOCTL");
        return LT_FAIL;
    }
    return LT_OK;
}

lt_ret_t lt_port_spi_csn_high (lt_l2_state_t *h)
{
    UNUSED(h);
    struct gpio_v2_line_values values;

    values.mask = 1;
    values.bits = 1;
    if (ioctl(s.gpioreq.fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &values) < 0) {
        LOG_ERR("GPIO_V2_LINE_SET_VALUES_IOCTL");
        return LT_FAIL;
    }
    return LT_OK;
}

lt_ret_t lt_port_spi_transfer (lt_l2_state_t *h, uint8_t offset, uint16_t tx_data_length, uint32_t timeout)
{
    UNUSED(timeout);
    int ret = 0;
    struct spi_ioc_transfer spi = {
        .tx_buf        = (unsigned long)h->buff + offset,
        .rx_buf        = (unsigned long)h->buff + offset,
        .len           = tx_data_length,
        .delay_usecs   = 0,
    };

    // LOG_OUT("-- Sending data through SPI bus, offset = %d, length = %d\n", offset, tx_data_length);
    //LOG_U8_ARRAY((char*)(h->l2_buff + offset), tx_data_length);
    ret = ioctl(s.fd, SPI_IOC_MESSAGE(1), &spi);
    // LOG_OUT("Received data (%d): \n", ret);
    //LOG_U8_ARRAY((char*)(h->l2_buff + offset), tx_data_length);
    if (ret >= 0)
        return LT_OK;
    return LT_FAIL;
}

lt_ret_t lt_port_delay (lt_l2_state_t *h, uint32_t wait_time_msecs)
{
    UNUSED(h);
    LOG_OUT("-- Waiting for the target.\n");

    usleep(wait_time_msecs*1000);

    return LT_OK;
}

lt_ret_t lt_port_random_bytes(uint32_t *buff, uint16_t len)
{
    for(int i=0; i<len; i++) {
        buff[i] = (uint16_t)rand(); //TODO: Why 16_t?
    }

    return LT_OK;
}