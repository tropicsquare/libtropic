#ifndef LT_TR01_ATTRS_H
#define LT_TR01_ATTRS_H

/**
 * @file lt_tr01_attrs.h
 * @brief Declarations for handling TROPIC01 attributes based on silicon revision and FW versions.
 * @copyright Copyright (c) 2020-2026 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <stdint.h>

#include "libtropic_common.h"

/**
 * @brief Initializes the lt_tr01_attrs_t structure based on the silicon revision (read from CHIP_ID)
 * and the Application FW version.
 * @warning This function expects that TROPIC01 is executing Application FW, otherwise Get_Info_Req for
 * the Application FW version will fail.
 *
 * @param h   Handle for communication with TROPIC01
 * @retval    LT_OK Function executed successfully
 * @retval    LT_SILICON_REV_UNKNOWN Silicon revision of TROPIC01 is not known to Libtropic
 * @retval    LT_APP_FW_TOO_NEW Application FW version of TROPIC01 is too new for Libtropic
 * @retval    other Function did not execute successully, you might use lt_ret_verbose() to get verbose
 * encoding
 */
lt_ret_t lt_init_tr01_attrs(lt_handle_t *h) __attribute__((warn_unused_result));

#endif  // LT_TR01_ATTRS_H