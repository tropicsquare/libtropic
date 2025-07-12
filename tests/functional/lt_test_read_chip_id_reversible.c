/**
 * @file lt_test_read_chip_id_reversible.c
 * @brief Test reading Chip ID and parse it.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */
#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "string.h"

#define BUFF_SIZE 196

/* Buffer for storing string containing hexadecimal bytes */
static char bytes_buffer[BUFF_SIZE];

/**
 * @brief Function to print bytes in hex format
 *
 * @param data      Pointer to data
 * @param len       Length of data
 * @return char*    Pointer to string with bytes in hex format
 */
static char* print_bytes(uint8_t* data, uint16_t len)
{
    if ((len > BUFF_SIZE) || (!data)) {
        memcpy(bytes_buffer, "Error print_bytes", 18);
        return bytes_buffer;
    }

    bytes_buffer[0] = '\0';
    for (uint16_t i = 0; i < len; i++) {
        char byte_str[4];
        snprintf(byte_str, sizeof(byte_str), "%02X", data[i]);
        // Check if appending the byte would exceed the buffer size
        if (strlen(bytes_buffer) + strlen(byte_str) + 1 > sizeof(bytes_buffer)) {
            break;  // Stop if the buffer is full
        }
        strncat(bytes_buffer, byte_str, sizeof(bytes_buffer) - strlen(bytes_buffer) - 1);
    }

    return bytes_buffer;
}

/* Buffer used by functions which interpret data */
static char buff_interpreted[64];

/**
 * @brief Function to interpret package ID
 *
 * @param pkg_type_id  Pointer to package ID data
 * @return char*    Pointer to string with interpreted package ID
 */
static char* interpret_pkg_type_id(uint8_t* pkg_type_id)
{
    if (!pkg_type_id) {
        memcpy(buff_interpreted, "Error decoding package_id", 26);
        return buff_interpreted;
    }

    if ((pkg_type_id[0] == 0x80) && (pkg_type_id[1] == 0xAA)) {
        memcpy(buff_interpreted, "QFN32, 4x4mm", 13);
    }
    else if ((pkg_type_id[0] == 0x80) && (pkg_type_id[1] == 0x00)) {
        memcpy(buff_interpreted, "Bare silicon die", 17);
    }
    else {
        memcpy(buff_interpreted, "Unknown type", 13);
    }

    return buff_interpreted;
}

/* Fab ID of Tropic Square Lab */
#define FAB_ID_TROPIC_SQUARE_LAB 0xF00
/* Fab ID of Production line #1 */
#define FAB_ID_EPS_BRNO 0x001
/**
 * @brief Function to interpret fabrication ID
 *
 * @param fab_id  Pointer to fabrication ID data
 * @return char*        Pointer to string with interpreted fabrication ID
 */
static char* interpret_fab_id(uint16_t fab_id)
{
    if ((fab_id == FAB_ID_TROPIC_SQUARE_LAB)) {
        memcpy(buff_interpreted, "Tropic Square Lab", 18);
    }
    else if ((fab_id == FAB_ID_EPS_BRNO)) {
        memcpy(buff_interpreted, "EPS Global - Brno", 18);
    }
    else {
        memcpy(buff_interpreted, "Unknown facility ID", 20);
    }

    return buff_interpreted;
}

/**
 * @brief Parses part number from `lt_part_num_t` (part of `lt_chip_id_t`).
 *
 * @param[in]  part_num Structure encapsulating Part Number
 * @param[out] res      Buffer for part number
 * @return     `lt_ret_t`
 */
lt_ret_t lt_get_part_number(struct lt_part_num_t* part_num, char* res)
{
    if (!part_num || !res || part_num->pn_len >= 0x0F) {
        return LT_PARAM_ERR;
    }

    for (uint8_t i = 0; i < part_num->pn_len; i++) {
        if ((part_num->pn_data[i] < 0x20) || (part_num->pn_data[i] > 0x7F)) {
            // TODO: new lt_ret_t value?
            return LT_PARAM_ERR;
        }
        res[i] = part_num->pn_data[i];
    }
    res[part_num->pn_len] = '\0';
    return LT_OK;
}

/**
 * @brief Parses Fabrication line ID from `lt_ser_num_t.fab_id_pn_id` or `lt_prov_info_t.fab_id_pn_id`.
 *
 * @param[in]  fab_id_pn_id Array with Fabrication line ID and Short Part Number ID
 * @param[out] fab_id       Parsed Fabrication line ID
 * @return     `lt_ret_t`
 */
lt_ret_t lt_get_fab_id(uint8_t* fab_id_pn_id, uint16_t* fab_id)
{
    if (!fab_id_pn_id || !fab_id) {
        return LT_PARAM_ERR;
    }

    *fab_id = (((fab_id_pn_id[0] << 8) | fab_id_pn_id[1]) & 0xFFF0) >> 4;
    return LT_OK;
}

/**
 * @brief Parses Short Part Number ID from `lt_ser_num_t.fab_id_pn_id` or `lt_prov_info_t.fab_id_pn_id`.
 *
 * @param[in]  fab_id_pn_id Array with Fabrication line ID and Short Part Number ID
 * @param[out] pn_id        Parsed Short Part Number ID
 * @return     `lt_ret_t`
 */
lt_ret_t lt_get_pn_id(uint8_t* fab_id_pn_id, uint16_t* pn_id)
{
    if (!fab_id_pn_id || !pn_id) {
        return LT_PARAM_ERR;
    }

    *pn_id = ((fab_id_pn_id[1] << 8) | fab_id_pn_id[2]) & 0x0FFF;
    return LT_OK;
}

/**
 * @brief Parses Year of testing from `lt_wafer_lvl_test_info_t`.
 *
 * @param[in]  lt_wafer_lvl_test_info Structure encapsulating Wafer level test info
 * @param[out] year                   Year of testing
 * @return     `lt_ret_t`
 */
lt_ret_t lt_get_fab_test_year(struct lt_wafer_lvl_test_info_t* lt_wafer_lvl_test_info, uint16_t* year)
{
    if (!lt_wafer_lvl_test_info || !year) {
        return LT_PARAM_ERR;
    }

    if (lt_wafer_lvl_test_info->fab_test_year[0] == 0 && lt_wafer_lvl_test_info->fab_test_year[1] == 0) {
        // TODO: new lt_ret_t value?
        *year = 0;
        return LT_OK;
    }

    *year = 2000 + (lt_wafer_lvl_test_info->fab_test_year[0] * 10) + lt_wafer_lvl_test_info->fab_test_year[1];
    return LT_OK;
}

/**
 * @brief Function which prints out content of CHIP ID structure and interprets some of fields
 *
 * @param[in] chip_id    Structure containing all fields of CHIP ID
 * @return    `lt_ret_t`
 */
static lt_ret_t print_chip_id(struct lt_chip_id_t* chip_id)
{
    if (!chip_id) {
        LT_LOG_INFO("Error: chip_id is NULL");
        return LT_PARAM_ERR;
    }

    if (chip_id->chip_id_ver[0] != 0x01) {
        LT_LOG_INFO("Error: Unsupported CHIP_ID version");
        return LT_FAIL;
    }

    uint16_t fab_id, pn_id, fab_test_year;
    char part_number[chip_id->part_num.pn_len + 1];

    LT_LOG_INFO("CHIP_ID structure versioning");
    LT_LOG_INFO("\tCHIP_ID version = 0x%s (v%d.%d.%d.%d)",
                print_bytes(chip_id->chip_id_ver, sizeof(chip_id->chip_id_ver)), chip_id->chip_id_ver[0],
                chip_id->chip_id_ver[1], chip_id->chip_id_ver[2], chip_id->chip_id_ver[3]);

    LT_LOG_INFO("Wafer level test info");
    LT_LOG_INFO("\tFactory Level Chip Info");
    LT_LOG_INFO("\t\tDevice test status = 0x%x", chip_id->wafer_lvl_test_info.fab_test_sts);
    LT_ASSERT(LT_OK, lt_get_fab_test_year(&chip_id->wafer_lvl_test_info, &fab_test_year));
    LT_LOG_INFO("\t\tYear of testing    = %d", fab_test_year);
    LT_LOG_INFO("\t\tMonth of testing   = %d", chip_id->wafer_lvl_test_info.fab_test_month);
    LT_LOG_INFO("\t\tDay of testing     = %d", chip_id->wafer_lvl_test_info.fab_test_day);
    LT_LOG_INFO("\t\tLOT ID             = 0x%s (\"%c%c%c%c%c\")",
                print_bytes(chip_id->wafer_lvl_test_info.fab_lot_id, sizeof(chip_id->wafer_lvl_test_info.fab_lot_id)),
                chip_id->wafer_lvl_test_info.fab_lot_id[0], chip_id->wafer_lvl_test_info.fab_lot_id[1],
                chip_id->wafer_lvl_test_info.fab_lot_id[2], chip_id->wafer_lvl_test_info.fab_lot_id[3],
                chip_id->wafer_lvl_test_info.fab_lot_id[4]);
    LT_LOG_INFO("\t\tWafer ID           = 0x%x", chip_id->wafer_lvl_test_info.fab_wafer_id);
    LT_LOG_INFO("\t\tX coord            = %d", chip_id->wafer_lvl_test_info.fab_x_coord);
    LT_LOG_INFO("\t\tY coord            = %d", chip_id->wafer_lvl_test_info.fab_y_coord);

    LT_LOG_INFO("Manufacturing level test info");
    LT_LOG_INFO("\tFunction test Info");
    LT_LOG_INFO("\t\tFunctional test status = 0x%x", chip_id->manuf_lvl_test_info.func_test_info.func_test_sts);
    LT_LOG_INFO("\t\tFunctional test ID     = 0x%s (\"%c%c%c%c\")",
                print_bytes(chip_id->manuf_lvl_test_info.func_test_info.func_test_id,
                            sizeof(chip_id->manuf_lvl_test_info.func_test_info.func_test_id)),
                chip_id->manuf_lvl_test_info.func_test_info.func_test_id[0],
                chip_id->manuf_lvl_test_info.func_test_info.func_test_id[1],
                chip_id->manuf_lvl_test_info.func_test_info.func_test_id[2],
                chip_id->manuf_lvl_test_info.func_test_info.func_test_id[3]);
    LT_LOG_INFO(
        "\tSilicon Revision ID = 0x%s (\"%c%c%c%c\")",
        print_bytes(chip_id->manuf_lvl_test_info.silicon_rev_id, sizeof(chip_id->manuf_lvl_test_info.silicon_rev_id)),
        chip_id->manuf_lvl_test_info.silicon_rev_id[0], chip_id->manuf_lvl_test_info.silicon_rev_id[1],
        chip_id->manuf_lvl_test_info.silicon_rev_id[2], chip_id->manuf_lvl_test_info.silicon_rev_id[3]);
    LT_LOG_INFO("\tPackage Type ID     = 0x%s (\"%s\")",
                print_bytes(chip_id->manuf_lvl_test_info.pkg_type_id, sizeof(chip_id->manuf_lvl_test_info.pkg_type_id)),
                interpret_pkg_type_id(chip_id->manuf_lvl_test_info.pkg_type_id));

    LT_LOG_INFO("Provisioning info");
    LT_LOG_INFO("\tProv Info Version  = 0x%s",
                print_bytes(&chip_id->prov_info.prov_info_ver, sizeof(chip_id->prov_info.prov_info_ver)));
    LT_ASSERT(LT_OK, lt_get_fab_id(chip_id->prov_info.fab_id_pn_id, &fab_id));
    LT_LOG_INFO("\tFab ID             = 0x%x (\"%s\")", fab_id, interpret_fab_id(fab_id));
    LT_ASSERT(LT_OK, lt_get_pn_id(chip_id->prov_info.fab_id_pn_id, &pn_id));
    LT_LOG_INFO("\tP/N ID             = 0x%x", pn_id);
    LT_LOG_INFO("\tProvisioning date  = 0x%s",
                print_bytes(chip_id->prov_info.prov_date, sizeof(chip_id->prov_info.prov_date)));
    LT_LOG_INFO("\tHSM version        = 0x%s (v%d.%d.%d)",
                print_bytes(chip_id->prov_info.hsm_ver, sizeof(chip_id->prov_info.hsm_ver)),
                chip_id->prov_info.hsm_ver[1], chip_id->prov_info.hsm_ver[2], chip_id->prov_info.hsm_ver[3]);
    LT_LOG_INFO("\tProgrammer version = 0x%s",
                print_bytes(chip_id->prov_info.programmer_ver, sizeof(chip_id->prov_info.programmer_ver)));

    LT_LOG_INFO("Serial Number");
    LT_LOG_INFO("\tS/N ver  = 0x%s", print_bytes(&chip_id->ser_num.sn_ver, sizeof(chip_id->ser_num.sn_ver)));
    LT_ASSERT(LT_OK, lt_get_fab_id(chip_id->ser_num.fab_id_pn_id, &fab_id));
    LT_LOG_INFO("\tFab ID   = 0x%x (\"%s\")", fab_id, interpret_fab_id(fab_id));
    LT_ASSERT(LT_OK, lt_get_pn_id(chip_id->ser_num.fab_id_pn_id, &pn_id));
    LT_LOG_INFO("\tP/N ID   = 0x%x", pn_id);
    LT_LOG_INFO("\tFab date = 0x%s", print_bytes(chip_id->ser_num.fab_date, sizeof(chip_id->ser_num.fab_date)));
    LT_LOG_INFO("\tLOT ID   = 0x%s (%c%c%c%c%c)", print_bytes(chip_id->ser_num.lot_id, sizeof(chip_id->ser_num.lot_id)),
                chip_id->ser_num.lot_id[0], chip_id->ser_num.lot_id[1], chip_id->ser_num.lot_id[2],
                chip_id->ser_num.lot_id[3], chip_id->ser_num.lot_id[4]);
    LT_LOG_INFO("\tWafer ID = %d", chip_id->ser_num.wafer_id);
    LT_LOG_INFO("\tX coord  = %d", chip_id->ser_num.x_coord);
    LT_LOG_INFO("\tY coord  = %d", chip_id->ser_num.y_coord);

    LT_LOG_INFO("Part Number");
    LT_LOG_INFO("\tP/N length = 0x%s", print_bytes(&chip_id->part_num.pn_len, sizeof(chip_id->part_num.pn_len)));
    LT_ASSERT(LT_OK, lt_get_part_number(&chip_id->part_num, part_number));
    LT_LOG_INFO("\tP/N data   = 0x%s (\"%s\")",
                print_bytes(chip_id->part_num.pn_data, sizeof(chip_id->part_num.pn_data)), part_number);

    LT_LOG_INFO("Provisioning Data version");
    LT_LOG_INFO("\tProvisioning Template Version      = 0x%s (v%d.%d)",
                print_bytes(chip_id->prov_data_ver.prov_template_ver, sizeof(chip_id->prov_data_ver.prov_template_ver)),
                chip_id->prov_data_ver.prov_template_ver[0], chip_id->prov_data_ver.prov_template_ver[1]);
    LT_LOG_INFO(
        "\tProvisioning Template Tag          = 0x%s",
        print_bytes(chip_id->prov_data_ver.prov_template_tag, sizeof(chip_id->prov_data_ver.prov_template_tag)));
    LT_LOG_INFO("\tProvisioning Specification Version = 0x%s (v%d.%d)",
                print_bytes(chip_id->prov_data_ver.prov_spec_ver, sizeof(chip_id->prov_data_ver.prov_spec_ver)),
                chip_id->prov_data_ver.prov_spec_ver[0], chip_id->prov_data_ver.prov_spec_ver[1]);
    LT_LOG_INFO("\tProvisioning Specification Tag     = 0x%s",
                print_bytes(chip_id->prov_data_ver.prov_spec_tag, sizeof(chip_id->prov_data_ver.prov_spec_tag)));
    LT_LOG_INFO("\tBatch ID                           = 0x%s",
                print_bytes(chip_id->prov_data_ver.batch_id, sizeof(chip_id->prov_data_ver.batch_id)));

    return LT_OK;
}

void lt_test_read_chip_id_reversible(void)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_read_chip_id_reversible()");
    LT_LOG_INFO("----------------------------------------------");

    lt_handle_t h = {0};
#if LT_SEPARATE_L3_BUFF
    uint8_t l3_buffer[L3_FRAME_MAX_SIZE] __attribute__((aligned(16))) = {0};
    h.l3.buff = l3_buffer;
    h.l3.buff_len = sizeof(l3_buffer);
#endif
    struct lt_chip_id_t chip_id;

    LT_LOG_INFO("Initializing handle");
    LT_ASSERT(LT_OK, lt_init(&h));

    LT_LOG_INFO("Reading Chip ID:");
    LT_ASSERT(LT_OK, lt_get_info_chip_id(&h, &chip_id));
    LT_ASSERT(LT_OK, print_chip_id(&chip_id));

    LT_LOG_INFO("Deinitializing handle");
    LT_ASSERT(LT_OK, lt_deinit(&h));
}
