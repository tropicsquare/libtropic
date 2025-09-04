/**
 * @file libtropic_port_unix_spi.c
 * @author Evgeny Beysembaev <evgeny@contentwise.tech>
 * @brief Port for communication using Generic SPI and GPIO Linux UAPI.
 *
 * @note The chip select (CS) pin is controlled separately using GPIO, as the protocol requires
 *       manual handling of the chip select.
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
#include <inttypes.h>
#include <linux/gpio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libtropic_common.h"
#include "libtropic_logging.h"
#include "libtropic_macros.h"
#include "libtropic_port.h"
#include "libtropic_port_unix_spi.h"

lt_ret_t lt_port_init(lt_l2_state_t *s2)
{
    lt_dev_unix_spi_t *device = (lt_dev_unix_spi_t *)(s2->device);
    uint32_t request_mode;

    srand(device->rng_seed);

    LT_LOG_DEBUG("Initializing SPI...\n");
    LT_LOG_DEBUG("SPI speed: %d", device->spi_speed);
    LT_LOG_DEBUG("SPI device: %s", device->spi_dev);
    LT_LOG_DEBUG("GPIO device: %s", device->gpio_dev);
    LT_LOG_DEBUG("GPIO CS pin: %d", device->gpio_cs_num);

    device->mode = SPI_MODE_0;
    device->fd = open(device->spi_dev, O_RDWR);
    if (device->fd < 0) {
        LT_LOG_ERROR("Can't open device!");
        return LT_FAIL;
    }

    request_mode = device->mode;
    if (ioctl(device->fd, SPI_IOC_WR_MODE32, &device->mode) < 0) {
        LT_LOG_ERROR("Can't set SPI mode!");
        close(device->fd);
        return LT_FAIL;
    }

    // RD is read what mode the device actually is in.
    if (ioctl(device->fd, SPI_IOC_RD_MODE32, &device->mode) < 0) {
        LT_LOG_ERROR("Can't get SPI mode!");
        close(device->fd);
        return LT_FAIL;
    }
    if (request_mode != device->mode) {
        LT_LOG_WARN("Device does not support requested mode 0x%" PRIx32, request_mode);
    }

    if (ioctl(device->fd, SPI_IOC_WR_MAX_SPEED_HZ, &device->spi_speed) < 0) {
        LT_LOG_ERROR("Can't set max SPI speed.");
        close(device->fd);
        return LT_FAIL;
    }

    // CS is controlled separately.
    device->gpio_fd = open(device->gpio_dev, O_RDWR | O_CLOEXEC);
    if (device->gpio_fd < 0) {
        LT_LOG_ERROR("Can't open GPIO device!");
        close(device->fd);
        return LT_FAIL;
    }

    struct gpiochip_info info;
    if (ioctl(device->gpio_fd, GPIO_GET_CHIPINFO_IOCTL, &info) < 0) {
        LT_LOG_ERROR("GPIO_GET_CHIPINFO_IOCTL error!");
        LT_LOG_ERROR("Error string: %s", strerror(errno));
        close(device->fd);
        close(device->gpio_fd);
        return LT_FAIL;
    }

    LT_LOG_DEBUG("GPIO chip information:");
    LT_LOG_DEBUG("- info.name  = \"%s\"", info.name);
    LT_LOG_DEBUG("- info.label = \"%s\"", info.label);
    LT_LOG_DEBUG("- info.lines = \"%u\"", info.lines);

    memset(&device->gpioreq, 0, sizeof(device->gpioreq));
    device->gpioreq.offsets[0] = device->gpio_cs_num;
    device->gpioreq.num_lines = 1;
    device->gpioreq.config.flags = GPIO_V2_LINE_FLAG_OUTPUT;
    device->gpioreq.config.num_attrs = 1;
    device->gpioreq.config.attrs[0].attr.id = GPIO_V2_LINE_ATTR_ID_OUTPUT_VALUES;
    device->gpioreq.config.attrs[0].mask = 1;
    device->gpioreq.config.attrs[0].attr.values = 1;  // initial value = 1
    if (ioctl(device->gpio_fd, GPIO_V2_GET_LINE_IOCTL, &device->gpioreq) < 0) {
        LT_LOG_ERROR("GPIO_V2_GET_LINE_IOCTL error!");
        LT_LOG_ERROR("Error string: %s", strerror(errno));
        close(device->fd);
        close(device->gpio_fd);
        return LT_FAIL;
    }

    return LT_OK;
}

lt_ret_t lt_port_deinit(lt_l2_state_t *s2)
{
    lt_dev_unix_spi_t *device = (lt_dev_unix_spi_t *)(s2->device);

    // We want to attempt to close both, even if one of them fails, hence storing the return val
    // and checking later.
    int gpio_close_ret = close(device->gpio_fd);
    int spi_close_ret = close(device->fd);

    if (gpio_close_ret || spi_close_ret) {
        return LT_FAIL;
    }
    return LT_OK;
}

lt_ret_t lt_port_spi_csn_low(lt_l2_state_t *s2)
{
    lt_dev_unix_spi_t *device = (lt_dev_unix_spi_t *)(s2->device);
    struct gpio_v2_line_values values;

    values.mask = 1;
    values.bits = 0;
    if (ioctl(device->gpioreq.fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &values) < 0) {
        LT_LOG_ERROR("GPIO_V2_LINE_SET_VALUES_IOCTL error!");
        LT_LOG_ERROR("Error string: %s", strerror(errno));
        return LT_FAIL;
    }
    return LT_OK;
}

lt_ret_t lt_port_spi_csn_high(lt_l2_state_t *s2)
{
    lt_dev_unix_spi_t *device = (lt_dev_unix_spi_t *)(s2->device);
    struct gpio_v2_line_values values;

    values.mask = 1;
    values.bits = 1;
    if (ioctl(device->gpioreq.fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &values) < 0) {
        LT_LOG_ERROR("GPIO_V2_LINE_SET_VALUES_IOCTL error!");
        LT_LOG_ERROR("Error string: %s", strerror(errno));
        return LT_FAIL;
    }
    return LT_OK;
}

lt_ret_t lt_port_spi_transfer(lt_l2_state_t *s2, uint8_t offset, uint16_t tx_data_length, uint32_t timeout_ms)
{
    UNUSED(timeout_ms);
    lt_dev_unix_spi_t *device = (lt_dev_unix_spi_t *)(s2->device);

    int ret = 0;
    struct spi_ioc_transfer spi = {
        .tx_buf = (unsigned long)s2->buff + offset,
        .rx_buf = (unsigned long)s2->buff + offset,
        .len = tx_data_length,
        .delay_usecs = 0,
    };

    ret = ioctl(device->fd, SPI_IOC_MESSAGE(1), &spi);
    if (ret >= 0) {
        return LT_OK;
    }
    return LT_FAIL;
}

lt_ret_t lt_port_delay(lt_l2_state_t *s2, uint32_t ms)
{
    UNUSED(s2);
    LT_LOG_DEBUG("-- Waiting for the target.");

    int ret = usleep(ms * 1000);
    if (ret != 0) {
        LT_LOG_ERROR("usleep() failed: %s (%d)", strerror(errno), ret);
        return LT_FAIL;
    }

    return LT_OK;
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