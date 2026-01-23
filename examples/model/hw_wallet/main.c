/**
 * @file main.c
 * @brief Generic hardware wallet example with TROPIC01 model.
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <arpa/inet.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ed25519.h"  // External dependency used for signature verification.
#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_mbedtls_v4.h"
#include "libtropic_port_posix_tcp.h"
#include "psa/crypto.h"

/** @brief Message to send with Ping L3 command. */
#define PING_MSG "This is Hello World message from TROPIC01!!"
/** @brief Size of the Ping message, including '\0'. */
#define PING_MSG_SIZE 44

// Pairing keys the model was configured with, defaults to prod0 keys.
// Provide your own keys here if you configured the model differently.
#define DEFAULT_SH0_PRIV sh0priv_prod0
#define DEFAULT_SH0_PUB sh0pub_prod0

/** @brief Attestation key for ECC slot 0. */
uint8_t attestation_key[TR01_CURVE_PRIVKEY_LEN]
    = {0x22, 0x57, 0xa8, 0x2f, 0x85, 0x8f, 0x13, 0x32, 0xfa, 0x0f, 0xf6, 0x0c, 0x76, 0x29, 0x42, 0x70,
       0xa9, 0x58, 0x9d, 0xfd, 0x47, 0xa5, 0x23, 0x78, 0x18, 0x4d, 0x2d, 0x38, 0xf0, 0xa7, 0xc4, 0x01};

/** @brief X25519 private key to execute a Secure Channel Handshake on Pairing Key slot 1. */
const uint8_t sh1priv[]
    = {0x58, 0xc4, 0x81, 0x88, 0xf8, 0xb1, 0xcb, 0xd4, 0x19, 0x00, 0x2e, 0x9c, 0x8d, 0xf8, 0xce, 0xea,
       0xf3, 0xa9, 0x11, 0xde, 0xb6, 0x6b, 0xc8, 0x87, 0xae, 0xe7, 0x88, 0x10, 0xfb, 0x48, 0xb6, 0x74};

/** @brief X25519 public key to execute a Secure Channel Handshake on Pairing Key slot 1. */
const uint8_t sh1pub[]
    = {0xe1, 0xdc, 0xf9, 0xc3, 0x46, 0xbc, 0xf2, 0xe7, 0x8b, 0xa8, 0xf0, 0x27, 0xd8, 0x0a, 0x8a, 0x33,
       0xcc, 0xf3, 0xe9, 0xdf, 0x6b, 0xdf, 0x65, 0xa2, 0xc1, 0xae, 0xc4, 0xd9, 0x21, 0xe1, 0x8d, 0x51};

/** @brief X25519 private key to execute a Secure Channel Handshake on Pairing Key slot 2. */
const uint8_t sh2priv[]
    = {0x00, 0x40, 0x5e, 0x19, 0x46, 0x75, 0xab, 0xe1, 0x5f, 0x0b, 0x57, 0xf2, 0x5b, 0x12, 0x86, 0x62,
       0xab, 0xb0, 0xe9, 0xc6, 0xa7, 0xc3, 0xca, 0xdf, 0x1c, 0xb1, 0xd2, 0xb7, 0xf8, 0xcf, 0x35, 0x47};

/** @brief X25519 public key to execute a Secure Channel Handshake on Pairing Key slot 2. */
const uint8_t sh2pub[]
    = {0x66, 0xb9, 0x92, 0x5a, 0x85, 0x66, 0xe8, 0x09, 0x5c, 0x56, 0x80, 0xfb, 0x22, 0xd4, 0xb8, 0x4b,
       0xf8, 0xe3, 0x12, 0xb2, 0x7c, 0x4b, 0xac, 0xce, 0x26, 0x3c, 0x78, 0x39, 0x6d, 0x4c, 0x16, 0x6c};

/** @brief X25519 private key to execute a Secure Channel Handshake on Pairing Key slot 3. */
const uint8_t sh3priv[]
    = {0xb0, 0x90, 0x9f, 0xe1, 0xf3, 0x1f, 0xa1, 0x21, 0x75, 0xef, 0x45, 0xb1, 0x42, 0xde, 0x0e, 0xdd,
       0xa1, 0xf4, 0x51, 0x01, 0x40, 0xc2, 0xe5, 0x2c, 0xf4, 0x68, 0xac, 0x96, 0xa1, 0x0e, 0xcb, 0x46};

/** @brief X25519 public key to execute a Secure Channel Handshake on Pairing Key slot 3. */
const uint8_t sh3pub[]
    = {0x22, 0x57, 0xa8, 0x2f, 0x85, 0x8f, 0x13, 0x32, 0xfa, 0x0f, 0xf6, 0x0c, 0x76, 0x29, 0x42, 0x70,
       0xa9, 0x58, 0x9d, 0xfd, 0x47, 0xa5, 0x23, 0x78, 0x18, 0x4d, 0x2d, 0x38, 0xf0, 0xa7, 0xc4, 0x01};

/**
 * @brief Creates an HW wallet example config from the virgin R config.
 *
 * @param r_config R config to modify
 */
static void create_example_r_config(struct lt_config_t *r_config)
{
    //-------CFG_START_UP------------------------------------
    // Enable MBIST and RNGTEST (DIS in their names stands for disable, so writing 0 enables them)
    r_config->obj[TR01_CFG_START_UP_IDX]
        &= ~(BOOTLOADER_CO_CFG_START_UP_MBIST_DIS_MASK | BOOTLOADER_CO_CFG_START_UP_RNGTEST_DIS_MASK);

    //-------CFG_SENSORS-------------------------------------
    // Enable all sensors (DIS in their names stands for disable, so writing 0 enables them)
    r_config->obj[TR01_CFG_SENSORS_IDX] &= ~(
        BOOTLOADER_CO_CFG_SENSORS_PTRNG0_TEST_DIS_MASK | BOOTLOADER_CO_CFG_SENSORS_PTRNG1_TEST_DIS_MASK
        | BOOTLOADER_CO_CFG_SENSORS_OSCILLATOR_MON_DIS_MASK | BOOTLOADER_CO_CFG_SENSORS_SHIELD_DIS_MASK
        | BOOTLOADER_CO_CFG_SENSORS_VOLTAGE_MON_DIS_MASK | BOOTLOADER_CO_CFG_SENSORS_GLITCH_DET_DIS_MASK
        | BOOTLOADER_CO_CFG_SENSORS_TEMP_SENS_DIS_MASK | BOOTLOADER_CO_CFG_SENSORS_LASER_DET_DIS_MASK
        | BOOTLOADER_CO_CFG_SENSORS_EM_PULSE_DET_DIS_MASK | BOOTLOADER_CO_CFG_SENSORS_CPU_ALERT_DIS_MASK
        | BOOTLOADER_CO_CFG_SENSORS_PIN_VERIF_BIT_FLIP_DIS_MASK | BOOTLOADER_CO_CFG_SENSORS_SCB_BIT_FLIP_DIS_MASK
        | BOOTLOADER_CO_CFG_SENSORS_CPB_BIT_FLIP_DIS_MASK | BOOTLOADER_CO_CFG_SENSORS_ECC_BIT_FLIP_DIS_MASK
        | BOOTLOADER_CO_CFG_SENSORS_R_MEM_BIT_FLIP_DIS_MASK | BOOTLOADER_CO_CFG_SENSORS_EKDB_BIT_FLIP_DIS_MASK
        | BOOTLOADER_CO_CFG_SENSORS_I_MEM_BIT_FLIP_DIS_MASK | BOOTLOADER_CO_CFG_SENSORS_PLATFORM_BIT_FLIP_DIS_MASK);

    //-------CFG_DEBUG---------------------------------------
    // Disable FW logging
    r_config->obj[TR01_CFG_DEBUG_IDX] &= ~BOOTLOADER_CO_CFG_DEBUG_FW_LOG_EN_MASK;

    //-------TR01_CFG_GPO-----------------------------------------
    // Keep at reset value

    //-------TR01_CFG_SLEEP_MODE----------------------------------
    // Enable sleep mode
    r_config->obj[TR01_CFG_SLEEP_MODE_IDX] |= APPLICATION_CO_CFG_SLEEP_MODE_SLEEP_MODE_EN_MASK;

    //------- TR01_CFG_UAP_PAIRING_KEY_WRITE ---------------------
    // Disable writing pairing keys for all slots
    r_config->obj[TR01_CFG_UAP_PAIRING_KEY_WRITE_IDX] &= ~LT_TO_PAIRING_KEY_SH0(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_PAIRING_KEY_WRITE_IDX] &= ~LT_TO_PAIRING_KEY_SH1(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_PAIRING_KEY_WRITE_IDX] &= ~LT_TO_PAIRING_KEY_SH2(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_PAIRING_KEY_WRITE_IDX] &= ~LT_TO_PAIRING_KEY_SH3(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);

    //------- TR01_CFG_UAP_PAIRING_KEY_READ ----------------------
    // All sessions can read pairing keys
    r_config->obj[TR01_CFG_UAP_PAIRING_KEY_READ_IDX] |= LT_TO_PAIRING_KEY_SH0(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_PAIRING_KEY_READ_IDX] |= LT_TO_PAIRING_KEY_SH1(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_PAIRING_KEY_READ_IDX] |= LT_TO_PAIRING_KEY_SH2(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_PAIRING_KEY_READ_IDX] |= LT_TO_PAIRING_KEY_SH3(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);

    //------- TR01_CFG_UAP_PAIRING_KEY_INVALIDATE ----------------
    // 1. Disable all, then enable only specific ones
    r_config->obj[TR01_CFG_UAP_PAIRING_KEY_INVALIDATE_IDX] &= ~LT_TO_PAIRING_KEY_SH0(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_PAIRING_KEY_INVALIDATE_IDX] &= ~LT_TO_PAIRING_KEY_SH1(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_PAIRING_KEY_INVALIDATE_IDX] &= ~LT_TO_PAIRING_KEY_SH2(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_PAIRING_KEY_INVALIDATE_IDX] &= ~LT_TO_PAIRING_KEY_SH3(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    // 2. Pairing key SH0PUB can be invalidated only from session with SH0PUB
    r_config->obj[TR01_CFG_UAP_PAIRING_KEY_INVALIDATE_IDX] |= LT_TO_PAIRING_KEY_SH0(LT_SESSION_SH0_HAS_ACCESS);
    // 3. Pairing keys SH1PUB, SH2PUB and SH3PUB can be invalidated only from session with SH3PUB
    r_config->obj[TR01_CFG_UAP_PAIRING_KEY_INVALIDATE_IDX] |= LT_TO_PAIRING_KEY_SH1(LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_PAIRING_KEY_INVALIDATE_IDX] |= LT_TO_PAIRING_KEY_SH2(LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_PAIRING_KEY_INVALIDATE_IDX] |= LT_TO_PAIRING_KEY_SH3(LT_SESSION_SH3_HAS_ACCESS);

    //------- TR01_CFG_UAP_R_CONFIG_WRITE_ERASE ------------------
    // Keep at reset value, not used currently

    //------- TR01_CFG_UAP_R_CONFIG_READ -------------------------
    // Keep at reset value, not used currently

    //------- TR01_CFG_UAP_I_CONFIG_WRITE ------------------------
    // Keep at reset value, not used currently

    //------- TR01_CFG_UAP_I_CONFIG_READ -------------------------
    // Keep at reset value, not used currently

    //------- TR01_CFG_UAP_PING ----------------------------------
    // Enable for all pairing keys
    r_config->obj[TR01_CFG_UAP_PING_IDX] |= (LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS
                                             | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);

    //------- TR01_CFG_UAP_R_MEM_DATA_WRITE ----------------------
    // Reset value, not used currently

    //------- TR01_CFG_UAP_R_MEM_DATA_READ -----------------------
    // Reset value, not used currently

    //------- TR01_CFG_UAP_R_MEM_DATA_ERASE ----------------------
    // Reset value, not used currently

    //------- TR01_CFG_UAP_RANDOM_VALUE_GET ----------------------
    // Enable for all pairing keys
    r_config->obj[TR01_CFG_UAP_RANDOM_VALUE_GET_IDX] |= (LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS
                                                         | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);

    //------- TR01_CFG_UAP_ECC_KEY_GENERATE ----------------------
    // 1. Disable all, then enable only specific ones
    r_config->obj[TR01_CFG_UAP_ECC_KEY_GENERATE_IDX] &= ~LT_TO_ECC_KEY_SLOT_0_7(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_ECC_KEY_GENERATE_IDX] &= ~LT_TO_ECC_KEY_SLOT_8_15(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_ECC_KEY_GENERATE_IDX] &= ~LT_TO_ECC_KEY_SLOT_16_23(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_ECC_KEY_GENERATE_IDX] &= ~LT_TO_ECC_KEY_SLOT_24_31(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    // 2. Only session with SH3PUB can generate keys in slots 8-31
    r_config->obj[TR01_CFG_UAP_ECC_KEY_GENERATE_IDX]
        |= (LT_TO_ECC_KEY_SLOT_8_15(LT_SESSION_SH3_HAS_ACCESS) | LT_TO_ECC_KEY_SLOT_16_23(LT_SESSION_SH3_HAS_ACCESS)
            | LT_TO_ECC_KEY_SLOT_24_31(LT_SESSION_SH3_HAS_ACCESS));

    //------- TR01_CFG_UAP_ECC_KEY_STORE -------------------------
    // 1. Disable all, then enable only specific ones
    r_config->obj[TR01_CFG_UAP_ECC_KEY_STORE_IDX] &= ~LT_TO_ECC_KEY_SLOT_0_7(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_ECC_KEY_STORE_IDX] &= ~LT_TO_ECC_KEY_SLOT_8_15(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_ECC_KEY_STORE_IDX] &= ~LT_TO_ECC_KEY_SLOT_16_23(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_ECC_KEY_STORE_IDX] &= ~LT_TO_ECC_KEY_SLOT_24_31(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    // 2. Session with SH1PUB can store key into ECC key slot 0-7
    r_config->obj[TR01_CFG_UAP_ECC_KEY_STORE_IDX] |= LT_TO_ECC_KEY_SLOT_0_7(LT_SESSION_SH1_HAS_ACCESS);
    // 3. Session with SH3PUB can store key into ECC key slot 8-31
    r_config->obj[TR01_CFG_UAP_ECC_KEY_STORE_IDX] |= LT_TO_ECC_KEY_SLOT_8_15(LT_SESSION_SH3_HAS_ACCESS)
                                                     | LT_TO_ECC_KEY_SLOT_16_23(LT_SESSION_SH3_HAS_ACCESS)
                                                     | LT_TO_ECC_KEY_SLOT_24_31(LT_SESSION_SH3_HAS_ACCESS);

    //------- TR01_CFG_UAP_ECC_KEY_READ --------------------------
    // Enable for all pairing keys
    r_config->obj[TR01_CFG_UAP_ECC_KEY_READ_IDX] |= LT_TO_ECC_KEY_SLOT_0_7(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_ECC_KEY_READ_IDX] |= LT_TO_ECC_KEY_SLOT_8_15(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_ECC_KEY_READ_IDX] |= LT_TO_ECC_KEY_SLOT_16_23(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_ECC_KEY_READ_IDX] |= LT_TO_ECC_KEY_SLOT_24_31(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);

    //------- TR01_CFG_UAP_ECC_KEY_ERASE -------------------------
    // 1. Disable all, then enable only specific ones
    r_config->obj[TR01_CFG_UAP_ECC_KEY_ERASE_IDX] &= ~LT_TO_ECC_KEY_SLOT_0_7(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_ECC_KEY_ERASE_IDX] &= ~LT_TO_ECC_KEY_SLOT_8_15(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_ECC_KEY_ERASE_IDX] &= ~LT_TO_ECC_KEY_SLOT_16_23(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_ECC_KEY_ERASE_IDX] &= ~LT_TO_ECC_KEY_SLOT_24_31(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    // 2. Session with SH1PUB can erase ECC key slots 0-7
    r_config->obj[TR01_CFG_UAP_ECC_KEY_ERASE_IDX] |= LT_TO_ECC_KEY_SLOT_0_7(LT_SESSION_SH1_HAS_ACCESS);
    // 3. Session with SH3PUB can erase ECC key slots 8-31
    r_config->obj[TR01_CFG_UAP_ECC_KEY_ERASE_IDX] |= LT_TO_ECC_KEY_SLOT_8_15(LT_SESSION_SH3_HAS_ACCESS)
                                                     | LT_TO_ECC_KEY_SLOT_16_23(LT_SESSION_SH3_HAS_ACCESS)
                                                     | LT_TO_ECC_KEY_SLOT_24_31(LT_SESSION_SH3_HAS_ACCESS);

    //------- TR01_CFG_UAP_ECDSA_SIGN ----------------------------
    // 1. Disable all, then enable only specific ones
    r_config->obj[TR01_CFG_UAP_ECDSA_SIGN_IDX] &= ~LT_TO_ECC_KEY_SLOT_0_7(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_ECDSA_SIGN_IDX] &= ~LT_TO_ECC_KEY_SLOT_8_15(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_ECDSA_SIGN_IDX] &= ~LT_TO_ECC_KEY_SLOT_16_23(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_ECDSA_SIGN_IDX] &= ~LT_TO_ECC_KEY_SLOT_24_31(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    // 2. Session with SH3PUB can sign with all ECC key slots
    r_config->obj[TR01_CFG_UAP_ECDSA_SIGN_IDX]
        |= (LT_TO_ECC_KEY_SLOT_0_7(LT_SESSION_SH3_HAS_ACCESS) | LT_TO_ECC_KEY_SLOT_8_15(LT_SESSION_SH3_HAS_ACCESS)
            | LT_TO_ECC_KEY_SLOT_16_23(LT_SESSION_SH3_HAS_ACCESS)
            | LT_TO_ECC_KEY_SLOT_24_31(LT_SESSION_SH3_HAS_ACCESS));

    //------- TR01_CFG_UAP_EDDSA_SIGN ----------------------------
    // 1. Disable all, then enable only specific ones
    r_config->obj[TR01_CFG_UAP_EDDSA_SIGN_IDX] &= ~LT_TO_ECC_KEY_SLOT_0_7(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_EDDSA_SIGN_IDX] &= ~LT_TO_ECC_KEY_SLOT_8_15(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_EDDSA_SIGN_IDX] &= ~LT_TO_ECC_KEY_SLOT_16_23(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_EDDSA_SIGN_IDX] &= ~LT_TO_ECC_KEY_SLOT_24_31(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    // 2. Session with SH3PUB can sign with all ECC key slots
    r_config->obj[TR01_CFG_UAP_EDDSA_SIGN_IDX]
        |= (LT_TO_ECC_KEY_SLOT_0_7(LT_SESSION_SH3_HAS_ACCESS) | LT_TO_ECC_KEY_SLOT_8_15(LT_SESSION_SH3_HAS_ACCESS)
            | LT_TO_ECC_KEY_SLOT_16_23(LT_SESSION_SH3_HAS_ACCESS)
            | LT_TO_ECC_KEY_SLOT_24_31(LT_SESSION_SH3_HAS_ACCESS));

    //------- TR01_CFG_UAP_MCOUNTER_INIT -------------------------
    // 1. Disable all, then enable only specific ones
    r_config->obj[TR01_CFG_UAP_MCOUNTER_INIT_IDX] &= ~LT_TO_MCOUNTER_0_3(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_MCOUNTER_INIT_IDX] &= ~LT_TO_MCOUNTER_4_7(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_MCOUNTER_INIT_IDX] &= ~LT_TO_MCOUNTER_8_11(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_MCOUNTER_INIT_IDX] &= ~LT_TO_MCOUNTER_12_15(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    // 2. Session with SH3PUB can init all mcounters
    r_config->obj[TR01_CFG_UAP_MCOUNTER_INIT_IDX]
        |= (LT_TO_MCOUNTER_0_3(LT_SESSION_SH3_HAS_ACCESS) | LT_TO_MCOUNTER_4_7(LT_SESSION_SH3_HAS_ACCESS)
            | LT_TO_MCOUNTER_8_11(LT_SESSION_SH3_HAS_ACCESS) | LT_TO_MCOUNTER_12_15(LT_SESSION_SH3_HAS_ACCESS));

    //------- TR01_CFG_UAP_MCOUNTER_GET --------------------------
    // 1. Disable all, then enable only specific ones
    r_config->obj[TR01_CFG_UAP_MCOUNTER_GET_IDX] &= ~LT_TO_MCOUNTER_0_3(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_MCOUNTER_GET_IDX] &= ~LT_TO_MCOUNTER_4_7(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_MCOUNTER_GET_IDX] &= ~LT_TO_MCOUNTER_8_11(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_MCOUNTER_GET_IDX] &= ~LT_TO_MCOUNTER_12_15(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    // 2. Session with SH3PUB can get all mcounters
    r_config->obj[TR01_CFG_UAP_MCOUNTER_GET_IDX]
        |= (LT_TO_MCOUNTER_0_3(LT_SESSION_SH3_HAS_ACCESS) | LT_TO_MCOUNTER_4_7(LT_SESSION_SH3_HAS_ACCESS)
            | LT_TO_MCOUNTER_8_11(LT_SESSION_SH3_HAS_ACCESS) | LT_TO_MCOUNTER_12_15(LT_SESSION_SH3_HAS_ACCESS));

    //------- TR01_CFG_UAP_MCOUNTER_UPDATE -----------------------
    // 1. Disable all, then enable only specific ones
    r_config->obj[TR01_CFG_UAP_MCOUNTER_UPDATE_IDX] &= ~LT_TO_MCOUNTER_0_3(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_MCOUNTER_UPDATE_IDX] &= ~LT_TO_MCOUNTER_4_7(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_MCOUNTER_UPDATE_IDX] &= ~LT_TO_MCOUNTER_8_11(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_MCOUNTER_UPDATE_IDX] &= ~LT_TO_MCOUNTER_12_15(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    // 2. Session with SH3PUB can update all mcounters
    r_config->obj[TR01_CFG_UAP_MCOUNTER_UPDATE_IDX]
        |= (LT_TO_MCOUNTER_0_3(LT_SESSION_SH3_HAS_ACCESS) | LT_TO_MCOUNTER_4_7(LT_SESSION_SH3_HAS_ACCESS)
            | LT_TO_MCOUNTER_8_11(LT_SESSION_SH3_HAS_ACCESS) | LT_TO_MCOUNTER_12_15(LT_SESSION_SH3_HAS_ACCESS));

    //------- TR01_CFG_UAP_MAC_AND_DESTROY_ADDR -----------------------
    // Enable for all pairing key slots
    r_config->obj[TR01_CFG_UAP_MAC_AND_DESTROY_IDX] |= LT_TO_MACANDD_SLOT_0_31(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_MAC_AND_DESTROY_IDX] |= LT_TO_MACANDD_SLOT_32_63(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_MAC_AND_DESTROY_IDX] |= LT_TO_MACANDD_SLOT_64_95(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
    r_config->obj[TR01_CFG_UAP_MAC_AND_DESTROY_IDX] |= LT_TO_MACANDD_SLOT_96_127(
        LT_SESSION_SH0_HAS_ACCESS | LT_SESSION_SH1_HAS_ACCESS | LT_SESSION_SH2_HAS_ACCESS | LT_SESSION_SH3_HAS_ACCESS);
}

/**
 * @brief Initial session, when TROPIC01 is powered for the first time during manufacturing.
 *
 * This function does the following:
 * 1. Starts a Secure Session using the default SH0 keys.
 * 2. Reads the whole R-Config.
 * 3. Erases the whole R-Config.
 * 4. Writes example configuration to the R-Config.
 * 5. Writes new pairing keys to the pairing key slots 1,2,3.
 * 6. Invalidates the pairing key slot 0.
 * 7. Aborts the Secure Session.
 * 8. Reboots TROPIC01 to apply the changes.
 *
 * @param h  Handle for communication with TROPIC01
 * @return   0 if success, -1 otherwise
 */
static int session_initial(lt_handle_t *h)
{
    lt_ret_t ret;
    struct lt_config_t r_config;
    const uint8_t *pub_keys[] = {DEFAULT_SH0_PUB, sh1pub, sh2pub, sh3pub};

    printf("Starting Secure Session with key slot %d...", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    ret = lt_verify_chip_and_start_secure_session(h, DEFAULT_SH0_PRIV, DEFAULT_SH0_PUB, TR01_PAIRING_KEY_SLOT_INDEX_0);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to start Secure Session with key %d, ret=%s\n", (int)TR01_PAIRING_KEY_SLOT_INDEX_0,
                lt_ret_verbose(ret));
        fprintf(stderr,
                "Check if you use correct SH0 keys! Hint: if you use an engineering sample chip, compile with "
                "-DLT_SH0_KEYS=eng_sample\n");
        return -1;
    }
    printf("OK\n");

    printf("Reading the whole R-Config...");
    ret = lt_read_whole_R_config(h, &r_config);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to read R-Config, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(h);
        return -1;
    }
    printf("OK\n");

    printf("Current R-Config:\n");
    for (int i = 0; i < LT_CONFIG_OBJ_CNT; i++) {
        printf("\t%s: 0x%08" PRIx32 "\n", cfg_desc_table[i].desc, r_config.obj[i]);
    }

    printf("Erasing R-Config in case it is already written...");
    ret = lt_r_config_erase(h);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to erase R-Config, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(h);
        return -1;
    }
    printf("OK\n");

    printf("Example R-Config to be written:\n");
    create_example_r_config(&r_config);
    for (int i = 0; i < LT_CONFIG_OBJ_CNT; i++) {
        printf("\t%s: 0x%08" PRIx32 "\n", cfg_desc_table[i].desc, r_config.obj[i]);
    }

    printf("Writing the example R-Config...");
    ret = lt_write_whole_R_config(h, &r_config);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to write R-Config, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(h);
        return -1;
    }
    printf("OK\n");

    // Write pairing keys into slots 1,2,3
    printf("Will write new pairing keys to slots 1, 2 and 3:\n");
    for (uint8_t i = TR01_PAIRING_KEY_SLOT_INDEX_1; i <= TR01_PAIRING_KEY_SLOT_INDEX_3; i++) {
        printf("\tWriting to pairing key slot %" PRIu8 "...", i);
        ret = lt_pairing_key_write(h, pub_keys[i], i);
        if (LT_OK != ret) {
            fprintf(stderr, "\n\tFailed to write pairing key, ret=%s\n", lt_ret_verbose(ret));
            lt_session_abort(h);
            return -1;
        }
        printf("OK\n");
    }

    printf("Invalidating pairing key slot %d...", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    ret = lt_pairing_key_invalidate(h, TR01_PAIRING_KEY_SLOT_INDEX_0);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to invalidate pairing key slot, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(h);
        return -1;
    }
    printf("OK\n");

    printf("Aborting Secure Session...");
    ret = lt_session_abort(h);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to abort Secure Session, ret=%s\n", lt_ret_verbose(ret));
        return -1;
    }
    printf("OK\n");

    printf("Rebooting TROPIC01 to apply changes...");
    ret = lt_reboot(h, TR01_REBOOT);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to reboot, ret=%s\n", lt_ret_verbose(ret));
        return -1;
    }
    printf("OK\n");

    return 0;
}

/**
 * @brief Tries to start a Secure Session with previously invalidated pairing key slot 0 and checks that it fails.
 *
 * @param h  Handle for communication with TROPIC01
 * @return   0 if success, -1 otherwise
 */
static int session0(lt_handle_t *h)
{
    lt_ret_t ret;

    printf("Starting Secure Session with key slot %d (should fail)...", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    ret = lt_verify_chip_and_start_secure_session(h, DEFAULT_SH0_PRIV, DEFAULT_SH0_PUB, TR01_PAIRING_KEY_SLOT_INDEX_0);
    if (ret != LT_L2_HSK_ERR) {
        fprintf(stderr, "\nReturn value is not LT_L2_HSK_ERR, ret=%s\n", lt_ret_verbose(ret));
        return -1;
    }
    printf("OK\n");

    return 0;
}

/**
 * @brief This function does the following:
 * 1. Starts a Secure Session with the previously written pairing key slot 1.
 * 2. Sends Ping command.
 * 3. Stores an attestation key into ECC slot 0.
 * 4. Tries to write all pairing key slots (should fail due to unauthorized access).
 * 5. Aborts the Secure Session.
 *
 * @param h  Handle for communication with TROPIC01
 * @return   0 if success, -1 otherwise
 */
static int session1(lt_handle_t *h)
{
    lt_ret_t ret;

    printf("Starting Secure Session with key slot %d...", (int)TR01_PAIRING_KEY_SLOT_INDEX_1);
    ret = lt_verify_chip_and_start_secure_session(h, sh1priv, sh1pub, TR01_PAIRING_KEY_SLOT_INDEX_1);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to start Secure Session with key slot %d, ret=%s\n",
                (int)TR01_PAIRING_KEY_SLOT_INDEX_1, lt_ret_verbose(ret));
        return -1;
    }
    printf("OK\n");

    uint8_t recv_buf[PING_MSG_SIZE];
    printf("Sending Ping command...\n");
    printf("\t--> Message sent to TROPIC01: '%s'\n", PING_MSG);
    ret = lt_ping(h, (const uint8_t *)PING_MSG, recv_buf, PING_MSG_SIZE);
    if (LT_OK != ret) {
        fprintf(stderr, "Ping command failed, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(h);
        return -1;
    }
    printf("\t<-- Message received from TROPIC01: '%s'\n", recv_buf);

    printf("Storing attestation key into ECC slot %d...", (int)TR01_ECC_SLOT_0);
    ret = lt_ecc_key_store(h, TR01_ECC_SLOT_0, TR01_CURVE_ED25519, attestation_key);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to store ECC key to slot %d, ret=%s\n", (int)TR01_ECC_SLOT_0, lt_ret_verbose(ret));
        lt_session_abort(h);
        return -1;
    }
    printf("OK\n");

    uint8_t dummy_key[TR01_SHIPUB_LEN] = {0};
    printf("Will try to write all pairing key slots (should fail due to unauthorized access):\n");
    for (uint8_t i = TR01_PAIRING_KEY_SLOT_INDEX_0; i <= TR01_PAIRING_KEY_SLOT_INDEX_3; i++) {
        printf("\tWriting pairing key slot %" PRIu8 "...", i);
        ret = lt_pairing_key_write(h, dummy_key, i);
        if (LT_L3_UNAUTHORIZED != ret) {
            fprintf(stderr, "\n\tReturn value is not LT_L3_UNAUTHORIZED, ret=%s\n", lt_ret_verbose(ret));
            lt_session_abort(h);
            return -1;
        }
        printf("OK (failed)\n");
    }

    printf("Aborting Secure Session...");
    ret = lt_session_abort(h);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to abort Secure Session, ret=%s\n", lt_ret_verbose(ret));
        return -1;
    }
    printf("OK\n");

    return 0;
}

/**
 * @brief This function does the following:
 * 1. Starts a Secure Session with the previously written pairing key slot 2.
 * 2. Sends Ping command.
 * 3. Tries to store an ECC key into slot 0 (should fail due to unauthorized access).
 * 4. Tries to write all pairing key slots (should fail due to unauthorized access).
 * 5. Tries to initialize, update and get mcounter 0 (should fail due to unauthorized access).
 * 6. Aborts the Secure Session.
 *
 * @param h  Handle for communication with TROPIC01
 * @return   0 if success, -1 otherwise
 */
static int session2(lt_handle_t *h)
{
    lt_ret_t ret;

    printf("Starting Secure Session with key slot %d...\n", (int)TR01_PAIRING_KEY_SLOT_INDEX_2);
    ret = lt_verify_chip_and_start_secure_session(h, sh2priv, sh2pub, TR01_PAIRING_KEY_SLOT_INDEX_2);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to start Secure Session with key slot %d, ret=%s\n",
                (int)TR01_PAIRING_KEY_SLOT_INDEX_2, lt_ret_verbose(ret));
        return -1;
    }

    uint8_t recv_buf[PING_MSG_SIZE];
    printf("Sending Ping command...\n");
    printf("\t--> Message sent to TROPIC01: '%s'\n", PING_MSG);
    ret = lt_ping(h, (const uint8_t *)PING_MSG, recv_buf, PING_MSG_SIZE);
    if (LT_OK != ret) {
        fprintf(stderr, "Ping command failed, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(h);
        return -1;
    }
    printf("\t<-- Message received from TROPIC01: '%s'\n", recv_buf);

    uint8_t dummy_key[TR01_CURVE_PRIVKEY_LEN] = {0};
    printf("Trying to store key into ECC slot %d (should fail due to unauthorized access)...", (int)TR01_ECC_SLOT_0);
    ret = lt_ecc_key_store(h, TR01_ECC_SLOT_0, TR01_CURVE_ED25519, dummy_key);
    if (LT_L3_UNAUTHORIZED != ret) {
        fprintf(stderr, "\n\tReturn value is not LT_L3_UNAUTHORIZED, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(h);
        return -1;
    }
    printf("OK (failed)\n");

    printf("Will try to write all pairing key slots (should fail due to unauthorized access):\n");
    for (uint8_t i = TR01_PAIRING_KEY_SLOT_INDEX_0; i <= TR01_PAIRING_KEY_SLOT_INDEX_3; i++) {
        printf("\tWriting pairing key slot %" PRIu8 "...", i);
        ret = lt_pairing_key_write(h, dummy_key, i);
        if (LT_L3_UNAUTHORIZED != ret) {
            fprintf(stderr, "\n\tReturn value is not LT_L3_UNAUTHORIZED, ret=%s\n", lt_ret_verbose(ret));
            lt_session_abort(h);
            return -1;
        }
        printf("OK (failed)\n");
    }

    uint32_t mcounter_value = 0x000000ff;
    printf("Initializing mcounter 0 (should fail due to unauthorized access)...");
    ret = lt_mcounter_init(h, TR01_MCOUNTER_INDEX_0, mcounter_value);
    if (LT_L3_UNAUTHORIZED != ret) {
        fprintf(stderr, "\nReturn value is not LT_L3_UNAUTHORIZED, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(h);
        return -1;
    }
    printf("OK (failed)\n");

    printf("Updating mcounter 0 (should fail due to unauthorized access)...");
    ret = lt_mcounter_update(h, TR01_MCOUNTER_INDEX_0);
    if (LT_L3_UNAUTHORIZED != ret) {
        fprintf(stderr, "\nReturn value is not LT_L3_UNAUTHORIZED, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(h);
        return -1;
    }
    printf("OK (failed)\n");

    printf("Getting mcounter 0 (should fail due to unauthorized access)...");
    ret = lt_mcounter_get(h, TR01_MCOUNTER_INDEX_0, &mcounter_value);
    if (LT_L3_UNAUTHORIZED != ret) {
        fprintf(stderr, "\nReturn value is not LT_L3_UNAUTHORIZED, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(h);
        return -1;
    }
    printf("OK (failed)\n");

    printf("Aborting Secure Session...");
    ret = lt_session_abort(h);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to abort Secure Session, ret=%s\n", lt_ret_verbose(ret));
        return -1;
    }
    printf("OK\n");

    return 0;
}

/**
 * @brief This function does the following:
 * 1. Starts a Secure Session with the previously written pairing key slot 2.
 * 2. Sends Ping command.
 * 3. Signs a message with the attestation key stored in ECC slot 0.
 * 4. Reads the public key from ECC slot 0.
 * 5. Verifies the signature using an external ed25519 library.
 * 6. Generates new ECC keys into slots 8, 16 and 24.
 * 7. Gets random bytes from TROPIC01's TRNG.
 * 8. Initializes, updates and gets mcounter 0.
 * 9. Tries to store ECC key into slot 0 (should fail due to unauthorized access).
 * 10. Tries to write all pairing key slots (should fail due to unauthorized access).
 * 11. Aborts the Secure Session.
 *
 * @param h  Handle for communication with TROPIC01
 * @return   0 if success, -1 otherwise
 */
static int session3(lt_handle_t *h)
{
    lt_ret_t ret;

    printf("Starting Secure Session with key slot %d...", (int)TR01_PAIRING_KEY_SLOT_INDEX_3);
    ret = lt_verify_chip_and_start_secure_session(h, sh3priv, sh3pub, TR01_PAIRING_KEY_SLOT_INDEX_3);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to start Secure Session with key slot %d, ret=%s\n",
                (int)TR01_PAIRING_KEY_SLOT_INDEX_3, lt_ret_verbose(ret));
        return -1;
    }
    printf("OK\n");

    uint8_t recv_buf[PING_MSG_SIZE];
    printf("Sending Ping command...\n");
    printf("\t--> Message sent to TROPIC01: '%s'\n", PING_MSG);
    ret = lt_ping(h, (const uint8_t *)PING_MSG, recv_buf, PING_MSG_SIZE);
    if (LT_OK != ret) {
        fprintf(stderr, "Ping command failed, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(h);
        return -1;
    }
    printf("\t<-- Message received from TROPIC01: '%s'\n", recv_buf);

    printf("Signing with previously written attestation ECC key in slot %d...", (int)TR01_ECC_SLOT_0);
    uint8_t msg[] = {'a', 'h', 'o', 'j'};
    uint8_t rs[TR01_ECDSA_EDDSA_SIGNATURE_LENGTH];
    ret = lt_ecc_eddsa_sign(h, TR01_ECC_SLOT_0, msg, sizeof(msg), rs);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to sign, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(h);
        return -1;
    }
    printf("OK\n");

    printf("Reading ECC key slot %d...", (int)TR01_ECC_SLOT_0);
    uint8_t ed25519_pubkey[TR01_CURVE_ED25519_PUBKEY_LEN];
    lt_ecc_curve_type_t curve;
    lt_ecc_key_origin_t origin;
    ret = lt_ecc_key_read(h, TR01_ECC_SLOT_0, ed25519_pubkey, sizeof(ed25519_pubkey), &curve, &origin);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to read ECC slot, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(h);
        return -1;
    }
    printf("OK\n");

    // We use an external dependency here for signature verification.
    // See the example's CMakeLists.txt for more information about it.
    printf("Verifying signature using external ed25519 library...");
    if (!ed25519_verify(rs, msg, sizeof(msg), ed25519_pubkey)) {
        fprintf(stderr, "\nSignature verification failed!\n");
        lt_session_abort(h);
        return -1;
    }
    printf("OK\n");

    printf("Generating ECC key in slot %d...", (int)TR01_ECC_SLOT_8);
    ret = lt_ecc_key_generate(h, TR01_ECC_SLOT_8, TR01_CURVE_ED25519);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to generate ECC key, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(h);
        return -1;
    }
    printf("OK\n");

    printf("Generating ECC key in slot %d...", (int)TR01_ECC_SLOT_16);
    ret = lt_ecc_key_generate(h, TR01_ECC_SLOT_16, TR01_CURVE_ED25519);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to generate ECC key, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(h);
        return -1;
    }
    printf("OK\n");

    printf("Generating ECC key in slot %d...", (int)TR01_ECC_SLOT_24);
    ret = lt_ecc_key_generate(h, TR01_ECC_SLOT_24, TR01_CURVE_ED25519);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to generate ECC key, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(h);
        return -1;
    }
    printf("OK\n");

    printf("Getting %d random bytes...", (int)TR01_RANDOM_VALUE_GET_LEN_MAX);
    uint8_t buff[TR01_RANDOM_VALUE_GET_LEN_MAX];
    ret = lt_random_value_get(h, buff, TR01_RANDOM_VALUE_GET_LEN_MAX);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to get random bytes, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(h);
        return -1;
    }
    printf("OK\n");

    uint32_t mcounter_value = 0x000000ff;
    printf("Initializing mcounter 0...");
    ret = lt_mcounter_init(h, TR01_MCOUNTER_INDEX_0, mcounter_value);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to initialize mcounter, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(h);
        return -1;
    }
    printf("OK\n");

    printf("Updating mcounter 0...");
    ret = lt_mcounter_update(h, TR01_MCOUNTER_INDEX_0);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to update mcounter, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(h);
        return -1;
    }
    printf("OK\n");

    printf("Getting mcounter 0...");
    ret = lt_mcounter_get(h, TR01_MCOUNTER_INDEX_0, &mcounter_value);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to get mcounter, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(h);
        return -1;
    }
    printf("OK\n");

    uint8_t dummy_key[TR01_CURVE_PRIVKEY_LEN] = {0};
    printf("Trying to store key into ECC slot %d (should fail due to unauthorized access)...", (int)TR01_ECC_SLOT_0);
    ret = lt_ecc_key_store(h, TR01_ECC_SLOT_0, TR01_CURVE_ED25519, dummy_key);
    if (LT_L3_UNAUTHORIZED != ret) {
        fprintf(stderr, "\nReturn value is not LT_L3_UNAUTHORIZED, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(h);
        return -1;
    }
    printf("OK (failed)\n");

    printf("Will try to write all pairing key slots (should fail due to unauthorized access):\n");
    for (uint8_t i = TR01_PAIRING_KEY_SLOT_INDEX_0; i <= TR01_PAIRING_KEY_SLOT_INDEX_3; i++) {
        printf("\tWriting pairing key slot %" PRIu8 "...", i);
        ret = lt_pairing_key_write(h, dummy_key, i);
        if (LT_L3_UNAUTHORIZED != ret) {
            fprintf(stderr, "\n\tReturn value is not LT_L3_UNAUTHORIZED, ret=%s\n", lt_ret_verbose(ret));
            lt_session_abort(h);
            return -1;
        }
        printf("OK (failed)\n");
    }

    printf("Aborting Secure Session...");
    ret = lt_session_abort(h);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to abort Secure Session, ret=%s\n", lt_ret_verbose(ret));
        return -1;
    }
    printf("OK\n");

    return 0;
}

int main(void)
{
    // Cosmetics: Disable buffering to keep output in order. You do not need to do this in your app if you don't care about
    // stdout/stderr output being shuffled or you use stdout only (or different output mechanism altogether).
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    printf("==========================================\n");
    printf("==== TROPIC01 Hardware Wallet Example ====\n");
    printf("==========================================\n");

    // Cryptographic function provider initialization.
    //
    // In production, this would typically be done only once,
    // usually at the start of the application or before
    // the first use of cryptographic functions but no later than
    // the first occurrence of any Libtropic function
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        fprintf(stderr, "PSA Crypto initialization failed, status=%d (psa_status_t)\n", status);
        return -1;
    }

    // Libtropic handle.
    //
    // It is declared here (on stack) for
    // simplicity. In production, you put it on heap if needed.
    lt_handle_t lt_handle = {0};

    // Initialize device before handing handle to the test.
    lt_dev_posix_tcp_t device;
    device.addr = inet_addr("127.0.0.1");
    device.port = 28992;
    lt_handle.l2.device = &device;

    // Generate seed for the PRNG and seed it.
    // Note: model uses rand(), which is not cryptographically secure. Better alternative should be used in production.
    unsigned int prng_seed;
    if (0 != getentropy(&prng_seed, sizeof(prng_seed))) {
        fprintf(stderr, "main: getentropy() failed (%s)!\n", strerror(errno));
        mbedtls_psa_crypto_free();
        return -1;
    }
    srand(prng_seed);
    printf("PRNG initialized with seed=%u\n", prng_seed);

    // Crypto abstraction layer (CAL) context.
    lt_ctx_mbedtls_v4_t crypto_ctx;
    lt_handle.l3.crypto_ctx = &crypto_ctx;

    printf("Initializing handle...");
    lt_ret_t ret = lt_init(&lt_handle);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to initialize handle, ret=%s\n", lt_ret_verbose(ret));
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("OK\n");

    // We need to ensure we are not in the Start-up Mode, as L3 commands are available only in the Application Firmware.
    printf("Sending reboot request...");
    ret = lt_reboot(&lt_handle, TR01_REBOOT);
    if (ret != LT_OK) {
        fprintf(stderr, "\nlt_reboot() failed, ret=%s\n", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("OK\n");

    printf("\nExecuting initial example session...\n");
    if (session_initial(&lt_handle) != 0) {
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }

    printf("\nExecuting example session 0...\n");
    if (session0(&lt_handle) != 0) {
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }

    printf("\nExecuting example session 1...\n");
    if (session1(&lt_handle) != 0) {
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }

    printf("\nExecuting example session 2...\n");
    if (session2(&lt_handle) != 0) {
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }

    printf("\nExecuting example session 3...\n");
    if (session3(&lt_handle) != 0) {
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }

    printf("\nAll example sessions executed successfully!\n");

    printf("Deinitializing handle...");
    ret = lt_deinit(&lt_handle);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to deinitialize handle, ret=%s\n", lt_ret_verbose(ret));
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("OK\n");

    // Cryptographic function provider deinitialization.
    //
    // In production, this would be done only once, typically
    // during termination of the application.
    mbedtls_psa_crypto_free();

    return 0;
}