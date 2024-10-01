#ifndef LT_L3_API_H
#define LT_L3_API_H

/**
* @file lt_l3_api_structs.h
* @brief API structures for layer 3 commands and results
* @author Tropic Square s.r.o.
*/

#include "libtropic_common.h"

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Command ID */
#define LT_L3_PING_CMD 0x01

/** This structure declares how "Ping command" l3 packet is organized */
struct lt_l3_ping_cmd_t {
    /** L3 packet size */
    u16 cmd_size;
    /** L3 Command Identifier */
    u8 cmd_id;
    /** Data transferred from host into chip */
    u8 data[4096];
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

/** This structure declares how "Ping result" l3 packet is organized */
struct lt_l3_ping_res_t{
    /** L3 packet size */
    u16 res_size;
    /** L3 Result status indication */
    u8 result;
    /** Data transferred from chip into host */
    u8 data[4096];
    /* L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Command ID */
#define LT_L3_PAIRING_KEY_WRITE_CMD 0x10u
#define LT_L3_PAIRING_KEY_WRITE_CMD_SIZE 0x24u

struct lt_l3_pairing_key_write_cmd_t {
    /** L3 packet size */
    u16 cmd_size;
    /** L3 Command Identifier */
    u8 cmd_id;
    /** Pairing key slot */
    u16 slot;
    /** Padding  */
    u8 padding;
    /** Pairing key bytes */
    u8 key[32];
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

struct lt_l3_pairing_key_write_res_t{
    /** L3 packet size */
    u16 res_size;
    /** L3 Result status indication */
    u8 result;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Command ID */
#define LT_L3_PAIRING_KEY_READ_CMD 0x11u
#define LT_L3_PAIRING_KEY_READ_CMD_SIZE 0x3u

struct lt_l3_pairing_key_read_cmd_t {
    /** L3 packet size */
    u16 cmd_size;
    /** L3 Command Identifier */
    u8 cmd_id;
    /** Pairing key slot */
    u16 slot;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

#define LT_L3_PAIRING_KEY_READ_RES_SIZE 0x24u
struct lt_l3_pairing_key_read_res_t{
    /** L3 packet size */
    u16 res_size;
    /** L3 Result status indication */
    u8 result;

    u8 padding[3];
    u8 key[32];

    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Command ID */
#define LT_L3_PAIRING_KEY_INVALIDATE_CMD 0x12u
#define LT_L3_PAIRING_KEY_INVALIDATE_CMD_SIZE 0x3u

struct lt_l3_pairing_key_invalidate_cmd_t {
    /** L3 packet size */
    u16 cmd_size;
    /** L3 Command Identifier */
    u8 cmd_id;
    /** Pairing key slot */
    u16 slot;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

struct lt_l3_pairing_key_invalidate_res_t{
    /** L3 packet size */
    u16 res_size;
    /** L3 Result status indication */
    u8 result;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Command ID */
#define LT_L3_R_CONFIG_WRITE 0x20
/** @brief Command length */
#define LT_L3_R_CONFIG_WRITE_SIZE 0x8u

/**  */
struct lt_l3_r_config_write_cmd_t {
    /** L3 packet size */
    u16 cmd_size;
    /** L3 Command Identifier */
    u8 command;
    /** CO offset address */
    u16 address;
    /** Padding */
    u8 padding;
    /**  */
    u32 value;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

/**  */
struct lt_l3_r_config_write_res_t{
    /** L3 packet size */
    u16 res_size;
    /** L3 Result status indication */
    u8 result;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Command ID */
#define LT_L3_R_CONFIG_READ 0x21
/** @brief Command length */
#define LT_L3_R_CONFIG_READ_SIZE 0x3u

/**  */
struct lt_l3_r_config_read_cmd_t {
    /** L3 packet size */
    u16 cmd_size;
    /** L3 Command Identifier */
    u8 command;
    /** CO offset address */
    u16 address;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

/**  */
struct lt_l3_r_config_read_res_t{
    /** L3 packet size */
    u16 res_size;
    /** L3 Result status indication */
    u8 result;
    /** Padding */
    u8 padding[3];
    /** Value */
    u32 value;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Command ID */
#define LT_L3_R_CONFIG_ERASE 0x22
/** @brief Command length */
#define LT_L3_R_CONFIG_ERASE_SIZE 0x1u

/**  */
struct lt_l3_r_config_erase_cmd_t {
    /** L3 packet size */
    u16 cmd_size;
    /** L3 Command Identifier */
    u8 command;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

/**  */
struct lt_l3_r_config_erase_res_t{
    /** L3 packet size */
    u16 res_size;
    /** L3 Result status indication */
    u8 result;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Command ID */
#define LT_L3_I_CONFIG_WRITE 0x30
/** @brief Command length */
#define LT_L3_I_CONFIG_WRITE_SIZE 0x4u

/**  */
struct lt_l3_i_config_write_cmd_t {
    /** L3 packet size */
    u16 cmd_size;
    /** L3 Command Identifier */
    u8 command;
    /** CO offset address */
    u16 address;
    /** Padding */
    u8 bit_index;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

/**  */
struct lt_l3_i_config_write_res_t{
    /** L3 packet size */
    u16 res_size;
    /** L3 Result status indication */
    u8 result;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Command ID */
#define LT_L3_I_CONFIG_READ 0x31
/** @brief Command length */
#define LT_L3_I_CONFIG_READ_SIZE 0x3u

/**  */
struct lt_l3_i_config_read_cmd_t {
    /** L3 packet size */
    u16 cmd_size;
    /** L3 Command Identifier */
    u8 command;
    /** CO offset address */
    u16 address;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

/**  */
struct lt_l3_i_config_read_res_t{
    /** L3 packet size */
    u16 res_size;
    /** L3 Result status indication */
    u8 result;
    /** Padding */
    u8 padding[3];
    /** Value */
    u32 value;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Command ID */
#define LT_L3_R_MEM_DATA_WRITE_CMD 0x40u
//#define LT_L3_R_MEM_DATA_WRITE_CMD_SIZE 0x3u

struct lt_l3_r_mem_data_write_cmd_t {
    /** L3 packet size */
    u16 cmd_size;
    /** L3 Command Identifier */
    u8 cmd_id;
    /** Pairing key slot */
    u16 slot;
    u8 padding;
    u8 data[444];
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

struct lt_l3_r_mem_data_write_res_t{
    /** L3 packet size */
    u16 res_size;
    /** L3 Result status indication */
    u8 result;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Command ID */
#define LT_L3_R_MEM_DATA_READ_CMD 0x41u
#define LT_L3_R_MEM_DATA_READ_CMD_SIZE 0x3u

struct lt_l3_r_mem_data_read_cmd_t {
    /** L3 packet size */
    u16 cmd_size;
    /** L3 Command Identifier */
    u8 cmd_id;
    /** Pairing key slot */
    u16 slot;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

struct lt_l3_r_mem_data_read_res_t{
    /** L3 packet size */
    u16 res_size;
    /** L3 Result status indication */
    u8 result;
    u8 padding[3];
    u8 data[444];
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Command ID */
#define LT_L3_R_MEM_DATA_ERASE_CMD 0x42u
#define LT_L3_R_MEM_DATA_ERASE_CMD_SIZE 0x3u

struct lt_l3_r_mem_data_erase_cmd_t {
    /** L3 packet size */
    u16 cmd_size;
    /** L3 Command Identifier */
    u8 cmd_id;
    /** Pairing key slot */
    u16 slot;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

struct lt_l3_r_mem_data_erase_res_t{
    /** L3 packet size */
    u16 res_size;
    /** L3 Result status indication */
    u8 result;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Command ID */
#define LT_L3_RANDOM_VALUE_GET_CMD 0x50
/** @brief Command length */
#define LT_L3_RANDOM_VALUE_GET_CMD_SIZE 2

/** This structure declares how "random value get command" l3 packet is organized */
struct lt_l3_random_value_get_cmd_t {
    /* L3 packet size */
    u16 cmd_size;
    /** L3 Command Identifier */
    u8 cmd_id;
    /** The number of random bytes to get */
    u8 n_bytes;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

/** This structure declares how "random value get result" l3 packet is organized */
struct lt_l3_random_value_get_res_t{
    /** L3 packet size */
    u16 res_size;
    /** L3 Result status indication */
    u8 result;
    /** The padding by dummy data */
    u8 padding[3];
    /** The random data from TRNG2 in the number of bytes specified in the N_BYTES L3 Field */
    u8 random_data[255];
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

//--------------------------------------------------------------------------------------------------------------------//
/** @brief ECC_Key_generate command ID */
#define LT_L3_ECC_KEY_GENERATE_CMD 0x60
/** @brief ECC_Key_generate command size */
#define LT_L3_ECC_KEY_GENERATE_CMD_SIZE 0x04u
/** @brief ECC_Key_generate min slot number */
#define LT_L3_ECC_KEY_GENERATE_SLOT_MIN 0
/** @brief ECC_Key_generate max slot number */
#define LT_L3_ECC_KEY_GENERATE_SLOT_MAX 31

/** This structure declares how "ECC key generate command" l3 packet is organized */
struct lt_l3_ecc_key_generate_cmd_t {
    /** L3 packet size */
    u16 cmd_size;
    /** L3 Command Identifier */
    u8 cmd_id;
    /** The slot to write the generated key. Valid values are 0 - 31 */
    u16 slot;
    /** The Elliptic Curve the key is generated from */
    u8 curve;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

/** This structure declares how "ECC key generate result" l3 packet is organized */
struct lt_l3_ecc_key_generate_res_t{
    /** L3 packet size */
    u16 res_size;
    /** L3 Result status indication */
    u8 result;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

//--------------------------------------------------------------------------------------------------------------------//
/** @brief ECC_Key_store command ID */
#define LT_L3_ECC_KEY_STORE_CMD 0x61
/** @brief ECC_Key_store command size */
#define LT_L3_ECC_KEY_STORE_CMD_SIZE 0x30u
/** This structure declares how "ECC key store command" l3 packet is organized */
struct lt_l3_ecc_key_store_cmd_t {
    /** L3 packet size */
    u16 cmd_size;
    /** L3 Command Identifier */
    u8 cmd_id;
    /** The slot to write the store key. Valid values are 0 - 31 */
    u16 slot;
    /** The Elliptic Curve the key is store from */
    u8 curve;
    /** Padding */
    u8 padding[12];
    /** Key to store */
    u8 key[32];
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

/** This structure declares how "ECC key store result" l3 packet is organized */
struct lt_l3_ecc_key_store_res_t{
    /** L3 packet size */
    u16 res_size;
    /** L3 Result status indication */
    u8 result;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Command ID */
#define LT_L3_ECC_KEY_READ_CMD 0x62
/** @brief Command length */
#define LT_L3_ECC_KEY_READ_CMD_SIZE 0x03

/** This structure declares how "ECC key read command" l3 packet is organized */
struct lt_l3_ecc_key_read_cmd_t {
    /**L3 packet size */
    u16 cmd_size;
    /** L3 Command Identifier */
    u8 cmd_id;
    /** ECC Key slot */
    u16 slot;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

/** This structure declares how "ECC key read result" l3 packet is organized */
struct lt_l3_ecc_key_read_res_t{
    /** L3 packet size */
    u16 res_size;
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

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Command ID */
#define LT_L3_ECC_KEY_ERASE_CMD 0x63u
/** @brief Command length */
#define LT_L3_ECC_KEY_ERASE_CMD_SIZE 0x03u

/** This structure declares how "EC key erase command" l3 packet is organized */
struct lt_l3_ecc_key_erase_cmd_t {
    /** L3 packet size */
    u16 cmd_size;
    /** L3 Command Identifier */
    u8 cmd_id;
    /** ECC Key slot */
    u16 slot;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

/** This structure declares how "EC key erase result" l3 packet is organized */
struct lt_l3_ecc_key_erase_res_t{
    /** L3 packet size */
    u16 res_size;
    /** L3 Result status indication */
    u8 result;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Command ID */
#define LT_L3_ECDSA_SIGN 0x70
/** @brief Command length */
#define LT_L3_ECDSA_SIGN_CMD_SIZE 0x30u
/** TODO Max length of message to be signed with ECDSA */
#define LT_L3_ECDSA_SIGN_MSG_LEN_MAX (4096  - L3_CMD_ID_SIZE)

/** This structure declares how "ECDSA sign command" l3 packet is organized */
struct lt_l3_ecdsa_sign_cmd_t {
    /** L3 packet size */
    u16 cmd_size;
    /** L3 Command Identifier */
    u8 cmd_id;
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
struct lt_l3_ecdsa_sign_res_t{
    /** L3 packet size */
    u16 res_size;
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

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Command ID */
#define LT_L3_EDDSA_SIGN_CMD 0x71
/** @brief Command length */
#define LT_L3_EDDSA_SIGN_CMD_SIZE 0x10u
/** Minimal length of message to be signed with EdDSA */
#define LT_L3_EDDSA_SIGN_MSG_LEN_MIN 0x01
/** Maximal length of message to be signed with EdDSA */
#define LT_L3_EDDSA_SIGN_MSG_LEN_MAX 4096

/** This structure declares how "EdDSA sign command" l3 packet is organized */
struct lt_l3_eddsa_sign_cmd_t {
    /** L3 packet size */
    u16 cmd_size;
    /** L3 Command Identifier */
    u8 cmd_id;
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
struct lt_l3_eddsa_sign_res_t{
    /** L3 packet size */
    u16 res_size;
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

//--------------------------------------------------------------------------------------------------------------------//
/*

MCounter_Init

MCounter_Update

MCounter_Get

*/

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Command ID */
#define LT_L3_SERIAL_DATA_GET_CMD 0xa0
/** @brief Command length */
#define LT_L3_SERIAL_DATA_GET_SIZE 0x01

/** This structure declares how "Serial data get command" l3 packet is organized */
struct lt_l3_serial_data_get_cmd_t {
    /**L3 packet size */
    u16 cmd_size;
    /** L3 Command Identifier */
    u8 cmd_id;
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

/** This structure declares how "Serial data get result" l3 packet is organized */
struct lt_l3_serial_data_get_res_t {
    /** L3 packet size */
    u16 res_size;
    /** L3 Result status indication */
    u8 result;
    /** Padding */
    u8 padding[3];
    /** Serial code data */
    u8 serial_code[32];
    /** L3 tag */
    u8 tag[16];
} __attribute__((__packed__));

#endif
