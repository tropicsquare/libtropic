#ifndef TS_L2_API_H
#define TS_L2_API_H

/**
* @file ts_l2_api.h
* @brief Layer 2 API structures for various requests
* @author Tropic Square s.r.o.
*/

#include "ts_common.h"

/**
 * @defgroup group_private_API [PRIVATE API]
 *
 * @details Dear users, please DO NOT USE this API. Private API is used by libtropic internally.
 * @{
 */

/**
 * @defgroup group_L2_API [L2 API]
 * @brief Data Link Layer
 * @details This layer uses unencrypted Request/Response packets
 *
 * @{
 */

/**
 * @defgroup group_get_info_req get_info_req
 *
 * @brief Get some info
 * @details Get some info from the device
 *
 * @{
 */

/** @brief Command ID */
#define TS_L2_GET_INFO_REQ_ID 0x01
/** @brief Length of this request */
#define TS_L2_GET_INFO_REQ_LEN 0x02

/** This structure declares how "get info request" l2 frame is organized */
struct l2_get_info_req_t {
    /** Request ID  byte */
    u8 req_id;
    /** Length byte */
    u8 req_len;
    /** The Identifier of the requested object */
    u8 obj_id;
    /** The index of the 128 Byte long block to request */
    u8 block_index;
    /** Checksum */
    uint8_t crc[2];
} __attribute__((__packed__));

/** This structure declares how "get info response" l2 frame is organized */
struct l2_get_info_rsp_t{
    /** CHIP_STATUS byte */
    u8 chip_status;
    /** l2 status byte */
    u8 status;
    /** Length of incomming data */
    u8 rsp_len;
    /** The data content of the requested object block. */
    u8 data[128];
    /** Checksum */
    u8 crc[2];
} __attribute__((__packed__));

/** @} */ // end of group_get_info_req

/**
 * @defgroup group_handshake_req handshake_req
 * @brief Establish a secure session
 * @details Request to execute a Secure Channel Handshake and establish a new Secure Channel Session (TROPIC01 moves to Secure Channel Mode).
 * @{
 */

/** @brief Command ID */
#define TS_L2_HANDSHAKE_REQ_ID 0x02
/** @brief Length of this request */
#define TS_L2_HANDSHAKE_REQ_LEN 0x21

/** This structure declares how "handshake request" l2 frame is organized */
struct l2_handshake_req_t {
    /** Request ID  byte */
    u8 req_id;
    /** Length byte */
    u8 req_len;
    /** EHPUB key */
    uint8_t e_hpub[32];
    /** The index of corresponding pairing key */
    uint8_t pkey_index;
    /** Checksum */
    uint8_t crc[2];
} __attribute__((__packed__));


/** This structure declares how "handshake response" l2 frame is are organized */
struct l2_handshake_rsp_t{
    /** CHIP_STATUS byte */
    u8 chip_status;
    /** l2 status byte */
    u8 status;
    /** Length of incomming data */
    u8 rsp_len;
    /** ETPUB comment */
    uint8_t e_tpub[32];
    /** T_AUTH comment */
    uint8_t t_auth[16];
    /** Checksum */
    u8 crc[2];
} __attribute__((__packed__));

/** @} */ // end of group_handshake_req

/**
 * @defgroup group_encrypted_cmd_req encrypted_cmd_req
 * @brief Request to execute encrypted command
 * @details Transmission of one encrypted command may consist of multiple encrypted_cmd_req requests/responses
 * @{
 */

/** Command ID */
#define TS_L2_ENCRYPTED_CMD_REQ_ID 0x04

/** This structure declares how "encrypted cmd request" l2 frame is organized */
struct l2_encrypted_cmd_req_t {
    /** Request ID  byte */
    u8 req_id;
    /** Length byte */
    u8 req_len;
    /** Contains encrypted command */
    uint8_t body[L2_CHUNK_MAX_DATA_SIZE];
    /** Checksum */
    uint8_t crc[2];
} __attribute__((__packed__));

/** This structure declares how "encrypted cmd response" l2 frame is organized */
struct l2_encrypted_cmd_rsp_t{
    /** CHIP_STATUS byte */
    u8 chip_status;
    /** l2 status byte */
    u8 status;
    /** Length of incomming data */
    u8 rsp_len;
    /** Contains encrypted result */
    uint8_t body[L2_CHUNK_MAX_DATA_SIZE];
    /** Checksum */
    u8 crc[2];
} __attribute__((__packed__));


/** @} */ // end of group_encrypted_cmd_req

/** @} */ // end of group_L2_API

/** @} */ // end of group_private_API

#endif
