#ifndef LT_LIBTROPIC_SEPARATE_API_H
#define LT_LIBTROPIC_SEPARATE_API_H

/**
 * @defgroup libtropic_l3 libtropic l3 functions
 * @brief Expected to be used by an application when using separate calls for l3 data out and l3 data in
 * @details This module provides functions for separate l3 communication with TROPIC01. Function prepended with
 * 'lt_out__' are used to prepare data for sending to TROPIC01, while functions prepended with 'lt_in__' are used to
 * decode data received from TROPIC01.
 *
 * Sending and receiving data is done through l2 layer, which is not covered by this module and user is expected to call
 * lt_l2_send() at the point when data is ready to be sent to TROPIC01.
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
 * @brief Initiates secure session. After succesfull execution, handle->l2_buff will contain data for L2 handshake
 * request.
 *
 *        For more information read info at the top of this file.
 *
 * @param h           Device's handle
 * @param pkey_index  Index of pairing public key
 * @param state       Structure holding session state, will be filled by this function. Content is used to finish secure
 * session establishment in lt_in__session_start().
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__session_start(lt_handle_t *h, const pkey_index_t pkey_index, session_state_t *state);

/**
 * @brief Decodes 'TROPIC01's response during secure session's establishment. Designed to be used together with
 * lt_out__session_start(), lt_l2_send() and lt_l2_receive()
 *
 *        Secure session will be established after succesfull execution. For more information read info at the top of
 * this file.
 *
 * @param h           Device's handle
 * @param stpub       STPUB from device's certificate
 * @param pkey_index  Index of pairing public key
 * @param shipriv     Secure host private key
 * @param shipub      Secure host public key
 * @param state       Content must be filled with lt_out__session_start and is used to finish secure session
 * establishment.
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__session_start(lt_handle_t *h, const uint8_t *stpub, const pkey_index_t pkey_index,
                              const uint8_t *shipriv, const uint8_t *shipub, session_state_t *state);

/**
 * @brief Encodes 'ping' command payload. Used for separate l3 communication, for more information read info at the top
 * of this file.
 *
 * @param h           Device's handle
 * @param msg_out     Ping message going out
 * @param len         Length of sent message
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__ping(lt_handle_t *h, const uint8_t *msg_out, const uint16_t len);

/**
 * @brief Decodes 'ping' result payload. Used for separate l3 communication, for more information read info at the top
 * of this file.
 *
 * @param h           Device's handle
 * @param msg_in      Buffer to receive ping message
 * @param len         Expected length of received message, the same as sent in lt_out__ping()
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__ping(lt_handle_t *h, uint8_t *msg_in, const uint16_t len);

/**
 * @brief Encodes 'pairing key write' command payload. Used for separate l3 communication, for more information read
 * info at the top of this file.
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__pairing_key_write(lt_handle_t *h, const uint8_t *pairing_pub, const uint8_t slot);
/**
 * @brief Decodes 'pairing key write' result payload. Used for separate l3 communication, for more information read info
 * at the top of this file.
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__pairing_key_write(lt_handle_t *h);

/**
 * @brief Encodes 'pairing key read' command payload. Used for separate l3 communication, for more information read info
 * at the top of this file.
 *
 * @param h           Device's handle
 * @param slot        Slot to read pairing key from
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__pairing_key_read(lt_handle_t *h, const uint8_t slot);

/**
 * @brief Decodes 'pairing key read' result payload. Used for separate l3 communication, for more information read info
 * at the top of this file.
 *
 * @param h           Device's handle
 * @param pubkey      Buffer to receive pairing public key
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__pairing_key_read(lt_handle_t *h, uint8_t *pubkey);

/**
 * @brief Encodes 'pairing key invalidate' command payload. Used for separate l3 communication, for more information
 * read info at the top of this file.
 *
 * @param h           Device's handle
 * @param slot        Slot to invalidate pairing key in
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__pairing_key_invalidate(lt_handle_t *h, const uint8_t slot);

/**
 * @brief Decodes 'pairing key invalidate' result payload. Used for separate l3 communication, for more information read
 * info at the top of this file.
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__pairing_key_invalidate(lt_handle_t *h);

/**
 * @brief Encodes 'r config write' command payload. Used for separate l3 communication, for more information read info
 * at the top of this file.
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__r_config_write(lt_handle_t *h, enum CONFIGURATION_OBJECTS_REGS addr, const uint32_t obj);

/**
 * @brief Decodes 'r config write' result payload. Used for separate l3 communication, for more information read info at
 * the top of this file.
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__r_config_write(lt_handle_t *h);

/**
 * @brief Encodes 'r config read' command payload. Used for separate l3 communication, for more information read info at
 * the top of this file.
 *
 * @param h           Device's handle
 * @param addr        Address to read configuration object from
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__r_config_read(lt_handle_t *h, const enum CONFIGURATION_OBJECTS_REGS addr);

/**
 * @brief Decodes 'r config read' result payload. Used for separate l3 communication, for more information read info at
 * the top of this file.
 *
 * @param h           Device's handle
 * @param obj         Buffer to receive configuration object
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__r_config_read(lt_handle_t *h, uint32_t *obj);

/**
 * @brief Encodes 'r config erase' command payload. Used for separate l3 communication, for more information read info
 * at the top of this file.
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__r_config_erase(lt_handle_t *h);

/**
 * @brief Decodes 'r config erase' result payload. Used for separate l3 communication, for more information read info at
 * the top of this file.
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__r_config_erase(lt_handle_t *h);

/**
 * @brief Encodes 'i config write' command payload. Used for separate l3 communication, for more information read info
 * at the top of this file.
 *
 * @param h           Device's handle
 * @param addr        Address to write configuration object to
 * @param bit_index   Bit index to write in the configuration object
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__i_config_write(lt_handle_t *h, const enum CONFIGURATION_OBJECTS_REGS addr, const uint8_t bit_index);

/**
 * @brief Decodes 'i config write' result payload. Used for separate l3 communication, for more information read info at
 * the top of this file.
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__i_config_write(lt_handle_t *h);

/**
 * @brief Encodes 'i config read' command payload. Used for separate l3 communication, for more information read info at
 * the top of this file.
 *
 * @param h           Device's handle
 * @param addr        Address to read configuration object from
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__i_config_read(lt_handle_t *h, const enum CONFIGURATION_OBJECTS_REGS addr);

/**
 * @brief Decodes 'i config read' result payload. Used for separate l3 communication, for more information read info at
 * the top of this file.
 *
 * @param h           Device's handle
 * @param obj         Buffer to receive configuration object
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__i_config_read(lt_handle_t *h, uint32_t *obj);

/**
 * @brief Encodes 'r mem data write' command payload. Used for separate l3 communication, for more information read info
 * at the top of this file.
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__r_mem_data_write(lt_handle_t *h, const uint16_t udata_slot, uint8_t *data, const uint16_t size);

/**
 * @brief Decodes 'r mem data write' result payload. Used for separate l3 communication, for more information read info
 * at the top of this file.
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__r_mem_data_write(lt_handle_t *h);

/**
 * @brief Encodes 'r mem data read' command payload. Used for separate l3 communication, for more information read info
 * at the top of this file.
 *
 * @param h           Device's handle
 * @param udata_slot  Slot to read data from
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__r_mem_data_read(lt_handle_t *h, const uint16_t udata_slot);

/**
 * @brief Decodes 'r mem data read' result payload. Used for separate l3 communication, for more information read info
 * at the top of this file.
 *
 * @param h           Device's handle
 * @param data        Buffer to receive data
 * @param size        Number of bytes read into data
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__r_mem_data_read(lt_handle_t *h, uint8_t *data, uint16_t *size);

/**
 * @brief Encodes 'r mem data erase' command payload. Used for separate l3 communication, for more information read info
 * at the top of this file.
 *
 * @param h           Device's handle
 * @param udata_slot  Slot to erase data from
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__r_mem_data_erase(lt_handle_t *h, const uint16_t udata_slot);

/**
 * @brief Decodes 'r mem data erase' result payload. Used for separate l3 communication, for more information read info
 * at the top of this file.
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__r_mem_data_erase(lt_handle_t *h);

/**
 * @brief Encodes 'random get' command payload. Used for separate l3 communication, for more information read info at
 * the top of this file.
 *
 * @param h           Device's handle
 * @param len         Length of random data to get
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__random_get(lt_handle_t *h, const uint16_t len);

/**
 * @brief Decodes 'random get' result payload. Used for separate l3 communication, for more information read info at the
 * top of this file.
 *
 * @param h           Device's handle
 * @param buff        Buffer to receive random data
 * @param len         Length of random data to get, must be the same as sent in lt_out__random_get()
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__random_get(lt_handle_t *h, uint8_t *buff, const uint16_t len);

/**
 * @brief Encodes 'ECC key generation' command payload. Used for separate l3 communication, for more information read
 * info at the top of this file.
 *
 * @param h           Device's handle
 * @param slot        ECC key slot to generate key in
 * @param curve       ECC curve type to use for key generation
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__ecc_key_generate(lt_handle_t *h, const ecc_slot_t slot, const lt_ecc_curve_type_t curve);

/**
 * @brief Decodes 'ECC key generation' result payload. Used for separate l3 communication, for more information read
 * info at the top of this file.
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__ecc_key_generate(lt_handle_t *h);

/**
 * @brief Encodes 'ECC key store' command payload. Used for separate l3 communication, for more information read info at
 * the top of this file.
 *
 * @param h           Device's handle
 * @param slot        ECC key slot to store key in
 * @param curve       ECC curve type to use for key storage
 * @param key         Ecc key to store, 32B length
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__ecc_key_store(lt_handle_t *h, const ecc_slot_t slot, const lt_ecc_curve_type_t curve,
                               const uint8_t *key);

/**
 * @brief Decodes 'ECC key store' result payload. Used for separate l3 communication, for more information read info at
 * the top of this file.
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__ecc_key_store(lt_handle_t *h);

/**
 * @brief Encodes 'ECC key read' command payload. Used for separate l3 communication, for more information read info at
 * the top of this file.
 *
 * @param h           Device's handle
 * @param slot        ECC key slot to read key from
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__ecc_key_read(lt_handle_t *h, const ecc_slot_t slot);

/**
 * @brief Decodes 'ECC key read' result payload. Used for separate l3 communication, for more information read info at
 * the top of this file.
 *
 * @param h           Device's handle
 * @param key         Buffer to receive ECC public key (length of the key is given by *curve*)
 * @param curve       Will be filled by curve type
 * @param origin      Will be filled by origin type
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__ecc_key_read(lt_handle_t *h, uint8_t *key, lt_ecc_curve_type_t *curve, ecc_key_origin_t *origin);

/**
 * @brief Encodes 'ECC key erase' command payload. Used for separate l3 communication, for more information read info at
 * the top of this file.
 *
 * @param h           Device's handle
 * @param slot        ECC key slot to erase key from
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__ecc_key_erase(lt_handle_t *h, const ecc_slot_t slot);

/**
 * @brief Decodes 'ECC key erase' result payload. Used for separate l3 communication, for more information read info at
 * the top of this file.
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__ecc_key_erase(lt_handle_t *h);

/**
 * @brief Encodes 'ECC ECDSA sign' command payload. Used for separate l3 communication, for more information read info
 * at the top of this file.
 *
 * @param h           Device's handle
 * @param slot        ECC key slot to use for signing
 * @param msg         Message to sign
 * @param msg_len     Length of the message
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__ecc_ecdsa_sign(lt_handle_t *h, const ecc_slot_t slot, const uint8_t *msg, const uint16_t msg_len);

/**
 * @brief Decodes 'ECC ECDSA sign' result payload. Used for separate l3 communication, for more information read info at
 * the top of this file.
 *
 * @param h           Device's handle
 * @param rs          Buffer to receive signature (r,s)
 * @param rs_len      Length of the signature buffer, must be at least 64 bytes
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__ecc_ecdsa_sign(lt_handle_t *h, uint8_t *rs, const uint8_t rs_len);

/**
 * @brief Encodes 'ECC EdDSA sign' command payload. Used for separate l3 communication, for more information read info
 * at the top of this file.
 *
 * @param h           Device's handle
 * @param ecc_slot    ECC key slot to use for signing
 * @param msg         Message to sign
 * @param msg_len     Length of the message
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__ecc_eddsa_sign(lt_handle_t *h, const ecc_slot_t ecc_slot, const uint8_t *msg, const uint16_t msg_len);

/**
 * @brief Decodes 'ECC EdDSA sign' result payload. Used for separate l3 communication, for more information read info at
 * the top of this file.
 *
 * @param h           Device's handle
 * @param rs          Buffer to receive signature (r,s)
 * @param rs_len      Length of the signature buffer, must be at least 64 bytes
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__ecc_eddsa_sign(lt_handle_t *h, uint8_t *rs, const uint8_t rs_len);

/**
 * @brief Encodes 'ECC verify' command payload. Used for separate l3 communication, for more information read info at
 * the top of this file.
 *
 * @param h           Device's handle
 * @param slot        ECC key slot to use for verification
 * @param msg         Message to verify
 * @param msg_len     Length of the message
 * @param rs          Signature (r,s) to verify
 * @param rs_len      Length of the signature, must be at least 64 bytes
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__mcounter_init(lt_handle_t *h, const enum lt_mcounter_index_t mcounter_index,
                               const uint32_t mcounter_value);

/**
 * @brief Decodes 'ECC verify' result payload. Used for separate l3 communication, for more information read info at the
 * top of this file.
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__mcounter_init(lt_handle_t *h);

/**
 * @brief Encodes 'monotonic counter update' command payload. Used for separate l3 communication, for more information
 * read info at the top of this file.
 *
 * @param h           Device's handle
 * @param mcounter_index  Index of the monotonic counter to update
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__mcounter_update(lt_handle_t *h, const enum lt_mcounter_index_t mcounter_index);

/**
 * @brief Decodes 'monotonic counter update' result payload. Used for separate l3 communication, for more information
 * read info at the top of this file.
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__mcounter_update(lt_handle_t *h);

/**
 * @brief Encodes 'monotonic counter get' command payload. Used for separate l3 communication, for more information read
 * info at the top of this file.
 *
 * @param h           Device's handle
 * @param mcounter_index  Index of the monotonic counter to get
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__mcounter_get(lt_handle_t *h, const enum lt_mcounter_index_t mcounter_index);

/**
 * @brief Decodes 'monotonic counter get' result payload. Used for separate l3 communication, for more information read
 * info at the top of this file.
 *
 * @param h           Device's handle
 * @param mcounter_value  Buffer to receive the value of the monotonic counter
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__mcounter_get(lt_handle_t *h, uint32_t *mcounter_value);

/**
 * @brief Encodes 'MAC and destroy' command payload. Used for separate l3 communication, for more information read info
 * at the top of this file.
 *
 * @param h           Device's handle
 * @param slot        Slot to use for MAC and destroy operation
 * @param data_out    Data to be sent out, must be 32 bytes long
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_out__mac_and_destroy(lt_handle_t *h, mac_and_destroy_slot_t slot, const uint8_t *data_out);

/**
 * @brief Decodes 'MAC and destroy' result payload. Used for separate l3 communication, for more information read info
 * at the top of this file.
 *
 * @param h           Device's handle
 * @param data_in     Buffer to receive data, must be 32 bytes long
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_in__mac_and_destroy(lt_handle_t *h, uint8_t *data_in);

/** @} */  // end of group_libtropic_l3

#endif