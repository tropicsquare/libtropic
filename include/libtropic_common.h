#ifndef LT_LIBTROPIC_COMMON_H
#define LT_LIBTROPIC_COMMON_H

/**
 * @file libtropic_common.h
 * @brief Shared definitions and functions commonly used by more libtropic's layers
 * @author Tropic Square s.r.o. *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "stdint.h"

/** Alias for unsigned 8 bit integer */
typedef uint8_t u8;
/** Alias for unsigned 16 bit integer */
typedef uint16_t u16;
/** Alias for unsigned 32 bit integer */
typedef uint32_t u32;

// This macro is used to change static functions into exported one, when compiling unit tests.
// It allows to unit test static functions.
#ifndef TEST
#define STATIC static
#else
#define STATIC
#endif

/** Macro to sanitize compiler warnings */
#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

/** @brief This particular value means that secure session was succesfully established and it is currently ON */
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
#define L3_RES_SIZE_SIZE sizeof(uint16_t)
/** @brief Size of CMD_SIZE field */
#define L3_CMD_SIZE_SIZE sizeof(uint16_t)
/** @brief Size of l3 CMD_ID field */
#define L3_CMD_ID_SIZE (1)
/** @brief Maximal size of l3 RES/RSP DATA field */
#define L3_CMD_DATA_SIZE_MAX (4097)

/** @brief TODO Maximal size of data field in one L2 transfer */
#define L2_CHUNK_MAX_DATA_SIZE 252u
/** @brief Maximal size of one l2 frame */
#define L2_MAX_FRAME_SIZE (1 + 1 + L2_CHUNK_MAX_DATA_SIZE + 2)
/** @brief Maximal number of data bytes in one L1 transfer */
#define LT_L1_LEN_MIN 1
/** @brief Maximal number of data bytes in one L1 transfer */
#define LT_L1_LEN_MAX (1 + 1 + 1 + L2_CHUNK_MAX_DATA_SIZE + 2)

/** @brief Maximum size of l3 ciphertext (or decrypted l3 packet) */
#define L3_PACKET_MAX_SIZE (L3_CMD_ID_SIZE + L3_CMD_DATA_SIZE_MAX)
/** @brief Max size of one unit of transport on l3 layer */
#define L3_FRAME_MAX_SIZE (L3_RES_SIZE_SIZE + L3_PACKET_MAX_SIZE + L3_TAG_SIZE)

/** @brief Generic L3 command and result frame */
struct __attribute__((packed)) lt_l3_gen_frame_t {
    /** @brief RES_SIZE or CMD_SIZE value */
    uint16_t cmd_size;
    /** @brief Command or result data including ID and TAG */
    uint8_t data[L3_FRAME_MAX_SIZE - L3_RES_SIZE_SIZE];
};

#define UART_DEV_MAX_LEN 32
typedef struct {
    char device[UART_DEV_MAX_LEN];  // = "/dev/ttyACM0";
    uint32_t baud_rate;             // = 115200;
} lt_uart_def_unix_t;

typedef struct {
    void *device;
    uint8_t mode;
    uint8_t buff[1 + L2_MAX_FRAME_SIZE];
} lt_l2_state_t;

// #define LT_SIZE_OF_L3_BUFF (1000)
#ifndef LT_SIZE_OF_L3_BUFF
#define LT_SIZE_OF_L3_BUFF L3_FRAME_MAX_SIZE
#endif

typedef struct {
    uint32_t session;
    uint8_t encryption_IV[12];
    uint8_t decryption_IV[12];
#if LT_USE_TREZOR_CRYPTO
    uint8_t encrypt[352] __attribute__((aligned(16)));  // Because sizeof(lt_aes_gcm_ctx_t) == 352;
    uint8_t decrypt[352] __attribute__((aligned(16)));
#elif USE_MBEDTLS
#warning "Warning: MBED Tls is not implemented yet";
#else  // TODO figure out how to allocate correct space without a need of passing LT_USE_TREZOR_CRYPTO from platform's
       // cmake
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

/** @brief Enum return type */
typedef enum {
    /** @brief Operation was successful */
    LT_OK = 0,
    /** @brief Operation was not succesfull */
    LT_FAIL = 1,
    /* Host no session */
    LT_HOST_NO_SESSION = 2,
    /** @brief Some parameter was not accepted by function */
    LT_PARAM_ERR = 3,
    /** @brief Error detected during cryptographic operation */
    LT_CRYPTO_ERR = 4,

    /** @brief Spi transfer returned error */
    LT_L1_SPI_ERROR = 5,
    /** @brief Data does not have an expected length */
    LT_L1_DATA_LEN_ERROR = 6,
    /** @brief Chip is in STARTUP mode */
    LT_L1_CHIP_STARTUP_MODE = 7,
    /** @brief Chip is in ALARM mode */
    LT_L1_CHIP_ALARM_MODE = 8,
    /** @brief Chip is BUSY - typically chip is still booting */
    LT_L1_CHIP_BUSY,
    /** @brief Interrupt pin did not fire as expected */
    LT_L1_INT_TIMEOUT,

    /** Return values based on RESULT field */
    /** @brief L3 result [API r_mem_data_write]: write failed, because slot is already written in */
    LT_L3_R_MEM_DATA_WRITE_WRITE_FAIL = 10,
    /** @brief L3 result [API r_mem_data_write]: writing operation limit is reached for a given slot */
    LT_L3_R_MEM_DATA_WRITE_SLOT_EXPIRED = 11,
    /** @brief L3 result [API EDDSA_sign, ECDSA_sign, ecc_key_read]: The key in the requested slot does not exist, or is
       invalid. */
    LT_L3_ECC_INVALID_KEY = 12,
    /** @brief L3 result [API mcounter_update]: Failure to update the speciﬁed Monotonic Counter. The Monotonic Counter
       is already at 0. */
    LT_L3_MCOUNTER_UPDATE_UPDATE_ERR = 13,
    /** @brief L3 result [API mcounter_update, mcounter_get]: The Monotonic Counter detects an attack and is locked. The
       counter must be reinitialized. */
    LT_L3_COUNTER_INVALID = 14,
    /** @brief L3 result [API pairing_key_read], The Pairing key slot is in "Blank" state. A Pairing Key has not been
       written to it yet */
    LT_L3_PAIRING_KEY_EMPTY = 15,
    /** @brief L3 result [API pairing_key_read], The Pairing key slot is in "Invalidated" state. The Pairing key has
       been invalidated */
    LT_L3_PAIRING_KEY_INVALID = 16,
    /** @brief L3 command was received correctly*/
    LT_L3_OK = 17,
    /** @brief L3 command was not received correctly */
    LT_L3_FAIL = 18,
    /** @brief Current pairing keys are not authorized for execution of the last command */
    LT_L3_UNAUTHORIZED = 19,
    /** @brief Received L3 command is invalid */
    LT_L3_INVALID_CMD = 20,
    /** @brief L3 data does not have an expected length */
    LT_L3_DATA_LEN_ERROR = 21,

    /** Return values based on STATUS field */
    /** @brief l2 response frame contains CRC error */
    LT_L2_IN_CRC_ERR = 22,
    /** @brief There is more than one chunk to be expected for a current request */
    LT_L2_REQ_CONT = 23,
    /** @brief There is more than one chunk to be received for a current response */
    LT_L2_RES_CONT = 24,
    /** @brief There were an error during handshake establishing */
    LT_L2_HSK_ERR = 25,
    /** @brief There is no secure session */
    LT_L2_NO_SESSION = 26,
    /** @brief There were error during checking message authenticity */
    LT_L2_TAG_ERR = 27,
    /** @brief l2 request contained crc error */
    LT_L2_CRC_ERR = 28,
    /** @brief There were some other error */
    LT_L2_GEN_ERR = 29,
    /** @brief Chip has no response to be transmitted */
    LT_L2_NO_RESP = 30,
    /** @brief ID of last request is not known to TROPIC01 */
    LT_L2_UNKNOWN_REQ = 31,
    /** @brief Returned status byte is not recognized at all */
    LT_L2_STATUS_NOT_RECOGNIZED = 32,
    /** @brief L2 data does not have an expected length */
    LT_L2_DATA_LEN_ERROR = 33,

    /** Certificate store related errors */
    /** Certificate store likely does not contain valid data */
    LT_CERT_STORE_INVALID = 34,
    /** Certificate store contains ASN1-DER syntax that is beyond the supported subset*/
    LT_CERT_UNSUPPORTED = 35,
    /** Certificate does not contain requested item */
    LT_CERT_ITEM_NOT_FOUND = 36,

    LT_L2_LAST_RET = 37
} lt_ret_t;

#define LT_TROPIC01_REBOOT_DELAY_MS 100

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal size of TROPIC01's certificate */
#define LT_L2_GET_INFO_REQ_CERT_SIZE_TOTAL 3840
#define LT_L2_GET_INFO_REQ_CERT_SIZE_SINGLE 700

typedef enum {
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
struct lt_cert_store_t {
    uint8_t *certs[LT_NUM_CERTIFICATES];    /** Certificates */
    uint16_t buf_len[LT_NUM_CERTIFICATES];  /** Length of buffers for certificates */
    uint16_t cert_len[LT_NUM_CERTIFICATES]; /** Lenght of certificates (from Cert store header) */
};

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal size of returned CHIP ID */
#define LT_L2_GET_INFO_CHIP_ID_SIZE 128

/**
 * @brief Structure used to parse content of CHIP_ID field
 *
 * @details This structure contains fields for parsing the chip's serial number data.
 */
struct lt_ser_num_t {
    uint8_t sn;          /**< 8 bits for serial number */
    uint8_t fab_data[3]; /**< 12 bits fab ID, 12 bits part number ID */
    uint16_t fab_date;   /**< 16 bits for fabrication date */
    uint8_t lot_id[5];   /**< 40 bits for lot ID */
    uint8_t wafer_id;    /**< 8 bits for wafer ID */
    uint16_t x_coord;    /**< 16 bits for x-coordinate */
    uint16_t y_coord;    /**< 16 bits for y-coordinate */
} __attribute__((__packed__));

/**
 * @brief Data in this struct comes from BP (batch package) yml file. CHIP_INFO is read into this struct.
 */
struct lt_chip_id_t {
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
} __attribute__((__packed__));

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal size of returned RISCV fw version */
#define LT_L2_GET_INFO_RISCV_FW_SIZE 4

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal size of returned SPECT fw version */
#define LT_L2_GET_INFO_SPECT_FW_SIZE 4

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal size of returned fw header */
#define LT_L2_GET_INFO_FW_HEADER_SIZE 20

/** @brief BANK ID */
typedef enum {
    FW_BANK_FW1 = 1,      // Firmware bank 1.
    FW_BANK_FW2 = 2,      // Firmware bank 2
    FW_BANK_SPECT1 = 17,  // SPECT bank 1.
    FW_BANK_SPECT2 = 18,  // SPECT bank 2
} bank_id_t;

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Pairing key indexes corresponds to S_HiPub */
typedef enum {
    PAIRING_KEY_SLOT_INDEX_0,
    PAIRING_KEY_SLOT_INDEX_1,
    PAIRING_KEY_SLOT_INDEX_2,
    PAIRING_KEY_SLOT_INDEX_3,
} pkey_index_t;

/** @brief Structure used to store variables used during establishment of a secure session */
typedef struct {
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
#define PING_LEN_MAX (L3_CMD_DATA_SIZE_MAX - L3_CMD_ID_SIZE)

//--------------------------------------------------------------------------------------------------------------------//
/** @brief ECC key slot indexes */
typedef enum {
    SH0PUB = 0,
    SH1PUB,
    SH2PUB,
    SH3PUB,
} pairing_key_slot_t;

//--------------------------------------------------------------------------------------------------------------------//
/** @brief CONFIGURATION_OBJECTS_REGISTERS memory map */
enum CONFIGURATION_OBJECTS_REGS {
    CONFIGURATION_OBJECTS_CFG_START_UP_ADDR = 0X0,
    CONFIGURATION_OBJECTS_CFG_SLEEP_MODE_ADDR = 0X4,
    CONFIGURATION_OBJECTS_CFG_SENSORS_ADDR = 0X8,
    CONFIGURATION_OBJECTS_CFG_DEBUG_ADDR = 0X10,
    CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_WRITE_ADDR = 0X20,
    CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_READ_ADDR = 0X24,
    CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_INVALIDATE_ADDR = 0X28,
    CONFIGURATION_OBJECTS_CFG_UAP_R_CONFIG_WRITE_ERASE_ADDR = 0X30,
    CONFIGURATION_OBJECTS_CFG_UAP_R_CONFIG_READ_ADDR = 0X34,
    CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_WRITE_ADDR = 0X40,
    CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_READ_ADDR = 0X44,
    CONFIGURATION_OBJECTS_CFG_UAP_PING_ADDR = 0X100,
    CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_WRITE_ADDR = 0X110,
    CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_READ_ADDR = 0X114,
    CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_ERASE_ADDR = 0X118,
    CONFIGURATION_OBJECTS_CFG_UAP_RANDOM_VALUE_GET_ADDR = 0X120,
    CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_GENERATE_ADDR = 0X130,
    CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_STORE_ADDR = 0X134,
    CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_READ_ADDR = 0X138,
    CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_ERASE_ADDR = 0X13C,
    CONFIGURATION_OBJECTS_CFG_UAP_ECDSA_SIGN_ADDR = 0X140,
    CONFIGURATION_OBJECTS_CFG_UAP_EDDSA_SIGN_ADDR = 0X144,
    CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_INIT_ADDR = 0X150,
    CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_GET_ADDR = 0X154,
    CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_UPDATE_ADDR = 0X158,
    CONFIGURATION_OBJECTS_CFG_UAP_MAC_AND_DESTROY_ADDR = 0X160,
    CONFIGURATION_OBJECTS_CFG_UAP_SERIAL_CODE_GET_ADDR = 0X170
};

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal size of one data slot in bytes */
#define R_MEM_DATA_SIZE_MAX (444)
/** @brief Index of last data slot. TROPIC01 contains 512 slots indexed 0-511, but the last one is used for M&D data.
 * Example code can be found in examples/lt_ex_macandd.c, for more info about Mac And Destroy functionality read app
 * note */
#define R_MEM_DATA_SLOT_MAX (510)
/** @brief Memory slot used for storing of M&D related data in Mac And Destroy example code. For more info see
 * examples/lt_ex_macandd.c */
#define R_MEM_DATA_SLOT_MACANDD (511)

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximum number of random bytes requested at once */
#define RANDOM_VALUE_GET_LEN_MAX 255

//--------------------------------------------------------------------------------------------------------------------//
/** @brief ECC key slot indexes */
typedef enum {
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
typedef enum { CURVE_P256 = 1, CURVE_ED25519 } lt_ecc_curve_type_t;

/** @brief ECC key origin */
typedef enum { CURVE_GENERATED = 1, CURVE_STORED } ecc_key_origin_t;

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Use to choose one from 16 counters */
enum lt_mcounter_index_t {
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
};

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal size of returned MAC-and-Destroy data */
#define MAC_AND_DESTROY_DATA_SIZE 32u
/** @brief Maximal number of Mac And Destroy tries possible with TROPIC01 */
#define MACANDD_ROUNDS_MAX 128

/** @brief Mac-and-Destroy slot indexes */
typedef enum {
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
/** @brief Macro to controll which session can access command targeting content of pairing key SH0 */
#define TO_PAIRING_KEY_SH0(x) ((x) << 0)
/** @brief Macro to controll which session can access command targeting content of pairing key SH1 */
#define TO_PAIRING_KEY_SH1(x) ((x) << 8)
/** @brief Macro to controll which session can access command targeting content of pairing key SH2 */
#define TO_PAIRING_KEY_SH2(x) ((x) << 16)
/** @brief Macro to controll which session can access command targeting content of pairing key SH3 */
#define TO_PAIRING_KEY_SH3(x) ((x) << 24)

/** @brief Macro to controll which session can access command targeting counter 0-3 */
#define TO_LT_MCOUNTER_0_3(x) ((x) << 0)
/** @brief Macro to controll which session can access command targeting counter 4-7 */
#define TO_LT_MCOUNTER_4_7(x) ((x) << 8)
/** @brief Macro to controll which session can access command targeting counter 8-11 */
#define TO_LT_MCOUNTER_8_11(x) ((x) << 16)
/** @brief Macro to controll which session can access command targeting counter 12-15 */
#define TO_LT_MCOUNTER_12_15(x) ((x) << 24)

/** @brief Macro to controll which session can access command targeting ECC_SLOT 0-7 */
#define TO_ECC_KEY_SLOT_0_7(x) ((x) << 0)
/** @brief Macro to controll which session can access command targeting ECC_SLOT 8-15 */
#define TO_ECC_KEY_SLOT_8_15(x) ((x) << 8)
/** @brief Macro to controll which session can access command targeting ECC_SLOT 16-23 */
#define TO_ECC_KEY_SLOT_16_23(x) ((x) << 16)
/** @brief Macro to controll which session can access command targeting ECC_SLOT 24-31 */
#define TO_ECC_KEY_SLOT_24_31(x) ((x) << 24)

/** @brief Macro to controll if session SH0 has access to a specific command */
#define SESSION_SH0_HAS_ACCESS (uint8_t)0x01
/** @brief Macro to controll if session SH1 has access to a specific command */
#define SESSION_SH1_HAS_ACCESS (uint8_t)0x02
/** @brief Macro to controll if session SH2 has access to a specific command */
#define SESSION_SH2_HAS_ACCESS (uint8_t)0x04
/** @brief Macro to controll if session SH3 has access to a specific command */
#define SESSION_SH3_HAS_ACCESS (uint8_t)0x08

/** @brief This structure is used in this example to simplify looping
 *         through all config addresses and printing out them into debug */
struct lt_config_obj_desc_t {
    char desc[60];
    enum CONFIGURATION_OBJECTS_REGS addr;
};

/** @brief Structure to hold all configuration objects */
struct lt_config_t {
    uint32_t obj[27];
};

#endif
