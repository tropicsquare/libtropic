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

/* Buffer for storing string containing hexadecimal bytes */
static char bytes_buffer[BUFF_SIZE];

/**
 * @brief Function to print bytes in hex format
 *
 * @param data      Pointer to data
 * @param len       Length of data
 * @return char*    Pointer to string with bytes in hex format
 */
static char* print_bytes(uint8_t *data, uint16_t len) {
    if((len > BUFF_SIZE) || (!data)) {
        memcpy(bytes_buffer, "Error print_bytes", 18);
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

/* Buffer used by functions which interpret data */
static char buff_interpreted[64];

/**
 * @brief Function to interpret chip ID version
 *
 * @param ver     Pointer to version data
 * @return char*   Pointer to string with interpreted version
 */
static char* interpret_chip_id_ver(uint8_t *ver) {
    if (!ver) {
        memcpy(buff_interpreted, "Error decoding chip_id ver", 27);
        return buff_interpreted;
    }

    buff_interpreted[0] = '\0';
    snprintf(buff_interpreted, sizeof(buff_interpreted), "(v%d.%d.%d.%d)", ver[0], ver[1], ver[2], ver[3]);

    return buff_interpreted;
}

/**
 * @brief Function to interpret fl chip info
 *
 * @param ver     Pointer to version data
 * @return char*   Pointer to string with interpreted version
 */
static char* interpret_fl_chip_info(uint8_t *fl_chip_info) {
    if (!fl_chip_info) {
        memcpy(buff_interpreted, "Error decoding fl_chip_info", 28);
        return buff_interpreted;
    }

    if(fl_chip_info[0] == 0x01) {
        memcpy(buff_interpreted, "(\"PASSED\")", 11);
    }  else {
        memcpy(buff_interpreted, "(\"N/A\")", 8);
    }
    return buff_interpreted;
}

/**
 * @brief Function to interpret function test info
 *
 * @param func_test_info  Pointer to "function test info" data
 * @return char*          Pointer to string with interpreted "function test info"
 */
static char* interpret_func_test_info(uint8_t *func_test_info) {
    if (!func_test_info) {
        memcpy(buff_interpreted, "Error decoding func_test_info", 30);
        return buff_interpreted;
    }

    if(func_test_info[0] == 0x01) {
        memcpy(buff_interpreted, "(\"PASSED\")", 11);
    }  else {
        memcpy(buff_interpreted, "(\"N/A\")", 8);
    }

    return buff_interpreted;
}

/**
 * @brief Function to interpret silicon revision
 *
 * @param rev     Pointer to silicon revision data
 * @return char*  Pointer to string with interpreted silicon revision
 */
static char* interpret_silicon_rev(uint8_t *rev) {
    if (!rev) {
        memcpy(buff_interpreted, "Error decoding silicon_rev", 27);
        return buff_interpreted;
    }

    buff_interpreted[0] = '\0';
    if(((rev[0] > 0x20 ) && (rev[0] < 0x7f)) &&
       ((rev[1] > 0x20 ) && (rev[1] < 0x7f)) &&
       ((rev[2] > 0x20 ) && (rev[2] < 0x7f)) &&
       ((rev[3] > 0x20 ) && (rev[3] < 0x7f))) {
        snprintf(buff_interpreted, sizeof(buff_interpreted), "(\"%c%c%c%c\")", rev[0], rev[1], rev[2], rev[3]);
    } else {
        memcpy(buff_interpreted, "Invalid character detected", 27);
    }

    return buff_interpreted;
}

/**
 * @brief Function to interpret package ID
 *
 * @param packg_id  Pointer to package ID data
 * @return char*    Pointer to string with interpreted package ID
 */
static char* interpret_packg_id(uint8_t *packg_id) {
    if (!packg_id) {
        memcpy(buff_interpreted, "Error decoding package_id", 26);
        return buff_interpreted;
    }

    if((packg_id[0] == 0x80) && (packg_id[1] == 0xAA)) {
        memcpy(buff_interpreted, "(\"QFN32, 4x4mm\")", 17);
    } else if ((packg_id[0] == 0x80) && (packg_id[1] == 0x00)) {
        memcpy(buff_interpreted, "(\"Bare silicon die\")", 21);
    } else {
        memcpy(buff_interpreted, "Unknown type", 13);
    }

    return buff_interpreted;
}

/**
 * @brief Function to interpret provisioning info version
 *
 * @param ver     Pointer to provisioning info version data
 * @return char*  Pointer to string with interpreted provisioning info version
 */
static char* interpret_prov_info_ver(uint8_t *ver) {
    if (!ver) {
        memcpy(buff_interpreted, "Error decoding prov_info_ver", 29);
        return buff_interpreted;
    }

    buff_interpreted[0] = '\0';
    snprintf(buff_interpreted, sizeof(buff_interpreted), "(v%d)", ver[0]);

    return buff_interpreted;
}

/* Fab ID of Tropic Square Lab */
#define FAB_ID_TROPIC_SQUARE_LAB 0xF00
/* Fab ID of Production line #1 */
#define FAB_ID_PROD_LINE_1 0x001
/**
 * @brief Function to interpret fabrication ID
 *
 * @param fab_id_value  Pointer to fabrication ID data
 * @return char*        Pointer to string with interpreted fabrication ID
 */
static char* interpret_fab_id(uint16_t fab_id_value) {

    if((fab_id_value == FAB_ID_TROPIC_SQUARE_LAB)) {
        memcpy(buff_interpreted, "(\"Tropic Square Lab\")", 22);
    } else if((fab_id_value == FAB_ID_PROD_LINE_1)) {
        memcpy(buff_interpreted, "(\"Production line #1\")", 23);
    } else {
        memcpy(buff_interpreted, "Unknown facility ID", 20);
    }

    return buff_interpreted;
}

/**
 * @brief Function to interpret part number
 * @param prov_date   Pointer to part number data
 * @return char*      Pointer to string with interpreted part number
 */
static char* interpret_prov_date(uint8_t *prov_date) {
    if (!prov_date) {
        memcpy(buff_interpreted, "Error decoding prov_date", 25);
        return buff_interpreted;
    }

    // Deconstruct 16bit value expected by decoding algorithm
    uint16_t date_value = ((prov_date[0] << 8) | (prov_date[1] ));
    // Prepare constant values used in decoding algorithm
    uint8_t days_in_months_standard_year[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    uint8_t days_in_months_leap_year[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    char* month_names[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

    // Get the year, 2023 is "year base"
    uint16_t year = (date_value / 1000) + 2023;

    // Based on the year decide what array of days in months to use
    uint8_t *p_days = NULL;
    if((year % 4) == 0) {
        p_days = days_in_months_leap_year;
    } else {
        p_days = days_in_months_standard_year;
    }

    // Get on which day of that year was TROPIC01 provisioned
    int16_t days_in_year = date_value % 1000;

    // Take previous value and calculate particular month and day
    int16_t month = 0;
    int16_t day = 0;
    for(int i=0; i<12; i++) {
        days_in_year = days_in_year - p_days[i];
        if(days_in_year <= 0) {
            month = i;
            day = days_in_year + p_days[i];
            break;
        }
    }

    snprintf(buff_interpreted, sizeof(buff_interpreted), "(\"%s %d, %d\")", month_names[month], day, year);

    return buff_interpreted;
}

/**
 * @brief Function to interpret part number
 *
 * @param pn_long   Pointer to part number data
 * @param len       Length of part number data
 * @return char*    Pointer to string with interpreted part number
 */
static char* interpret_pn_long(uint8_t *pn_long) {
    if (!pn_long) {
        memcpy(buff_interpreted, "Error decoding pn_long", 23);
        return buff_interpreted;
    }

    uint8_t len = pn_long[0];
    if (len < 0x0F) {
        // Now check if all are valid ASCII characters:
        for(uint8_t i = 1; i< len; i++) {
            if((pn_long[i] < 0x20) || (pn_long[i] > 0x7F)) {
                memcpy(buff_interpreted, "Invalid character detected", 27);
                return buff_interpreted;
            }
        }

        // Looks all good, prepare data like a string:
        memcpy(buff_interpreted, "(\"", 2);
        memcpy(buff_interpreted + 2, pn_long+1, len);
        memcpy(buff_interpreted + 2 + pn_long[0], "\")", 3);
    } else {
        memcpy(buff_interpreted, "No interpretation available", 28);
    }

    return buff_interpreted;
}

/**
 * @brief Function to interpret provisioning template version
 *
 * @param prov_temp_ver  Pointer to provisioning template version data
 * @return char*         Pointer to string with interpreted provisioning template version
 */
static char* interpret_prov_temp_ver(uint8_t *prov_temp_ver) {
    if (!prov_temp_ver) {
        memcpy(buff_interpreted, "Error decoding prov_temp_ver", 29);
        return buff_interpreted;
    }

    buff_interpreted[0] = '\0';
    snprintf(buff_interpreted, sizeof(buff_interpreted), "(v%d.%d)", prov_temp_ver[0], prov_temp_ver[1]);

    return buff_interpreted;
}

/**
 * @brief Function to interpret version of provisioning specification
 *
 * @param prov_spec_ver  Pointer to data containing version of provisioning specification
 * @return char*         Pointer to string with interpreted provisioning specification version
 */
static char* interpret_prov_spec_ver(uint8_t *prov_spec_ver) {
    if (!prov_spec_ver) {
        memcpy(buff_interpreted, "Error decoding prov_spec_ver", 29);
        return buff_interpreted;
    }

    buff_interpreted[0] = '\0';
    snprintf(buff_interpreted, sizeof(buff_interpreted), "(v%d.%d)", prov_spec_ver[0], prov_spec_ver[1]);

    return buff_interpreted;
}

/**
 * @brief Function to test interpretation of provisioning date data, uses known values.
 *
 *   Expected:
 *      754A    January 26, 2053
 *      07DA    January 10, 2025
 *      0BE0    February 9, 2026
 *      0032    February 19, 2023
 *      0C0D    March 26, 2026
 *      B7E2    March 15, 2070
 *      BFCF    April 12, 2072
 *      238F    April 12, 2032
 *      3359    May 24, 2036
 *      5E43    May 11, 2047
 *      A4B0    June 9, 2065
 *      E728    June 25, 2082
 *      C033    July 21, 2072
 *      A8BA    July 13, 2066
 *      C81C    August 16, 2074
 *      0C8F    August 3, 2026
 *      B0CF    September 19, 2068
 *      10AC    September 25, 2027
 *      2056    October 5, 2031
 *      C098    October 30, 2072
 *      4F61    November 17, 2043
 *      0138    November 8, 2023
 *      A948    December 2, 2066
 *      2C59    December 19, 2034
 */
void test_provisioning_date_decoding(void) {
   LT_LOG("\tTest decode static \"provisioning date\" values:");
   uint8_t testdata[] = {0x75, 0x4a, 0x07, 0xda, 0x0b, 0xe0, 0x00, 0x32, 0x0c, 0x0d, 0xb7, 0xe2, 0xbf, 0xcf, 0x23, 0x8f,
                         0x33, 0x59, 0x5e, 0x43, 0xa4, 0xb0, 0xe7, 0x28, 0xc0, 0x33, 0xa8, 0xba, 0xc8, 0x1c, 0x0c, 0x8f,
                         0xb0, 0xcf, 0x10, 0xac, 0x20, 0x56, 0xc0, 0x98, 0x4f, 0x61, 0x01, 0x38, 0xa9, 0x48, 0x2c, 0x59};
   for(int i=0; i<24;i++) {
       LT_LOG("\tProv date              = %s    %s"        , print_bytes(&testdata[2*i], 2), interpret_prov_date(&testdata[2*i]));
   }
}

/**
 * @brief Function which prints out content of CHIP ID structure and interprets some of fields
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

    LT_LOG("\tCHIP_ID version        = %s\t\t\t\t%s"    , print_bytes(chip_id->chip_id_ver, sizeof(chip_id->chip_id_ver)),         interpret_chip_id_ver(chip_id->chip_id_ver));
    LT_LOG("\tFL_PROD_DATA           = %s\t%s"      , print_bytes(chip_id->fl_chip_info, sizeof(chip_id->fl_chip_info)),           interpret_fl_chip_info(chip_id->fl_chip_info));
    LT_LOG("\tMAN_FUNC_TEST          = %s\t\t\t%s"      , print_bytes(chip_id->func_test_info, sizeof(chip_id->func_test_info)),   interpret_func_test_info(chip_id->func_test_info));
    LT_LOG("\tSilicon Rev            = %s\t\t\t\t%s"    , print_bytes(chip_id->silicon_rev, sizeof(chip_id->silicon_rev)),         interpret_silicon_rev(chip_id->silicon_rev));
    LT_LOG("\tPackage ID             = %s\t\t\t\t\t%s"  , print_bytes(chip_id->packg_type_id, sizeof(chip_id->packg_type_id)),     interpret_packg_id(chip_id->packg_type_id));
    // Reserved 2B RFU
    LT_LOG("\tProv info version      = %02X\t\t\t\t\t%s", chip_id->prov_ver_fab_id_pn[0],                                          interpret_prov_info_ver(chip_id->prov_ver_fab_id_pn));
    uint16_t parsed_fab_id = ((chip_id->prov_ver_fab_id_pn[1] << 4) | (chip_id->prov_ver_fab_id_pn[2] >> 4)) & 0xfff;
    LT_LOG("\tFab ID                 = %03X\t\t\t\t\t%s", parsed_fab_id, interpret_fab_id(parsed_fab_id));
    uint16_t parsed_short_pn = ((chip_id->prov_ver_fab_id_pn[2] << 8) | (chip_id->prov_ver_fab_id_pn[3])) & 0xfff;
    LT_LOG("\tP/N ID (short P/N)     = %03X"    , parsed_short_pn);
    LT_LOG("\tProv date              = %s\t\t\t\t\t%s"        , print_bytes(chip_id->provisioning_date, sizeof(chip_id->provisioning_date)), interpret_prov_date(chip_id->provisioning_date));
    LT_LOG("\tHSM HW/FW/SW version   = %s", print_bytes(chip_id->hsm_ver, sizeof(chip_id->hsm_ver)));
    LT_LOG("\tProgrammer version     = %s", print_bytes(chip_id->prog_ver, sizeof(chip_id->prog_ver)));
    // Reserved 2B RFU
    LT_LOG("\tS/N                    = %s"              , print_bytes((uint8_t*)&chip_id->ser_num, sizeof(chip_id->ser_num)));
    LT_LOG("\tP/N (long)             = %s\t%s"        , print_bytes(chip_id->part_num_data, sizeof(chip_id->part_num_data)),       interpret_pn_long(chip_id->part_num_data));
    LT_LOG("\tProv template version: = %s\t\t\t\t\t%s"  , print_bytes(chip_id->prov_templ_ver, sizeof(chip_id->prov_templ_ver)),   interpret_prov_temp_ver(chip_id->prov_templ_ver));
    LT_LOG("\tProv template tag:     = %s"              , print_bytes(chip_id->prov_templ_tag, sizeof(chip_id->prov_templ_tag)));
    LT_LOG("\tProv spec version:     = %s\t\t\t\t\t%s"  , print_bytes(chip_id->prov_spec_ver, sizeof(chip_id->prov_spec_ver)),     interpret_prov_spec_ver(chip_id->prov_spec_ver));
    LT_LOG("\tProv spec tag:         = %s"              , print_bytes(chip_id->prov_spec_tag, sizeof(chip_id->prov_spec_tag)));
    LT_LOG("\tBatch ID:              = %s"              , print_bytes(chip_id->batch_id, sizeof(chip_id->batch_id)));
    // Reserved 27B RFU

    //test_provisioning_date_decoding();

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
