//   Ordt 230321.02 autogenerated file 
//   Input: /projects/tropic01/work/avrba/tassic/doc/user_api/tropic01_configuration_objects.rdl
//   Parms: /projects/tropic01/work/avrba/tassic/scripts/param_file
//   Date: Tue Jul 16 13:19:30 CEST 2024
//

#ifndef __CONFIGURATION_OBJECTS_REGISTER_MAP__
#define __CONFIGURATION_OBJECTS_REGISTER_MAP__

#include "bits.h"

/* CONFIGURATION_OBJECTS_REGISTERS memory map */
enum CONFIGURATION_OBJECTS_REGS {
  CONFIGURATION_OBJECTS_CFG_START_UP_ADDR = 0X0,
  CONFIGURATION_OBJECTS_CFG_SLEEP_MODE_ADDR = 0X4,
  CONFIGURATION_OBJECTS_CFG_SENSORS_ADDR = 0X8,
  CONFIGURATION_OBJECTS_CFG_DEBUG_ADDR = 0X10,
  CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_WRITE_ADDR = 0X20,
  CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_READ_ADDR = 0X24,
  CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_INVALIDATE_ADDR = 0X28,
  CONFIGURATION_OBJECTS_CFG_UAP_R_CONFIG_WRITE_ERASE_ADDR = 0X30,
  CONFIGURATION_OBJECTS_CFG_UAP_R_CONFIG_READ_ADDR = 0X34,
  CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_WRITE_ADDR = 0X40,
  CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_READ_ADDR = 0X44,
  CONFIGURATION_OBJECTS_CFG_UAP_PING_ADDR = 0X100,
  CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_WRITE_ADDR = 0X110,
  CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_READ_ADDR = 0X114,
  CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_ERASE_ADDR = 0X118,
  CONFIGURATION_OBJECTS_CFG_UAP_RANDOM_VALUE_GET_ADDR = 0X120,
  CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_GENERATE_ADDR = 0X130,
  CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_STORE_ADDR = 0X134,
  CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_READ_ADDR = 0X138,
  CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_ERASE_ADDR = 0X13C,
  CONFIGURATION_OBJECTS_CFG_UAP_ECDSA_SIGN_ADDR = 0X140,
  CONFIGURATION_OBJECTS_CFG_UAP_EDDSA_SIGN_ADDR = 0X144,
  CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_INIT_ADDR = 0X150,
  CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_GET_ADDR = 0X154,
  CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_UPDATE_ADDR = 0X158,
  CONFIGURATION_OBJECTS_CFG_UAP_MAC_AND_DESTROY_ADDR = 0X160,
  CONFIGURATION_OBJECTS_CFG_UAP_SERIAL_CODE_GET_ADDR = 0X170
};

/* CFG_START_UP register fields */

#define CONFIGURATION_OBJECTS_CFG_START_UP_MBIST_DIS_MASK BIT(1)
#define CONFIGURATION_OBJECTS_CFG_START_UP_MBIST_DIS_POS 1

#define CONFIGURATION_OBJECTS_CFG_START_UP_RNGTEST_DIS_MASK BIT(2)
#define CONFIGURATION_OBJECTS_CFG_START_UP_RNGTEST_DIS_POS 2

#define CONFIGURATION_OBJECTS_CFG_START_UP_MAINTENANCE_ENA_MASK BIT(3)
#define CONFIGURATION_OBJECTS_CFG_START_UP_MAINTENANCE_ENA_POS 3

#define CONFIGURATION_OBJECTS_CFG_START_UP_CPU_FW_VERIFY_DIS_MASK BIT(4)
#define CONFIGURATION_OBJECTS_CFG_START_UP_CPU_FW_VERIFY_DIS_POS 4

#define CONFIGURATION_OBJECTS_CFG_START_UP_SPECT_FW_VERIFY_DIS_MASK BIT(5)
#define CONFIGURATION_OBJECTS_CFG_START_UP_SPECT_FW_VERIFY_DIS_POS 5


/* CFG_SLEEP_MODE register fields */

#define CONFIGURATION_OBJECTS_CFG_SLEEP_MODE_SLEEP_MODE_EN_MASK BIT(0)
#define CONFIGURATION_OBJECTS_CFG_SLEEP_MODE_SLEEP_MODE_EN_POS 0

#define CONFIGURATION_OBJECTS_CFG_SLEEP_MODE_DEEP_SLEEP_MODE_EN_MASK BIT(1)
#define CONFIGURATION_OBJECTS_CFG_SLEEP_MODE_DEEP_SLEEP_MODE_EN_POS 1


/* CFG_SENSORS register fields */

#define CONFIGURATION_OBJECTS_CFG_SENSORS_PTRNG0_TEST_DIS_MASK BIT(0)
#define CONFIGURATION_OBJECTS_CFG_SENSORS_PTRNG0_TEST_DIS_POS 0

#define CONFIGURATION_OBJECTS_CFG_SENSORS_PTRNG1_TEST_DIS_MASK BIT(1)
#define CONFIGURATION_OBJECTS_CFG_SENSORS_PTRNG1_TEST_DIS_POS 1

#define CONFIGURATION_OBJECTS_CFG_SENSORS_OSCILLATOR_MON_DIS_MASK BIT(2)
#define CONFIGURATION_OBJECTS_CFG_SENSORS_OSCILLATOR_MON_DIS_POS 2

#define CONFIGURATION_OBJECTS_CFG_SENSORS_SHIELD_DIS_MASK BIT(3)
#define CONFIGURATION_OBJECTS_CFG_SENSORS_SHIELD_DIS_POS 3

#define CONFIGURATION_OBJECTS_CFG_SENSORS_VOLTAGE_MON_DIS_MASK BIT(4)
#define CONFIGURATION_OBJECTS_CFG_SENSORS_VOLTAGE_MON_DIS_POS 4

#define CONFIGURATION_OBJECTS_CFG_SENSORS_GLITCH_DET_DIS_MASK BIT(5)
#define CONFIGURATION_OBJECTS_CFG_SENSORS_GLITCH_DET_DIS_POS 5

#define CONFIGURATION_OBJECTS_CFG_SENSORS_TEMP_SENS_DIS_MASK BIT(6)
#define CONFIGURATION_OBJECTS_CFG_SENSORS_TEMP_SENS_DIS_POS 6

#define CONFIGURATION_OBJECTS_CFG_SENSORS_LASER_DET_DIS_MASK BIT(7)
#define CONFIGURATION_OBJECTS_CFG_SENSORS_LASER_DET_DIS_POS 7

#define CONFIGURATION_OBJECTS_CFG_SENSORS_EM_PULSE_DET_DIS_MASK BIT(8)
#define CONFIGURATION_OBJECTS_CFG_SENSORS_EM_PULSE_DET_DIS_POS 8

#define CONFIGURATION_OBJECTS_CFG_SENSORS_CPU_ALERT_DIS_MASK BIT(9)
#define CONFIGURATION_OBJECTS_CFG_SENSORS_CPU_ALERT_DIS_POS 9

#define CONFIGURATION_OBJECTS_CFG_SENSORS_PIN_VERIF_BIT_FLIP_DIS_MASK BIT(10)
#define CONFIGURATION_OBJECTS_CFG_SENSORS_PIN_VERIF_BIT_FLIP_DIS_POS 10

#define CONFIGURATION_OBJECTS_CFG_SENSORS_SCB_BIT_FLIP_DIS_MASK BIT(11)
#define CONFIGURATION_OBJECTS_CFG_SENSORS_SCB_BIT_FLIP_DIS_POS 11

#define CONFIGURATION_OBJECTS_CFG_SENSORS_CPB_BIT_FLIP_DIS_MASK BIT(12)
#define CONFIGURATION_OBJECTS_CFG_SENSORS_CPB_BIT_FLIP_DIS_POS 12

#define CONFIGURATION_OBJECTS_CFG_SENSORS_ECC_BIT_FLIP_DIS_MASK BIT(13)
#define CONFIGURATION_OBJECTS_CFG_SENSORS_ECC_BIT_FLIP_DIS_POS 13

#define CONFIGURATION_OBJECTS_CFG_SENSORS_R_MEM_BIT_FLIP_DIS_MASK BIT(14)
#define CONFIGURATION_OBJECTS_CFG_SENSORS_R_MEM_BIT_FLIP_DIS_POS 14

#define CONFIGURATION_OBJECTS_CFG_SENSORS_EKDB_BIT_FLIP_DIS_MASK BIT(15)
#define CONFIGURATION_OBJECTS_CFG_SENSORS_EKDB_BIT_FLIP_DIS_POS 15

#define CONFIGURATION_OBJECTS_CFG_SENSORS_I_MEM_BIT_FLIP_DIS_MASK BIT(16)
#define CONFIGURATION_OBJECTS_CFG_SENSORS_I_MEM_BIT_FLIP_DIS_POS 16

#define CONFIGURATION_OBJECTS_CFG_SENSORS_PLATFORM_BIT_FLIP_DIS_MASK BIT(17)
#define CONFIGURATION_OBJECTS_CFG_SENSORS_PLATFORM_BIT_FLIP_DIS_POS 17


/* CFG_DEBUG register fields */

#define CONFIGURATION_OBJECTS_CFG_DEBUG_FW_LOG_EN_MASK BIT(0)
#define CONFIGURATION_OBJECTS_CFG_DEBUG_FW_LOG_EN_POS 0


/* CFG_UAP_PAIRING_KEY_WRITE register fields */

#define CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_WRITE_WRITE_PKEY_SLOT_0_MASK GENMASK(7, 0)
#define CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_WRITE_WRITE_PKEY_SLOT_0_POS 0

#define CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_WRITE_WRITE_PKEY_SLOT_1_MASK GENMASK(15, 8)
#define CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_WRITE_WRITE_PKEY_SLOT_1_POS 8

#define CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_WRITE_WRITE_PKEY_SLOT_2_MASK GENMASK(23, 16)
#define CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_WRITE_WRITE_PKEY_SLOT_2_POS 16

#define CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_WRITE_WRITE_PKEY_SLOT_3_MASK GENMASK(31, 24)
#define CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_WRITE_WRITE_PKEY_SLOT_3_POS 24


/* CFG_UAP_PAIRING_KEY_READ register fields */

#define CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_READ_READ_PKEY_SLOT_0_MASK GENMASK(7, 0)
#define CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_READ_READ_PKEY_SLOT_0_POS 0

#define CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_READ_READ_PKEY_SLOT_1_MASK GENMASK(15, 8)
#define CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_READ_READ_PKEY_SLOT_1_POS 8

#define CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_READ_READ_PKEY_SLOT_2_MASK GENMASK(23, 16)
#define CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_READ_READ_PKEY_SLOT_2_POS 16

#define CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_READ_READ_PKEY_SLOT_3_MASK GENMASK(31, 24)
#define CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_READ_READ_PKEY_SLOT_3_POS 24


/* CFG_UAP_PAIRING_KEY_INVALIDATE register fields */

#define CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_INVALIDATE_INVALIDATE_PKEY_SLOT_0_MASK GENMASK(7, 0)
#define CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_INVALIDATE_INVALIDATE_PKEY_SLOT_0_POS 0

#define CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_INVALIDATE_INVALIDATE_PKEY_SLOT_1_MASK GENMASK(15, 8)
#define CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_INVALIDATE_INVALIDATE_PKEY_SLOT_1_POS 8

#define CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_INVALIDATE_INVALIDATE_PKEY_SLOT_2_MASK GENMASK(23, 16)
#define CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_INVALIDATE_INVALIDATE_PKEY_SLOT_2_POS 16

#define CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_INVALIDATE_INVALIDATE_PKEY_SLOT_3_MASK GENMASK(31, 24)
#define CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_INVALIDATE_INVALIDATE_PKEY_SLOT_3_POS 24


/* CFG_UAP_R_CONFIG_WRITE_ERASE register fields */

#define CONFIGURATION_OBJECTS_CFG_UAP_R_CONFIG_WRITE_ERASE_R_CONFIG_WRITE_ERASE_MASK GENMASK(7, 0)
#define CONFIGURATION_OBJECTS_CFG_UAP_R_CONFIG_WRITE_ERASE_R_CONFIG_WRITE_ERASE_POS 0


/* CFG_UAP_R_CONFIG_READ register fields */

#define CONFIGURATION_OBJECTS_CFG_UAP_R_CONFIG_READ_R_CONFIG_READ_CFG_MASK GENMASK(7, 0)
#define CONFIGURATION_OBJECTS_CFG_UAP_R_CONFIG_READ_R_CONFIG_READ_CFG_POS 0

#define CONFIGURATION_OBJECTS_CFG_UAP_R_CONFIG_READ_R_CONFIG_READ_FUNC_MASK GENMASK(15, 8)
#define CONFIGURATION_OBJECTS_CFG_UAP_R_CONFIG_READ_R_CONFIG_READ_FUNC_POS 8


/* CFG_UAP_I_CONFIG_WRITE register fields */

#define CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_WRITE_I_CONFIG_WRITE_CFG_MASK GENMASK(7, 0)
#define CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_WRITE_I_CONFIG_WRITE_CFG_POS 0

#define CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_WRITE_I_CONFIG_WRITE_FUNC_MASK GENMASK(15, 8)
#define CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_WRITE_I_CONFIG_WRITE_FUNC_POS 8


/* CFG_UAP_I_CONFIG_READ register fields */

#define CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_READ_I_CONFIG_READ_CFG_MASK GENMASK(7, 0)
#define CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_READ_I_CONFIG_READ_CFG_POS 0

#define CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_READ_I_CONFIG_READ_FUNC_MASK GENMASK(15, 8)
#define CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_READ_I_CONFIG_READ_FUNC_POS 8


/* CFG_UAP_PING register fields */

#define CONFIGURATION_OBJECTS_CFG_UAP_PING_PING_MASK GENMASK(7, 0)
#define CONFIGURATION_OBJECTS_CFG_UAP_PING_PING_POS 0


/* CFG_UAP_R_MEM_DATA_WRITE register fields */

#define CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_WRITE_WRITE_UDATA_SLOT_0_127_MASK GENMASK(7, 0)
#define CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_WRITE_WRITE_UDATA_SLOT_0_127_POS 0

#define CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_WRITE_WRITE_UDATA_SLOT_128_255_MASK GENMASK(15, 8)
#define CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_WRITE_WRITE_UDATA_SLOT_128_255_POS 8

#define CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_WRITE_WRITE_UDATA_SLOT_256_383_MASK GENMASK(23, 16)
#define CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_WRITE_WRITE_UDATA_SLOT_256_383_POS 16

#define CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_WRITE_WRITE_UDATA_SLOT_384_511_MASK GENMASK(31, 24)
#define CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_WRITE_WRITE_UDATA_SLOT_384_511_POS 24


/* CFG_UAP_R_MEM_DATA_READ register fields */

#define CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_READ_READ_UDATA_SLOT_0_127_MASK GENMASK(7, 0)
#define CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_READ_READ_UDATA_SLOT_0_127_POS 0

#define CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_READ_READ_UDATA_SLOT_128_255_MASK GENMASK(15, 8)
#define CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_READ_READ_UDATA_SLOT_128_255_POS 8

#define CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_READ_READ_UDATA_SLOT_256_383_MASK GENMASK(23, 16)
#define CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_READ_READ_UDATA_SLOT_256_383_POS 16

#define CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_READ_READ_UDATA_SLOT_384_511_MASK GENMASK(31, 24)
#define CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_READ_READ_UDATA_SLOT_384_511_POS 24


/* CFG_UAP_R_MEM_DATA_ERASE register fields */

#define CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_ERASE_ERASE_UDATA_SLOT_0_127_MASK GENMASK(7, 0)
#define CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_ERASE_ERASE_UDATA_SLOT_0_127_POS 0

#define CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_ERASE_ERASE_UDATA_SLOT_128_255_MASK GENMASK(15, 8)
#define CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_ERASE_ERASE_UDATA_SLOT_128_255_POS 8

#define CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_ERASE_ERASE_UDATA_SLOT_256_383_MASK GENMASK(23, 16)
#define CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_ERASE_ERASE_UDATA_SLOT_256_383_POS 16

#define CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_ERASE_ERASE_UDATA_SLOT_384_511_MASK GENMASK(31, 24)
#define CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_ERASE_ERASE_UDATA_SLOT_384_511_POS 24


/* CFG_UAP_RANDOM_VALUE_GET register fields */

#define CONFIGURATION_OBJECTS_CFG_UAP_RANDOM_VALUE_GET_RANDOM_VALUE_GET_MASK GENMASK(7, 0)
#define CONFIGURATION_OBJECTS_CFG_UAP_RANDOM_VALUE_GET_RANDOM_VALUE_GET_POS 0


/* CFG_UAP_ECC_KEY_GENERATE register fields */

#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_GENERATE_GEN_ECCKEY_SLOT_0_7_MASK GENMASK(7, 0)
#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_GENERATE_GEN_ECCKEY_SLOT_0_7_POS 0

#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_GENERATE_GEN_ECCKEY_SLOT_8_15_MASK GENMASK(15, 8)
#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_GENERATE_GEN_ECCKEY_SLOT_8_15_POS 8

#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_GENERATE_GEN_ECCKEY_SLOT_16_23_MASK GENMASK(23, 16)
#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_GENERATE_GEN_ECCKEY_SLOT_16_23_POS 16

#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_GENERATE_GEN_ECCKEY_SLOT_24_31_MASK GENMASK(31, 24)
#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_GENERATE_GEN_ECCKEY_SLOT_24_31_POS 24


/* CFG_UAP_ECC_KEY_STORE register fields */

#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_STORE_STORE_ECCKEY_SLOT_0_7_MASK GENMASK(7, 0)
#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_STORE_STORE_ECCKEY_SLOT_0_7_POS 0

#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_STORE_STORE_ECCKEY_SLOT_8_15_MASK GENMASK(15, 8)
#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_STORE_STORE_ECCKEY_SLOT_8_15_POS 8

#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_STORE_STORE_ECCKEY_SLOT_16_23_MASK GENMASK(23, 16)
#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_STORE_STORE_ECCKEY_SLOT_16_23_POS 16

#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_STORE_STORE_ECCKEY_SLOT_24_31_MASK GENMASK(31, 24)
#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_STORE_STORE_ECCKEY_SLOT_24_31_POS 24


/* CFG_UAP_ECC_KEY_READ register fields */

#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_READ_READ_ECCKEY_SLOT_0_7_MASK GENMASK(7, 0)
#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_READ_READ_ECCKEY_SLOT_0_7_POS 0

#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_READ_READ_ECCKEY_SLOT_8_15_MASK GENMASK(15, 8)
#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_READ_READ_ECCKEY_SLOT_8_15_POS 8

#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_READ_READ_ECCKEY_SLOT_16_23_MASK GENMASK(23, 16)
#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_READ_READ_ECCKEY_SLOT_16_23_POS 16

#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_READ_READ_ECCKEY_SLOT_24_31_MASK GENMASK(31, 24)
#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_READ_READ_ECCKEY_SLOT_24_31_POS 24


/* CFG_UAP_ECC_KEY_ERASE register fields */

#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_ERASE_ERASE_ECCKEY_SLOT_0_7_MASK GENMASK(7, 0)
#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_ERASE_ERASE_ECCKEY_SLOT_0_7_POS 0

#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_ERASE_ERASE_ECCKEY_SLOT_8_15_MASK GENMASK(15, 8)
#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_ERASE_ERASE_ECCKEY_SLOT_8_15_POS 8

#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_ERASE_ERASE_ECCKEY_SLOT_16_23_MASK GENMASK(23, 16)
#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_ERASE_ERASE_ECCKEY_SLOT_16_23_POS 16

#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_ERASE_ERASE_ECCKEY_SLOT_24_31_MASK GENMASK(31, 24)
#define CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_ERASE_ERASE_ECCKEY_SLOT_24_31_POS 24


/* CFG_UAP_ECDSA_SIGN register fields */

#define CONFIGURATION_OBJECTS_CFG_UAP_ECDSA_SIGN_ECDSA_ECCKEY_SLOT_0_7_MASK GENMASK(7, 0)
#define CONFIGURATION_OBJECTS_CFG_UAP_ECDSA_SIGN_ECDSA_ECCKEY_SLOT_0_7_POS 0

#define CONFIGURATION_OBJECTS_CFG_UAP_ECDSA_SIGN_ECDSA_ECCKEY_SLOT_8_15_MASK GENMASK(15, 8)
#define CONFIGURATION_OBJECTS_CFG_UAP_ECDSA_SIGN_ECDSA_ECCKEY_SLOT_8_15_POS 8

#define CONFIGURATION_OBJECTS_CFG_UAP_ECDSA_SIGN_ECDSA_ECCKEY_SLOT_16_23_MASK GENMASK(23, 16)
#define CONFIGURATION_OBJECTS_CFG_UAP_ECDSA_SIGN_ECDSA_ECCKEY_SLOT_16_23_POS 16

#define CONFIGURATION_OBJECTS_CFG_UAP_ECDSA_SIGN_ECDSA_ECCKEY_SLOT_24_31_MASK GENMASK(31, 24)
#define CONFIGURATION_OBJECTS_CFG_UAP_ECDSA_SIGN_ECDSA_ECCKEY_SLOT_24_31_POS 24


/* CFG_UAP_EDDSA_SIGN register fields */

#define CONFIGURATION_OBJECTS_CFG_UAP_EDDSA_SIGN_EDDSA_ECCKEY_SLOT_0_7_MASK GENMASK(7, 0)
#define CONFIGURATION_OBJECTS_CFG_UAP_EDDSA_SIGN_EDDSA_ECCKEY_SLOT_0_7_POS 0

#define CONFIGURATION_OBJECTS_CFG_UAP_EDDSA_SIGN_EDDSA_ECCKEY_SLOT_8_15_MASK GENMASK(15, 8)
#define CONFIGURATION_OBJECTS_CFG_UAP_EDDSA_SIGN_EDDSA_ECCKEY_SLOT_8_15_POS 8

#define CONFIGURATION_OBJECTS_CFG_UAP_EDDSA_SIGN_EDDSA_ECCKEY_SLOT_16_23_MASK GENMASK(23, 16)
#define CONFIGURATION_OBJECTS_CFG_UAP_EDDSA_SIGN_EDDSA_ECCKEY_SLOT_16_23_POS 16

#define CONFIGURATION_OBJECTS_CFG_UAP_EDDSA_SIGN_EDDSA_ECCKEY_SLOT_24_31_MASK GENMASK(31, 24)
#define CONFIGURATION_OBJECTS_CFG_UAP_EDDSA_SIGN_EDDSA_ECCKEY_SLOT_24_31_POS 24


/* CFG_UAP_MCOUNTER_INIT register fields */

#define CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_INIT_MCOUNTER_INIT_0_3_MASK GENMASK(7, 0)
#define CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_INIT_MCOUNTER_INIT_0_3_POS 0

#define CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_INIT_MCOUNTER_INIT_4_7_MASK GENMASK(15, 8)
#define CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_INIT_MCOUNTER_INIT_4_7_POS 8

#define CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_INIT_MCOUNTER_INIT_8_11_MASK GENMASK(23, 16)
#define CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_INIT_MCOUNTER_INIT_8_11_POS 16

#define CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_INIT_MCOUNTER_INIT_12_15_MASK GENMASK(31, 24)
#define CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_INIT_MCOUNTER_INIT_12_15_POS 24


/* CFG_UAP_MCOUNTER_GET register fields */

#define CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_GET_MCOUNTER_GET_0_3_MASK GENMASK(7, 0)
#define CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_GET_MCOUNTER_GET_0_3_POS 0

#define CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_GET_MCOUNTER_GET_4_7_MASK GENMASK(15, 8)
#define CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_GET_MCOUNTER_GET_4_7_POS 8

#define CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_GET_MCOUNTER_GET_8_11_MASK GENMASK(23, 16)
#define CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_GET_MCOUNTER_GET_8_11_POS 16

#define CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_GET_MCOUNTER_GET_12_15_MASK GENMASK(31, 24)
#define CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_GET_MCOUNTER_GET_12_15_POS 24


/* CFG_UAP_MCOUNTER_UPDATE register fields */

#define CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_UPDATE_MCOUNTER_UPDATE_0_3_MASK GENMASK(7, 0)
#define CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_UPDATE_MCOUNTER_UPDATE_0_3_POS 0

#define CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_UPDATE_MCOUNTER_UPDATE_4_7_MASK GENMASK(15, 8)
#define CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_UPDATE_MCOUNTER_UPDATE_4_7_POS 8

#define CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_UPDATE_MCOUNTER_UPDATE_8_11_MASK GENMASK(23, 16)
#define CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_UPDATE_MCOUNTER_UPDATE_8_11_POS 16

#define CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_UPDATE_MCOUNTER_UPDATE_12_15_MASK GENMASK(31, 24)
#define CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_UPDATE_MCOUNTER_UPDATE_12_15_POS 24


/* CFG_UAP_MAC_AND_DESTROY register fields */

#define CONFIGURATION_OBJECTS_CFG_UAP_MAC_AND_DESTROY_MACANDD_0_31_MASK GENMASK(7, 0)
#define CONFIGURATION_OBJECTS_CFG_UAP_MAC_AND_DESTROY_MACANDD_0_31_POS 0

#define CONFIGURATION_OBJECTS_CFG_UAP_MAC_AND_DESTROY_MACANDD_32_63_MASK GENMASK(15, 8)
#define CONFIGURATION_OBJECTS_CFG_UAP_MAC_AND_DESTROY_MACANDD_32_63_POS 8

#define CONFIGURATION_OBJECTS_CFG_UAP_MAC_AND_DESTROY_MACANDD_64_95_MASK GENMASK(23, 16)
#define CONFIGURATION_OBJECTS_CFG_UAP_MAC_AND_DESTROY_MACANDD_64_95_POS 16

#define CONFIGURATION_OBJECTS_CFG_UAP_MAC_AND_DESTROY_MACANDD_96_127_MASK GENMASK(31, 24)
#define CONFIGURATION_OBJECTS_CFG_UAP_MAC_AND_DESTROY_MACANDD_96_127_POS 24


/* CFG_UAP_SERIAL_CODE_GET register fields */

#define CONFIGURATION_OBJECTS_CFG_UAP_SERIAL_CODE_GET_SERIAL_CODE_MASK GENMASK(7, 0)
#define CONFIGURATION_OBJECTS_CFG_UAP_SERIAL_CODE_GET_SERIAL_CODE_POS 0


#endif

