/**
 * @file lt_ex_macandd.c
 * @brief Example usage of TROPIC01 flagship feature - 'Mac And Destroy' PIN verification engine.
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <inttypes.h>

#include "libtropic.h"
#include "libtropic_examples.h"
#include "libtropic_logging.h"
#include "string.h"

// Needed to access to lt_random_bytes()
#include "lt_random.h"
// Needed to access HMAC_SHA256
#include "lt_hmac_sha256.h"

/** @brief Last slot in User memory used for storing of M&D related data (only in this example). */
#define R_MEM_DATA_SLOT_MACANDD (511)

/** @brief Size of the print buffer. */
#define PRINT_BUFF_SIZE 196

#ifndef MACANDD_ROUNDS
#define MACANDD_ROUNDS 12
#endif

#if (MACANDD_ROUNDS > 12)
#error \
    "MACANDD_ROUNDS must be less than 12 here, or generally than TR01_MACANDD_ROUNDS_MAX. Read explanation at the beginning of this file"
#endif

/** @brief Minimal size of MAC-and-Destroy additional data */
#define MAC_AND_DESTROY_ADD_SIZE_MIN 0
/** @brief Maximal size of MAC-and-Destroy additional data */
#define MAC_AND_DESTROY_ADD_SIZE_MAX 128u
/** @brief Minimal size of MAC-and-Destroy PIN input */
#define MAC_AND_DESTROY_PIN_SIZE_MIN 4u
/** @brief Maximal size of MAC-and-Destroy PIN input */
#define MAC_AND_DESTROY_PIN_SIZE_MAX 8u

/**
 * @brief This structure holds data used by host during MAC and Destroy sequence
 * Content of this struct must be stored in non-volatile memory, because it is used
 * between power cycles
 */
struct lt_macandd_nvm_t {
    uint8_t i;
    uint8_t ci[MACANDD_ROUNDS * 32];
    uint8_t t[32];
} __attribute__((__packed__));

/**
 * @brief Example function for setting PIN with Mac And Destroy.
 *
 * @details There are more ways how to implement Mac And Destroy 'PIN set' functionality, differences could be in way of
 * handling nvm data, number of tries, algorithm used for encryption, etc. This function is just one of the possible
 * implementations of "PIN set", therefore we do not expose this through official libtropic API.
 *
 *          Take it as an inspiration, copy it into your project and adapt it to your specific hw resources.
 *
 * @param h           Device's handle
 * @param PIN         Array of bytes (size between MAC_AND_DESTROY_PIN_SIZE_MIN and MAC_AND_DESTROY_PIN_SIZE_MAX)
 * representing PIN
 * @param PIN_size    Length of the PIN field
 * @param add         Additional data to be used in M&D sequence (size between MAC_AND_DESTROY_ADD_SIZE_MIN and
 * MAC_AND_DESTROY_ADD_SIZE_MAX)
 * @param add_size    Length of additional data
 * @param secret      Buffer into which secret will be placed when all went successfully
 * @return lt_ret_t   LT_OK if correct, otherwise LT_FAIL
 */
static lt_ret_t lt_PIN_set(lt_handle_t *h, const uint8_t *PIN, const uint8_t PIN_size, const uint8_t *add,
                           const uint8_t add_size, uint8_t *secret)
{
    if (!h || !PIN || (PIN_size < MAC_AND_DESTROY_PIN_SIZE_MIN) || (PIN_size > MAC_AND_DESTROY_PIN_SIZE_MAX) || !add
        || (add_size > MAC_AND_DESTROY_ADD_SIZE_MAX) || !secret) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Clear variable for released secret so there is known data (zeroes) in case this function ended sooner then secret
    // was prepared
    memset(secret, 0, 32);

    // Variable used during a process of getting a encryption key k_i
    uint8_t v[32] = {0};
    // Variable used during a process of getting a encryption key k_i
    uint8_t w[32] = {0};
    // Encryption key
    uint8_t k_i[32] = {0};
    // Random secret
    uint8_t s[32] = {0};
    // Variable used to initialize slot(s)
    uint8_t u[32] = {0};

    // This organizes data which will be stored into nvm
    struct lt_macandd_nvm_t nvm = {0};

    // User is expected to pass not only PIN, but might also pass another data(e.g. HW ID, ...)
    // Both arrays are concatenated and used together as an input for KDF
    uint8_t kdf_input_buff[MAC_AND_DESTROY_PIN_SIZE_MAX + MAC_AND_DESTROY_ADD_SIZE_MAX];
    memcpy(kdf_input_buff, PIN, PIN_size);
    memcpy(kdf_input_buff + PIN_size, add, add_size);

    LT_LOG_INFO("Generating random secret s...");
    lt_ret_t ret = lt_random_bytes(&h->l2, s, sizeof(s));
    if (ret != LT_OK) {
        LT_LOG_ERROR("Failed to get random bytes, ret=%s", lt_ret_verbose(ret));
        goto exit;
    }
    LT_LOG_INFO("\tOK");

    // Erase a slot in R memory, which will be used as a storage for NVM data
    LT_LOG_INFO("Erasing R_Mem User slot %d...", R_MEM_DATA_SLOT_MACANDD);
    ret = lt_r_mem_data_erase(h, R_MEM_DATA_SLOT_MACANDD);
    if (ret != LT_OK) {
        LT_LOG_ERROR("Failed to erase User slot, ret=%s", lt_ret_verbose(ret));
        goto exit;
    }
    LT_LOG_INFO("\tOK");

    // Store number of attempts
    nvm.i = MACANDD_ROUNDS;
    // Compute tag t = KDF(s, "0"), save into nvm struct
    // Tag will be later used during lt_PIN_check() to verify validity of secret
    lt_hmac_sha256(s, 32, (uint8_t *)"0", 1, nvm.t);

    // Compute u = KDF(s, "1")
    // This value will be sent through M&D sequence to initialize a slot
    lt_hmac_sha256(s, 32, (uint8_t *)"1", 1, u);

    // Compute v = KDF(0, PIN||A) where 0 is all zeroes key
    lt_hmac_sha256((uint8_t*)"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 32, kdf_input_buff, PIN_size+add_size, v);

    for (int i = 0; i < nvm.i; i++) {
        uint8_t garbage[32] = {0};

        // This call of a M&D sequence results in initialization of one slot
        LT_LOG_INFO("Doing M&D sequence to initialize a slot...");
        ret = lt_mac_and_destroy(h, i, u, garbage);
        if (ret != LT_OK) {
            LT_LOG_ERROR("Failed while doing M&D sequence, ret=%s", lt_ret_verbose(ret));
            goto exit;
        }
        LT_LOG_INFO("\tOK");

        // This call of a M&D sequence overwrites a previous slot, but key w is returned.
        // This key is later used to derive k_i (used to encrypt precious secret)
        LT_LOG_INFO("Doing M&D sequence to overwrite previous slot...");
        ret = lt_mac_and_destroy(h, i, v, w);
        if (ret != LT_OK) {
            LT_LOG_ERROR("Failed while doing M&D sequence, ret=%s", lt_ret_verbose(ret));
            goto exit;
        }
        LT_LOG_INFO("\tOK");

        // Now the slot is initialized again by calling M&D sequence again with 'u'
        LT_LOG_INFO("Doing M&D sequence again to initialize a slot...");
        ret = lt_mac_and_destroy(h, i, u, garbage);
        if (ret != LT_OK) {
            LT_LOG_ERROR("Failed while doing M&D sequence, ret=%s", lt_ret_verbose(ret));
            goto exit;
        }
        LT_LOG_INFO("\tOK");

        // Derive k_i = KDF(w, PIN||A)
        // This key will be used to encrypt secret s
        lt_hmac_sha256(w, 32, kdf_input_buff, PIN_size + add_size, k_i);

        // Encrypt s using k_i as a key
        //
        // Warning: All s[n] are encrypted here using k_i[n] as a key, results are stored into ci[n].
        // Because size of the 'message s' is the same as a size of the 'key k_i', simple XOR is used here - discuss
        // more apropriate method with experts on cryptography.
        for (int j = 0; j < 32; j++) {
            *(nvm.ci + (i * 32 + j)) = k_i[j] ^ s[j];
        }
    }

    // Persistently save nvm data into TROPIC01's R memory slot
    LT_LOG_INFO("Writing NVM data into R_Mem User slot %d...", R_MEM_DATA_SLOT_MACANDD);
    ret = lt_r_mem_data_write(h, R_MEM_DATA_SLOT_MACANDD, (uint8_t *)&nvm, sizeof(nvm));
    if (ret != LT_OK) {
        LT_LOG_ERROR("Failed to write User slot, ret=%s", lt_ret_verbose(ret));
        goto exit;
    }

    // Final secret is released to the caller
    lt_hmac_sha256(s, 32, (uint8_t *)"2", 1, secret);

// Cleanup all sensitive data from memory
exit:
    memset(kdf_input_buff, 0, PIN_size + add_size);
    memset(u, 0, 32);
    memset(v, 0, 32);
    memset(w, 0, 32);
    memset(k_i, 0, 32);

    return ret;
}

/**
 * @brief Example function for checking PIN with Mac And Destroy.
 *
 * @details There are more ways how to implement Mac And Destroy 'PIN check' functionality, differences could be in way
 * of handling nvm data, number of tries, algorithm used for decryption, etc. This function is just one of the possible
 * implementations of "PIN check", therefore we do not expose this through official libtropic API.
 *
 *          Take it as an inspiration, copy it into your project and adapt it to your specific hw resources.
 *
 * @param h           Device's handle
 * @param PIN         Array of bytes (size between MAC_AND_DESTROY_PIN_SIZE_MIN and MAC_AND_DESTROY_PIN_SIZE_MAX)
 * representing PIN
 * @param PIN_size    Length of the PIN field
 * @param add         Additional data to be used in M&D sequence (size between MAC_AND_DESTROY_ADD_SIZE_MIN and
 * MAC_AND_DESTROY_ADD_SIZE_MAX)
 * @param add_size    Length of additional data
 * @param secret      Buffer ito which secret will be saved
 * @return lt_ret_t   LT_OK if correct, otherwise LT_FAIL
 */
static lt_ret_t lt_PIN_check(lt_handle_t *h, const uint8_t *PIN, const uint8_t PIN_size, const uint8_t *add,
                             const uint8_t add_size, uint8_t *secret)
{
    if (!h || !PIN || (PIN_size < MAC_AND_DESTROY_PIN_SIZE_MIN) || (PIN_size > MAC_AND_DESTROY_PIN_SIZE_MAX) || !add
        || (add_size > MAC_AND_DESTROY_ADD_SIZE_MAX) || !secret) {
        return LT_PARAM_ERR;
    }
    if (h->l3.session != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Clear variable for released secret so there is known data (zeroes) in case this function ended sooner then secret
    // was prepared
    memset(secret, 0, 32);

    // Variable used during a process of getting a decryption key k_i
    uint8_t v_[32] = {0};
    // Variable used during a process of getting a decryption key k_i
    uint8_t w_[32] = {0};
    // Decryption key
    uint8_t k_i[32] = {0};
    // Secret
    uint8_t s_[32] = {0};
    // Tag
    uint8_t t_[32] = {0};
    // Value used to initialize Mac And Destroy's slot after a correct PIN try
    uint8_t u[32] = {0};

    // This organizes data which will be read from nvm
    struct lt_macandd_nvm_t nvm = {0};

    // User is expected to pass not only PIN, but might also pass another data(e.g. HW ID, ...)
    // Both arrays are concatenated and used together as an input for KDF
    uint8_t kdf_input_buff[MAC_AND_DESTROY_PIN_SIZE_MAX + MAC_AND_DESTROY_ADD_SIZE_MAX];
    memcpy(kdf_input_buff, PIN, PIN_size);
    memcpy(kdf_input_buff + PIN_size, add, add_size);

    // Load M&D data from TROPIC01's R memory
    LT_LOG_INFO("Reading M&D data from R_Mem User slot %d...", R_MEM_DATA_SLOT_MACANDD);
    uint16_t read_size;
    lt_ret_t ret = lt_r_mem_data_read(h, R_MEM_DATA_SLOT_MACANDD, (uint8_t *)&nvm, &read_size);
    if (ret != LT_OK) {
        LT_LOG_ERROR("Failed to read User slot, ret=%s", lt_ret_verbose(ret));
        goto exit;
    }
    LT_LOG_INFO("\tOK");

    // if i == 0: FAIL (no attempts remaining)
    LT_LOG_INFO("Checking if nvm.i != 0...");
    if (nvm.i == 0) {
        LT_LOG_ERROR("nvm.i == 0");
        goto exit;
    }
    LT_LOG_INFO("\tOK");

    // Decrement variable which holds number of tries
    // Let i = i - 1
    nvm.i--;

    // and store M&D data back to TROPIC01's R memory
    LT_LOG_INFO("Writing back M&D data into R_Mem User slot %d (erase, then write)...", R_MEM_DATA_SLOT_MACANDD);
    ret = lt_r_mem_data_erase(h, R_MEM_DATA_SLOT_MACANDD);
    if (ret != LT_OK) {
        LT_LOG_ERROR("Failed to erase User slot, ret=%s", lt_ret_verbose(ret));
        goto exit;
    }
    ret = lt_r_mem_data_write(h, R_MEM_DATA_SLOT_MACANDD, (uint8_t *)&nvm, sizeof(nvm));
    if (ret != LT_OK) {
        LT_LOG_ERROR("Failed to write User slot, ret=%s", lt_ret_verbose(ret));
        goto exit;
    }
    LT_LOG_INFO("\tOK");

    // Compute v’ = KDF(0, PIN’||A).
    lt_hmac_sha256((uint8_t*)"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 32, kdf_input_buff, PIN_size + add_size, v_);

    // Execute w’ = MACANDD(i, v’)
    LT_LOG_INFO("Doing M&D sequence...");
    ret = lt_mac_and_destroy(h, nvm.i, v_, w_);
    if (ret != LT_OK) {
        LT_LOG_ERROR("Failed while doing M&D sequence, ret=%s", lt_ret_verbose(ret));
        goto exit;
    }
    LT_LOG_INFO("\tOK");

    // Compute k’_i = KDF(w’, PIN’||A)
    lt_hmac_sha256(w_, 32, kdf_input_buff, PIN_size + add_size, k_i);

    // Read the ciphertext c_i and tag t from NVM, decrypt c_i with k’_i as the key and obtain s_

    // Warning: All ci[n] arrays were during PIN set phase encrypted by XORing k_i[n] and a key s[n].
    //
    // For getting s[n] again, they are decrypted here by XORing ci[n] with k_i[n]
    // Discuss more propriate method with experts on cryptography.
    for (int j = 0; j < 32; j++) {
        s_[j] = *(nvm.ci + (nvm.i * 32 + j)) ^ k_i[j];
    }

    // Compute tag t = KDF(s, "0x00")
    lt_hmac_sha256(s_, 32, (uint8_t *)"0", 1, t_);

    // If t’ != t: FAIL
    if (memcmp(nvm.t, t_, 32) != 0) {
        ret = LT_FAIL;
        goto exit;
    }

    // Pin is correct, now initialize macandd slots again:
    // Compute u = KDF(s’, "0x01")
    lt_hmac_sha256(s_, 32, (uint8_t *)"1", 1, u);

    for (int x = nvm.i; x < MACANDD_ROUNDS - 1; x++) {
        uint8_t garbage[32] = {0};

        LT_LOG_INFO("Doing M&D sequence...");
        ret = lt_mac_and_destroy(h, x, u, garbage);
        if (ret != LT_OK) {
            LT_LOG_ERROR("Failed while doing M&D sequence, ret=%s", lt_ret_verbose(ret));
            goto exit;
        }
        LT_LOG_INFO("\tOK");
    }

    // Set variable which holds number of tries back to initial state MACANDD_ROUNDS
    nvm.i = MACANDD_ROUNDS;

    // Store NVM data for future use
    LT_LOG_INFO("Writing M&D data into R_Mem User slot %d for future use (erase, then write)...",
                R_MEM_DATA_SLOT_MACANDD);
    ret = lt_r_mem_data_erase(h, R_MEM_DATA_SLOT_MACANDD);
    if (ret != LT_OK) {
        LT_LOG_ERROR("Failed to erase User slot, ret=%s", lt_ret_verbose(ret));
        goto exit;
    }
    ret = lt_r_mem_data_write(h, R_MEM_DATA_SLOT_MACANDD, (uint8_t *)&nvm, sizeof(nvm));
    if (ret != LT_OK) {
        LT_LOG_ERROR("Failed to write User slot, ret=%s", lt_ret_verbose(ret));
        goto exit;
    }
    LT_LOG_INFO("\tOK");

    // Calculate secret and store it into passed array
    lt_hmac_sha256(s_, 32, (uint8_t *)"2", 1, secret);

// Cleanup all sensitive data from memory
exit:
    memset(kdf_input_buff, 0, PIN_size + add_size);
    memset(w_, 0, 32);
    memset(k_i, 0, 32);
    memset(v_, 0, 32);

    return ret;
}

int lt_ex_macandd(lt_handle_t *h)
{
    LT_LOG_INFO("==========================================");
    LT_LOG_INFO("==== TROPIC01 Mac and Destroy Example ====");
    LT_LOG_INFO("==========================================");

    lt_ret_t ret;

    LT_LOG_INFO("Initializing handle");
    ret = lt_init(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to initialize handle, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }

    LT_LOG_INFO("Starting Secure Session with key %d", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    ret = lt_verify_chip_and_start_secure_session(h, sh0priv, sh0pub, TR01_PAIRING_KEY_SLOT_INDEX_0);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to start Secure Session with key %d, ret=%s", (int)TR01_PAIRING_KEY_SLOT_INDEX_0,
                     lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }

    // This variable stores secret which is released to the user after successful PIN check or PIN set
    uint8_t secret[32] = {0};

    // Additional data passed by user besides PIN - this is optional, but recommended
    uint8_t additional_data[]
        = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
           0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    uint8_t additional_data_size = sizeof(additional_data);

    // User's PIN
    uint8_t pin[] = {1, 2, 3, 4};
    uint8_t pin_wrong[] = {2, 2, 3, 4};

    LT_LOG_LINE();

    // Set the PIN and log out the secret
    LT_LOG("Setting the user PIN...");
    ret = lt_PIN_set(h, pin, 4, additional_data, additional_data_size, secret);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to set the user PIN, ret=%s", lt_ret_verbose(ret));
        lt_session_abort(h);
        lt_deinit(h);
        return -1;
    }
    LT_LOG_INFO("\tOK");
    char print_buff[PRINT_BUFF_SIZE];
    ret = lt_print_bytes(secret, 32, print_buff, PRINT_BUFF_SIZE);
    if (LT_OK != ret) {
        LT_LOG_ERROR("lt_print_bytes failed, ret=%s", lt_ret_verbose(ret));
        return -1;
    }
    LT_LOG_INFO("Initialized secret: %s", print_buff);
    LT_LOG_LINE();

    LT_LOG_INFO("Doing %d PIN check attempts with wrong PIN...", MACANDD_ROUNDS);
    for (int i = 1; i < MACANDD_ROUNDS; i++) {
        LT_LOG_INFO("\tInputting wrong PIN -> slot #%d destroyed", i);
        ret = lt_PIN_check(h, pin_wrong, 4, additional_data, additional_data_size, secret);
        if (LT_FAIL != ret) {
            LT_LOG_ERROR("Return value is not LT_FAIL, ret=%s", lt_ret_verbose(ret));
            lt_session_abort(h);
            lt_deinit(h);
            return -1;
        }
        ret = lt_print_bytes(secret, 32, print_buff, PRINT_BUFF_SIZE);
        if (LT_OK != ret) {
            LT_LOG_ERROR("lt_print_bytes failed, ret=%s", lt_ret_verbose(ret));
            return -1;
        }
        LT_LOG_INFO("\tSecret: %s", print_buff);
    }
    LT_LOG_INFO("\tOK");

    LT_LOG_INFO("Doing Final PIN attempt with correct PIN, slots are reinitialized again...");
    ret = lt_PIN_check(h, pin, 4, additional_data, additional_data_size, secret);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Attempt with correct PIN failed, ret=%s", lt_ret_verbose(ret));
        lt_session_abort(h);
        lt_deinit(h);
        return -1;
    }
    ret = lt_print_bytes(secret, 32, print_buff, PRINT_BUFF_SIZE);
    if (LT_OK != ret) {
        LT_LOG_ERROR("lt_print_bytes failed, ret=%s", lt_ret_verbose(ret));
        return -1;
    }
    LT_LOG_INFO("\tExported secret: %s", print_buff);
    LT_LOG_INFO("\tOK");
    LT_LOG_LINE();

    LT_LOG_INFO("Aborting Secure Session");
    ret = lt_session_abort(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to abort Secure Session, ret=%s", lt_ret_verbose(ret));
        return -1;
    }

    LT_LOG_INFO("Deinitializing handle");
    ret = lt_deinit(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to deinitialize handle, ret=%s", lt_ret_verbose(ret));
        return -1;
    }

    return 0;
}
