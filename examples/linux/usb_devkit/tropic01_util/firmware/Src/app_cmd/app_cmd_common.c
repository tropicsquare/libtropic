#include "app_cmd/app_cmd_common.h"

#include "app_cmd/pin_set.h"
#include "app_cmd/pin_verify.h"
#include "main.h"
#include "usb_devkit_messages.pb.h"

HAL_StatusTypeDef flash_write(uint32_t addr, const void *data, size_t data_len)
{
    uint32_t page_error = 0U;
    uint32_t write_addr = addr;
    HAL_StatusTypeDef hal_status;
    FLASH_EraseInitTypeDef erase = {0};
    const uint8_t *src = (const uint8_t *)data;
    size_t remaining = data_len;
    uint8_t quadword[16];

    erase.TypeErase = FLASH_TYPEERASE_PAGES;
    erase.Banks = ((addr - FLASH_BASE) < FLASH_BANK_SIZE) ? FLASH_BANK_1 : FLASH_BANK_2;
    erase.Page = ((addr - FLASH_BASE) % FLASH_BANK_SIZE) / FLASH_PAGE_SIZE;
    erase.NbPages = 1;

    hal_status = HAL_FLASH_Unlock();
    if (hal_status != HAL_OK) {
        return hal_status;
    }

    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

    hal_status = HAL_FLASHEx_Erase(&erase, &page_error);
    if (hal_status != HAL_OK) {
        goto lock_and_exit;
    }

    while (remaining > 0) {
        size_t chunk = remaining > sizeof(quadword) ? sizeof(quadword) : remaining;

        memset(quadword, 0xFF, sizeof(quadword));
        memcpy(quadword, src, chunk);

        hal_status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, write_addr, (uint32_t)quadword);
        if (hal_status != HAL_OK) {
            goto lock_and_exit;
        }

        src += chunk;
        remaining -= chunk;
        write_addr += sizeof(quadword);
    }

    if (memcmp((const void *)addr, data, data_len) != 0) {
        hal_status = HAL_ERROR;
    }

lock_and_exit:
    (void)HAL_FLASH_Lock();
    return hal_status;
}

void process_app_cmd(const UsbDevkitCmd *cmd, UsbDevkitResp *resp)
{
    resp->which_type = UsbDevkitResp_app_tag;

    switch (cmd->type.app.which_type) {
        case AppCmd_pin_set_tag:
            resp->type.app.which_type = AppResp_pin_set_tag;
            pin_set(&cmd->type.app.type.pin_set, &resp->type.app);
            break;

        case AppCmd_pin_verify_tag:
            resp->type.app.which_type = AppResp_pin_verify_tag;
            pin_verify(&cmd->type.app.type.pin_verify, &resp->type.app);
            break;

            // case AppCmd_r_mem_read_tag:
            //     break;

            // case AppCmd_r_mem_write_tag:
            //     break;

        default:
            resp->which_type = UsbDevkitResp_error_tag;
            resp->type.error.res_code = ERROR_RESP_CODE_UNKNOWN_CMD;
            break;
    }
}