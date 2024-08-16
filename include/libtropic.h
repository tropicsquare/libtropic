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

/**
 * @brief Initialize handle and transport layer
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_init(lt_handle_t *h);


/**
 * @brief Deinitialize handle and transport layer
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_deinit(lt_handle_t *h);

/** @brief PAiring key indexes corresponds to S_HiPub */
typedef enum {
    PAIRING_KEY_SLOT_INDEX_0,
    PAIRING_KEY_SLOT_INDEX_1,
    PAIRING_KEY_SLOT_INDEX_2,
    PAIRING_KEY_SLOT_INDEX_3,
} pkey_index_t;

/**
 * @brief This function provides secure handshake functionality.
 *
 * @param h           Device's handle
 * @param stpub       STPUB from device's certificate
 * @param pkey_index  Index of pairing public key
 * @param shipriv     Secure host private key
 * @param shipub      Secure host public key
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_handshake(lt_handle_t *h, const uint8_t *stpub, const pkey_index_t pkey_index, const uint8_t *shipriv, const uint8_t *shipub);

/** @brief Maximal length of Ping command message */
#define PING_LEN_MAX                     L3_CMD_DATA_SIZE_MAX

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

/** @brief ECC key slot indexes */
typedef enum {
    ECC_SLOT_1 = 1, ECC_SLOT_2,  ECC_SLOT_3,  ECC_SLOT_4,
    ECC_SLOT_5,     ECC_SLOT_6,  ECC_SLOT_7,  ECC_SLOT_8,
    ECC_SLOT_9,     ECC_SLOT_10, ECC_SLOT_11, ECC_SLOT_12, 
    ECC_SLOT_13,    ECC_SLOT_14, ECC_SLOT_15, ECC_SLOT_16,
    ECC_SLOT_17,    ECC_SLOT_18, ECC_SLOT_19, ECC_SLOT_20,
    ECC_SLOT_21,    ECC_SLOT_22, ECC_SLOT_23, ECC_SLOT_24,
    ECC_SLOT_25,    ECC_SLOT_26, ECC_SLOT_27, ECC_SLOT_28,
    ECC_SLOT_29,    ECC_SLOT_30, ECC_SLOT_31, ECC_SLOT_32,
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
lt_ret_t lt_ecc_key_read(lt_handle_t *h, const ecc_slot_t slot, uint8_t *key, const int8_t keylen, ecc_curve_type_t *curve, ecc_key_origin_t *origin);

/**
 * @brief EdDSA sign message with a private key stored in TROPIC01 device
 *
 * @param h           Device's handle
 * @param slot        Slot containing a private key, ECC_SLOT_1 - ECC_SLOT_32
 * @param msg         Buffer containing a message to sign, max length is 4096B
 * @param msg_len     Length of a message
 * @param rs          Buffer for storing a signature in a form of R and S bytes
 * @param rs_len      Length of rs buffer should be 64B
 * @return            LT_OK if success, otherwise returns other error code.
 *
 */
lt_ret_t lt_ecc_eddsa_sign(lt_handle_t *h, const ecc_slot_t slot, const uint8_t *msg, const int16_t msg_len, uint8_t *rs, const int8_t rs_len);

/**
 * @brief EdDSA signature verify
 *
 * @param msg         Message
 * @param msg_len     Length of message. Max length is 4095
 * @param pubkey      Public key related to private key which signed the message
 * @param rs          Signature to be verified, in a form of R and S bytes
 * @return            LT_OK if success, otherwise returns other error code.
 *
 */
lt_ret_t lt_ecc_eddsa_sig_verify(const uint8_t *msg, const uint16_t msg_len, const uint8_t *pubkey, const uint8_t *rs);

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
lt_ret_t lt_ecc_ecdsa_sign(lt_handle_t *h, const ecc_slot_t slot, const uint8_t *msg_hash, const int16_t msg_hash_len, uint8_t *rs, const int8_t rs_len);

/**
 * @brief Erase ECC key from device's slot
 *
 * @param h           Device's handle
 * @param slot        Slot number ECC_SLOT_1 - ECC_SLOT_32
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_ecc_key_erase(lt_handle_t *h, const ecc_slot_t slot);

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
lt_ret_t lt_get_info_cert(lt_handle_t *h, uint8_t *cert, const int16_t max_len);

/**
 * @brief Verify certificate chain and parse STPUB
 *
 * @param cert        Certificate in DER format
 * @param max_len     Len of certificate buffer
 * @param stpub       TROPIC01 STPUB, unique for each device
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_cert_verify_and_parse(const uint8_t *cert, const int16_t max_len, uint8_t *stpub);

/**
 * @details Helper function for printing out name of returned value
 *
 * @param ret lt_ret_t returned type value
 * @return const char* description of return value.
 */
const char *lt_ret_verbose(lt_ret_t ret);

/** @} */ // end of group_libtropic_API

// TODO
//lt_ret_t lt_get_info_chip_id(lt_handle_t *h, uint8_t chip_id, uint16_t max_len);
//lt_ret_t lt_get_info_riscv_fw_ver(lt_handle_t *h, uint8_t ver, uint16_t max_len);
//lt_ret_t lt_get_info_spect_fw_ver(lt_handle_t *h, uint8_t ver, uint16_t max_len);
//lt_ret_t lt_get_info_fw_bank(lt_handle_t *h, uint8_t fw_bank, uint16_t max_len);

#endif
