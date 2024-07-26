#ifndef TS_COMMON_H
#define TS_COMMON_H

/**
* @file ts_common.h
* @brief Shared definitions and functions commonly used by more layers
* @author Tropic Square s.r.o.
*/

#include "stdint.h"
#include "ts_aesgcm.h"

/** Alias for unsigned 8 bit integer */
typedef uint8_t u8;
/** Alias for unsigned 16 bit integer */
typedef uint16_t u16;

/** Macro to sanitize compiler warnings */
#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

/** This particular value means that secure session was succesfully established and it is currently ON */
#define SESSION_ON   0xAA55AA55
/** This particular value means that secure session is currently OFF */
#define SESSION_OFF  0x00000000

/** Size of l3 ID field */
#define L3_ID_SIZE 1u
/** Size of l3 TAG field */
#define L3_TAG_SIZE 16u
/** Size of IV */
#define L3_IV_SIZE 12u

/** Size of RES_SIZE field */
#define L3_RES_SIZE_SIZE sizeof(uint16_t)
/** Size of l3 CMD_ID field */
#define L3_CMD_ID_SIZE (1)
/** Maximal size of l3 RES/RSP DATA field */
#define L3_CMD_DATA_SIZE_MAX (4095)

/** Maximal size of data field in one L2 transfer */
#define L2_CHUNK_MAX_DATA_SIZE 252u
/** Maximal size of one l2 frame */
#define L2_MAX_FRAME_SIZE (1 + 1 + L2_CHUNK_MAX_DATA_SIZE + 2)
/** Maximal number of data bytes in one L1 transfer */
#define TS_L1_LEN_MIN 1
/** Maximal number of data bytes in one L1 transfer */
#define TS_L1_LEN_MAX (1 + 1 + 1 + L2_CHUNK_MAX_DATA_SIZE + 2)

/** Maximum size of l3 ciphertext (or decrypted l3 packet) */
#define L3_PACKET_MAX_SIZE         (L3_CMD_ID_SIZE + L3_CMD_DATA_SIZE_MAX)
/** Max size of one unit of transport on l3 layer */
#define L3_FRAME_MAX_SIZE         (L3_RES_SIZE_SIZE + L3_PACKET_MAX_SIZE + L3_TAG_SIZE)

/** Generic L3 command and result frame */
struct __attribute__((packed)) l3_frame_t{
    uint16_t packet_size;                   /** RES_SIZE or CMD_SIZE value */
    uint8_t data[L3_FRAME_MAX_SIZE - L3_RES_SIZE_SIZE]; /** Command or result data including ID and TAG */
};

/**
 * @details This structure holds data related to one physical chip.
 * Contains AESGCM contexts for encrypting and decrypting L3 commands, nonce and device void pointer, which can be used for passing arbitrary data.
 */
typedef struct ts_handle_t {
    void *device;
    uint32_t session;
    uint8_t IV[12];
    ts_aes_gcm_ctx_t encrypt;
    ts_aes_gcm_ctx_t decrypt;
    uint8_t l2_buff [1 + L2_MAX_FRAME_SIZE];
    uint8_t l3_buff[L3_FRAME_MAX_SIZE];
} ts_handle_t;

/** Enum return type */
typedef enum {
    /** Operation was successful */
    TS_OK,
    /** Operation was not succesfull */
    TS_FAIL,
    /** Some parameter was not accepted by function */
    TS_PARAM_ERR,

    /** Spi transfer returned error */
    TS_L1_SPI_ERROR,
    /** Data does not have an expected length */
    TS_L1_DATA_LEN_ERROR,
    /** Chip is in STARTUP mode */
    TS_L1_CHIP_STARTUP_MODE,
    /** Chip is in ALARM mode */
    TS_L1_CHIP_ALARM_MODE,
    /** Chip is BUSY - typically chip is still booting */
    TS_L1_CHIP_BUSY,

    /** Return values based on STATUS field */
    /** Indicates that there are more l2 frames to be received */
    TS_L2_IN_CRC_ERR,
    TS_L2_REQ_CONT,
    TS_L2_RES_CONT,
    TS_L2_HSK_ERR,
    TS_L2_NO_SESSION,
    TS_L2_TAG_ERR,
    TS_L2_CRC_ERR,
    TS_L2_GEN_ERR,
    TS_L2_NO_RESP,
    TS_L2_UNKNOWN_REQ,
    /** L2 data does not have an expected length */
    TS_L2_DATA_LEN_ERROR,

    /* L3 */
    TS_L3_OK,
    TS_L3_FAIL,
    TS_L3_UNAUTHORIZED,
    TS_L3_INVALID_CMD,

    /* Host */
    TS_HOST_NO_SESSION,
} ts_ret_t;

/**
 * @details Helper function for printing out error's associated name
 *
 * @param ret ts_ret_t return type
 * @return const char* name of error.
 */
const char *ts_ret_verbose(ts_ret_t ret);

#endif
