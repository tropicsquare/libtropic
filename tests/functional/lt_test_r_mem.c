/**
 * @file lt_test_r_mem.c
 * @brief Test R_Mem_Data_* commands on all User Data slots
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */
#include "inttypes.h"
#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "string.h"

/**
 * @brief
 *
 * @return int
 */
int lt_test_r_mem(void)
{
    LT_LOG(
        "  "
        "------------------------------------------------------------------------------------------------------------"
        "-");
    LT_LOG(
        "  -------------- lt_test_r_mem()  "
        "-----------------------------------------------------------------------------");
    LT_LOG(
        "  "
        "------------------------------------------------------------------------------------------------------------"
        "-");

    LT_LOG("TODO: Redo this test after fix in FW.");
    LT_ASSERT(0, 1);

    lt_handle_t h = {0};
#if LT_SEPARATE_L3_BUFF
    uint8_t l3_buffer[L3_FRAME_MAX_SIZE] __attribute__((aligned(16))) = {0};
    h.l3.buff = l3_buffer;
    h.l3.buff_len = sizeof(l3_buffer);
#endif
    uint8_t r_mem_data[R_MEM_DATA_SIZE_MAX];
    uint8_t r_mem_data_expected[R_MEM_DATA_SIZE_MAX];

    LT_LOG("%s", "Initialize handle");
    LT_ASSERT(LT_OK, lt_init(&h));

    // Start secure session with SH0
    LT_LOG("%s with %d", "verify_chip_and_start_secure_session()", PAIRING_KEY_SLOT_INDEX_0);
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, PAIRING_KEY_SLOT_INDEX_0));

    LT_LOG("---------------- Loop through all mem slots and check if they are empty ----------------");

    LT_LOG("Looping through slots 0-%d.", R_MEM_DATA_SLOT_MAX);
    for (uint16_t i = 0; i <= R_MEM_DATA_SLOT_MAX; i++) {
        // LT_LOG("lt_r_mem_data_read() on slot #%d", i);
        // TODO: Read should fail
        LT_ASSERT(LT_OK, lt_r_mem_data_read(&h, i, r_mem_data, R_MEM_DATA_SIZE_MAX));

        // printf("r_mem_data[] = {");
        // for (int j = 0; j < R_MEM_DATA_SIZE_MAX; j++)
        //     printf("0x%02x, ", r_mem_data[j]);
        // printf("};\n");
    }
    LT_LOG("All slots are empty.");

    LT_LOG("---------------- Loop through all mem slots, write 0xAB and check if written ----------------");

    uint8_t write_data[R_MEM_DATA_SIZE_MAX];
    memset(write_data, 0xAB, R_MEM_DATA_SIZE_MAX);
    LT_LOG("Looping through slots 0-%d.", R_MEM_DATA_SLOT_MAX);
    for (uint16_t i = 0; i <= R_MEM_DATA_SLOT_MAX; i++) {
        LT_ASSERT(LT_OK, lt_r_mem_data_write(&h, i, write_data, R_MEM_DATA_SIZE_MAX));
        LT_ASSERT(LT_OK, lt_r_mem_data_read(&h, i, r_mem_data, R_MEM_DATA_SIZE_MAX));
        LT_ASSERT(0, memcmp(r_mem_data, write_data, R_MEM_DATA_SIZE_MAX));
    }
    LT_LOG("All slots were successfuly written.");

    LT_LOG("---------------- Loop through all mem slots and erase them ----------------");

    LT_LOG("Looping through slots 0-%d.", R_MEM_DATA_SLOT_MAX);
    for (uint16_t i = 0; i <= R_MEM_DATA_SLOT_MAX; i++) {
        LT_ASSERT(LT_OK, lt_r_mem_data_erase(&h, i));
        // TODO: Read should fail
        LT_ASSERT(LT_OK, lt_r_mem_data_read(&h, i, r_mem_data, R_MEM_DATA_SIZE_MAX));

        // LT_LOG("Slot #%d after erasing:", i);
        // printf("r_mem_data[] = {");
        // for (int j = 0; j < R_MEM_DATA_SIZE_MAX; j++)
        //     printf("0x%02x, ", r_mem_data[j]);
        // printf("};\n");
    }
    LT_LOG("All slots were successfuly erased.");

    LT_LOG("%s", "lt_session_abort()");
    LT_ASSERT(LT_OK, lt_session_abort(&h));

    LT_LOG("%s", "lt_deinit()");
    LT_ASSERT(LT_OK, lt_deinit(&h));

    return 0;
}
