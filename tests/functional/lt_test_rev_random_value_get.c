/**
 * @file lt_test_rev_random_value_get.c
 * @brief Tests Random_Value_Get command.
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

#define RANDOM_VALUE_GET_LOOPS 300

void lt_test_rev_random_value_get(void)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_rev_random_value_get()");
    LT_LOG_INFO("----------------------------------------------");

    lt_handle_t h = {0};
#if LT_SEPARATE_L3_BUFF
    uint8_t l3_buffer[L3_FRAME_MAX_SIZE] __attribute__((aligned(16))) = {0};
    h.l3.buff = l3_buffer;
    h.l3.buff_len = sizeof(l3_buffer);
#endif
    uint32_t random_data_size;
    uint8_t random_data[RANDOM_VALUE_GET_LEN_MAX];

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(&h));

    LT_LOG_INFO("Starting Secure Session with key %d", PAIRING_KEY_SLOT_INDEX_0);
    LT_TEST_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, PAIRING_KEY_SLOT_INDEX_0));
    LT_LOG_LINE();

    LT_LOG_INFO("Random_Value_Get will be executed %d times", RANDOM_VALUE_GET_LOOPS);
    for (uint16_t i = 0; i < RANDOM_VALUE_GET_LOOPS; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Generating random data length <= %d (with lt_port_random_bytes())...", RANDOM_VALUE_GET_LEN_MAX);
        LT_TEST_ASSERT(LT_OK, lt_port_random_bytes(&random_data_size, 1));
        random_data_size %= RANDOM_VALUE_GET_LEN_MAX + 1;  // 0-255

        LT_LOG_INFO("Getting %d random numbers from TROPIC01...", random_data_size);
        LT_TEST_ASSERT(LT_OK, lt_random_value_get(&h, random_data, random_data_size));
        LT_LOG_INFO("Random data from TROPIC01:");
        hexdump_8byte(random_data, random_data_size);
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Aborting Secure Session");
    LT_TEST_ASSERT(LT_OK, lt_session_abort(&h));

    LT_LOG_INFO("Deinitializing handle");
    LT_TEST_ASSERT(LT_OK, lt_deinit(&h));
}