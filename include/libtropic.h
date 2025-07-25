#ifndef LT_LIBTROPIC_H
#define LT_LIBTROPIC_H

/**
 * @defgroup libtropic_API libtropic API functions
 * @brief Expected to be used by an application
 * @details Dear users, please use this API. It contains all functions you need to interface with TROPIC01 device.
 * @{
 */

/**
 * @file libtropic.h
 * @brief libtropic library main API header file
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdbool.h>
#include <stddef.h>

#include "TROPIC01_configuration_objects.h"
#include "libtropic_common.h"

/**
 * @brief Initialize handle and transport layer
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_init(lt_handle_t *h);

/**
 * @brief Deinitialize handle and transport layer
 *
 * @note              Data used for the Secure Session are always invalidated regardless
 *                    of the return value (if you pass correct handle).
 *                    After calling this function, it will not be possible to send L3 commands
 *                    unless new Secure Session is started.
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_deinit(lt_handle_t *h);

/**
 * @brief Update mode variable in handle
 * Read one byte from spi, check CHIP_MODE_STARTUP_bit and update this information in handle.alignas
 *
 * Info from this bit is updated in handle on every l1 transaction anyway.
 * This function can be used to actualize it whenever user wants.
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_update_mode(lt_handle_t *h);

/**
 * @brief Read out PKI chain from Tropic01 certificate store
 *
 * @param h           Device's handle
 * @param store       Certificate store handle to be filled
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_get_info_cert_store(lt_handle_t *h, struct lt_cert_store_t *store);

/**
 * @brief Extract ST_Pub from certificate store
 *
 * @param store       Certificate store handle
 * @param stpub       TROPIC01 STPUB to be filled, unique for each device
 * @param stpub_len   Length of buffer for STPub
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_get_st_pub(const struct lt_cert_store_t *store, uint8_t *stpub, int stpub_len);

//--------------------------------------------------------------------------------------------------------------------//
/** @brief Maximal size of returned CHIP ID */
#define LT_L2_GET_INFO_CHIP_ID_SIZE 128

/**
 * @brief Read TROPIC01's CHIP ID
 *
 * @param h           Device's handle
 * @param chip_id     Structure which holds all fields of CHIP ID
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_get_info_chip_id(lt_handle_t *h, struct lt_chip_id_t *chip_id);

/**
 * @brief Read TROPIC01's RISCV firmware version
 *
 * @param h           Device's handle
 * @param ver         Buffer for FW version bytes
 * @param max_len     Length of a buffer to store fw version in
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_get_info_riscv_fw_ver(lt_handle_t *h, uint8_t *ver, const uint16_t max_len);

/**
 * @brief Read TROPIC01's SPECT firmware version
 *
 * @param h           Device's handle
 * @param ver         Buffer for SPECT version bytes
 * @param max_len     Length of a buffer to store fw version in
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_get_info_spect_fw_ver(lt_handle_t *h, uint8_t *ver, const uint16_t max_len);

/**
 * @brief Read TROPIC01's fw bank info
 *
 * @param h           Device's handle
 * @param bank_id     ID of firmware bank (one from enum bank_id_t)
 * @param header      Buffer to store fw header bytes into
 * @param max_len     Length of a buffer
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_get_info_fw_bank(lt_handle_t *h, const bank_id_t bank_id, uint8_t *header, const uint16_t max_len);

/**
 * @brief Establish encrypted session between TROPIC01 and host MCU
 *
 * @note              To successfully estabilish Secure Session, you need to know Tropic01's X25519 public key.
 *                    (STPUB). The STPUB can be obtained using lt_get_st_pub, or you can use
 *                    verify_chip_and_start_secure_session helper function, which will obtain the STPUB
 *                    automatically and set up the Secure Session for you.
 *
 * @param h           Device's handle
 * @param stpub       STPUB from device's certificate
 * @param pkey_index  Index of pairing public key
 * @param shipriv     Secure host private key
 * @param shipub      Secure host public key
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_session_start(lt_handle_t *h, const uint8_t *stpub, const pkey_index_t pkey_index, const uint8_t *shipriv,
                          const uint8_t *shipub);

/**
 * @brief Abort encrypted session between TROPIC01 and host MCU
 *
 * @note              Data used for the Secure Session are always invalidated regardless
 *                    of the result of the abort request (if you pass correct handle).
 *                    After calling this function, it will not be possible to send L3 commands
 *                    unless new Secure Session is started.
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_session_abort(lt_handle_t *h);

/**
 * @brief Put TROPIC01 into sleep
 *
 * @param h           Device's handle
 * @param sleep_kind  Kind of sleep
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_sleep(lt_handle_t *h, const uint8_t sleep_kind);

/**
 * @brief Reboot TROPIC01
 *
 * @param h           Device's handle
 * @param startup_id  Startup ID
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_reboot(lt_handle_t *h, const uint8_t startup_id);

/**
 * @brief Erase mutable firmware in one of banks
 *
 * @param h           Device's handle
 * @param bank_id     enum bank_id_t
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_mutable_fw_erase(lt_handle_t *h, bank_id_t bank_id);

/**
 * @brief Update mutable firmware in one of banks
 *
 * @param h           Device's handle
 * @param fw_data     Array with firmware bytes
 * @param fw_data_size  Number of firmware's bytes in passed array
 * @param bank_id     enum bank_id_t
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_mutable_fw_update(lt_handle_t *h, const uint8_t *fw_data, const uint16_t fw_data_size, bank_id_t bank_id);

/**
 * @brief Get TROPIC01's internal log message (if enabled/available)
 *
 * @param h           Device's handle
 * @param log_msg     Log message
 * @param msg_len_max Max possible length of TROPIC01's log message
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_get_log(lt_handle_t *h, uint8_t *log_msg, uint16_t msg_len_max);

/**
 * @brief Test secure session by exchanging a message with chip
 *  msg_out of length 'len' is echoed through secure channel.
 *
 * @param h           Device's handle
 * @param msg_out     Ping message going out
 * @param msg_in      Ping message going in
 * @param len         Length of both messages (msg_out and msg_in)
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_ping(lt_handle_t *h, const uint8_t *msg_out, uint8_t *msg_in, const uint16_t len);

/**
 * @brief Write pairing public key into TROPIC01's pairing key slot 0-3
 *
 * @param h           Device's handle
 * @param pairing_pub 32B of pubkey
 * @param slot        Pairing key lot SH0PUB - SH3PUB
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_pairing_key_write(lt_handle_t *h, const uint8_t *pairing_pub, const uint8_t slot);

/**
 * @brief Read pairing public key from TROPIC01's pairing key slot 0-3
 *
 * @param h           Device's handle
 * @param pairing_pub 32B of pubkey
 * @param slot        Pairing key lot SH0PUB - SH3PUB
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_pairing_key_read(lt_handle_t *h, uint8_t *pairing_pub, const uint8_t slot);

/**
 * @brief Invalidate pairing key in slo 0-3
 *
 * @param h           Device's handle
 * @param slot        Pairing key lot SH0PUB - SH3PUB
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_pairing_key_invalidate(lt_handle_t *h, const uint8_t slot);

/**
 * @brief Write one configuration object
 *
 * @param h           Device's handle
 * @param addr        Address of a config object
 * @param obj         Content to be written
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_r_config_write(lt_handle_t *h, enum CONFIGURATION_OBJECTS_REGS addr, const uint32_t obj);

/**
 * @brief Read one configuration object
 *
 * @param h           Device's handle
 * @param addr        Address of a config object
 * @param obj         Variable to read content into
 * @return            LT_OK if success, otherwise returns other error code.
 *
 */
lt_ret_t lt_r_config_read(lt_handle_t *h, const enum CONFIGURATION_OBJECTS_REGS addr, uint32_t *obj);

/**
 * @brief Erase the whole config space
 *
 * @param h           Device's handle
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_r_config_erase(lt_handle_t *h);

/**
 * @brief Write one I config object
 *
 * @param h           Device's handle
 * @param addr        Address of a config object
 * @param bit_index   Index of bit to write from 1 to 0
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_i_config_write(lt_handle_t *h, const enum CONFIGURATION_OBJECTS_REGS addr, const uint8_t bit_index);

/**
 * @brief Read one I config object
 *
 * @param h           Device's handle
 * @param addr        Address of a config object
 * @param obj         Variable to read content into
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_i_config_read(lt_handle_t *h, const enum CONFIGURATION_OBJECTS_REGS addr, uint32_t *obj);

/**
 * @brief Write bytes into a given slot of R MEMORY
 *
 * @param h           Device's handle
 * @param udata_slot  Memory's slot to be written
 * @param data        Buffer of data to be written into R MEMORY slot
 * @param size        Size of data to be read
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_r_mem_data_write(lt_handle_t *h, const uint16_t udata_slot, uint8_t *data, const uint16_t size);

/**
 * @brief Read bytes from a given slot of R MEMORY
 *
 * @param h           Device's handle
 * @param udata_slot  Memory's slot to be read
 * @param data        Buffer to read data into
 * @param size        Number of bytes read into data
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_r_mem_data_read(lt_handle_t *h, const uint16_t udata_slot, uint8_t *data, uint16_t *size);

/**
 * @brief Erase bytes from a given slot of R MEMORY
 *
 * @param h           Device's handle
 * @param udata_slot  Memory's slot to be erased
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_r_mem_data_erase(lt_handle_t *h, const uint16_t udata_slot);

/**
 * @brief Get random bytes from TROPIC01.
 *
 * @param h           Device's handle
 * @param buff        Buffer
 * @param len         Number of random bytes (255 bytes is the maximum)
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_random_value_get(lt_handle_t *h, uint8_t *buff, const uint16_t len);

/**
 * @brief Generate ECC key in the device's ECC key slot
 *
 * @param h           Device's handle
 * @param slot        Slot number ecc_slot_t
 * @param curve       Type of ECC curve. Use L3_ECC_KEY_GENERATE_CURVE_ED25519 or L3_ECC_KEY_GENERATE_CURVE_P256
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_ecc_key_generate(lt_handle_t *h, const ecc_slot_t slot, const lt_ecc_curve_type_t curve);

/**
 * @brief Store ECC key in the device's ECC key slot.
 *
 * @param h           Device's handle
 * @param slot        Slot number ecc_slot_t
 * @param curve       Type of ECC curve. Use L3_ECC_KEY_GENERATE_CURVE_ED25519 or L3_ECC_KEY_GENERATE_CURVE_P256
 * @param key         Key to store (only the first 32 bytes are stored)
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_ecc_key_store(lt_handle_t *h, const ecc_slot_t slot, const lt_ecc_curve_type_t curve, const uint8_t *key);

/**
 * @brief Read ECC public key corresponding to a private key in device's slot.
 *
 * @param h           Device's handle
 * @param ecc_slot    Slot number ECC_SLOT_0 - ECC_SLOT_31
 * @param key         Buffer for retrieving a key; length depends on the type of key in the slot (32B for Ed25519, 64B
 * for P256), according to *curve*
 * @param curve       Will be filled by curve byte
 * @param origin      Will be filled by origin byte
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_ecc_key_read(lt_handle_t *h, const ecc_slot_t ecc_slot, uint8_t *key, lt_ecc_curve_type_t *curve,
                         ecc_key_origin_t *origin);

/**
 * @brief Erase ECC key from device's slot
 *
 * @param h           Device's handle
 * @param ecc_slot    Slot number ECC_SLOT_0 - ECC_SLOT_31
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_ecc_key_erase(lt_handle_t *h, const ecc_slot_t ecc_slot);

/**
 * @brief ECDSA sign message with a private key stored in TROPIC01 device
 *
 * @param h           Device's handle
 * @param ecc_slot    Slot containing a private key, ECC_SLOT_0 - ECC_SLOT_31
 * @param msg         Buffer containing a message
 * @param msg_len     Length of msg's buffer
 * @param rs          Buffer for storing a signature in a form of R and S bytes (should always have length 64B)
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_ecc_ecdsa_sign(lt_handle_t *h, const ecc_slot_t ecc_slot, const uint8_t *msg, const uint32_t msg_len,
                           uint8_t *rs);

/**
 * @brief ECDSA signature verify, HOST SIDE ONLY, does not require TROPIC01 device
 *
 * @param msg         Message
 * @param msg_len     Length of message
 * @param pubkey      Public key related to private key which signed the message (64B)
 * @param rs          Signature to be verified, in a form of R and S bytes (should always have length 64B)
 * @return            LT_OK if success, otherwise returns other error code.
 *
 */
lt_ret_t lt_ecc_ecdsa_sig_verify(const uint8_t *msg, const uint32_t msg_len, const uint8_t *pubkey, const uint8_t *rs);

/**
 * @brief EdDSA sign message with a private key stored in TROPIC01 device
 *
 * @param h           Device's handle
 * @param ecc_slot    Slot containing a private key, ECC_SLOT_0 - ECC_SLOT_31
 * @param msg         Buffer containing a message to sign, max length is 4096B
 * @param msg_len     Length of a message
 * @param rs          Buffer for storing a signature in a form of R and S bytes (should always have length 64B)
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_ecc_eddsa_sign(lt_handle_t *h, const ecc_slot_t ecc_slot, const uint8_t *msg, const uint16_t msg_len,
                           uint8_t *rs);

/**
 * @brief EdDSA signature verify, HOST SIDE ONLY, does not require TROPIC01 device
 *
 * @param msg         Message
 * @param msg_len     Length of message. Max length is 4095
 * @param pubkey      Public key related to private key which signed the message (32B)
 * @param rs          Signature to be verified, in a form of R and S bytes (should always have length 64B)
 * @return            LT_OK if success, otherwise returns other error code. TODO info about other ret values
 *
 */
lt_ret_t lt_ecc_eddsa_sig_verify(const uint8_t *msg, const uint16_t msg_len, const uint8_t *pubkey, const uint8_t *rs);

/**
 * @brief Initialize monotonic counter of a given index
 *
 * @param h               Device's handle
 * @param mcounter_index  Index of monotonic counter
 * @param mcounter_value  Value to set as an initial value
 * @return                LT_OK if success, otherwise returns other error code. TODO info about other ret values
 */
lt_ret_t lt_mcounter_init(lt_handle_t *h, const enum lt_mcounter_index_t mcounter_index, const uint32_t mcounter_value);

/**
 * @brief Update monotonic counter of a given index
 *
 * @param h               Device's handle
 * @param mcounter_index  Index of monotonic counter
 * @return                LT_OK if success, otherwise returns other error code. TODO info about other ret values
 */
lt_ret_t lt_mcounter_update(lt_handle_t *h, const enum lt_mcounter_index_t mcounter_index);

/**
 * @brief Get a value of a monotonic counter of a given index
 *
 * @param h               Device's handle
 * @param mcounter_index  Index of monotonic counter
 * @param mcounter_value  Value of monotonic counter
 * @return                LT_OK if success, otherwise returns other error code. TODO info about other ret values
 */
lt_ret_t lt_mcounter_get(lt_handle_t *h, const enum lt_mcounter_index_t mcounter_index, uint32_t *mcounter_value);

/**
 * @brief Execute the MAC-and-Destroy sequence.
 * @details This command is just a part of MAc And Destroy sequence, which takes place between the host and TROPIC01.
 *          Example code can be found in examples/lt_ex_macandd.c, for more info about Mac And Destroy functionality
 * read app note.
 *
 * @param h           Device's handle
 * @param slot        Mac-and-Destroy slot index, valid values are 0-127
 * @param data_out    Data to be sent from host to TROPIC01
 * @param data_in     Data returned from TROPIC01 to host
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_mac_and_destroy(lt_handle_t *h, mac_and_destroy_slot_t slot, const uint8_t *data_out, uint8_t *data_in);

/**
 * @details Helper function for printing out name of returned value
 *
 * @param ret lt_ret_t returned type value
 * @return const char* description of return value.
 */
const char *lt_ret_verbose(lt_ret_t ret);

#ifdef LT_HELPERS

/** @brief Helper structure, holding string name and address for each configuration object. */
extern struct lt_config_obj_desc_t cfg_desc_table[LT_CONFIG_OBJ_CNT];

/**
 * @brief This function reads config objects from TROPIC01 and prints them out
 *
 * @param h           Device's handle
 * @param config      Array into which objects are read
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t write_whole_R_config(lt_handle_t *h, const struct lt_config_t *config);

/**
 * @brief This function reads R config objects into structure
 *
 * @param h           Device's handle
 * @param config      Struct into which objects are readed
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t read_whole_R_config(lt_handle_t *h, struct lt_config_t *config);

/**
 * @brief This function reads I config objects into structure
 *
 * @param h           Device's handle
 * @param config      Struct into which objects are readed
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t read_whole_I_config(lt_handle_t *h, struct lt_config_t *config);

/**
 * @brief This function establish a secure channel between host MCU and TROPIC01 chip
 *
 * @param h           Device's handle
 * @param shipriv     Host's private pairing key (SHiPUB)
 * @param shipub      Host's public pairing key  (SHiPUB)
 * @param pkey_index  Pairing key's index
 * @return            LT_OK if success, otherwise returns other error code.
 */
lt_ret_t verify_chip_and_start_secure_session(lt_handle_t *h, uint8_t *shipriv, uint8_t *shipub, uint8_t pkey_index);

#endif

/** @} */  // end of group_libtropic_API

#endif