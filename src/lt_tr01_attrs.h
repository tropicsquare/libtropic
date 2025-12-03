#ifndef LT_TR01_ATTRS_H
#define LT_TR01_ATTRS_H

/**
 * @file lt_tr01_attrs.h
 * @brief Declarations for handling TROPIC01 attributes based on FW versions.
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md file in the root directory of this source tree.
 */

#include <stdint.h>

#include "libtropic_common.h"

/**
 * @brief Initializes the lt_tr01_attrs_t structure based on the read Application FW version.
 *
 * @param h   Handle for communication with TROPIC01
 * @retval    LT_OK Function executed successfully
 * @retval    other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 */
lt_ret_t lt_init_tr01_attrs(lt_handle_t *h) __attribute__((warn_unused_result));

#endif  // LT_TR01_ATTRS_H