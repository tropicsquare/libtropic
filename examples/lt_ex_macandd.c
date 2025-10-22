/**
 * @file lt_ex_macandd.c
 * @brief Example usage of TROPIC01 flagship feature - 'Mac And Destroy' PIN verification engine.
 * For more info please refer to ODN_TR01_app_002_pin_verif.pdf
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
/** @brief The size of random data passed during MAC-and-Destroy PIN set */
#define TR01_MAC_AND_DESTROY_MASTER_SECRET_SIZE 32u

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
    uint8_t ci[MACANDD_ROUNDS * TR01_MAC_AND_DESTROY_DATA_SIZE];
    uint8_t t[LT_HMAC_SHA256_HASH_LEN];
} __attribute__((__packed__));

/**
 * @brief Simple XOR "encryption" function. Replace with another encryption algorithm if needed.
 *
 * @param data         32B of data to be encrypted
 * @param key          32B key used for encryption
 * @param destination  Buffer into which 32B of encrypted data will be placed
 */
static void encrypt(const uint8_t *data, const uint8_t *key, uint8_t *destination)
{
    for (uint8_t i = 0; i < 32; i++) {
        destination[i] = data[i] ^ key[i];
    }
}

/**
 * @brief Simple XOR "decryption" function. Replace with another decryption algorithm if needed.
 *
 * @param data         32B of data to be decrypted
 * @param key          32B key used for decryption
 * @param destination  Buffer into which 32B of decrypted data will be placed
 */
static void decrypt(const uint8_t *data, const uint8_t *key, uint8_t *destination)
{
    for (uint8_t i = 0; i < 32; i++) {
        destination[i] = data[i] ^ key[i];  //*(data + i) ^= *(key + i);
    }
}

/**
 * @brief Example function for setting PIN with Mac And Destroy.
 *
 * @details The New PIN Setup procedure takes the user PIN, add data and high entropy master_secret as an input,
 * initializes the scheme slots and returns a 32-byte key final_key as derivative of the master_secret.
 *
 * The MAC-and-Destroy PIN veriﬁcation scheme uses slots located in the TROPIC01’s ﬂash memory – one slot per
 * PIN entry attempt. These slots are ﬁrst initialized when a new PIN is being set up.
 * The slots are then invalidated (destroyed) one by one with each PIN entry attempt. When the correct PIN is
 * entered, the slots are initialized again, therefore the PIN entry limit is reset.
 * PIN entry attempt fails if:
 *  * PIN is invalid
 *  * The current slot is not initialized for a given PIN
 *  * The current slot is destroyed by previous invalid PIN entry attempt.
 *
 * There are more ways how to implement Mac And Destroy 'PIN set' functionality, differences could be in way of
 * handling nvm data, number of tries, algorithm used for encryption, etc. This function is just one of the possible
 * implementations of "PIN set".
 *
 * Take it as an inspiration, copy it into your project and adapt it to your specific hw resources.
 *
 * @param h           Device's handle
 * @param master_secret  32 bytes of random data (determines final_key)
 * @param PIN         Array of bytes (size between MAC_AND_DESTROY_PIN_SIZE_MIN and MAC_AND_DESTROY_PIN_SIZE_MAX)
 * representing PIN
 * @param PIN_size    Length of the PIN field
 * @param add         Additional data to be used in M&D sequence (size between MAC_AND_DESTROY_ADD_SIZE_MIN and
 * MAC_AND_DESTROY_ADD_SIZE_MAX). Pass NULL if no additional data should be used.
 * @param add_size    Length of additional data
 * @param final_key      Buffer into which final key will be placed when all went successfully
 * @return lt_ret_t   LT_OK if correct, otherwise LT_FAIL
 */
static lt_ret_t lt_new_PIN_setup(lt_handle_t *h, const uint8_t *master_secret, const uint8_t *PIN,
                                 const uint8_t PIN_size, const uint8_t *add, const uint8_t add_size, uint8_t *final_key)
{
    if (!h || !master_secret || !PIN || (PIN_size < MAC_AND_DESTROY_PIN_SIZE_MIN)
        || (PIN_size > MAC_AND_DESTROY_PIN_SIZE_MAX) || (add_size > MAC_AND_DESTROY_ADD_SIZE_MAX) || !final_key) {
        // add parameter is not checked for NULL, because it can be NULL (handled in the lines below)
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    uint8_t add_size_checked = add_size;
    if (!add) {
        add_size_checked = 0;
    }

    // Clear variable for released final_key so there is known data (zeroes) in case this function ended sooner then
    // final_key was prepared
    memset(final_key, 0, TR01_MAC_AND_DESTROY_DATA_SIZE);

    // Variable used during a process of getting a encryption key k_i
    uint8_t v[LT_HMAC_SHA256_HASH_LEN] = {0};
    // Variable used during a process of getting a encryption key k_i
    uint8_t w_i[TR01_MAC_AND_DESTROY_DATA_SIZE] = {0};
    // Encryption key
    uint8_t k_i[LT_HMAC_SHA256_HASH_LEN] = {0};
    // Variable used to initialize slot(s)
    uint8_t u[LT_HMAC_SHA256_HASH_LEN] = {0};

    // This organizes data which will be stored into nvm
    struct lt_macandd_nvm_t nvm = {0};

    // User is expected to pass not only PIN, but might also pass another data (e.g. HW ID, ...)
    // Both arrays are concatenated and used together as an input for KDF
    uint8_t kdf_input_buff[MAC_AND_DESTROY_PIN_SIZE_MAX + MAC_AND_DESTROY_ADD_SIZE_MAX];
    memcpy(kdf_input_buff, PIN, PIN_size);
    if (!add || add_size_checked == 0) {
        LT_LOG_INFO("No additional data will be used in the following M&D sequence");
    }
    else {
        memcpy(kdf_input_buff + PIN_size, add, add_size_checked);
    }

    // Erase a slot in R memory, which will be used as a storage for NVM data
    LT_LOG_INFO("Erasing R_Mem User slot %d...", R_MEM_DATA_SLOT_MACANDD);
    lt_ret_t ret = lt_r_mem_data_erase(h, R_MEM_DATA_SLOT_MACANDD);
    if (ret != LT_OK) {
        LT_LOG_ERROR("Failed to erase User slot, ret=%s", lt_ret_verbose(ret));
        goto exit;
    }
    LT_LOG_INFO("\tOK");

    // Store number of attempts
    nvm.i = MACANDD_ROUNDS;
    // Compute tag t = KDF(s, 0x00), save into nvm struct
    // Tag will be later used during lt_PIN_entry_check() to verify validity of final_key
    ret = lt_hmac_sha256(master_secret, TR01_MAC_AND_DESTROY_MASTER_SECRET_SIZE, (uint8_t[]){0x00}, 1, nvm.t);
    if (ret != LT_OK) {
        LT_LOG_ERROR("t = KDF(s, 0x00) failed, ret=%s", lt_ret_verbose(ret));
        goto exit;
    }

    // Compute u = KDF(s, 0x01)
    // This value will be sent through M&D sequence to initialize a slot
    ret = lt_hmac_sha256(master_secret, TR01_MAC_AND_DESTROY_MASTER_SECRET_SIZE, (uint8_t[]){0x01}, 1, u);
    if (ret != LT_OK) {
        LT_LOG_ERROR("u = KDF(s, 0x01) failed, ret=%s", lt_ret_verbose(ret));
        goto exit;
    }

    // Compute v = KDF(0, PIN||A) where 0 is all zeroes key
    const uint8_t zeros[32] = {0};
    ret = lt_hmac_sha256(zeros, sizeof(zeros), kdf_input_buff, PIN_size + add_size_checked, v);
    if (ret != LT_OK) {
        LT_LOG_ERROR("v = KDF(0, PIN||A) failed, ret=%s", lt_ret_verbose(ret));
        goto exit;
    }

    for (int i = 0; i < nvm.i; i++) {
        uint8_t ignore[TR01_MAC_AND_DESTROY_DATA_SIZE] = {0};

        // This call of a M&D sequence results in initialization of one slot
        LT_LOG_INFO("Doing M&D sequence to initialize a slot...");
        ret = lt_mac_and_destroy(h, i, u, ignore);
        if (ret != LT_OK) {
            LT_LOG_ERROR("Failed while doing M&D sequence, ret=%s", lt_ret_verbose(ret));
            goto exit;
        }
        LT_LOG_INFO("\tOK");

        // This call of a M&D sequence overwrites a previous slot, but key w is returned.
        // This key is later used to derive k_i (used to encrypt precious final_key)
        LT_LOG_INFO("Doing M&D sequence to overwrite previous slot...");
        ret = lt_mac_and_destroy(h, i, v, w_i);
        if (ret != LT_OK) {
            LT_LOG_ERROR("Failed while doing M&D sequence, ret=%s", lt_ret_verbose(ret));
            goto exit;
        }
        LT_LOG_INFO("\tOK");

        // Now the slot is initialized again by calling M&D sequence again with 'u'
        LT_LOG_INFO("Doing M&D sequence again to initialize a slot...");
        ret = lt_mac_and_destroy(h, i, u, ignore);
        if (ret != LT_OK) {
            LT_LOG_ERROR("Failed while doing M&D sequence, ret=%s", lt_ret_verbose(ret));
            goto exit;
        }
        LT_LOG_INFO("\tOK");

        // Derive k_i = KDF(w_i, PIN||A); k_i will be used to encrypt master_secret
        ret = lt_hmac_sha256(w_i, sizeof(w_i), kdf_input_buff, PIN_size + add_size_checked, k_i);
        if (ret != LT_OK) {
            LT_LOG_ERROR("k_i = KDF(w_i, PIN||A) failed, ret=%s", lt_ret_verbose(ret));
            goto exit;
        }

        // Encrypt master_secret using k_i as a key and store ciphertext into non volatile storage
        encrypt(master_secret, k_i, nvm.ci + (i * TR01_MAC_AND_DESTROY_DATA_SIZE));
    }

    // Persistently save nvm data into TROPIC01's R memory slot
    LT_LOG_INFO("Writing NVM data into R_Mem User slot %d...", R_MEM_DATA_SLOT_MACANDD);
    ret = lt_r_mem_data_write(h, R_MEM_DATA_SLOT_MACANDD, (uint8_t *)&nvm, sizeof(nvm));
    if (ret != LT_OK) {
        LT_LOG_ERROR("Failed to write User slot, ret=%s", lt_ret_verbose(ret));
        goto exit;
    }

    // final_key is released to the caller
    ret = lt_hmac_sha256(master_secret, TR01_MAC_AND_DESTROY_MASTER_SECRET_SIZE, (uint8_t *)"2", 1, final_key);
    if (ret != LT_OK) {
        LT_LOG_ERROR("Fail during last computation of final_key, ret=%s", lt_ret_verbose(ret));
        goto exit;
    }

// Cleanup all sensitive data from memory
exit:
    memset(kdf_input_buff, 0, PIN_size + add_size_checked);
    memset(u, 0, sizeof(u));
    memset(v, 0, sizeof(v));
    memset(w_i, 0, sizeof(w_i));
    memset(k_i, 0, sizeof(k_i));

    return ret;
}

/**
 * @brief Example function for checking PIN with Mac And Destroy.
 *
 * @details The Pin Entry Check procedure takes the PIN and additional add data entered by the user as an input, and
 * checks the PIN. If successful, the correct key k is returned.
 *
 * There are more ways how to implement Mac And Destroy 'PIN check' functionality, differences could be in way
 * of handling nvm data, number of tries, algorithm used for decryption, etc. This function is just one of the possible
 * implementations of "PIN check".
 *
 * Take it as an inspiration, copy it into your project and adapt it to your specific hw resources.
 *
 * @param h           Device's handle
 * @param PIN         Array of bytes (size between MAC_AND_DESTROY_PIN_SIZE_MIN and MAC_AND_DESTROY_PIN_SIZE_MAX)
 * representing PIN
 * @param PIN_size    Length of the PIN field
 * @param add         Additional data to be used in M&D sequence (size between MAC_AND_DESTROY_ADD_SIZE_MIN and
 * MAC_AND_DESTROY_ADD_SIZE_MAX). Pass NULL if no additional data should be used.
 * @param add_size    Length of additional data
 * @param final_key   Buffer into which final_key will be saved
 * @return lt_ret_t   LT_OK if correct, otherwise LT_FAIL
 */
static lt_ret_t lt_PIN_entry_check(lt_handle_t *h, const uint8_t *PIN, const uint8_t PIN_size, const uint8_t *add,
                                   const uint8_t add_size, uint8_t *final_key)
{
    if (!h || !PIN || (PIN_size < MAC_AND_DESTROY_PIN_SIZE_MIN) || (PIN_size > MAC_AND_DESTROY_PIN_SIZE_MAX)
        || (add_size > MAC_AND_DESTROY_ADD_SIZE_MAX) || !final_key) {
        // add parameter is not checked for NULL, because it can be NULL (handled in the lines below)
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    uint8_t add_size_checked = add_size;
    if (!add) {
        add_size_checked = 0;
    }

    // Clear variable for released final_key so there is known data (zeroes) in case this function ended sooner then
    // final_key was prepared
    memset(final_key, 0, TR01_MAC_AND_DESTROY_DATA_SIZE);

    // Variable used during a process of getting a decryption key k_i
    uint8_t v_[LT_HMAC_SHA256_HASH_LEN] = {0};
    // Variable used during a process of getting a decryption key k_i
    uint8_t w_i[TR01_MAC_AND_DESTROY_DATA_SIZE] = {0};
    // Decryption key
    uint8_t k_i[LT_HMAC_SHA256_HASH_LEN] = {0};
    // Secret
    uint8_t s_[TR01_MAC_AND_DESTROY_DATA_SIZE] = {0};
    // Tag
    uint8_t t_[LT_HMAC_SHA256_HASH_LEN] = {0};
    // Value used to initialize Mac And Destroy's slot after a correct PIN try
    uint8_t u[LT_HMAC_SHA256_HASH_LEN] = {0};

    // This organizes data which will be read from nvm
    struct lt_macandd_nvm_t nvm = {0};

    // User might pass not only PIN, but also another data(e.g. HW ID, ...) if needed
    // Both arrays are concatenated and used together as an input for KDF
    uint8_t kdf_input_buff[MAC_AND_DESTROY_PIN_SIZE_MAX + MAC_AND_DESTROY_ADD_SIZE_MAX];
    memcpy(kdf_input_buff, PIN, PIN_size);
    if (!add || add_size_checked == 0) {
        LT_LOG_INFO("No additional data will be used in the following M&D sequence");
    }
    else {
        memcpy(kdf_input_buff + PIN_size, add, add_size_checked);
    }

    // Load M&D data from TROPIC01's R memory
    LT_LOG_INFO("Reading M&D data from R_Mem User slot %d...", R_MEM_DATA_SLOT_MACANDD);
    uint16_t read_size;
    lt_ret_t ret = lt_r_mem_data_read(h, R_MEM_DATA_SLOT_MACANDD, (uint8_t *)&nvm, sizeof(nvm), &read_size);
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
    const uint8_t zeros[32] = {0};
    ret = lt_hmac_sha256(zeros, sizeof(zeros), kdf_input_buff, PIN_size + add_size_checked, v_);
    if (ret != LT_OK) {
        LT_LOG_ERROR("v' = KDF(0, PIN'||A) failed, ret=%s", lt_ret_verbose(ret));
        goto exit;
    }

    // Execute w’ = MACANDD(i, v’)
    LT_LOG_INFO("Doing M&D sequence...");
    ret = lt_mac_and_destroy(h, nvm.i, v_, w_i);
    if (ret != LT_OK) {
        LT_LOG_ERROR("Failed while doing M&D sequence, ret=%s", lt_ret_verbose(ret));
        goto exit;
    }
    LT_LOG_INFO("\tOK");

    // Compute k’_i = KDF(w’, PIN’||A)
    ret = lt_hmac_sha256(w_i, sizeof(w_i), kdf_input_buff, PIN_size + add_size_checked, k_i);
    if (ret != LT_OK) {
        LT_LOG_ERROR("k'_i = KDF(w', PIN'||A) failed, ret=%s", lt_ret_verbose(ret));
        goto exit;
    }

    // Read the ciphertext c_i and tag t from NVM,
    // decrypt c_i with k’_i as the key and obtain s_
    decrypt(nvm.ci + (nvm.i * TR01_MAC_AND_DESTROY_DATA_SIZE), k_i, s_);

    // Compute tag t = KDF(s_, "0x00")
    ret = lt_hmac_sha256(s_, sizeof(s_), (uint8_t[]){0x00}, 1, t_);
    if (ret != LT_OK) {
        LT_LOG_ERROR("t = KDF(s_, \"0x00\") failed, ret=%s", lt_ret_verbose(ret));
        goto exit;
    }

    // If t’ != t: FAIL
    if (memcmp(nvm.t, t_, sizeof(t_)) != 0) {
        ret = LT_FAIL;
        goto exit;
    }

    // Pin is correct, now initialize macandd slots again:
    // Compute u = KDF(s’, "0x01")
    ret = lt_hmac_sha256(s_, sizeof(s_), (uint8_t[]){0x01}, 1, u);
    if (ret != LT_OK) {
        LT_LOG_ERROR("u = KDF(s', \"0x01\") failed, ret=%s", lt_ret_verbose(ret));
        goto exit;
    }

    for (int x = nvm.i; x < MACANDD_ROUNDS - 1; x++) {
        uint8_t ignore[TR01_MAC_AND_DESTROY_DATA_SIZE] = {0};

        LT_LOG_INFO("Doing M&D sequence...");
        ret = lt_mac_and_destroy(h, x, u, ignore);
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

    // Calculate final_key and store it into passed array
    ret = lt_hmac_sha256(s_, sizeof(s_), (uint8_t *)"2", 1, final_key);
    if (ret != LT_OK) {
        LT_LOG_ERROR("Fail during last computation of final_key, ret=%s", lt_ret_verbose(ret));
        goto exit;
    }

// Cleanup all sensitive data from memory
exit:
    memset(kdf_input_buff, 0, PIN_size + add_size_checked);
    memset(w_i, 0, sizeof(w_i));
    memset(k_i, 0, sizeof(k_i));
    memset(v_, 0, sizeof(v_));

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
    ret = lt_verify_chip_and_start_secure_session(h, LT_EX_SH0_PRIV, LT_EX_SH0_PUB, TR01_PAIRING_KEY_SLOT_INDEX_0);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to start Secure Session with key %d, ret=%s", (int)TR01_PAIRING_KEY_SLOT_INDEX_0,
                     lt_ret_verbose(ret));
        lt_deinit(h);
        return -1;
    }

    // This variable stores final_key which is released to the user after successful PIN check or PIN set
    uint8_t final_key_initialized[TR01_MAC_AND_DESTROY_DATA_SIZE] = {0};

    // Additional data passed by user besides PIN - this is optional, but recommended
    uint8_t additional_data[]
        = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
           0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

    // User's PIN
    uint8_t pin[] = {1, 2, 3, 4};
    uint8_t pin_wrong[] = {2, 2, 3, 4};

    LT_LOG_LINE();

    LT_LOG_INFO("Initializing Mac And Destroy");
    LT_LOG_INFO("Generating random master_secret...");
    uint8_t master_secret[TR01_MAC_AND_DESTROY_MASTER_SECRET_SIZE] = {0};
    ret = lt_random_bytes(h, master_secret, TR01_MAC_AND_DESTROY_MASTER_SECRET_SIZE);
    if (ret != LT_OK) {
        LT_LOG_ERROR("Failed to get random bytes, ret=%s", lt_ret_verbose(ret));
        lt_session_abort(h);
        lt_deinit(h);
        return -1;
    }
    LT_LOG_INFO("\tOK");
    char print_buff[PRINT_BUFF_SIZE];
    ret = lt_print_bytes(master_secret, sizeof(master_secret), print_buff, PRINT_BUFF_SIZE);
    if (LT_OK != ret) {
        LT_LOG_ERROR("lt_print_bytes failed, ret=%s", lt_ret_verbose(ret));
        lt_session_abort(h);
        lt_deinit(h);
        return -1;
    }
    LT_LOG_INFO("Generated master_secret: %s", print_buff);

    // Set the PIN and log out the final_key
    LT_LOG("Setting the user PIN...");
    ret = lt_new_PIN_setup(h, master_secret, pin, sizeof(pin), NULL, sizeof(additional_data), final_key_initialized);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to set the user PIN, ret=%s", lt_ret_verbose(ret));
        lt_session_abort(h);
        lt_deinit(h);
        return -1;
    }
    LT_LOG_INFO("\tOK");

    ret = lt_print_bytes(final_key_initialized, sizeof(final_key_initialized), print_buff, PRINT_BUFF_SIZE);
    if (LT_OK != ret) {
        LT_LOG_ERROR("lt_print_bytes failed, ret=%s", lt_ret_verbose(ret));
        lt_session_abort(h);
        lt_deinit(h);
        return -1;
    }
    LT_LOG_INFO("Initialized final_key: %s", print_buff);
    LT_LOG_LINE();

    uint8_t final_key_exported[TR01_MAC_AND_DESTROY_DATA_SIZE] = {0};
    LT_LOG_INFO("Doing %d PIN check attempts with wrong PIN...", MACANDD_ROUNDS);
    for (int i = 1; i < MACANDD_ROUNDS; i++) {
        LT_LOG_INFO("\tInputting wrong PIN -> slot #%d destroyed", i);
        ret = lt_PIN_entry_check(h, pin_wrong, sizeof(pin_wrong), NULL, sizeof(additional_data), final_key_exported);
        if (LT_FAIL != ret) {
            LT_LOG_ERROR("Return value is not LT_FAIL, ret=%s", lt_ret_verbose(ret));
            lt_session_abort(h);
            lt_deinit(h);
            return -1;
        }
        ret = lt_print_bytes(final_key_exported, sizeof(final_key_exported), print_buff, PRINT_BUFF_SIZE);
        if (LT_OK != ret) {
            LT_LOG_ERROR("lt_print_bytes failed, ret=%s", lt_ret_verbose(ret));
            lt_session_abort(h);
            lt_deinit(h);
            return -1;
        }
        LT_LOG_INFO("\tSecret: %s", print_buff);
    }
    LT_LOG_INFO("\tOK");

    LT_LOG_INFO("Doing Final PIN attempt with correct PIN, slots are reinitialized again...");
    ret = lt_PIN_entry_check(h, pin, sizeof(pin), NULL, sizeof(additional_data), final_key_exported);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Attempt with correct PIN failed, ret=%s", lt_ret_verbose(ret));
        lt_session_abort(h);
        lt_deinit(h);
        return -1;
    }
    ret = lt_print_bytes(final_key_exported, sizeof(final_key_exported), print_buff, PRINT_BUFF_SIZE);
    if (LT_OK != ret) {
        LT_LOG_ERROR("lt_print_bytes failed, ret=%s", lt_ret_verbose(ret));
        lt_session_abort(h);
        lt_deinit(h);
        return -1;
    }
    LT_LOG_INFO("\tExported final_key: %s", print_buff);
    LT_LOG_INFO("\tOK");
    LT_LOG_LINE();

    if (memcmp(final_key_initialized, final_key_exported, sizeof(final_key_initialized))) {
        LT_LOG_ERROR("final_key and final_key_exported DO NOT MATCH");
        lt_session_abort(h);
        lt_deinit(h);
        return -1;
    }
    else {
        LT_LOG_INFO("final_key and final_key_exported MATCH");
    }

    LT_LOG_INFO("Aborting Secure Session");
    ret = lt_session_abort(h);
    if (LT_OK != ret) {
        LT_LOG_ERROR("Failed to abort Secure Session, ret=%s", lt_ret_verbose(ret));
        lt_deinit(h);
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
