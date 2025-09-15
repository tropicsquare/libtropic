/**
 * @file lt_ex_hw_wallet.c
 * @brief Example usage of TROPIC01 chip in a generic *hardware wallet* project.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <inttypes.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_examples.h"
#include "libtropic_logging.h"
#include "string.h"

/** @brief Message to send with Ping L3 command. */
#define PING_MSG "Ping message for TROPIC01"
/** @brief Size of the Ping message, including '\0'. */
#define PING_MSG_SIZE 26

/** @brief Attestation key for ECC slot 0. */
uint8_t attestation_key[32]
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
 * @brief Initial session, when chip is powered for the first time during manufacturing.
 *        This function writes chip's configuration into R config.
 *
 * @param h  Device's handle
 * @return   0 if success, -1 otherwise
 */
static int session_initial(lt_handle_t *h)
{
    lt_ret_t ret;
    struct lt_config_t r_config;
    uint8_t *pub_keys[] = {sh0pub, sh1pub, sh2pub, sh3pub};

    LT_LOG_INFO("Initializing handle");
    ret = lt_init(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to initialize handle, ret=%s", lt_ret_verbose(ret));
        return -1;
    }

    LT_LOG_INFO("Starting Secure Session with key %d", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    ret = lt_verify_chip_and_start_secure_session(h, sh0priv, sh0pub, TR01_PAIRING_KEY_SLOT_INDEX_0);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to start Secure Session with key %d, ret=%s", (int)TR01_PAIRING_KEY_SLOT_INDEX_0,
                     lt_ret_verbose(ret));
        return -1;
    }

    LT_LOG_INFO("Reading the whole R config:");
    ret = lt_read_whole_R_config(h, &r_config);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to read R config, ret=%s", lt_ret_verbose(ret));
        return -1;
    }
    for (int i = 0; i < LT_CONFIG_OBJ_CNT; i++) {
        LT_LOG_INFO("%s: 0x%08" PRIx32, cfg_desc_table[i].desc, r_config.obj[i]);
    }

    LT_LOG_INFO("Creating an example config from the read R config...");
    create_example_r_config(&r_config);

    LT_LOG_INFO("Erasing R config in case it is already written...");
    ret = lt_r_config_erase(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to erase R config, ret=%s", lt_ret_verbose(ret));
        return -1;
    }
    LT_LOG_INFO("\tOK");

    LT_LOG_INFO("Writing the whole R config with the example config...");
    ret = lt_write_whole_R_config(h, &r_config);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to write R config, ret=%s", lt_ret_verbose(ret));
        return -1;
    }
    LT_LOG_INFO("\tOK");

    LT_LOG_INFO("Reading the whole R config again:");
    ret = lt_read_whole_R_config(h, &r_config);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to read R config, ret=%s", lt_ret_verbose(ret));
        return -1;
    }
    for (int i = 0; i < LT_CONFIG_OBJ_CNT; i++) {
        LT_LOG_INFO("%s: 0x%08" PRIx32, cfg_desc_table[i].desc, r_config.obj[i]);
    }

    // Write pairing keys into slots 1,2,3
    for (uint8_t i = TR01_PAIRING_KEY_SLOT_INDEX_1; i <= TR01_PAIRING_KEY_SLOT_INDEX_3; i++) {
        LT_LOG_INFO("Writing to pairing key slot %" PRIu8 "...", i);
        ret = lt_pairing_key_write(h, pub_keys[i], i);
        if (LT_OK != ret) {
            LT_LOG_ERROR("Failed to write pairing key, ret=%s", lt_ret_verbose(ret));
            return -1;
        }
        LT_LOG_INFO("\tOK");
    }

    LT_LOG_INFO("Invalidating pairing key slot %d...", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    ret = lt_pairing_key_invalidate(h, TR01_PAIRING_KEY_SLOT_INDEX_0);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to invalidate pairing key slot, ret=%s", lt_ret_verbose(ret));
        return -1;
    }
    LT_LOG_INFO("\tOK");

    LT_LOG_INFO("Aborting Secure Session");
    ret = lt_session_abort(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to abort Secure Session, ret=%s", lt_ret_verbose(ret));
        return -1;
    }

    LT_LOG_INFO("Rebooting TROPIC01 to apply changes...");
    ret = lt_reboot(h, TR01_MODE_APP);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to reboot, ret=%s", lt_ret_verbose(ret));
        return -1;
    }
    LT_LOG_INFO("\tOK");

    LT_LOG_INFO("Deinitializing handle");
    ret = lt_deinit(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to deinitialize handle, ret=%s", lt_ret_verbose(ret));
        return -1;
    }

    return 0;
}

/**
 * @brief Session with pairing key slot 0
 *
 * @param h  Device's handle
 * @return   0 if success, -1 otherwise
 */
static int session0(lt_handle_t *h)
{
    lt_ret_t ret;

    LT_LOG_INFO("Initializing handle");
    ret = lt_init(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to initialize handle, ret=%s", lt_ret_verbose(ret));
        return -1;
    }

    LT_LOG_INFO("Starting Secure Session with key %d (should fail)", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    ret = lt_verify_chip_and_start_secure_session(h, sh0priv, sh0pub, TR01_PAIRING_KEY_SLOT_INDEX_0);
    if (LT_L2_HSK_ERR != ret) {
        LT_LOG_ERROR("Return value is not LT_L2_HSK_ERR, ret=%s", lt_ret_verbose(ret));
        return -1;
    }
    LT_LOG_INFO("\tOK");

    LT_LOG_INFO("Deinitializing handle");
    ret = lt_deinit(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to deinitialize handle, ret=%s", lt_ret_verbose(ret));
        return -1;
    }

    return 0;
}

/**
 * @brief Session with pairing key slot 1
 *
 * @param h  Device's handle
 * @return   0 if success, -1 otherwise
 */
static int session1(lt_handle_t *h)
{
    lt_ret_t ret;

    LT_LOG_INFO("Initializing handle");
    ret = lt_init(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to initialize handle, ret=%s", lt_ret_verbose(ret));
        return -1;
    }

    LT_LOG_INFO("Starting Secure Session with key %d", (int)TR01_PAIRING_KEY_SLOT_INDEX_1);
    ret = lt_verify_chip_and_start_secure_session(h, sh1priv, sh1pub, TR01_PAIRING_KEY_SLOT_INDEX_1);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to start Secure Session with key %d, ret=%s", (int)TR01_PAIRING_KEY_SLOT_INDEX_1,
                     lt_ret_verbose(ret));
        return -1;
    }

    uint8_t recv_buf[PING_MSG_SIZE];
    LT_LOG_INFO("Sending Ping command with message:");
    LT_LOG_INFO("\t\"%s\"", PING_MSG);
    ret = lt_ping(h, (const uint8_t *)PING_MSG, recv_buf, PING_MSG_SIZE);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Ping command failed, ret=%s", lt_ret_verbose(ret));
        return -1;
    }

    LT_LOG_INFO("Message received from TROPIC01:");
    LT_LOG_INFO("\t\"%s\"", recv_buf);

    LT_LOG_INFO("Storing attestation key into ECC slot %d...", (int)TR01_ECC_SLOT_0);
    ret = lt_ecc_key_store(h, TR01_ECC_SLOT_0, TR01_CURVE_ED25519, attestation_key);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to store ECC key to slot %d, ret=%s", (int)TR01_ECC_SLOT_0, lt_ret_verbose(ret));
        return -1;
    }
    LT_LOG_INFO("\tOK");

    uint8_t dummy_key[32] = {0};
    LT_LOG_INFO("Writing all pairing key slots (should fail):");
    for (uint8_t i = TR01_PAIRING_KEY_SLOT_INDEX_0; i <= TR01_PAIRING_KEY_SLOT_INDEX_3; i++) {
        LT_LOG_INFO("\tWriting pairing key slot %" PRIu8 "...", i);
        ret = lt_pairing_key_write(h, dummy_key, i);
        if (LT_L3_UNAUTHORIZED != ret) {
            LT_LOG_ERROR("Return value is not LT_L3_UNAUTHORIZED, ret=%s", lt_ret_verbose(ret));
            return -1;
        }
        LT_LOG_INFO("\t\tOK");
    }

    LT_LOG_INFO("Aborting Secure Session");
    ret = lt_session_abort(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to abort Secure Session, ret=%s", lt_ret_verbose(ret));
        return -1;
    }

    LT_LOG_INFO("Deinitializing handle");
    ret = lt_deinit(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to deinitialize handle, ret=%s", lt_ret_verbose(ret));
        return -1;
    }

    return 0;
}

/**
 * @brief Session with pairing key slot 2
 *
 * @param h  Device's handle
 * @return   0 if success, -1 otherwise
 */
static int session2(lt_handle_t *h)
{
    lt_ret_t ret;

    LT_LOG_INFO("Initializing handle");
    ret = lt_init(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to initialize handle, ret=%s", lt_ret_verbose(ret));
        return -1;
    }

    LT_LOG_INFO("Starting Secure Session with key %d", (int)TR01_PAIRING_KEY_SLOT_INDEX_2);
    ret = lt_verify_chip_and_start_secure_session(h, sh2priv, sh2pub, TR01_PAIRING_KEY_SLOT_INDEX_2);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to start Secure Session with key %d, ret=%s", (int)TR01_PAIRING_KEY_SLOT_INDEX_2,
                     lt_ret_verbose(ret));
        return -1;
    }

    uint8_t recv_buf[PING_MSG_SIZE];
    LT_LOG_INFO("Sending Ping command with message:");
    LT_LOG_INFO("\t\"%s\"", PING_MSG);
    ret = lt_ping(h, (const uint8_t *)PING_MSG, recv_buf, PING_MSG_SIZE);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Ping command failed, ret=%s", lt_ret_verbose(ret));
        return -1;
    }

    LT_LOG_INFO("Message received from TROPIC01:");
    LT_LOG_INFO("\t\"%s\"", recv_buf);

    uint8_t dummy_key[32] = {0};
    LT_LOG_INFO("Trying to store key into ECC slot %d (should fail)", (int)TR01_ECC_SLOT_0);
    ret = lt_ecc_key_store(h, TR01_ECC_SLOT_0, TR01_CURVE_ED25519, dummy_key);
    if (LT_L3_UNAUTHORIZED != ret) {
        LT_LOG_ERROR("Return value is not LT_L3_UNAUTHORIZED, ret=%s", lt_ret_verbose(ret));
        return -1;
    }
    LT_LOG_INFO("\tOK");

    LT_LOG_INFO("Writing all pairing key slots (should fail):");
    for (uint8_t i = TR01_PAIRING_KEY_SLOT_INDEX_0; i <= TR01_PAIRING_KEY_SLOT_INDEX_3; i++) {
        LT_LOG_INFO("\tWriting pairing key slot %" PRIu8 "...", i);
        ret = lt_pairing_key_write(h, dummy_key, i);
        if (LT_L3_UNAUTHORIZED != ret) {
            LT_LOG_ERROR("Return value is not LT_L3_UNAUTHORIZED, ret=%s", lt_ret_verbose(ret));
            return -1;
        }
        LT_LOG_INFO("\t\tOK");
    }

    uint32_t mcounter_value = 0x000000ff;
    LT_LOG_INFO("Initializing mcounter 0 (should fail)...");
    ret = lt_mcounter_init(h, 0, mcounter_value);
    if (LT_L3_UNAUTHORIZED != ret) {
        LT_LOG_ERROR("Return value is not LT_L3_UNAUTHORIZED, ret=%s", lt_ret_verbose(ret));
        return -1;
    }
    LT_LOG_INFO("\tOK");

    LT_LOG_INFO("Updating mcounter 0 (should fail)...");
    ret = lt_mcounter_update(h, 0);
    if (LT_L3_UNAUTHORIZED != ret) {
        LT_LOG_ERROR("Return value is not LT_L3_UNAUTHORIZED, ret=%s", lt_ret_verbose(ret));
        return -1;
    }
    LT_LOG_INFO("\tOK");

    LT_LOG_INFO("Getting mcounter 0 (should fail)...");
    ret = lt_mcounter_get(h, 0, &mcounter_value);
    if (LT_L3_UNAUTHORIZED != ret) {
        LT_LOG_ERROR("Return value is not LT_L3_UNAUTHORIZED, ret=%s", lt_ret_verbose(ret));
        return -1;
    }
    LT_LOG_INFO("\tOK");

    LT_LOG_INFO("Aborting Secure Session");
    ret = lt_session_abort(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to abort Secure Session, ret=%s", lt_ret_verbose(ret));
        return -1;
    }

    LT_LOG_INFO("Deinitializing handle");
    ret = lt_deinit(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to deinitialize handle, ret=%s", lt_ret_verbose(ret));
        return -1;
    }

    return 0;
}

/**
 * @brief Session with pairing key slot 3
 *
 * @param h  Device's handle
 * @return   0 if success, -1 otherwise
 */
static int session3(lt_handle_t *h)
{
    lt_ret_t ret;

    LT_LOG_INFO("Initializing handle");
    ret = lt_init(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to initialize handle, ret=%s", lt_ret_verbose(ret));
        return -1;
    }

    LT_LOG_INFO("Starting Secure Session with key %d", (int)TR01_PAIRING_KEY_SLOT_INDEX_3);
    ret = lt_verify_chip_and_start_secure_session(h, sh3priv, sh3pub, TR01_PAIRING_KEY_SLOT_INDEX_3);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to start Secure Session with key %d, ret=%s", (int)TR01_PAIRING_KEY_SLOT_INDEX_3,
                     lt_ret_verbose(ret));
        return -1;
    }

    uint8_t recv_buf[PING_MSG_SIZE];
    LT_LOG_INFO("Sending Ping command with message:");
    LT_LOG_INFO("\t\"%s\"", PING_MSG);
    ret = lt_ping(h, (const uint8_t *)PING_MSG, recv_buf, PING_MSG_SIZE);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Ping command failed, ret=%s", lt_ret_verbose(ret));
        return -1;
    }

    LT_LOG_INFO("Message received from TROPIC01:");
    LT_LOG_INFO("\t\"%s\"", recv_buf);

    LT_LOG_INFO("Signing with attestation key which was updated through pairing key slot 1");
    uint8_t msg[] = {'a', 'h', 'o', 'j'};
    uint8_t rs[64];
    ret = lt_ecc_eddsa_sign(h, TR01_ECC_SLOT_0, msg, 4, rs);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to sign, ret=%s", lt_ret_verbose(ret));
        return -1;
    }
    LT_LOG_INFO("\tOK");

    LT_LOG_INFO("Reading ECC key slot %d...", (int)TR01_ECC_SLOT_0);
    uint8_t slot_0_pubkey[64];
    lt_ecc_curve_type_t curve;
    lt_ecc_key_origin_t origin;
    ret = lt_ecc_key_read(h, TR01_ECC_SLOT_0, slot_0_pubkey, &curve, &origin);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to read ECC slot, ret=%s", lt_ret_verbose(ret));
        return -1;
    }
    LT_LOG_INFO("\tOK");

    LT_LOG_INFO("Verifying with lt_ecc_eddsa_sig_verify()...");
    ret = lt_ecc_eddsa_sig_verify(msg, 4, slot_0_pubkey, rs);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to verify, ret%s", lt_ret_verbose(ret));
        return -1;
    }
    LT_LOG_INFO("\tOK");

    LT_LOG_INFO("Generating ECC key in slot %d...", (int)TR01_ECC_SLOT_8);
    ret = lt_ecc_key_generate(h, TR01_ECC_SLOT_8, TR01_CURVE_ED25519);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to generate ECC key, ret=%s", lt_ret_verbose(ret));
        return -1;
    }
    LT_LOG_INFO("\tOK");

    LT_LOG_INFO("Generating ECC key in slot %d...", (int)TR01_ECC_SLOT_16);
    ret = lt_ecc_key_generate(h, TR01_ECC_SLOT_16, TR01_CURVE_ED25519);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to generate ECC key, ret=%s", lt_ret_verbose(ret));
        return -1;
    }
    LT_LOG_INFO("\tOK");

    LT_LOG_INFO("Generating ECC key in slot %d...", (int)TR01_ECC_SLOT_24);
    ret = lt_ecc_key_generate(h, TR01_ECC_SLOT_24, TR01_CURVE_ED25519);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to generate ECC key, ret=%s", lt_ret_verbose(ret));
        return -1;
    }
    LT_LOG_INFO("\tOK");

    LT_LOG_INFO("Getting %d random bytes...", (int)TR01_RANDOM_VALUE_GET_LEN_MAX);
    uint8_t buff[TR01_RANDOM_VALUE_GET_LEN_MAX];
    ret = lt_random_value_get(h, buff, TR01_RANDOM_VALUE_GET_LEN_MAX);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to get random bytes, ret=%s", lt_ret_verbose(ret));
        return -1;
    }
    LT_LOG_INFO("\tOK");

    uint32_t mcounter_value = 0x000000ff;
    LT_LOG_INFO("Initializing mcounter 0...");
    ret = lt_mcounter_init(h, 0, mcounter_value);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to initialize mcounter, ret=%s", lt_ret_verbose(ret));
        return -1;
    }
    LT_LOG_INFO("\tOK");

    LT_LOG_INFO("Updating mcounter 0...");
    ret = lt_mcounter_update(h, 0);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to update mcounter, ret=%s", lt_ret_verbose(ret));
        return -1;
    }
    LT_LOG_INFO("\tOK");

    LT_LOG_INFO("Getting mcounter 0...");
    ret = lt_mcounter_get(h, 0, &mcounter_value);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to get mcounter, ret=%s", lt_ret_verbose(ret));
        return -1;
    }
    LT_LOG_INFO("\tOK");

    uint8_t dummy_key[32];
    LT_LOG_INFO("Trying to store key into ECC slot %d (should fail)", (int)TR01_ECC_SLOT_0);
    ret = lt_ecc_key_store(h, TR01_ECC_SLOT_0, TR01_CURVE_ED25519, dummy_key);
    if (LT_L3_UNAUTHORIZED != ret) {
        LT_LOG_ERROR("Return value is not LT_L3_UNAUTHORIZED, ret=%s", lt_ret_verbose(ret));
        return -1;
    }
    LT_LOG_INFO("\tOK");

    LT_LOG_INFO("Writing all pairing key slots (should fail):");
    for (uint8_t i = TR01_PAIRING_KEY_SLOT_INDEX_0; i <= TR01_PAIRING_KEY_SLOT_INDEX_3; i++) {
        LT_LOG_INFO("\tWriting pairing key slot %" PRIu8 "...", i);
        ret = lt_pairing_key_write(h, dummy_key, i);
        if (LT_L3_UNAUTHORIZED != ret) {
            LT_LOG_ERROR("Return value is not LT_L3_UNAUTHORIZED, ret=%s", lt_ret_verbose(ret));
            return -1;
        }
        LT_LOG_INFO("\t\tOK");
    }

    LT_LOG_INFO("Aborting Secure Session");
    ret = lt_session_abort(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to abort Secure Session, ret=%s", lt_ret_verbose(ret));
        return -1;
    }

    LT_LOG_INFO("Deinitializing handle");
    ret = lt_deinit(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to deinitialize handle, ret=%s", lt_ret_verbose(ret));
        return -1;
    }

    return 0;
}

int lt_ex_hardware_wallet(lt_handle_t *h)
{
    LT_LOG_INFO("==========================================");
    LT_LOG_INFO("==== TROPIC01 Hardware Wallet Example ====");
    LT_LOG_INFO("==========================================");

    LT_LOG_LINE();
    LT_LOG_INFO("Initial session with pairing key slot 0");
    if (session_initial(h) == -1) {
        if (h->l3.session == LT_SECURE_SESSION_ON) lt_session_abort(h);
        lt_deinit(h);
        return -1;
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Session with pairing key slot 0");
    if (session0(h) == -1) {
        if (h->l3.session == LT_SECURE_SESSION_ON) lt_session_abort(h);
        lt_deinit(h);
        return -1;
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Session with pairing key slot 1");
    if (session1(h) == -1) {
        if (h->l3.session == LT_SECURE_SESSION_ON) lt_session_abort(h);
        lt_deinit(h);
        return -1;
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Session with pairing key slot 2");
    if (session2(h) == -1) {
        if (h->l3.session == LT_SECURE_SESSION_ON) lt_session_abort(h);
        lt_deinit(h);
        return -1;
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Session with pairing key slot 3");
    if (session3(h) == -1) {
        if (h->l3.session == LT_SECURE_SESSION_ON) lt_session_abort(h);
        lt_deinit(h);
        return -1;
    }

    return 0;
}
