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
/** @brief Maximal size of certificate */
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
#define LT_L2_GET_INFO_FW_HEADER_SIZE       512
/**
 * @brief Read TROPIC01's fw bank info
 *
 * @param h           Device's handle
 * @param header      Buffer to store fw header bytes into
 * @param max_len     Length of a buffer
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_get_info_fw_bank(lt_handle_t *h, uint8_t *header, const uint16_t max_len);

//--------------------------------------------------------------------------------------------------------------------//
/** @brief PAiring key indexes corresponds to S_HiPub */
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
#define LT_L2_STARTUP_ID_REBOOT 0x01
#define LT_L2_STARTUP_ID_MAINTENANCE 0x03

/**
 * @brief Reboot TROPIC01
 *
 * @param h           Device's handle
 * @param startup_id  Startup ID
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_reboot(lt_handle_t *h, const uint8_t startup_id);

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal length of Ping command message */
#define PING_LEN_MAX                     (L3_CMD_DATA_SIZE_MAX - L3_CMD_ID_SIZE)

/**
 * @brief Test secure session by exchanging a message with chip
 *
 *
 * @param h           Device's handle
 * @param msg_out     Ping message going out
 * @param msg_in      Ping message going in
 * @param len         Length of ping message
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
#define R_MEM_DATA_SIZE_MAX                    (444)
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
#define RANDOM_VALUE_GET_LEN_MAX         L2_CHUNK_MAX_DATA_SIZE

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
} ecc_curve_type_t;

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
lt_ret_t lt_ecc_key_generate(lt_handle_t *h, const ecc_slot_t slot, const ecc_curve_type_t curve);

/**
 * @brief Store ECC key in the device's ECC key slot
 *
 * @param h           Device's handle
 * @param slot        Slot number ecc_slot_t
 * @param curve       Type of ECC curve. Use L3_ECC_KEY_GENERATE_CURVE_ED25519 or L3_ECC_KEY_GENERATE_CURVE_P256
 * @param key         Key to store
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_ecc_key_store(lt_handle_t *h, const ecc_slot_t slot, const ecc_curve_type_t curve, const uint8_t *key);

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
lt_ret_t lt_ecc_key_read(lt_handle_t *h, const ecc_slot_t slot, uint8_t *key, const uint8_t keylen, ecc_curve_type_t *curve, ecc_key_origin_t *origin);

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
 * @param msg_hash    Buffer containing hash of a message
 * @param msg_hash_len Length of hash's buffer should be 32B
 * @param rs          Buffer for storing a signature in a form of R and S bytes
 * @param rs_len      Length of rs buffer should be 64B
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_ecc_ecdsa_sign(lt_handle_t *h, const ecc_slot_t slot, const uint8_t *msg_hash, const uint16_t msg_hash_len, uint8_t *rs, const uint8_t rs_len);

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


lt_ret_t lt_mcounter_init(lt_handle_t *h,  const uint16_t mcounter_index, const uint32_t mcounter_value);
lt_ret_t lt_mcounter_update(lt_handle_t *h,  const uint16_t mcounter_index);
lt_ret_t lt_mcounter_get(lt_handle_t *h,  const uint16_t mcounter_index, uint32_t *mcounter_value);



//--------------------------------------------------------------------------------------------------------------------//
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

/** @} */ // end of group_libtropic_API

#endif
