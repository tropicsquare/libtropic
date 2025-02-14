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

/** Macro to sanitize compiler warnings */
#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

/** This particular value means that secure session was succesfully established and it is currently ON */
#define SESSION_ON   0xA5A55A5A

/** Size of l3 ID field */
#define L3_ID_SIZE 1u
/** Size of l3 TAG field */
#define L3_TAG_SIZE 16u
/** Size of IV */
#define L3_IV_SIZE 12u

/** Size of RES_SIZE field */
#define L3_RES_SIZE_SIZE sizeof(uint16_t)
/** Size of CMD_SIZE field */
#define L3_CMD_SIZE_SIZE sizeof(uint16_t)
/** Size of l3 CMD_ID field */
#define L3_CMD_ID_SIZE (1)
/** Maximal size of l3 RES/RSP DATA field */
#define L3_CMD_DATA_SIZE_MAX (4097)

/** TODO Maximal size of data field in one L2 transfer */
#define L2_CHUNK_MAX_DATA_SIZE 252u
/** Maximal size of one l2 frame */
#define L2_MAX_FRAME_SIZE (1 + 1 + L2_CHUNK_MAX_DATA_SIZE + 2)
/** Maximal number of data bytes in one L1 transfer */
#define LT_L1_LEN_MIN 1
/** Maximal number of data bytes in one L1 transfer */
#define LT_L1_LEN_MAX (1 + 1 + 1 + L2_CHUNK_MAX_DATA_SIZE + 2)

/** Maximum size of l3 ciphertext (or decrypted l3 packet) */
#define L3_PACKET_MAX_SIZE         (L3_CMD_ID_SIZE + L3_CMD_DATA_SIZE_MAX)
/** Max size of one unit of transport on l3 layer */
#define L3_FRAME_MAX_SIZE         (L3_RES_SIZE_SIZE + L3_PACKET_MAX_SIZE + L3_TAG_SIZE)

/** Generic L3 command and result frame */
struct __attribute__((packed)) lt_l3_gen_frame_t {
    /** RES_SIZE or CMD_SIZE value */
    uint16_t cmd_size;
    /** Command or result data including ID and TAG */
    uint8_t data[L3_FRAME_MAX_SIZE - L3_RES_SIZE_SIZE];
};

/**
 * @details This structure holds data related to one physical chip.
 * Contains AESGCM contexts for encrypting and decrypting L3 commands, nonce and device void pointer, which can be used for passing arbitrary data.
 */
typedef struct lt_handle_t {
    void *device;
    uint32_t session;
    uint8_t mode;
    uint8_t IV[12];
#if LT_USE_TREZOR_CRYPTO
    uint8_t encrypt[352] __attribute__ ((aligned (16))); // Because sizeof(lt_aes_gcm_ctx_t) == 352;
    uint8_t decrypt[352] __attribute__ ((aligned (16)));
#elif USE_MBEDTLS
#warning "Warning: MBED Tls is not implemented yet";
#else // TODO figure out how to allocate correct space without a need of passing LT_USE_TREZOR_CRYPTO from platform's cmake
    uint8_t encrypt[352] __attribute__ ((aligned (16)));
    uint8_t decrypt[352] __attribute__ ((aligned (16)));
#endif
    uint8_t l2_buff [1 + L2_MAX_FRAME_SIZE];
    uint8_t l3_buff[L3_FRAME_MAX_SIZE];
} lt_handle_t;

/** Enum return type */
typedef enum {
    /** Operation was successful */
    LT_OK,
    /** Operation was not succesfull */
    LT_FAIL,
    /* Host no session */
    LT_HOST_NO_SESSION,
    /** Some parameter was not accepted by function */
    LT_PARAM_ERR,
    /** Error detected during cryptographic operation */
    LT_CRYPTO_ERR,

    /** Spi transfer returned error */
    LT_L1_SPI_ERROR,
    /** Data does not have an expected length */
    LT_L1_DATA_LEN_ERROR,
    /** Chip is in STARTUP mode */
    LT_L1_CHIP_STARTUP_MODE,
    /** Chip is in ALARM mode */
    LT_L1_CHIP_ALARM_MODE,
    /** Chip is BUSY - typically chip is still booting */
    LT_L1_CHIP_BUSY,

    /** Return values based on RESULT field */
    /** L3 result [API r_mem_data_write]: write failed, because slot is already written in */
    LT_L3_R_MEM_DATA_WRITE_WRITE_FAIL,
    /** L3 result [API r_mem_data_write]: writing operation limit is reached for a given slot */
    LT_L3_R_MEM_DATA_WRITE_SLOT_EXPIRED,
    /** L3 result [API EDDSA_sign, ECDSA_sign, ecc_key_read]: The key in the requested slot does not exist, or is invalid. */
    LT_L3_ECC_INVALID_KEY,
    /** L3 result [API mcounter_update]: Failure to update the speciﬁed Monotonic Counter. The Monotonic Counter is already at 0. */
    LT_L3_MCOUNTER_UPDATE_UPDATE_ERR,
    /** L3 result [API mcounter_update, mcounter_get]: The Monotonic Counter detects an attack and is locked. The counter must be reinitialized. */
    LT_L3_COUNTER_INVALID,
    /** L3 result [API pairing_key_read], The Pairing key slot is in "Blank" state. A Pairing Key has not been written to it yet */
    LT_L3_PAIRING_KEY_EMPTY,
    /** L3 result [API pairing_key_read], The Pairing key slot is in "Invalidated" state. The Pairing key has been invalidated */
    LT_L3_PAIRING_KEY_INVALID,
    /** L3 command was received correctly*/
    LT_L3_OK,
    /** L3 command was not received correctly */
    LT_L3_FAIL,
    /** Current pairing keys are not authorized for execution of the last command */
    LT_L3_UNAUTHORIZED,
    /** Received L3 command is invalid */
    LT_L3_INVALID_CMD,
    /** L3 data does not have an expected length */
    LT_L3_DATA_LEN_ERROR,

    /** Return values based on STATUS field */
    /** l2 response frame contains CRC error */
    LT_L2_IN_CRC_ERR,
    /** There is more than one chunk to be expected for a current request */
    LT_L2_REQ_CONT,
    /** There is more than one chunk to be received for a current response */
    LT_L2_RES_CONT,
    /** There were an error during handshake establishing */
    LT_L2_HSK_ERR,
    /** There is no secure session */
    LT_L2_NO_SESSION,
    /** There were error during checking message authenticity */
    LT_L2_TAG_ERR,
    /** l2 request contained crc error */
    LT_L2_CRC_ERR,
    /** There were some other error */
    LT_L2_GEN_ERR,
    /** Chip has no response to be transmitted */
    LT_L2_NO_RESP,
    /** ID of last request is not known to TROPIC01 */
    LT_L2_UNKNOWN_REQ,
    /** Returned status byte is not recognized at all */
    LT_L2_STATUS_NOT_RECOGNIZED,
    /** L2 data does not have an expected length */
    LT_L2_DATA_LEN_ERROR,

} lt_ret_t;

#define LT_TROPIC01_REBOOT_DELAY_MS  75

#endif
