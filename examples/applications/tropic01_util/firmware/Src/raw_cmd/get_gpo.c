#include "raw_cmd/get_gpo.h"

#include "main.h"
#include "usb_devkit_messages.pb.h"

void get_gpo(const GetGpoCmd *cmd, GetGpoResp *resp)
{
    resp->high = (HAL_GPIO_ReadPin(TR01_GPO_GPIO_Port, TR01_GPO_Pin) == GPIO_PIN_SET);
    resp->res_code = GET_GPO_RESP_CODE_OK;
}