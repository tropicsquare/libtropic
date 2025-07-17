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
#include "libtropic_port.h"
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
    uint32_t random_data[R_MEM_DATA_SIZE_MAX / sizeof(uint32_t)], random_data_size;

    LT_LOG_INFO("Initializing handle");
    LT_ASSERT(LT_OK, lt_init(&h));

    LT_LOG_INFO("Starting Secure Session with key %d", PAIRING_KEY_SLOT_INDEX_0);
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, PAIRING_KEY_SLOT_INDEX_0));
    LT_LOG_LINE();

    LT_LOG_INFO("Checking if all slots are empty...");
    for (uint16_t i = 0; i <= R_MEM_DATA_SLOT_MAX; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Reading slot #%d (should fail)...", i);
        LT_ASSERT(LT_L3_R_MEM_DATA_READ_SLOT_EMPTY, lt_r_mem_data_read(&h, i, r_mem_data, &read_data_size));

        LT_LOG_INFO("Checking number of read bytes (should be 0)...");
        LT_ASSERT(1, (read_data_size == 0));
    }
    LT_LOG_LINE();
    
    LT_LOG_INFO("Testing writing all slots entirely...");
    for (uint16_t i = 0; i <= R_MEM_DATA_SLOT_MAX; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Generating random data for slot #%d...", i);
        LT_ASSERT(LT_OK, lt_port_random_bytes(random_data, sizeof(random_data) / sizeof(uint32_t)));
        memcpy(write_data, random_data, sizeof(write_data));

        LT_LOG_INFO("Writing to slot #%d...", i);
        LT_ASSERT(LT_OK, lt_r_mem_data_write(&h, i, write_data, R_MEM_DATA_SIZE_MAX));

        LT_LOG_INFO("Reading slot #%d...", i);
        LT_ASSERT(LT_OK, lt_r_mem_data_read(&h, i, r_mem_data, &read_data_size));

        LT_LOG_INFO("Checking number of read bytes...");
        LT_ASSERT(1, (read_data_size == R_MEM_DATA_SIZE_MAX));

        LT_LOG_INFO("Checking contents...");
        LT_ASSERT(0, memcmp(r_mem_data, write_data, R_MEM_DATA_SIZE_MAX));

        LT_LOG_INFO("Writing zeros to slot #%d (should fail)...", i);
        LT_ASSERT(LT_L3_R_MEM_DATA_WRITE_WRITE_FAIL, lt_r_mem_data_write(&h, i, zeros, R_MEM_DATA_SIZE_MAX));

        LT_LOG_INFO("Reading slot #%d...", i);
        read_data_size = 0; // Set different value just in case
        LT_ASSERT(LT_OK, lt_r_mem_data_read(&h, i, r_mem_data, &read_data_size));

        LT_LOG_INFO("Checking number of read bytes...");
        LT_ASSERT(1, (read_data_size == R_MEM_DATA_SIZE_MAX));

        LT_LOG_INFO("Checking contents (should still contain original data)...");
        LT_ASSERT(0, memcmp(r_mem_data, write_data, R_MEM_DATA_SIZE_MAX));
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Erasing all slots...");
    for (uint16_t i = 0; i <= R_MEM_DATA_SLOT_MAX; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Erasing slot #%d...", i);
        LT_ASSERT(LT_OK, lt_r_mem_data_erase(&h, i));

        LT_LOG_INFO("Reading slot #%d (should fail)...", i);
        LT_ASSERT(LT_L3_R_MEM_DATA_READ_SLOT_EMPTY, lt_r_mem_data_read(&h, i, r_mem_data, &read_data_size));

        LT_LOG_INFO("Checking number of read bytes (should be 0)...");
        LT_ASSERT(1, (read_data_size == 0));
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Testing writing all slots partially or with zero data size...");
    for (uint16_t i = 0; i <= R_MEM_DATA_SLOT_MAX; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Generating random data length < %d...", R_MEM_DATA_SIZE_MAX);
        LT_ASSERT(LT_OK, lt_port_random_bytes(&random_data_size, 1));
        random_data_size %= R_MEM_DATA_SIZE_MAX;

        LT_LOG_INFO("Generating %d random bytes for slot #%d...", random_data_size, i);
        LT_ASSERT(LT_OK, lt_port_random_bytes(random_data, sizeof(random_data) / sizeof(uint32_t)));
        memcpy(write_data, random_data, random_data_size);

        LT_LOG_INFO("Writing to slot #%d...", i);
        LT_ASSERT_COND(lt_r_mem_data_write(&h, i, write_data, random_data_size), random_data_size != 0, LT_OK, LT_L3_FAIL);

        LT_LOG_INFO("Reading slot #%d...", i);
        LT_ASSERT_COND(lt_r_mem_data_read(&h, i, r_mem_data, &read_data_size), random_data_size != 0, LT_OK, LT_L3_R_MEM_DATA_READ_SLOT_EMPTY);

        LT_LOG_INFO("Checking number of read bytes...");
        LT_ASSERT(1, (read_data_size == random_data_size));

        LT_LOG_INFO("Checking contents...");
        LT_ASSERT(0, memcmp(r_mem_data, write_data, random_data_size));
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Erasing all slots...");
    for (uint16_t i = 0; i <= R_MEM_DATA_SLOT_MAX; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Erasing slot #%d...", i);
        LT_ASSERT(LT_OK, lt_r_mem_data_erase(&h, i));

        LT_LOG_INFO("Reading slot #%d (should fail)...", i);
        LT_ASSERT(LT_L3_R_MEM_DATA_READ_SLOT_EMPTY, lt_r_mem_data_read(&h, i, r_mem_data, &read_data_size));

        LT_LOG_INFO("Checking number of read bytes (should be 0)...");
        LT_ASSERT(1, (read_data_size == 0));
    }
    LT_LOG_LINE();


    LT_LOG_INFO("Aborting Secure Session");
    LT_ASSERT(LT_OK, lt_session_abort(&h));

    LT_LOG_INFO("Deinitializing handle");
    LT_ASSERT(LT_OK, lt_deinit(&h));
}
