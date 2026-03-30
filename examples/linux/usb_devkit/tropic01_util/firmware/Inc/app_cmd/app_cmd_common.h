#ifndef APP_CMD_COMMON_H
#define APP_CMD_COMMON_H

#include <stdint.h>

#include "libtropic_common.h"
#include "main.h"
#include "psa/crypto.h"
#include "usb_devkit_messages.pb.h"

/**
 * @brief MAC-and-destroy related data to store into unprotected non-volatile memory.
 *
 */
typedef struct macandd_data_t {
    // We use 32B secret as input into AES-256, so output is also 32B.
    uint8_t ciphertexts[TR01_MACANDD_ROUNDS_MAX][TR01_MAC_AND_DESTROY_DATA_SIZE];
    uint8_t auth_tag[PSA_HASH_LENGTH(PSA_ALG_SHA_256)];
    lt_mac_and_destroy_slot_t depleted_attempts;
} macandd_data_t;

extern macandd_data_t g_macandd_data;

/**
 * @brief Process USB DevKit application command.
 *
 * @param[in]  cmd   USB DevKit command.
 * @param[out] resp  USB DevKit response.
 */
void process_app_cmd(const UsbDevkitCmd *cmd, UsbDevkitResp *resp);

/**
 * @brief STM32 HASH peripheral HMAC-SHA256 wrapper.
 *
 * @param key       Key data buffer
 * @param key_len   Length of data in key buffer
 * @param data      Data buffer
 * @param data_len  Length of data buffer
 * @param output    Output buffer for HMAC result
 * @return          HAL_StatusTypeDef
 */
HAL_StatusTypeDef hmac_sha256(const uint8_t *key, const size_t key_len, const uint8_t *data,
                              const size_t data_len, uint8_t *output);

HAL_StatusTypeDef flash_write(uint32_t addr, const void *data, size_t data_len);

#endif  // APP_CMD_COMMON_H