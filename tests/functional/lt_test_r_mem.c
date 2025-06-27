/**
 * @file lt_test_r_mem.c
 * @brief Test all R MEM operations on all R memory slots
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

uint8_t r_mem_buff[R_MEM_DATA_SIZE_MAX];
uint8_t r_mem_expected_ff[R_MEM_DATA_SIZE_MAX];

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

    memset(r_mem_buff, 0x00, R_MEM_DATA_SIZE_MAX);
    memset(r_mem_expected_ff, 0x00, R_MEM_DATA_SIZE_MAX);

    lt_handle_t h = {0};
#if LT_SEPARATE_L3_BUFF
    uint8_t l3_buffer[L3_FRAME_MAX_SIZE] __attribute__((aligned(16))) = {0};
    h.l3.buff = l3_buffer;
    h.l3.buff_len = sizeof(l3_buffer);
#endif

    LT_LOG("%s", "Initialize handle");
    LT_ASSERT(LT_OK, lt_init(&h));

    // Ping with SH0
    LT_LOG("%s with %d", "verify_chip_and_start_secure_session()", PAIRING_KEY_SLOT_INDEX_0);
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, PAIRING_KEY_SLOT_INDEX_0));

    LT_LOG("---------------- Loop through all mem slots and check if they are empty ----------------");

    for (uint16_t i = 0; i < 5; i++) {
        LT_LOG("lt_r_mem_data_read() slot        n.%d  ", i);
        LT_LOG_RESULT("%s", lt_ret_verbose(lt_r_mem_data_read(&h, i, r_mem_buff, R_MEM_DATA_SIZE_MAX)));
        LT_ASSERT(0, memcmp(r_mem_buff, r_mem_expected_ff, R_MEM_DATA_SIZE_MAX));
    }

    LT_LOG("---------------- Loop through all mem slots, fill them with 0x00 and check if written   ----------------");

    for (uint16_t i = 0; i < 5; i++) {
        uint8_t zeroes[R_MEM_DATA_SIZE_MAX] = {0};
        // Write zeroes
        LT_LOG("lt_r_mem_data_write() slot        n.%d  ", i);
        LT_LOG_RESULT("%s", lt_ret_verbose(lt_r_mem_data_write(&h, i, zeroes, R_MEM_DATA_SIZE_MAX)));

        // Read content
        LT_LOG("lt_r_mem_data_read() slot        n.%d  ", i);
        LT_LOG_RESULT("%s", lt_ret_verbose(lt_r_mem_data_read(&h, i, r_mem_buff, R_MEM_DATA_SIZE_MAX)));
        LT_ASSERT(0, memcmp(r_mem_buff, zeroes, R_MEM_DATA_SIZE_MAX));
    }

    LT_LOG("---------------- Loop through all mem slots and erase them   ----------------");

    for (uint16_t i = 0; i < 5; i++) {
        // Erase
        LT_LOG("lt_r_mem_data_erase() slot        n.%d  ", i);
        LT_LOG_RESULT("%s", lt_ret_verbose(lt_r_mem_data_erase(&h, i)));

        // Read content
        LT_LOG("lt_r_mem_data_read() slot        n.%d  ", i);
        LT_LOG_RESULT("%s", lt_ret_verbose(lt_r_mem_data_read(&h, i, r_mem_buff, R_MEM_DATA_SIZE_MAX)));
        LT_ASSERT(0, memcmp(r_mem_buff, r_mem_expected_ff, R_MEM_DATA_SIZE_MAX));
    }

    LT_LOG("%s", "lt_session_abort()");
    LT_ASSERT(LT_OK, lt_session_abort(&h));

    // Deinit handle
    LT_LOG("%s", "lt_deinit()");
    LT_ASSERT(LT_OK, lt_deinit(&h));

    return 0;
}
