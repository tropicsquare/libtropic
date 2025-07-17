/**
 * @file lt_test_rev_r_mem.c
 * @brief Test R_Mem_Data_* commands on all User Data slots
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "string.h"

void lt_test_rev_r_mem(void)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_rev_r_mem()");
    LT_LOG_INFO("----------------------------------------------");

    lt_handle_t h = {0};
#if LT_SEPARATE_L3_BUFF
    uint8_t l3_buffer[L3_FRAME_MAX_SIZE] __attribute__((aligned(16))) = {0};
    h.l3.buff = l3_buffer;
    h.l3.buff_len = sizeof(l3_buffer);
#endif
    uint8_t r_mem_data[R_MEM_DATA_SIZE_MAX], write_data[R_MEM_DATA_SIZE_MAX], zeros[R_MEM_DATA_SIZE_MAX] = {0};
    uint16_t read_data_size;

    LT_LOG_INFO("Initializing handle");
    LT_ASSERT(LT_OK, lt_init(&h));

    LT_LOG_INFO("Starting Secure Session with key %d", PAIRING_KEY_SLOT_INDEX_0);
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, PAIRING_KEY_SLOT_INDEX_0));
    LT_LOG_LINE();

    LT_LOG_INFO("Looping through slots 0-%d and checking if empty...", R_MEM_DATA_SLOT_MAX);
    for (uint16_t i = 0; i <= R_MEM_DATA_SLOT_MAX; i++) {
        LT_ASSERT(LT_L3_R_MEM_DATA_READ_SLOT_EMPTY, lt_r_mem_data_read(&h, i, r_mem_data, &read_data_size));
        LT_ASSERT(1, (read_data_size == 0));
    }
    LT_LOG_INFO("All slots are empty");
    LT_LOG_LINE();

    LT_LOG_INFO("Looping through slots 0-%d, writing them entirely with 0xAB and checking if written...",
                R_MEM_DATA_SLOT_MAX);
    memset(write_data, 0xAB, R_MEM_DATA_SIZE_MAX);
    for (uint16_t i = 0; i <= R_MEM_DATA_SLOT_MAX; i++) {
        LT_ASSERT(LT_OK, lt_r_mem_data_write(&h, i, write_data, R_MEM_DATA_SIZE_MAX));
        LT_ASSERT(LT_OK, lt_r_mem_data_read(&h, i, r_mem_data, &read_data_size));
        LT_ASSERT(1, (read_data_size == R_MEM_DATA_SIZE_MAX));
        LT_ASSERT(0, memcmp(r_mem_data, write_data, R_MEM_DATA_SIZE_MAX));
    }
    LT_LOG_INFO("All slots were successfuly written");
    LT_LOG_LINE();

    LT_LOG_INFO("Looping through slots 0-%d, writing 0x00 and checking for fail...", R_MEM_DATA_SLOT_MAX);
    // Set these two to different values just in case
    read_data_size = 0;
    memset(r_mem_data, 0, R_MEM_DATA_SIZE_MAX);
    for (uint16_t i = 0; i <= R_MEM_DATA_SLOT_MAX; i++) {
        LT_ASSERT(LT_L3_R_MEM_DATA_WRITE_WRITE_FAIL, lt_r_mem_data_write(&h, i, zeros, R_MEM_DATA_SIZE_MAX));
        LT_ASSERT(LT_OK, lt_r_mem_data_read(&h, i, r_mem_data, &read_data_size));
        LT_ASSERT(1, (read_data_size == R_MEM_DATA_SIZE_MAX));
        LT_ASSERT(0, memcmp(r_mem_data, write_data, R_MEM_DATA_SIZE_MAX));
    }
    LT_LOG_INFO("No slots were written (correct)");
    LT_LOG_LINE();

    LT_LOG_INFO("Looping through slots 0-%d, erasing them and checking if erased...", R_MEM_DATA_SLOT_MAX);
    for (uint16_t i = 0; i <= R_MEM_DATA_SLOT_MAX; i++) {
        LT_ASSERT(LT_OK, lt_r_mem_data_erase(&h, i));
        LT_ASSERT(LT_L3_R_MEM_DATA_READ_SLOT_EMPTY, lt_r_mem_data_read(&h, i, r_mem_data, &read_data_size));
        LT_ASSERT(1, (read_data_size == 0));
    }
    LT_LOG_INFO("All slots were successfuly erased");
    LT_LOG_LINE();

    LT_LOG_INFO("Looping through slots 0-%d, writing them partially with 0xCD and checking if written...",
                R_MEM_DATA_SLOT_MAX);
    memset(write_data, 0xCD, 100);
    for (uint16_t i = 0; i <= R_MEM_DATA_SLOT_MAX; i++) {
        LT_ASSERT(LT_OK, lt_r_mem_data_write(&h, i, write_data, 100));
        LT_ASSERT(LT_OK, lt_r_mem_data_read(&h, i, r_mem_data, &read_data_size));
        LT_ASSERT(1, (read_data_size == 100));
        LT_ASSERT(0, memcmp(r_mem_data, write_data, 100));
    }
    LT_LOG_INFO("All slots were successfuly written");
    LT_LOG_LINE();

    LT_LOG_INFO("Loop through slots 0-%d, erasing them and checking if erased...", R_MEM_DATA_SLOT_MAX);
    for (uint16_t i = 0; i <= R_MEM_DATA_SLOT_MAX; i++) {
        LT_ASSERT(LT_OK, lt_r_mem_data_erase(&h, i));
        LT_ASSERT(LT_L3_R_MEM_DATA_READ_SLOT_EMPTY, lt_r_mem_data_read(&h, i, r_mem_data, &read_data_size));
        LT_ASSERT(1, (read_data_size == 0));
    }
    LT_LOG_INFO("All slots were successfuly erased");
    LT_LOG_LINE();

    LT_LOG_INFO("Aborting Secure Session");
    LT_ASSERT(LT_OK, lt_session_abort(&h));

    LT_LOG_INFO("Deinitializing handle");
    LT_ASSERT(LT_OK, lt_deinit(&h));
}
