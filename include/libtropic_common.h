#ifndef LT_LIBTROPIC_COMMON_H
#define LT_LIBTROPIC_COMMON_H

/**
 * @file libtropic_common.h
 * @brief Shared definitions and functions commonly used by more libtropic's layers
 * @author Tropic Square s.r.o. *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic_macros.h"
#include "stdint.h"
#include "tropic01_application_co.h"
#include "tropic01_bootloader_co.h"

// This macro is used to change static functions into exported one, when compiling unit tests.
// It allows to unit test static functions.
#ifndef TEST
#define LT_STATIC static
#else
#define LT_STATIC
#endif

/** @brief This particular value means that secure session was successfully established and it is currently ON */
#define SESSION_ON 0xA5A55A5A
/** @brief This particular value means that secure session is currently OFF */
#define SESSION_OFF 0x0

/** @brief Size of l3 ID field */
#define L3_ID_SIZE 1u
/** @brief Size of l3 TAG field */
#define L3_TAG_SIZE 16u
/** @brief Size of IV */
#define L3_IV_SIZE 12u

/** @brief Size of RES_SIZE field */
#define L3_RES_SIZE_SIZE 2
/** @brief Size of CMD_SIZE field */
#define L3_CMD_SIZE_SIZE 2
/** @brief Size of l3 CMD_ID field */
#define L3_CMD_ID_SIZE (1)
/** @brief Maximal size of l3 RES/RSP DATA field */
#define L3_CMD_DATA_SIZE_MAX (4111)

/** @brief Maximal size of data field in one L2 transfer */
#define L2_CHUNK_MAX_DATA_SIZE 252u
/** @brief Maximal size of one l2 frame */
#define L2_MAX_FRAME_SIZE (1 + 1 + L2_CHUNK_MAX_DATA_SIZE + 2)
/** @brief Maximal number of data bytes in one L1 transfer */
#define LT_L1_LEN_MIN 1
/** @brief Maximal number of data bytes in one L1 transfer */
#define LT_L1_LEN_MAX (1 + 1 + 1 + L2_CHUNK_MAX_DATA_SIZE + 2)

/** @brief Maximum size of l3 ciphertext (or decrypted l3 packet) */
#define L3_CYPHERTEXT_MAX_SIZE (L3_CMD_ID_SIZE + L3_CMD_DATA_SIZE_MAX)
/**
 * @brief Max size of one unit of transport on l3 layer
 *
 * The number 13 is given by the longest possible padding, which is given by the EDDSA_Sign command.
 */
#define L3_PACKET_MAX_SIZE (L3_RES_SIZE_SIZE + L3_CYPHERTEXT_MAX_SIZE + 13 + L3_TAG_SIZE)

//--------------------------------------------------------------------------------------------------------------------//

/** @brief Generic L3 command and result frame */
typedef struct lt_l3_gen_frame_t {
    /** @brief RES_SIZE or CMD_SIZE value */
    uint16_t cmd_size;
    /** @brief Command or result data including ID and TAG */
    uint8_t data[L3_PACKET_MAX_SIZE - L3_RES_SIZE_SIZE];
} __attribute__((packed)) lt_l3_gen_frame_t;

// clang-format off
/** \cond */
/** \cond */
STATIC_ASSERT(
    sizeof(struct lt_l3_gen_frame_t) ==
    (
        MEMBER_SIZE(struct lt_l3_gen_frame_t, cmd_size) +
        MEMBER_SIZE(struct lt_l3_gen_frame_t, data)
    )
)
/** \endcond */
/** \endcond */
// clang-format on

//--------------------------------------------------------------------------------------------------------------------//

typedef struct lt_l2_state_t {
    void *device;
    uint8_t mode;
    uint8_t buff[1 + L2_MAX_FRAME_SIZE];
} lt_l2_state_t;

// #define LT_SIZE_OF_L3_BUFF (1000)
#ifndef LT_SIZE_OF_L3_BUFF
#define LT_SIZE_OF_L3_BUFF L3_PACKET_MAX_SIZE
#endif

typedef struct lt_l3_state_t {
    uint32_t session;
    uint8_t encryption_IV[12];
    uint8_t decryption_IV[12];
#if LT_USE_TREZOR_CRYPTO
    uint8_t encrypt[352] __attribute__((aligned(16)));  // Because sizeof(lt_aes_gcm_ctx_t) == 352;
    uint8_t decrypt[352] __attribute__((aligned(16)));
#elif USE_MBEDTLS
#warning "Warning: MBED Tls is not implemented yet";
#else
    // Default size of gcm context structures are set to reflect sizes used in trezor_crypto library
    uint8_t encrypt[352] __attribute__((aligned(16)));
    uint8_t decrypt[352] __attribute__((aligned(16)));
#endif
#if LT_SEPARATE_L3_BUFF
    /** User shall define buffer's array and store its pointer into handle */
    uint8_t *buff __attribute__((aligned(16)));
#else
    /** Buffer for L3 commands and results */
    uint8_t buff[LT_SIZE_OF_L3_BUFF] __attribute__((aligned(16)));
#endif
    uint16_t buff_len; /**< Length of the buffer */
} lt_l3_state_t;

/**
 * @details This structure holds data related to one physical chip.
 * Contains AESGCM contexts for encrypting and decrypting L3 commands, nonce and device void pointer, which can be used
 * for passing arbitrary data.
 */
typedef struct lt_handle_t {
    lt_l2_state_t l2;
    lt_l3_state_t l3;
} lt_handle_t;

/**
 * @brief Enum return type.
 * @note Specific values are given for easier lookup of values.
 */
typedef enum lt_ret_t {
    /** @brief Operation was successful */
    LT_OK = 0,
    /** @brief Operation was not successful */
    LT_FAIL = 1,
    /* Host no session */
    LT_HOST_NO_SESSION = 2,
    /** @brief Some parameter was not accepted by function */
    LT_PARAM_ERR = 3,
    /** @brief Error detected during cryptographic operation */
    LT_CRYPTO_ERR = 4,

    /** @brief Some SPI related operation was not successful */
    LT_L1_SPI_ERROR = 5,
    /** @brief Data does not have an expected length */
    LT_L1_DATA_LEN_ERROR = 6,
    /** @brief Chip is in STARTUP mode */
    LT_L1_CHIP_STARTUP_MODE = 7,
    /** @brief Chip is in ALARM mode */
    LT_L1_CHIP_ALARM_MODE = 8,
    /** @brief Chip is BUSY - typically chip is still booting */
    LT_L1_CHIP_BUSY = 9,
    /** @brief Interrupt pin did not fire as expected */
    LT_L1_INT_TIMEOUT = 10,

    // Return values based on RESULT field
    /** @brief User slot is empty */
    LT_L3_R_MEM_DATA_READ_SLOT_EMPTY = 11,
    /** @brief L3 result [API r_mem_data_write]: write failed, because slot is already written in */
    LT_L3_R_MEM_DATA_WRITE_WRITE_FAIL = 12,
    /** @brief L3 result [API r_mem_data_write]: writing operation limit is reached for a given slot */
    LT_L3_R_MEM_DATA_WRITE_SLOT_EXPIRED = 13,
    /** @brief L3 result [API EDDSA_sign, ECDSA_sign, ecc_key_read]: The key in the requested slot does not exist, or is
       invalid. */
    LT_L3_ECC_INVALID_KEY = 14,
    /** @brief L3 result [API mcounter_update]: Failure to update the speciﬁed Monotonic Counter. The Monotonic Counter
       is already at 0. */
    LT_L3_MCOUNTER_UPDATE_UPDATE_ERR = 15,
    /** @brief L3 result [API mcounter_update, mcounter_get]: The Monotonic Counter detects an attack and is locked. The
       counter must be reinitialized. */
    LT_L3_COUNTER_INVALID = 16,
    /** @brief L3 result [API pairing_key_read], The Pairing key slot is in "Blank" state. A Pairing Key has not been
       written to it yet */
    LT_L3_PAIRING_KEY_EMPTY = 17,
    /** @brief L3 result [API pairing_key_read], The Pairing key slot is in "Invalidated" state. The Pairing key has
       been invalidated */
    LT_L3_PAIRING_KEY_INVALID = 18,
    /** @brief L3 command was received correctly*/
    LT_L3_OK = 19,
    /** @brief L3 command was not received correctly */
    LT_L3_FAIL = 20,
    /** @brief Current pairing keys are not authorized for execution of the last command */
    LT_L3_UNAUTHORIZED = 21,
    /** @brief Received L3 command is invalid */
    LT_L3_INVALID_CMD = 22,
    /** @brief L3 data does not have an expected length */
    LT_L3_DATA_LEN_ERROR = 23,

    // Return values based on STATUS field
    /** @brief l2 response frame contains CRC error */
    LT_L2_IN_CRC_ERR = 24,
    /** @brief There is more than one chunk to be expected for a current request */
    LT_L2_REQ_CONT = 25,
    /** @brief There is more than one chunk to be received for a current response */
    LT_L2_RES_CONT = 26,
    /** @brief The L2 Request frame is disabled and can’t be executed */
    LT_L2_RESP_DISABLED = 27,
    /** @brief There were an error during handshake establishing */
    LT_L2_HSK_ERR = 28,
    /** @brief There is no secure session */
    LT_L2_NO_SESSION = 29,
    /** @brief There were error during checking message authenticity */
    LT_L2_TAG_ERR = 30,
    /** @brief l2 request contained crc error */
    LT_L2_CRC_ERR = 31,
    /** @brief There were some other error */
    LT_L2_GEN_ERR = 32,
    /** @brief Chip has no response to be transmitted */
    LT_L2_NO_RESP = 33,
    /** @brief ID of last request is not known to TROPIC01 */
    LT_L2_UNKNOWN_REQ = 34,
    /** @brief Returned status byte is not recognized at all */
    LT_L2_STATUS_NOT_RECOGNIZED = 35,
    /** @brief L2 data does not have an expected length */
    LT_L2_DATA_LEN_ERROR = 36,

    // Certificate store related errors
    /** @brief Certificate store likely does not contain valid data */
    LT_CERT_STORE_INVALID = 37,
    /** @brief Certificate store contains ASN1-DER syntax that is beyond the supported subset*/
    LT_CERT_UNSUPPORTED = 38,
    /** @brief Certificate does not contain requested item */
    LT_CERT_ITEM_NOT_FOUND = 39,
    /** @brief The nonce has reached its maximum value. */
    LT_NONCE_OVERFLOW = 40,

    /** @brief Special helper value used to signalize the last enum value, used in lt_ret_verbose. */
    LT_RET_T_LAST_VALUE = 41
} lt_ret_t;

#define LT_TROPIC01_REBOOT_DELAY_MS 250

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal size of TROPIC01's certificate */
#define LT_L2_GET_INFO_REQ_CERT_SIZE_TOTAL 3840
#define LT_L2_GET_INFO_REQ_CERT_SIZE_SINGLE 700

typedef enum lt_cert_kind_t {
    LT_CERT_KIND_DEVICE = 0,
    LT_CERT_KIND_XXXX = 1,
    LT_CERT_KIND_TROPIC01 = 2,
    LT_CERT_KIND_TROPIC_ROOT = 3,
} lt_cert_kind_t;

#define LT_CERT_STORE_VERSION 1
#define LT_NUM_CERTIFICATES 4

/**
 * @brief Certificate store contents
 */
typedef struct lt_cert_store_t {
    uint8_t *certs[LT_NUM_CERTIFICATES];    /** Certificates */
    uint16_t buf_len[LT_NUM_CERTIFICATES];  /** Length of buffers for certificates */
    uint16_t cert_len[LT_NUM_CERTIFICATES]; /** Lenght of certificates (from Cert store header) */
} lt_cert_store_t;

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal size of returned CHIP ID */
#define LT_L2_GET_INFO_CHIP_ID_SIZE 128

//--------------------------------------------------------------------------------------------------------------------//
/**
 * @brief Structure used to parse content of CHIP_ID field
 *
 * @details This structure contains fields for parsing the chip's serial number data.
 */
typedef struct lt_ser_num_t {
    uint8_t sn;          /**< 8 bits for serial number */
    uint8_t fab_data[3]; /**< 12 bits fab ID, 12 bits part number ID */
    uint16_t fab_date;   /**< 16 bits for fabrication date */
    uint8_t lot_id[5];   /**< 40 bits for lot ID */
    uint8_t wafer_id;    /**< 8 bits for wafer ID */
    uint16_t x_coord;    /**< 16 bits for x-coordinate */
    uint16_t y_coord;    /**< 16 bits for y-coordinate */
} __attribute__((packed)) lt_ser_num_t;

// clang-format off
/** \cond */
STATIC_ASSERT(
    ( sizeof(struct lt_ser_num_t) )
    ==
    (
        MEMBER_SIZE(struct lt_ser_num_t, sn) + 
        MEMBER_SIZE(struct lt_ser_num_t, fab_data) + 
        MEMBER_SIZE(struct lt_ser_num_t, fab_date) + 
        MEMBER_SIZE(struct lt_ser_num_t, lot_id) + 
        MEMBER_SIZE(struct lt_ser_num_t, wafer_id) + 
        MEMBER_SIZE(struct lt_ser_num_t, x_coord) + 
        MEMBER_SIZE(struct lt_ser_num_t, y_coord)
    )
)
/** \endcond */
// clang-format on

//--------------------------------------------------------------------------------------------------------------------//

/** @brief Package type ID for bare silicon. */
#define CHIP_PKG_BARE_SILICON_ID 0x8000
/** @brief Package type ID for QFN32. */
#define CHIP_PKG_QFN32_ID 0x80AA
/* Fab ID of Tropic Square Lab. */
#define FAB_ID_TROPIC_SQUARE_LAB 0xF00
/* Fab ID of Production line #1. */
#define FAB_ID_EPS_BRNO 0x001

/**
 * @brief Data in this struct comes from BP (batch package) yml file. CHIP_INFO is read into this struct.
 */
typedef struct lt_chip_id_t {
    /**
     * @brief CHIP_ID structure versioning (32 bits), defined by Tropic Square in BP.
     * @details Example encoding: v1.2.3.4 = 0x01,0x02,0x03,0x04
     */
    uint8_t chip_id_ver[4];

    /**
     * @brief Factory level test info (128 bits), structure retrieved from silicon provider.
     * @details The exact copy of FL_PROD_DATA structure. If missing, it is filled with 0x00.
     */
    uint8_t fl_chip_info[16];

    /**
     * @brief Manufacturing level test info (128 bits), structure retrieved from test line and BP.
     * @details The exact copy of ﬁrst two words of MAN_FUNC_TEST structure. In case of missing, it is filled with 0x00
     */
    uint8_t func_test_info[8];

    /**
     * @brief Silicon revision (32 bits).
     * @details ASCII encoded string value deﬁned by Tropic Square. Example: ’ACAB’ = 0x41434142
     */
    uint8_t silicon_rev[4];

    /**
     * @brief Package Type ID deﬁned by Tropic Square
     */
    uint8_t packg_type_id[2];

    /**
     * @brief Reserved field 1 (16 bits).
     */
    uint8_t rfu_1[2];

    /**
     * @brief Provisioning info (128 bits), filled by the provisioning station.
     * @details
     * - 8 bits: Provisioning info version.
     * - 12 bits: Fabrication ID.
     * - 12 bits: Part Number ID.
     */
    uint8_t prov_ver_fab_id_pn[4];

    /**
     * @brief Provisioning date (16 bits).
     */
    uint8_t provisioning_date[2];

    /**
     * @brief HSM version (32 bits).
     * @details Byte 0: RFU, Byte 1: Major version, Byte 2: Minor version, Byte 3: Patch version
     */
    uint8_t hsm_ver[4];

    /**
     * @brief Program version (32 bits).
     */
    uint8_t prog_ver[4];

    /**
     * @brief Reserved field 2 (16 bits).
     */
    uint8_t rfu_2[2];

    /**
     * @brief Serial Number (128 bits).
     */
    struct lt_ser_num_t ser_num;

    /**
     * @brief Part Number (128 bits), defined by Tropic Square in BP.
     */
    uint8_t part_num_data[16]; /**< Part number data. */

    /**
     * @brief Provisioning Data version (160 bits), defined by Tropic Square for each batch in BP.
     */
    uint8_t prov_templ_ver[2]; /**< Provisioning template version. */
    uint8_t prov_templ_tag[4]; /**< Provisioning template tag. */
    uint8_t prov_spec_ver[2];  /**< Provisioning specification version. */
    uint8_t prov_spec_tag[4];  /**< Provisioning specification tag. */

    /**
     * @brief Batch ID (40 bits).
     */
    uint8_t batch_id[5];

    /**
     * @brief Reserved field 3 (24 bits).
     */
    uint8_t rfu_3[3];

    /**
     * @brief Padding (192 bits).
     */
    uint8_t rfu_4[24];
} __attribute__((packed)) lt_chip_id_t;

// clang-format off
/** \cond */
STATIC_ASSERT(
    ( sizeof(struct lt_chip_id_t) )
    ==
    (
        MEMBER_SIZE(struct lt_chip_id_t, chip_id_ver) + 
        MEMBER_SIZE(struct lt_chip_id_t, fl_chip_info) + 
        MEMBER_SIZE(struct lt_chip_id_t, func_test_info) + 
        MEMBER_SIZE(struct lt_chip_id_t, silicon_rev) + 
        MEMBER_SIZE(struct lt_chip_id_t, packg_type_id) + 
        MEMBER_SIZE(struct lt_chip_id_t, rfu_1) + 
        MEMBER_SIZE(struct lt_chip_id_t, prov_ver_fab_id_pn) + 
        MEMBER_SIZE(struct lt_chip_id_t, provisioning_date) + 
        MEMBER_SIZE(struct lt_chip_id_t, hsm_ver) + 
        MEMBER_SIZE(struct lt_chip_id_t, prog_ver) + 
        MEMBER_SIZE(struct lt_chip_id_t, rfu_2) + 
        MEMBER_SIZE(struct lt_chip_id_t, ser_num) + 
        MEMBER_SIZE(struct lt_chip_id_t, part_num_data) + 
        MEMBER_SIZE(struct lt_chip_id_t, prov_templ_ver) + 
        MEMBER_SIZE(struct lt_chip_id_t, prov_templ_tag) + 
        MEMBER_SIZE(struct lt_chip_id_t, prov_spec_ver) + 
        MEMBER_SIZE(struct lt_chip_id_t, prov_spec_tag) + 
        MEMBER_SIZE(struct lt_chip_id_t, batch_id) + 
        MEMBER_SIZE(struct lt_chip_id_t, rfu_3) + 
        MEMBER_SIZE(struct lt_chip_id_t, rfu_4)
    )
)
/** \endcond */
// clang-format on

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal size of returned RISCV fw version */
#define LT_L2_GET_INFO_RISCV_FW_SIZE 4

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal size of returned SPECT fw version */
#define LT_L2_GET_INFO_SPECT_FW_SIZE 4

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal size of returned fw header */
#define LT_L2_GET_INFO_FW_HEADER_SIZE_BOOT_V1 20
#define LT_L2_GET_INFO_FW_HEADER_SIZE_BOOT_V2 52
#define LT_L2_GET_INFO_FW_HEADER_SIZE_BOOT_V2_EMPTY_BANK 0
/** @brief Maximal size of returned fw header */
#define LT_L2_GET_INFO_FW_HEADER_SIZE LT_L2_GET_INFO_FW_HEADER_SIZE_BOOT_V2

/** @brief BANK ID */
typedef enum lt_bank_id_t {
    FW_BANK_FW1 = 1,      // Firmware bank 1.
    FW_BANK_FW2 = 2,      // Firmware bank 2
    FW_BANK_SPECT1 = 17,  // SPECT bank 1.
    FW_BANK_SPECT2 = 18,  // SPECT bank 2
} lt_bank_id_t;

/**
 * @brief When in MAINTENANCE mode, it is possible to read firmware header from a firmware bank. Returned data differs
 * based on bootloader version. This header layout is returned by bootloader version v1.0.1
 */
typedef struct header_boot_v1_t {
    uint8_t type[4];
    uint8_t version[4];
    uint8_t size[4];
    uint8_t git_hash[4];
    uint8_t hash[4];
} __attribute__((packed)) header_boot_v1_t;

// clang-format off
/** \cond */
STATIC_ASSERT(
    ( sizeof(struct header_boot_v1_t) )
    ==
    (
        MEMBER_SIZE(struct header_boot_v1_t, type) + 
        MEMBER_SIZE(struct header_boot_v1_t, version) + 
        MEMBER_SIZE(struct header_boot_v1_t, size) + 
        MEMBER_SIZE(struct header_boot_v1_t, git_hash) + 
        MEMBER_SIZE(struct header_boot_v1_t, hash)
    )
)
/** \endcond */
// clang-format on

/**
 * @brief When in MAINTENANCE mode, it is possible to read firmware header from a firmware bank. Returned data differs
 * based on bootloader version. This header layout is returned by bootloader version v1.0.1
 */
typedef struct header_boot_v2_t {
    /** @brief Currently only two types supported:
     * 1 == FW for RISCV coprocessor.
     * 2 == FW for SPECT coprocessor */
    uint16_t type;
    uint8_t padding;
    /** @brief This header version. */
    uint8_t header_version;
    /** @brief FW version, the same number as *TS_L2_GET_INFO_REQ_OBJECT_ID_RISCV_FW_VERSION* or
     * *TS_L2_GET_INFO_REQ_OBJECT_ID_SPECT_ROM_ID*. */
    uint32_t ver;
    /** @brief FW size in bytes (always aligned to uint32_t). */
    uint32_t size;
    /** @brief GIT hash of the underlying FW repository. */
    uint32_t git_hash;
    /** @brief Hash for data integrity (SHA256, 32B). */
    uint8_t hash[32];
    /** @brief Other FW version compatibility. In case RISCV FW there may be SPECT version to match. Zero means any
     * version.*/
    uint32_t pair_version;

} __attribute__((packed)) header_boot_v2_t;

// clang-format off
/** \cond */
STATIC_ASSERT(
    ( sizeof(struct header_boot_v2_t) )
    ==
    (
        MEMBER_SIZE(struct header_boot_v2_t, type) + 
        MEMBER_SIZE(struct header_boot_v2_t, padding) + 
        MEMBER_SIZE(struct header_boot_v2_t, header_version) + 
        MEMBER_SIZE(struct header_boot_v2_t, ver) + 
        MEMBER_SIZE(struct header_boot_v2_t, size) +
        MEMBER_SIZE(struct header_boot_v2_t, git_hash) +
        MEMBER_SIZE(struct header_boot_v2_t, hash) +
        MEMBER_SIZE(struct header_boot_v2_t, pair_version)
    )
)
/** \endcond */
// clang-format on

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Pairing key indexes corresponds to S_HiPub */
typedef enum pkey_index_t {
    PAIRING_KEY_SLOT_INDEX_0,
    PAIRING_KEY_SLOT_INDEX_1,
    PAIRING_KEY_SLOT_INDEX_2,
    PAIRING_KEY_SLOT_INDEX_3,
} pkey_index_t;

/** @brief Structure used to store variables used during establishment of a secure session */
typedef struct session_state_t {
    uint8_t ehpriv[32];
    uint8_t ehpub[32];
    // pkey_index_t pkey_index;
} session_state_t;

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Basic sleep mode */
#define LT_L2_SLEEP_KIND_SLEEP 0x05
/** @brief Deep sleep mode */
#define LT_L2_SLEEP_KIND_DEEP_SLEEP 0x0a

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Reboot TROPIC01 chip */
#define LT_MODE_APP 0x01
/** @brief Reboot TROPIC01 chip and stay in maintenance mode */
#define LT_MODE_MAINTENANCE 0x03

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal length of TROPIC01's log message */
#define GET_LOG_MAX_MSG_LEN 255

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal length of Ping command message */
#define PING_LEN_MAX 4096

//--------------------------------------------------------------------------------------------------------------------//
/** @brief ECC key slot indexes */
typedef enum pairing_key_slot_t {
    SH0PUB = 0,
    SH1PUB,
    SH2PUB,
    SH3PUB,
} pairing_key_slot_t;

//--------------------------------------------------------------------------------------------------------------------//
/** @brief CONFIGURATION_OBJECTS_REGISTERS memory map */
typedef enum CONFIGURATION_OBJECTS_REGS {
    CONFIGURATION_OBJECTS_CFG_START_UP_ADDR = BOOTLOADER_CO_CFG_START_UP_ADDR,
    CONFIGURATION_OBJECTS_CFG_SENSORS_ADDR = BOOTLOADER_CO_CFG_SENSORS_ADDR,
    CONFIGURATION_OBJECTS_CFG_DEBUG_ADDR = BOOTLOADER_CO_CFG_DEBUG_ADDR,
    CONFIGURATION_OBJECTS_CFG_GPO_ADDR = APPLICATION_CO_CFG_GPO_ADDR,
    CONFIGURATION_OBJECTS_CFG_SLEEP_MODE_ADDR = APPLICATION_CO_CFG_SLEEP_MODE_ADDR,
    CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_WRITE_ADDR = APPLICATION_CO_CFG_UAP_PAIRING_KEY_WRITE_ADDR,
    CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_READ_ADDR = APPLICATION_CO_CFG_UAP_PAIRING_KEY_READ_ADDR,
    CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_INVALIDATE_ADDR = APPLICATION_CO_CFG_UAP_PAIRING_KEY_INVALIDATE_ADDR,
    CONFIGURATION_OBJECTS_CFG_UAP_R_CONFIG_WRITE_ERASE_ADDR = APPLICATION_CO_CFG_UAP_R_CONFIG_WRITE_ERASE_ADDR,
    CONFIGURATION_OBJECTS_CFG_UAP_R_CONFIG_READ_ADDR = APPLICATION_CO_CFG_UAP_R_CONFIG_READ_ADDR,
    CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_WRITE_ADDR = APPLICATION_CO_CFG_UAP_I_CONFIG_WRITE_ADDR,
    CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_READ_ADDR = APPLICATION_CO_CFG_UAP_I_CONFIG_READ_ADDR,
    CONFIGURATION_OBJECTS_CFG_UAP_PING_ADDR = APPLICATION_CO_CFG_UAP_PING_ADDR,
    CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_WRITE_ADDR = APPLICATION_CO_CFG_UAP_R_MEM_DATA_WRITE_ADDR,
    CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_READ_ADDR = APPLICATION_CO_CFG_UAP_R_MEM_DATA_READ_ADDR,
    CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_ERASE_ADDR = APPLICATION_CO_CFG_UAP_R_MEM_DATA_ERASE_ADDR,
    CONFIGURATION_OBJECTS_CFG_UAP_RANDOM_VALUE_GET_ADDR = APPLICATION_CO_CFG_UAP_RANDOM_VALUE_GET_ADDR,
    CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_GENERATE_ADDR = APPLICATION_CO_CFG_UAP_ECC_KEY_GENERATE_ADDR,
    CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_STORE_ADDR = APPLICATION_CO_CFG_UAP_ECC_KEY_STORE_ADDR,
    CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_READ_ADDR = APPLICATION_CO_CFG_UAP_ECC_KEY_READ_ADDR,
    CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_ERASE_ADDR = APPLICATION_CO_CFG_UAP_ECC_KEY_ERASE_ADDR,
    CONFIGURATION_OBJECTS_CFG_UAP_ECDSA_SIGN_ADDR = APPLICATION_CO_CFG_UAP_ECDSA_SIGN_ADDR,
    CONFIGURATION_OBJECTS_CFG_UAP_EDDSA_SIGN_ADDR = APPLICATION_CO_CFG_UAP_EDDSA_SIGN_ADDR,
    CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_INIT_ADDR = APPLICATION_CO_CFG_UAP_MCOUNTER_INIT_ADDR,
    CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_GET_ADDR = APPLICATION_CO_CFG_UAP_MCOUNTER_GET_ADDR,
    CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_UPDATE_ADDR = APPLICATION_CO_CFG_UAP_MCOUNTER_UPDATE_ADDR,
    CONFIGURATION_OBJECTS_CFG_UAP_MAC_AND_DESTROY_ADDR = APPLICATION_CO_CFG_UAP_MAC_AND_DESTROY_ADDR
} CONFIGURATION_OBJECTS_REGS;

//--------------------------------------------------------------------------------------------------------------------//
/**
 * @brief CONFIGURATION_OBJECTS_REGISTERS index map to lt_config_t.
 * @warning Must reflect the order in cfg_desc_table.
 */
typedef enum CONFIGURATION_OBJECTS_REGS_IDX {
    CONFIGURATION_OBJECTS_CFG_START_UP_IDX = 0,
    CONFIGURATION_OBJECTS_CFG_SENSORS_IDX,
    CONFIGURATION_OBJECTS_CFG_DEBUG_IDX,
    CONFIGURATION_OBJECTS_CFG_GPO_IDX,
    CONFIGURATION_OBJECTS_CFG_SLEEP_MODE_IDX,
    CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_WRITE_IDX,
    CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_READ_IDX,
    CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_INVALIDATE_IDX,
    CONFIGURATION_OBJECTS_CFG_UAP_R_CONFIG_WRITE_ERASE_IDX,
    CONFIGURATION_OBJECTS_CFG_UAP_R_CONFIG_READ_IDX,
    CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_WRITE_IDX,
    CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_READ_IDX,
    CONFIGURATION_OBJECTS_CFG_UAP_PING_IDX,
    CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_WRITE_IDX,
    CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_READ_IDX,
    CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_ERASE_IDX,
    CONFIGURATION_OBJECTS_CFG_UAP_RANDOM_VALUE_GET_IDX,
    CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_GENERATE_IDX,
    CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_STORE_IDX,
    CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_READ_IDX,
    CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_ERASE_IDX,
    CONFIGURATION_OBJECTS_CFG_UAP_ECDSA_SIGN_IDX,
    CONFIGURATION_OBJECTS_CFG_UAP_EDDSA_SIGN_IDX,
    CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_INIT_IDX,
    CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_GET_IDX,
    CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_UPDATE_IDX,
    CONFIGURATION_OBJECTS_CFG_UAP_MAC_AND_DESTROY_IDX,
} CONFIGURATION_OBJECTS_REGS_IDX;

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Minimal size of one data slot in bytes */
#define R_MEM_DATA_SIZE_MIN (1)
/** @brief Maximal size of one data slot in bytes */
#define R_MEM_DATA_SIZE_MAX (444)
/** @brief Index of last data slot. TROPIC01 contains 512 slots indexed 0-511. */
#define R_MEM_DATA_SLOT_MAX (511)

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximum number of random bytes requested at once */
#define RANDOM_VALUE_GET_LEN_MAX 255

//--------------------------------------------------------------------------------------------------------------------//
/** @brief ECC key slot indexes */
typedef enum ecc_slot_t {
    ECC_SLOT_0 = 0,
    ECC_SLOT_1,
    ECC_SLOT_2,
    ECC_SLOT_3,
    ECC_SLOT_4,
    ECC_SLOT_5,
    ECC_SLOT_6,
    ECC_SLOT_7,
    ECC_SLOT_8,
    ECC_SLOT_9,
    ECC_SLOT_10,
    ECC_SLOT_11,
    ECC_SLOT_12,
    ECC_SLOT_13,
    ECC_SLOT_14,
    ECC_SLOT_15,
    ECC_SLOT_16,
    ECC_SLOT_17,
    ECC_SLOT_18,
    ECC_SLOT_19,
    ECC_SLOT_20,
    ECC_SLOT_21,
    ECC_SLOT_22,
    ECC_SLOT_23,
    ECC_SLOT_24,
    ECC_SLOT_25,
    ECC_SLOT_26,
    ECC_SLOT_27,
    ECC_SLOT_28,
    ECC_SLOT_29,
    ECC_SLOT_30,
    ECC_SLOT_31,
} ecc_slot_t;

/** @brief ECC key type */
typedef enum lt_ecc_curve_type_t { CURVE_P256 = 1, CURVE_ED25519 } lt_ecc_curve_type_t;

/** @brief ECC key origin */
typedef enum lt_ecc_key_origin_t { CURVE_GENERATED = 1, CURVE_STORED } lt_ecc_key_origin_t;

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal allowed value of the monotonic counter. */
#define MCOUNTER_VALUE_MAX 0xFFFFFFFE

/** @brief Use to choose one from 16 counters */
typedef enum lt_mcounter_index_t {
    MCOUNTER_INDEX_0 = 0,
    MCOUNTER_INDEX_1 = 1,
    MCOUNTER_INDEX_2 = 2,
    MCOUNTER_INDEX_3 = 3,
    MCOUNTER_INDEX_4 = 4,
    MCOUNTER_INDEX_5 = 5,
    MCOUNTER_INDEX_6 = 6,
    MCOUNTER_INDEX_7 = 7,
    MCOUNTER_INDEX_8 = 8,
    MCOUNTER_INDEX_9 = 9,
    MCOUNTER_INDEX_10 = 10,
    MCOUNTER_INDEX_11 = 11,
    MCOUNTER_INDEX_12 = 12,
    MCOUNTER_INDEX_13 = 13,
    MCOUNTER_INDEX_14 = 14,
    MCOUNTER_INDEX_15 = 15
} lt_mcounter_index_t;

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal size of returned MAC-and-Destroy data */
#define MAC_AND_DESTROY_DATA_SIZE 32u
/** @brief Maximal number of Mac And Destroy tries possible with TROPIC01 */
#define MACANDD_ROUNDS_MAX 128

/** @brief Mac-and-Destroy slot indexes */
typedef enum mac_and_destroy_slot_t {
    MAC_AND_DESTROY_SLOT_0 = 0,
    MAC_AND_DESTROY_SLOT_1,
    MAC_AND_DESTROY_SLOT_2,
    MAC_AND_DESTROY_SLOT_3,
    MAC_AND_DESTROY_SLOT_4,
    MAC_AND_DESTROY_SLOT_5,
    MAC_AND_DESTROY_SLOT_6,
    MAC_AND_DESTROY_SLOT_7,
    MAC_AND_DESTROY_SLOT_8,
    MAC_AND_DESTROY_SLOT_9,
    MAC_AND_DESTROY_SLOT_10,
    MAC_AND_DESTROY_SLOT_11,
    MAC_AND_DESTROY_SLOT_12,
    MAC_AND_DESTROY_SLOT_13,
    MAC_AND_DESTROY_SLOT_14,
    MAC_AND_DESTROY_SLOT_15,
    MAC_AND_DESTROY_SLOT_16,
    MAC_AND_DESTROY_SLOT_17,
    MAC_AND_DESTROY_SLOT_18,
    MAC_AND_DESTROY_SLOT_19,
    MAC_AND_DESTROY_SLOT_20,
    MAC_AND_DESTROY_SLOT_21,
    MAC_AND_DESTROY_SLOT_22,
    MAC_AND_DESTROY_SLOT_23,
    MAC_AND_DESTROY_SLOT_24,
    MAC_AND_DESTROY_SLOT_25,
    MAC_AND_DESTROY_SLOT_26,
    MAC_AND_DESTROY_SLOT_27,
    MAC_AND_DESTROY_SLOT_28,
    MAC_AND_DESTROY_SLOT_29,
    MAC_AND_DESTROY_SLOT_30,
    MAC_AND_DESTROY_SLOT_31,
    MAC_AND_DESTROY_SLOT_32,
    MAC_AND_DESTROY_SLOT_33,
    MAC_AND_DESTROY_SLOT_34,
    MAC_AND_DESTROY_SLOT_35,
    MAC_AND_DESTROY_SLOT_36,
    MAC_AND_DESTROY_SLOT_37,
    MAC_AND_DESTROY_SLOT_38,
    MAC_AND_DESTROY_SLOT_39,
    MAC_AND_DESTROY_SLOT_40,
    MAC_AND_DESTROY_SLOT_41,
    MAC_AND_DESTROY_SLOT_42,
    MAC_AND_DESTROY_SLOT_43,
    MAC_AND_DESTROY_SLOT_44,
    MAC_AND_DESTROY_SLOT_45,
    MAC_AND_DESTROY_SLOT_46,
    MAC_AND_DESTROY_SLOT_47,
    MAC_AND_DESTROY_SLOT_48,
    MAC_AND_DESTROY_SLOT_49,
    MAC_AND_DESTROY_SLOT_50,
    MAC_AND_DESTROY_SLOT_51,
    MAC_AND_DESTROY_SLOT_52,
    MAC_AND_DESTROY_SLOT_53,
    MAC_AND_DESTROY_SLOT_54,
    MAC_AND_DESTROY_SLOT_55,
    MAC_AND_DESTROY_SLOT_56,
    MAC_AND_DESTROY_SLOT_57,
    MAC_AND_DESTROY_SLOT_58,
    MAC_AND_DESTROY_SLOT_59,
    MAC_AND_DESTROY_SLOT_60,
    MAC_AND_DESTROY_SLOT_61,
    MAC_AND_DESTROY_SLOT_62,
    MAC_AND_DESTROY_SLOT_63,
    MAC_AND_DESTROY_SLOT_64,
    MAC_AND_DESTROY_SLOT_65,
    MAC_AND_DESTROY_SLOT_66,
    MAC_AND_DESTROY_SLOT_67,
    MAC_AND_DESTROY_SLOT_68,
    MAC_AND_DESTROY_SLOT_69,
    MAC_AND_DESTROY_SLOT_70,
    MAC_AND_DESTROY_SLOT_71,
    MAC_AND_DESTROY_SLOT_72,
    MAC_AND_DESTROY_SLOT_73,
    MAC_AND_DESTROY_SLOT_74,
    MAC_AND_DESTROY_SLOT_75,
    MAC_AND_DESTROY_SLOT_76,
    MAC_AND_DESTROY_SLOT_77,
    MAC_AND_DESTROY_SLOT_78,
    MAC_AND_DESTROY_SLOT_79,
    MAC_AND_DESTROY_SLOT_80,
    MAC_AND_DESTROY_SLOT_81,
    MAC_AND_DESTROY_SLOT_82,
    MAC_AND_DESTROY_SLOT_83,
    MAC_AND_DESTROY_SLOT_84,
    MAC_AND_DESTROY_SLOT_85,
    MAC_AND_DESTROY_SLOT_86,
    MAC_AND_DESTROY_SLOT_87,
    MAC_AND_DESTROY_SLOT_88,
    MAC_AND_DESTROY_SLOT_89,
    MAC_AND_DESTROY_SLOT_90,
    MAC_AND_DESTROY_SLOT_91,
    MAC_AND_DESTROY_SLOT_92,
    MAC_AND_DESTROY_SLOT_93,
    MAC_AND_DESTROY_SLOT_94,
    MAC_AND_DESTROY_SLOT_95,
    MAC_AND_DESTROY_SLOT_96,
    MAC_AND_DESTROY_SLOT_97,
    MAC_AND_DESTROY_SLOT_98,
    MAC_AND_DESTROY_SLOT_99,
    MAC_AND_DESTROY_SLOT_100,
    MAC_AND_DESTROY_SLOT_101,
    MAC_AND_DESTROY_SLOT_102,
    MAC_AND_DESTROY_SLOT_103,
    MAC_AND_DESTROY_SLOT_104,
    MAC_AND_DESTROY_SLOT_105,
    MAC_AND_DESTROY_SLOT_106,
    MAC_AND_DESTROY_SLOT_107,
    MAC_AND_DESTROY_SLOT_108,
    MAC_AND_DESTROY_SLOT_109,
    MAC_AND_DESTROY_SLOT_110,
    MAC_AND_DESTROY_SLOT_111,
    MAC_AND_DESTROY_SLOT_112,
    MAC_AND_DESTROY_SLOT_113,
    MAC_AND_DESTROY_SLOT_114,
    MAC_AND_DESTROY_SLOT_115,
    MAC_AND_DESTROY_SLOT_116,
    MAC_AND_DESTROY_SLOT_117,
    MAC_AND_DESTROY_SLOT_118,
    MAC_AND_DESTROY_SLOT_119,
    MAC_AND_DESTROY_SLOT_120,
    MAC_AND_DESTROY_SLOT_121,
    MAC_AND_DESTROY_SLOT_122,
    MAC_AND_DESTROY_SLOT_123,
    MAC_AND_DESTROY_SLOT_124,
    MAC_AND_DESTROY_SLOT_125,
    MAC_AND_DESTROY_SLOT_126,
    MAC_AND_DESTROY_SLOT_127
} mac_and_destroy_slot_t;

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal size of returned serial code */
#define SERIAL_CODE_SIZE 32u

//---------------------------------------------------------------------------------------------------------------------//
/** @brief Macro to control which session can access command targeting content of pairing key SH0 */
#define TO_PAIRING_KEY_SH0(x) ((x) << 0)
/** @brief Macro to control which session can access command targeting content of pairing key SH1 */
#define TO_PAIRING_KEY_SH1(x) ((x) << 8)
/** @brief Macro to control which session can access command targeting content of pairing key SH2 */
#define TO_PAIRING_KEY_SH2(x) ((x) << 16)
/** @brief Macro to control which session can access command targeting content of pairing key SH3 */
#define TO_PAIRING_KEY_SH3(x) ((x) << 24)

/** @brief Macro to control which session can access command targeting counter 0-3 */
#define TO_LT_MCOUNTER_0_3(x) ((x) << 0)
/** @brief Macro to control which session can access command targeting counter 4-7 */
#define TO_LT_MCOUNTER_4_7(x) ((x) << 8)
/** @brief Macro to control which session can access command targeting counter 8-11 */
#define TO_LT_MCOUNTER_8_11(x) ((x) << 16)
/** @brief Macro to control which session can access command targeting counter 12-15 */
#define TO_LT_MCOUNTER_12_15(x) ((x) << 24)

/** @brief Macro to control which session can access command targeting ECC_SLOT 0-7 */
#define TO_ECC_KEY_SLOT_0_7(x) ((x) << 0)
/** @brief Macro to control which session can access command targeting ECC_SLOT 8-15 */
#define TO_ECC_KEY_SLOT_8_15(x) ((x) << 8)
/** @brief Macro to control which session can access command targeting ECC_SLOT 16-23 */
#define TO_ECC_KEY_SLOT_16_23(x) ((x) << 16)
/** @brief Macro to control which session can access command targeting ECC_SLOT 24-31 */
#define TO_ECC_KEY_SLOT_24_31(x) ((x) << 24)

/** @brief Macro to control which session can access command targeting MACANDD slot 0-31 */
#define TO_MACANDD_SLOT_0_31(x) ((x) << APPLICATION_CO_CFG_UAP_MAC_AND_DESTROY_MACANDD_0_31_POS)
/** @brief Macro to control which session can access command targeting MACANDD slot 32-63 */
#define TO_MACANDD_SLOT_32_63(x) ((x) << APPLICATION_CO_CFG_UAP_MAC_AND_DESTROY_MACANDD_32_63_POS)
/** @brief Macro to control which session can access command targeting MACANDD slot 64-95 */
#define TO_MACANDD_SLOT_64_95(x) ((x) << APPLICATION_CO_CFG_UAP_MAC_AND_DESTROY_MACANDD_64_95_POS)
/** @brief Macro to control which session can access command targeting MACANDD slot 96-127 */
#define TO_MACANDD_SLOT_96_127(x) ((x) << APPLICATION_CO_CFG_UAP_MAC_AND_DESTROY_MACANDD_96_127_POS)

/** @brief Macro to control if session SH0 has access to a specific command */
#define SESSION_SH0_HAS_ACCESS (uint8_t)0x01
/** @brief Macro to control if session SH1 has access to a specific command */
#define SESSION_SH1_HAS_ACCESS (uint8_t)0x02
/** @brief Macro to control if session SH2 has access to a specific command */
#define SESSION_SH2_HAS_ACCESS (uint8_t)0x04
/** @brief Macro to control if session SH3 has access to a specific command */
#define SESSION_SH3_HAS_ACCESS (uint8_t)0x08

/** @brief This structure is used in this example to simplify looping
 *         through all config addresses and printing out them into debug */
typedef struct lt_config_obj_desc_t {
    char desc[60];
    enum CONFIGURATION_OBJECTS_REGS addr;
} lt_config_obj_desc_t;

/** @brief Number of configuration objects in lt_config_t */
#define LT_CONFIG_OBJ_CNT 27

/** @brief Structure to hold all configuration objects */
typedef struct lt_config_t {
    uint32_t obj[LT_CONFIG_OBJ_CNT];
} lt_config_t;

#endif
