#ifndef LT_MOCK_HELPERS_H
#define LT_MOCK_HELPERS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void add_resp_crc(void *resp_buf);

size_t calc_mocked_resp_len(const void *resp_buf);

#ifdef __cplusplus
}
#endif

#endif // LT_MOCK_HELPERS_H