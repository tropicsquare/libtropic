#ifndef LT_LIBTROPIC_EXAMPLES_H
#define LT_LIBTROPIC_EXAMPLES_H

/**
 * @defgroup libtropic_examples 2. Examples
 * @brief Show simple example usage of libtropic.
 * @{
 */

/**
 * @file libtropic_examples.h
 * @brief Functions with examples of usage of TROPIC01 chip
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <stdint.h>

#include "libtropic_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Establishes Secure Session and executes Ping L3 command.
 * @note We recommend reading TROPIC01's datasheet before diving into this example!
 *
 * @param  h     Handle for communication with TROPIC01. It is assumed that the `h.l2.device` and `h.l3.crypto_ctx`
 * members were already initialized. Because these members are pointers, the assigned structures must exist throughout
 * the whole life-cycle of the handle. Refer to the 'Get Started'->'Integrating Libtropic'->'How to Use' Section in the
 * Libtropic documentation for more information.
 *
 * @retval       0  Function executed successfully
 * @retval      -1  Function did not execute successfully
 */
int lt_ex_hello_world(lt_handle_t *h);

/**
 * @brief Establishes Secure Session and executes Ping L3 command using separated API.
 *
 * This example shows how to use separated API calls with TROPIC01. Separate calls are named lt_out__* and
 * lt_in__* and they provide splitting of the commands/results, which might be used for example for communication over a
 * tunnel. Let's say we want to speak with TROPIC01 from a server, then lt_out__* part is done on the server, then
 * encrypted payload is transferred over tunnel to the point where SPI is wired to TROPIC01. L2 communication is
 * executed, encrypted result is transferred back to the server, where lt_in__* function is used to decrypt the
 * response.
 *
 * To have a better understanding have a look into lt_ex_hello_world.c, both examples shows similar procedure.
 *
 * This might be used for example in production, where we want to establish a secure channel between HSM and TROPIC01 on
 * PCB.
 *
 * @param  h     Handle for communication with TROPIC01. It is assumed that the `h.l2.device` and `h.l3.crypto_ctx`
 * members were already initialized. Because these members are pointers, the assigned structures must exist throughout
 * the whole life-cycle of the handle. Refer to the 'Get Started'->'Integrating Libtropic'->'How to Use' Section in the
 * Libtropic documentation for more information.
 *
 * @retval       0  Function executed successfully
 * @retval      -1  Function did not execute successfully
 */
int lt_ex_hello_world_separate_API(lt_handle_t *h);

/**
 * @brief Example usage of TROPIC01 chip in a generic *hardware wallet* project.
 *
 * It is not focused on final application's usage, even though there is a few hints in session3() function.
 * Instead of that, this code mainly walks you through a different stages during a chip's lifecycle:
 *  - Initial power up during PCB manufacturing
 *  - Attestation key uploading
 *  - Final product usage

 * Example shows how content of config objects might be used to set different access rights and how chip behaves during
 * the device's lifecycle.
 *
 * @note We recommend reading TROPIC01's datasheet before diving into this example!
 * @warning We strongly recommend running this example against the TROPIC01 model only, as it does irreversible
 operations!
 *
 * @param  h     Handle for communication with TROPIC01. It is assumed that the `h.l2.device` and `h.l3.crypto_ctx`
 members were already
 * initialized. Because these members are pointers, the assigned structures must exist throughout the whole life-cycle
 * of the handle. Refer to the 'Get Started'->'Integrating Libtropic'->'How to Use' Section in the Libtropic
 * documentation for more information.
 *
 * @retval       0  Function executed successfully
 * @retval      -1  Function did not execute successfully
 */
int lt_ex_hardware_wallet(lt_handle_t *h);

/**
 * @brief Example how to update TROPIC01 firmware. Process is described in detail in 'ODN_TR01_app_007_fw_update.pdf'
 * Application Note.
 *
 * It is recommended to update both Application firmware banks with the same Application firmware
 * and both SPECT firmware banks with the same SPECT firmware.
 *
 * @param  h     Handle for communication with TROPIC01. It is assumed that the `h.l2.device` and `h.l3.crypto_ctx`
 * members were already initialized. Because these members are pointers, the assigned structures must exist throughout
 * the whole life-cycle of the handle. Refer to the 'Get Started'->'Integrating Libtropic'->'How to Use' Section in the
 * Libtropic documentation for more information.
 *
 * @retval       0  Function executed successfully
 * @retval      -1  Function did not execute successfully
 */
int lt_ex_fw_update(lt_handle_t *h);

/**
 * @brief Example usage of TROPIC01 flagship feature - 'Mac And Destroy' PIN verification engine.
 *
 * Value MACANDD_ROUNDS represents a number of possible PIN gueses, this value also affects size of lt_macandd_nvm_t
 * struct. Technically TROPIC01 is capable to have this set to 128, therefore provide 128 Mac And Destroy tries, which
 * would require roughly 128*32 bytes in non volatile memory for storing data related to M&D tries.
 *
 * In this example, TROPIC01's User R-Memory is used as a storage for data during power cycle (specifically, the last
 * slot is used). For a sake of simplicity, only one User R-Memory slot is used as a storage.
 *
 * Therefore MACANDD_ROUNDS is here limited to 12 -> the biggest possible number of tries which fits into one User
 * R-Memory slot (slot size in User R-Memory is atleast 444B in all TROPIC01 chips).
 *
 * @note We recommend reading TROPIC01's datasheet before diving into this example!
 *
 * @param  h     Handle for communication with TROPIC01. It is assumed that the `h.l2.device` and `h.l3.crypto_ctx`
 * members were already initialized. Because these members are pointers, the assigned structures must exist throughout
 * the whole life-cycle of the handle. Refer to the 'Get Started'->'Integrating Libtropic'->'How to Use' Section in the
 * Libtropic documentation for more information.
 *
 * @retval       0  Function executed successfully
 * @retval      -1  Function did not execute successfully
 */
int lt_ex_macandd(lt_handle_t *h);

/**
 * @brief This example shows how to read TROPIC01's chip ID and firmware versions. Prints chip application firmware
 * versions and also bootloader version.
 *
 * @note We recommend reading TROPIC01's datasheet before diving into this example!
 *
 * @param  h     Handle for communication with TROPIC01. It is assumed that the `h.l2.device` and `h.l3.crypto_ctx`
 * members were already initialized. Because these members are pointers, the assigned structures must exist throughout
 * the whole life-cycle of the handle. Refer to the 'Get Started'->'Integrating Libtropic'->'How to Use' Section in the
 * Libtropic documentation for more information.
 *
 * @retval       0  Function executed successfully
 * @retval      -1  Function did not execute successfully
 */
int lt_ex_show_chip_id_and_fwver(lt_handle_t *h);

/** @} */  // end of libtropic_examples group

#ifdef __cplusplus
}
#endif

#endif  // LT_LIBTROPIC_EXAMPLES_H
