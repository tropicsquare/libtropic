#ifndef TS_L2_H
#define TS_L2_H

/**
* @file ts_l2.h
* @brief Layer 2 interfaces
* @author Tropic Square s.r.o.
*/

#include "ts_common.h"

/** Chip status READY bit mask. If CHIP_STATUS byte contains this bit, chip is ready. */
#define CHIP_STATUS_READY          0x01
/** Chip status ALARM bit mask. If CHIP_STATUS byte contains this bit, chip is in ALARM mode. */
#define CHIP_STATUS_ALARM          0x02
/** Chip status START bit mask. If CHIP_STATUS byte contains this bit, chip is in START mode.  */
#define CHIP_STATUS_START          0x04


// L2 Requests
/** "Get_info" request */
#define L2_GET_INFO_REQ_ID         0x01
#define L2_GET_INFO_REQ_LEN        0x02

/** Handshake_Req request */
#define L2_HANDSHAKE_REQ_ID        0x02
#define L2_HANDSHAKE_REQ_LEN       0x21

/** Encrypted_Cmd_Req request */
#define L2_ENCRYPTED_CMD_REQ_ID    0x04




/** STATUS ﬁeld value */
#define L2_STATUS_REQUEST_OK       0x01
/** STATUS ﬁeld value */
#define L2_STATUS_RESULT_OK        0x02
/** STATUS ﬁeld value */
#define L2_STATUS_REQUEST_CONT     0x03
/** STATUS ﬁeld value */
#define L2_STATUS_RESULT_CONT      0x04
/** STATUS ﬁeld value */
#define L2_STATUS_HSK_ERR          0x79
/** STATUS ﬁeld value */
#define L2_STATUS_NO_SESSION       0x7A
/** STATUS ﬁeld value */
#define L2_STATUS_TAG_ERR          0x7B
/** STATUS ﬁeld value */
#define L2_STATUS_CRC_ERR          0x7C
/** STATUS ﬁeld value */
#define L2_STATUS_UNKNOWN_ERR      0x7E
/** STATUS ﬁeld value */
#define L2_STATUS_GEN_ERR          0x7F
/** STATUS ﬁeld value */
#define L2_STATUS_NO_RESP          0xFF



/** Get_Info_Req */
#define L2_GET_INFO_REQ_OBJ_ID_X509                  0x00
#define L2_GET_INFO_REQ_OBJ_ID_CHIP_ID               0x01
#define L2_GET_INFO_REQ_OBJ_ID_RISCV_FW_VERSION      0x02
#define L2_GET_INFO_REQ_OBJ_ID_SPECT_FW_VERSION      0x04
#define L2_GET_INFO_REQ_OBJ_ID_FW_BANK               0xb0

#define L2_GET_INFO_REQ_DATA_CHUNK_0_127             0x00
#define L2_GET_INFO_REQ_DATA_CHUNK_128_255           0x01
#define L2_GET_INFO_REQ_DATA_CHUNK_256_383           0x02
#define L2_GET_INFO_REQ_DATA_CHUNK_384_511           0x03

#define L2_GET_INFO_REQ_LEN_MAX          128


/**
 * @defgroup get_info_req_group get_info
 * Request to execute a Secure Channel Handshake and establish a new Secure Channel Session (TROPIC01 moves to Secure Channel Mode).
 * @{
 */

/**
 * @brief Command ID
 */
#define TS_L2_GET_INFO_REQ 0x01

struct l2_get_info_req_t {
    /*
    * Request ID  byte
    */
    u8 req_id;

    /*
    * Length byte
    */
    u8 req_len;

    /*
    The Identifier of the requested object.
    */
    # define TS_L2_GET_INFO_REQ_OBJECT_ID_X509_CERTIFICATE 0 // The X.509 chip certificate read from I-Memory and signed by Tropic Square (max length of 512B).
    # define TS_L2_GET_INFO_REQ_OBJECT_ID_CHIP_ID 1 // The chip ID - the chip silicon revision and unique device ID (max length of 128B).
    # define TS_L2_GET_INFO_REQ_OBJECT_ID_RISCV_FW_VERSION 2 // The RISCV current running FW version (4 Bytes)
    # define TS_L2_GET_INFO_REQ_OBJECT_ID_SPECT_FW_VERSION 4 // The SPECT FW version (4 Bytes)
    # define TS_L2_GET_INFO_REQ_OBJECT_ID_FW_BANK 176 // The FW header read from the selected bank id (shown as an index). Supported only in Start-up mode.
    u8 obj_id;

    /*
    The index of the 128 Byte long block to request
    */
    # define TS_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_0_127 0 // Request for data bytes 0-127 of the object.
    # define TS_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_128_255 1 // Request for data bytes 128-255 of the object (only needed for the X.509 certificate).
    # define TS_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_256_383 2 // Request for data bytes 128-383 of object (only needed for the X.509 certificate).
    # define TS_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_384_511 3 // Request for data bytes 384-511 of object (only needed for the X.509 certificate).
    u8 block_index;

    /*
     * Checksum
    */
    uint8_t crc[2];
} __attribute__((__packed__));


struct l2_get_info_rsp_t{
    /*
    * CHIP_STATUS byte
    */
    u8 chip_status;

    /*
    * l2 status byte
    */
    u8 status;

    /*
    * Length of incomming data
    */
    u8 rsp_len;

    /*
    * The data content of the requested object block.
    */
    u8 data[128];

    /*
    * Checksum
    */
    u8 crc[2];
} __attribute__((__packed__));


/** @} */ // end of get_info_req_group


/**
 * @brief Corresponds to $S_{H0Pub}$.
 */
# define TS_L2_HANDSHAKE_REQ_PKEY_INDEX_PAIRING_KEY_SLOT_0 0
/**
 * @brief Corresponds to $S_{H1Pub}$.
 */
# define TS_L2_HANDSHAKE_REQ_PKEY_INDEX_PAIRING_KEY_SLOT_1 1
/**
 * @brief Corresponds to $S_{H2Pub}$.
 */
# define TS_L2_HANDSHAKE_REQ_PKEY_INDEX_PAIRING_KEY_SLOT_2 2
/**
 * @brief Corresponds to $S_{H3Pub}$.
 */
# define TS_L2_HANDSHAKE_REQ_PKEY_INDEX_PAIRING_KEY_SLOT_3 3

/**
 * @details Handshake_Req request object
 */
struct l2_handshake_req_t{
    uint8_t req_id;
    uint8_t req_len;
    uint8_t e_hpub[32];
    uint8_t pkey_index;
    uint8_t crc[2];
}__attribute__((packed));

/**
 * @details Handshake_Req response object
 */
struct l2_handshake_rsp_t{
    uint8_t chip_status;
    uint8_t status;
    uint8_t rsp_len;
    uint8_t e_tpub[32];
    uint8_t t_auth[16];
    uint8_t crc[2];
}__attribute__((packed));

/**
 * @details L2 data frame going from host into chip
 */
struct l2_encrypted_req_t {
    uint8_t req_id;
    uint8_t req_len;
    uint8_t body[L2_CHUNK_MAX_DATA_SIZE];
    uint8_t crc[2];
}__attribute__((packed));

/**
 * @details L2 data frame going into host from chip
 */
struct l2_encrypted_rsp_t {
    uint8_t chip_status;
    uint8_t status;
    uint8_t resp_len;
    uint8_t body[L2_CHUNK_MAX_DATA_SIZE];
    uint8_t crc[2];
}__attribute__((packed));

/**
 * @brief This function checks if incomming L2 frame is valid
 *
 * @param             frame
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_l2_frame_check(const uint8_t *frame);

/**
 * @brief
 *
 * @param h           Chip's handle
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_l2_transfer(ts_handle_t *h);

/**
 * @brief This function executes generic L3 command. It expects command's data correctly encrypted using keys created during previsously called ts_l2_handshake_req()
 *
 * @param h           Chip's handle
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_l2_encrypted_cmd(ts_handle_t *h);

#endif
