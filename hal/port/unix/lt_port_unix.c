/**
 * @file lt_port_unix.c
 * @author Evgeny Beysembaev <evgeny@contentwise.tech>
 * @brief  L1 implementation for Generic SPI and GPIO Linux UAPI
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

// SPI-related includes
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <unistd.h>

// GPIO
#include <errno.h>
#include <linux/gpio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libtropic_common.h"
#include "libtropic_logging.h"
#include "libtropic_port.h"

typedef struct {
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

    lt_dev_unix_spi_t *device = (lt_dev_unix_spi_t *)(h->device);

    s.h = h;

    LT_LOG_DEBUG("Initializing SPI...\n");
    LT_LOG_DEBUG("SPI speed: %d", device->spi_speed);
    LT_LOG_DEBUG("SPI device: %s", device->spi_dev);
    LT_LOG_DEBUG("GPIO device: %s", device->gpio_dev);
    LT_LOG_DEBUG("GPIO CS pin: %d", device->gpio_cs_num);

    s.mode = SPI_MODE_0;
    s.fd = open(device->spi_dev, O_RDWR);
    if (s.fd < 0) {
        LT_LOG_ERROR("can't open device");
        return LT_FAIL;
    }

    request_mode = s.mode;
    if (ioctl(s.fd, SPI_IOC_WR_MODE32, &s.mode) < 0) {
        LT_LOG_ERROR("can't set spi mode");
        return LT_FAIL;
    }
    /* RD is read what mode the device actually is in */
    if (ioctl(s.fd, SPI_IOC_RD_MODE32, &s.mode) < 0) {
        LT_LOG_ERROR("can't get spi mode");
        return LT_FAIL;
    }
    if (request_mode != s.mode) LT_LOG_ERROR("WARNING device does not support requested mode 0x%x\n", request_mode);

    if (ioctl(s.fd, SPI_IOC_WR_MAX_SPEED_HZ, &device->spi_speed) < 0) {
        LT_LOG_ERROR("can't set max speed Hz");
        return LT_FAIL;
    }

    // CS is controlled separately
    s.gpiofd = open(device->gpio_dev, O_RDWR | O_CLOEXEC);
    if (s.gpiofd < 0) {
        LT_LOG_ERROR("can't open GPIO device");
        return LT_FAIL;
    }

    struct gpiochip_info info;
    if (ioctl(s.gpiofd, GPIO_GET_CHIPINFO_IOCTL, &info) < 0) {
        LT_LOG_ERROR("GPIO_GET_CHIPINFO_IOCTL");
        return LT_FAIL;
    }

    LT_LOG_DEBUG("Device information:");
    LT_LOG_DEBUG("[+] info.name = \"%s\"", info.name);
    LT_LOG_DEBUG("[+] info.label = \"%s\"", info.label);
    LT_LOG_DEBUG("[+] info.lines = \"%u\"", info.lines);

    memset(&s.gpioreq, 0, sizeof(s.gpioreq));
    s.gpioreq.offsets[0] = device->gpio_cs_num;
    s.gpioreq.num_lines = 1;
    s.gpioreq.config.flags = GPIO_V2_LINE_FLAG_OUTPUT;
    s.gpioreq.config.num_attrs = 1;
    s.gpioreq.config.attrs[0].attr.id = GPIO_V2_LINE_ATTR_ID_OUTPUT_VALUES;
    s.gpioreq.config.attrs[0].mask = 1;
    s.gpioreq.config.attrs[0].attr.values = 1;  // initial value = 1
    if (ioctl(s.gpiofd, GPIO_V2_GET_LINE_IOCTL, &s.gpioreq) < 0) {
        LT_LOG_ERROR("GPIO_V2_GET_LINE_IOCTL\n");
        LT_LOG_ERROR("Errno: %s\n", strerror(errno));
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

lt_ret_t lt_port_spi_csn_low(lt_l2_state_t *h)
{
    UNUSED(h);
    struct gpio_v2_line_values values;

    values.mask = 1;
    values.bits = 0;
    if (ioctl(s.gpioreq.fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &values) < 0) {
        LT_LOG_ERROR("GPIO_V2_LINE_SET_VALUES_IOCTL");
        return LT_FAIL;
    }
    return LT_OK;
}

lt_ret_t lt_port_spi_csn_high(lt_l2_state_t *h)
{
    UNUSED(h);
    struct gpio_v2_line_values values;

    values.mask = 1;
    values.bits = 1;
    if (ioctl(s.gpioreq.fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &values) < 0) {
        LT_LOG_ERROR("GPIO_V2_LINE_SET_VALUES_IOCTL");
        return LT_FAIL;
    }
    return LT_OK;
}

lt_ret_t lt_port_spi_transfer(lt_l2_state_t *h, uint8_t offset, uint16_t tx_data_length, uint32_t timeout)
{
    UNUSED(timeout);
    int ret = 0;
    struct spi_ioc_transfer spi = {
        .tx_buf = (unsigned long)h->buff + offset,
        .rx_buf = (unsigned long)h->buff + offset,
        .len = tx_data_length,
        .delay_usecs = 0,
    };

    ret = ioctl(s.fd, SPI_IOC_MESSAGE(1), &spi);
    if (ret >= 0) return LT_OK;
    return LT_FAIL;
}

lt_ret_t lt_port_delay(lt_l2_state_t *h, uint32_t wait_time_msecs)
{
    UNUSED(h);
    LT_LOG_DEBUG("-- Waiting for the target.");

    usleep(wait_time_msecs * 1000);

    return LT_OK;
}

lt_ret_t lt_port_random_bytes(uint32_t *buff, uint16_t len)
{
    for (int i = 0; i < len; i++) {
        buff[i] = (uint16_t)rand();  // TODO: Why 16_t?
    }

    return LT_OK;
}