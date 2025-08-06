/**
 * @file lt_port_unix.h
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <linux/gpio.h>

#include "libtropic_port.h"

typedef struct {
    int spi_speed;
    char spi_dev[DEVICE_PATH_MAX_LEN];
    char gpio_dev[DEVICE_PATH_MAX_LEN];
    int gpio_cs_num;
    unsigned int rng_seed;

    int fd;
    int gpio_fd;
    struct gpio_v2_line_request gpioreq;
    uint32_t mode;
} lt_dev_unix_spi_t;