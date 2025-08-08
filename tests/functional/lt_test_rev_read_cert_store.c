/**
 * @file lt_test_rev_read_cert_store.c
 * @brief Read Certificate Store with 4 certificates and print it to log.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <inttypes.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"

/** @brief Length of the buffers for certificates. */
#define CERTS_BUF_LEN 700

void lt_test_rev_read_cert_store(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_rev_read_cert_store()");
    LT_LOG_INFO("----------------------------------------------");

    uint8_t cert1[CERTS_BUF_LEN] = {0}, cert2[CERTS_BUF_LEN] = {0}, cert3[CERTS_BUF_LEN] = {0}, cert4[CERTS_BUF_LEN] = {0};
    struct lt_cert_store_t store = {.certs = {cert1, cert2, cert3, cert4},
                                    .buf_len = {CERTS_BUF_LEN, CERTS_BUF_LEN, CERTS_BUF_LEN, CERTS_BUF_LEN}};

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    LT_LOG_INFO("Reading Certificate store");
    LT_TEST_ASSERT(LT_OK, lt_get_info_cert_store(h, &store));
    LT_LOG_INFO();

    uint8_t *cert;
    for (int i = 0; i < 4; i++) {
        cert = store.certs[i];
        LT_LOG_INFO("Certificate number: %d", i);
        LT_LOG_INFO("Checking if size of certificate is not zero");
        LT_TEST_ASSERT(1, (store.cert_len[i] != 0));
        LT_LOG_INFO("Size in bytes: %" PRIu16, store.cert_len[i]);

        for (int j = 0; j < store.cert_len[i]; j += 16)
            LT_LOG_INFO("%02" PRIx8 "%02" PRIx8 "%02" PRIx8 "%02" PRIx8 "%02" PRIx8 "%02" PRIx8 "%02" PRIx8 "%02" PRIx8
                        "%02" PRIx8 "%02" PRIx8 "%02" PRIx8 "%02" PRIx8 "%02" PRIx8 "%02" PRIx8 "%02" PRIx8 "%02" PRIx8,
                        cert[j], cert[j + 1], cert[j + 2], cert[j + 3], cert[j + 4], cert[j + 5], cert[j + 6],
                        cert[j + 7], cert[j + 8], cert[j + 9], cert[j + 10], cert[j + 11], cert[j + 12], cert[j + 13],
                        cert[j + 14], cert[j + 15]);
        LT_LOG_INFO();
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Deinitializing handle");
    LT_TEST_ASSERT(LT_OK, lt_deinit(h));
}
