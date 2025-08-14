/**
 * @file lt_test_rev_resend_req_app.c
 * @brief Test L2 Resend Request in Application mode.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <string.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "lt_l1.h"
#include "lt_l2.h"
#include "lt_l2_api_structs.h"
#include "lt_l2_frame_check.h"

void lt_test_rev_resend_req_app(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------");
    LT_LOG_INFO("lt_test_rev_resend_req_app()");
    LT_LOG_INFO("----------------------------------");

    LT_LOG_INFO("Preparing handle.");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    // Sending dummy Get_Info_Req. We don't need the data, just something to request a resend later.
    // We utilize that Get_Info_Req fits into one frame.
    struct lt_chip_id_t prev_chip_id;
    LT_LOG_INFO("Sending L2 Get_Info_Req...");
    LT_TEST_ASSERT(LT_OK, lt_get_info_chip_id(h, &prev_chip_id));

    // Requesting a resend.
    LT_LOG_INFO("Asking to resend last response frame...");
    LT_TEST_ASSERT(LT_OK, lt_l2_resend_response(&h->l2));

    LT_LOG_INFO("Compare if previously received and now resended frames match.");
    // Setup a request pointer to l2 buffer with response data
    struct lt_l2_get_info_rsp_t *p_l2_resp = (struct lt_l2_get_info_rsp_t *)h->l2.buff;
    // Check incomming l3 length
    LT_TEST_ASSERT(LT_L2_GET_INFO_CHIP_ID_SIZE, p_l2_resp->rsp_len);
    // Compare contents.
    LT_TEST_ASSERT(0, memcmp(&prev_chip_id, p_l2_resp->object, LT_L2_GET_INFO_CHIP_ID_SIZE));
}