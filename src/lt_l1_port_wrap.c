/**
 * @file lt_l1_port_wrap.c
 * @brief Layer 1 functions definitions
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "lt_l1_port_wrap.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "libtropic_common.h"
#include "libtropic_port.h"

lt_ret_t lt_l1_init(lt_l2_state_t *s2)
{
#ifdef LT_REDUNDANT_ARG_CHECK
    if (!s2) {
        return LT_PARAM_ERR;
    }
#endif
    return lt_port_init(s2);
}

lt_ret_t lt_l1_deinit(lt_l2_state_t *s2)
{
#ifdef LT_REDUNDANT_ARG_CHECK
    if (!s2) {
        return LT_PARAM_ERR;
    }
#endif
    return lt_port_deinit(s2);
}

lt_ret_t lt_l1_spi_csn_low(lt_l2_state_t *s2)
{
#ifdef LT_REDUNDANT_ARG_CHECK
    if (!s2) {
        return LT_PARAM_ERR;
    }
#endif
    return lt_port_spi_csn_low(s2);
}

lt_ret_t lt_l1_spi_csn_high(lt_l2_state_t *s2)
{
#ifdef LT_REDUNDANT_ARG_CHECK
    if (!s2) {
        return LT_PARAM_ERR;
    }
#endif
    return lt_port_spi_csn_high(s2);
}

lt_ret_t lt_l1_spi_transfer(lt_l2_state_t *s2, uint8_t offset, uint16_t tx_len, uint32_t timeout_ms)
{
#ifdef LT_REDUNDANT_ARG_CHECK
    if (!s2) {
        return LT_PARAM_ERR;
    }
#endif
    return lt_port_spi_transfer(s2, offset, tx_len, timeout_ms);
}

lt_ret_t lt_l1_delay(lt_l2_state_t *s2, uint32_t ms)
{
#ifdef LT_REDUNDANT_ARG_CHECK
    if (!s2) {
        return LT_PARAM_ERR;
    }
#endif
    return lt_port_delay(s2, ms);
}

#if LT_USE_INT_PIN

lt_ret_t lt_l1_delay_on_int(lt_l2_state_t *s2, uint32_t ms)
{
#ifdef LT_REDUNDANT_ARG_CHECK
    if (!s2) {
        return LT_PARAM_ERR;
    }
#endif
    return lt_port_delay_on_int(s2, ms);
}
#endif
