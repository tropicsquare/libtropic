#include "app_cmd/r_mem_read.h"

#include "libtropic.h"
#include "libtropic_common.h"
#include "main.h"
#include "usb_devkit_messages.pb.h"

void r_mem_read(const RMemReadCmd *cmd, AppResp *resp)
{
    lt_ret_t lt_ret = lt_r_mem_data_read(&lt_handle, cmd->udata_slot, resp->type.r_mem_read.data.bytes,
                                         sizeof(resp->type.r_mem_read.data.bytes),
                                         &resp->type.r_mem_read.data.size);

    if (lt_ret == LT_OK) {
        resp->type.r_mem_read.res_code = R_MEM_READ_RESP_CODE_OK;
    }
    else if (lt_ret == LT_L3_R_MEM_DATA_READ_SLOT_EMPTY) {
        resp->type.r_mem_read.res_code = R_MEM_READ_RESP_CODE_SLOT_EMPTY;
    }
    else {
        resp->type.r_mem_read.res_code = R_MEM_READ_RESP_CODE_ERROR;
        resp->has_libtropic_res_code = true;
        resp->libtropic_res_code = lt_ret;
    }
}