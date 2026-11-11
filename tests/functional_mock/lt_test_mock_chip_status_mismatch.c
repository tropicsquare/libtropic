/**
 * @file lt_test_mock_chip_status_mismatch.c
 * @brief Test for checking the mitigation of CHIP_STATUS corruption after start-up.
 * @copyright Copyright (c) 2020-2026 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <stddef.h>
#include <stdint.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_logging.h"
#include "libtropic_port_mock.h"
#include "lt_functional_mock_tests.h"
#include "lt_l1.h"
#include "lt_l2_api_structs.h"
#include "lt_l2_frame_check.h"
#include "lt_mock_helpers.h"
#include "lt_test_common.h"

static const uint8_t app = TR01_L1_CHIP_MODE_READY_bit;
static const uint8_t startup = TR01_L1_CHIP_MODE_READY_bit | TR01_L1_CHIP_MODE_STARTUP_bit;
static const uint8_t alarm = TR01_L1_CHIP_MODE_ALARM_bit;

/**
 * @brief Enqueues the given CHIP_STATUS bytes as responses to consecutive Get_Response requests.
 *
 * @param h            Handle for communication with TROPIC01
 * @param chip_status  CHIP_STATUS bytes to enqueue, one per Get_Response request
 * @param count        Number of CHIP_STATUS bytes
 */
static void enqueue_chip_statuses(lt_handle_t *h, const uint8_t *chip_status, const size_t count)
{
    for (size_t i = 0; i < count; i++) {
        LT_TEST_ASSERT(LT_OK, lt_mock_hal_enqueue_response(&h->l2, &chip_status[i], 1));
    }
}

/**
 * @brief Enqueues responses to Startup_Req (L2 Request acknowledgement and L2 Response).
 *
 * @param h  Handle for communication with TROPIC01
 */
static void enqueue_startup_req(lt_handle_t *h)
{
    struct lt_l2_startup_rsp_t startup_req_resp = {.chip_status = app,
                                                   .status = TR01_L2_STATUS_REQUEST_OK,
                                                   .rsp_len = TR01_L2_STARTUP_RSP_LEN,
                                                   .crc = {0}};
    add_resp_crc(&startup_req_resp);

    LT_TEST_ASSERT(LT_OK, lt_mock_hal_enqueue_response(&h->l2, &app, sizeof(app)));
    LT_TEST_ASSERT(LT_OK, lt_mock_hal_enqueue_response(&h->l2, (uint8_t *)&startup_req_resp,
                                                       calc_mocked_resp_len(&startup_req_resp)));
}

void lt_test_mock_chip_status_mismatch(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_mock_chip_status_mismatch()");
    LT_LOG_INFO("----------------------------------------------");

    lt_mock_hal_reset(&h->l2);

    // lt_init() and lt_reboot() share the same start-up mitigation; lt_init() is checked by
    // mock_init_communication() expecting two CHIP_STATUS reads, the corruption is tested on
    // lt_reboot().
    LT_LOG_INFO("Mocking initialization...");
    LT_TEST_ASSERT(LT_OK,
                   mock_init_communication(h, &mock_chip_id_acab,
                                           (uint8_t[]){0x00, 0x00, 0x00, 0x02}));  // Version 2.0.0

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    LT_LOG_INFO("Mocking reboot with corrupted CHIP_STATUS (ALARM bit)...");
    enqueue_startup_req(h);
    enqueue_chip_statuses(h, (const uint8_t[]){alarm, app}, 2);
    LT_TEST_ASSERT(LT_OK, lt_reboot(h, TR01_REBOOT));

    LT_LOG_INFO("Mocking maintenance reboot with corrupted CHIP_STATUS (START bit)...");
    enqueue_startup_req(h);
    enqueue_chip_statuses(h, (const uint8_t[]){app, startup}, 2);
    LT_TEST_ASSERT(LT_OK, lt_reboot(h, TR01_MAINTENANCE_REBOOT));

    LT_LOG_INFO("Mocking reboot into ALARM Mode...");
    enqueue_startup_req(h);
    enqueue_chip_statuses(h, (const uint8_t[]){alarm, alarm}, 2);
    LT_TEST_ASSERT(LT_L1_CHIP_ALARM_MODE, lt_reboot(h, TR01_REBOOT));

    LT_LOG_INFO("Deinitializing handle");
    LT_TEST_ASSERT(LT_OK, lt_deinit(h));
}
