/**
 * @file libtropic_port_linux_spi.c
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
#include <poll.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Other
#include <sys/random.h>

#include "libtropic_common.h"
#include "libtropic_logging.h"
#include "libtropic_macros.h"
#include "libtropic_port.h"
#include "libtropic_port_linux_spi.h"

lt_ret_t lt_port_init(lt_l2_state_t *s2)
{
    lt_dev_linux_spi_t *device = (lt_dev_linux_spi_t *)(s2->device);
    uint32_t request_mode;

    // Initialize file descriptors to -1 so lt_port_deinit() can always execute safely.
    device->gpioreq_cs.fd = -1;
#if LT_USE_INT_PIN
    device->gpioreq_int.fd = -1;
#endif
    device->gpio_fd = -1;
    device->spi_fd = -1;

    LT_LOG_DEBUG("Initializing SPI...\n");
    LT_LOG_DEBUG("SPI speed: %d", device->spi_speed);
    LT_LOG_DEBUG("SPI device: %s", device->spi_dev);
    LT_LOG_DEBUG("GPIO device: %s", device->gpio_dev);
    LT_LOG_DEBUG("GPIO CS pin: %d", device->gpio_cs_num);
#if LT_USE_INT_PIN
    LT_LOG_DEBUG("GPIO interrupt pin: %d", device->gpio_int_num);
#endif

    device->mode = SPI_MODE_0;
    device->spi_fd = open(device->spi_dev, O_RDWR);
    if (device->spi_fd < 0) {
        LT_LOG_ERROR("Can't open device!");
        return LT_FAIL;
    }

    request_mode = device->mode;
    if (ioctl(device->spi_fd, SPI_IOC_WR_MODE32, &device->mode) < 0) {
        LT_LOG_ERROR("Can't set SPI mode!");
        close(device->spi_fd);
        return LT_FAIL;
    }

    // RD is read what mode the device actually is in.
    if (ioctl(device->spi_fd, SPI_IOC_RD_MODE32, &device->mode) < 0) {
        LT_LOG_ERROR("Can't get SPI mode!");
        close(device->spi_fd);
        return LT_FAIL;
    }
    if (request_mode != device->mode) {
        LT_LOG_WARN("Device does not support requested mode 0x%" PRIx32, request_mode);
    }

    if (ioctl(device->spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &device->spi_speed) < 0) {
        LT_LOG_ERROR("Can't set max SPI speed.");
        close(device->spi_fd);
        return LT_FAIL;
    }

    // CS is controlled separately.
    device->gpio_fd = open(device->gpio_dev, O_RDWR | O_CLOEXEC);
    if (device->gpio_fd < 0) {
        LT_LOG_ERROR("Can't open GPIO device!");
        close(device->spi_fd);
        return LT_FAIL;
    }

    struct gpiochip_info info;
    if (ioctl(device->gpio_fd, GPIO_GET_CHIPINFO_IOCTL, &info) < 0) {
        LT_LOG_ERROR("GPIO_GET_CHIPINFO_IOCTL error!");
        LT_LOG_ERROR("Error string: %s", strerror(errno));
        close(device->spi_fd);
        close(device->gpio_fd);
        return LT_FAIL;
    }

    LT_LOG_DEBUG("GPIO chip information:");
    LT_LOG_DEBUG("- info.name  = \"%s\"", info.name);
    LT_LOG_DEBUG("- info.label = \"%s\"", info.label);
    LT_LOG_DEBUG("- info.lines = \"%u\"", info.lines);

    // Setup for CS pin (OUTPUT)
    device->gpioreq_cs.offsets[0] = device->gpio_cs_num;
    device->gpioreq_cs.num_lines = 1;
    device->gpioreq_cs.config.flags = GPIO_V2_LINE_FLAG_OUTPUT;
    device->gpioreq_cs.config.num_attrs = 1;
    device->gpioreq_cs.config.attrs[0].attr.id = GPIO_V2_LINE_ATTR_ID_OUTPUT_VALUES;
    device->gpioreq_cs.config.attrs[0].mask = 1;
    device->gpioreq_cs.config.attrs[0].attr.values = 1;  // initial value = 1
    if (ioctl(device->gpio_fd, GPIO_V2_GET_LINE_IOCTL, &device->gpioreq_cs) < 0) {
        LT_LOG_ERROR("GPIO_V2_GET_LINE_IOCTL (CS pin) error!");
        LT_LOG_ERROR("Error string: %s", strerror(errno));
        close(device->spi_fd);
        close(device->gpio_fd);
        return LT_FAIL;
    }

#if LT_USE_INT_PIN
    // Setup for INT pin (INPUT, RISING EDGE)
    device->gpioreq_int.offsets[0] = device->gpio_int_num;
    device->gpioreq_int.num_lines = 1;
    // Configure as input with rising edge detection
    device->gpioreq_int.config.flags = GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_EDGE_RISING;

    if (ioctl(device->gpio_fd, GPIO_V2_GET_LINE_IOCTL, &device->gpioreq_int) < 0) {
        LT_LOG_ERROR("GPIO_V2_GET_LINE_IOCTL (INT pin) error!");
        LT_LOG_ERROR("Error string: %s", strerror(errno));
        close(device->gpioreq_cs.fd);  // Clean up the CS pin FD
        close(device->spi_fd);
        close(device->gpio_fd);
        return LT_FAIL;
    }
#endif

    return LT_OK;
}

lt_ret_t lt_port_deinit(lt_l2_state_t *s2)
{
    lt_dev_linux_spi_t *device = (lt_dev_linux_spi_t *)(s2->device);

    close(device->gpioreq_cs.fd);
#if LT_USE_INT_PIN
    close(device->gpioreq_int.fd);
#endif
    close(device->gpio_fd);
    close(device->spi_fd);

    // Mark all file descriptors as uninitialized.
    device->gpioreq_cs.fd = -1;
#if LT_USE_INT_PIN
    device->gpioreq_int.fd = -1;
#endif
    device->gpio_fd = -1;
    device->spi_fd = -1;

    return LT_OK;
}

lt_ret_t lt_port_spi_csn_low(lt_l2_state_t *s2)
{
    lt_dev_linux_spi_t *device = (lt_dev_linux_spi_t *)(s2->device);
    struct gpio_v2_line_values values;

    values.mask = 1;
    values.bits = 0;
    if (ioctl(device->gpioreq_cs.fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &values) < 0) {
        LT_LOG_ERROR("GPIO_V2_LINE_SET_VALUES_IOCTL error!");
        LT_LOG_ERROR("Error string: %s", strerror(errno));
        return LT_FAIL;
    }
    return LT_OK;
}

lt_ret_t lt_port_spi_csn_high(lt_l2_state_t *s2)
{
    lt_dev_linux_spi_t *device = (lt_dev_linux_spi_t *)(s2->device);
    struct gpio_v2_line_values values;

    values.mask = 1;
    values.bits = 1;
    if (ioctl(device->gpioreq_cs.fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &values) < 0) {
        LT_LOG_ERROR("GPIO_V2_LINE_SET_VALUES_IOCTL error!");
        LT_LOG_ERROR("Error string: %s", strerror(errno));
        return LT_FAIL;
    }
    return LT_OK;
}

lt_ret_t lt_port_spi_transfer(lt_l2_state_t *s2, uint8_t offset, uint16_t tx_data_length, uint32_t timeout_ms)
{
    LT_UNUSED(timeout_ms);
    lt_dev_linux_spi_t *device = (lt_dev_linux_spi_t *)(s2->device);

    int ret = 0;
    struct spi_ioc_transfer spi = {
        .tx_buf = (unsigned long)s2->buff + offset,
        .rx_buf = (unsigned long)s2->buff + offset,
        .len = tx_data_length,
        .delay_usecs = 0,
    };

    ret = ioctl(device->spi_fd, SPI_IOC_MESSAGE(1), &spi);
    if (ret >= 0) {
        return LT_OK;
    }
    return LT_FAIL;
}

lt_ret_t lt_port_delay(lt_l2_state_t *s2, uint32_t ms)
{
    LT_UNUSED(s2);
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
    LT_UNUSED(s2);

    ssize_t ret = getrandom(buff, count, 0);

    if (ret < 0) {
        LT_LOG_ERROR("lt_port_random_bytes: getrandom() failed (%s)!", strerror(errno));
        return LT_FAIL;
    }

    if ((size_t)ret != count) {
        LT_LOG_ERROR("lt_port_random_bytes: getrandom() generated %zd bytes instead of requested %zu bytes!", ret,
                     count);
        return LT_FAIL;
    }

    return LT_OK;
}

#if LT_USE_INT_PIN
lt_ret_t lt_port_delay_on_int(lt_l2_state_t *s2, uint32_t ms)
{
    lt_dev_linux_spi_t *device = (lt_dev_linux_spi_t *)(s2->device);
    struct pollfd pfd;
    int ret;

    // Set up the poll structure
    pfd.fd = device->gpioreq_int.fd;
    pfd.events = POLLIN | POLLPRI;  // Wait for data or priority event (GPIO edge)
    pfd.revents = 0;

    LT_LOG_DEBUG("Polling on INT pin (fd: %d) for %u ms...", pfd.fd, ms);

    // Wait for the event or timeout
    ret = poll(&pfd, 1, (int)ms);

    if (ret < 0) {
        LT_LOG_ERROR("poll() failed: %s", strerror(errno));
        return LT_FAIL;
    }

    if (ret == 0) {
        LT_LOG_WARN("Timeout waiting for INT pin.");
        return LT_L1_INT_TIMEOUT;
    }

    // Event occurred, check if it's the right type
    if (pfd.revents & (POLLIN | POLLPRI)) {
        // Event is ready. We MUST read it to consume it, otherwise poll()
        // will return immediately on the next call.
        struct gpio_v2_line_event event;
        ret = read(pfd.fd, &event, sizeof(event));

        if (ret < 0) {
            LT_LOG_ERROR("read() on INT pin failed: %s", strerror(errno));
            return LT_FAIL;
        }

        if (ret != sizeof(event)) {
            LT_LOG_ERROR("read() on INT pin returned unexpected size: %d", ret);
            return LT_FAIL;
        }

        // Since we only configured for RISING_EDGE, any event is the one we want.
        // event.id == GPIO_V2_LINE_EVENT_RISING_EDGE
        LT_LOG_DEBUG("Interrupt received!");
        return LT_OK;
    }

    LT_LOG_ERROR("Poll returned positive but no expected revents.");
    return LT_FAIL;
}
#endif