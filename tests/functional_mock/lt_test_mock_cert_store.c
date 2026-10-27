/**
 * @file lt_test_mock_cert_store.c
 * @brief Test for parsing the X.509 Certificate Store with lt_get_info_cert_store().
 * @copyright Copyright (c) 2020-2026 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <stdbool.h>
#include <string.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_logging.h"
#include "libtropic_macros.h"
#include "libtropic_port_mock.h"
#include "lt_functional_mock_tests.h"
#include "lt_l1.h"
#include "lt_l2_api_structs.h"
#include "lt_l2_frame_check.h"
#include "lt_mock_helpers.h"
#include "lt_test_common.h"

/** Size of a Get_Info block of the X.509 Certificate Store. */
#define CERT_STORE_BLOCK_LEN 128
/** Size of the X.509 Certificate Store header (version, count, 2 B length for each certificate). */
#define CERT_STORE_HEADER_LEN (2 + 2 * LT_NUM_CERTIFICATES)
/** Maximum combined length of all certificates (the space left after the header). */
#define CERT_STORE_CERTS_MAX_LEN (TR01_L2_GET_INFO_REQ_CERT_SIZE_TOTAL - CERT_STORE_HEADER_LEN)
/** Bytes after each certificate buffer that must never be written by the parser. */
#define CERT_BUF_GUARD_LEN 16
/** Value the certificate buffers are filled with before each test case. */
#define CERT_BUF_FILL 0xA5

typedef struct cert_store_test_case_t {
    const char *name;
    uint8_t version;
    uint8_t num_certs;
    uint16_t cert_len[LT_NUM_CERTIFICATES];
    uint16_t buf_len[LT_NUM_CERTIFICATES];
    lt_ret_t expected_ret;
} cert_store_test_case_t;

// Buffers passed to the parser (each followed by a guard area) and the mocked store contents.
static uint8_t cert_bufs[LT_NUM_CERTIFICATES]
                        [TR01_L2_GET_INFO_REQ_CERT_SIZE_TOTAL + CERT_BUF_GUARD_LEN];
static uint8_t cert_store[TR01_L2_GET_INFO_REQ_CERT_SIZE_TOTAL];

#define VER LT_CERT_STORE_VERSION
#define NUM LT_NUM_CERTIFICATES
// clang-format off
static const cert_store_test_case_t test_cases[] = {
    // name                                  version  num_certs  cert_len                  buf_len                   expected_ret

    // Valid certificate stores.
    {"realistic lengths",                    VER,     NUM,       {471, 602, 590, 571},     {700, 700, 700, 700},     LT_OK},
    {"first cert ends inside block 0",       VER,     NUM,       {100, 300, 300, 300},     {700, 700, 700, 700},     LT_OK},
    {"short cert with exact-size buffer",    VER,     NUM,       {200, 20, 300, 300},      {700, 20, 700, 700},      LT_OK},
    {"all certs in block 0, minimal length", VER,     NUM,       {2, 2, 2, 2},             {2, 2, 2, 2},             LT_OK},
    // Store offsets after each cert: 10 + 118 = 128, 256, 512, 1152 (= 9 * 128).
    {"certs end on block boundaries",        VER,     NUM,       {118, 128, 256, 640},     {118, 128, 256, 640},     LT_OK},
    // 3824 + 3 * 2 = CERT_STORE_CERTS_MAX_LEN.
    {"store completely full",                VER,     NUM,       {3824, 2, 2, 2},          {3824, 2, 2, 2},          LT_OK},

    // Malformed certificate stores.
    {"invalid version",                      VER + 1, NUM,       {471, 602, 590, 571},     {700, 700, 700, 700},     LT_CERT_STORE_INVALID},
    {"invalid number of certs",              VER,     NUM - 1,   {471, 602, 590, 571},     {700, 700, 700, 700},     LT_CERT_STORE_INVALID},
    {"zero-length cert",                     VER,     NUM,       {200, 0, 300, 300},       {700, 700, 700, 700},     LT_CERT_STORE_INVALID},
    // Minimum certificate length is 2.
    {"cert length below minimum",            VER,     NUM,       {200, 300, 1, 300},       {700, 700, 700, 700},     LT_CERT_STORE_INVALID},
    // Maximum certificate length is 3837. Note that any cert longer than 3824 also fails the
    // "certs fit into the store" check, so this case only checks the returned error code.
    {"cert length above maximum",            VER,     NUM,       {3838, 2, 2, 2},          {3840, 700, 700, 700},    LT_CERT_STORE_INVALID},
    // 3900 > CERT_STORE_CERTS_MAX_LEN.
    {"certs do not fit into the store",      VER,     NUM,       {1000, 1000, 1000, 900},  {1000, 1000, 1000, 1000}, LT_CERT_STORE_INVALID},

    // Valid certificate store, but a buffer provided by the caller is too small.
    {"buffer too small",                     VER,     NUM,       {471, 602, 590, 571},     {700, 500, 700, 700},     LT_PARAM_ERR},
};
// clang-format on
#undef VER
#undef NUM

/** @brief Combined length of all certificates declared by the test case. */
static uint32_t total_cert_len(const cert_store_test_case_t *tc)
{
    uint32_t len = 0;
    for (int i = 0; i < LT_NUM_CERTIFICATES; i++) {
        len += tc->cert_len[i];
    }
    return len;
}

/**
 * @brief Byte at the given offset of the concatenated certificates.
 * The pattern is position-dependent, so any misplaced byte is detected.
 */
static uint8_t cert_pattern_byte(uint32_t offset) { return (uint8_t)(offset * 7 + 3); }

/** @brief Fills the mocked certificate store: header followed by the certificates and 0xFF padding. */
static void build_cert_store(const cert_store_test_case_t *tc)
{
    memset(cert_store, 0xFF, sizeof(cert_store));

    uint8_t *p = cert_store;
    *p++ = tc->version;
    *p++ = tc->num_certs;
    for (int i = 0; i < LT_NUM_CERTIFICATES; i++) {
        *p++ = tc->cert_len[i] >> 8;
        *p++ = (uint8_t)tc->cert_len[i];
    }

    // Malformed stores may declare more certificate bytes than the store can hold.
    uint32_t certs_len = lt_min(total_cert_len(tc), (uint32_t)CERT_STORE_CERTS_MAX_LEN);
    for (uint32_t i = 0; i < certs_len; i++) {
        *p++ = cert_pattern_byte(i);
    }
}

/**
 * @brief Number of Get_Info blocks lt_get_info_cert_store() is expected to read: all blocks up to the
 * end of the last certificate for a valid store, only block 0 (with the header) otherwise.
 */
static int expected_block_count(const cert_store_test_case_t *tc)
{
    if (tc->expected_ret != LT_OK) {
        return 1;
    }
    uint32_t store_len = CERT_STORE_HEADER_LEN + total_cert_len(tc);
    return (store_len + CERT_STORE_BLOCK_LEN - 1) / CERT_STORE_BLOCK_LEN;
}

/**
 * @brief Enqueues the mocked responses: chip mode check, then one Get_Info L2 Request/Response pair
 * for each block.
 */
static void enqueue_cert_store(lt_handle_t *h, int block_count)
{
    const uint8_t chip_ready = TR01_L1_CHIP_MODE_READY_bit;

    // lt_get_tr01_mode()
    LT_TEST_ASSERT(LT_OK, lt_mock_hal_enqueue_response(&h->l2, &chip_ready, sizeof(chip_ready)));

    for (int block = 0; block < block_count; block++) {
        struct lt_l2_get_info_rsp_t get_info_resp = {.chip_status = TR01_L1_CHIP_MODE_READY_bit,
                                                     .status = TR01_L2_STATUS_REQUEST_OK,
                                                     .rsp_len = CERT_STORE_BLOCK_LEN,
                                                     .object = {0}};
        memcpy(get_info_resp.object, cert_store + block * CERT_STORE_BLOCK_LEN, CERT_STORE_BLOCK_LEN);
        add_resp_crc(&get_info_resp);

        // Get_Info L2 Request.
        LT_TEST_ASSERT(LT_OK, lt_mock_hal_enqueue_response(&h->l2, &chip_ready, sizeof(chip_ready)));
        // Get_Info L2 Response with this block.
        LT_TEST_ASSERT(LT_OK, lt_mock_hal_enqueue_response(&h->l2, (uint8_t *)&get_info_resp,
                                                           calc_mocked_resp_len(&get_info_resp)));
    }
}

/** @brief Fills the certificate buffers with CERT_BUF_FILL and points the store at them. */
static void prepare_cert_bufs(struct lt_cert_store_t *store, const cert_store_test_case_t *tc)
{
    memset(cert_bufs, CERT_BUF_FILL, sizeof(cert_bufs));
    memset(store, 0, sizeof(*store));
    for (int i = 0; i < LT_NUM_CERTIFICATES; i++) {
        store->certs[i] = cert_bufs[i];
        store->buf_len[i] = tc->buf_len[i];
    }
}

/** @brief Checks that the len bytes at buf still contain CERT_BUF_FILL. */
static bool is_untouched(const uint8_t *buf, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        if (buf[i] != CERT_BUF_FILL) {
            return false;
        }
    }
    return true;
}

/** @brief Checks the contents of the certificate buffers after lt_get_info_cert_store(). */
static void check_cert_bufs(const struct lt_cert_store_t *store, const cert_store_test_case_t *tc)
{
    // Nothing is written past the buffer provided by the caller.
    for (int i = 0; i < LT_NUM_CERTIFICATES; i++) {
        LT_TEST_ASSERT(true, is_untouched(cert_bufs[i] + tc->buf_len[i], CERT_BUF_GUARD_LEN));
    }

    if (tc->expected_ret != LT_OK) {
        return;
    }

    // Each certificate is copied unchanged and nothing is written past its length.
    size_t cert_offset = CERT_STORE_HEADER_LEN;
    for (int i = 0; i < LT_NUM_CERTIFICATES; i++) {
        LT_TEST_ASSERT(tc->cert_len[i], store->cert_len[i]);
        LT_TEST_ASSERT(0, memcmp(cert_bufs[i], cert_store + cert_offset, tc->cert_len[i]));
        LT_TEST_ASSERT(true,
                       is_untouched(cert_bufs[i] + tc->cert_len[i], tc->buf_len[i] - tc->cert_len[i]));
        cert_offset += tc->cert_len[i];
    }
}

void lt_test_mock_cert_store(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_mock_cert_store()");
    LT_LOG_INFO("----------------------------------------------");

    lt_dev_mock_t *mock_dev = (lt_dev_mock_t *)h->l2.device;

    // 1. Mock init sequence and initialize Libtropic handle.
    // ---------------------------------------------------------------------------------------------
    lt_mock_hal_reset(&h->l2);
    LT_LOG_INFO("Mocking initialization...");
    LT_TEST_ASSERT(LT_OK,
                   mock_init_communication(h, (uint8_t[]){0x00, 0x00, 0x00, 0x02}));  // Version 2.0.0

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    // 2. Read each mocked certificate store and check the result.
    // ---------------------------------------------------------------------------------------------
    for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
        const cert_store_test_case_t *tc = &test_cases[i];
        LT_LOG_INFO("Test case: %s", tc->name);

        build_cert_store(tc);
        lt_mock_hal_reset(&h->l2);
        enqueue_cert_store(h, expected_block_count(tc));

        struct lt_cert_store_t store;
        prepare_cert_bufs(&store, tc);

        LT_TEST_ASSERT(tc->expected_ret, lt_get_info_cert_store(h, &store));

        // Exactly the expected blocks were read: not fewer, and no more after the last certificate.
        LT_TEST_ASSERT(0, mock_dev->mock_queue_count);

        check_cert_bufs(&store, tc);
    }

    // 3. Deinitialize Libtropic handle.
    // ---------------------------------------------------------------------------------------------
    LT_LOG_INFO("Deinitializing handle");
    LT_TEST_ASSERT(LT_OK, lt_deinit(h));
}
