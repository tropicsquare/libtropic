#ifndef USB_DEVKIT_APP_COMMANDS_H
#define USB_DEVKIT_APP_COMMANDS_H

#include "libtropic_common.h"
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

void set_pin(const PinSetCmd *cmd, AppResp *resp);
void verify_pin(const PinVerifyCmd *cmd, AppResp *resp);

#endif  // USB_DEVKIT_APP_COMMANDS_H