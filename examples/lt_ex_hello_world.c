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

 static void print_bytes(uint8_t *data, uint16_t len) {
    char buffer[256] = {0};
    for (uint16_t i = 0; i < len; i++) {
        char byte_str[4];
        snprintf(byte_str, sizeof(byte_str), "%02X ", data[i]);
        strncat(buffer, byte_str, sizeof(buffer) - strlen(buffer) - 1);

        // Print the buffer every 32 bytes or at the end of the data
        if ((i + 1) % 32 == 0 || i == len - 1) {
            LT_LOG_INFO("%s", buffer);
            buffer[0] = '\0'; // Clear the buffer for the next line
        }
    }
}

static void print_chip_id(struct lt_chip_id_t *chip_id) {
    if (!chip_id) {
        LT_LOG("Error: chip_id is NULL");
        return;
    }

    LT_LOG("Chip ID Details:");

    // CHIP_ID structure versioning
    LT_LOG("\tCHIP_ID Version:         0x%08X", chip_id->chip_id_ver);

    // Wafer level test info
    LT_LOG("\tWafer Level Test Info:");
    print_bytes(chip_id->fl_chip_info, sizeof(chip_id->fl_chip_info));

    // Manufacturing level test info
    LT_LOG("\tManufacturing Level Test Info:");
    print_bytes(chip_id->func_test_info, sizeof(chip_id->func_test_info));

    // Silicon revision
    LT_LOG("\tSilicon Revision:        0x%02X%02X%02X%02X",
           chip_id->silicon_rev[0], chip_id->silicon_rev[1],
           chip_id->silicon_rev[2], chip_id->silicon_rev[3]);

    // Package type ID
    LT_LOG("\tPackage Type ID:         0x%02X%02X",
           chip_id->packg_type_id[0], chip_id->packg_type_id[1]);

    // Reserved field 1
    LT_LOG("\tReserved1:               0x%04X", chip_id->reserved1);

    // Provisioning info
    LT_LOG("\tProvisioning Info Version: 0x%02X", (chip_id->prov_ver_fab_id_pn >> 24) & 0xFF);
    LT_LOG("\tFabrication ID:          0x%03X", (chip_id->prov_ver_fab_id_pn >> 12) & 0xFFF);
    LT_LOG("\tPart Number ID:          0x%03X", chip_id->prov_ver_fab_id_pn & 0xFFF);
    LT_LOG("\tProvisioning Date:       0x%04X", chip_id->provisioning_date);
    LT_LOG("\tHSM Version:             0x%08X", chip_id->hsm_ver);
    LT_LOG("\tProgram Version:         0x%08X", chip_id->prog_ver);
    LT_LOG("\tReserved2:               0x%04X", chip_id->reserved2);

    // Serial Number
    LT_LOG("\tSerial Number:");
    LT_LOG("\t\tSerial Number (SN):      0x%02X", chip_id->ser_num.sn);
    uint16_t fab_id = ((chip_id->ser_num.fab_data[0] << 4) | (chip_id->ser_num.fab_data[1] >> 4));
    LT_LOG("\t\tFabrication ID (Fab ID): 0x%03X", fab_id);
    uint16_t part_num_id = ((chip_id->ser_num.fab_data[1] << 4) | (chip_id->ser_num.fab_data[2] >> 4));
    LT_LOG("\t\tPart Number ID (PN ID):  0x%03X", part_num_id);
    LT_LOG("\t\tFabrication Date:        0x%04X", chip_id->ser_num.fab_date);
    LT_LOG("\t\tLot ID:                  0x%02X%02X%02X%02X%02X",
           chip_id->ser_num.lot_id[0], chip_id->ser_num.lot_id[1],
           chip_id->ser_num.lot_id[2], chip_id->ser_num.lot_id[3],
           chip_id->ser_num.lot_id[4]);
    LT_LOG("\t\tWafer ID:                0x%02X", chip_id->ser_num.wafer_id);
    LT_LOG("\t\tX Coordinate:            0x%04X", chip_id->ser_num.x_coord);
    LT_LOG("\t\tY Coordinate:            0x%04X", chip_id->ser_num.y_coord);

    // Part Number
    LT_LOG("\tPart Number Length:      %d", chip_id->part_num_len);
    LT_LOG("\tPart Number Data:");
    print_bytes(chip_id->part_num_data, chip_id->part_num_len);

    // Provisioning Data version
    LT_LOG("\tProvisioning Template Version: 0x%04X", chip_id->prov_templ_ver);
    LT_LOG("\tProvisioning Template Tag:     0x%08X", chip_id->prov_templ_tag);
    LT_LOG("\tProvisioning Spec Version:     0x%04X", chip_id->prov_spec_ver);
    LT_LOG("\tProvisioning Spec Tag:         0x%08X", chip_id->prov_spec_tag);

    // Batch ID
    LT_LOG("\tBatch ID:                0x%02X%02X%02X%02X%02X",
           chip_id->batch_id[0], chip_id->batch_id[1],
           chip_id->batch_id[2], chip_id->batch_id[3],
           chip_id->batch_id[4]);

    // Reserved fields
    LT_LOG("\tReserved3:");
    print_bytes(chip_id->reserved3, sizeof(chip_id->reserved3));

    // Padding
    LT_LOG("\tPadding:");
    print_bytes(chip_id->padding, sizeof(chip_id->padding));
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
