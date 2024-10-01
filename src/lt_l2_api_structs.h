#ifndef LT_L2_API_H
#define LT_L2_API_H

/**
 * @file lt_l2_api_structs.h
 * @brief API structures for layer 2 requests and responses
 * @author Tropic Square s.r.o.
 * 
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic_common.h"

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Request's ID */
#define LT_L2_GET_INFO_REQ_ID 0x01
/** @brief Length of this request */
#define LT_L2_GET_INFO_REQ_LEN 0x02

/** This structure declares how "get info request" l2 frame is organized */
struct lt_l2_get_info_req_t {
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
struct lt_l2_get_info_rsp_t{
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

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Request's ID */
#define LT_L2_HANDSHAKE_REQ_ID 0x02
/** @brief Length of this request */
#define LT_L2_HANDSHAKE_REQ_LEN 0x21

/** This structure declares how "handshake request" l2 frame is organized */
struct lt_l2_handshake_req_t {
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
struct lt_l2_handshake_rsp_t{
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

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Request's ID */
#define LT_L2_ENCRYPTED_CMD_REQ_ID 0x04

/** This structure declares how "encrypted cmd request" l2 frame is organized */
struct lt_l2_encrypted_cmd_req_t {
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
struct lt_l2_encrypted_cmd_rsp_t{
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

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Request's ID */
#define LT_L2_ENCRYPTED_SESSION_ABT_REQ_ID 0x08

/** This structure declares how "encrypted_session_abt request" l2 frame is organized */
struct lt_l2_encrypted_session_abt_req_t {
    /** Request ID  byte */
    u8 req_id;
    /** Length byte */
    u8 req_len;
    /** Checksum */
    uint8_t crc[2];
} __attribute__((__packed__));

/** This structure declares how "encrypted_session_abt response" l2 frame is organized */
struct lt_l2_encrypted_session_abt_rsp_t{
    /** CHIP_STATUS byte */
    u8 chip_status;
    /** l2 status byte */
    u8 status;
    /** Length of incomming data */
    u8 rsp_len;
    /** Checksum */
    u8 crc[2];
} __attribute__((__packed__));

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Request's ID */
#define LT_L2_STARTUP_REQ_ID 0xB3
#define LT_L2_STARTUP_REQ_LEN 0x1

/** This structure declares how "startup_req request" l2 frame is organized */
struct lt_l2_startup_req_t {
    /** Request ID  byte */
    u8 req_id;
    /** Length byte */
    u8 req_len;
    /** Length byte */
    u8 startup_id;
    /** Checksum */
    uint8_t crc[2];
} __attribute__((__packed__));

/** This structure declares how "startup_req response" l2 frame is organized */
struct lt_l2_startup_rsp_t{
    /** CHIP_STATUS byte */
    u8 chip_status;
    /** l2 status byte */
    u8 status;
    /** Length of incomming data */
    u8 rsp_len;
    /** Checksum */
    u8 crc[2];
} __attribute__((__packed__));

#endif
