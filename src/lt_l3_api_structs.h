// GENERATED ON 2024-10-10 16:07:37.462449
// BY internal VERSION 1.6
// INPUT FILE: 00C671449AFC726579F38A3055D3702F0AE393D7B036D6A525CD097863562904
//
// Copyright 2024 TropicSquare
// SPDX-License-Identifier: Apache-2.0

#ifndef LT_L3_API_STRUCTS_H
#define LT_L3_API_STRUCTS_H

/**
 * @file lt_l3_api_structs.h
 * @brief Layer 3 structure declaration
 * @author Tropic Square s.r.o.
 */

#include "libtropic_common.h"

/** @brief Command ID */
#define TR01_L3_PING_CMD_ID 0x01
/** @brief Command length */
#define TR01_L3_PING_CMD_SIZE_MIN 1u
/** Minimal length of field data_in */
#define TR01_L3_PING_CMD_DATA_IN_LEN_MIN 0u
/** Maximal length of field data_in */
#define TR01_L3_PING_CMD_DATA_IN_LEN_MAX 4096u

/** @brief Result length */
#define TR01_L3_PING_RES_SIZE_MIN 1u

/**
 * @brief
 * A dummy command to check the Secure Channel Session communication.
 */
struct lt_l3_ping_cmd_t {
    uint16_t cmd_size; /**< L3 command size */
    uint8_t cmd_id;    /**< Command Identifier */
    /**
     * @brief
     * The input data
     */
    uint8_t data_in[4096]; /**< Data in */
    uint8_t tag[16];       /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_ping_cmd_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_ping_cmd_t, cmd_size) +
        LT_MEMBER_SIZE(struct lt_l3_ping_cmd_t, cmd_id) +
        LT_MEMBER_SIZE(struct lt_l3_ping_cmd_t, data_in) +
        LT_MEMBER_SIZE(struct lt_l3_ping_cmd_t, tag)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * A dummy command to check the Secure Channel Session communication.
 */
struct lt_l3_ping_res_t {
    uint16_t res_size; /**< L3 result size */
    uint8_t result;    /**< Result status indication */
    /**
     * @brief
     * The output data (loopback of the DATA_IN L3 Field).
     */
    uint8_t data_out[4096]; /**< Data out */
    uint8_t tag[16];        /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_ping_res_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_ping_res_t, res_size) +
        LT_MEMBER_SIZE(struct lt_l3_ping_res_t, result) +
        LT_MEMBER_SIZE(struct lt_l3_ping_res_t, data_out) +
        LT_MEMBER_SIZE(struct lt_l3_ping_res_t, tag)
    )
)
/** \endcond */
// clang-format on

/** @brief Command ID */
#define TR01_L3_PAIRING_KEY_WRITE_CMD_ID 0x10
/** @brief Command length */
#define TR01_L3_PAIRING_KEY_WRITE_CMD_SIZE 36u

/** @brief Result length */
#define TR01_L3_PAIRING_KEY_WRITE_RES_SIZE 1u

/**
 * @brief
 * Command to write the X25519 public key to a Pairing Key slot.
 */
struct lt_l3_pairing_key_write_cmd_t {
    uint16_t cmd_size; /**< L3 command size */
    uint8_t cmd_id;    /**< Command Identifier */
    /**
     * @brief
     * The Pairing Key slot. Valid values are 0 - 3.
     */
    uint16_t slot; /**< Slot to write in */
    /**
     * @brief
     * The padding by dummy data.
     */
    uint8_t padding; /**< Padding */
    /**
     * @brief
     * The X25519 public key to be written in the Pairing Key slot specified in the SLOT field.
     */
    uint8_t s_hipub[32]; /**< Public Key */
    uint8_t tag[16];     /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_pairing_key_write_cmd_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_pairing_key_write_cmd_t, cmd_size) +
        LT_MEMBER_SIZE(struct lt_l3_pairing_key_write_cmd_t, cmd_id) +
        LT_MEMBER_SIZE(struct lt_l3_pairing_key_write_cmd_t, slot) +
        LT_MEMBER_SIZE(struct lt_l3_pairing_key_write_cmd_t, padding) +
        LT_MEMBER_SIZE(struct lt_l3_pairing_key_write_cmd_t, s_hipub) +
        LT_MEMBER_SIZE(struct lt_l3_pairing_key_write_cmd_t, tag)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Command to write the X25519 public key to a Pairing Key slot.
 */
struct lt_l3_pairing_key_write_res_t {
    uint16_t res_size; /**< L3 result size */
    uint8_t result;    /**< Result status indication */
    uint8_t tag[16];   /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_pairing_key_write_res_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_pairing_key_write_res_t, res_size) +
        LT_MEMBER_SIZE(struct lt_l3_pairing_key_write_res_t, result) +
        LT_MEMBER_SIZE(struct lt_l3_pairing_key_write_res_t, tag)
    )
)
/** \endcond */
// clang-format on

/** @brief Command ID */
#define TR01_L3_PAIRING_KEY_READ_CMD_ID 0x11
/** @brief Command length */
#define TR01_L3_PAIRING_KEY_READ_CMD_SIZE 3u

/** @brief Result length */
#define TR01_L3_PAIRING_KEY_READ_RES_SIZE 36u

/**
 * @brief
 * Command to read the X25519 public key from a Pairing Key slot.
 */
struct lt_l3_pairing_key_read_cmd_t {
    uint16_t cmd_size; /**< L3 command size */
    uint8_t cmd_id;    /**< Command Identifier */
    /**
     * @brief
     * The Pairing Key slot. Valid values are 0 - 3.
     */
    uint16_t slot;   /**< Slot to Read */
    uint8_t tag[16]; /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_pairing_key_read_cmd_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_pairing_key_read_cmd_t, cmd_size) +
        LT_MEMBER_SIZE(struct lt_l3_pairing_key_read_cmd_t, cmd_id) +
        LT_MEMBER_SIZE(struct lt_l3_pairing_key_read_cmd_t, slot) +
        LT_MEMBER_SIZE(struct lt_l3_pairing_key_read_cmd_t, tag)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Command to read the X25519 public key from a Pairing Key slot.
 */
struct lt_l3_pairing_key_read_res_t {
    uint16_t res_size; /**< L3 result size */
    uint8_t result;    /**< Result status indication */
    /**
     * @brief
     * The padding by dummy data.
     */
    uint8_t padding[3]; /**< Padding */
    /**
     * @brief
     * The X25519 public key to be written in the Pairing Key slot specified in the SLOT field.
     */
    uint8_t s_hipub[32]; /**< Public Key */
    uint8_t tag[16];     /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_pairing_key_read_res_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_pairing_key_read_res_t, res_size) +
        LT_MEMBER_SIZE(struct lt_l3_pairing_key_read_res_t, result) +
        LT_MEMBER_SIZE(struct lt_l3_pairing_key_read_res_t, padding) +
        LT_MEMBER_SIZE(struct lt_l3_pairing_key_read_res_t, s_hipub) +
        LT_MEMBER_SIZE(struct lt_l3_pairing_key_read_res_t, tag)
    )
)
/** \endcond */
// clang-format on

/** @brief Command ID */
#define TR01_L3_PAIRING_KEY_INVALIDATE_CMD_ID 0x12
/** @brief Command length */
#define TR01_L3_PAIRING_KEY_INVALIDATE_CMD_SIZE 3u

/** @brief Result length */
#define TR01_L3_PAIRING_KEY_INVALIDATE_RES_SIZE 1u

/**
 * @brief
 * Command to invalidate the X25519 public key in a Pairing Key slot.
 */
struct lt_l3_pairing_key_invalidate_cmd_t {
    uint16_t cmd_size; /**< L3 command size */
    uint8_t cmd_id;    /**< Command Identifier */
    /**
     * @brief
     * The Pairing Key slot. Valid values are 0 - 3.
     */
    uint16_t slot;   /**< Slot to Invalidate */
    uint8_t tag[16]; /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_pairing_key_invalidate_cmd_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_pairing_key_invalidate_cmd_t, cmd_size) +
        LT_MEMBER_SIZE(struct lt_l3_pairing_key_invalidate_cmd_t, cmd_id) +
        LT_MEMBER_SIZE(struct lt_l3_pairing_key_invalidate_cmd_t, slot) +
        LT_MEMBER_SIZE(struct lt_l3_pairing_key_invalidate_cmd_t, tag)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Command to invalidate the X25519 public key in a Pairing Key slot.
 */
struct lt_l3_pairing_key_invalidate_res_t {
    uint16_t res_size; /**< L3 result size */
    uint8_t result;    /**< Result status indication */
    uint8_t tag[16];   /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_pairing_key_invalidate_res_t) == 
    (
        LT_MEMBER_SIZE(struct lt_l3_pairing_key_invalidate_res_t, res_size) +
        LT_MEMBER_SIZE(struct lt_l3_pairing_key_invalidate_res_t, result) +
        LT_MEMBER_SIZE(struct lt_l3_pairing_key_invalidate_res_t, tag)
    )
)
/** \endcond */
// clang-format on

/** @brief Command ID */
#define TR01_L3_R_CONFIG_WRITE_CMD_ID 0x20
/** @brief Command length */
#define TR01_L3_R_CONFIG_WRITE_CMD_SIZE 8u

/** @brief Result length */
#define TR01_L3_R_CONFIG_WRITE_RES_SIZE 1u

/**
 * @brief
 * Command to write a single CO to R-Config.
 */
struct lt_l3_r_config_write_cmd_t {
    uint16_t cmd_size; /**< L3 command size */
    uint8_t cmd_id;    /**< Command Identifier */
    /**
     * @brief
     * The CO address offset for TROPIC01 to compute the actual CO address.
     */
    uint16_t address; /**< Configuration object address */
    /**
     * @brief
     * The padding by dummy data.
     */
    uint8_t padding; /**< Padding */
    /**
     * @brief
     * The CO value to write in the computed address.
     */
    uint32_t value;  /**< Configuration object value */
    uint8_t tag[16]; /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_r_config_write_cmd_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_r_config_write_cmd_t, cmd_size) +
        LT_MEMBER_SIZE(struct lt_l3_r_config_write_cmd_t, cmd_id) +
        LT_MEMBER_SIZE(struct lt_l3_r_config_write_cmd_t, address) +
        LT_MEMBER_SIZE(struct lt_l3_r_config_write_cmd_t, padding) +
        LT_MEMBER_SIZE(struct lt_l3_r_config_write_cmd_t, value) +
        LT_MEMBER_SIZE(struct lt_l3_r_config_write_cmd_t, tag)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Command to write a single CO to R-Config.
 */
struct lt_l3_r_config_write_res_t {
    uint16_t res_size; /**< L3 result size */
    uint8_t result;    /**< Result status indication */
    uint8_t tag[16];   /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_r_config_write_res_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_r_config_write_res_t, res_size) +
        LT_MEMBER_SIZE(struct lt_l3_r_config_write_res_t, result) +
        LT_MEMBER_SIZE(struct lt_l3_r_config_write_res_t, tag)
    )
)
/** \endcond */
// clang-format on

/** @brief Command ID */
#define TR01_L3_R_CONFIG_READ_CMD_ID 0x21
/** @brief Command length */
#define TR01_L3_R_CONFIG_READ_CMD_SIZE 3u

/** @brief Result length */
#define TR01_L3_R_CONFIG_READ_RES_SIZE 8u

/**
 * @brief
 * Command to read a single CO from R-Config.
 */
struct lt_l3_r_config_read_cmd_t {
    uint16_t cmd_size; /**< L3 command size */
    uint8_t cmd_id;    /**< Command Identifier */
    /**
     * @brief
     * The CO address offset for TROPIC01 to compute the actual CO address.
     */
    uint16_t address; /**< Configuration object address */
    uint8_t tag[16];  /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_r_config_read_cmd_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_r_config_read_cmd_t, cmd_size) +
        LT_MEMBER_SIZE(struct lt_l3_r_config_read_cmd_t, cmd_id) +
        LT_MEMBER_SIZE(struct lt_l3_r_config_read_cmd_t, address) +
        LT_MEMBER_SIZE(struct lt_l3_r_config_read_cmd_t, tag)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Command to read a single CO from R-Config.
 */
struct lt_l3_r_config_read_res_t {
    uint16_t res_size; /**< L3 result size */
    uint8_t result;    /**< Result status indication */
    /**
     * @brief
     * The padding by dummy data.
     */
    uint8_t padding[3]; /**< Padding */
    /**
     * @brief
     * The CO value TROPIC01 read from the computed address.
     */
    uint32_t value;  /**< Configuration object value */
    uint8_t tag[16]; /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_r_config_read_res_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_r_config_read_res_t, res_size) +
        LT_MEMBER_SIZE(struct lt_l3_r_config_read_res_t, result) +
        LT_MEMBER_SIZE(struct lt_l3_r_config_read_res_t, padding) +
        LT_MEMBER_SIZE(struct lt_l3_r_config_read_res_t, value) +
        LT_MEMBER_SIZE(struct lt_l3_r_config_read_res_t, tag)
    )
)
/** \endcond */
// clang-format on

/** @brief Command ID */
#define TR01_L3_R_CONFIG_ERASE_CMD_ID 0x22
/** @brief Command length */
#define TR01_L3_R_CONFIG_ERASE_CMD_SIZE 1u

/** @brief Result length */
#define TR01_L3_R_CONFIG_ERASE_RES_SIZE 1u

/**
 * @brief
 * Command to erase the whole R-Config (convert the bits of all CO to 1).
 */
struct lt_l3_r_config_erase_cmd_t {
    uint16_t cmd_size; /**< L3 command size */
    uint8_t cmd_id;    /**< Command Identifier */
    uint8_t tag[16];   /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_r_config_erase_cmd_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_r_config_erase_cmd_t, cmd_size) +
        LT_MEMBER_SIZE(struct lt_l3_r_config_erase_cmd_t, cmd_id) +
        LT_MEMBER_SIZE(struct lt_l3_r_config_erase_cmd_t, tag)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Command to erase the whole R-Config (convert the bits of all CO to 1).
 */
struct lt_l3_r_config_erase_res_t {
    uint16_t res_size; /**< L3 result size */
    uint8_t result;    /**< Result status indication */
    uint8_t tag[16];   /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_r_config_erase_res_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_r_config_erase_res_t, res_size) +
        LT_MEMBER_SIZE(struct lt_l3_r_config_erase_res_t, result) +
        LT_MEMBER_SIZE(struct lt_l3_r_config_erase_res_t, tag)
    )
)
/** \endcond */
// clang-format on

/** @brief Command ID */
#define TR01_L3_I_CONFIG_WRITE_CMD_ID 0x30
/** @brief Command length */
#define TR01_L3_I_CONFIG_WRITE_CMD_SIZE 4u

/** @brief Result length */
#define TR01_L3_I_CONFIG_WRITE_RES_SIZE 1u

/**
 * @brief
 * Command to write a single bit of CO (from I-Config) from 1 to 0.
 */
struct lt_l3_i_config_write_cmd_t {
    uint16_t cmd_size; /**< L3 command size */
    uint8_t cmd_id;    /**< Command Identifier */
    /**
     * @brief
     * The CO address offset for TROPIC01 to compute the actual CO address.
     */
    uint16_t address; /**< Configuration object address */
    /**
     * @brief
     * The bit to write from 1 to 0. Valid values are 0-31.
     */
    uint8_t bit_index; /**< Bit to write. */
    uint8_t tag[16];   /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_i_config_write_cmd_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_i_config_write_cmd_t, cmd_size) +
        LT_MEMBER_SIZE(struct lt_l3_i_config_write_cmd_t, cmd_id) +
        LT_MEMBER_SIZE(struct lt_l3_i_config_write_cmd_t, address) + 
        LT_MEMBER_SIZE(struct lt_l3_i_config_write_cmd_t, bit_index) + 
        LT_MEMBER_SIZE(struct lt_l3_i_config_write_cmd_t, tag)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Command to write a single bit of CO (from I-Config) from 1 to 0.
 */
struct lt_l3_i_config_write_res_t {
    uint16_t res_size; /**< L3 result size */
    uint8_t result;    /**< Result status indication */
    uint8_t tag[16];   /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_i_config_write_res_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_i_config_write_res_t, res_size) +
        LT_MEMBER_SIZE(struct lt_l3_i_config_write_res_t, result) +
        LT_MEMBER_SIZE(struct lt_l3_i_config_write_res_t, tag)
    )
)
/** \endcond */
// clang-format on

/** @brief Command ID */
#define TR01_L3_I_CONFIG_READ_CMD_ID 0x31
/** @brief Command length */
#define TR01_L3_I_CONFIG_READ_CMD_SIZE 3u

/** @brief Result length */
#define TR01_L3_I_CONFIG_READ_RES_SIZE 8u

/**
 * @brief
 * Command to read a single CO from I-Config.
 */
struct lt_l3_i_config_read_cmd_t {
    uint16_t cmd_size; /**< L3 command size */
    uint8_t cmd_id;    /**< Command Identifier */
    /**
     * @brief
     * The CO address offset for TROPIC01 to compute the actual CO address.
     */
    uint16_t address; /**< Configuration object address */
    uint8_t tag[16];  /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_i_config_read_cmd_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_i_config_read_cmd_t, cmd_size) +
        LT_MEMBER_SIZE(struct lt_l3_i_config_read_cmd_t, cmd_id) +
        LT_MEMBER_SIZE(struct lt_l3_i_config_read_cmd_t, address) +
        LT_MEMBER_SIZE(struct lt_l3_i_config_read_cmd_t, tag)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Command to read a single CO from I-Config.
 */
struct lt_l3_i_config_read_res_t {
    uint16_t res_size; /**< L3 result size */
    uint8_t result;    /**< Result status indication */
    /**
     * @brief
     * The padding by dummy data.
     */
    uint8_t padding[3]; /**< Padding */
    /**
     * @brief
     * The CO value TROPIC01 read from the computed address.
     */
    uint32_t value;  /**< Configuration object value */
    uint8_t tag[16]; /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_i_config_read_res_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_i_config_read_res_t, res_size) +
        LT_MEMBER_SIZE(struct lt_l3_i_config_read_res_t, result) +
        LT_MEMBER_SIZE(struct lt_l3_i_config_read_res_t, padding) +
        LT_MEMBER_SIZE(struct lt_l3_i_config_read_res_t, value) +
        LT_MEMBER_SIZE(struct lt_l3_i_config_read_res_t, tag)
    )
)
/** \endcond */
// clang-format on

/** @brief Command ID */
#define TR01_L3_R_MEM_DATA_WRITE_CMD_ID 0x40
/** @brief Command length */
#define TR01_L3_R_MEM_DATA_WRITE_CMD_SIZE_MIN 5u
/** Minimal length of field data */
#define TR01_L3_R_MEM_DATA_WRITE_CMD_DATA_LEN_MIN 1u
/** Maximal length of field data */
#define TR01_L3_R_MEM_DATA_WRITE_CMD_DATA_LEN_MAX 444u

/** @brief Result length */
#define TR01_L3_R_MEM_DATA_WRITE_RES_SIZE 1u

/**
 * @brief
 * Command to write general purpose data in a slot from the User Data partition in R-Memory.
 */
struct lt_l3_r_mem_data_write_cmd_t {
    uint16_t cmd_size; /**< L3 command size */
    uint8_t cmd_id;    /**< Command Identifier */
    /**
     * @brief
     * The slot of the User Data partition. Valid values are 0 - 511.
     */
    uint16_t udata_slot; /**< Slot to write */
    /**
     * @brief
     * The padding by dummy data.
     */
    uint8_t padding; /**< Padding */
    /**
     * @brief
     * The data stream to be written in the slot specified in the UDATA_SLOT L3 field.
     */
    uint8_t data[444]; /**< Data to write */
    uint8_t tag[16];   /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_r_mem_data_write_cmd_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_r_mem_data_write_cmd_t, cmd_size) +
        LT_MEMBER_SIZE(struct lt_l3_r_mem_data_write_cmd_t, cmd_id) +
        LT_MEMBER_SIZE(struct lt_l3_r_mem_data_write_cmd_t, udata_slot) +
        LT_MEMBER_SIZE(struct lt_l3_r_mem_data_write_cmd_t, padding) +
        LT_MEMBER_SIZE(struct lt_l3_r_mem_data_write_cmd_t, data) +
        LT_MEMBER_SIZE(struct lt_l3_r_mem_data_write_cmd_t, tag)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Command to write general purpose data in a slot from the User Data partition in R-Memory.
 */
struct lt_l3_r_mem_data_write_res_t {
    uint16_t res_size; /**< L3 result size */
    uint8_t result;    /**< Result status indication */
    uint8_t tag[16];   /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_r_mem_data_write_res_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_r_mem_data_write_res_t, res_size) +
        LT_MEMBER_SIZE(struct lt_l3_r_mem_data_write_res_t, result) +
        LT_MEMBER_SIZE(struct lt_l3_r_mem_data_write_res_t, tag)
    )
)
/** \endcond */
// clang-format on

/** @brief Command ID */
#define TR01_L3_R_MEM_DATA_READ_CMD_ID 0x41
/** @brief Command length */
#define TR01_L3_R_MEM_DATA_READ_CMD_SIZE 3u

/** @brief Result length */
#define TR01_L3_R_MEM_DATA_READ_RES_SIZE_MIN 4u
#define TR01_L3_R_MEM_DATA_READ_RES_SIZE_MAX 448u

/**
 * @brief
 * Command to read the general purpose data from a slot of the User Data partition in R-Memory.
 */
struct lt_l3_r_mem_data_read_cmd_t {
    uint16_t cmd_size; /**< L3 command size */
    uint8_t cmd_id;    /**< Command Identifier */
    /**
     * @brief
     * The slot of the User Data partition. Valid values are 0 - 511.
     */
    uint16_t udata_slot; /**< Slot to read */
    uint8_t tag[16];     /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_r_mem_data_read_cmd_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_r_mem_data_read_cmd_t, cmd_size) +
        LT_MEMBER_SIZE(struct lt_l3_r_mem_data_read_cmd_t, cmd_id) +
        LT_MEMBER_SIZE(struct lt_l3_r_mem_data_read_cmd_t, udata_slot) +
        LT_MEMBER_SIZE(struct lt_l3_r_mem_data_read_cmd_t, tag)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Command to read the general purpose data from a slot of the User Data partition in R-Memory.
 */
struct lt_l3_r_mem_data_read_res_t {
    uint16_t res_size; /**< L3 result size */
    uint8_t result;    /**< Result status indication */
    /**
     * @brief
     * The padding by dummy data.
     */
    uint8_t padding[3]; /**< Padding */
    /**
     * @brief
     * The data stream read from the slot specified in the UDATA_SLOT L3 field.
     */
    uint8_t data[444]; /**< Data to read */
    uint8_t tag[16];   /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_r_mem_data_read_res_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_r_mem_data_read_res_t, res_size) +
        LT_MEMBER_SIZE(struct lt_l3_r_mem_data_read_res_t, result) +
        LT_MEMBER_SIZE(struct lt_l3_r_mem_data_read_res_t, padding) +
        LT_MEMBER_SIZE(struct lt_l3_r_mem_data_read_res_t, data) +
        LT_MEMBER_SIZE(struct lt_l3_r_mem_data_read_res_t, tag)
    )
)
/** \endcond */
// clang-format on

/** @brief Command ID */
#define TR01_L3_R_MEM_DATA_ERASE_CMD_ID 0x42
/** @brief Command length */
#define TR01_L3_R_MEM_DATA_ERASE_CMD_SIZE 3u

/** @brief Result length */
#define TR01_L3_R_MEM_DATA_ERASE_RES_SIZE 1u

/**
 * @brief
 * Command to erase a slot from the User Data partition in R-Memory.
 */
struct lt_l3_r_mem_data_erase_cmd_t {
    uint16_t cmd_size; /**< L3 command size */
    uint8_t cmd_id;    /**< Command Identifier */
    /**
     * @brief
     * The slot of the User Data partition. Valid values are 0 - 511.
     */
    uint16_t udata_slot; /**< Slot to erase */
    uint8_t tag[16];     /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_r_mem_data_erase_cmd_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_r_mem_data_erase_cmd_t, cmd_size) +
        LT_MEMBER_SIZE(struct lt_l3_r_mem_data_erase_cmd_t, cmd_id) +
        LT_MEMBER_SIZE(struct lt_l3_r_mem_data_erase_cmd_t, udata_slot) +
        LT_MEMBER_SIZE(struct lt_l3_r_mem_data_erase_cmd_t, tag)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Command to erase a slot from the User Data partition in R-Memory.
 */
struct lt_l3_r_mem_data_erase_res_t {
    uint16_t res_size; /**< L3 result size */
    uint8_t result;    /**< Result status indication */
    uint8_t tag[16];   /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_r_mem_data_erase_res_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_r_mem_data_erase_res_t, res_size) +
        LT_MEMBER_SIZE(struct lt_l3_r_mem_data_erase_res_t, result) +
        LT_MEMBER_SIZE(struct lt_l3_r_mem_data_erase_res_t, tag)
    )
)
/** \endcond */
// clang-format on

/** @brief Command ID */
#define TR01_L3_RANDOM_VALUE_GET_CMD_ID 0x50
/** @brief Command length */
#define TR01_L3_RANDOM_VALUE_GET_CMD_SIZE 2u

/** @brief Result length */
#define TR01_L3_RANDOM_VALUE_GET_RES_SIZE_MIN 4u

/**
 * @brief
 * Command to get random numbers generated by TRNG2.
 */
struct lt_l3_random_value_get_cmd_t {
    uint16_t cmd_size; /**< L3 command size */
    uint8_t cmd_id;    /**< Command Identifier */
    /**
     * @brief
     * The number of random bytes to get.
     */
    uint8_t n_bytes; /**< Number of bytes to get. */
    uint8_t tag[16]; /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_random_value_get_cmd_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_random_value_get_cmd_t, cmd_size) +
        LT_MEMBER_SIZE(struct lt_l3_random_value_get_cmd_t, cmd_id) +
        LT_MEMBER_SIZE(struct lt_l3_random_value_get_cmd_t, n_bytes) +
        LT_MEMBER_SIZE(struct lt_l3_random_value_get_cmd_t, tag)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Command to get random numbers generated by TRNG2.
 */
struct lt_l3_random_value_get_res_t {
    uint16_t res_size; /**< L3 result size */
    uint8_t result;    /**< Result status indication */
    /**
     * @brief
     * The padding by dummy data.
     */
    uint8_t padding[3]; /**< Padding */
    /**
     * @brief
     * The random data from TRNG2 in the number of bytes specified in the N_BYTES L3 Field.
     */
    uint8_t random_data[255]; /**< Random data */
    uint8_t tag[16];          /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_random_value_get_res_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_random_value_get_res_t, res_size) +
        LT_MEMBER_SIZE(struct lt_l3_random_value_get_res_t, result) +
        LT_MEMBER_SIZE(struct lt_l3_random_value_get_res_t, padding) +
        LT_MEMBER_SIZE(struct lt_l3_random_value_get_res_t, random_data) +
        LT_MEMBER_SIZE(struct lt_l3_random_value_get_res_t, tag)
    )
)
/** \endcond */
// clang-format on

/** @brief Command ID */
#define TR01_L3_ECC_KEY_GENERATE_CMD_ID 0x60
/** @brief Command length */
#define TR01_L3_ECC_KEY_GENERATE_CMD_SIZE 4u
/** @brief P256 Curve - 64-byte long public key. */
#define TR01_L3_ECC_KEY_GENERATE_CMD_CURVE_P256 0x01
/** @brief Ed25519 Curve - 32-byte long public key. */
#define TR01_L3_ECC_KEY_GENERATE_CMD_CURVE_ED25519 0x02

/** @brief Result length */
#define TR01_L3_ECC_KEY_GENERATE_RES_SIZE 1u

/**
 * @brief
 * Command to generate an ECC Key and store the key in a slot from the ECC Keys partition in R-Memory.
 */
struct lt_l3_ecc_key_generate_cmd_t {
    uint16_t cmd_size; /**< L3 command size */
    uint8_t cmd_id;    /**< Command Identifier */
    /**
     * @brief
     * The slot to write the generated key. Valid values are 0 - 31.
     */
    uint16_t slot; /**< ECC Key slot */
    /**
     * @brief
     * The Elliptic Curve the key is generated from.
     */
    uint8_t curve;   /**< Elliptic Curve */
    uint8_t tag[16]; /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_ecc_key_generate_cmd_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_generate_cmd_t, cmd_size) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_generate_cmd_t, cmd_id) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_generate_cmd_t, slot) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_generate_cmd_t, curve) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_generate_cmd_t, tag)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Command to generate an ECC Key and store the key in a slot from the ECC Keys partition in R-Memory.
 */
struct lt_l3_ecc_key_generate_res_t {
    uint16_t res_size; /**< L3 result size */
    uint8_t result;    /**< Result status indication */
    uint8_t tag[16];   /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_ecc_key_generate_res_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_generate_res_t, res_size) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_generate_res_t, result) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_generate_res_t, tag)
    )
)
/** \endcond */
//clang-format on

/** @brief Command ID */
#define TR01_L3_ECC_KEY_STORE_CMD_ID 0x61
/** @brief Command length */
#define TR01_L3_ECC_KEY_STORE_CMD_SIZE 48u
/** @brief P256 Curve - 64-byte long public key. */
#define TR01_L3_ECC_KEY_STORE_CMD_CURVE_P256 0x01
/** @brief Ed25519 Curve - 32-byte long public key. */
#define TR01_L3_ECC_KEY_STORE_CMD_CURVE_ED25519 0x02

/** @brief Result length */
#define TR01_L3_ECC_KEY_STORE_RES_SIZE 1u

/**
 * @brief
 * Command to store an ECC Key in a slot from the ECC Keys partition in R-Memory.
 */
struct lt_l3_ecc_key_store_cmd_t {
    uint16_t cmd_size; /**< L3 command size */
    uint8_t cmd_id;    /**< Command Identifier */
    /**
     * @brief
     * The slot to write the K L3 Field. Valid values are 0 - 31.
     */
    uint16_t slot; /**< ECC Key slot */
    /**
     * @brief
     * The Elliptic Curve the key is generated from.
     */
    uint8_t curve; /**< The type of Elliptic Curve the K L3 Field belongs to. */
    /**
     * @brief
     * The padding by dummy data.
     */
    uint8_t padding[12]; /**< Padding */
    /**
     * @brief
     * The ECC Key to store. The key must be a member of the field given by the curve specified in the CURVE L3 Field.
     */
    uint8_t k[32];   /**< Key to store */
    uint8_t tag[16]; /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_ecc_key_store_cmd_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_store_cmd_t, cmd_size) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_store_cmd_t, cmd_id) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_store_cmd_t, slot) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_store_cmd_t, curve) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_store_cmd_t, padding) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_store_cmd_t, k) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_store_cmd_t, tag)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Command to store an ECC Key in a slot from the ECC Keys partition in R-Memory.
 */
struct lt_l3_ecc_key_store_res_t {
    uint16_t res_size; /**< L3 result size */
    uint8_t result;    /**< Result status indication */
    uint8_t tag[16];   /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_ecc_key_store_res_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_store_res_t, res_size) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_store_res_t, result) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_store_res_t, tag)
    )
)
/** \endcond */
// clang-format on

/** @brief Command ID */
#define TR01_L3_ECC_KEY_READ_CMD_ID 0x62
/** @brief Command length */
#define TR01_L3_ECC_KEY_READ_CMD_SIZE 3u
/** @brief P256 Curve - 64-byte long public key. */
#define TR01_L3_ECC_KEY_READ_CMD_CURVE_P256 0x01
/** @brief Ed25519 Curve - 32-byte long public key. */
#define TR01_L3_ECC_KEY_READ_CMD_CURVE_ED25519 0x02
/** @brief The key is from key generation on the device. */
#define TR01_L3_ECC_KEY_READ_CMD_ORIGIN_ECC_KEY_GENERATE 0x01
/** @brief The key is from key storage in the device. */
#define TR01_L3_ECC_KEY_READ_CMD_ORIGIN_ECC_KEY_STORE 0x02

/** @brief Result length */
#define TR01_L3_ECC_KEY_READ_RES_SIZE_MIN 48u

/**
 * @brief
 * Command to read the public ECC Key from a slot of the ECC Keys partition in R-Memory.
 */
struct lt_l3_ecc_key_read_cmd_t {
    uint16_t cmd_size; /**< L3 command size */
    uint8_t cmd_id;    /**< Command Identifier */
    /**
     * @brief
     * The slot to read the public ECC Key from. Valid values are 0 - 31.
     */
    uint16_t slot;   /**< ECC Key slot */
    uint8_t tag[16]; /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_ecc_key_read_cmd_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_read_cmd_t, cmd_size) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_read_cmd_t, cmd_id) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_read_cmd_t, slot) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_read_cmd_t, tag)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Command to read the public ECC Key from a slot of the ECC Keys partition in R-Memory.
 */
struct lt_l3_ecc_key_read_res_t {
    uint16_t res_size; /**< L3 result size */
    uint8_t result;    /**< Result status indication */
    /**
     * @brief
     * The type of Elliptic Curve public key returned.
     */
    uint8_t curve; /**< Elliptic Curve */
    /**
     * @brief
     * The origin of the key.
     */
    uint8_t origin; /**< Origin of the key. */
    /**
     * @brief
     * The padding by dummy data.
     */
    uint8_t padding[13]; /**< Padding */
    /**
     * @brief
     * The public key from the ECC Key slot as specified in the SLOT L3 Field.
     */
    uint8_t pub_key[64]; /**< Public Key */
    uint8_t tag[16];     /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_ecc_key_read_res_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_read_res_t, res_size) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_read_res_t, result) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_read_res_t, curve) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_read_res_t, origin) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_read_res_t, padding) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_read_res_t, pub_key) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_read_res_t, tag)
    )
)
/** \endcond */
// clang-format on

/** @brief Command ID */
#define TR01_L3_ECC_KEY_ERASE_CMD_ID 0x63
/** @brief Command length */
#define TR01_L3_ECC_KEY_ERASE_CMD_SIZE 3u

/** @brief Result length */
#define TR01_L3_ECC_KEY_ERASE_RES_SIZE 1u

/**
 * @brief
 * Command to erase an ECC Key from a slot in the ECC Keys partition in R-Memory.
 */
struct lt_l3_ecc_key_erase_cmd_t {
    uint16_t cmd_size; /**< L3 command size */
    uint8_t cmd_id;    /**< Command Identifier */
    /**
     * @brief
     * The slot to erase. Valid values are 0 - 31.
     */
    uint16_t slot;   /**< ECC Key slot */
    uint8_t tag[16]; /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_ecc_key_erase_cmd_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_erase_cmd_t, cmd_size) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_erase_cmd_t, cmd_id) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_erase_cmd_t, slot) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_erase_cmd_t, tag)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Command to erase an ECC Key from a slot in the ECC Keys partition in R-Memory.
 */
struct lt_l3_ecc_key_erase_res_t {
    uint16_t res_size; /**< L3 result size */
    uint8_t result;    /**< Result status indication */
    uint8_t tag[16];   /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_ecc_key_erase_res_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_erase_res_t, res_size) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_erase_res_t, result) +
        LT_MEMBER_SIZE(struct lt_l3_ecc_key_erase_res_t, tag)
    )
)
/** \endcond */
// clang-format on

/** @brief Command ID */
#define TR01_L3_ECDSA_SIGN_CMD_ID 0x70
/** @brief Command length */
#define TR01_L3_ECDSA_SIGN_CMD_SIZE 48u

/** @brief Result length */
#define TR01_L3_ECDSA_SIGN_RES_SIZE 80u

/**
 * @brief
 * Command to sign a message hash with an ECDSA algorithm.
 */
struct lt_l3_ecdsa_sign_cmd_t {
    uint16_t cmd_size; /**< L3 command size */
    uint8_t cmd_id;    /**< Command Identifier */
    /**
     * @brief
     * The slot (from the ECC Keys partition in R-Memory) to read the key for ECDSA signing.
     */
    uint16_t slot; /**< ECC Key slot */
    /**
     * @brief
     * The padding by dummy data.
     */
    uint8_t padding[13]; /**< Padding */
    /**
     * @brief
     * The hash of the message to sign (max size of 32 bytes).
     */
    uint8_t msg_hash[32]; /**< Hash of the Message to sign. */
    uint8_t tag[16];      /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_ecdsa_sign_cmd_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_ecdsa_sign_cmd_t, cmd_size) +
        LT_MEMBER_SIZE(struct lt_l3_ecdsa_sign_cmd_t, cmd_id) +
        LT_MEMBER_SIZE(struct lt_l3_ecdsa_sign_cmd_t, slot) +
        LT_MEMBER_SIZE(struct lt_l3_ecdsa_sign_cmd_t, padding) +
        LT_MEMBER_SIZE(struct lt_l3_ecdsa_sign_cmd_t, msg_hash) +
        LT_MEMBER_SIZE(struct lt_l3_ecdsa_sign_cmd_t, tag)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Command to sign a message hash with an ECDSA algorithm.
 */
struct lt_l3_ecdsa_sign_res_t {
    uint16_t res_size; /**< L3 result size */
    uint8_t result;    /**< Result status indication */
    /**
     * @brief
     * The padding by dummy data.
     */
    uint8_t padding[15]; /**< Padding */
    /**
     * @brief
     * ECDSA signature - The R part
     */
    uint8_t r[32]; /**< ECDSA Signature - R part */
    /**
     * @brief
     * ECDSA signature - The S part
     */
    uint8_t s[32];   /**< ECDSA Signature - S part */
    uint8_t tag[16]; /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_ecdsa_sign_res_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_ecdsa_sign_res_t, res_size) +
        LT_MEMBER_SIZE(struct lt_l3_ecdsa_sign_res_t, result) +
        LT_MEMBER_SIZE(struct lt_l3_ecdsa_sign_res_t, padding) +
        LT_MEMBER_SIZE(struct lt_l3_ecdsa_sign_res_t, r) +
        LT_MEMBER_SIZE(struct lt_l3_ecdsa_sign_res_t, s) +
        LT_MEMBER_SIZE(struct lt_l3_ecdsa_sign_res_t, tag)
    )
)
/** \endcond */
// clang-format on

/** @brief Command ID */
#define TR01_L3_EDDSA_SIGN_CMD_ID 0x71
/** @brief Command length */
#define TR01_L3_EDDSA_SIGN_CMD_SIZE_MIN 17u
/** @brief Maximal length of field msg */
#define TR01_L3_EDDSA_SIGN_CMD_MSG_LEN_MAX 4096u

/** @brief Result length */
#define TR01_L3_EDDSA_SIGN_RES_SIZE 80u

/**
 * @brief
 * Command to sign a message with an EdDSA algorithm.
 */
struct lt_l3_eddsa_sign_cmd_t {
    uint16_t cmd_size; /**< L3 command size */
    uint8_t cmd_id;    /**< Command Identifier */
    /**
     * @brief
     * The slot (from the ECC Keys partition in R-Memory) to read the key for EdDSA signing.
     */
    uint16_t slot; /**< ECC Key slot */
    /**
     * @brief
     * The padding by dummy data.
     */
    uint8_t padding[13]; /**< Padding */
    /**
     * @brief
     * The message to sign (max size of 4096 bytes).
     */
    uint8_t msg[4096]; /**< Message to sign. */
    uint8_t tag[16];   /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_eddsa_sign_cmd_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_eddsa_sign_cmd_t, cmd_size) +
        LT_MEMBER_SIZE(struct lt_l3_eddsa_sign_cmd_t, cmd_id) +
        LT_MEMBER_SIZE(struct lt_l3_eddsa_sign_cmd_t, slot) +
        LT_MEMBER_SIZE(struct lt_l3_eddsa_sign_cmd_t, padding) +
        LT_MEMBER_SIZE(struct lt_l3_eddsa_sign_cmd_t, msg) +
        LT_MEMBER_SIZE(struct lt_l3_eddsa_sign_cmd_t, tag)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Command to sign a message with an EdDSA algorithm.
 */
struct lt_l3_eddsa_sign_res_t {
    uint16_t res_size; /**< L3 result size */
    uint8_t result;    /**< Result status indication */
    /**
     * @brief
     * The padding by dummy data.
     */
    uint8_t padding[15]; /**< Padding */
    /**
     * @brief
     * EdDSA signature - The R part
     */
    uint8_t r[32]; /**< EDDSA Signature - R part */
    /**
     * @brief
     * EdDSA signature - The S part
     */
    uint8_t s[32];   /**< EDDSA Signature - S part */
    uint8_t tag[16]; /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_eddsa_sign_res_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_eddsa_sign_res_t, res_size) +
        LT_MEMBER_SIZE(struct lt_l3_eddsa_sign_res_t, result) +
        LT_MEMBER_SIZE(struct lt_l3_eddsa_sign_res_t, padding) +
        LT_MEMBER_SIZE(struct lt_l3_eddsa_sign_res_t, r) +
        LT_MEMBER_SIZE(struct lt_l3_eddsa_sign_res_t, s) +
        LT_MEMBER_SIZE(struct lt_l3_eddsa_sign_res_t, tag)
    )
)
/** \endcond */
// clang-format on

/** @brief Command ID */
#define TR01_L3_MCOUNTER_INIT_CMD_ID 0x80
/** @brief Command length */
#define TR01_L3_MCOUNTER_INIT_CMD_SIZE 8u

/** @brief Result length */
#define TR01_L3_MCOUNTER_INIT_RES_SIZE 1u

/**
 * @brief
 * Command to initialize the Monotonic Counter.
 */
struct lt_l3_mcounter_init_cmd_t {
    uint16_t cmd_size; /**< L3 command size */
    uint8_t cmd_id;    /**< Command Identifier */
    /**
     * @brief
     * The index of the Monotonic Counter to initialize. Valid values are 0 - 15.
     */
    uint16_t mcounter_index; /**< Index of Monotonic Counter */
    /**
     * @brief
     * The padding by dummy data.
     */
    uint8_t padding; /**< Padding */
    /**
     * @brief
     * The initialization value of the Monotonic Counter.
     */
    uint32_t mcounter_val; /**< Initialization value. */
    uint8_t tag[16];       /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_mcounter_init_cmd_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_mcounter_init_cmd_t, cmd_size) +
        LT_MEMBER_SIZE(struct lt_l3_mcounter_init_cmd_t, cmd_id) +
        LT_MEMBER_SIZE(struct lt_l3_mcounter_init_cmd_t, mcounter_index) +
        LT_MEMBER_SIZE(struct lt_l3_mcounter_init_cmd_t, padding) +
        LT_MEMBER_SIZE(struct lt_l3_mcounter_init_cmd_t, mcounter_val) +
        LT_MEMBER_SIZE(struct lt_l3_mcounter_init_cmd_t, tag)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Command to initialize the Monotonic Counter.
 */
struct lt_l3_mcounter_init_res_t {
    uint16_t res_size; /**< L3 result size */
    uint8_t result;    /**< Result status indication */
    uint8_t tag[16];   /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_mcounter_init_res_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_mcounter_init_res_t, res_size) +
        LT_MEMBER_SIZE(struct lt_l3_mcounter_init_res_t, result) +
        LT_MEMBER_SIZE(struct lt_l3_mcounter_init_res_t, tag)
    )
)
/** \endcond */
// clang-format on

/** @brief Command ID */
#define TR01_L3_MCOUNTER_UPDATE_CMD_ID 0x81
/** @brief Command length */
#define TR01_L3_MCOUNTER_UPDATE_CMD_SIZE 3u

/** @brief Result length */
#define TR01_L3_MCOUNTER_UPDATE_RES_SIZE 1u

/**
 * @brief
 * Command to update the Monotonic Counter (decrement by 1).
 */
struct lt_l3_mcounter_update_cmd_t {
    uint16_t cmd_size; /**< L3 command size */
    uint8_t cmd_id;    /**< Command Identifier */
    /**
     * @brief
     * The index of the Monotonic Counter to update. Valid values are 0 - 15.
     */
    uint16_t mcounter_index; /**< Index of Monotonic Counter */
    uint8_t tag[16];         /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_mcounter_update_cmd_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_mcounter_update_cmd_t, cmd_size) +
        LT_MEMBER_SIZE(struct lt_l3_mcounter_update_cmd_t, cmd_id) +
        LT_MEMBER_SIZE(struct lt_l3_mcounter_update_cmd_t, mcounter_index) +
        LT_MEMBER_SIZE(struct lt_l3_mcounter_update_cmd_t, tag)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Command to update the Monotonic Counter (decrement by 1).
 */
struct lt_l3_mcounter_update_res_t {
    uint16_t res_size; /**< L3 result size */
    uint8_t result;    /**< Result status indication */
    uint8_t tag[16];   /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_mcounter_update_res_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_mcounter_update_res_t, res_size) +
        LT_MEMBER_SIZE(struct lt_l3_mcounter_update_res_t, result) +
        LT_MEMBER_SIZE(struct lt_l3_mcounter_update_res_t, tag)
    )
)
/** \endcond */
// clang-format on

/** @brief Command ID */
#define TR01_L3_MCOUNTER_GET_CMD_ID 0x82
/** @brief Command length */
#define TR01_L3_MCOUNTER_GET_CMD_SIZE 3u

/** @brief Result length */
#define TR01_L3_MCOUNTER_GET_RES_SIZE 8u

/**
 * @brief
 * Command to get the value of the Monotonic Counter.
 */
struct lt_l3_mcounter_get_cmd_t {
    uint16_t cmd_size; /**< L3 command size */
    uint8_t cmd_id;    /**< Command Identifier */
    /**
     * @brief
     * The index of the Monotonic Counter to get the value of. Valid index values are 0 - 15.
     */
    uint16_t mcounter_index; /**< Index of Monotonic Counter */
    uint8_t tag[16];         /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_mcounter_get_cmd_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_mcounter_get_cmd_t, cmd_size) +
        LT_MEMBER_SIZE(struct lt_l3_mcounter_get_cmd_t, cmd_id) +
        LT_MEMBER_SIZE(struct lt_l3_mcounter_get_cmd_t, mcounter_index) +
        LT_MEMBER_SIZE(struct lt_l3_mcounter_get_cmd_t, tag)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Command to get the value of the Monotonic Counter.
 */
struct lt_l3_mcounter_get_res_t {
    uint16_t res_size; /**< L3 result size */
    uint8_t result;    /**< Result status indication */
    /**
     * @brief
     * The padding by dummy data.
     */
    uint8_t padding[3]; /**< Padding */
    /**
     * @brief
     * The value of the Monotonic Counter specified by the MCOUNTER_INDEX L3 Field.
     */
    uint32_t mcounter_val; /**< Initialization value. */
    uint8_t tag[16];       /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_mcounter_get_res_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_mcounter_get_res_t, res_size) +
        LT_MEMBER_SIZE(struct lt_l3_mcounter_get_res_t, result) +
        LT_MEMBER_SIZE(struct lt_l3_mcounter_get_res_t, padding) +
        LT_MEMBER_SIZE(struct lt_l3_mcounter_get_res_t, mcounter_val) +
        LT_MEMBER_SIZE(struct lt_l3_mcounter_get_res_t, tag)
    )
)
/** \endcond */
// clang-format on

/** @brief Command ID */
#define TR01_L3_MAC_AND_DESTROY_CMD_ID 0x90
/** @brief Command length */
#define TR01_L3_MAC_AND_DESTROY_CMD_SIZE 36u

/** @brief Result length */
#define TR01_L3_MAC_AND_DESTROY_RES_SIZE 36u

/**
 * @brief
 * Command to execute the MAC-and-Destroy sequence.
 */
struct lt_l3_mac_and_destroy_cmd_t {
    uint16_t cmd_size; /**< L3 command size */
    uint8_t cmd_id;    /**< Command Identifier */
    /**
     * @brief
     * The slot (from the MAC-and-Destroy data partition in R-Memory) to execute the MAC_And_Destroy sequence. Valid
     * values are 0 - 127.
     */
    uint16_t slot; /**< Mac-and-Destroy slot */
    /**
     * @brief
     * The padding by dummy data.
     */
    uint8_t padding; /**< Padding */
    /**
     * @brief
     * The data input for the MAC-and-Destroy sequence.
     */
    uint8_t data_in[32]; /**< Input data */
    uint8_t tag[16];     /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_mac_and_destroy_cmd_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_mac_and_destroy_cmd_t, cmd_size) +
        LT_MEMBER_SIZE(struct lt_l3_mac_and_destroy_cmd_t, cmd_id) +
        LT_MEMBER_SIZE(struct lt_l3_mac_and_destroy_cmd_t, slot) +
        LT_MEMBER_SIZE(struct lt_l3_mac_and_destroy_cmd_t, padding) +
        LT_MEMBER_SIZE(struct lt_l3_mac_and_destroy_cmd_t, data_in) +
        LT_MEMBER_SIZE(struct lt_l3_mac_and_destroy_cmd_t, tag)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Command to execute the MAC-and-Destroy sequence.
 */
struct lt_l3_mac_and_destroy_res_t {
    uint16_t res_size; /**< L3 result size */
    uint8_t result;    /**< Result status indication */
    /**
     * @brief
     * The padding by dummy data.
     */
    uint8_t padding[3]; /**< Padding */
    /**
     * @brief
     * The data output from the MAC-and-Destroy sequence.
     */
    uint8_t data_out[32]; /**< Output data */
    uint8_t tag[16];      /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_mac_and_destroy_res_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_mac_and_destroy_res_t, res_size) +
        LT_MEMBER_SIZE(struct lt_l3_mac_and_destroy_res_t, result) +
        LT_MEMBER_SIZE(struct lt_l3_mac_and_destroy_res_t, padding) +
        LT_MEMBER_SIZE(struct lt_l3_mac_and_destroy_res_t, data_out) +
        LT_MEMBER_SIZE(struct lt_l3_mac_and_destroy_res_t, tag)
    )
)
/** \endcond */
// clang-format on

/** @brief Command ID */
#define TR01_L3_SERIAL_CODE_GET_CMD_ID 0xa0
/** @brief Command length */
#define TR01_L3_SERIAL_CODE_GET_CMD_SIZE 1u

/** @brief Result length */
#define TR01_L3_SERIAL_CODE_GET_RES_SIZE 36u

/**
 * @brief
 * Command to obtain the unique per-chip identifier.
 */
struct lt_l3_serial_code_get_cmd_t {
    uint16_t cmd_size; /**< L3 command size */
    uint8_t cmd_id;    /**< Command Identifier */
    uint8_t tag[16];   /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_serial_code_get_cmd_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_serial_code_get_cmd_t, cmd_size) +
        LT_MEMBER_SIZE(struct lt_l3_serial_code_get_cmd_t, cmd_id) +
        LT_MEMBER_SIZE(struct lt_l3_serial_code_get_cmd_t, tag)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief
 * Command to obtain the unique per-chip identifier.
 */
struct lt_l3_serial_code_get_res_t {
    uint16_t res_size; /**< L3 result size */
    uint8_t result;    /**< Result status indication */
    /**
     * @brief
     * The padding by dummy data.
     */
    uint8_t padding[3]; /**< Padding */
    /**
     * @brief
     * The unique per-chip identifier.
     */
    uint8_t serial_code[32]; /**< Serial code */
    uint8_t tag[16];         /**< L3 tag */
} __attribute__((packed));

// clang-format off
/** \cond */
LT_STATIC_ASSERT(
    sizeof(struct lt_l3_serial_code_get_res_t) ==
    (
        LT_MEMBER_SIZE(struct lt_l3_serial_code_get_res_t, res_size) +
        LT_MEMBER_SIZE(struct lt_l3_serial_code_get_res_t, result) +
        LT_MEMBER_SIZE(struct lt_l3_serial_code_get_res_t, padding) +
        LT_MEMBER_SIZE(struct lt_l3_serial_code_get_res_t, serial_code) +
        LT_MEMBER_SIZE(struct lt_l3_serial_code_get_res_t, tag)
    )
)
/** \endcond */
// clang-format on

#endif  // !LT_L3_API_STRUCTS_H
