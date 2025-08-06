/**
 * @file lt_port_unix_usb_dongle.h
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <linux/gpio.h>
#include "libtropic_port.h"

typedef struct lt_dev_unix_uart_t {
    char dev_path[DEVICE_PATH_MAX_LEN];
    uint32_t baud_rate;
    unsigned int rng_seed;
} lt_dev_unix_uart_t;