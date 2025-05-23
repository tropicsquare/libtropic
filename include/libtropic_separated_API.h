#ifndef LT_LIBTROPIC_SEPARATE_API_H
#define LT_LIBTROPIC_SEPARATE_API_H

/**
 * @defgroup libtropic_API libtropic API functions
 * @brief Expected to be used by an application
 * @details Dear users, please use this API. It contains all functions you need to interface with TROPIC01 device.
 * @{
 */

/**
 * @file libtropic.h
 * @brief libtropic library main API header file
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdbool.h>
#include <stddef.h>

#include "libtropic_common.h"
#include "TROPIC01_configuration_objects.h"


lt_ret_t lt_out__session_start(lt_handle_t *h, const pkey_index_t pkey_index, session_state_t *state);
lt_ret_t lt_in__session_start(lt_handle_t *h, const uint8_t *stpub, const pkey_index_t pkey_index, const uint8_t *shipriv, const uint8_t *shipub, session_state_t *state);


lt_ret_t lt_out__ping(lt_handle_t *h, ping_state_t *state, const uint8_t *msg_out, const uint16_t len);
lt_ret_t lt_in__ping(lt_handle_t *h, ping_state_t *state, uint8_t *msg_in);


lt_ret_t lt_out__ecc_key_generate(lt_handle_t *h, const ecc_slot_t slot, const lt_ecc_curve_type_t curve);
lt_ret_t lt_in__ecc_key_generate(lt_handle_t *h);


/** @} */ // end of group_libtropic_API

#endif