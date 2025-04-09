

/**
 * @file lt_test_reversible.c
 * @brief This function executes all reversible l3 commands. Its purpose is to run during platform's test, must not brick the chip.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "inttypes.h"
#include "string.h"

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_examples.h"


/** @brief  User Conﬁguration Objects - values defined here affect configuration of chip */
struct lt_config_t config = {.obj={
    //-------CONFIGURATION_OBJECTS_CFG_START_UP------------------------------------
    // Enable checks on boot
        (CONFIGURATION_OBJECTS_CFG_START_UP_MBIST_DIS_MASK |
         CONFIGURATION_OBJECTS_CFG_START_UP_RNGTEST_DIS_MASK |
         CONFIGURATION_OBJECTS_CFG_START_UP_MAINTENANCE_ENA_MASK |
         CONFIGURATION_OBJECTS_CFG_START_UP_CPU_FW_VERIFY_DIS_MASK |
         CONFIGURATION_OBJECTS_CFG_START_UP_SPECT_FW_VERIFY_DIS_MASK),
    //-------CONFIGURATION_OBJECTS_CFG_SLEEP_MODE----------------------------------
    // Enable sleep mode only
        (//CONFIGURATION_OBJECTS_CFG_SLEEP_MODE_SLEEP_MODE_EN_MASK
        0),
    //-------CONFIGURATION_OBJECTS_CFG_SENSORS-------------------------------------
    // Enable all sensors
        (CONFIGURATION_OBJECTS_CFG_SENSORS_PTRNG0_TEST_DIS_MASK |
        CONFIGURATION_OBJECTS_CFG_SENSORS_PTRNG1_TEST_DIS_MASK |
        CONFIGURATION_OBJECTS_CFG_SENSORS_OSCILLATOR_MON_DIS_MASK |
        CONFIGURATION_OBJECTS_CFG_SENSORS_SHIELD_DIS_MASK |
        CONFIGURATION_OBJECTS_CFG_SENSORS_VOLTAGE_MON_DIS_MASK |
        CONFIGURATION_OBJECTS_CFG_SENSORS_GLITCH_DET_DIS_MASK |
        CONFIGURATION_OBJECTS_CFG_SENSORS_TEMP_SENS_DIS_MASK |
        CONFIGURATION_OBJECTS_CFG_SENSORS_LASER_DET_DIS_MASK |
        CONFIGURATION_OBJECTS_CFG_SENSORS_EM_PULSE_DET_DIS_MASK |
        CONFIGURATION_OBJECTS_CFG_SENSORS_CPU_ALERT_DIS_MASK |
        CONFIGURATION_OBJECTS_CFG_SENSORS_PIN_VERIF_BIT_FLIP_DIS_MASK |
        CONFIGURATION_OBJECTS_CFG_SENSORS_SCB_BIT_FLIP_DIS_MASK |
        CONFIGURATION_OBJECTS_CFG_SENSORS_CPB_BIT_FLIP_DIS_MASK |
        CONFIGURATION_OBJECTS_CFG_SENSORS_ECC_BIT_FLIP_DIS_MASK |
        CONFIGURATION_OBJECTS_CFG_SENSORS_R_MEM_BIT_FLIP_DIS_MASK |
        CONFIGURATION_OBJECTS_CFG_SENSORS_EKDB_BIT_FLIP_DIS_MASK |
        CONFIGURATION_OBJECTS_CFG_SENSORS_I_MEM_BIT_FLIP_DIS_MASK |
        CONFIGURATION_OBJECTS_CFG_SENSORS_PLATFORM_BIT_FLIP_DIS_MASK
        ),
    //------- CONFIGURATION_OBJECTS_CFG_DEBUG -------------------------------------
    // Enable TROPIC01's fw log
    (CONFIGURATION_OBJECTS_CFG_DEBUG_FW_LOG_EN_MASK),
    //------- CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_WRITE ---------------------
    // No session can write pairing keys
    0,
    //------- CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_READ ----------------------
    // All sessions can read pairing keys
        (TO_PAIRING_KEY_SH0(SESSION_SH0_HAS_ACCESS | SESSION_SH1_HAS_ACCESS | SESSION_SH2_HAS_ACCESS | SESSION_SH3_HAS_ACCESS) |
         TO_PAIRING_KEY_SH1(SESSION_SH0_HAS_ACCESS | SESSION_SH1_HAS_ACCESS | SESSION_SH2_HAS_ACCESS | SESSION_SH3_HAS_ACCESS) |
         TO_PAIRING_KEY_SH2(SESSION_SH0_HAS_ACCESS | SESSION_SH1_HAS_ACCESS | SESSION_SH2_HAS_ACCESS | SESSION_SH3_HAS_ACCESS) |
         TO_PAIRING_KEY_SH3(SESSION_SH0_HAS_ACCESS | SESSION_SH1_HAS_ACCESS | SESSION_SH2_HAS_ACCESS | SESSION_SH3_HAS_ACCESS)),
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
        (TO_ECC_KEY_SLOT_0_7(SESSION_SH0_HAS_ACCESS)                  |
         TO_ECC_KEY_SLOT_8_15(SESSION_SH0_HAS_ACCESS)  |
         TO_ECC_KEY_SLOT_16_23(SESSION_SH0_HAS_ACCESS) |
         TO_ECC_KEY_SLOT_24_31(SESSION_SH0_HAS_ACCESS)),
    //------- CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_STORE -------------------------
    // Only session SH0 can store key into ecc key slot 0-31
        (TO_ECC_KEY_SLOT_0_7(SESSION_SH0_HAS_ACCESS)   |
         TO_ECC_KEY_SLOT_8_15(SESSION_SH0_HAS_ACCESS)  |
         TO_ECC_KEY_SLOT_16_23(SESSION_SH0_HAS_ACCESS) |
         TO_ECC_KEY_SLOT_24_31(SESSION_SH0_HAS_ACCESS)),
    //------- CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_READ --------------------------
    // Only session SH0 can read pubkey from slot 0-31
        (TO_ECC_KEY_SLOT_0_7(  SESSION_SH0_HAS_ACCESS) |
         TO_ECC_KEY_SLOT_8_15( SESSION_SH0_HAS_ACCESS) |
         TO_ECC_KEY_SLOT_16_23(SESSION_SH0_HAS_ACCESS) |
         TO_ECC_KEY_SLOT_24_31(SESSION_SH0_HAS_ACCESS)),
    //------- CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_ERASE -------------------------
    // Only session SH0 can erase keys from slot 0-31
         (TO_ECC_KEY_SLOT_0_7(SESSION_SH0_HAS_ACCESS)  |
         TO_ECC_KEY_SLOT_8_15(SESSION_SH0_HAS_ACCESS)  |
         TO_ECC_KEY_SLOT_16_23(SESSION_SH0_HAS_ACCESS) |
         TO_ECC_KEY_SLOT_24_31(SESSION_SH0_HAS_ACCESS)),
    //------- CONFIGURATION_OBJECTS_CFG_UAP_ECDSA_SIGN ----------------------------
    // Only session SH0 can ECDSA sign with keys from slot 0-31
        (TO_ECC_KEY_SLOT_0_7(  SESSION_SH0_HAS_ACCESS) |
         TO_ECC_KEY_SLOT_8_15( SESSION_SH0_HAS_ACCESS) |
         TO_ECC_KEY_SLOT_16_23(SESSION_SH0_HAS_ACCESS) |
         TO_ECC_KEY_SLOT_24_31(SESSION_SH0_HAS_ACCESS)),
    //------- CONFIGURATION_OBJECTS_CFG_UAP_EDDSA_SIGN ----------------------------
    // Only session SH0 can EDDSA sign with keys from slot 0-31
        (TO_ECC_KEY_SLOT_0_7(  SESSION_SH0_HAS_ACCESS) |
         TO_ECC_KEY_SLOT_8_15( SESSION_SH0_HAS_ACCESS) |
         TO_ECC_KEY_SLOT_16_23(SESSION_SH0_HAS_ACCESS) |
         TO_ECC_KEY_SLOT_24_31(SESSION_SH0_HAS_ACCESS)),
    //------- CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_INIT -------------------------
    // SH0 can init all counters
        (TO_LT_MCOUNTER_0_3(  SESSION_SH0_HAS_ACCESS) |
         TO_LT_MCOUNTER_4_7( SESSION_SH0_HAS_ACCESS)  |
         TO_LT_MCOUNTER_8_11(SESSION_SH0_HAS_ACCESS)  |
         TO_LT_MCOUNTER_12_15(SESSION_SH0_HAS_ACCESS)),
    //------- CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_GET --------------------------
    // SH0 can get all counters
        (TO_LT_MCOUNTER_0_3(  SESSION_SH0_HAS_ACCESS) |
         TO_LT_MCOUNTER_4_7( SESSION_SH0_HAS_ACCESS)  |
         TO_LT_MCOUNTER_8_11(SESSION_SH0_HAS_ACCESS)  |
         TO_LT_MCOUNTER_12_15(SESSION_SH0_HAS_ACCESS)),
    //------- CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_UPDATE -----------------------
    // SH0 can update all counters
        (TO_LT_MCOUNTER_0_3(  SESSION_SH0_HAS_ACCESS) |
         TO_LT_MCOUNTER_4_7( SESSION_SH0_HAS_ACCESS)  |
         TO_LT_MCOUNTER_8_11(SESSION_SH0_HAS_ACCESS)  |
         TO_LT_MCOUNTER_12_15(SESSION_SH0_HAS_ACCESS)),
    //------- CONFIGURATION_OBJECTS_TODO -----------------------
    // All slots can use MAC-and-destroy
        (0xffffffff),
    //------- CONFIGURATION_OBJECTS_CFG_UAP_SERIAL_CODE_GET -----------------------
        (0x000000ff),
    }
};

void lt_test_reversible(void)
{
    LT_LOG("reversible features");

    lt_handle_t h = {0};

    lt_init(&h);

    LT_LOG("%s", "lt_get_info_chip_id() ");
    struct lt_chip_id_t chip_id = {0};
    LT_LOG_RESULT("%s", lt_ret_verbose(lt_get_info_chip_id(&h, &chip_id)));

    LT_LOG("%s", "lt_get_info_riscv_fw_ver() ");
    uint8_t riscv_fw_ver[LT_L2_GET_INFO_RISCV_FW_SIZE] = {0};
    LT_LOG_RESULT("%s", lt_ret_verbose(lt_get_info_riscv_fw_ver(&h, riscv_fw_ver, LT_L2_GET_INFO_RISCV_FW_SIZE)));

    LT_LOG("%s", "lt_get_info_spect_fw_ver() ");
    uint8_t spect_fw_ver[LT_L2_GET_INFO_SPECT_FW_SIZE] = {0};
    LT_LOG_RESULT("%s", lt_ret_verbose(lt_get_info_spect_fw_ver(&h, spect_fw_ver, LT_L2_GET_INFO_SPECT_FW_SIZE)));

    uint8_t X509_cert[LT_L2_GET_INFO_REQ_CERT_SIZE] = {0};
    LT_LOG("%s", "lt_get_info_cert() ");
    LT_LOG_RESULT("%s", lt_ret_verbose(lt_get_info_cert(&h, X509_cert, LT_L2_GET_INFO_REQ_CERT_SIZE)));

    LT_LOG("%s", "lt_cert_verify_and_parse() ");
    uint8_t stpub[32] = {0};
    LT_LOG_RESULT("%s", lt_ret_verbose(lt_cert_verify_and_parse(X509_cert, LT_L2_GET_INFO_REQ_CERT_SIZE, stpub)));

    LT_LOG("%s", "lt_session_start() ");
    LT_LOG_RESULT("%s", lt_ret_verbose(lt_session_start(&h, stpub, pkey_index_0, sh0priv, sh0pub)));

    LT_LOG("%s", "R CONFIG read:");
    struct lt_config_t r_config_read;
    LT_ASSERT(LT_OK, read_whole_R_config(&h, &r_config_read));
    // Print r config
    for (int i=0; i<27;i++) {
        LT_LOG("    %s,  %08" PRIX32, get_conf_desc(i), r_config_read.obj[i]);
    }

    LT_LOG("%s", "write_whole_R_config()");
    LT_ASSERT(LT_OK, write_whole_R_config(&h, &config));
    LT_LOG("%s", "lt_reboot(LT_L2_STARTUP_ID_REBOOT)");
    LT_ASSERT(LT_OK, lt_reboot(&h, LT_L2_STARTUP_ID_REBOOT));

    // Session was lost during reboot, init new session
    LT_LOG("%s", "Creating session with initial factory keys H0");
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, pkey_index_0));

    LT_LOG("%s", "R CONFIG read:");
    LT_ASSERT(LT_OK, read_whole_R_config(&h, &r_config_read));
    // Print r config
    for (int i=0; i<27;i++) {
        LT_LOG("    %s,  %08" PRIX32, get_conf_desc(i), r_config_read.obj[i]);
    }

    LT_LOG("%s", "lt_ping() ");
    uint8_t in[PING_LEN_MAX];
    uint8_t out[PING_LEN_MAX];
    memset(out, 0xaa, PING_LEN_MAX);
    LT_LOG("%s", lt_ret_verbose(lt_ping(&h, out, in, PING_LEN_MAX)));

    // Only reversible command for pairing key is pairing_key_read:
    uint8_t pairing_key[32];
    LT_LOG("%s", "lt_pairing_key_read(PAIRING_KEY_SLOT_INDEX_0) ");
    LT_LOG_RESULT("%s", lt_ret_verbose(lt_pairing_key_read(&h, pairing_key, PAIRING_KEY_SLOT_INDEX_0)));
    LT_LOG("%s", "lt_pairing_key_read(PAIRING_KEY_SLOT_INDEX_1) ");
    LT_LOG_RESULT("%s", lt_ret_verbose(lt_pairing_key_read(&h, pairing_key, PAIRING_KEY_SLOT_INDEX_1)));
    LT_LOG("%s", "lt_pairing_key_read(PAIRING_KEY_SLOT_INDEX_2) ");
    LT_LOG_RESULT("%s", lt_ret_verbose(lt_pairing_key_read(&h, pairing_key, PAIRING_KEY_SLOT_INDEX_2)));
    LT_LOG("%s", "lt_pairing_key_read(PAIRING_KEY_SLOT_INDEX_3) ");
    LT_LOG_RESULT("%s", lt_ret_verbose(lt_pairing_key_read(&h, pairing_key, PAIRING_KEY_SLOT_INDEX_3)));

    LT_LOG("---------------- Loop through all key slots, ed25519, stored key ----------------");

    for(uint16_t i=0; i<32; i++) {
        LT_LOG("lt_ecc_key_store() slot        %d  ", i);
        uint8_t attestation_key[32] = {0x22,0x57,0xa8,0x2f,0x85,0x8f,0x13,0x32,0xfa,0x0f,0xf6,0x0c,0x76,0x29,0x42,0x70,0xa9,0x58,0x9d,0xfd,0x47,0xa5,0x23,0x78,0x18,0x4d,0x2d,0x38,0xf0,0xa7,0xc4,0x01};
        LT_LOG_RESULT("%s", lt_ret_verbose(lt_ecc_key_store(&h, i, CURVE_ED25519, attestation_key)));

        LT_LOG("lt_ecc_key_read() slot         %d  ", i);
        uint8_t slot_0_pubkey[64];
        lt_ecc_curve_type_t curve;
        ecc_key_origin_t origin;
        LT_ASSERT(lt_ecc_key_read(&h, i, slot_0_pubkey, 64, &curve, &origin), LT_OK);

        LT_LOG("lt_ecc_eddsa_sign() slot       %d  ", i);
        uint8_t msg[] = {'a', 'h', 'o', 'j'};
        uint8_t rs[64];
        LT_LOG_RESULT("%s", lt_ret_verbose(lt_ecc_eddsa_sign(&h, i, msg, 4, rs, 64)));

        LT_LOG("lt_ecc_eddsa_sig_verify() slot %d  ", i);
        LT_LOG_RESULT("%s", lt_ret_verbose(lt_ecc_eddsa_sig_verify(msg, 4, slot_0_pubkey, rs)));

        LT_LOG("lt_ecc_key_erase() slot        %d  ", i);
        LT_LOG_RESULT("%s", lt_ret_verbose(lt_ecc_key_erase(&h, i)));

        LT_LOG("");
    }

    LT_LOG("---------------- Loop through all key slots, ed25519, generated key ----------------");

    for(int i=0; i<32; i++) {
        LT_LOG("lt_ecc_key_generate() slot     %d  ", i);
        LT_LOG_RESULT("%s", lt_ret_verbose(lt_ecc_key_generate(&h, i, CURVE_ED25519)));

        LT_LOG("lt_ecc_key_read() slot         %d  ", i);
        uint8_t slot_0_pubkey[64];
        lt_ecc_curve_type_t curve;
        ecc_key_origin_t origin;
        LT_LOG_RESULT("%s", lt_ret_verbose(lt_ecc_key_read(&h, i, slot_0_pubkey, 64, &curve, &origin)));

        LT_LOG("lt_ecc_eddsa_sign() slot       %d  ", i);
        uint8_t msg[] = {'a', 'h', 'o', 'j'};
        uint8_t rs[64];
        LT_LOG_RESULT("%s", lt_ret_verbose(lt_ecc_eddsa_sign(&h, i, msg, 4, rs, 64)));

        LT_LOG("lt_ecc_eddsa_sig_verify() slot %d  ", i);
        LT_LOG_RESULT("%s", lt_ret_verbose(lt_ecc_eddsa_sig_verify(msg, 4, slot_0_pubkey, rs)));

        LT_LOG("lt_ecc_key_erase() slot        %d  ", i);
        LT_LOG_RESULT("%s", lt_ret_verbose(lt_ecc_key_erase(&h, i)));

        LT_LOG("");
    }

    LT_LOG("lt_random_get() RANDOM_VALUE_GET_LEN_MAX == 255 ");
    uint8_t buff[RANDOM_VALUE_GET_LEN_MAX];
    LT_LOG_RESULT("%s", lt_ret_verbose(lt_random_get(&h, buff, RANDOM_VALUE_GET_LEN_MAX)));

    LT_LOG("lt_r_config_erase()");
    LT_ASSERT(LT_OK, lt_r_config_erase(&h));

    LT_LOG("lt_reboot(LT_L2_STARTUP_ID_REBOOT)");
    LT_ASSERT(LT_OK, lt_reboot(&h, LT_L2_STARTUP_ID_REBOOT));


    // Session was lost during reboot, init new session
    LT_LOG("%s", "Creating session with initial factory keys H0");
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, pkey_index_0));

    LT_LOG("read_whole_R_config()");
    LT_ASSERT(LT_OK, read_whole_R_config(&h, &r_config_read));
    // Print r config
    for (int i=0; i<27;i++) {
        LT_LOG("    %s,  %08" PRIX32, get_conf_desc(i), r_config_read.obj[i]);
    }

    LT_LOG("lt_session_abort()");
    LT_LOG_RESULT("%s", lt_ret_verbose(lt_session_abort(&h)));

    LT_LOG("lt_deinit()");
    LT_LOG_RESULT("%s", lt_ret_verbose(lt_deinit(&h)));

}
