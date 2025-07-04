/**
 * @file lt_test_write_r_config.c
 * @brief Test function which writes testing r config
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "inttypes.h"
#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "string.h"

/** @brief  User Conﬁguration Objects - values defined here affect configuration of chip */
struct lt_config_t config = {
    .obj = {
        //-------CONFIGURATION_OBJECTS_CFG_START_UP------------------------------------
        // Enable checks on boot
        (CONFIGURATION_OBJECTS_CFG_START_UP_MBIST_DIS_MASK | CONFIGURATION_OBJECTS_CFG_START_UP_RNGTEST_DIS_MASK
         | CONFIGURATION_OBJECTS_CFG_START_UP_MAINTENANCE_ENA_MASK
         | CONFIGURATION_OBJECTS_CFG_START_UP_CPU_FW_VERIFY_DIS_MASK
         | CONFIGURATION_OBJECTS_CFG_START_UP_SPECT_FW_VERIFY_DIS_MASK),
        //-------CONFIGURATION_OBJECTS_CFG_SLEEP_MODE----------------------------------
        // Enable sleep mode only
        (  // CONFIGURATION_OBJECTS_CFG_SLEEP_MODE_SLEEP_MODE_EN_MASK
            0),
        //-------CONFIGURATION_OBJECTS_CFG_SENSORS-------------------------------------
        // Enable all sensors
        (CONFIGURATION_OBJECTS_CFG_SENSORS_PTRNG0_TEST_DIS_MASK | CONFIGURATION_OBJECTS_CFG_SENSORS_PTRNG1_TEST_DIS_MASK
         | CONFIGURATION_OBJECTS_CFG_SENSORS_OSCILLATOR_MON_DIS_MASK | CONFIGURATION_OBJECTS_CFG_SENSORS_SHIELD_DIS_MASK
         | CONFIGURATION_OBJECTS_CFG_SENSORS_VOLTAGE_MON_DIS_MASK
         | CONFIGURATION_OBJECTS_CFG_SENSORS_GLITCH_DET_DIS_MASK | CONFIGURATION_OBJECTS_CFG_SENSORS_TEMP_SENS_DIS_MASK
         | CONFIGURATION_OBJECTS_CFG_SENSORS_LASER_DET_DIS_MASK
         | CONFIGURATION_OBJECTS_CFG_SENSORS_EM_PULSE_DET_DIS_MASK
         | CONFIGURATION_OBJECTS_CFG_SENSORS_CPU_ALERT_DIS_MASK
         | CONFIGURATION_OBJECTS_CFG_SENSORS_PIN_VERIF_BIT_FLIP_DIS_MASK
         | CONFIGURATION_OBJECTS_CFG_SENSORS_SCB_BIT_FLIP_DIS_MASK
         | CONFIGURATION_OBJECTS_CFG_SENSORS_CPB_BIT_FLIP_DIS_MASK
         | CONFIGURATION_OBJECTS_CFG_SENSORS_ECC_BIT_FLIP_DIS_MASK
         | CONFIGURATION_OBJECTS_CFG_SENSORS_R_MEM_BIT_FLIP_DIS_MASK
         | CONFIGURATION_OBJECTS_CFG_SENSORS_EKDB_BIT_FLIP_DIS_MASK
         | CONFIGURATION_OBJECTS_CFG_SENSORS_I_MEM_BIT_FLIP_DIS_MASK
         | CONFIGURATION_OBJECTS_CFG_SENSORS_PLATFORM_BIT_FLIP_DIS_MASK),
        //------- CONFIGURATION_OBJECTS_CFG_DEBUG -------------------------------------
        // Enable TROPIC01's fw log
        (CONFIGURATION_OBJECTS_CFG_DEBUG_FW_LOG_EN_MASK),
        //------- CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_WRITE ---------------------
        // No session can write pairing keys
        0,
        //------- CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_READ ----------------------
        // All sessions can read pairing keys
        (TO_PAIRING_KEY_SH0(SESSION_SH0_HAS_ACCESS | SESSION_SH1_HAS_ACCESS | SESSION_SH2_HAS_ACCESS
                            | SESSION_SH3_HAS_ACCESS)
         | TO_PAIRING_KEY_SH1(SESSION_SH0_HAS_ACCESS | SESSION_SH1_HAS_ACCESS | SESSION_SH2_HAS_ACCESS
                              | SESSION_SH3_HAS_ACCESS)
         | TO_PAIRING_KEY_SH2(SESSION_SH0_HAS_ACCESS | SESSION_SH1_HAS_ACCESS | SESSION_SH2_HAS_ACCESS
                              | SESSION_SH3_HAS_ACCESS)
         | TO_PAIRING_KEY_SH3(SESSION_SH0_HAS_ACCESS | SESSION_SH1_HAS_ACCESS | SESSION_SH2_HAS_ACCESS
                              | SESSION_SH3_HAS_ACCESS)),
        //------- CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_INVALIDATE ----------------
        // Pairing key SH0PUB can be invalidated only from session SH0PUB
        // No session can invalidate pairing keys
        (0),
        //------- CONFIGURATION_OBJECTS_CFG_UAP_R_CONFIG_WRITE_ERASE ------------------
        // Reset value, all sessions has access
        0x000000ff,
        //------- CONFIGURATION_OBJECTS_CFG_UAP_R_CONFIG_READ -------------------------
        // Reset value, all sessions has access
        0x0000ffff,
        //------- CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_WRITE ------------------------
        // We do not plan to write I-config in this example
        0x00000000,
        //------- CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_READ -------------------------
        // Reset value, all sessions have access
        0x0000ffff,
        //------- CONFIGURATION_OBJECTS_CFG_UAP_PING ----------------------------------
        // Reset value, Ping command is available for all session keys
        0x000000ff,
        //------- CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_WRITE ----------------------
        // Reset value, all sessions have access
        0xffffffff,
        //------- CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_READ -----------------------
        // Reset value, all sessions have access
        0xffffffff,
        //------- CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_ERASE ----------------------
        // Reset value, all sessions have access
        0xffffffff,
        //------- CONFIGURATION_OBJECTS_CFG_UAP_RANDOM_VALUE_GET ----------------------
        // Reset value, all sessions have access
        0x000000ff,
        //------- CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_GENERATE ----------------------
        // Only session SH0 can generate key in slots 0-31
        (TO_ECC_KEY_SLOT_0_7(SESSION_SH0_HAS_ACCESS) | TO_ECC_KEY_SLOT_8_15(SESSION_SH0_HAS_ACCESS)
         | TO_ECC_KEY_SLOT_16_23(SESSION_SH0_HAS_ACCESS) | TO_ECC_KEY_SLOT_24_31(SESSION_SH0_HAS_ACCESS)),
        //------- CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_STORE -------------------------
        // Only session SH0 can store key into ecc key slot 0-31
        (TO_ECC_KEY_SLOT_0_7(SESSION_SH0_HAS_ACCESS) | TO_ECC_KEY_SLOT_8_15(SESSION_SH0_HAS_ACCESS)
         | TO_ECC_KEY_SLOT_16_23(SESSION_SH0_HAS_ACCESS) | TO_ECC_KEY_SLOT_24_31(SESSION_SH0_HAS_ACCESS)),
        //------- CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_READ --------------------------
        // Only session SH0 can read pubkey from slot 0-31
        (TO_ECC_KEY_SLOT_0_7(SESSION_SH0_HAS_ACCESS) | TO_ECC_KEY_SLOT_8_15(SESSION_SH0_HAS_ACCESS)
         | TO_ECC_KEY_SLOT_16_23(SESSION_SH0_HAS_ACCESS) | TO_ECC_KEY_SLOT_24_31(SESSION_SH0_HAS_ACCESS)),
        //------- CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_ERASE -------------------------
        // Only session SH0 can erase keys from slot 0-31
        (TO_ECC_KEY_SLOT_0_7(SESSION_SH0_HAS_ACCESS) | TO_ECC_KEY_SLOT_8_15(SESSION_SH0_HAS_ACCESS)
         | TO_ECC_KEY_SLOT_16_23(SESSION_SH0_HAS_ACCESS) | TO_ECC_KEY_SLOT_24_31(SESSION_SH0_HAS_ACCESS)),
        //------- CONFIGURATION_OBJECTS_CFG_UAP_ECDSA_SIGN ----------------------------
        // Only session SH0 can ECDSA sign with keys from slot 0-31
        (TO_ECC_KEY_SLOT_0_7(SESSION_SH0_HAS_ACCESS) | TO_ECC_KEY_SLOT_8_15(SESSION_SH0_HAS_ACCESS)
         | TO_ECC_KEY_SLOT_16_23(SESSION_SH0_HAS_ACCESS) | TO_ECC_KEY_SLOT_24_31(SESSION_SH0_HAS_ACCESS)),
        //------- CONFIGURATION_OBJECTS_CFG_UAP_EDDSA_SIGN ----------------------------
        // Only session SH0 can EDDSA sign with keys from slot 0-31
        (TO_ECC_KEY_SLOT_0_7(SESSION_SH0_HAS_ACCESS) | TO_ECC_KEY_SLOT_8_15(SESSION_SH0_HAS_ACCESS)
         | TO_ECC_KEY_SLOT_16_23(SESSION_SH0_HAS_ACCESS) | TO_ECC_KEY_SLOT_24_31(SESSION_SH0_HAS_ACCESS)),
        //------- CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_INIT -------------------------
        // SH0 can init all counters
        (TO_LT_MCOUNTER_0_3(SESSION_SH0_HAS_ACCESS) | TO_LT_MCOUNTER_4_7(SESSION_SH0_HAS_ACCESS)
         | TO_LT_MCOUNTER_8_11(SESSION_SH0_HAS_ACCESS) | TO_LT_MCOUNTER_12_15(SESSION_SH0_HAS_ACCESS)),
        //------- CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_GET --------------------------
        // SH0 can get all counters
        (TO_LT_MCOUNTER_0_3(SESSION_SH0_HAS_ACCESS) | TO_LT_MCOUNTER_4_7(SESSION_SH0_HAS_ACCESS)
         | TO_LT_MCOUNTER_8_11(SESSION_SH0_HAS_ACCESS) | TO_LT_MCOUNTER_12_15(SESSION_SH0_HAS_ACCESS)),
        //------- CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_UPDATE -----------------------
        // SH0 can update all counters
        (TO_LT_MCOUNTER_0_3(SESSION_SH0_HAS_ACCESS) | TO_LT_MCOUNTER_4_7(SESSION_SH0_HAS_ACCESS)
         | TO_LT_MCOUNTER_8_11(SESSION_SH0_HAS_ACCESS) | TO_LT_MCOUNTER_12_15(SESSION_SH0_HAS_ACCESS)),
        //------- CONFIGURATION_OBJECTS_TODO -----------------------
        // All slots can use MAC-and-destroy
        (0xffffffff),
        //------- CONFIGURATION_OBJECTS_CFG_UAP_SERIAL_CODE_GET -----------------------
        (0x000000ff),
    }};

/**
 * @brief
 *
 * @return int
 */
int lt_test_write_r_config(void)
{
    LT_LOG(
        "  "
        "------------------------------------------------------------------------------------------------------------"
        "-");
    LT_LOG(
        "  -------- lt_test_write_r_config() "
        "-------------------------------------------------------------------------------------");
    LT_LOG(
        "  "
        "------------------------------------------------------------------------------------------------------------"
        "-");

    lt_handle_t h = {0};
#if LT_SEPARATE_L3_BUFF
    uint8_t l3_buffer[L3_FRAME_MAX_SIZE] __attribute__((aligned(16))) = {0};
    h.l3.buff = l3_buffer;
    h.l3.buff_len = sizeof(l3_buffer);
#endif

    LT_LOG("%s", "Initialize handle");
    LT_ASSERT(LT_OK, lt_init(&h));
    LT_LOG("%s with %d", "verify_chip_and_start_secure_session() with key H", PAIRING_KEY_SLOT_INDEX_0);
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, PAIRING_KEY_SLOT_INDEX_0));

    LT_LOG("%s", "R CONFIG read:");
    struct lt_config_t r_config_read = {0};
    LT_ASSERT(LT_OK, read_whole_R_config(&h, &r_config_read));
    // Print r config
    for (int i = 0; i < 27; i++) {
        LT_LOG("    %s,  %08" PRIX32, get_conf_desc(i), r_config_read.obj[i]);
    }

    LT_LOG("%s", "write_whole_R_config()");
    LT_ASSERT(LT_OK, write_whole_R_config(&h, &config));

    LT_LOG("%s", "R CONFIG read again:");
    LT_ASSERT(LT_OK, read_whole_R_config(&h, &r_config_read));
    // Print r config
    for (int i = 0; i < 27; i++) {
        LT_LOG("    %s,  %08" PRIX32, get_conf_desc(i), r_config_read.obj[i]);
    }

    LT_LOG("%s", "lt_reboot(LT_MODE_APP)");
    LT_ASSERT(LT_OK, lt_reboot(&h, LT_MODE_APP));

    LT_LOG("%s", "lt_deinit()");
    LT_ASSERT(LT_OK, lt_deinit(&h));

    return 0;
}
