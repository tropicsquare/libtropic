#include <stdio.h>

#include "libtropic.h"
#include "utils.h"

struct lt_config_obj_t {
    char desc[60];
    enum CONFIGURATION_OBJECTS_REGS reg;
    union config_obj config;
};

struct lt_config_obj_t config_table[] = {
    {"CONFIGURATION_OBJECTS_CFG_START_UP_ADDR                   ", CONFIGURATION_OBJECTS_CFG_START_UP_ADDR, 0},
    {"CONFIGURATION_OBJECTS_CFG_SLEEP_MODE_ADDR                 ", CONFIGURATION_OBJECTS_CFG_SLEEP_MODE_ADDR, 0},
    {"CONFIGURATION_OBJECTS_CFG_SENSORS_ADDR                    ", CONFIGURATION_OBJECTS_CFG_SENSORS_ADDR, 0},
    {"CONFIGURATION_OBJECTS_CFG_DEBUG_ADDR                      ", CONFIGURATION_OBJECTS_CFG_DEBUG_ADDR, 0},
    {"CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_WRITE_ADDR      ", CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_WRITE_ADDR, 0},
    {"CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_READ_ADDR       ", CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_READ_ADDR, 0},
    {"CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_INVALIDATE_ADDR ", CONFIGURATION_OBJECTS_CFG_UAP_PAIRING_KEY_INVALIDATE_ADDR, 0},
    {"CONFIGURATION_OBJECTS_CFG_UAP_R_CONFIG_WRITE_ERASE_ADDR   ", CONFIGURATION_OBJECTS_CFG_UAP_R_CONFIG_WRITE_ERASE_ADDR, 0},
    {"CONFIGURATION_OBJECTS_CFG_UAP_R_CONFIG_READ_ADDR          ", CONFIGURATION_OBJECTS_CFG_UAP_R_CONFIG_READ_ADDR, 0},
    {"CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_WRITE_ADDR         ", CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_WRITE_ADDR, 0},
    {"CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_READ_ADDR          ", CONFIGURATION_OBJECTS_CFG_UAP_I_CONFIG_READ_ADDR, 0},
    {"CONFIGURATION_OBJECTS_CFG_UAP_PING_ADDR                   ", CONFIGURATION_OBJECTS_CFG_UAP_PING_ADDR, 0},
    {"CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_WRITE_ADDR       ", CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_WRITE_ADDR, 0},
    {"CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_READ_ADDR        ", CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_READ_ADDR, 0},
    {"CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_ERASE_ADDR       ", CONFIGURATION_OBJECTS_CFG_UAP_R_MEM_DATA_ERASE_ADDR, 0},
    {"CONFIGURATION_OBJECTS_CFG_UAP_RANDOM_VALUE_GET_ADDR       ", CONFIGURATION_OBJECTS_CFG_UAP_RANDOM_VALUE_GET_ADDR, 0},
    {"CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_GENERATE_ADDR       ", CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_GENERATE_ADDR, 0},
    {"CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_STORE_ADDR          ", CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_STORE_ADDR, 0},
    {"CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_READ_ADDR           ", CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_READ_ADDR, 0},
    {"CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_ERASE_ADDR          ", CONFIGURATION_OBJECTS_CFG_UAP_ECC_KEY_ERASE_ADDR, 0},
    {"CONFIGURATION_OBJECTS_CFG_UAP_ECDSA_SIGN_ADDR             ", CONFIGURATION_OBJECTS_CFG_UAP_ECDSA_SIGN_ADDR, 0},
    {"CONFIGURATION_OBJECTS_CFG_UAP_EDDSA_SIGN_ADDR             ", CONFIGURATION_OBJECTS_CFG_UAP_EDDSA_SIGN_ADDR, 0},
    {"CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_INIT_ADDR          ", CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_INIT_ADDR, 0},
    {"CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_GET_ADDR           ", CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_GET_ADDR, 0},
    {"CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_UPDATE_ADDR        ", CONFIGURATION_OBJECTS_CFG_UAP_MCOUNTER_UPDATE_ADDR, 0},
    {"CONFIGURATION_OBJECTS_CFG_UAP_MAC_AND_DESTROY_ADDR        ", CONFIGURATION_OBJECTS_CFG_UAP_MAC_AND_DESTROY_ADDR, 0},
    {"CONFIGURATION_OBJECTS_CFG_UAP_SERIAL_CODE_GET_ADDR        ", CONFIGURATION_OBJECTS_CFG_UAP_SERIAL_CODE_GET_ADDR, 0}
};

lt_ret_t libtropic_example___show_config_objects(lt_handle_t *h)
{
    LOG_OUT_INFO("Read sensors r config object: :\r\n");
    LOG_OUT_LINE();
    LOG_OUT_INFO("CONFIG OBJECTS;\r\n");

    for (int i=0; i<27;i++) {

        lt_ret_t ret = lt_r_config_read(h, config_table[i].reg, &config_table[i].config);
        if(ret == LT_OK) {
            LOG_OUT_LINE();
        } else {
            LOG_OUT_VALUE("lt_ret_t:      %s\r\n", lt_ret_verbose(ret));
            ret = lt_deinit(h);
            return 1;
        }
        /* Following lines only print out some debug: */
        char config_str[8+1] = {0};
        bytes_to_chars(config_table[i].config.byte, config_str, 4);
        printf("\t\t\t\t\t%s  %s\r\n", config_table[i].desc, config_str);
    }
    LOG_OUT_LINE();

    union config_obj conf;
    conf.word = 0x11223344;
    for (int i=0; i<27;i++) {
        lt_ret_t ret = lt_r_config_write(h, config_table[i].reg, &conf);
        if(ret == LT_OK) {
            LOG_OUT_LINE();
        } else {
            LOG_OUT_VALUE("lt_ret_t:      %s\r\n", lt_ret_verbose(ret));
            ret = lt_deinit(h);
            return 1;
        }
    }

    LOG_OUT_LINE();

    for (int i=0; i<27;i++) {

        lt_ret_t ret = lt_r_config_read(h, config_table[i].reg, &config_table[i].config);
        if(ret == LT_OK) {
            LOG_OUT_LINE();
        } else {
            LOG_OUT_VALUE("lt_ret_t:      %s\r\n", lt_ret_verbose(ret));
            ret = lt_deinit(h);
            return 1;
        }
        /* Following lines only print out some debug: */
        char config_str[8+1] = {0};
        bytes_to_chars(config_table[i].config.byte, config_str, 4);
        printf("\t\t\t\t\t%s  %s\r\n", config_table[i].desc, config_str);
    }
}
