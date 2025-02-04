// GENERATED ON 2024-10-10 16:07:36.959632
// BY internal VERSION 1.6
// INPUT FILE: 20402444E43870FE5FEA170E6AADDC8A15CFA812CA74D39EEC3A8B75FA10ED56
//
// Copyright 2024 TropicSquare
// SPDX-License-Identifier: Apache-2.0

#ifndef LT_L2_API_STRUCTS_H
#define LT_L2_API_STRUCTS_H

/**
 * @file lt_l2_api_structs.h
 * @brief Layer 2 structure declaration
 * @author Tropic Square s.r.o.
 */

#include "libtropic_common.h"


/** @brief Request ID */
#define LT_L2_GET_INFO_REQ_ID 0x01
/** @brief Request length */
#define LT_L2_GET_INFO_REQ_LEN 2u
//------- Certificate -------------------------------------------------------------------------------------//
/** @brief The X.509 chip certificate read from I-Memory and signed by Tropic Square (max length of 512B). */
#define LT_L2_GET_INFO_REQ_OBJECT_ID_X509_CERTIFICATE 0x00
/** @brief Request for data bytes 0-127 of the object. */
#define LT_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_0_127 0x00
/** @brief Request for data bytes 128-255 of the object (only needed for the X.509 certificate). */
#define LT_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_128_255 0x01
/** @brief Request for data bytes 128-383 of object (only needed for the X.509 certificate). */
#define LT_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_256_383 0x02
/** @brief Request for data bytes 384-511 of object (only needed for the X.509 certificate). */
#define LT_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_384_511 0x03
/** @brief The chip ID - the chip silicon revision and unique device ID (max length of 128B). */
#define LT_L2_GET_INFO_REQ_OBJECT_ID_CHIP_ID 0x01
/** @brief The RISCV current running FW version (4 Bytes) */
#define LT_L2_GET_INFO_REQ_OBJECT_ID_RISCV_FW_VERSION 0x02
/** @brief The SPECT FW version (4 Bytes) */
#define LT_L2_GET_INFO_REQ_OBJECT_ID_SPECT_FW_VERSION 0x04
//------- FW Bank -----------------------------------------------------------------------------------------//
/** @brief The FW header read from the selected bank id (shown as an index). Supported only in Start-up mode. */
#define LT_L2_GET_INFO_REQ_OBJECT_ID_FW_BANK 0xb0


/** @brief Response length */
#define LT_L2_GET_INFO_RSP_LEN_MIN 1u

/**
 * @brief
 * Request to obtain information about TROPIC01. The type of information obtained is distinguished by OBJECT_ID.
 *
 * NOTE: If Start-up mode is active, TROPIC01 executes the immutable FW. Any version identification then has the highest bit set to 1.
 * SPECT_FW_VERSION then returns a dummy value of 0x80000000 because the SPECT FW is part of the immutable FW.
 */
struct lt_l2_get_info_req_t {
    u8 req_id; /**< Request ID byte */
    u8 req_len; /**< Length byte */
    /**
     * @brief
     * The Identifier of the requested object.
     */
    u8 object_id;
    /**
     * @brief
     * The index of the 128 Byte long block to request
     */
    u8 block_index;
    u8 crc[2]; /**< Checksum */
} __attribute__((__packed__));

/**
 * @brief
 * Request to obtain information about TROPIC01. The type of information obtained is distinguished by OBJECT_ID.
 *
 * NOTE: If Start-up mode is active, TROPIC01 executes the immutable FW. Any version identification then has the highest bit set to 1.
 * SPECT_FW_VERSION then returns a dummy value of 0x80000000 because the SPECT FW is part of the immutable FW.
 */
struct lt_l2_get_info_rsp_t {
    u8 chip_status; /**< CHIP_STATUS byte */
    u8 status; /**< L2 status byte */
    u8 rsp_len; /**< Length of incoming data */
    /**
     * @brief
     * The data content of the requested object block.
     */
    u8 object[128];
    u8 crc[2]; /**< Checksum */
} __attribute__((__packed__));


/** @brief Request ID */
#define LT_L2_HANDSHAKE_REQ_ID 0x02
/** @brief Request length */
#define LT_L2_HANDSHAKE_REQ_LEN 33u
/** @brief Corresponds to $S_{H0Pub}$. */
#define LT_L2_HANDSHAKE_REQ_PKEY_INDEX_PAIRING_KEY_SLOT_0 0x00
/** @brief Corresponds to $S_{H1Pub}$. */
#define LT_L2_HANDSHAKE_REQ_PKEY_INDEX_PAIRING_KEY_SLOT_1 0x01
/** @brief Corresponds to $S_{H2Pub}$. */
#define LT_L2_HANDSHAKE_REQ_PKEY_INDEX_PAIRING_KEY_SLOT_2 0x02
/** @brief Corresponds to $S_{H3Pub}$. */
#define LT_L2_HANDSHAKE_REQ_PKEY_INDEX_PAIRING_KEY_SLOT_3 0x03


/** @brief Response length */
#define LT_L2_HANDSHAKE_RSP_LEN 48u

/**
 * @brief
 * Request to execute a Secure Channel Handshake and establish a new Secure Channel Session (TROPIC01 moves to Secure Channel Mode).
 */
struct lt_l2_handshake_req_t {
    u8 req_id; /**< Request ID byte */
    u8 req_len; /**< Length byte */
    /**
     * @brief
     * The Host MCU's Ephemeral X25519 public key. A little endian encoding of the x-coordinate from the public Curve25519 point.
     */
    u8 e_hpub[32]; /**< Ephemeral Key of Host MCU. */
    /**
     * @brief
     * The index of the Pairing Key slot to establish a Secure Channel Session with (TROPIC01 fetches $S_{HiPub}$ from the Pairing Key slot specified in this field).
     */
    u8 pkey_index; /**< Pairing Key slot */
    u8 crc[2]; /**< Checksum */
} __attribute__((__packed__));

/**
 * @brief
 * Request to execute a Secure Channel Handshake and establish a new Secure Channel Session (TROPIC01 moves to Secure Channel Mode).
 */
struct lt_l2_handshake_rsp_t {
    u8 chip_status; /**< CHIP_STATUS byte */
    u8 status; /**< L2 status byte */
    u8 rsp_len; /**< Length of incoming data */
    /**
     * @brief
     * TROPIC01's X25519 Ephemeral key.
     */
    u8 e_tpub[32]; /**< Ephemeral Key of TROPIC01. */
    /**
     * @brief
     * The Secure Channel Handshake Authentication Tag.
     */
    u8 t_tauth[16]; /**< Authentication Tag */
    u8 crc[2]; /**< Checksum */
} __attribute__((__packed__));


/** @brief Request ID */
#define LT_L2_ENCRYPTED_CMD_REQ_ID 0x04
/** @brief Request length */
#define LT_L2_ENCRYPTED_CMD_REQ_LEN_MIN 19u
/** Minimal length of field cmd_ciphertext */
#define LT_L2_ENCRYPTED_CMD_REQ_CMD_CIPHERTEXT_LEN_MIN 1u
/** Maximal length of field cmd_ciphertext */
#define LT_L2_ENCRYPTED_CMD_REQ_CMD_CIPHERTEXT_LEN_MAX 4096u


/** @brief Response length */
#define LT_L2_ENCRYPTED_CMD_RSP_LEN_MIN 19u

/**
 * @brief
 * Request to execute an L3 Command.
 */
struct lt_l2_encrypted_cmd_req_t {
    u8 req_id; /**< Request ID byte */
    u8 req_len; /**< Length byte */
    /** Contains a chunk of encrypted command */
    uint8_t l3_chunk[255];
    u8 crc[2]; /**< Checksum */
} __attribute__((__packed__));

/**
 * @brief
 * Request to execute an L3 Command.
 */
struct lt_l2_encrypted_cmd_rsp_t {
    u8 chip_status; /**< CHIP_STATUS byte */
    u8 status; /**< L2 status byte */
    u8 rsp_len; /**< Length of incoming data */
    /**
     * @brief
     * The size of the RES_CIPHERTEXT L3 Field in bytes.
     */
    /** Contains a chunk of encrypted command */
    uint8_t l3_chunk[255];
    u8 crc[2]; /**< Checksum */
} __attribute__((__packed__));


/** @brief Request ID */
#define LT_L2_ENCRYPTED_SESSION_ABT_ID 0x08
/** @brief Request length */
#define LT_L2_ENCRYPTED_SESSION_ABT_LEN 0u


/** @brief Response length */
#define LT_L2_ENCRYPTED_SESSION_ABT_RSP_LEN 0u

/**
 * @brief
 * Request to abort current Secure Channel Session and execution of L3 command (TROPIC01 moves to Idle Mode).
 */
struct lt_l2_encrypted_session_abt_req_t {
    u8 req_id; /**< Request ID byte */
    u8 req_len; /**< Length byte */
    u8 crc[2]; /**< Checksum */
} __attribute__((__packed__));

/**
 * @brief
 * Request to abort current Secure Channel Session and execution of L3 command (TROPIC01 moves to Idle Mode).
 */
struct lt_l2_encrypted_session_abt_rsp_t {
    u8 chip_status; /**< CHIP_STATUS byte */
    u8 status; /**< L2 status byte */
    u8 rsp_len; /**< Length of incoming data */
    u8 crc[2]; /**< Checksum */
} __attribute__((__packed__));


/** @brief Request ID */
#define LT_L2_RESEND_REQ_ID 0x10
/** @brief Request length */
#define LT_L2_RESEND_REQ_LEN 0u


/** @brief Response length */
#define LT_L2_RESEND_RSP_LEN 0u

/**
 * @brief
 * Request for TROPIC01 to resend the last L2 Response.
 */
struct lt_l2_resend_req_t {
    u8 req_id; /**< Request ID byte */
    u8 req_len; /**< Length byte */
    u8 crc[2]; /**< Checksum */
} __attribute__((__packed__));

/**
 * @brief
 * Request for TROPIC01 to resend the last L2 Response.
 */
struct lt_l2_resend_rsp_t {
    u8 chip_status; /**< CHIP_STATUS byte */
    u8 status; /**< L2 status byte */
    u8 rsp_len; /**< Length of incoming data */
    u8 crc[2]; /**< Checksum */
} __attribute__((__packed__));


/** @brief Request ID */
#define LT_L2_SLEEP_REQ_ID 0x20
/** @brief Request length */
#define LT_L2_SLEEP_REQ_LEN 1u
/** @brief Sleep Mode */
#define LT_L2_SLEEP_REQ_SLEEP_KIND_SLEEP_MODE 0x05
/** @brief Deep Sleep Mode */
#define LT_L2_SLEEP_REQ_SLEEP_KIND_DEEP_SLEEP_MODE 0x0a


/** @brief Response length */
#define LT_L2_SLEEP_RSP_LEN 0u

/**
 * @brief
 * Request for TROPIC01 to go to Sleep Mode or Deep Sleep Mode.
 */
struct lt_l2_sleep_req_t {
    u8 req_id; /**< Request ID byte */
    u8 req_len; /**< Length byte */
    /**
     * @brief
     * The type of Sleep mode TROPIC01 moves to.
     */
    u8 sleep_kind; /**< Sleep Kind */
    u8 crc[2]; /**< Checksum */
} __attribute__((__packed__));

/**
 * @brief
 * Request for TROPIC01 to go to Sleep Mode or Deep Sleep Mode.
 */
struct lt_l2_sleep_rsp_t {
    u8 chip_status; /**< CHIP_STATUS byte */
    u8 status; /**< L2 status byte */
    u8 rsp_len; /**< Length of incoming data */
    u8 crc[2]; /**< Checksum */
} __attribute__((__packed__));


/** @brief Request ID */
#define LT_L2_STARTUP_REQ_ID 0xb3
/** @brief Request length */
#define LT_L2_STARTUP_REQ_LEN 1u
/** @brief Restart, then initialize as if a power-cycle was applied. */
#define LT_L2_STARTUP_REQ_STARTUP_ID_REBOOT 0x01
/** @brief Restart, then initialize. Stay in Start-up mode and do not load the mutable FW from R-Memory. */
#define LT_L2_STARTUP_REQ_STARTUP_ID_MAINTENANCE_REBOOT 0x03


/** @brief Response length */
#define LT_L2_STARTUP_RSP_LEN 0u

/**
 * @brief
 * Request for TROPIC01 to reset.
 */
struct lt_l2_startup_req_t {
    u8 req_id; /**< Request ID byte */
    u8 req_len; /**< Length byte */
    u8 startup_id; /**< The request ID */
    u8 crc[2]; /**< Checksum */
} __attribute__((__packed__));

/**
 * @brief
 * Request for TROPIC01 to reset.
 */
struct lt_l2_startup_rsp_t {
    u8 chip_status; /**< CHIP_STATUS byte */
    u8 status; /**< L2 status byte */
    u8 rsp_len; /**< Length of incoming data */
    u8 crc[2]; /**< Checksum */
} __attribute__((__packed__));


/** @brief Request ID */
#define LT_L2_MUTABLE_FW_UPDATE_REQ_ID 0xb1
/** @brief Request min length */
#define LT_L2_MUTABLE_FW_UPDATE_REQ_LEN_MIN 3u
/** Minimal length of field data */
#define LT_L2_MUTABLE_FW_UPDATE_REQ_DATA_LEN_MIN 4u
/** Maximal length of field data */
#define LT_L2_MUTABLE_FW_UPDATE_REQ_DATA_LEN_MAX 248u
/** @brief Firmware bank 1. */
#define LT_L2_MUTABLE_FW_UPDATE_REQ_BANK_ID_FW1 0x01
/** @brief Firmware bank 2 */
#define LT_L2_MUTABLE_FW_UPDATE_REQ_BANK_ID_FW2 0x02
/** @brief SPECT bank 1. */
#define LT_L2_MUTABLE_FW_UPDATE_REQ_BANK_ID_SPECT1 0x11
/** @brief SPECT bank 2 */
#define LT_L2_MUTABLE_FW_UPDATE_REQ_BANK_ID_SPECT2 0x12


/** @brief Response length */
#define LT_L2_MUTABLE_FW_UPDATE_RSP_LEN 0u

/**
 * @brief
 * Request to write a chunk of the new mutable FW to a R-Memory bank.
 * Supported only in Start-up mode (i.e. after Startup_Req with MAINTENANCE_REBOOT).
 *
 * NOTE: Write only to the correctly erased bank (see Mutable_FW_Erase_Req).
 */
struct lt_l2_mutable_fw_update_req_t {
    u8 req_id; /**< Request ID byte */
    u8 req_len; /**< Length byte */
    u8 bank_id; /**< The Identifier of the bank to write in. */
    u16 offset; /**< The offset of the specific bank to write the chunk */
    u8 data[248]; /**< The binary data to write. Data size should be a multiple of 4. */
    u8 crc[2]; /**< Checksum */
} __attribute__((__packed__));

/**
 * @brief
 * Request to write a chunk of the new mutable FW to a R-Memory bank.
 * Supported only in Start-up mode (i.e. after Startup_Req with MAINTENANCE_REBOOT).
 *
 * NOTE: Write only to the correctly erased bank (see Mutable_FW_Erase_Req).
 */
struct lt_l2_mutable_fw_update_rsp_t {
    u8 chip_status; /**< CHIP_STATUS byte */
    u8 status; /**< L2 status byte */
    u8 rsp_len; /**< Length of incoming data */
    u8 crc[2]; /**< Checksum */
} __attribute__((__packed__));


/** @brief Request ID */
#define LT_L2_MUTABLE_FW_ERASE_REQ_ID 0xb2
/** @brief Request length */
#define LT_L2_MUTABLE_FW_ERASE_REQ_LEN 1u
/** @brief Firmware bank 1. */
#define LT_L2_MUTABLE_FW_ERASE_REQ_BANK_ID_FW1 0x01
/** @brief Firmware bank 2 */
#define LT_L2_MUTABLE_FW_ERASE_REQ_BANK_ID_FW2 0x02
/** @brief SPECT bank 1. */
#define LT_L2_MUTABLE_FW_ERASE_REQ_BANK_ID_SPECT1 0x11
/** @brief SPECT bank 2 */
#define LT_L2_MUTABLE_FW_ERASE_REQ_BANK_ID_SPECT2 0x12


/** @brief Response length */
#define LT_L2_MUTABLE_FW_ERASE_RSP_LEN 0u

/**
 * @brief
 * Request to erase the mutable FW stored in a R-Memory bank.
 * Supported only in Start-up mode (i.e. after Startup_Req with MAINTENANCE_REBOOT).
 */
struct lt_l2_mutable_fw_erase_req_t {
    u8 req_id; /**< Request ID byte */
    u8 req_len; /**< Length byte */
    u8 bank_id; /**< The Identifier of the bank to erase. The same choices as above. */
    u8 crc[2]; /**< Checksum */
} __attribute__((__packed__));

/**
 * @brief
 * Request to erase the mutable FW stored in a R-Memory bank.
 * Supported only in Start-up mode (i.e. after Startup_Req with MAINTENANCE_REBOOT).
 */
struct lt_l2_mutable_fw_erase_rsp_t {
    u8 chip_status; /**< CHIP_STATUS byte */
    u8 status; /**< L2 status byte */
    u8 rsp_len; /**< Length of incoming data */
    u8 crc[2]; /**< Checksum */
} __attribute__((__packed__));


/** @brief Request ID */
#define LT_L2_GET_LOG_REQ_ID 0xa2
/** @brief Request length */
#define LT_L2_GET_LOG_REQ_LEN 0u


/** @brief Response length */
#define LT_L2_GET_LOG_RSP_LEN_MIN 0u

/**
 * @brief
 * Get log from FW running on RISCV CPU.
 */
struct lt_l2_get_log_req_t {
    u8 req_id; /**< Request ID byte */
    u8 req_len; /**< Length byte */
    u8 crc[2]; /**< Checksum */
} __attribute__((__packed__));

/**
 * @brief
 * Get log from FW running on RISCV CPU.
 */
struct lt_l2_get_log_rsp_t {
    u8 chip_status; /**< CHIP_STATUS byte */
    u8 status; /**< L2 status byte */
    u8 rsp_len; /**< Length of incoming data */
    /**
     * @brief
     * Log message of RISCV FW.
     */
    u8 log_msg[255]; /**< Log message */
    u8 crc[2]; /**< Checksum */
} __attribute__((__packed__));


#endif // !LT_L2_API_STRUCTS_H
