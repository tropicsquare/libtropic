/**
* @file lt_test_chip_id.c
* @brief Test function which prints out CHIP ID
* @author Tropic Square s.r.o.
*
* @license For the license see file LICENSE.txt file in the root directory of this source tree.
*/
#include "string.h"
#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"

#define BUFF_SIZE 196

// Buffer for storing string with bytes
static char bytes_buffer[BUFF_SIZE];
static char* print_bytes(uint8_t *data, uint16_t len) {
    if((len > BUFF_SIZE) || (!data)) {
        memcpy(bytes_buffer, "error_str_decoding", 20);
        return bytes_buffer;
    }
    bytes_buffer[0] = '\0';
    for (uint16_t i = 0; i < len; i++) {
        char byte_str[4];
        snprintf(byte_str, sizeof(byte_str), "%02X", data[i]);
        // Check if appending the byte would exceed the buffer size
        if (strlen(bytes_buffer) + strlen(byte_str) + 1 > sizeof(bytes_buffer)) {
            break; // Stop if the buffer is full
        }
        strncat(bytes_buffer, byte_str, sizeof(bytes_buffer) - strlen(bytes_buffer) - 1);
    }

    return bytes_buffer;
}

// Buffer for storing string with version ID
static char buffer_chip_id_ver[32];
static char* interpret_chip_id_ver(uint8_t *ver) {
    if (!ver) {
        memcpy(buffer_chip_id_ver, "error_ver_str_decoding", 24);
        return buffer_chip_id_ver;
    }
    buffer_chip_id_ver[0] = '\0';
    snprintf(buffer_chip_id_ver, sizeof(buffer_chip_id_ver), "(v%d.%d.%d.%d)", ver[0], ver[1], ver[2], ver[3]);

    return buffer_chip_id_ver;
}

/**
 * @brief Function which prints out content of CHIP ID structure
 *
 * @param chip_id        Structure containing all fields of CHIP ID
 * @return lt_ret_t      LT_OK if success, otherwise returns other error code.
 */
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
    // Reserved 2B RFU

    // Provisioning info
    LT_LOG("\tProv info version      = %02X", chip_id->prov_ver_fab_id_pn[0]);
    LT_LOG("\tFab ID                 = %03X", (*chip_id->prov_ver_fab_id_pn >> 12) & 0xFFF);
    LT_LOG("\tP/N ID (short P/N)     = %02X", (*chip_id->prov_ver_fab_id_pn >> 24) & 0xFF);
    LT_LOG("\tProv date              = %s", print_bytes(chip_id->provisioning_date, sizeof(chip_id->provisioning_date)));
    LT_LOG("\tHSM HW/FW/SW version   = %s", print_bytes(chip_id->hsm_ver, sizeof(chip_id->hsm_ver)));
    LT_LOG("\tProgrammer version     = %s", print_bytes(chip_id->prog_ver, sizeof(chip_id->prog_ver)));
    // Reserved 2B RFU
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
    // Reserved 27B RFU

    return LT_OK;
}

int lt_test_chip_id(void)
{
    LT_LOG("  -------------------------------------------------------------------------------------------------------------");
    LT_LOG("  -------- lt_test_chip_id() ----------------------------------------------------------------------------------");
    LT_LOG("  -------------------------------------------------------------------------------------------------------------");

    lt_handle_t h = {0};

    lt_init(&h);

    // Read chip ID
    LT_LOG("lt_get_info_chip_id()");
    struct lt_chip_id_t chip_id = {0};
    LT_ASSERT(LT_OK, lt_get_info_chip_id(&h, &chip_id));

    LT_LOG("print_chip_id()");
    print_chip_id(&chip_id);

    // Deinit handle
    LT_LOG("%s", "lt_deinit()");
    LT_ASSERT(LT_OK, lt_deinit(&h));

    return 0;
}
