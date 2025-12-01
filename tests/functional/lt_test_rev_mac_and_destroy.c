/**
 * @file lt_test_rev_mac_and_destroy.c
 * @brief Tests MAC_And_Destroy command using HMAC-SHA256 as the Key Derivation Function (KDF).
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <inttypes.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_functional_tests.h"
#include "libtropic_logging.h"
#include "lt_hmac_sha256.h"
#include "lt_port_wrap.h"
#include "lt_test_common.h"
#include "string.h"

#define PIN_LEN_MAX 2048

uint8_t kdf_key_zeros[256] = {0};

static int pin_check(lt_handle_t *h, uint8_t *pin, uint16_t pin_len, lt_mac_and_destroy_slot_t slot,
                     uint8_t ciphertexts[TR01_MACANDD_ROUNDS_MAX][TR01_MAC_AND_DESTROY_DATA_SIZE],
                     uint8_t t[LT_HMAC_SHA256_HASH_LEN], uint8_t s[TR01_MAC_AND_DESTROY_DATA_SIZE])
{
    uint8_t v[LT_HMAC_SHA256_HASH_LEN], w[TR01_MAC_AND_DESTROY_DATA_SIZE], k_i[LT_HMAC_SHA256_HASH_LEN],
        t_[LT_HMAC_SHA256_HASH_LEN];

    LT_LOG_INFO("Computing v = KDF(0, PIN_DATA)...");
    LT_TEST_ASSERT(LT_OK, lt_hmac_sha256(kdf_key_zeros, sizeof(kdf_key_zeros), pin, pin_len, v));

    LT_LOG_INFO("Executing MAC_And_Destroy with v and slot #%d...", (int)slot);
    LT_TEST_ASSERT(LT_OK, lt_mac_and_destroy(h, slot, v, w));

    LT_LOG_INFO("Computing k_i = KDF(w, PIN_DATA)...");
    LT_TEST_ASSERT(LT_OK, lt_hmac_sha256(w, sizeof(w), pin, pin_len, k_i));

    LT_LOG_INFO("Decrypting (XOR) c_i using k_i...");
    for (uint8_t j = 0; j < TR01_MAC_AND_DESTROY_DATA_SIZE; j++) s[j] = ciphertexts[slot][j] ^ k_i[j];

    LT_LOG_INFO("Computing t' = KDF(s, \"0\")...");
    LT_TEST_ASSERT(LT_OK, lt_hmac_sha256(s, TR01_MAC_AND_DESTROY_DATA_SIZE, (uint8_t *)"0", 1, t_));

    LT_LOG_INFO("Checking if t' != t...");
    for (uint8_t i = 0; i < sizeof(t_); i++)
        if (t_[i] != t[i]) return 1;

    return 0;
}

void lt_test_rev_mac_and_destroy(lt_handle_t *h)
{
    LT_LOG_INFO("----------------------------------------------");
    LT_LOG_INFO("lt_test_rev_mac_and_destroy()");
    LT_LOG_INFO("----------------------------------------------");

    uint8_t n, wrong_attempts, s[TR01_MAC_AND_DESTROY_DATA_SIZE], t[LT_HMAC_SHA256_HASH_LEN],
        u[LT_HMAC_SHA256_HASH_LEN], v[LT_HMAC_SHA256_HASH_LEN], w[TR01_MAC_AND_DESTROY_DATA_SIZE],
        k_from_setup[LT_HMAC_SHA256_HASH_LEN], k_from_check[LT_HMAC_SHA256_HASH_LEN], k_i[LT_HMAC_SHA256_HASH_LEN],
        ignored[TR01_MAC_AND_DESTROY_DATA_SIZE], pin[PIN_LEN_MAX], pin_wrong[PIN_LEN_MAX],
        ciphertexts[TR01_MACANDD_ROUNDS_MAX][TR01_MAC_AND_DESTROY_DATA_SIZE];
    uint16_t pin_len;

    LT_LOG_INFO("Initializing handle");
    LT_TEST_ASSERT(LT_OK, lt_init(h));

    LT_LOG_INFO("Starting Secure Session with key %d", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    LT_TEST_ASSERT(LT_OK, lt_verify_chip_and_start_secure_session(h, LT_TEST_SH0_PRIV, LT_TEST_SH0_PUB,
                                                                  TR01_PAIRING_KEY_SLOT_INDEX_0));
    LT_LOG_LINE();

    LT_LOG_INFO("Setup PIN");
    LT_LOG_INFO();

    LT_LOG_INFO("Generating random number of max attempts n from {1...%d}...", (int)TR01_MAC_AND_DESTROY_SLOT_127);
    LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, &n, sizeof(n)));
    n = (n % TR01_MAC_AND_DESTROY_SLOT_127) + 1;

    LT_LOG_INFO("Generating random 32B secret s...");
    LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, s, sizeof(s)));

    LT_LOG_INFO("Generating random length from {1...%d} for the PIN...", PIN_LEN_MAX);
    LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, &pin_len, sizeof(pin_len)));
    pin_len = (pin_len % PIN_LEN_MAX) + 1;

    LT_LOG_INFO("Generating random PIN with length %" PRIu16 "...", pin_len);
    LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, pin, sizeof(pin)));

    LT_LOG_INFO("Computing t = KDF(s, \"0\")...");
    LT_TEST_ASSERT(LT_OK, lt_hmac_sha256(s, sizeof(s), (uint8_t *)"0", 1, t));

    LT_LOG_INFO("Computing u = KDF(s, \"1\")...");
    LT_TEST_ASSERT(LT_OK, lt_hmac_sha256(s, sizeof(s), (uint8_t *)"1", 1, u));

    LT_LOG_INFO("Computing v = KDF(0, PIN_DATA)...");
    LT_TEST_ASSERT(LT_OK, lt_hmac_sha256(kdf_key_zeros, sizeof(kdf_key_zeros), pin, pin_len, v));

    LT_LOG_INFO("Starting n=%" PRIu8 " blocks of MAC_And_Destroy sequences", n);
    for (uint8_t i = 0; i < n; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Executing MAC_And_Destroy with u and slot #%" PRIu8 "...", i);
        LT_TEST_ASSERT(LT_OK, lt_mac_and_destroy(h, i, u, ignored));

        LT_LOG_INFO("Executing MAC_And_Destroy with v and slot #%" PRIu8 "...", i);
        LT_TEST_ASSERT(LT_OK, lt_mac_and_destroy(h, i, v, w));

        LT_LOG_INFO("Executing MAC_And_Destroy with u and slot #%" PRIu8 "...", i);
        LT_TEST_ASSERT(LT_OK, lt_mac_and_destroy(h, i, u, ignored));

        LT_LOG_INFO("Computing k_i = KDF(w, PIN_DATA)...");
        LT_TEST_ASSERT(LT_OK, lt_hmac_sha256(w, sizeof(w), pin, pin_len, k_i));

        LT_LOG_INFO("Encrypting (XOR) s using k_i...");
        for (uint8_t j = 0; j < sizeof(ciphertexts[i]); j++) {
            ciphertexts[i][j] = s[j] ^ k_i[j];
        }
    }
    LT_LOG_LINE();
    // Compute the cryptographic key k
    LT_TEST_ASSERT(LT_OK, lt_hmac_sha256(s, sizeof(s), (uint8_t *)"2", 1, k_from_setup));

    LT_LOG_INFO("Check PIN");
    LT_LOG_INFO();

    LT_LOG_INFO("Generating a random number of wrong attempts from {0...n-1=%" PRIu8 "}...", n - 1);
    LT_TEST_ASSERT(LT_OK, lt_random_bytes(h, &wrong_attempts, sizeof(wrong_attempts)));
    wrong_attempts %= n;

    memcpy(pin_wrong, pin, pin_len);
    pin_wrong[0] = ~pin_wrong[0];  // make the PIN wrong
    LT_LOG_INFO("Starting %" PRIu8 " wrong attempts", wrong_attempts);
    for (uint8_t i = 0; i < wrong_attempts; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Doing wrong attempt #%" PRIu8 "...", i);
        LT_TEST_ASSERT(1, pin_check(h, pin_wrong, pin_len, i, ciphertexts, t, s));
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Checking PIN with the first undestroyed slot #%" PRIu8 "...", wrong_attempts);
    LT_LOG_INFO();
    LT_TEST_ASSERT(0, pin_check(h, pin, pin_len, wrong_attempts, ciphertexts, t, s));

    LT_LOG_INFO("Comparing cryptographic key k to the one from the setup phase...");
    // Compute the cryptographic key k
    LT_TEST_ASSERT(LT_OK, lt_hmac_sha256(s, sizeof(s), (uint8_t *)"2", 1, k_from_check));
    for (uint8_t i = 0; i < sizeof(k_from_setup); i++) {
        LT_TEST_ASSERT(1, (k_from_setup[i] == k_from_check[i]));
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Starting a restoration of destroyed slots");
    LT_LOG_INFO();
    LT_LOG_INFO("Computing u = KDF(s, \"1\")...");
    LT_TEST_ASSERT(LT_OK, lt_hmac_sha256(s, sizeof(s), (uint8_t *)"1", 1, u));

    for (uint8_t i = 0; i <= wrong_attempts; i++) {
        LT_LOG_INFO("Restoring slot #%" PRIu8 "...", i);
        LT_TEST_ASSERT(LT_OK, lt_mac_and_destroy(h, i, u, ignored));
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Checking PIN with all used slots...");
    for (uint8_t i = 0; i < n; i++) {
        LT_LOG_INFO();
        LT_LOG_INFO("Doing an attempt with the correct PIN with slot #%" PRIu8 "...", i);
        LT_TEST_ASSERT(0, pin_check(h, pin, pin_len, i, ciphertexts, t, s));

        LT_LOG_INFO("Comparing cryptographic key k to the one from the setup phase...");
        // Compute the cryptographic key k
        LT_TEST_ASSERT(LT_OK, lt_hmac_sha256(s, sizeof(s), (uint8_t *)"2", 1, k_from_check));
        for (uint8_t j = 0; j < sizeof(k_from_setup); j++) {
            LT_TEST_ASSERT(1, (k_from_setup[j] == k_from_check[j]));
        }
    }
    LT_LOG_LINE();

    LT_LOG_INFO("Aborting Secure Session");
    LT_TEST_ASSERT(LT_OK, lt_session_abort(h));

    LT_LOG_INFO("Deinitializing handle");
    LT_TEST_ASSERT(LT_OK, lt_deinit(h));
}