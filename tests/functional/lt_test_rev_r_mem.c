/**
 * @file lt_test_rev_r_mem.c
 * @brief Test R_Mem_Data_* commands on all User Data slots
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <inttypes.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "lt_random.h"
#include "string.h"

// Shared with cleanup function
lt_handle_t *g_h;

static lt_ret_t lt_test_rev_r_mem_cleanup(void)
{
    lt_ret_t ret;
    uint8_t r_mem_data[TR01_R_MEM_DATA_SIZE_MAX];
    uint16_t read_data_size;

    LT_LOG_INFO("Starting secure session with slot %d", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    ret = lt_verify_chip_and_start_secure_session(g_h, sh0priv, sh0pub, TR01_PAIRING_KEY_SLOT_INDEX_0);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to establish secure session.");
        return ret;
    }

    LT_LOG_INFO("Erasing all slots...");
    for (uint16_t i = 0; i <= TR01_R_MEM_DATA_SLOT_MAX; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Erasing slot #%" PRIu16 "...", i);
        ret = lt_r_mem_data_erase(g_h, i);
        if (LT_OK != ret) {
            LT_LOG_ERROR("Failed to erase slot.");
            return ret;
        }

        LT_LOG_INFO("Reading slot #%" PRIu16 " (should fail)...", i);
        ret = lt_r_mem_data_read(g_h, i, r_mem_data, &read_data_size);
        if (LT_L3_R_MEM_DATA_READ_SLOT_EMPTY != ret) {
            LT_LOG_ERROR("Return value is not LT_L3_R_MEM_DATA_READ_SLOT_EMPTY.");
            return ret;
        }

        LT_LOG_INFO("Checking number of read bytes (should be 0)...");
        if (read_data_size != 0) {
            LT_LOG_ERROR("Number of read bytes is not zero.");
            return LT_FAIL;
        }
    }

    LT_LOG_INFO("Aborting secure session");
    ret = lt_session_abort(g_h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to abort secure session.");
        return ret;
    }

    LT_LOG_INFO("Deinitializing handle");
    ret = lt_deinit(g_h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to deinitialize handle.");
        return ret;
    }

    return LT_OK;
}

void lt_test_rev_r_mem(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_rev_r_mem()");
    LT_LOG_INFO("----------------------------------------------");

    // Making the handle accessible to the cleanup function.
    g_h = h;

    uint8_t r_mem_data[TR01_R_MEM_DATA_SIZE_MAX], write_data[TR01_R_MEM_DATA_SIZE_MAX],
        zeros[TR01_R_MEM_DATA_SIZE_MAX] = {0};
    uint16_t read_data_size, write_data_len;

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    LT_LOG_INFO("Starting Secure Session with key %d", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    LT_TEST_ASSERT(LT_OK, lt_verify_chip_and_start_secure_session(h, sh0priv, sh0pub, TR01_PAIRING_KEY_SLOT_INDEX_0));
    LT_LOG_LINE();

    LT_LOG_INFO("Checking if all slots are empty...");
    for (uint16_t i = 0; i <= TR01_R_MEM_DATA_SLOT_MAX; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Reading slot #%" PRIu16 " (should fail)...", i);
        LT_TEST_ASSERT(LT_L3_R_MEM_DATA_READ_SLOT_EMPTY, lt_r_mem_data_read(h, i, r_mem_data, &read_data_size));

        LT_LOG_INFO("Checking number of read bytes (should be 0)...");
        LT_TEST_ASSERT(1, (read_data_size == 0));
    }
    LT_LOG_LINE();

    // We might need erasing if fail occurs in the following code
    lt_test_cleanup_function = &lt_test_rev_r_mem_cleanup;

    LT_LOG_INFO("Testing writing all slots entirely...");
    for (uint16_t i = 0; i <= TR01_R_MEM_DATA_SLOT_MAX; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Generating random data for slot #%" PRIu16 "...", i);
        LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, write_data, sizeof(write_data)));

        LT_LOG_INFO("Writing to slot #%" PRIu16 "...", i);
        LT_TEST_ASSERT(LT_OK, lt_r_mem_data_write(h, i, write_data, TR01_R_MEM_DATA_SIZE_MAX));

        LT_LOG_INFO("Reading slot #%" PRIu16 "...", i);
        LT_TEST_ASSERT(LT_OK, lt_r_mem_data_read(h, i, r_mem_data, &read_data_size));

        LT_LOG_INFO("Checking number of read bytes...");
        LT_TEST_ASSERT(1, (read_data_size == TR01_R_MEM_DATA_SIZE_MAX));

        LT_LOG_INFO("Checking contents...");
        LT_TEST_ASSERT(0, memcmp(r_mem_data, write_data, TR01_R_MEM_DATA_SIZE_MAX));

        LT_LOG_INFO("Writing zeros to slot #%" PRIu16 " (should fail)...", i);
        LT_TEST_ASSERT(LT_L3_R_MEM_DATA_WRITE_WRITE_FAIL, lt_r_mem_data_write(h, i, zeros, TR01_R_MEM_DATA_SIZE_MAX));

        LT_LOG_INFO("Reading slot #%" PRIu16 "...", i);
        read_data_size = 0;  // Set different value just in case
        LT_TEST_ASSERT(LT_OK, lt_r_mem_data_read(h, i, r_mem_data, &read_data_size));

        LT_LOG_INFO("Checking number of read bytes...");
        LT_TEST_ASSERT(1, (read_data_size == TR01_R_MEM_DATA_SIZE_MAX));

        LT_LOG_INFO("Checking contents (should still contain original data)...");
        LT_TEST_ASSERT(0, memcmp(r_mem_data, write_data, TR01_R_MEM_DATA_SIZE_MAX));
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Erasing all slots...");
    for (uint16_t i = 0; i <= TR01_R_MEM_DATA_SLOT_MAX; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Erasing slot #%" PRIu16 "...", i);
        LT_TEST_ASSERT(LT_OK, lt_r_mem_data_erase(h, i));

        LT_LOG_INFO("Reading slot #%" PRIu16 " (should fail)...", i);
        LT_TEST_ASSERT(LT_L3_R_MEM_DATA_READ_SLOT_EMPTY, lt_r_mem_data_read(h, i, r_mem_data, &read_data_size));

        LT_LOG_INFO("Checking number of read bytes (should be 0)...");
        LT_TEST_ASSERT(1, (read_data_size == 0));
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Testing writing all slots partially...");
    for (uint16_t i = 0; i <= TR01_R_MEM_DATA_SLOT_MAX; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Generating random data length < %d...", (int)TR01_R_MEM_DATA_SIZE_MAX);
        LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, &write_data_len, sizeof(write_data_len)));
        write_data_len %= TR01_R_MEM_DATA_SIZE_MAX - 1;  // 0-442
        write_data_len += 1;                             // 1-443

        LT_LOG_INFO("Generating %" PRIu16 " random bytes for slot #%" PRIu16 "...", write_data_len, i);
        LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, write_data, write_data_len));

        LT_LOG_INFO("Writing to slot #%" PRIu16 "...", i);
        LT_TEST_ASSERT_COND(lt_r_mem_data_write(h, i, write_data, write_data_len), write_data_len != 0, LT_OK,
                            LT_L3_FAIL);

        LT_LOG_INFO("Reading slot #%" PRIu16 "...", i);
        LT_TEST_ASSERT_COND(lt_r_mem_data_read(h, i, r_mem_data, &read_data_size), write_data_len != 0, LT_OK,
                            LT_L3_R_MEM_DATA_READ_SLOT_EMPTY);

        LT_LOG_INFO("Checking number of read bytes...");
        LT_TEST_ASSERT(1, (read_data_size == write_data_len));

        LT_LOG_INFO("Checking contents...");
        LT_TEST_ASSERT(0, memcmp(r_mem_data, write_data, write_data_len));
    }
    LT_LOG_LINE();

    // Call cleanup function, but don't call it from LT_TEST_ASSERT anymore.
    lt_test_cleanup_function = NULL;
    LT_LOG_INFO("Starting post-test cleanup");
    LT_TEST_ASSERT(LT_OK, lt_test_rev_r_mem_cleanup());
    LT_LOG_INFO("Post-test cleanup was successful");
}
