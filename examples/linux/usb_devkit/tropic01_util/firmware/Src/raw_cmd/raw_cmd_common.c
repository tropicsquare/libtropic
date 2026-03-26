#include "raw_cmd/raw_cmd_common.h"

#include "main.h"
#include "raw_cmd/get_gpo.h"
#include "raw_cmd/send_spi_data.h"
#include "raw_cmd/set_auto_cs_mode.h"
#include "raw_cmd/set_cs.h"
#include "raw_cmd/set_tr01_pwr.h"
#include "usb_devkit_messages.pb.h"

static bool auto_cs_mode = true;

void process_raw_cmd(const UsbDevkitCmd *cmd, UsbDevkitResp *resp)
{
    resp->which_type = UsbDevkitResp_raw_tag;

    switch (cmd->type.raw.which_type) {
        case RawCmd_send_spi_data_tag:
            resp->type.raw.which_type = RawResp_send_spi_data_tag;
            send_spi_data(&cmd->type.raw.type.send_spi_data, &resp->type.raw.type.send_spi_data,
                          auto_cs_mode);
            break;

        case RawCmd_set_auto_cs_mode_tag:
            resp->type.raw.which_type = RawResp_set_auto_cs_mode_tag;
            set_auto_cs_mode(&cmd->type.raw.type.set_auto_cs_mode,
                             &resp->type.raw.type.set_auto_cs_mode, &auto_cs_mode);
            break;

        case RawCmd_set_cs_tag:
            resp->type.raw.which_type = RawResp_set_cs_tag;
            set_cs(&cmd->type.raw.type.set_cs, &resp->type.raw.type.set_cs, auto_cs_mode);
            break;

        case RawCmd_set_tr01_pwr_tag:
            resp->type.raw.which_type = RawResp_set_tr01_pwr_tag;
            set_tr01_pwr(&cmd->type.raw.type.set_tr01_pwr, &resp->type.raw.type.set_tr01_pwr);
            break;

        case RawCmd_get_gpo_tag:
            resp->type.raw.which_type = RawResp_get_gpo_tag;
            get_gpo(&cmd->type.raw.type.get_gpo, &resp->type.raw.type.get_gpo);
            break;

        default:
            resp->which_type = UsbDevkitResp_error_tag;
            resp->type.error.res_code = ERROR_RESP_CODE_UNKNOWN_CMD;
            break;
    }
}