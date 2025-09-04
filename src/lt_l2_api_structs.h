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
#define TR01_L2_GET_INFO_REQ_ID 0x01
/** @brief Request length */
#define TR01_L2_GET_INFO_REQ_LEN 2u
//------- Certificate -------------------------------------------------------------------------------------//
/** @brief The X.509 chip certificate read from I-Memory and signed by Tropic Square (max length of 512B). */
#define TR01_L2_GET_INFO_REQ_OBJECT_ID_X509_CERTIFICATE 0x00
/** @brief Request for data bytes 0-127 of the object. */
#define TR01_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_0_127 0x00
/** @brief Request for data bytes 128-255 of the object (only needed for the X.509 certificate). */
#define TR01_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_128_255 0x01
/** @brief Request for data bytes 128-383 of object (only needed for the X.509 certificate). */
#define TR01_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_256_383 0x02
/** @brief Request for data bytes 384-511 of object (only needed for the X.509 certificate). */
#define TR01_L2_GET_INFO_REQ_BLOCK_INDEX_DATA_CHUNK_384_511 0x03
/** @brief The chip ID - the chip silicon revision and unique device ID (max length of 128B). */
#define TR01_L2_GET_INFO_REQ_OBJECT_ID_CHIP_ID 0x01
/** @brief The RISCV current running FW version (4 Bytes) */
#define TR01_L2_GET_INFO_REQ_OBJECT_ID_RISCV_FW_VERSION 0x02
/** @brief The SPECT FW version (4 Bytes) */
#define TR01_L2_GET_INFO_REQ_OBJECT_ID_SPECT_FW_VERSION 0x04
//------- FW Bank -----------------------------------------------------------------------------------------//
/** @brief The FW header read from the selected bank id (shown as an index). Supported only in Start-up mode. */
#define TR01_L2_GET_INFO_REQ_OBJECT_ID_FW_BANK 0xb0

/** @brief Response length */
#define TR01_L2_GET_INFO_RSP_LEN_MIN 1u

/**
 * @brief
 * Request to obtain information about TROPIC01. The type of information obtained is distinguished by OBJECT_ID.
 *
 * NOTE: If Start-up mode is active, TROPIC01 executes the immutable FW. Any version identification then has the highest
 * bit set to 1. SPECT_FW_VERSION then returns a dummy value of 0x80000000 because the SPECT FW is part of the immutable
 * FW.
 */
struct lt_l2_get_info_req_t {
    uint8_t req_id;  /**< Request ID byte */
    uint8_t req_len; /**< Length byte */
    /**
     * @brief
     * The Identifier of the requested object.
     */
    uint8_t object_id;
    /**
     * @brief
     * The index of the 128 Byte long block to request
     */
    uint8_t block_index;
    uint8_t crc[2]; /**< Checksum */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l2_get_info_req_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l2_get_info_req_t, req_id) +
        LT_MEMBER_SIZE(struct lt_l2_get_info_req_t, req_len) +
        LT_MEMBER_SIZE(struct lt_l2_get_info_req_t, object_id) +
        LT_MEMBER_SIZE(struct lt_l2_get_info_req_t, block_index) +
        LT_MEMBER_SIZE(struct lt_l2_get_info_req_t, crc)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Request to obtain information about TROPIC01. The type of information obtained is distinguished by OBJECT_ID.
 *
 * NOTE: If Start-up mode is active, TROPIC01 executes the immutable FW. Any version identification then has the highest
 * bit set to 1. SPECT_FW_VERSION then returns a dummy value of 0x80000000 because the SPECT FW is part of the immutable
 * FW.
 */
struct lt_l2_get_info_rsp_t {
    uint8_t chip_status; /**< CHIP_STATUS byte */
    uint8_t status;      /**< L2 status byte */
    uint8_t rsp_len;     /**< Length of incoming data */
    /**
     * @brief
     * The data content of the requested object block.
     */
    uint8_t object[128];
    uint8_t crc[2]; /**< Checksum */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l2_get_info_rsp_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l2_get_info_rsp_t, chip_status) +
        LT_MEMBER_SIZE(struct lt_l2_get_info_rsp_t, rsp_len) +
        LT_MEMBER_SIZE(struct lt_l2_get_info_rsp_t, rsp_len) +
        LT_MEMBER_SIZE(struct lt_l2_get_info_rsp_t, object) +
        LT_MEMBER_SIZE(struct lt_l2_get_info_rsp_t, crc)
    )
)
/** \endcond */
// clang-format on

/** @brief Handshake request ID. */
#define TR01_L2_HANDSHAKE_REQ_ID 0x02
/** @brief Handshake request length. */
#define TR01_L2_HANDSHAKE_REQ_LEN 33u
/** @brief Handshake response length. */
#define TR01_L2_HANDSHAKE_RSP_LEN 48u

/**
 * @brief
 * Request to execute a Secure Channel Handshake and establish a new Secure Channel Session (TROPIC01 moves to Secure
 * Channel Mode).
 */
struct lt_l2_handshake_req_t {
    uint8_t req_id;  /**< Request ID byte */
    uint8_t req_len; /**< Length byte */
    /**
     * @brief
     * The Host MCU's Ephemeral X25519 public key. A little endian encoding of the x-coordinate from the public
     * Curve25519 point.
     */
    uint8_t e_hpub[32]; /**< Ephemeral Key of Host MCU. */
    /**
     * @brief
     * The index of the Pairing Key slot to establish a Secure Channel Session with (TROPIC01 fetches $S_{HiPub}$ from
     * the Pairing Key slot specified in this field).
     */
    uint8_t pkey_index; /**< Pairing Key slot */
    uint8_t crc[2];     /**< Checksum */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l2_handshake_req_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l2_handshake_req_t, req_id) +
        LT_MEMBER_SIZE(struct lt_l2_handshake_req_t, req_len) +
        LT_MEMBER_SIZE(struct lt_l2_handshake_req_t, e_hpub) +
        LT_MEMBER_SIZE(struct lt_l2_handshake_req_t, pkey_index) +
        LT_MEMBER_SIZE(struct lt_l2_handshake_req_t, crc)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Request to execute a Secure Channel Handshake and establish a new Secure Channel Session (TROPIC01 moves to Secure
 * Channel Mode).
 */
struct lt_l2_handshake_rsp_t {
    uint8_t chip_status; /**< CHIP_STATUS byte */
    uint8_t status;      /**< L2 status byte */
    uint8_t rsp_len;     /**< Length of incoming data */
    /**
     * @brief
     * TROPIC01's X25519 Ephemeral key.
     */
    uint8_t e_tpub[32]; /**< Ephemeral Key of TROPIC01. */
    /**
     * @brief
     * The Secure Channel Handshake Authentication Tag.
     */
    uint8_t t_tauth[16]; /**< Authentication Tag */
    uint8_t crc[2];      /**< Checksum */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l2_handshake_req_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l2_handshake_req_t, req_id) +
        LT_MEMBER_SIZE(struct lt_l2_handshake_req_t, req_len) +
        LT_MEMBER_SIZE(struct lt_l2_handshake_req_t, e_hpub) +
        LT_MEMBER_SIZE(struct lt_l2_handshake_req_t, pkey_index) +
        LT_MEMBER_SIZE(struct lt_l2_handshake_req_t, crc)
    )
)
/** \endcond */
// clang-format on

/** @brief Request ID */
#define TR01_L2_ENCRYPTED_CMD_REQ_ID 0x04
/** @brief Request length */
#define TR01_L2_ENCRYPTED_CMD_REQ_LEN_MIN 19u
/** Minimal length of field cmd_ciphertext */
#define TR01_L2_ENCRYPTED_CMD_REQ_CMD_CIPHERTEXT_LEN_MIN 1u
/** Maximal length of field cmd_ciphertext */
#define TR01_L2_ENCRYPTED_CMD_REQ_CMD_CIPHERTEXT_LEN_MAX 4096u

/** @brief Response length */
#define TR01_L2_ENCRYPTED_CMD_RSP_LEN_MIN 19u

/**
 * @brief
 * Request to execute an L3 Command.
 */
struct lt_l2_encrypted_cmd_req_t {
    uint8_t req_id;  /**< Request ID byte */
    uint8_t req_len; /**< Length byte */
    /** Contains a chunk of encrypted command */
    uint8_t l3_chunk[255];
    uint8_t crc[2]; /**< Checksum */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l2_encrypted_cmd_req_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l2_encrypted_cmd_req_t, req_id) +
        LT_MEMBER_SIZE(struct lt_l2_encrypted_cmd_req_t, req_len) +
        LT_MEMBER_SIZE(struct lt_l2_encrypted_cmd_req_t, l3_chunk) +
        LT_MEMBER_SIZE(struct lt_l2_encrypted_cmd_req_t, crc)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Request to execute an L3 Command.
 */
struct lt_l2_encrypted_cmd_rsp_t {
    uint8_t chip_status; /**< CHIP_STATUS byte */
    uint8_t status;      /**< L2 status byte */
    uint8_t rsp_len;     /**< Length of incoming data */
    /**
     * @brief
     * The size of the RES_CIPHERTEXT L3 Field in bytes.
     */
    /** Contains a chunk of encrypted command */
    uint8_t l3_chunk[255];
    uint8_t crc[2]; /**< Checksum */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l2_encrypted_cmd_rsp_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l2_encrypted_cmd_rsp_t, chip_status) +
        LT_MEMBER_SIZE(struct lt_l2_encrypted_cmd_rsp_t, status) +
        LT_MEMBER_SIZE(struct lt_l2_encrypted_cmd_rsp_t, rsp_len) +
        LT_MEMBER_SIZE(struct lt_l2_encrypted_cmd_rsp_t, l3_chunk) +
        LT_MEMBER_SIZE(struct lt_l2_encrypted_cmd_rsp_t, crc)
    )
)
/** \endcond */
// clang-format on

/** @brief Request ID */
#define TR01_L2_ENCRYPTED_SESSION_ABT_ID 0x08
/** @brief Request length */
#define TR01_L2_ENCRYPTED_SESSION_ABT_LEN 0u

/** @brief Response length */
#define TR01_L2_ENCRYPTED_SESSION_ABT_RSP_LEN 0u

/**
 * @brief
 * Request to abort current Secure Channel Session and execution of L3 command (TROPIC01 moves to Idle Mode).
 */
struct lt_l2_encrypted_session_abt_req_t {
    uint8_t req_id;  /**< Request ID byte */
    uint8_t req_len; /**< Length byte */
    uint8_t crc[2];  /**< Checksum */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l2_encrypted_session_abt_req_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l2_encrypted_session_abt_req_t, req_id) +
        LT_MEMBER_SIZE(struct lt_l2_encrypted_session_abt_req_t, req_len) +
        LT_MEMBER_SIZE(struct lt_l2_encrypted_session_abt_req_t, crc)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Request to abort current Secure Channel Session and execution of L3 command (TROPIC01 moves to Idle Mode).
 */
struct lt_l2_encrypted_session_abt_rsp_t {
    uint8_t chip_status; /**< CHIP_STATUS byte */
    uint8_t status;      /**< L2 status byte */
    uint8_t rsp_len;     /**< Length of incoming data */
    uint8_t crc[2];      /**< Checksum */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l2_encrypted_session_abt_rsp_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l2_encrypted_session_abt_rsp_t, chip_status) +
        LT_MEMBER_SIZE(struct lt_l2_encrypted_session_abt_rsp_t, status) +
        LT_MEMBER_SIZE(struct lt_l2_encrypted_session_abt_rsp_t, rsp_len) +
        LT_MEMBER_SIZE(struct lt_l2_encrypted_session_abt_rsp_t, crc)
    )
)
/** \endcond */
// clang-format on

/** @brief Request ID */
#define TR01_L2_RESEND_REQ_ID 0x10
/** @brief Request length */
#define TR01_L2_RESEND_REQ_LEN 0u

/** @brief Response length */
#define TR01_L2_RESEND_RSP_LEN 0u

/**
 * @brief
 * Request for TROPIC01 to resend the last L2 Response.
 */
struct lt_l2_resend_req_t {
    uint8_t req_id;  /**< Request ID byte */
    uint8_t req_len; /**< Length byte */
    uint8_t crc[2];  /**< Checksum */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l2_resend_req_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l2_resend_req_t, req_id) +
        LT_MEMBER_SIZE(struct lt_l2_resend_req_t, req_len) +
        LT_MEMBER_SIZE(struct lt_l2_resend_req_t, crc)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Request for TROPIC01 to resend the last L2 Response.
 */
struct lt_l2_resend_rsp_t {
    uint8_t chip_status; /**< CHIP_STATUS byte */
    uint8_t status;      /**< L2 status byte */
    uint8_t rsp_len;     /**< Length of incoming data */
    uint8_t crc[2];      /**< Checksum */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l2_resend_rsp_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l2_resend_rsp_t, chip_status) +
        LT_MEMBER_SIZE(struct lt_l2_resend_rsp_t, status) +
        LT_MEMBER_SIZE(struct lt_l2_resend_rsp_t, rsp_len) +
        LT_MEMBER_SIZE(struct lt_l2_resend_rsp_t, crc)
    )
)
/** \endcond */
// clang-format on

/** @brief Request ID */
#define TR01_L2_SLEEP_REQ_ID 0x20
/** @brief Request length */
#define TR01_L2_SLEEP_REQ_LEN 1u
/** @brief Sleep Mode */
#define TR01_L2_SLEEP_REQ_SLEEP_KIND_SLEEP_MODE 0x05

/** @brief Response length */
#define TR01_L2_SLEEP_RSP_LEN 0u

/**
 * @brief
 * Request for TROPIC01 to go to Sleep Mode or Deep Sleep Mode.
 */
struct lt_l2_sleep_req_t {
    uint8_t req_id;  /**< Request ID byte */
    uint8_t req_len; /**< Length byte */
    /**
     * @brief
     * The type of Sleep mode TROPIC01 moves to.
     */
    uint8_t sleep_kind; /**< Sleep Kind */
    uint8_t crc[2];     /**< Checksum */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l2_sleep_req_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l2_sleep_req_t, req_id) +
        LT_MEMBER_SIZE(struct lt_l2_sleep_req_t, req_len) +
        LT_MEMBER_SIZE(struct lt_l2_sleep_req_t, sleep_kind) +
        LT_MEMBER_SIZE(struct lt_l2_sleep_req_t, crc)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Request for TROPIC01 to go to Sleep Mode or Deep Sleep Mode.
 */
struct lt_l2_sleep_rsp_t {
    uint8_t chip_status; /**< CHIP_STATUS byte */
    uint8_t status;      /**< L2 status byte */
    uint8_t rsp_len;     /**< Length of incoming data */
    uint8_t crc[2];      /**< Checksum */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l2_sleep_rsp_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l2_sleep_rsp_t, chip_status) +
        LT_MEMBER_SIZE(struct lt_l2_sleep_rsp_t, status) +
        LT_MEMBER_SIZE(struct lt_l2_sleep_rsp_t, rsp_len) +
        LT_MEMBER_SIZE(struct lt_l2_sleep_rsp_t, crc)
    )
)
/** \endcond */
// clang-format on

/** @brief Request ID */
#define TR01_L2_STARTUP_REQ_ID 0xb3
/** @brief Request length */
#define TR01_L2_STARTUP_REQ_LEN 1u
/** @brief Restart, then initialize as if a power-cycle was applied. */
#define TR01_L2_STARTUP_REQ_STARTUP_ID_REBOOT 0x01
/** @brief Restart, then initialize. Stay in Start-up mode and do not load the mutable FW from R-Memory. */
#define TR01_L2_STARTUP_REQ_STARTUP_ID_MAINTENANCE_REBOOT 0x03

/** @brief Response length */
#define TR01_L2_STARTUP_RSP_LEN 0u

/**
 * @brief
 * Request for TROPIC01 to reset.
 */
struct lt_l2_startup_req_t {
    uint8_t req_id;     /**< Request ID byte */
    uint8_t req_len;    /**< Length byte */
    uint8_t startup_id; /**< The request ID */
    uint8_t crc[2];     /**< Checksum */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l2_startup_req_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l2_startup_req_t, req_id) +
        LT_MEMBER_SIZE(struct lt_l2_startup_req_t, req_len) +
        LT_MEMBER_SIZE(struct lt_l2_startup_req_t, startup_id) +
        LT_MEMBER_SIZE(struct lt_l2_startup_req_t, crc)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Request for TROPIC01 to reset.
 */
struct lt_l2_startup_rsp_t {
    uint8_t chip_status; /**< CHIP_STATUS byte */
    uint8_t status;      /**< L2 status byte */
    uint8_t rsp_len;     /**< Length of incoming data */
    uint8_t crc[2];      /**< Checksum */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l2_startup_rsp_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l2_startup_rsp_t, chip_status) +
        LT_MEMBER_SIZE(struct lt_l2_startup_rsp_t, status) +
        LT_MEMBER_SIZE(struct lt_l2_startup_rsp_t, rsp_len) +
        LT_MEMBER_SIZE(struct lt_l2_startup_rsp_t, crc)
    )
)
/** \endcond */
// clang-format on

// Firmware update API structs for ABAB silicon revision
#ifdef ABAB
/** @brief Request ID */
#define TR01_L2_MUTABLE_FW_UPDATE_REQ_ID 0xb1
/** @brief Request min length */
#define TR01_L2_MUTABLE_FW_UPDATE_REQ_LEN_MIN 3u
/** Minimal length of field data */
#define TR01_L2_MUTABLE_FW_UPDATE_REQ_DATA_LEN_MIN 4u
/** Maximal length of field data */
#define TR01_L2_MUTABLE_FW_UPDATE_REQ_DATA_LEN_MAX 248u
/** @brief Firmware bank 1. */
#define TR01_L2_MUTABLE_FW_UPDATE_REQ_BANK_ID_FW1 0x01
/** @brief Firmware bank 2 */
#define TR01_L2_MUTABLE_FW_UPDATE_REQ_BANK_ID_FW2 0x02
/** @brief SPECT bank 1. */
#define TR01_L2_MUTABLE_FW_UPDATE_REQ_BANK_ID_SPECT1 0x11
/** @brief SPECT bank 2 */
#define TR01_L2_MUTABLE_FW_UPDATE_REQ_BANK_ID_SPECT2 0x12

/** @brief Response length */
#define TR01_L2_MUTABLE_FW_UPDATE_RSP_LEN 0u

/**
 * @brief
 * Request to write a chunk of the new mutable FW to a R-Memory bank.
 * Supported only in Start-up mode (i.e. after Startup_Req with MAINTENANCE_REBOOT).
 *
 * NOTE: Write only to the correctly erased bank (see Mutable_FW_Erase_Req).
 */
struct lt_l2_mutable_fw_update_req_t {
    uint8_t req_id;    /**< Request ID byte */
    uint8_t req_len;   /**< Length byte */
    uint8_t bank_id;   /**< The Identifier of the bank to write in. */
    uint16_t offset;   /**< The offset of the specific bank to write the chunk */
    uint8_t data[248]; /**< The binary data to write. Data size should be a multiple of 4. */
    uint8_t crc[2];    /**< Checksum */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l2_mutable_fw_update_req_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_req_t, req_id) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_req_t, req_len) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_req_t, bank_id) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_req_t, offset) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_req_t, data) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_req_t, crc)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Request to write a chunk of the new mutable FW to a R-Memory bank.
 * Supported only in Start-up mode (i.e. after Startup_Req with MAINTENANCE_REBOOT).
 *
 * NOTE: Write only to the correctly erased bank (see Mutable_FW_Erase_Req).
 */
struct lt_l2_mutable_fw_update_rsp_t {
    uint8_t chip_status; /**< CHIP_STATUS byte */
    uint8_t status;      /**< L2 status byte */
    uint8_t rsp_len;     /**< Length of incoming data */
    uint8_t crc[2];      /**< Checksum */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l2_mutable_fw_update_rsp_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_rsp_t, chip_status) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_rsp_t, status) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_rsp_t, rsp_len) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_rsp_t, crc)
    )
)
/** \endcond */
// clang-format on
#endif

// Firmware update API structs for ACAB silicon revision
#ifdef ACAB
/** @brief Request ID */
#define TR01_L2_MUTABLE_FW_UPDATE_REQ_ID 0xb0
/** @brief Request min length */
#define TR01_L2_MUTABLE_FW_UPDATE_REQ_LEN 0x68
/** @brief Response length */
#define TR01_L2_MUTABLE_FW_UPDATE_RSP_LEN 0u
/** @brief Firmware type for RISC-V main CPU*/
#define TR01_L2_MUTABLE_FW_UPDATE_REQ_TYPE_FW_TYPE_CPU 1
/** @brief Firmware type for SPECT coprocessor */
#define TR01_L2_MUTABLE_FW_UPDATE_REQ_TYPE_FW_TYPE_SPECT 2

/**
 * @brief
 * Request to start updating mutable FW.
 * Supported only in Start-up mode (i.e. after Startup_Req with MAINTENANCE_REBOOT).
 * Possible to update only same or newer version.
 * NOTE: Chip automatically selects memory space for FW storage and erases it.
 */
struct lt_l2_mutable_fw_update_req_t {
    uint8_t req_id;         /**< Request ID byte */
    uint8_t req_len;        /**< Length byte */
    uint8_t signature[64];  /**< Signature of SHA256 hash of all following data in this packet */
    uint8_t hash[32];       /**< SHA256 HASH of first FW chunk of data sent using Mutable_FW_Update_Data */
    uint16_t type;          /**< FW type which is going to be updated */
    uint8_t padding;        /**< Padding, zero value */
    uint8_t header_version; /**< Version of used header */
    uint32_t version;       /**< Version of FW */
    uint8_t crc[2];         /**< Checksum */
} __attribute__((__packed__));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l2_mutable_fw_update_req_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_req_t, req_id) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_req_t, req_len) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_req_t, signature) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_req_t, hash) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_req_t, type) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_req_t, padding) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_req_t, header_version) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_req_t, version) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_req_t, crc)
    )
)
/** \endcond */
// clang-format on

/** @brief Request ID */
#define TR01_L2_MUTABLE_FW_UPDATE_DATA_REQ 0xb1

/**
 * @brief Response on lt_l2_mutable_fw_update_req_t
 */
struct lt_l2_mutable_fw_update_rsp_t {
    uint8_t chip_status; /**< CHIP_STATUS byte */
    uint8_t status;      /**< L2 status byte */
    uint8_t rsp_len;     /**< Length of incoming data */
    uint8_t crc[2];      /**< Checksum */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l2_mutable_fw_update_rsp_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_rsp_t, chip_status) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_rsp_t, status) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_rsp_t, rsp_len) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_rsp_t, crc)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief Request to write a chunk of the new mutable FW into memory bank
 * Supported only in Start-up mode after Mutable_FW_Update_Req successfully processed.
 */
struct lt_l2_mutable_fw_update_data_req_t {
    uint8_t req_id;    /**< Request ID byte */
    uint8_t req_len;   /**< Length byte */
    uint8_t hash[32];  /**< SHA256 HASH of the next FW chunk of data sent using Mutable_FW_Update_Data */
    uint16_t offset;   /**< The offset of the specific bank to write the FW chunk data to */
    uint8_t data[220]; /**< The binary data to write. Data size should be a multiple of 4 */
    uint8_t crc[2];    /**< Checksum */
} __attribute__((__packed__));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l2_mutable_fw_update_data_req_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_data_req_t, req_id) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_data_req_t, req_len) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_data_req_t, hash) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_data_req_t, offset) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_data_req_t, data) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_data_req_t, crc)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief response on lt_l2_mutable_fw_update_data_req_t
 */
struct lt_l2_mutable_fw_update_data_rsp_t {
    uint8_t chip_status; /**< CHIP_STATUS byte */
    uint8_t status;      /**< L2 status byte */
    uint8_t rsp_len;     /**< Length of incoming data */
    uint8_t crc[2];      /**< Checksum */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l2_mutable_fw_update_data_rsp_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_data_rsp_t, chip_status) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_data_rsp_t, status) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_data_rsp_t, rsp_len) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_update_data_rsp_t, crc)
    )
)
/** \endcond */
// clang-format on
#endif

/** @brief Request ID */
#define TR01_L2_MUTABLE_FW_ERASE_REQ_ID 0xb2
/** @brief Request length */
#define TR01_L2_MUTABLE_FW_ERASE_REQ_LEN 1u
/** @brief Firmware bank 1. */
#define TR01_L2_MUTABLE_FW_ERASE_REQ_BANK_ID_FW1 0x01
/** @brief Firmware bank 2 */
#define TR01_L2_MUTABLE_FW_ERASE_REQ_BANK_ID_FW2 0x02
/** @brief SPECT bank 1. */
#define TR01_L2_MUTABLE_FW_ERASE_REQ_BANK_ID_SPECT1 0x11
/** @brief SPECT bank 2 */
#define TR01_L2_MUTABLE_FW_ERASE_REQ_BANK_ID_SPECT2 0x12

/** @brief Response length */
#define TR01_L2_MUTABLE_FW_ERASE_RSP_LEN 0u

/**
 * @brief
 * Request to erase the mutable FW stored in a R-Memory bank.
 * Supported only in Start-up mode (i.e. after Startup_Req with MAINTENANCE_REBOOT).
 */
struct lt_l2_mutable_fw_erase_req_t {
    uint8_t req_id;  /**< Request ID byte */
    uint8_t req_len; /**< Length byte */
    uint8_t bank_id; /**< The Identifier of the bank to erase. The same choices as above. */
    uint8_t crc[2];  /**< Checksum */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l2_mutable_fw_erase_req_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_erase_req_t, req_id) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_erase_req_t, req_len) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_erase_req_t, bank_id) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_erase_req_t, crc)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Request to erase the mutable FW stored in a R-Memory bank.
 * Supported only in Start-up mode (i.e. after Startup_Req with MAINTENANCE_REBOOT).
 */
struct lt_l2_mutable_fw_erase_rsp_t {
    uint8_t chip_status; /**< CHIP_STATUS byte */
    uint8_t status;      /**< L2 status byte */
    uint8_t rsp_len;     /**< Length of incoming data */
    uint8_t crc[2];      /**< Checksum */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l2_mutable_fw_erase_rsp_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_erase_rsp_t, chip_status) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_erase_rsp_t, status) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_erase_rsp_t, rsp_len) +
        LT_MEMBER_SIZE(struct lt_l2_mutable_fw_erase_rsp_t, crc)
    )
)
/** \endcond */
// clang-format on

/** @brief Request ID */
#define TR01_L2_GET_LOG_REQ_ID 0xa2
/** @brief Request length */
#define TR01_L2_GET_LOG_REQ_LEN 0u

/** @brief Response length */
#define TR01_L2_GET_LOG_RSP_LEN_MIN 0u

/**
 * @brief
 * Get log from FW running on RISCV CPU.
 */
struct lt_l2_get_log_req_t {
    uint8_t req_id;  /**< Request ID byte */
    uint8_t req_len; /**< Length byte */
    uint8_t crc[2];  /**< Checksum */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l2_get_log_req_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l2_get_log_req_t, req_id) +
        LT_MEMBER_SIZE(struct lt_l2_get_log_req_t, req_len) +
        LT_MEMBER_SIZE(struct lt_l2_get_log_req_t, crc)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Get log from FW running on RISCV CPU.
 */
struct lt_l2_get_log_rsp_t {
    uint8_t chip_status;                       /**< CHIP_STATUS byte */
    uint8_t status;                            /**< L2 status byte */
    uint8_t rsp_len;                           /**< Length of incoming data */
    uint8_t log_msg[TR01_GET_LOG_MAX_MSG_LEN]; /**< Log message of RISC-V FW */
    uint8_t crc[2];                            /**< Checksum */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l2_get_log_rsp_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l2_get_log_rsp_t, chip_status) +
        LT_MEMBER_SIZE(struct lt_l2_get_log_rsp_t, status) +
        LT_MEMBER_SIZE(struct lt_l2_get_log_rsp_t, rsp_len) +
        LT_MEMBER_SIZE(struct lt_l2_get_log_rsp_t, log_msg) +
        LT_MEMBER_SIZE(struct lt_l2_get_log_rsp_t, crc)
    )
)
/** \endcond */
// clang-format on

#endif  // !LT_L2_API_STRUCTS_H
