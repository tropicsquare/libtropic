/**
 * @file TROPIC01_hw_wallet.c
 * @brief Example usage of TROPIC01 chip in a generic *hardware wallet* project.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "string.h"
#include "inttypes.h"

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_examples.h"

/**
 * @name CONCEPT: Mac And Destroy
 * @note We recommend reading TROPIC01's datasheet before diving into this example!
 * @par
    This code aims to show an example usage
 */

/**
 * @brief Session with H0 pairing keys, pin is set and checked twice with macandd, two different pins
 *
 * @param h           Device's handle
 * @return            0 if success, otherwise -1
 */
static int session_H0(void)
{
    lt_handle_t h = {0};

    lt_init(&h);

    /* Establish secure handshake with default H1 pairing keys */
    LT_LOG("%s", "Creating session with H1 keys");
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, pkey_index_0));

    uint8_t in[100];
    uint8_t out[100];
    LT_LOG("%s", "lt_ping() ");
    LT_ASSERT(LT_OK, lt_ping(&h, out, in, 100));

    // Product of macandd
    uint8_t secret[32] = {0};


    LT_LOG("%s", "--------------------------------------------------\r\n");
    // User's pin 1
    uint8_t pin1[4] = {1,2,3,4};
    uint8_t pin1_wrong[4] = {1,2,3,5};

    // Pin set
    LT_LOG("%s", "lt_PIN_set() pin 1");
    LT_ASSERT(LT_OK, lt_PIN_set(&h, pin1, 4, secret));

    // Pin attempts with wrong pin
    for (int i = 0; i<MACANDD_ROUNDS-1; i++) {
        LT_LOG("%s", "lt_PIN_check() with wrong PIN");
        LT_ASSERT(LT_FAIL, lt_PIN_check(&h, pin1_wrong, 4, secret));
    }

    // Final pin attempt with correct pin, slots should be reinitialized again
    LT_LOG("%s", "lt_PIN_check() with correct PIN");
    LT_ASSERT(LT_OK, lt_PIN_check(&h, pin1, 4, secret));

    // Pin attempt with wrong pin
    LT_LOG("%s", "lt_PIN_check() with wrong PIN");
    LT_ASSERT(LT_FAIL, lt_PIN_check(&h, pin1_wrong, 4, secret));

    // Pin attempt with correct pin
    LT_LOG("%s", "lt_PIN_check() with correct PIN");
    LT_ASSERT(LT_OK, lt_PIN_check(&h, pin1, 4, secret));

    LT_LOG("%s", "--------------------------------------------------\r\n");
    // User's pin 2
    uint8_t pin2[4] = {9,8,7,6};
    uint8_t pin2_wrong[4] = {9,8,7,5};

    // Pin set
    LT_LOG("%s", "lt_PIN_set() pin 2");
    LT_ASSERT(LT_OK, lt_PIN_set(&h, pin2, 4, secret));

    // Pin attempts with wrong pin
    for (int i = 0; i<MACANDD_ROUNDS-1; i++) {
        LT_LOG("%s", "lt_PIN_check() with wrong PIN");
        LT_ASSERT(LT_FAIL, lt_PIN_check(&h, pin2_wrong, 4, secret));
    }

    // Final pin attempt with correct pin, slots should be reinitialized again
    LT_LOG("%s", "lt_PIN_check() with correct PIN");
    LT_ASSERT(LT_OK, lt_PIN_check(&h, pin2, 4, secret));

    // Pin attempt with wrong pin
    LT_LOG("%s", "lt_PIN_check() with wrong PIN");
    LT_ASSERT(LT_FAIL, lt_PIN_check(&h, pin2_wrong, 4, secret));

    // Pin attempt with correct pin
    LT_LOG("%s", "lt_PIN_check() with correct PIN");
    LT_ASSERT(LT_OK, lt_PIN_check(&h, pin2, 4, secret));

    LT_LOG("%s", "Aborting session H1");
    LT_ASSERT(LT_OK, lt_session_abort(&h));

    lt_deinit(&h);

    return LT_OK;
}

int lt_ex_macandd(void)
{
    LT_LOG("\r\n");
    LT_LOG("\t=======================================================================\r\n");
    LT_LOG("\t=====  TROPIC01 example - Mac And Destroy                           ===\r\n");
    LT_LOG("\t=======================================================================\r\n\n");

    LT_LOG_LINE();
    printf("\t Session initial: \r\n\n");
    if(session_H0() == -1) {
        printf("\r\nError during session_H0()\r\n");
        return -1;
    }

   LT_LOG_LINE();

    return 0;
}
