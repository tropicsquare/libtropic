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
 * @brief Libtropic library main API header file.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdbool.h>
#include <stddef.h>

#include "libtropic_common.h"

/**
 * @brief Initialize handle and transport layer
 *
 * @param h           Device's handle
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 *
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
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_deinit(lt_handle_t *h);

/**
 * @brief Update mode variable in handle.
 * Reads one byte from SPI, checks `CHIP_MODE_STARTUP_bit` and updates this information in `lt_l2_state_t` (part of the
 * handle).
 *
 * Info from this bit is updated in handle on every L1 transaction anyway.
 * This function can be used to actualize it whenever user wants.
 *
 * @param h           Device's handle
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_update_mode(lt_handle_t *h);

/**
 * @brief Read out PKI chain from TROPIC01's Certificate Store
 *
 * @param h           Device's handle
 * @param store       Certificate store handle to be filled
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_get_info_cert_store(lt_handle_t *h, struct lt_cert_store_t *store);

/**
 * @brief Extracts ST_Pub from TROPIC01's Certificate Store
 *
 * @param store       Certificate store handle
 * @param stpub       TROPIC01 STPUB to be filled, unique for each device
 * @param stpub_len   Length of buffer for STPub
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
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
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_get_info_chip_id(lt_handle_t *h, struct lt_chip_id_t *chip_id);

/**
 * @brief Read TROPIC01's RISC-V firmware version
 *
 * @param h           Device's handle
 * @param ver Buffer for FW version bytes with size `LT_L2_GET_INFO_RISCV_FW_SIZE`
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_get_info_riscv_fw_ver(lt_handle_t *h, uint8_t *ver);

/**
 * @brief Read TROPIC01's SPECT firmware version
 *
 * @param h           Device's handle
 * @param ver Buffer for SPECT version bytes with size `LT_L2_GET_INFO_SPECT_FW_SIZE`
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_get_info_spect_fw_ver(lt_handle_t *h, uint8_t *ver);

/**
 * @brief Read TROPIC01's firmware bank info
 *
 * @param h           Device's handle
 * @param bank_id     ID of firmware bank (one from enum lt_bank_id_t)
 * @param header      Buffer to store fw header bytes into
 * @param max_len     Length of a buffer
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_get_info_fw_bank(lt_handle_t *h, const lt_bank_id_t bank_id, uint8_t *header, const uint16_t max_len);

/**
 * @brief Establishes encrypted secure session between TROPIC01 and host MCU
 *
 * @note              To successfully estabilish Secure Session, you need to know Tropic01's X25519 public key.
 *                    (STPUB). The STPUB can be obtained using lt_get_st_pub, or you can use
 *                    lt_verify_chip_and_start_secure_session helper function, which will obtain the STPUB
 *                    automatically and set up the Secure Session for you.
 *
 * @param h           Device's handle
 * @param stpub       STPUB from device's certificate
 * @param pkey_index  Index of pairing public key
 * @param shipriv     Secure host private key
 * @param shipub      Secure host public key
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_session_start(lt_handle_t *h, const uint8_t *stpub, const lt_pkey_index_t pkey_index, const uint8_t *shipriv,
                          const uint8_t *shipub);

/**
 * @brief Aborts encrypted secure session between TROPIC01 and host MCU
 *
 * @note              Data used for the Secure Session are always invalidated regardless
 *                    of the result of the abort request (if you pass correct handle).
 *                    After calling this function, it will not be possible to send L3 commands
 *                    unless new Secure Session is started.
 *
 * @param h           Device's handle
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_session_abort(lt_handle_t *h);

/**
 * @brief Puts TROPIC01 into sleep
 *
 * @param h           Device's handle
 * @param sleep_kind  Kind of sleep
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_sleep(lt_handle_t *h, const uint8_t sleep_kind);

/**
 * @brief Reboots TROPIC01
 *
 * @param h           Device's handle
 * @param startup_id  Startup ID
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_reboot(lt_handle_t *h, const uint8_t startup_id);

#ifdef ABAB
/** @brief Maximal size of update data */
#define LT_MUTABLE_FW_UPDATE_SIZE_MAX 25600
/**
 * @brief Erase mutable firmware in one of banks
 *
 * @param h           Device's handle
 * @param bank_id     enum lt_bank_id_t
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_mutable_fw_erase(lt_handle_t *h, const lt_bank_id_t bank_id);

/**
 * @brief Update mutable firmware in one of banks
 *
 * @param h             Device's handle
 * @param fw_data       Array with firmware bytes
 * @param fw_data_size  Number of firmware's bytes in passed array
 * @param bank_id       enum lt_bank_id_t
 * lt_ret_t             LT_OK            - SUCCESS
 *                      other parameters - ERROR, for verbose output pass return value to function lt_ret_verbose()
 */
lt_ret_t lt_mutable_fw_update(lt_handle_t *h, const uint8_t *fw_data, const uint16_t fw_data_size, lt_bank_id_t bank_id);

#elif ACAB
/** @brief Maximal size of update data */
#define LT_MUTABLE_FW_UPDATE_SIZE_MAX 30720

/**
 * @brief Sends mutable firmware update L2 request to TROPIC01 with silicon revision ACAB
 *
 * @param h               Device's handle
 * @param update_request  Array with firmware update request bytes
 * @return                LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_mutable_fw_update(lt_handle_t *h, const uint8_t *update_request);

/**
 * @brief Sends mutable firmware update data to TROPIC01 with silicon revision ACAB. Function
 * `lt_mutable_fw_update()` must be called first to start authenticated mutable fw update.
 *
 * @param h                 Device's handle
 * @param update_data       Array with firmware update data bytes
 * @param update_data_size  Size of update data
 * @return                  LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_mutable_fw_update_data(lt_handle_t *h, const uint8_t *update_data, const uint16_t update_data_size);

#endif
/**
 * @brief Gets Log message of TROPIC01's RISC-V FW (if enabled/available).
 * @note RISC-V FW logging can be disabled in the I/R-Config and for the production chips, it **will** be disabled. This
 * function is used mainly for internal debugging and not expected to be used by the user.
 *
 * @param h            Device's handle
 * @param log_msg      Buffer for the log message (atleast 255B)
 * @param log_msg_len  Length of the log message
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_get_log_req(lt_handle_t *h, uint8_t *log_msg, uint16_t *log_msg_len);

/**
 * @brief A dummy command to check the Secure Channel Session communication by exchanging a message with TROPIC01, whish
 * is echoed through the Secure Channel.
 *
 * @param h           Device's handle
 * @param msg_out     Ping message going out
 * @param msg_in      Ping message going in
 * @param len         Length of both messages (msg_out and msg_in)
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_ping(lt_handle_t *h, const uint8_t *msg_out, uint8_t *msg_in, const uint16_t len);

/**
 * @brief Writes pairing public key into TROPIC01's pairing key slot 0-3
 *
 * @param h           Device's handle
 * @param pairing_pub 32B of pubkey
 * @param slot        Pairing key lot SH0PUB - SH3PUB
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_pairing_key_write(lt_handle_t *h, const uint8_t *pairing_pub, const uint8_t slot);

/**
 * @brief Reads pairing public key from TROPIC01's pairing key slot 0-3
 *
 * @param h           Device's handle
 * @param pairing_pub 32B of pubkey
 * @param slot        Pairing key lot SH0PUB - SH3PUB
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_pairing_key_read(lt_handle_t *h, uint8_t *pairing_pub, const uint8_t slot);

/**
 * @brief Invalidates pairing key in slot 0-3
 *
 * @param h           Device's handle
 * @param slot        Pairing key lot SH0PUB - SH3PUB
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_pairing_key_invalidate(lt_handle_t *h, const uint8_t slot);

/**
 * @brief Writes configuration object specified by `addr`
 *
 * @param h           Device's handle
 * @param addr        Address of a config object
 * @param obj         Content to be written
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_r_config_write(lt_handle_t *h, enum CONFIGURATION_OBJECTS_REGS addr, const uint32_t obj);

/**
 * @brief Reads configuration object specified by `addr`
 *
 * @param h           Device's handle
 * @param addr        Address of a config object
 * @param obj         Variable to read content into
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 *
 */
lt_ret_t lt_r_config_read(lt_handle_t *h, const enum CONFIGURATION_OBJECTS_REGS addr, uint32_t *obj);

/**
 * @brief Erases all configuration objects
 *
 * @param h           Device's handle
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_r_config_erase(lt_handle_t *h);

/**
 * @brief Writes configuration object specified by `addr` to I-Config
 *
 * @param h           Device's handle
 * @param addr        Address of a config object
 * @param bit_index   Index of bit to write from 1 to 0
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_i_config_write(lt_handle_t *h, const enum CONFIGURATION_OBJECTS_REGS addr, const uint8_t bit_index);

/**
 * @brief Reads configuration object specified by `addr` from I-Config
 *
 * @param h           Device's handle
 * @param addr        Address of a config object
 * @param obj         Variable to read content into
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_i_config_read(lt_handle_t *h, const enum CONFIGURATION_OBJECTS_REGS addr, uint32_t *obj);

/**
 * @brief Writes bytes into a given slot of the User Partition in the R memory
 *
 * @param h           Device's handle
 * @param udata_slot  Memory's slot to be written
 * @param data        Buffer of data to be written into R MEMORY slot
 * @param size        Size of data to be written (valid range given by macros `R_MEM_DATA_SIZE_MIN` and
 * `R_MEM_DATA_SIZE_MAX`)
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_r_mem_data_write(lt_handle_t *h, const uint16_t udata_slot, const uint8_t *data, const uint16_t size);

/**
 * @brief Reads bytes from a given slot of the User Partition in the R memory
 *
 * @param h           Device's handle
 * @param udata_slot  Memory's slot to be read
 * @param data        Buffer to read data into
 * @param size        Number of bytes read into data
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_r_mem_data_read(lt_handle_t *h, const uint16_t udata_slot, uint8_t *data, uint16_t *size);

/**
 * @brief Erases the given slot of the User Partition in the R memory
 *
 * @param h           Device's handle
 * @param udata_slot  Memory's slot to be erased
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_r_mem_data_erase(lt_handle_t *h, const uint16_t udata_slot);

/**
 * @brief Gets random bytes from TROPIC01's Random Number Generator.
 *
 * @param h           Device's handle
 * @param buff        Buffer
 * @param len         Number of random bytes (255 bytes is the maximum)
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_random_value_get(lt_handle_t *h, uint8_t *buff, const uint16_t len);

/**
 * @brief Generates ECC key in the specified ECC key slot
 *
 * @param h           Device's handle
 * @param slot        Slot number ecc_slot_t
 * @param curve       Type of ECC curve. Use L3_ECC_KEY_GENERATE_CURVE_ED25519 or L3_ECC_KEY_GENERATE_CURVE_P256
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_ecc_key_generate(lt_handle_t *h, const ecc_slot_t slot, const lt_ecc_curve_type_t curve);

/**
 * @brief Stores ECC key to the specified ECC key slot
 *
 * @param h           Device's handle
 * @param slot        Slot number ecc_slot_t
 * @param curve       Type of ECC curve. Use L3_ECC_KEY_GENERATE_CURVE_ED25519 or L3_ECC_KEY_GENERATE_CURVE_P256
 * @param key         Key to store (only the first 32 bytes are stored)
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_ecc_key_store(lt_handle_t *h, const ecc_slot_t slot, const lt_ecc_curve_type_t curve, const uint8_t *key);

/**
 * @brief Reads ECC public key corresponding to a private key in the specified ECC key slot.
 *
 * @param h           Device's handle
 * @param ecc_slot    Slot number ECC_SLOT_0 - ECC_SLOT_31
 * @param key         Buffer for retrieving a key; length depends on the type of key in the slot (32B for Ed25519, 64B
 * for P256), according to *curve*
 * @param curve       Will be filled by curve byte
 * @param origin      Will be filled by origin byte
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_ecc_key_read(lt_handle_t *h, const ecc_slot_t ecc_slot, uint8_t *key, lt_ecc_curve_type_t *curve,
                         lt_ecc_key_origin_t *origin);

/**
 * @brief Erases ECC key from the specified ECC key slot
 *
 * @param h           Device's handle
 * @param ecc_slot    Slot number ECC_SLOT_0 - ECC_SLOT_31
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_ecc_key_erase(lt_handle_t *h, const ecc_slot_t ecc_slot);

/**
 * @brief Performs ECDSA sign of a message with a private ECC key stored in TROPIC01
 *
 * @param h           Device's handle
 * @param ecc_slot    Slot containing a private key, ECC_SLOT_0 - ECC_SLOT_31
 * @param msg         Buffer containing a message
 * @param msg_len     Length of msg's buffer
 * @param rs          Buffer for storing a signature in a form of R and S bytes (should always have length 64B)
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_ecc_ecdsa_sign(lt_handle_t *h, const ecc_slot_t ecc_slot, const uint8_t *msg, const uint32_t msg_len,
                           uint8_t *rs);

/**
 * @brief Verifies ECDSA signature. Host side only, does not require TROPIC01.
 *
 * @param msg         Message
 * @param msg_len     Length of message
 * @param pubkey      Public key related to private key which signed the message (64B)
 * @param rs          Signature to be verified, in a form of R and S bytes (should always have length 64B)
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 *
 */
lt_ret_t lt_ecc_ecdsa_sig_verify(const uint8_t *msg, const uint32_t msg_len, const uint8_t *pubkey, const uint8_t *rs);

/**
 * @brief Performs EdDSA sign of a message with a private ECC key stored in TROPIC01
 *
 * @param h           Device's handle
 * @param ecc_slot    Slot containing a private key, ECC_SLOT_0 - ECC_SLOT_31
 * @param msg         Buffer containing a message to sign, max length is 4096B
 * @param msg_len     Length of a message
 * @param rs          Buffer for storing a signature in a form of R and S bytes (should always have length 64B)
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_ecc_eddsa_sign(lt_handle_t *h, const ecc_slot_t ecc_slot, const uint8_t *msg, const uint16_t msg_len,
                           uint8_t *rs);

/**
 * @brief Verifies EdDSA signature. Host side only, does not require TROPIC01.
 *
 * @param msg         Message
 * @param msg_len     Length of message. Max length is 4095
 * @param pubkey      Public key related to private key which signed the message (32B)
 * @param rs          Signature to be verified, in a form of R and S bytes (should always have length 64B)
 * lt_ret_t           LT_OK          - success
 *                    LT_PARAM_ERROR - wrong parameters were passed
 *                    LT_FAIL        - signature verify failed
 */
lt_ret_t lt_ecc_eddsa_sig_verify(const uint8_t *msg, const uint16_t msg_len, const uint8_t *pubkey, const uint8_t *rs);

/**
 * @brief Initializes monotonic counter of a given index
 *
 * @param h               Device's handle
 * @param mcounter_index  Index of monotonic counter
 * @param mcounter_value  Value to set as an initial value (allowed range is 0-MCOUNTER_VALUE_MAX)
 *
 * @retval                LT_OK Function executed successfully
 * @retval                other Function did not execute successully, you might use lt_ret_verbose() to get verbose
 * encoding of returned value
 */
lt_ret_t lt_mcounter_init(lt_handle_t *h, const enum lt_mcounter_index_t mcounter_index, const uint32_t mcounter_value);

/**
 * @brief Updates monotonic counter of a given index
 *
 * @param h               Device's handle
 * @param mcounter_index  Index of monotonic counter
 *
 * @retval                LT_OK Function executed successfully
 * @retval                other Function did not execute successully, you might use lt_ret_verbose() to get verbose
 * encoding of returned value
 */
lt_ret_t lt_mcounter_update(lt_handle_t *h, const enum lt_mcounter_index_t mcounter_index);

/**
 * @brief Gets a value of a monotonic counter of a given index
 *
 * @param h               Device's handle
 * @param mcounter_index  Index of monotonic counter
 * @param mcounter_value  Value of monotonic counter (from range 0-MCOUNTER_VALUE_MAX)
 *
 * @retval                LT_OK Function executed successfully
 * @retval                other Function did not execute successully, you might use lt_ret_verbose() to get verbose
 * encoding of returned value
 */
lt_ret_t lt_mcounter_get(lt_handle_t *h, const enum lt_mcounter_index_t mcounter_index, uint32_t *mcounter_value);

/**
 * @brief Executes the MAC-and-Destroy sequence.
 * @details This command is just a part of MAC And Destroy sequence, which takes place between the host and TROPIC01.
 *          Example code can be found in examples/lt_ex_macandd.c, for more info about Mac And Destroy functionality,
 * read the Application note.
 *
 * @param h           Device's handle
 * @param slot        Mac-and-Destroy slot index, valid values are 0-127
 * @param data_out    Data to be sent from host to TROPIC01
 * @param data_in     Data returned from TROPIC01 to host
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_mac_and_destroy(lt_handle_t *h, mac_and_destroy_slot_t slot, const uint8_t *data_out, uint8_t *data_in);

/** @} */  // end of libtropic_API group

#ifdef LT_HELPERS
/**
 * @defgroup libtropic_API_helpers libtropic API helpers
 * @brief These functions are usually wrappers around one or more TROPIC01 commands, beside `lt_ret_verbose()` and
 * `lt_print_bytes()`.
 * @{
 */

/** @brief Upper bound for CHIP_ID fields as hex string (used in lt_print_chip_id()). */
#define CHIP_ID_FIELD_MAX_SIZE 35

/** @brief Helper structure, holding string name and address for each configuration object. */
extern struct lt_config_obj_desc_t cfg_desc_table[LT_CONFIG_OBJ_CNT];

/**
 * @brief Prints out a name of the returned value.
 *
 * @param ret         lt_ret_t returned type value
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
const char *lt_ret_verbose(lt_ret_t ret);

/**
 * @brief Writes the whole R-Config with the passed `config`.
 *
 * @param h           Device's handle
 * @param config      Array into which objects are read
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_write_whole_R_config(lt_handle_t *h, const struct lt_config_t *config);

/**
 * @brief Reads all of the R-Config objects into `config`.
 *
 * @param h           Device's handle
 * @param config      Struct into which objects are readed
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_read_whole_R_config(lt_handle_t *h, struct lt_config_t *config);

/**
 * @brief Reads all of the I-Config objects into `config`.
 *
 * @param h           Device's handle
 * @param config      Struct into which objects are readed
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_read_whole_I_config(lt_handle_t *h, struct lt_config_t *config);

/**
 * @brief Writes the whole I-Config with the passed `config`.
 * @details Only the zero bits in `config` are written.
 *
 * @param h           Device's handle
 * @param config      Array into which objects are read
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_write_whole_I_config(lt_handle_t *h, const struct lt_config_t *config);

/**
 * @brief Establishes a secure channel between host MCU and TROPIC01
 *
 * @warning This function currently DOES NOT validate/verify the whole certificate chain, it just parses out STPUB from
 * the device's certificate, because STPUB is used for handshake.
 *
 * To verify the whole certificate chain we recommend to download all certificates from chip by using
 * lt_get_info_cert_store() and use any apropriate third party tool to verify validity of certificate chain.
 *
 * @param h           Device's handle
 * @param shipriv     Host's private pairing key for the slot `pkey_index`
 * @param shipub      Host's public pairing key for the slot `pkey_index`
 * @param pkey_index  Pairing key index
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_verify_chip_and_start_secure_session(lt_handle_t *h, uint8_t *shipriv, uint8_t *shipub, uint8_t pkey_index);

/**
 * @brief Prints bytes in hex format to the given output buffer.
 *
 * @param   bytes         Bytes to print
 * @param   length        Length of `bytes`
 * @param   out_buf       Output buffer to print to
 * @param   out_buf_size  Size of `out_buf`
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_print_bytes(const uint8_t *bytes, const uint16_t length, char *out_buf, uint16_t out_buf_size);

/**
 * @brief Interprets fields of CHIP_ID and prints them using the passed printf-like function.
 *
 * @param  chip_id     CHIP_ID structure
 * @param  print_func  printf-like function to use for printing
 *
 * @retval            LT_OK Function executed successfully
 * @retval            other Function did not execute successully, you might use lt_ret_verbose() to get verbose encoding
 * of returned value
 */
lt_ret_t lt_print_chip_id(const struct lt_chip_id_t *chip_id, int (*print_func)(const char *format, ...));

/**
 * @brief Performs mutable firmware update on ABAB and ACAB silicon revisions.
 *
 * @param h          Device's handle
 * @param update_data  Pointer to the data to be written
 * @param update_data_size  Size of the data to be written
 * @param bank_id  Bank ID where the update should be applied, valid values are
 *                     For ABAB: FW_BANK_FW1, FW_BANK_FW2, FW_BANK_SPECT1, FW_BANK_SPECT2
 *                     For ACAB: Parameter is ignored, chip is handling firmware banks on its own
 * @return             LT_OK if success, otherwise returns other error code.
 */
lt_ret_t lt_do_mutable_fw_update(lt_handle_t *h, const uint8_t *update_data, const uint16_t update_data_size,
                                 lt_bank_id_t bank_id);

/** @} */  // end of libtropic_API_helpers group
#endif

#endif