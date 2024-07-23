#ifndef TS_HOST_H
#define TS_HOST_H

/**
* @file libtropic.h
* @brief libtropic header file
* @author Tropic Square s.r.o.
*/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ts_common.h"
#include "ts_l3.h"

/**
 * @brief Initialize handle and transport layer
 *
 * @param h           Device's handle
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_init(ts_handle_t *h);

/**
 * @brief Deinitialize handle and transport layer
 *
 * @param h           Device's handle
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_deinit(ts_handle_t *h);

/**
 * @brief This function provides secure handshake functionality. After succesfull execution, gcm contexts in passed handle will have kcmd and kres keys set. From this point, device will accept L3 commands.
 *
 * @param h           Device's handle
 * @param stpub       STPUB from device's certificate
 * @param pkey_index  Index of pairing public key
 * @param shipriv     Secure host private key
 * @param shipub      Secure host public key
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_handshake(ts_handle_t *h, const uint8_t pkey_index, const uint8_t *shipriv, const uint8_t *shipub);

/**
 * @brief             Test secure session by exchanging a message with chip
 *
 * @param h           Device's handle
 * @param msg_out     Ping message going out
 * @param msg_in      Ping message going in
 * @param len         Length of ping message
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_ping(ts_handle_t *h, const uint8_t *msg_out, uint8_t *msg_in, const uint16_t len);

/**
 * @brief             Get number of random bytes
 *
 * @param h           Device's handle
 * @param buff        Buffer
 * @param len         Number of random bytes
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_random_get(ts_handle_t *h, uint8_t *buff, const uint16_t len);

/**
 * @brief Generate ECC key in the chip's ECC key slot
 *
 * @param h           Device's handle
 * @param slot        Slot number ECC_SLOT_1 - ECC_SLOT_32
 * @param curve       ECC curve L3_ECC_KEY_GENERATE_CURVE_ED25519 or L3_ECC_KEY_GENERATE_CURVE_P256
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_ecc_key_generate(ts_handle_t *h, const uint8_t slot, const uint8_t curve);

/**
 * @brief
 *
 * @param h           Device's handle
 * @param slot        Slot number ECC_SLOT_1 - ECC_SLOT_32
 * @param key         Buffer for retrieving a key
 * @param keylen      Length of the key's buffer
 * @param curve       Will be filled by curve byte
 * @param origin      Will be filled by origin byte
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_ecc_key_read(ts_handle_t *h, const uint8_t slot, uint8_t *key, const int8_t keylen, uint8_t *curve, uint8_t *origin);

/**
 * @brief Tropic EDDSA signs with ECC key
 *
 * @param h           Device's handle
 * @param slot        Slot number ECC_SLOT_1 - ECC_SLOT_32
 * @param msg         Buffer containing a message to sign, max length is 4096B
 * @param msg_len     Length of a message
 * @param rs          Buffer for storing a signature in a form of R and S bytes
 * @param rs_len      Length of rs buffer should be 64B
 * @return            TS_OK if success, otherwise returns other error code.
 *
 */
ts_ret_t ts_eddsa_sign(ts_handle_t *h, const uint8_t slot, const uint8_t *msg, const int16_t msg_len, uint8_t *rs, const int8_t rs_len);

/**
 * @brief
 *
 * @param h           Device's handle
 * @param slot        Slot number ECC_SLOT_1 - ECC_SLOT_32
 * @param msg         Buffer containing hash of a message
 * @param msg_len     Length of hash's buffer should be 32B
 * @param rs          Buffer for storing a signature in a form of R and S bytes
 * @param rs_len      Length of rs buffer should be 64B
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_ecdsa_sign(ts_handle_t *h, const uint8_t slot, const uint8_t *msg_hash, const int16_t msg_hash_len, uint8_t *rs, const int8_t rs_len);

/**
 * @brief Erase ECC key from chip-s slot
 *
 * @param h           Device's handle
 * @param slot        Slot number ECC_SLOT_1 - ECC_SLOT_32
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_ecc_key_erase(ts_handle_t *h, const uint8_t slot);

/**
 * @brief Get device's certificate
 *
 * @param h           Device's handle
 * @param cert        Certificate's buffer
 * @param max_len     Length of certificate's buffer
 * @return            TS_OK if success, otherwise returns other error code.
 */
ts_ret_t ts_get_info_cert(ts_handle_t *h, uint8_t *cert, const uint16_t max_len);

// TODO
//ts_ret_t ts_get_info_chip_id(ts_handle_t *h, uint8_t chip_id, uint16_t max_len);
//ts_ret_t ts_get_info_riscv_fw_ver(ts_handle_t *h, uint8_t ver, uint16_t max_len);
//ts_ret_t ts_get_info_spect_fw_ver(ts_handle_t *h, uint8_t ver, uint16_t max_len);
//ts_ret_t ts_get_info_fw_bank(ts_handle_t *h, uint8_t fw_bank, uint16_t max_len);

#endif
