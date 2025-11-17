#ifndef LT_TR01_ATTRS_H
#define LT_TR01_ATTRS_H

#include <stdint.h>

#include "libtropic_common.h"

/**
 * @brief Initializes the lt_tr01_attrs_t structure based on the read Application FW version.
 *
 * @param h   Device's handle
 * @retval    LT_OK Function executed successfully
 * @retval    other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 */
lt_ret_t lt_init_tr01_attrs(lt_handle_t *h);

#endif  // LT_TR01_ATTRS_H