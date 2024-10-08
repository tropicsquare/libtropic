/**
 * @file lt_l1_port_wrap.c
 * @brief Layer 1 functions definitions
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "libtropic_common.h"
#include "libtropic_port.h"

lt_ret_t lt_l1_init(lt_handle_t *h)
{
#ifdef LIBT_DEBUG
    if(!h) {
        return LT_PARAM_ERR;
    }
#endif
    return lt_port_init(h);
}

lt_ret_t lt_l1_deinit(lt_handle_t *h)
{
#ifdef LIBT_DEBUG
    if(!h) {
        return LT_PARAM_ERR;
    }
#endif
    return lt_port_deinit(h);
}

lt_ret_t lt_l1_spi_csn_low(lt_handle_t *h)
{
#ifdef LIBT_DEBUG
    if(!h) {
        return LT_PARAM_ERR;
    }
#endif
    return lt_port_spi_csn_low(h);
}

lt_ret_t lt_l1_spi_csn_high(lt_handle_t *h)
{
#ifdef LIBT_DEBUG
    if(!h) {
        return LT_PARAM_ERR;
    }
#endif
    return lt_port_spi_csn_high(h);
}

lt_ret_t lt_l1_spi_transfer(lt_handle_t *h, uint8_t offset, uint16_t tx_len, uint32_t timeout)
{
#ifdef LIBT_DEBUG
    if(!h) {
        return LT_PARAM_ERR;
    }
#endif
    return lt_port_spi_transfer(h, offset, tx_len, timeout);
}

lt_ret_t lt_l1_delay(lt_handle_t *h, uint32_t ms)
{
#ifdef LIBT_DEBUG
    if(!h) {
        return LT_PARAM_ERR;
    }
#endif
    return lt_port_delay(h, ms);
}
