#ifndef TS_L3_H
#define TS_L3_H

/**
* @file ts_l3.h
* @brief This file contains interfaces related to layer 3.
* @author Tropic Square s.r.o.
*/

#include "ts_common.h"
#include "ts_l2.h"

/** L3 RESULT ﬁeld Value */
#define L3_RESULT_OK                        0xC3u
/** L3 RESULT ﬁeld Value */
#define L3_RESULT_FAIL                      0x3Cu
/** L3 RESULT ﬁeld Value */
#define L3_RESULT_UNAUTHORIZED              0x01u
/** L3 RESULT ﬁeld Value */
#define L3_RESULT_INVALID_CMD               0x02u


// L3 Commands
/** "Ping" command */
//#define L3_PING_CMD_ID                      0x01u
// len min?
#define L3_PING_MSG_MAX_LEN                 L3_CMD_DATA_SIZE

/** "Random_Value_Get" command */
//#define L3_RANDOM_VALUE_GET_CMD_ID          0x50u
#define L3_RANDOM_VALUE_GET_LEN_MIN         0x00u
#define L3_RANDOM_VALUE_GET_LEN_MAX         L2_CHUNK_MAX_DATA_SIZE

/** "ECC_Key_Generate" command */
//#define L3_ECC_KEY_GENERATE_CMD_ID          0x60u
#define L3_ECC_KEY_GENERATE_CMD_LEN         0x04u
#define L3_ECC_KEY_GENERATE_SLOT_MIN        0x01u
#define L3_ECC_KEY_GENERATE_SLOT_MAX        0x20u
#define L3_ECC_KEY_GENERATE_CURVE_P256      0x01u
#define L3_ECC_KEY_GENERATE_CURVE_ED25519   0x02u

/** "ECC_Key_Read" command */
//#define L3_ECC_KEY_READ_CMD_ID              0x62u

/** "ECDSA_Sign" command */
#define L3_ECDSA_SIGN_CMD_ID                0x70u
#define L3_ECDSA_SIGN_CMD_LEN               0x30u
#define L3_ECDSA_SIGN_MSG_LEN               0x20u

/** "EDDSA_Sign" command */
//#define L3_EDDSA_SIGN_CMD_ID                0x71u
#define L3_EDDSA_SIGN_CMD_LEN               0x10u
#define L3_EDDSA_SIGN_MSG_LEN_MIN           0x01u
#define L3_EDDSA_SIGN_MSG_LEN_MAX           4096u

/** "ECC_Key_Erase" command */
#define L3_ECC_KEY_ERASE_CMD_ID             0x63u
#define L3_ECC_KEY_ERASE_CMD_LEN            0x03u


/**
 * @defgroup l3_ping
 * Comment: ping command
 * @{
 */

/**
 * @brief Command ID
 */
#define L3_PING_CMD_ID 0x01

struct ts_l3_ping_cmd_t {
    /**
    * < L3 packet size
    */
    u16 packet_size;

    /**
    * < L3 Command Identifier
    */
    u8 command;

    /**
    * < Data transferred from host into chip
    */
    u8 data[4096];

    /**
    * < L3 tag
    */
    u8 tag[16];

} __attribute__((__packed__));


struct ts_l3_ping_res_t{
    /**
    * < L3 packet size
    */
    u16 packet_size;

    /**
    * < L3 Result status indication
    */
    u8 result;

    /**
    * < Data transferred from chip into host
    */
    u8 data[4096];

    /**
    * < L3 tag
    */
    u8 tag[16];
} __attribute__((__packed__));


/** @} */ // end of l3_ping


/**
 * @defgroup l3_random_value_get
 * Comment: Random value get
 * @{
 */

/**
 * @brief Command ID
 */
#define TS_L3_RANDOM_VALUE_GET 0x50

struct ts_l3_random_value_get_cmd_t {
    /**
    * < L3 packet size
    */
    u16 packet_size;

    /**
    * < L3 Command Identifier
    */
    u8 command;

    /**
    * < The number of random bytes to get
    */
    u8 n_bytes;

    /**
    * < L3 tag
    */
    u8 tag[16];

} __attribute__((__packed__));


struct ts_l3_random_value_get_res_t{
    /**
    * < L3 packet size
    */
    u16 packet_size;

    /**
    * < L3 Result status indication
    */
    u8 result;

    /**
    * < The padding by dummy data
    */
    u8 padding[3];

    /**
    * < The random data from TRNG2 in the number of bytes specified in the N_BYTES L3 Field
    */
    u8 random_data[255];

    /**
    * < L3 tag
    */
    u8 tag[16];
} __attribute__((__packed__));


/** @} */ // end of l3_random_value_get



/**
 * @defgroup ecc_key_generate
 * Comment: ECC key generate
 * @{
 */

/**
 * @brief Command ID
 */
#define TS_L3_ECC_KEY_GENERATE 0x60

struct ts_l3_ecc_key_generate_cmd_t {
    /**
    * < L3 packet size
    */
    u16 packet_size;

    /**
    * < L3 Command Identifier
    */
    u8 command;

    /**
    * < The slot to write the generated key. Valid values are 0 - 31
    */
    #define ECC_SLOT_1                              1
    #define ECC_SLOT_2                              2
    #define ECC_SLOT_3                              3
    #define ECC_SLOT_4                              4
    #define ECC_SLOT_5                              5
    #define ECC_SLOT_6                              6
    #define ECC_SLOT_7                              7
    #define ECC_SLOT_8                              8
    #define ECC_SLOT_9                              9
    #define ECC_SLOT_10                            10
    #define ECC_SLOT_11                            11
    #define ECC_SLOT_12                            12
    #define ECC_SLOT_13                            13
    #define ECC_SLOT_14                            14
    #define ECC_SLOT_15                            15
    #define ECC_SLOT_16                            16
    #define ECC_SLOT_17                            17
    #define ECC_SLOT_18                            18
    #define ECC_SLOT_19                            19
    #define ECC_SLOT_20                            20
    #define ECC_SLOT_21                            21
    #define ECC_SLOT_22                            22
    #define ECC_SLOT_23                            23
    #define ECC_SLOT_24                            24
    #define ECC_SLOT_25                            25
    #define ECC_SLOT_26                            26
    #define ECC_SLOT_27                            27
    #define ECC_SLOT_28                            28
    #define ECC_SLOT_29                            29
    #define ECC_SLOT_30                            30
    #define ECC_SLOT_31                            31
    #define ECC_SLOT_32                            32
    u16 slot;

    /**
    * < The Elliptic Curve the key is generated from
    */
    # define TS_L3_ECC_KEY_GENERATE_CURVE_P256 1 // P256 Curve - 64-byte long public key.
    # define TS_L3_ECC_KEY_GENERATE_CURVE_ED25519 2 // Ed25519 Curve - 32-byte long public key.
    u8 curve;

    /**
    * < L3 tag
    */
    u8 tag[16];

} __attribute__((__packed__));


struct ts_l3_ecc_key_generate_res_t{
    /**
    * < L3 packet size
    */
    u16 packet_size;

    /**
    * < L3 Result status indication
    */
    u8 result;

    /**
    * < L3 tag
    */
    u8 tag[16];
} __attribute__((__packed__));


/** @} */ // end of ecc_key_generate



/**
 * @defgroup ecc_key_read
 * Comment: Read ECC key
 * @{
 */

/**
 * @brief Command ID
 */
#define TS_L3_ECC_KEY_READ 0x62

struct ts_l3_ecc_key_read_cmd_t {
    /**
    * < L3 packet size
    */
    u16 packet_size;

    /**
    * < L3 Command Identifier
    */
    u8 command;

    /**
    * < ECC Key slot
    */
    u16 slot;

    /**
    * < L3 tag
    */
    u8 tag[16];

} __attribute__((__packed__));


struct ts_l3_ecc_key_read_res_t{
    /**
    * < L3 packet size
    */
    u16 packet_size;

    /**
    * < L3 Result status indication
    */
    u8 result;

    /**
    * < Elliptic Curve
    */
    # define TS_L3_ECC_KEY_READ_CURVE_P256 1 // P256 Curve - 64-byte long public key.
    # define TS_L3_ECC_KEY_READ_CURVE_ED25519 2 // Ed25519 Curve - 32-byte long public key.
    u8 curve;

    /**
    * < The origin of the key
    */
    # define TS_L3_ECC_KEY_READ_ORIGIN_ECC_KEY_GENERATE 1 // The key is from key generation on the device.
    # define TS_L3_ECC_KEY_READ_ORIGIN_ECC_KEY_STORE 2 // The key is from key storage in the device.
    u8 origin;

    /**
    * < Padding
    */
    u8 padding[13];

    /**
    * < The public key from the ECC Key slot as specified in the SLOT L3 Field
    */
    u8 pub_key[64];

    /**
    * < L3 tag
    */
    u8 tag[16];
} __attribute__((__packed__));


/** @} */ // end of ecc_key_read



/**
 * @defgroup eddsa_sign
 * Comment: Sign with EDDSA key
 * @{
 */

/**
 * @brief Command ID
 */
#define TS_L3_EDDSA_SIGN 0x71

struct ts_l3_eddsa_sign_cmd_t {
    /**
    * < L3 packet size
    */
    u16 packet_size;

    /**
    * < L3 Command Identifier
    */
    u8 command;

    /**
    * < ECC Key slot
    */
    u16 slot;

    /**
    * < Padding
    */
    u8 padding[13];

    /**
    * < Message to sign
    */
    u8 msg[4096];

    /**
    * < L3 tag
    */
    u8 tag[16];

} __attribute__((__packed__));


struct ts_l3_eddsa_sign_res_t{
    /**
    * < L3 packet size
    */
    u16 packet_size;

    /**
    * < L3 Result status indication
    */
    u8 result;

    /**
    * < Padding
    */
    u8 padding[15];

    /**
    * < EdDSA signature - The R part
    */
    u8 r[32];

    /**
    * < EdDSA signature - The S part
    */
    u8 s[32];

    /**
    * < L3 tag
    */
    u8 tag[16];
} __attribute__((__packed__));


/** @} */ // end of eddsa_sign



/**
 * @defgroup ecdsa_sign
 * Comment: Sign with ECDSA key
 * @{
 */

/**
 * @brief Command ID
 */
#define TS_L3_ECDSA_SIGN 0x70

struct ts_l3_ecdsa_sign_cmd_t {
    /**
    * < L3 packet size
    */
    u16 packet_size;

    /**
    * < L3 Command Identifier
    */
    u8 command;

    /**
    * < ECC Key slot
    */
    u16 slot;

    /**
    * < Padding
    */
    u8 padding[13];

    /**
    * < Message to sign
    */
    u8 msg_hash[32];

    /**
    * < L3 tag
    */
    u8 tag[16];

} __attribute__((__packed__));


struct ts_l3_ecdsa_sign_res_t{
    /**
    * < L3 packet size
    */
    u16 packet_size;

    /**
    * < L3 Result status indication
    */
    u8 result;

    /**
    * < Padding
    */
    u8 padding[15];

    /**
    * < EdDSA signature - The R part
    */
    u8 r[32];

    /**
    * < EdDSA signature - The S part
    */
    u8 s[32];

    /**
    * < L3 tag
    */
    u8 tag[16];
} __attribute__((__packed__));


/** @} */ // end of ecdsa_sign




/**
 * @details  Initializes nonce in handle to 0. This function is used during secure handshake.
 *
 * @param h           Chip's handle
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_l3_nonce_init(ts_handle_t *h);

/**
 * @details Increases by one nonce stored in handle. This function is used after successfull reception of L3 response.
 * , uint16_t cmd_len,
 * @param h           Chip's handle
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_l3_nonce_increase(ts_handle_t *h);

/**
 * @details Perform l3 encrypted command operation.
 *
 * @param h           Chip's handle
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_l3_cmd(ts_handle_t *h);

#endif
