#ifndef TS_L3_API_H
#define TS_L3_API_H

/**
* @file ts_l3_api.h
* @brief Layer 3 API structures for various requests
* @author Tropic Square s.r.o.
*/

#include "ts_common.h"


/**
 * @defgroup group_L3_API [L3 API]
 * @brief Secure Session Layer
 * @details This layer uses encrypted Command/Result packets
 * @ingroup group_private_API
 *
 * @{
 */

/**
 * @defgroup group_l3_ping Ping
 * @brief Ping command
 * @details Run arbitrary message through the secure session to verify that all works.
 * @{
 */

/** @brief Command ID */
#define TS_L3_PING_CMD 0x01

/** This structure declares how "Ping command" l3 packet is organized */
struct ts_l3_ping_cmd_t {
    /** L3 packet size */
    u16 packet_size;
    /** L3 Command Identifier */
    u8 command;
    /** Data transferred from host into chip */
    u8 data[4096];
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

/** This structure declares how "Ping result" l3 packet is organized */
struct ts_l3_ping_res_t{
    /** L3 packet size */
    u16 packet_size;
    /** L3 Result status indication */
    u8 result;
    /** Data transferred from chip into host */
    u8 data[4096];
    /* L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

/** @} */ // end of group_l3_ping

/**
 * @defgroup group_l3_random_value_get Random_Value_Get
 * @brief Get random bytes from TROPIC01
 * @details Uses TRNG2 inside of TROPIC01
 * @{
 */

/** @brief Command ID */
#define TS_L3_RANDOM_VALUE_GET_CMD 0x50
/** @brief Command length */
#define TS_L3_RANDOM_VALUE_GET_CMD_SIZE 2

/** This structure declares how "random value get command" l3 packet is organized */
struct ts_l3_random_value_get_cmd_t {
    /* L3 packet size */
    u16 packet_size;
    /** L3 Command Identifier */
    u8 command;
    /** The number of random bytes to get */
    u8 n_bytes;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

/** This structure declares how "random value get result" l3 packet is organized */
struct ts_l3_random_value_get_res_t{
    /** L3 packet size */
    u16 packet_size;
    /** L3 Result status indication */
    u8 result;
    /** The padding by dummy data */
    u8 padding[3];
    /** The random data from TRNG2 in the number of bytes specified in the N_BYTES L3 Field */
    u8 random_data[255];
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

/** @} */ // end of group_l3_random_value_get

/**
 * @defgroup group_ecc_key_generate ECC_Key_Generate
 * @brief Generate ECC key
 * @details Generate ECC private key inside of TROPIC01's memory slot.
 * @{
 */

/** @brief ECC_Key_generate command ID */
#define TS_L3_ECC_KEY_GENERATE_CMD 0x60
/** @brief ECC_Key_generate command size */
#define TS_L3_ECC_KEY_GENERATE_CMD_SIZE 0x04u
/** @brief ECC_Key_generate min slot number */
#define TS_L3_ECC_KEY_GENERATE_SLOT_MIN 0
/** @brief ECC_Key_generate max slot number */
#define TS_L3_ECC_KEY_GENERATE_SLOT_MAX 31

/** This structure declares how "ECC key generate command" l3 packet is organized */
struct ts_l3_ecc_key_generate_cmd_t {
    /** L3 packet size */
    u16 packet_size;
    /** L3 Command Identifier */
    u8 command;
    /** The slot to write the generated key. Valid values are 0 - 31 */
    u16 slot;
    /** The Elliptic Curve the key is generated from */
    u8 curve;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

/** This structure declares how "ECC key generate result" l3 packet is organized */
struct ts_l3_ecc_key_generate_res_t{
    /** L3 packet size */
    u16 packet_size;
    /** L3 Result status indication */
    u8 result;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

/** @} */ // end of group_ecc_key_generate

/**
 * @defgroup group_ecc_key_read ECC_Key_Read
 * @brief Read ECC key
 * @details Read ECC public key which corresponds to TROPIC01's memory slot.
 * @{
 */

/** @brief Command ID */
#define TS_L3_ECC_KEY_READ_CMD 0x62
/** @brief Command length */
#define TS_L3_ECC_KEY_READ_CMD_SIZE 0x03

/** This structure declares how "ECC key read command" l3 packet is organized */
struct ts_l3_ecc_key_read_cmd_t {
    /**L3 packet size */
    u16 packet_size;
    /** L3 Command Identifier */
    u8 command;
    /** ECC Key slot */
    u16 slot;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

/** This structure declares how "ECC key read result" l3 packet is organized */
struct ts_l3_ecc_key_read_res_t{
    /** L3 packet size */
    u16 packet_size;
    /** L3 Result status indication */
    u8 result;
    /** Elliptic Curve */
    u8 curve;
    /** The origin of the key */
    u8 origin;
    /** Padding */
    u8 padding[13];
    /** The public key from the ECC Key slot as specified in the SLOT L3 Field */
    u8 pub_key[64];
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

/** @} */ // end of group_ecc_key_read

/**
 * @defgroup group_ecc_key_erase ECC_Key_Erase
 * @brief Erase ECC key
 * @details Erase ECC private key in a given TROPIC01's memory slot
 * @{
 */

/** @brief Command ID */
#define TS_L3_ECC_KEY_ERASE_CMD 0x63u
/** @brief Command length */
#define TS_L3_ECC_KEY_ERASE_CMD_SIZE 0x03u

/** This structure declares how "EC key erase command" l3 packet is organized */
struct ts_l3_ecc_key_erase_cmd_t {
    /** L3 packet size */
    u16 packet_size;
    /** L3 Command Identifier */
    u8 command;
    /** ECC Key slot */
    u16 slot;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

/** This structure declares how "EC key erase result" l3 packet is organized */
struct ts_l3_ecc_key_erase_res_t{
    /** L3 packet size */
    u16 packet_size;
    /** L3 Result status indication */
    u8 result;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

/** @} */ // end of group_ecc_key_erase

/**
 * @defgroup group_eddsa_sign EDDSA_Sign
 * @brief Sign with EDDSA key
 * @details Use private key from a given slot to sign a message
 * @{
 */

/** @brief Command ID */
#define TS_L3_EDDSA_SIGN_CMD 0x71
/** @brief Command length */
#define TS_L3_EDDSA_SIGN_CMD_SIZE 0x10u
/** Minimal length of message to be signed with EdDSA */
#define TS_L3_EDDSA_SIGN_MSG_LEN_MIN 0x01
/** Maximal length of message to be signed with EdDSA */
#define TS_L3_EDDSA_SIGN_MSG_LEN_MAX 4096

/** This structure declares how "EdDSA sign command" l3 packet is organized */
struct ts_l3_eddsa_sign_cmd_t {
    /** L3 packet size */
    u16 packet_size;
    /** L3 Command Identifier */
    u8 command;
    /** ECC Key slot */
    u16 slot;
    /** Padding */
    u8 padding[13];
    /** Message to sign */
    u8 msg[4096];
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

/** This structure declares how "EdDSA sign result" l3 packet is organized */
struct ts_l3_eddsa_sign_res_t{
    /** L3 packet size */
    u16 packet_size;
    /** L3 Result status indication */
    u8 result;
    /** Padding */
    u8 padding[15];
    /** EdDSA signature - The R part */
    u8 r[32];
    /** EdDSA signature - The S part */
    u8 s[32];
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

/** @} */ // end of group_eddsa_sign

/**
 * @defgroup group_ecdsa_sign ECDSA_Sign
 * @brief Sign with ECDSA key
 * @details Use private key from a given slot to sign a message
 * @{
 */

/** @brief Command ID */
#define TS_L3_ECDSA_SIGN 0x70
/** @brief Command length */
#define TS_L3_ECDSA_SIGN_CMD_SIZE 0x30u
/** TODO Max length of message to be signed with ECDSA */
#define TS_L3_ECDSA_SIGN_MSG_LEN_MAX 4096 

/** This structure declares how "ECDSA sign command" l3 packet is organized */
struct ts_l3_ecdsa_sign_cmd_t {
    /** L3 packet size */
    u16 packet_size;
    /** L3 Command Identifier */
    u8 command;
    /** ECC Key slot */
    u16 slot;
    /** Padding */
    u8 padding[13];
    /** Message to sign */
    u8 msg_hash[32];
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

/** This structure declares how "ECDSA sign result" l3 packet is organized */
struct ts_l3_ecdsa_sign_res_t{
    /** L3 packet size */
    u16 packet_size;
    /** L3 Result status indication */
    u8 result;
    /** Padding */
    u8 padding[15];
    /** EdDSA signature - The R part */
    u8 r[32];
    /** EdDSA signature - The S part */
    u8 s[32];
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

/** @} */ // end of group_ecdsa_sign

/** @} */ // end of group_L3_API

#endif
