/**
 * @file lt_test_dump_cert_store.c
 * @brief Dumps content of TROPIC01 Certificate store
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
int lt_test_dump_cert_store(void)
{
    LT_LOG(
        "  "
        "------------------------------------------------------------------------------------------------------------"
        "-");
    LT_LOG(
        "  -------- lt_test_dump_cert_store() "
        "--------------------------------------------------------------------------");
    LT_LOG(
        "  "
        "------------------------------------------------------------------------------------------------------------"
        "-");

    lt_handle_t h = {0};

    LT_LOG("%s", "Initialize handle");
    LT_ASSERT(LT_OK, lt_init(&h));

    LT_LOG("%s with %d", "verify_chip_and_start_secure_session()", PAIRING_KEY_SLOT_INDEX_0);
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, PAIRING_KEY_SLOT_INDEX_0));

    LT_LOG("%s", "lt_get_info_cert_store");

    uint8_t cert1[700] = {0};
    uint8_t cert2[700] = {0};
    uint8_t cert3[700] = {0};
    uint8_t cert4[700] = {0};

    struct lt_cert_store_t store
        = {.cert_len = {0, 0, 0, 0}, .buf_len = {700, 700, 700, 700}, .certs = {cert1, cert2, cert3, cert4}};

    LT_ASSERT(LT_OK, lt_get_info_cert_store(&h, &store));

    for (int i = 0; i < 4; i++) {
        LT_LOG("Certificate number: %d", i);
        LT_LOG("Size in bytes: %d", store.cert_len[i]);

        uint8_t *cert = store.certs[i];

        for (int j = 0; j < store.cert_len[i]; j += 16)
            LT_LOG("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", cert[j], cert[j + 1],
                   cert[j + 2], cert[j + 3], cert[j + 4], cert[j + 5], cert[j + 6], cert[j + 7], cert[j + 8],
                   cert[j + 9], cert[j + 10], cert[j + 11], cert[j + 12], cert[j + 13], cert[j + 14], cert[j + 15]);
    }

    // Deinit handle
    LT_LOG("%s", "lt_deinit()");
    LT_ASSERT(LT_OK, lt_deinit(&h));

    return 0;
}
