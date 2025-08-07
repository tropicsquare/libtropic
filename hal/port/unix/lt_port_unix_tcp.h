/**
 * @file lt_port_unix_tcp.h
 * @author Tropic Square s.r.o.
 * @brief Port for communication with the TROPIC01 Model using TCP.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <netinet/in.h>

/**
 * @brief Device structure for Unix model port (TCP communication).
 * 
 * @note Public members are meant to be configured by the developer before passing the handle to
 *       libtropic.
 */
typedef struct lt_dev_unix_tcp_t {
    /** @public @brief Address of the model server. */
    in_addr_t addr;
    /** @public @brief Port of the model server. */
    in_port_t port;
    /** @public @brief Seed for the platform's random number generator. */
    unsigned int rng_seed;
} lt_dev_unix_tcp_t;