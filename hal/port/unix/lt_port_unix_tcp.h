/**
 * @file lt_port_unix_tcp.h
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <netinet/in.h>

typedef struct lt_dev_unix_tcp_t {
    in_addr_t addr;
    in_port_t port;
    unsigned int rng_seed;
} lt_dev_unix_tcp_t;