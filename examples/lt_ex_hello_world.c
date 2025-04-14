/**
 * @file TROPIC01_hw_wallet.c
 * @brief Example usage of TROPIC01 chip in a generic *hardware wallet* project.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "string.h"

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_examples.h"

/**
 * @name Hello World
 * @note We recommend reading TROPIC01's datasheet before diving into this example!
 * @par
 */

/**
 * @brief Converts a byte array into a formatted string.
 *
 * @param data Pointer to the byte array.
 * @param len Length of the byte array.
 * @return char* Pointer to the buffer containing the formatted string.
 */

 static char buffer[196]; // External buffer to store the formatted string

static char* print_bytes(uint8_t *data, uint16_t len) {
    if(len >196) {
        return NULL;
    }
    if (!data) {
        return NULL; // Check for NULL pointer
    }
    buffer[0] = '\0';         // Initialize the buffer as an empty string

    for (uint16_t i = 0; i < len; i++) {
        char byte_str[4]; // Temporary buffer for a single byte (e.g., "FF ")
        snprintf(byte_str, sizeof(byte_str), "%02X", data[i]);
        // Check if appending the byte would exceed the buffer size
        if (strlen(buffer) + strlen(byte_str) + 1 > sizeof(buffer)) {
            break; // Stop if the buffer is full
        }
        strncat(buffer, byte_str, sizeof(buffer) - strlen(buffer) - 1);
    }

    return buffer; // Return the pointer to the buffer
}

static char buffer_chip_id_ver[196]; // External buffer to store the formatted string
static char* interpret_chip_id_ver(uint8_t *ver) {
    if (!ver) {
        buffer_chip_id_ver[0] = '\0'; // Initialize the buffer as an empty string
        return buffer_chip_id_ver; // Check for NULL pointer
    }

    snprintf(buffer_chip_id_ver, sizeof(buffer_chip_id_ver), "(v%d.%d.%d.%d)", ver[0], ver[1], ver[2], ver[3]);

    return buffer_chip_id_ver;
}

static lt_ret_t print_chip_id(struct lt_chip_id_t *chip_id) {
    if (!chip_id) {
        LT_LOG("Error: chip_id is NULL");
        return LT_PARAM_ERR;
    }

    if (chip_id->chip_id_ver[0] != 0x01) {
        LT_LOG("Error: Unsupported CHIP_ID version");
        return LT_FAIL;
    }

    LT_LOG("\tCHIP_ID version        = %s   %s", print_bytes(chip_id->chip_id_ver, sizeof(chip_id->chip_id_ver)), interpret_chip_id_ver(chip_id->chip_id_ver));
    LT_LOG("\tFL_PROD_DATA           = %s", print_bytes(chip_id->fl_chip_info, sizeof(chip_id->fl_chip_info)));
    LT_LOG("\tMAN_FUNC_TEST          = %s", print_bytes(chip_id->func_test_info, sizeof(chip_id->func_test_info)));
    LT_LOG("\tSilicon Rev            = %s", print_bytes(chip_id->silicon_rev, sizeof(chip_id->silicon_rev)));
    LT_LOG("\tPackage ID             = %s", print_bytes(chip_id->packg_type_id, sizeof(chip_id->packg_type_id)));
    // 2B RFU

    // Provisioning info
    LT_LOG("\tProv info version      = %02X", chip_id->prov_ver_fab_id_pn[0]);
    LT_LOG("\tFab ID                 = %03X", (*chip_id->prov_ver_fab_id_pn >> 12) & 0xFFF);
    LT_LOG("\tP/N ID (short P/N)     = %02X", (*chip_id->prov_ver_fab_id_pn >> 24) & 0xFF);

    LT_LOG("\tProv date              = %s", print_bytes(chip_id->provisioning_date, sizeof(chip_id->provisioning_date)));
    LT_LOG("\tHSM HW/FW/SW version   = %s", print_bytes(chip_id->hsm_ver, sizeof(chip_id->hsm_ver)));
    LT_LOG("\tProgrammer version     = %s", print_bytes(chip_id->prog_ver, sizeof(chip_id->prog_ver)));
    // 2B RFU
    LT_LOG("\tS/N                    = %s", print_bytes((uint8_t*)&chip_id->ser_num, sizeof(chip_id->ser_num)));

    // Part Number
    LT_LOG("\tP/N (long)             = %s", print_bytes(chip_id->part_num_data, sizeof(chip_id->part_num_data)));
    // Provisioning Data version
    LT_LOG("\tProv template version: = %s", print_bytes(chip_id->prov_templ_ver, sizeof(chip_id->prov_templ_ver)));
    LT_LOG("\tProv template tag:     = %s", print_bytes(chip_id->prov_templ_tag, sizeof(chip_id->prov_templ_tag)));
    LT_LOG("\tProv spec version:     = %s", print_bytes(chip_id->prov_spec_ver, sizeof(chip_id->prov_spec_ver)));
    LT_LOG("\tProv spec tag:         = %s", print_bytes(chip_id->prov_spec_tag, sizeof(chip_id->prov_spec_tag)));

    // Batch ID
    LT_LOG("\tBatch ID:              = %s", print_bytes(chip_id->batch_id, sizeof(chip_id->batch_id)));

    // 27B RFU
    return LT_OK;
}

/**
 * @brief Session with H0 pairing keys
 *
 * @param h           Device's handle
 * @return            0 if success, otherwise -1
 */
static int session_H0(void)
{
    lt_handle_t h = {0};

    lt_init(&h);

    LT_LOG("%s", "Establish session with H0");

    // Read chip ID
    LT_LOG("lt_get_info_chip_id()");
    struct lt_chip_id_t chip_id = {0};
    LT_ASSERT(LT_OK, lt_get_info_chip_id(&h, &chip_id));
    LT_LOG("print_chip_id()");
    print_chip_id(&chip_id);
    LT_LOG_LINE();

    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, pkey_index_0));

    uint8_t in[100] = {0};
    uint8_t out[100] = {0};
    memcpy(out, "This is Hello World message from TROPIC01!!", 43);
    LT_LOG("%s", "lt_ping() ");
    LT_ASSERT(LT_OK, lt_ping(&h, out, in, 43));
    LT_LOG("\t\tMessage: %s", in);

    lt_deinit(&h);

    return 0;
}

int lt_ex_hello_world(void)
{
    LT_LOG("");
    LT_LOG("\t=======================================================================");
    LT_LOG("\t=====  TROPIC01 Hello World                                         ===");
    LT_LOG("\t=======================================================================");


    LT_LOG_LINE();
    LT_LOG("\t Session with H0 keys:");
    if(session_H0() == -1)  {
        LT_LOG("Error during session_H0()");
    }

    LT_LOG_LINE();

    LT_LOG("\t End of execution, no errors.");

    return 0;
}
