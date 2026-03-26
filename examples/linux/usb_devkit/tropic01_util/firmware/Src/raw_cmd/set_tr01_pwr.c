#include "raw_cmd/set_tr01_pwr.h"

#include "main.h"
#include "usb_devkit_messages.pb.h"

void set_tr01_pwr(const SetTr01PwrCmd *cmd, SetTr01PwrResp *resp)
{
    HAL_GPIO_WritePin(TR01_PWR_GPIO_Port, TR01_PWR_Pin, cmd->on ? GPIO_PIN_SET : GPIO_PIN_RESET);
    resp->res_code = SET_TR01_PWR_RESP_CODE_OK;
}