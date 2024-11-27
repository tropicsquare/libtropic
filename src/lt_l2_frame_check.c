/**
 * @file lt_l2_frame_check.c
 * @brief Layer 2 frame check functions definitions
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic_common.h"
#include "lt_crc16.h"
#include "lt_l2_frame_check.h"

lt_ret_t lt_l2_frame_check(const uint8_t *frame)
{
#ifdef LIBT_DEBUG
    if(!frame) {
        return LT_PARAM_ERR;
    }
#endif
    // Take status, len and crc values from incomming frame
    uint8_t status = frame[1];
    uint8_t len = frame[2];
    uint16_t frame_crc = frame[len + 4] | frame[len + 3] << 8;

    switch (status) {

        // Valid frames, or crc errors in INCOMMING frames are handled here:
        case L2_STATUS_REQUEST_OK:
        case L2_STATUS_RESULT_OK:
            if (frame_crc != crc16(frame+1, len + 2)) {
                return LT_L2_IN_CRC_ERR;
            }
            return LT_OK;

        // L2 statuses returned by Tropic chip are handled here
        case L2_STATUS_REQUEST_CONT:
            return LT_L2_REQ_CONT;
        case L2_STATUS_RESULT_CONT:
            return LT_L2_RES_CONT;
        case L2_STATUS_HSK_ERR:
            return LT_L2_HSK_ERR;
        case L2_STATUS_NO_SESSION:
            return LT_L2_NO_SESSION;
        case L2_STATUS_TAG_ERR:
            return LT_L2_TAG_ERR;
        case L2_STATUS_CRC_ERR:
            return LT_L2_CRC_ERR;
        case L2_STATUS_GEN_ERR:
            return LT_L2_GEN_ERR;
        case L2_STATUS_NO_RESP:
            return LT_L2_NO_RESP;
        case L2_STATUS_UNKNOWN_ERR:
            return LT_L2_UNKNOWN_REQ;
        default:
            return LT_L2_STATUS_NOT_RECOGNIZED;
    }
}
