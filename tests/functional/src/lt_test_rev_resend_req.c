/**
 * @file lt_test_rev_resend_req.c
 * @brief Test L2 Resend Request.
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <string.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_l2.h"
#include "libtropic_logging.h"
#include "lt_l1.h"
#include "lt_l2_api_structs.h"
#include "lt_l2_frame_check.h"
#include "lt_test_common.h"

lt_handle_t *g_h;

static void lt_test_rev_resend_req_body(void)
{
    // Sending dummy Get_Info_Req. We don't need the data, just something to request a resend later.
    // We utilize that Get_Info_Req fits into one frame.
    struct lt_chip_id_t prev_chip_id;
    LT_LOG_INFO("Sending L2 Get_Info_Req...");
    LT_TEST_ASSERT(LT_OK, lt_get_info_chip_id(g_h, &prev_chip_id));

    // Requesting a resend.
    LT_LOG_INFO("Asking to resend last response frame...");
    lt_l2_resend_response(&g_h->l2);

    LT_LOG_INFO("Compare if previously received and now resended frames match.");
    // Setup a request pointer to l2 buffer with response data
    struct lt_l2_get_info_rsp_t *p_l2_resp = (struct lt_l2_get_info_rsp_t *)g_h->l2.buff;
    // Check incomming l3 length
    LT_TEST_ASSERT(TR01_L2_GET_INFO_CHIP_ID_SIZE, p_l2_resp->rsp_len);
    // Compare contents.
    LT_TEST_ASSERT(0, memcmp(&prev_chip_id, p_l2_resp->object, TR01_L2_GET_INFO_CHIP_ID_SIZE));
}

static lt_ret_t lt_test_rev_resend_req_cleanup(void)
{
    lt_ret_t ret;

    LT_LOG_INFO("Rebooting to the Application mode...");
    ret = lt_reboot(g_h, TR01_REBOOT);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Couldn't reboot to the Application mode!");
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

void lt_test_rev_resend_req(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------");
    LT_LOG_INFO("lt_test_rev_resend_req()");
    LT_LOG_INFO("----------------------------------");

    g_h = h;

    LT_LOG_INFO("Preparing handle.");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    LT_LOG_INFO("Rebooting into Application mode...");
    LT_TEST_ASSERT(LT_OK, lt_reboot(h, TR01_REBOOT));

    lt_test_rev_resend_req_body();
    LT_LOG_LINE();

    LT_LOG_INFO("Rebooting into Maintenance mode...");
    LT_TEST_ASSERT(LT_OK, lt_reboot(h, TR01_MAINTENANCE_REBOOT));

    lt_test_cleanup_function = &lt_test_rev_resend_req_cleanup;

    lt_test_rev_resend_req_body();
    LT_LOG_LINE();

    // Call cleanup function, but don't call it from LT_TEST_ASSERT anymore.
    lt_test_cleanup_function = NULL;
    LT_LOG_INFO("Starting post-test cleanup");
    LT_TEST_ASSERT(LT_OK, lt_test_rev_resend_req_cleanup());
    LT_LOG_INFO("Post-test cleanup was successful");
}