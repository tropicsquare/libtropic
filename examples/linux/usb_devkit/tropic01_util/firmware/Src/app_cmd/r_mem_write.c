#include "app_cmd/r_mem_write.h"

#include "libtropic.h"
#include "libtropic_common.h"
#include "main.h"
#include "usb_devkit_messages.pb.h"

void r_mem_write(const RMemWriteCmd *cmd, AppResp *resp)
{
    lt_ret_t lt_ret = lt_r_mem_data_write(&lt_handle, cmd->udata_slot, cmd->data.bytes,
                                          sizeof(cmd->data.bytes));

    if (lt_ret != LT_OK) {
        resp->type.r_mem_write.res_code = R_MEM_WRITE_RESP_CODE_ERROR;
        resp->has_libtropic_error_code = true;
        resp->libtropic_error_code = lt_ret;
    }
    else {
        resp->type.r_mem_write.res_code = R_MEM_WRITE_RESP_CODE_OK;
    }
}