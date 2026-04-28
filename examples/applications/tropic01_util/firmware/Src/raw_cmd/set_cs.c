#include "raw_cmd/set_cs.h"

#include <stdbool.h>

#include "main.h"
#include "usb_devkit_messages.pb.h"

void set_cs(const SetCsCmd *cmd, SetCsResp *resp, bool auto_cs_mode)
{
    // CS can be manually set only if auto CS mode is off.
    if (auto_cs_mode) {
        resp->res_code = SET_CS_RESP_CODE_AUTO_CS_MODE_ON;
    }
    else {
        HAL_GPIO_WritePin(TR01_CS_GPIO_Port, TR01_CS_Pin, cmd->high ? GPIO_PIN_SET : GPIO_PIN_RESET);
        resp->res_code = SET_CS_RESP_CODE_OK;
    }
}