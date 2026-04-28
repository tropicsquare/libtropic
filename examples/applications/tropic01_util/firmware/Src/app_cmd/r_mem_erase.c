#include "app_cmd/r_mem_erase.h"

#include "libtropic.h"
#include "libtropic_common.h"
#include "main.h"
#include "usb_devkit_messages.pb.h"

void r_mem_erase(const RMemEraseCmd *cmd, AppResp *resp)
{
    lt_ret_t lt_ret = lt_r_mem_data_erase(&lt_handle, cmd->udata_slot);

    if (lt_ret != LT_OK) {
        resp->type.r_mem_erase.res_code = R_MEM_ERASE_RESP_CODE_ERROR;
        resp->has_libtropic_res_code = true;
        resp->libtropic_res_code = lt_ret;
    }
    else {
        resp->type.r_mem_erase.res_code = R_MEM_ERASE_RESP_CODE_OK;
    }
}