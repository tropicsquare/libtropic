#include <stdlib.h>

#include "lt_crc16.h"
#include "libtropic_common.h"

#include "lt_mock_helpers.h"

#include "libtropic_logging.h"

void add_resp_crc(void *resp_buf) {

    uint8_t *resp_buf_bytes = (uint8_t *)resp_buf;

    // CRC is calculated from STATUS, RSP_LEN and RSP_DATA, skipping CHIP_STATUS.
    uint16_t resp_len = TR01_L2_STATUS_SIZE + TR01_L2_REQ_RSP_LEN_SIZE + resp_buf_bytes[TR01_L2_RSP_LEN_OFFSET];
    uint16_t crc = crc16(resp_buf_bytes + TR01_L1_CHIP_STATUS_SIZE, resp_len);

    // Append CRC at the end of the response buffer.
    resp_buf_bytes[TR01_L1_CHIP_STATUS_SIZE + resp_len] = crc >> 8;
    resp_buf_bytes[TR01_L1_CHIP_STATUS_SIZE + resp_len + 1] = crc & 0x00FF;
}

size_t calc_mocked_resp_len(const void *resp_buf) {
    
    const uint8_t *resp_buf_bytes = (const uint8_t *)resp_buf;

    // Total length is CHIP_STATUS + STATUS + RSP_LEN + RSP_DATA + CRC
    return TR01_L1_CHIP_STATUS_SIZE + TR01_L2_STATUS_SIZE + TR01_L2_REQ_RSP_LEN_SIZE
           + resp_buf_bytes[TR01_L2_RSP_LEN_OFFSET] + TR01_L2_REQ_RSP_CRC_SIZE;

}
