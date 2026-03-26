#include "raw_cmd/set_auto_cs_mode.h"

#include <stdbool.h>

void set_auto_cs_mode(const SetAutoCsModeCmd *cmd, SetAutoCsModeResp *resp, bool *auto_cs_mode)
{
    *auto_cs_mode = cmd->on;
    resp->res_code = SET_AUTO_CS_MODE_RESP_CODE_OK;
}