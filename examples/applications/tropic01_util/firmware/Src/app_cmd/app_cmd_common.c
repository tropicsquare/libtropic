#include "app_cmd/app_cmd_common.h"

#include <stdint.h>
#include <string.h>

#include "app_cmd/get_rand_bytes.h"
#include "app_cmd/pin_set.h"
#include "app_cmd/pin_verify.h"
#include "app_cmd/r_mem_erase.h"
#include "app_cmd/r_mem_read.h"
#include "app_cmd/r_mem_write.h"
#include "main.h"
#include "usb_devkit_messages.pb.h"

HAL_StatusTypeDef hmac_sha256(const uint8_t *key, const size_t key_len, const uint8_t *data,
                              const size_t data_len, uint8_t *output)
{
    if (!key || !data || !output) {
        return HAL_ERROR;
    }

    hhash.Init.DataType = HASH_DATATYPE_8B;
    hhash.Init.KeySize = (uint32_t)key_len;
    hhash.Init.pKey = (uint8_t *)key;

    if (HAL_HASH_Init(&hhash) != HAL_OK) {
        return HAL_ERROR;
    }

    return HAL_HMACEx_SHA256_Start(&hhash, data, (uint32_t)data_len, output, HAL_MAX_DELAY);
}

HAL_StatusTypeDef flash_write(uint32_t addr, const void *data, size_t data_len)
{
    uint32_t page_error = 0U;
    uint32_t write_addr = addr;
    uint32_t padded_len;
    uint32_t end_addr;
    HAL_StatusTypeDef hal_status;
    FLASH_EraseInitTypeDef erase = {0};
    const uint8_t *src = (const uint8_t *)data;
    size_t remaining = data_len;
    uint8_t quadword[16];

    // HAL quadword programming requires valid input and a non-empty payload.
    if ((data == NULL) || (data_len == 0U)) {
        LOG_DEBUG("bad arguments");
        return HAL_ERROR;
    }

    // STM32U5 FLASH_TYPEPROGRAM_QUADWORD requires 16-byte aligned destination.
    if ((addr % sizeof(quadword)) != 0U) {
        LOG_DEBUG("destination not 16B aligned");
        return HAL_ERROR;
    }

    // Round up to full quadwords because partial tail writes are padded with 0xFF.
    padded_len = (uint32_t)(((data_len + (sizeof(quadword) - 1U)) / sizeof(quadword)) *
                            sizeof(quadword));
    if ((UINT32_MAX - addr) < (padded_len - 1U)) {
        LOG_DEBUG("padded length does not fit");
        return HAL_ERROR;
    }
    end_addr = addr + padded_len - 1U;

    erase.TypeErase = FLASH_TYPEERASE_PAGES;
    erase.Banks = ((addr - FLASH_BASE) < FLASH_BANK_SIZE) ? FLASH_BANK_1 : FLASH_BANK_2;
    erase.Page = ((addr - FLASH_BASE) % FLASH_BANK_SIZE) / FLASH_PAGE_SIZE;
    // This helper currently supports only single-bank writes in one call.
    if ((((addr - FLASH_BASE) < FLASH_BANK_SIZE) ? FLASH_BANK_1 : FLASH_BANK_2) !=
        (((end_addr - FLASH_BASE) < FLASH_BANK_SIZE) ? FLASH_BANK_1 : FLASH_BANK_2)) {
        LOG_DEBUG("writes to multiple banks not supported");
        return HAL_ERROR;
    }
    // Erase every page touched by the padded write range.
    erase.NbPages = ((((end_addr - FLASH_BASE) % FLASH_BANK_SIZE) / FLASH_PAGE_SIZE) - erase.Page) +
                    1U;

    hal_status = HAL_FLASH_Unlock();
    if (hal_status != HAL_OK) {
        LOG_DEBUG("failed to unlock flash, hal_status=%d", (int)hal_status);
        return hal_status;
    }

    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

    hal_status = HAL_FLASHEx_Erase(&erase, &page_error);
    if (hal_status != HAL_OK) {
        LOG_DEBUG("failed to erase flash, hal_status=%d", (int)hal_status);
        goto lock_and_exit;
    }

    // Program data in 16-byte chunks; short tail is padded with erased value 0xFF.
    while (remaining > 0) {
        size_t chunk = remaining > sizeof(quadword) ? sizeof(quadword) : remaining;

        memset(quadword, 0xFF, sizeof(quadword));
        memcpy(quadword, src, chunk);

        hal_status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, write_addr,
                                       (uint32_t)(uintptr_t)quadword);
        if (hal_status != HAL_OK) {
            LOG_DEBUG("failed to program flash, hal_status=%d", (int)hal_status);
            goto lock_and_exit;
        }

        src += chunk;
        remaining -= chunk;
        write_addr += sizeof(quadword);
    }

    // Ensure readback sees freshly programmed flash data.
    hal_status = HAL_ICACHE_Invalidate();
    if (hal_status != HAL_OK) {
        LOG_DEBUG("failed to invalidate I-Cache, hal_status=%d", (int)hal_status);
        goto lock_and_exit;
    }

    // Verify programmed bytes exactly match requested payload.
    if (memcmp((const void *)addr, data, data_len) != 0) {
        LOG_DEBUG("read out bytes after programming mismatch");
        hal_status = HAL_ERROR;
    }

lock_and_exit:
    HAL_StatusTypeDef lock_status = HAL_FLASH_Lock();
    if (lock_status != HAL_OK) {
        LOG_DEBUG("failed to lock flash, hal_status=%d", (int)lock_status);
    }
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

        case AppCmd_r_mem_read_tag:
            resp->type.app.which_type = AppResp_r_mem_read_tag;
            r_mem_read(&cmd->type.app.type.r_mem_read, &resp->type.app);
            break;

        case AppCmd_r_mem_write_tag:
            resp->type.app.which_type = AppResp_r_mem_write_tag;
            r_mem_write(&cmd->type.app.type.r_mem_write, &resp->type.app);
            break;

        case AppCmd_r_mem_erase_tag:
            resp->type.app.which_type = AppResp_r_mem_erase_tag;
            r_mem_erase(&cmd->type.app.type.r_mem_erase, &resp->type.app);
            break;

        case AppCmd_get_random_bytes_tag:
            resp->type.app.which_type = AppResp_get_random_bytes_tag;
            get_rand_bytes(&cmd->type.app.type.get_random_bytes, &resp->type.app);
            break;

        default:
            resp->which_type = UsbDevkitResp_error_tag;
            resp->type.error.res_code = ERROR_RESP_CODE_UNKNOWN_CMD;
            break;
    }
}