/**
* @file ts_common.c
* @brief Shared definitions and functions commonly used by more layers
* @author Tropic Square s.r.o.
*/

#include "ts_common.h"

/**
 * @brief Returns more verbose description of ts return value
 *
 * @param error         ts_ret_t value
 * @return const char*
 */
const char *ts_ret_verbose(ts_ret_t ret) {
    switch(ret) {
        /* SDK general ret values */
        case TS_OK:
            return "TS_OK";
        case TS_FAIL:
            return "TS_FAIL";
        case TS_PARAM_ERR:
            return "TS_PARAM_ERR";

        /* Chip MODE */
        case TS_L1_CHIP_ALARM_MODE:
            return "TS_L1_CHIP_ALARM_MODE";
        case TS_L1_CHIP_STARTUP_MODE:
            return "TS_L1_CHIP_STARTUP_MODE";
        case TS_L1_CHIP_BUSY:
            return "TS_L1_CHIP_BUSY";

        /* L1 */
        case TS_L1_SPI_ERROR:
            return "TS_L1_SPI_ERROR";
        case TS_L1_DATA_LEN_ERROR:
            return "TS_L1_DATA_LEN_ERROR";

        /* L2 */
        case TS_L2_IN_CRC_ERR:
            return "TS_L2_IN_CRC_ERR";
        case TS_L2_REQ_CONT:
            return "TS_L2_REQ_CONT";
        case TS_L2_RES_CONT:
            return "TS_L2_RES_CONT";
        case TS_L2_HSK_ERR:
            return "TS_L2_HSK_ERR";
        case TS_L2_NO_SESSION:
            return "TS_L2_NO_SESSION";
        case TS_L2_TAG_ERR:
            return "TS_L2_TAG_ERR";
        case TS_L2_CRC_ERR:
            return "TS_L2_CRC_ERR";
        case TS_L2_GEN_ERR:
            return "TS_L2_GEN_ERR";
        case TS_L2_NO_RESP:
            return "TS_L2_NO_RESP";
        case TS_L2_UNKNOWN_REQ:
            return "TS_L2_UNKNOWN_REQ";

        /* L3 */
        case TS_L3_OK:
            return "TS_L3_OK";
        case TS_L3_FAIL:
            return "TS_L3_FAIL";
        case TS_L3_UNAUTHORIZED:
            return "TS_L3_UNAUTHORIZED";
        case TS_L3_INVALID_CMD:
            return "TS_L3_INVALID_CMD";

        /* Host */
        case TS_HOST_NO_SESSION:
            return "TS_HOST_NO_SESSION";

        /* Default */
        default:
            return "UNKNOWN return value";
  }
}
