/**
 * @file lt_test_dump_cert_store.c
 * @brief Dumps content of Tropic01 Certificate store
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "string.h"
#include "inttypes.h"

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"

/**
 * @brief
 *
 * @return int
 */
int lt_test_dump_cert_store(void)
{
    LT_LOG("  -------------------------------------------------------------------------------------------------------------");
    LT_LOG("  -------- lt_test_dump_cert_store() --------------------------------------------------------------------------");
    LT_LOG("  -------------------------------------------------------------------------------------------------------------");

    lt_handle_t h = {0};
    uint8_t cert_store[3840];

    LT_LOG("%s", "Initialize handle");
    LT_ASSERT(LT_OK, lt_init(&h));

    LT_LOG("%s with %d", "verify_chip_and_start_secure_session()", pkey_index_0);
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, pkey_index_0));

    LT_LOG("%s", "lt_get_info_cert");
    LT_ASSERT(LT_OK, lt_get_info_cert(&h, cert_store, sizeof(cert_store)));

    LT_LOG("%s", "Certificate store: ");
    for (int i = 0; i < sizeof(cert_store); i += 16) {
        LT_LOG("%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x",
                cert_store[i],      cert_store[i+1],   cert_store[i+2],    cert_store[i+3],
                cert_store[i+4],    cert_store[i+5],   cert_store[i+6],    cert_store[i+7],
                cert_store[i+8],    cert_store[i+9],   cert_store[i+10],   cert_store[i+11],
                cert_store[i+12],   cert_store[i+13],  cert_store[i+14],   cert_store[i+15]
                );
    }

    // Deinit handle
    LT_LOG("%s", "lt_deinit()");
    LT_ASSERT(LT_OK, lt_deinit(&h));

    return 0;
}


