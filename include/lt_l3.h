#ifndef LT_LIBTROPIC_SEPARATE_API_H
#define LT_LIBTROPIC_SEPARATE_API_H

/**
 * @defgroup libtropic_l3 libtropic l3 functions
 * @brief Expected to be used by an application when using separate calls for l3 data out and l3 data in
 * @details This module provides functions for separate l3 communication with TROPIC01. Function prepended with 'lt_out__' are
 * used to prepare data for sending to TROPIC01, while functions prepended with 'lt_in__' are used to decode data received from TROPIC01.
 * 
 * Sending and receiving data is done through l2 layer, which is not covered by this module and user is expected to call lt_l2_send()
 * at the point when data is ready to be sent to TROPIC01.
 * 
 * For more information have a look into libtropic.c, how separate calls are used in a single call.
 * @{
 */

/**
 * @file lt_l3.h
 * @brief libtropic library main API header file
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdbool.h>
#include <stddef.h>

#include "libtropic_common.h"

/**
 * @brief Initiates secure session. After succesfull execution, handle->l2_buff will contain data for L2 handshake request. 
 *
 *        For more information read info at the top of this file.
 *
 * @param h           Device's handle
 * @param pkey_index  Index of pairing public key
 * @param state       Structure holding session state, will be filled by this function. Content is used to finish secure session establishment in lt_in__session_start().
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__session_start(lt_handle_t *h, const pkey_index_t pkey_index, session_state_t *state);

/**
 * @brief Decodes TROPIC01's response during secure session's establishment. Designed to be used together with lt_out__session_start(), lt_l2_send() and lt_l2_receive()
 *
 *        Secure session will be established after succesfull execution. For more information read info at the top of this file.
 *
 * @param h           Device's handle
 * @param stpub       STPUB from device's certificate
 * @param pkey_index  Index of pairing public key
 * @param shipriv     Secure host private key
 * @param shipub      Secure host public key
 * @param state       Content must be filled with lt_out__session_start and is used to finish secure session establishment.
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__session_start(lt_handle_t *h, const uint8_t *stpub, const pkey_index_t pkey_index, const uint8_t *shipriv, const uint8_t *shipub, session_state_t *state);

/**
 * @brief Prepares ping command payload. Used for separate l3 communication, for more information read info at the top of this file.
 *
 * @param h           Device's handle
 * @param msg_out     Ping message going out
 * @param len         Length of sent message
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__ping(lt_handle_t *h, const uint8_t *msg_out, const uint16_t len);

/**
 * @brief Decodes ping command payload. Used for separate l3 communication, for more information read info at the top of this file.
 *
 * @param h           Device's handle
 * @param msg_in      Buffer to receive ping message
 * @param len         Expected length of received message, the same as sent in lt_out__ping()
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__ping(lt_handle_t *h, uint8_t *msg_in, const uint16_t len);

/**
 * @brief Prepares ECC key generation command payload. Used for separate l3 communication, for more information read info at the top of this file.
 *
 * @param h           Device's handle
 * @param slot        ECC key slot to generate key in
 * @param curve       ECC curve type to use for key generation
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__ecc_key_generate(lt_handle_t *h, const ecc_slot_t slot, const lt_ecc_curve_type_t curve);

/**
 * @brief Decodes ECC key generation command payload. Used for separate l3 communication, for more information read info at the top of this file.
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__ecc_key_generate(lt_handle_t *h);

/**
 * @brief Prepares ECC key store command payload. Used for separate l3 communication, for more information read info at the top of this file.
 *
 * @param h           Device's handle
 * @param slot        ECC key slot to store key in
 * @param curve       ECC curve type to use for key storage
 * @param key         Ecc key to store, 32B length
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__ecc_key_store(lt_handle_t *h, const ecc_slot_t slot, const lt_ecc_curve_type_t curve, const uint8_t *key);

/**
 * @brief Decodes ECC key store command payload. Used for separate l3 communication, for more information read info at the top of this file.
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__ecc_key_store(lt_handle_t *h);

/**
 * @brief Prepares ECC key read command payload. Used for separate l3 communication, for more information read info at the top of this file.
 *
 * @param h           Device's handle
 * @param slot        ECC key slot to read key from
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__ecc_key_read(lt_handle_t *h, const ecc_slot_t slot);

/**
 * @brief Decodes ECC key read command payload. Used for separate l3 communication, for more information read info at the top of this file.
 *
 * @param h           Device's handle
 * @param key         Buffer to receive ECC public key
 * @param keylen      Length of the key's buffer, must be at least 64 bytes
 * @param curve       Will be filled by curve type
 * @param origin      Will be filled by origin type
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__ecc_key_read(lt_handle_t *h, uint8_t *key, const uint8_t keylen, lt_ecc_curve_type_t *curve, ecc_key_origin_t *origin);

/**
 * @brief Prepares ECC key erase command payload. Used for separate l3 communication, for more information read info at the top of this file.
 *
 * @param h           Device's handle
 * @param slot        ECC key slot to erase key from
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__ecc_key_erase(lt_handle_t *h, const ecc_slot_t slot);

/**
 * @brief Decodes ECC key erase command payload. Used for separate l3 communication, for more information read info at the top of this file.
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__ecc_key_erase(lt_handle_t *h);

/** @} */ // end of group_libtropic_l3

#endif