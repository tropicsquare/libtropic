#include "app_cmd/get_rand_bytes.h"

#include "libtropic.h"
#include "libtropic_common.h"
#include "main.h"
#include "usb_devkit_messages.pb.h"

void get_rand_bytes(const GetRandBytesCmd *cmd, AppResp *resp)
{
    lt_ret_t lt_ret = lt_random_value_get(&lt_handle, resp->type.get_random_bytes.random_bytes.bytes,
                                          cmd->count);

    if (lt_ret != LT_OK) {
        resp->type.get_random_bytes.res_code = GET_RAND_BYTES_RESP_CODE_ERROR;
        resp->has_libtropic_res_code = true;
        resp->libtropic_res_code = lt_ret;
    }
    else {
        resp->type.get_random_bytes.res_code = GET_RAND_BYTES_RESP_CODE_OK;
        resp->type.get_random_bytes.random_bytes.size = cmd->count;
    }
}