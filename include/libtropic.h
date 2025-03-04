#ifndef LT_LIBTROPIC_H
#define LT_LIBTROPIC_H

/**
 * @defgroup libtropic_API libtropic API functions
 * @brief Expected to be used by an application
 * @details Dear users, please use this API. It contains all functions you need to interface with TROPIC01 device.
 * @{
 */

/**
 * @file libtropic.h
 * @brief libtropic library main API header file
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdbool.h>
#include <stddef.h>

#include "libtropic_common.h"
#include "TROPIC01_configuration_objects.h"

//--------------------------------------------------------------------------------------------------------------------//
/**
 * @brief Initialize handle and transport layer
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_init(lt_handle_t *h);

//--------------------------------------------------------------------------------------------------------------------//
/**
 * @brief Deinitialize handle and transport layer
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_deinit(lt_handle_t *h);

//--------------------------------------------------------------------------------------------------------------------//
#define LT_MODE_STARTUP 1
#define LT_MODE_APP     0
/**
 * @brief Update mode variable in handle
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_update_mode(lt_handle_t *h);

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal size of TROPIC01's certificate */
#define LT_L2_GET_INFO_REQ_CERT_SIZE         512

/**
 * @brief Get device's certificate
 *
 * @param h           Device's handle
 * @param cert        Certificate's buffer
 * @param max_len     Length of certificate's buffer
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_get_info_cert(lt_handle_t *h, uint8_t *cert, const uint16_t max_len);

//--------------------------------------------------------------------------------------------------------------------//
/**
 * @brief Verify certificate chain and parse STPUB
 *
 * @param cert        Certificate in DER format
 * @param max_len     Len of certificate buffer
 * @param stpub       TROPIC01 STPUB, unique for each device
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_cert_verify_and_parse(const uint8_t *cert, const uint16_t max_len, uint8_t *stpub);

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal size of returned CHIP ID */
#define LT_L2_GET_INFO_CHIP_ID_SIZE         128
/**
 * @brief Read TROPIC01's chip ID
 *
 * @param h           Device's handle
 * @param chip_id     Buffer for CHIP ID bytes
 * @param max_len     Length of a buffer to store id in
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_get_info_chip_id(lt_handle_t *h, uint8_t *chip_id, const uint16_t max_len);

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal size of returned RISCV fw version */
#define LT_L2_GET_INFO_RISCV_FW_SIZE         4
/**
 * @brief Read TROPIC01's RISCV firmware version
 *
 * @param h           Device's handle
 * @param ver         Buffer for FW version bytes
 * @param max_len     Length of a buffer to store fw version in
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_get_info_riscv_fw_ver(lt_handle_t *h, uint8_t *ver, const uint16_t max_len);

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal size of returned SPECT fw version */
#define LT_L2_GET_INFO_SPECT_FW_SIZE        4
/**
 * @brief Read TROPIC01's SPECT firmware version
 *
 * @param h           Device's handle
 * @param ver         Buffer for SPECT version bytes
 * @param max_len     Length of a buffer to store fw version in
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_get_info_spect_fw_ver(lt_handle_t *h, uint8_t *ver, const uint16_t max_len);

//--------------------------------------------------------------------------------------------------------------------//
typedef enum {
    FW_BANK_FW1 = 1, // Firmware bank 1.
    FW_BANK_FW2 = 2, // Firmware bank 2
    FW_BANK_SPECT1 = 17, // SPECT bank 1.
    FW_BANK_SPECT2 = 18, // SPECT bank 2
} bank_id_t;

/** @brief Maximal size of returned fw header */
#define LT_L2_GET_INFO_FW_HEADER_SIZE       20
/**
 * @brief Read TROPIC01's fw bank info
 *
 * @param h           Device's handle
 * @param bank_id     ID of firmware bank (one from enum bank_id_t)
 * @param header      Buffer to store fw header bytes into
 * @param max_len     Length of a buffer
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_get_info_fw_bank(lt_handle_t *h, const bank_id_t bank_id, uint8_t *header, const uint16_t max_len);

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Pairing key indexes corresponds to S_HiPub */
typedef enum {
    PAIRING_KEY_SLOT_INDEX_0,
    PAIRING_KEY_SLOT_INDEX_1,
    PAIRING_KEY_SLOT_INDEX_2,
    PAIRING_KEY_SLOT_INDEX_3,
} pkey_index_t;

/**
 * @brief Establish encrypted session between TROPIC01 and host MCU
 *
 * @param h           Device's handle
 * @param stpub       STPUB from device's certificate
 * @param pkey_index  Index of pairing public key
 * @param shipriv     Secure host private key
 * @param shipub      Secure host public key
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_session_start(lt_handle_t *h, const uint8_t *stpub, const pkey_index_t pkey_index, const uint8_t *shipriv, const uint8_t *shipub);

//--------------------------------------------------------------------------------------------------------------------//
/**
 * @brief Abort encrypted session between TROPIC01 and host MCU
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_session_abort(lt_handle_t *h);

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Basic sleep mode */
#define LT_L2_SLEEP_KIND_SLEEP 0x05
/** @brief Deep sleep mode */
#define LT_L2_SLEEP_KIND_DEEP_SLEEP 0x0a

/**
 * @brief Put TROPIC01 into sleep
 *
 * @param h           Device's handle
 * @param sleep_kind  Kind of sleep
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_sleep(lt_handle_t *h, const uint8_t sleep_kind);

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Reboot TROPIC01 chip */
#define LT_L2_STARTUP_ID_REBOOT 0x01
/** @brief Reboot TROPIC01 chip and stay in maintenance mode */
#define LT_L2_STARTUP_ID_MAINTENANCE 0x03

/**
 * @brief Reboot TROPIC01
 *
 * @param h           Device's handle
 * @param startup_id  Startup ID
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_reboot(lt_handle_t *h, const uint8_t startup_id);

/**
 * @brief Erase mutable firmware in one of banks
 *
 * @param h           Device's handle
 * @param bank_id     enum bank_id_t
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_mutable_fw_erase(lt_handle_t *h, bank_id_t bank_id);

/**
 * @brief Update mutable firmware in one of banks
 *
 * @param h           Device's handle
 * @param fw_data     Array with firmware bytes
 * @param fw_data_size  Number of firmware's bytes in passed array
 * @param bank_id     enum bank_id_t
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_mutable_fw_update(lt_handle_t *h, const uint8_t *fw_data, const uint16_t fw_data_size, bank_id_t bank_id);

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal length of TROPIC01's log message */
#define GET_LOG_MAX_MSG_LEN    255
/**
 * @brief Get TROPIC01's internal log message (if enabled/available)
 *
 * @param h           Device's handle
 * @param log_msg     Log message
 * @param msg_len_max Max possible length of TROPIC01's log message
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_get_log(lt_handle_t *h, uint8_t *log_msg, uint16_t msg_len_max);

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal length of Ping command message */
#define PING_LEN_MAX                     (L3_CMD_DATA_SIZE_MAX - L3_CMD_ID_SIZE)

/**
 * @brief Test secure session by exchanging a message with chip
 *  msg_out of length 'len' is echoed through secure channel.
 *
 * @param h           Device's handle
 * @param msg_out     Ping message going out
 * @param msg_in      Ping message going in
 * @param len         Length of both messages (msg_out and msg_in)
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_ping(lt_handle_t *h, const uint8_t *msg_out, uint8_t *msg_in, const uint16_t len);

//--------------------------------------------------------------------------------------------------------------------//
/** @brief ECC key slot indexes */
typedef enum {
    SH0PUB = 0, SH1PUB,  SH2PUB,  SH3PUB,
} pairing_key_slot_t;

/**
 * @brief Write pairing public key into TROPIC01's pairing key slot 0-3
 *
 * @param h           Device's handle
 * @param pubkey      32B of pubkey
 * @param slot        Pairing key lot SH0PUB - SH3PUB
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_pairing_key_write(lt_handle_t *h, const uint8_t *pubkey, const uint8_t slot);

//--------------------------------------------------------------------------------------------------------------------//
/**
 * @brief Read pairing public key from TROPIC01's pairing key slot 0-3
 *
 * @param h           Device's handle
 * @param pubkey      32B of pubkey
 * @param slot        Pairing key lot SH0PUB - SH3PUB
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_pairing_key_read(lt_handle_t *h, uint8_t *pubkey, const uint8_t slot);

//--------------------------------------------------------------------------------------------------------------------//
/**
 * @brief Invalidate pairing key in slo 0-3
 *
 * @param h           Device's handle
 * @param slot        Pairing key lot SH0PUB - SH3PUB
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_pairing_key_invalidate(lt_handle_t *h, const uint8_t slot);

//--------------------------------------------------------------------------------------------------------------------//
/** @brief CONFIGURATION_OBJECTS_REGISTERS memory map */
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

/**
 * @brief Write one configuration object
 *
 * @param h           Device's handle
 * @param addr        Address of a config object
 * @param obj         Content to be written
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_r_config_write(lt_handle_t *h, enum CONFIGURATION_OBJECTS_REGS addr, const uint32_t obj);

/**
 * @brief Read one configuration object
 *
 * @param h           Device's handle
 * @param addr        Address of a config object
 * @param obj         Variable to read content into
 * @return            LT_OK if success, otherwise returns other error code.
 *
 */
lt_ret_t lt_r_config_read(lt_handle_t *h, const enum CONFIGURATION_OBJECTS_REGS addr, uint32_t *obj);

/**
 * @brief Erase the whole config space
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_r_config_erase(lt_handle_t *h);

/**
 * @brief Write one I config object
 *
 * @param h           Device's handle
 * @param addr        Address of a config object
 * @param bit_index   Index of bit to write from 1 to 0
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_i_config_write(lt_handle_t *h, const enum CONFIGURATION_OBJECTS_REGS addr, const uint8_t bit_index);

/**
 * @brief Read one I config object
 *
 * @param h           Device's handle
 * @param addr        Address of a config object
 * @param obj         Variable to read content into
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_i_config_read(lt_handle_t *h, const enum CONFIGURATION_OBJECTS_REGS addr, uint32_t *obj);

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal size of one data slot */
#define R_MEM_DATA_SIZE_MAX                    (444)
/** @brief Index of last data slot */
#define R_MEM_DATA_SLOT_MAX                    (511)
/**
 * @brief Write bytes into a given slot of R MEMORY
 *
 * @param h           Device's handle
 * @param udata_slot  Memory's slot to be written
 * @param data        Buffer of data to be written into R MEMORY slot
 * @param size        Size of data to be read
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_r_mem_data_write(lt_handle_t *h, const uint16_t udata_slot, uint8_t *data, const uint16_t size);

//--------------------------------------------------------------------------------------------------------------------//
/**
 * @brief Read bytes from a given slot of R MEMORY
 *
 * @param h           Device's handle
 * @param udata_slot  Memory's slot to be read
 * @param data        Buffer to read data into
 * @param size        Size of data to be read
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_r_mem_data_read(lt_handle_t *h, const uint16_t udata_slot, uint8_t *data, const uint16_t size);

//--------------------------------------------------------------------------------------------------------------------//
/**
 * @brief Erase bytes from a given slot of R MEMORY
 *
 * @param h           Device's handle
 * @param udata_slot  Memory's slot to be erased
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_r_mem_data_erase(lt_handle_t *h, const uint16_t udata_slot);

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximum number of random bytes requested at once */
#define RANDOM_VALUE_GET_LEN_MAX         255

/**
 * @brief Get number of random bytes
 *
 * @param h           Device's handle
 * @param buff        Buffer
 * @param len         Number of random bytes
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_random_get(lt_handle_t *h, uint8_t *buff, const uint16_t len);

//--------------------------------------------------------------------------------------------------------------------//
/** @brief ECC key slot indexes */
typedef enum {
    ECC_SLOT_0 = 0, ECC_SLOT_1, ECC_SLOT_2,  ECC_SLOT_3,
    ECC_SLOT_4, ECC_SLOT_5, ECC_SLOT_6,  ECC_SLOT_7,
    ECC_SLOT_8, ECC_SLOT_9, ECC_SLOT_10, ECC_SLOT_11,
    ECC_SLOT_12, ECC_SLOT_13, ECC_SLOT_14, ECC_SLOT_15,
    ECC_SLOT_16, ECC_SLOT_17, ECC_SLOT_18, ECC_SLOT_19,
    ECC_SLOT_20, ECC_SLOT_21, ECC_SLOT_22, ECC_SLOT_23,
    ECC_SLOT_24, ECC_SLOT_25, ECC_SLOT_26, ECC_SLOT_27,
    ECC_SLOT_28, ECC_SLOT_29, ECC_SLOT_30, ECC_SLOT_31,
} ecc_slot_t;

/** @brief ECC key type */
typedef enum {
    CURVE_P256 = 1,
    CURVE_ED25519
} lt_ecc_curve_type_t;

/** @brief ECC key origin */
typedef enum {
    CURVE_GENERATED = 1,
    CURVE_STORED
} ecc_key_origin_t;

/**
 * @brief Generate ECC key in the device's ECC key slot
 *
 * @param h           Device's handle
 * @param slot        Slot number ecc_slot_t
 * @param curve       Type of ECC curve. Use L3_ECC_KEY_GENERATE_CURVE_ED25519 or L3_ECC_KEY_GENERATE_CURVE_P256
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_ecc_key_generate(lt_handle_t *h, const ecc_slot_t slot, const lt_ecc_curve_type_t curve);

/**
 * @brief Store ECC key in the device's ECC key slot
 *
 * @param h           Device's handle
 * @param slot        Slot number ecc_slot_t
 * @param curve       Type of ECC curve. Use L3_ECC_KEY_GENERATE_CURVE_ED25519 or L3_ECC_KEY_GENERATE_CURVE_P256
 * @param key         Key to store
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_ecc_key_store(lt_handle_t *h, const ecc_slot_t slot, const lt_ecc_curve_type_t curve, const uint8_t *key);

/**
 * @brief Read ECC public key corresponding to a private key in device's slot
 *
 * @param h           Device's handle
 * @param slot        Slot number ECC_SLOT_1 - ECC_SLOT_32
 * @param key         Buffer for retrieving a key
 * @param keylen      Length of the key's buffer
 * @param curve       Will be filled by curve byte
 * @param origin      Will be filled by origin byte
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_ecc_key_read(lt_handle_t *h, const ecc_slot_t slot, uint8_t *key, const uint8_t keylen, lt_ecc_curve_type_t *curve, ecc_key_origin_t *origin);

/**
 * @brief Erase ECC key from device's slot
 *
 * @param h           Device's handle
 * @param slot        Slot number ECC_SLOT_1 - ECC_SLOT_32
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_ecc_key_erase(lt_handle_t *h, const ecc_slot_t slot);

/**
 * @brief ECDSA sign message with a private key stored in TROPIC01 device
 *
 * @param h           Device's handle
 * @param slot        Slot containing a private key, ECC_SLOT_1 - ECC_SLOT_32
 * @param msg         Buffer containing a message
 * @param msg_len     Length of msg's buffer
 * @param rs          Buffer for storing a signature in a form of R and S bytes
 * @param rs_len      Length of rs buffer should be 64B
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_ecc_ecdsa_sign(lt_handle_t *h, const ecc_slot_t slot, const uint8_t *msg, const uint16_t msg_len, uint8_t *rs, const uint8_t rs_len);

/**
 * @brief EdDSA sign message with a private key stored in TROPIC01 device
 *
 * @param h           Device's handle
 * @param slot        Slot containing a private key, ECC_SLOT_1 - ECC_SLOT_32
 * @param msg         Buffer containing a message to sign, max length is 4096B
 * @param msg_len     Length of a message
 * @param rs          Buffer for storing a silt_r_mem_data_erasewise returns other error code. TODO info about other ret values
 *
 */
lt_ret_t lt_ecc_eddsa_sign(lt_handle_t *h, const ecc_slot_t slot, const uint8_t *msg, const uint16_t msg_len, uint8_t *rs, const uint8_t rs_len);

/**
 * @brief EdDSA signature verify
 *
 * @param msg         Message
 * @param msg_len     Length of message. Max length is 4095
 * @param pubkey      Public key related to private key which signed the message
 * @param rs          Signature to be verified, in a form of R and S bytes
 * @return            LT_OK if success, otherwise returns other error code. TODO info about other ret values
 *
 */
lt_ret_t lt_ecc_eddsa_sig_verify(const uint8_t *msg, const uint16_t msg_len, const uint8_t *pubkey, const uint8_t *rs);

//--------------------------------------------------------------------------------------------------------------------//

enum lt_mcounter_index_t {
    MCOUNTER_INDEX_0 = 0, MCOUNTER_INDEX_1 = 1, MCOUNTER_INDEX_2 = 2, MCOUNTER_INDEX_3 = 3,
    MCOUNTER_INDEX_4 = 4, MCOUNTER_INDEX_5 = 5, MCOUNTER_INDEX_6 = 6, MCOUNTER_INDEX_7 = 7,
    MCOUNTER_INDEX_8 = 8, MCOUNTER_INDEX_9 = 9, MCOUNTER_INDEX_10 = 10, MCOUNTER_INDEX_11 = 11,
    MCOUNTER_INDEX_12 = 12, MCOUNTER_INDEX_13 = 13, MCOUNTER_INDEX_14 = 14, MCOUNTER_INDEX_15 = 15
};

/**
 * @brief Initialize monotonic counter of a given index
 *
 * @param h               Device's handle
 * @param mcounter_index  Index of monotonic counter
 * @param mcounter_value  Value to set as an initial value
 * @return                LT_OK if success, otherwise returns other error code. TODO info about other ret values
 */
lt_ret_t lt_mcounter_init(lt_handle_t *h,  const enum lt_mcounter_index_t mcounter_index, const uint32_t mcounter_value);

/**
 * @brief Update monotonic counter of a given index
 *
 * @param h               Device's handle
 * @param mcounter_index  Index of monotonic counter
 * @return                LT_OK if success, otherwise returns other error code. TODO info about other ret values
 */
lt_ret_t lt_mcounter_update(lt_handle_t *h,  const enum lt_mcounter_index_t mcounter_index);

/**
 * @brief Get a value of a monotonic counter of a given index
 *
 * @param h               Device's handle
 * @param mcounter_index  Index of monotonic counter
 * @param mcounter_value  Value of monotonic counter
 * @return                LT_OK if success, otherwise returns other error code. TODO info about other ret values
 */
lt_ret_t lt_mcounter_get(lt_handle_t *h,  const enum lt_mcounter_index_t mcounter_index, uint32_t *mcounter_value);

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal size of returned MAC-and-Destroy data */
#define MAC_AND_DESTROY_DATA_SIZE 32u

/** @brief Mac-and-Destroy slot indexes */
typedef enum {
    MAC_AND_DESTROY_SLOT_0 = 0, MAC_AND_DESTROY_SLOT_1, MAC_AND_DESTROY_SLOT_2,   MAC_AND_DESTROY_SLOT_3,
    MAC_AND_DESTROY_SLOT_4,   MAC_AND_DESTROY_SLOT_5,   MAC_AND_DESTROY_SLOT_6,   MAC_AND_DESTROY_SLOT_7,
    MAC_AND_DESTROY_SLOT_8,   MAC_AND_DESTROY_SLOT_9,   MAC_AND_DESTROY_SLOT_10,  MAC_AND_DESTROY_SLOT_11,
    MAC_AND_DESTROY_SLOT_12,  MAC_AND_DESTROY_SLOT_13,  MAC_AND_DESTROY_SLOT_14,  MAC_AND_DESTROY_SLOT_15,
    MAC_AND_DESTROY_SLOT_16,  MAC_AND_DESTROY_SLOT_17,  MAC_AND_DESTROY_SLOT_18,  MAC_AND_DESTROY_SLOT_19,
    MAC_AND_DESTROY_SLOT_20,  MAC_AND_DESTROY_SLOT_21,  MAC_AND_DESTROY_SLOT_22,  MAC_AND_DESTROY_SLOT_23,
    MAC_AND_DESTROY_SLOT_24,  MAC_AND_DESTROY_SLOT_25,  MAC_AND_DESTROY_SLOT_26,  MAC_AND_DESTROY_SLOT_27,
    MAC_AND_DESTROY_SLOT_28,  MAC_AND_DESTROY_SLOT_29,  MAC_AND_DESTROY_SLOT_30,  MAC_AND_DESTROY_SLOT_31,
    MAC_AND_DESTROY_SLOT_32,  MAC_AND_DESTROY_SLOT_33,  MAC_AND_DESTROY_SLOT_34,  MAC_AND_DESTROY_SLOT_35,
    MAC_AND_DESTROY_SLOT_36,  MAC_AND_DESTROY_SLOT_37,  MAC_AND_DESTROY_SLOT_38,  MAC_AND_DESTROY_SLOT_39,
    MAC_AND_DESTROY_SLOT_40,  MAC_AND_DESTROY_SLOT_41,  MAC_AND_DESTROY_SLOT_42,  MAC_AND_DESTROY_SLOT_43,
    MAC_AND_DESTROY_SLOT_44,  MAC_AND_DESTROY_SLOT_45,  MAC_AND_DESTROY_SLOT_46,  MAC_AND_DESTROY_SLOT_47,
    MAC_AND_DESTROY_SLOT_48,  MAC_AND_DESTROY_SLOT_49,  MAC_AND_DESTROY_SLOT_50,  MAC_AND_DESTROY_SLOT_51,
    MAC_AND_DESTROY_SLOT_52,  MAC_AND_DESTROY_SLOT_53,  MAC_AND_DESTROY_SLOT_54,  MAC_AND_DESTROY_SLOT_55,
    MAC_AND_DESTROY_SLOT_56,  MAC_AND_DESTROY_SLOT_57,  MAC_AND_DESTROY_SLOT_58,  MAC_AND_DESTROY_SLOT_59,
    MAC_AND_DESTROY_SLOT_60,  MAC_AND_DESTROY_SLOT_61,  MAC_AND_DESTROY_SLOT_62,  MAC_AND_DESTROY_SLOT_63,
    MAC_AND_DESTROY_SLOT_64,  MAC_AND_DESTROY_SLOT_65,  MAC_AND_DESTROY_SLOT_66,  MAC_AND_DESTROY_SLOT_67,
    MAC_AND_DESTROY_SLOT_68,  MAC_AND_DESTROY_SLOT_69,  MAC_AND_DESTROY_SLOT_70,  MAC_AND_DESTROY_SLOT_71,
    MAC_AND_DESTROY_SLOT_72,  MAC_AND_DESTROY_SLOT_73,  MAC_AND_DESTROY_SLOT_74,  MAC_AND_DESTROY_SLOT_75,
    MAC_AND_DESTROY_SLOT_76,  MAC_AND_DESTROY_SLOT_77,  MAC_AND_DESTROY_SLOT_78,  MAC_AND_DESTROY_SLOT_79,
    MAC_AND_DESTROY_SLOT_80,  MAC_AND_DESTROY_SLOT_81,  MAC_AND_DESTROY_SLOT_82,  MAC_AND_DESTROY_SLOT_83,
    MAC_AND_DESTROY_SLOT_84,  MAC_AND_DESTROY_SLOT_85,  MAC_AND_DESTROY_SLOT_86,  MAC_AND_DESTROY_SLOT_87,
    MAC_AND_DESTROY_SLOT_88,  MAC_AND_DESTROY_SLOT_89,  MAC_AND_DESTROY_SLOT_90,  MAC_AND_DESTROY_SLOT_91,
    MAC_AND_DESTROY_SLOT_92,  MAC_AND_DESTROY_SLOT_93,  MAC_AND_DESTROY_SLOT_94,  MAC_AND_DESTROY_SLOT_95,
    MAC_AND_DESTROY_SLOT_96,  MAC_AND_DESTROY_SLOT_97,  MAC_AND_DESTROY_SLOT_98,  MAC_AND_DESTROY_SLOT_99,
    MAC_AND_DESTROY_SLOT_100, MAC_AND_DESTROY_SLOT_101, MAC_AND_DESTROY_SLOT_102, MAC_AND_DESTROY_SLOT_103,
    MAC_AND_DESTROY_SLOT_104, MAC_AND_DESTROY_SLOT_105, MAC_AND_DESTROY_SLOT_106, MAC_AND_DESTROY_SLOT_107,
    MAC_AND_DESTROY_SLOT_108, MAC_AND_DESTROY_SLOT_109, MAC_AND_DESTROY_SLOT_110, MAC_AND_DESTROY_SLOT_111,
    MAC_AND_DESTROY_SLOT_112, MAC_AND_DESTROY_SLOT_113, MAC_AND_DESTROY_SLOT_114, MAC_AND_DESTROY_SLOT_115,
    MAC_AND_DESTROY_SLOT_116, MAC_AND_DESTROY_SLOT_117, MAC_AND_DESTROY_SLOT_118, MAC_AND_DESTROY_SLOT_119,
    MAC_AND_DESTROY_SLOT_120, MAC_AND_DESTROY_SLOT_121, MAC_AND_DESTROY_SLOT_122, MAC_AND_DESTROY_SLOT_123,
    MAC_AND_DESTROY_SLOT_124, MAC_AND_DESTROY_SLOT_125, MAC_AND_DESTROY_SLOT_126, MAC_AND_DESTROY_SLOT_127
} mac_and_destroy_slot_t;

/**
 * @brief Execute the MAC-and-Destroy sequence.
 *
 * @param h           Device's handle
 * @param slot        Mac-and-Destroy slot index, valid values are 0-127
 * @param data_out    Data to be sent into chip
 * @param data_in     Mac and destroy returned data
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_mac_and_destroy(lt_handle_t *h, mac_and_destroy_slot_t slot, const uint8_t *data_out, uint8_t *data_in);

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal size of returned serial code */
#define SERIAL_CODE_SIZE 32u

/**
 * @brief Get serial code field
 *
 * @param h           Device's handle
 * @param serial_code Serial code bytes
 * @param size        Size of buffer to read into
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_serial_code_get(lt_handle_t *h, uint8_t *serial_code, const uint16_t size);

/**
 * @details Helper function for printing out name of returned value
 *
 * @param ret lt_ret_t returned type value
 * @return const char* description of return value.
 */
const char *lt_ret_verbose(lt_ret_t ret);

//---------------------------------------------------------------------------------------------------------------------//

/** @brief Macro to controll which session can access command targeting content of pairing key SH0 */
#define TO_PAIRING_KEY_SH0(x)     ((x) << 0)
/** @brief Macro to controll which session can access command targeting content of pairing key SH1 */
#define TO_PAIRING_KEY_SH1(x)     ((x) << 8)
/** @brief Macro to controll which session can access command targeting content of pairing key SH2 */
#define TO_PAIRING_KEY_SH2(x)     ((x) << 16)
/** @brief Macro to controll which session can access command targeting content of pairing key SH3 */
#define TO_PAIRING_KEY_SH3(x)     ((x) << 24)

/** @brief Macro to controll which session can access command targeting counter 0-3 */
#define TO_LT_MCOUNTER_0_3(x)    ((x) << 0)
/** @brief Macro to controll which session can access command targeting counter 4-7 */
#define TO_LT_MCOUNTER_4_7(x)    ((x) << 8)
/** @brief Macro to controll which session can access command targeting counter 8-11 */
#define TO_LT_MCOUNTER_8_11(x)   ((x) << 16)
/** @brief Macro to controll which session can access command targeting counter 12-15 */
#define TO_LT_MCOUNTER_12_15(x)  ((x) << 24)

/** @brief Macro to controll which session can access command targeting ECC_SLOT 0-7 */
#define TO_ECC_KEY_SLOT_0_7(x)   ((x) << 0)
/** @brief Macro to controll which session can access command targeting ECC_SLOT 8-15 */
#define TO_ECC_KEY_SLOT_8_15(x)  ((x) << 8)
/** @brief Macro to controll which session can access command targeting ECC_SLOT 16-23 */
#define TO_ECC_KEY_SLOT_16_23(x) ((x) << 16)
/** @brief Macro to controll which session can access command targeting ECC_SLOT 24-31 */
#define TO_ECC_KEY_SLOT_24_31(x) ((x) << 24)

/** @brief Macro to controll if session SH0 has access to a specific command */
#define SESSION_SH0_HAS_ACCESS    (uint8_t)0x01
/** @brief Macro to controll if session SH1 has access to a specific command */
#define SESSION_SH1_HAS_ACCESS    (uint8_t)0x02
/** @brief Macro to controll if session SH2 has access to a specific command */
#define SESSION_SH2_HAS_ACCESS    (uint8_t)0x04
/** @brief Macro to controll if session SH3 has access to a specific command */
#define SESSION_SH3_HAS_ACCESS    (uint8_t)0x08

/** @brief This structure is used in this example to simplify looping
 *         through all config addresses and printing out them into debug */
struct lt_config_obj_desc_t {
    char desc[60];
    enum CONFIGURATION_OBJECTS_REGS addr;
};

/** @brief Structure to hold all configuration objects */
struct lt_config_t {
    uint32_t obj[27];
};

#ifdef LT_UTILS
/**
 * @brief Get the conf desc object from config description table
 *
 * @param i           Index in an array of objects
 * @return uint16_t   String with name of object on a given index
 */
const char *get_conf_desc(uint8_t i);

/**
 * @brief Get the address of a specific config object from config description table
 *
 * @param i           Index in an array of objects
 * @return uint16_t   Address of object on a given index
 */
uint16_t get_conf_addr(uint8_t i);

/**
 * @brief This function reads config objects from TROPIC01 and prints them out
 *
 * @param h           Device's handle
 * @param config      Array into which objects are read
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t write_whole_R_config(lt_handle_t *h, const struct lt_config_t *config);

/**
 * @brief This function writes config objecs into TROPIC01
 *
 * @param h           Device's handle
 * @param config      Array from which objects are taken and written into TROPIC01
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t read_whole_R_config(lt_handle_t *h, struct lt_config_t *config);

/**
 * @brief This function establish a secure channel between host MCU and TROPIC01 chip
 *
 * @param h           Device's handle
 * @param shipriv     Host's private pairing key (SHiPUB)
 * @param shipub      Host's public pairing key  (SHiPUB)
 * @param pkey_index  Pairing key's index
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t verify_chip_and_start_secure_session(lt_handle_t *h, uint8_t *shipriv, uint8_t *shipub, uint8_t pkey_index);

#endif

/** @} */ // end of group_libtropic_API

#endif
