/**
 * @file main.c
 * @brief Example usage of TROPIC01 flagship feature - 'Mac And Destroy' PIN verification engine.
 * For more info please refer to ODN_TR01_app_002_pin_verif.pdf
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md in the root directory of this source tree.
 */

#include <arpa/inet.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "libtropic.h"
#include "libtropic_common.h"
#include "libtropic_mbedtls_v4.h"
#include "libtropic_port_posix_tcp.h"
#include "psa/crypto.h"

// Pairing keys the model was configured with, defaults to prod0 keys.
// Provide your own keys here if you configured the model differently.
#define DEFAULT_SH0_PRIV sh0priv_prod0
#define DEFAULT_SH0_PUB sh0pub_prod0

/** @brief Last slot in User memory used for storing of M&D related data (only in this example). */
#define MACANDD_R_MEM_DATA_SLOT (511)

/** @brief Size of the print buffer. */
#define PRINT_BUFF_SIZE 196

/** @brief Number of MAC-and-Destroy rounds (only in this example). */
#define MACANDD_ROUNDS 12

#if (MACANDD_ROUNDS > 12)
#error "For this example, MACANDD_ROUNDS must be less than 12. Generally, the maximum is TR01_MACANDD_ROUNDS_MAX."
#endif

/** @brief Minimal size of MAC-and-Destroy additional data (only in this example). */
#define MACANDD_ADD_SIZE_MIN 0
/** @brief Maximal size of MAC-and-Destroy additional data (only in this example). */
#define MACANDD_ADD_SIZE_MAX 128u
/** @brief Minimal size of MAC-and-Destroy PIN input (only in this example). */
#define MACANDD_PIN_SIZE_MIN 4u
/** @brief Maximal size of MAC-and-Destroy PIN input (only in this example). */
#define MACANDD_PIN_SIZE_MAX 8u

/**
 * @brief This structure holds data used by host during MAC-And-Destroy sequence.
 * Content of this struct must be stored in non-volatile memory, because it is used
 * between power cycles.
 */
struct lt_macandd_nvm_t {
    uint8_t i;
    uint8_t ci[MACANDD_ROUNDS * TR01_MAC_AND_DESTROY_DATA_SIZE];
    uint8_t t[PSA_HASH_LENGTH(PSA_ALG_SHA_256)];
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
        destination[i] = data[i] ^ key[i];
    }
}

/**
 * @brief PSA Crypto HMAC-SHA256 wrapper.
 *
 * @param key       Key data buffer
 * @param key_len   Length of data in key buffer
 * @param data      Data buffer
 * @param data_len  Length of data buffer
 * @param output    Output buffer for HMAC result
 * @return          psa_status_t
 */
static psa_status_t hmac_sha256(const uint8_t *key, const size_t key_len, const uint8_t *data, const size_t data_len,
                                uint8_t *output)
{
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    psa_status_t status;
    size_t output_len;

    // Set key attributes for HMAC.
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_algorithm(&attributes, PSA_ALG_HMAC(PSA_ALG_SHA_256));
    psa_set_key_type(&attributes, PSA_KEY_TYPE_HMAC);

    // Import key.
    status = psa_import_key(&attributes, key, key_len, &key_id);
    if (status != PSA_SUCCESS) {
        goto cleanup;
    }

    // Compute HMAC.
    status = psa_mac_compute(key_id, PSA_ALG_HMAC(PSA_ALG_SHA_256), data, data_len, output,
                             PSA_HASH_LENGTH(PSA_ALG_SHA_256), &output_len);

cleanup:
    if (key_id != 0) psa_destroy_key(key_id);
    psa_reset_key_attributes(&attributes);
    return status;
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
 * @param h           Handle for communication with TROPIC01
 * @param master_secret  32 bytes of random data (determines final_key)
 * @param PIN         Array of bytes (size between MACANDD_PIN_SIZE_MIN and MACANDD_PIN_SIZE_MAX)
 * representing PIN
 * @param PIN_size    Length of the PIN field
 * @param add         Additional data to be used in M&D sequence (size between MACANDD_ADD_SIZE_MIN and
 * MACANDD_ADD_SIZE_MAX). Pass NULL if no additional data should be used.
 * @param add_size    Length of additional data
 * @param final_key      Buffer into which final key will be placed when all went successfully
 * @return lt_ret_t   LT_OK if correct, otherwise LT_FAIL
 */
static lt_ret_t new_PIN_setup(lt_handle_t *h, const uint8_t *master_secret, const uint8_t *PIN, const uint8_t PIN_size,
                              const uint8_t *add, const uint8_t add_size, uint8_t *final_key)
{
    if (!h || !master_secret || !PIN || (PIN_size < MACANDD_PIN_SIZE_MIN) || (PIN_size > MACANDD_PIN_SIZE_MAX)
        || (add_size > MACANDD_ADD_SIZE_MAX) || !final_key) {
        // `add` parameter is not checked for NULL, because it can be NULL (handled in the lines below).
        return LT_PARAM_ERR;
    }

    uint8_t add_size_checked = add_size;
    if (!add) {
        add_size_checked = 0;
    }

    psa_status_t psa_ret;

    // Clear variable for released final_key so there is known data (zeroes) in case this function ended sooner then
    // final_key was prepared.
    memset(final_key, 0, TR01_MAC_AND_DESTROY_DATA_SIZE);

    // Variable used during a process of getting a encryption key k_i.
    uint8_t v[PSA_HASH_LENGTH(PSA_ALG_SHA_256)] = {0};
    // Variable used during a process of getting a encryption key k_i.
    uint8_t w_i[TR01_MAC_AND_DESTROY_DATA_SIZE] = {0};
    // Encryption key.
    uint8_t k_i[PSA_HASH_LENGTH(PSA_ALG_SHA_256)] = {0};
    // Variable used to initialize slot(s).
    uint8_t u[PSA_HASH_LENGTH(PSA_ALG_SHA_256)] = {0};
    // Helper array of zeroes (used as a key in KDF).
    const uint8_t zeros[32] = {0};

    // This organizes data which will be stored into nvm.
    struct lt_macandd_nvm_t nvm = {0};

    // User is expected to pass not only PIN, but might also pass another data (e.g. HW ID, ...).
    // Both arrays are concatenated and used together as an input for KDF.
    uint8_t kdf_input_buff[MACANDD_PIN_SIZE_MAX + MACANDD_ADD_SIZE_MAX];
    memcpy(kdf_input_buff, PIN, PIN_size);
    if (!add || add_size_checked == 0) {
        printf("\tNo additional data will be used in the following M&D sequence\n");
    }
    else {
        memcpy(kdf_input_buff + PIN_size, add, add_size_checked);
    }

    // Erase a slot in R-Memory, which will be used as a storage for NVM data.
    printf("\tErasing R_Mem User slot %d...", MACANDD_R_MEM_DATA_SLOT);
    lt_ret_t ret = lt_r_mem_data_erase(h, MACANDD_R_MEM_DATA_SLOT);
    if (ret != LT_OK) {
        fprintf(stderr, "\n\tFailed to erase User slot, ret=%s\n", lt_ret_verbose(ret));
        goto exit;
    }
    printf("OK\n");

    // Store number of attempts.
    nvm.i = MACANDD_ROUNDS;
    // Compute tag t = KDF(s, 0x00), save into nvm struct.
    // Tag will be later used during PIN_entry_check() to verify validity of final_key.
    psa_ret = hmac_sha256(master_secret, TR01_MAC_AND_DESTROY_DATA_SIZE, (uint8_t[]){0x00}, 1, nvm.t);
    if (psa_ret != PSA_SUCCESS) {
        fprintf(stderr, "\tt = KDF(s, 0x00) failed, psa_status_t=%d\n", psa_ret);
        goto exit;
    }

    // Compute u = KDF(s, 0x01).
    // This value will be sent through M&D sequence to initialize a slot.
    psa_ret = hmac_sha256(master_secret, TR01_MAC_AND_DESTROY_DATA_SIZE, (uint8_t[]){0x01}, 1, u);
    if (psa_ret != PSA_SUCCESS) {
        fprintf(stderr, "\tu = KDF(s, 0x01) failed, psa_status_t=%d\n", psa_ret);
        goto exit;
    }

    // Compute v = KDF(0, PIN||A) where 0 is all zeroes key.
    psa_ret = hmac_sha256(zeros, sizeof(zeros), kdf_input_buff, PIN_size + add_size_checked, v);
    if (psa_ret != PSA_SUCCESS) {
        fprintf(stderr, "\tv = KDF(0, PIN||A) failed, psa_status_t=%d\n", psa_ret);
        goto exit;
    }

    for (int i = 0; i < nvm.i; i++) {
        uint8_t ignore[TR01_MAC_AND_DESTROY_DATA_SIZE] = {0};

        // This call of a M&D sequence results in initialization of one slot.
        printf("\n\tDoing M&D sequence to initialize a slot...");
        ret = lt_mac_and_destroy(h, i, u, ignore);
        if (ret != LT_OK) {
            fprintf(stderr, "\n\tFailed while doing M&D sequence, ret=%s\n", lt_ret_verbose(ret));
            goto exit;
        }
        printf("OK\n");

        // This call of a M&D sequence overwrites a previous slot, but key w is returned.
        // This key is later used to derive k_i, which is used to encrypt the master_secret.
        printf("\tDoing M&D sequence to overwrite previous slot...");
        ret = lt_mac_and_destroy(h, i, v, w_i);
        if (ret != LT_OK) {
            fprintf(stderr, "\nFailed while doing M&D sequence, ret=%s\n", lt_ret_verbose(ret));
            goto exit;
        }
        printf("OK\n");

        // Now the slot is initialized again by calling M&D sequence again with 'u'.
        printf("\tDoing M&D sequence again to initialize a slot...");
        ret = lt_mac_and_destroy(h, i, u, ignore);
        if (ret != LT_OK) {
            fprintf(stderr, "\n\tFailed while doing M&D sequence, ret=%s\n", lt_ret_verbose(ret));
            goto exit;
        }
        printf("OK\n");

        // Derive k_i = KDF(w_i, PIN||A); k_i will be used to encrypt master_secret.
        psa_ret = hmac_sha256(w_i, sizeof(w_i), kdf_input_buff, PIN_size + add_size_checked, k_i);
        if (psa_ret != PSA_SUCCESS) {
            fprintf(stderr, "\tk_i = KDF(w_i, PIN||A) failed, psa_status_t=%d\n", psa_ret);
            goto exit;
        }

        // Encrypt master_secret using k_i as a key and store ciphertext into non volatile storage.
        encrypt(master_secret, k_i, nvm.ci + (i * TR01_MAC_AND_DESTROY_DATA_SIZE));
    }

    // Persistently save nvm data into TROPIC01's R-Memory slot.
    printf("\n\tWriting NVM data into R_Mem User slot %d...", MACANDD_R_MEM_DATA_SLOT);
    ret = lt_r_mem_data_write(h, MACANDD_R_MEM_DATA_SLOT, (uint8_t *)&nvm, sizeof(nvm));
    if (ret != LT_OK) {
        fprintf(stderr, "\tFailed to write User slot, ret=%s\n", lt_ret_verbose(ret));
        goto exit;
    }
    printf("OK\n");

    // final_key is released to the caller.
    psa_ret = hmac_sha256(master_secret, TR01_MAC_AND_DESTROY_DATA_SIZE, (uint8_t *)"2", 1, final_key);
    if (psa_ret != PSA_SUCCESS) {
        fprintf(stderr, "\tFail during last computation of final_key, psa_status_t=%d\n", psa_ret);
        goto exit;
    }

// Cleanup all sensitive data from memory.
// We recommend using a safe function for this. Refer to the link below for some information.
// https://www.gnu.org/software/libc/manual/html_node/Erasing-Sensitive-Data.html
exit:
    memset(kdf_input_buff, 0, PIN_size + add_size_checked);
    memset(u, 0, sizeof(u));
    memset(v, 0, sizeof(v));
    memset(w_i, 0, sizeof(w_i));
    memset(k_i, 0, sizeof(k_i));
    memset(&nvm, 0, sizeof(nvm));

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
 * @param h           Handle for communication with TROPIC01
 * @param PIN         Array of bytes (size between MACANDD_PIN_SIZE_MIN and MACANDD_PIN_SIZE_MAX)
 * representing PIN
 * @param PIN_size    Length of the PIN field
 * @param add         Additional data to be used in M&D sequence (size between MACANDD_ADD_SIZE_MIN and
 * MACANDD_ADD_SIZE_MAX). Pass NULL if no additional data should be used.
 * @param add_size    Length of additional data
 * @param final_key   Buffer into which final_key will be saved
 * @return lt_ret_t   LT_OK if correct, otherwise LT_FAIL
 */
static lt_ret_t PIN_entry_check(lt_handle_t *h, const uint8_t *PIN, const uint8_t PIN_size, const uint8_t *add,
                                const uint8_t add_size, uint8_t *final_key)
{
    if (!h || !PIN || (PIN_size < MACANDD_PIN_SIZE_MIN) || (PIN_size > MACANDD_PIN_SIZE_MAX)
        || (add_size > MACANDD_ADD_SIZE_MAX) || !final_key) {
        // `add` parameter is not checked for NULL, because it can be NULL (handled in the lines below).
        return LT_PARAM_ERR;
    }
    if (h->l3.session_status != LT_SECURE_SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    uint8_t add_size_checked = add_size;
    if (!add) {
        add_size_checked = 0;
    }

    psa_status_t psa_ret;

    // Clear variable for released final_key so there is known data (zeroes) in case this function ended sooner then
    // final_key was prepared.
    memset(final_key, 0, TR01_MAC_AND_DESTROY_DATA_SIZE);

    // Variable used during a process of getting a decryption key k_i.
    uint8_t v_[PSA_HASH_LENGTH(PSA_ALG_SHA_256)] = {0};
    // Variable used during a process of getting a decryption key k_i.
    uint8_t w_i[TR01_MAC_AND_DESTROY_DATA_SIZE] = {0};
    // Decryption key.
    uint8_t k_i[PSA_HASH_LENGTH(PSA_ALG_SHA_256)] = {0};
    // Secret.
    uint8_t s_[TR01_MAC_AND_DESTROY_DATA_SIZE] = {0};
    // Tag.
    uint8_t t_[PSA_HASH_LENGTH(PSA_ALG_SHA_256)] = {0};
    // Value used to initialize Mac And Destroy's slot after a correct PIN try.
    uint8_t u[PSA_HASH_LENGTH(PSA_ALG_SHA_256)] = {0};
    // Helper array of zeroes (used as a key in KDF).
    const uint8_t zeros[32] = {0};

    // This organizes data which will be read from nvm.
    struct lt_macandd_nvm_t nvm = {0};

    // User might pass not only PIN, but also another data(e.g. HW ID, ...) if needed.
    // Both arrays are concatenated and used together as an input for KDF.
    uint8_t kdf_input_buff[MACANDD_PIN_SIZE_MAX + MACANDD_ADD_SIZE_MAX];
    memcpy(kdf_input_buff, PIN, PIN_size);
    if (!add || add_size_checked == 0) {
        printf("\tNo additional data will be used in the following M&D sequence\n");
    }
    else {
        memcpy(kdf_input_buff + PIN_size, add, add_size_checked);
    }

    // Load M&D data from TROPIC01's R-Memory.
    printf("\tReading M&D data from R_Mem User slot %d...", MACANDD_R_MEM_DATA_SLOT);
    uint16_t read_size;
    lt_ret_t ret = lt_r_mem_data_read(h, MACANDD_R_MEM_DATA_SLOT, (uint8_t *)&nvm, sizeof(nvm), &read_size);
    if (ret != LT_OK) {
        fprintf(stderr, "\n\tFailed to read User slot, ret=%s\n", lt_ret_verbose(ret));
        goto exit;
    }
    printf("OK\n");

    // if i == 0: FAIL (no attempts remaining).
    printf("\tChecking if nvm.i != 0...");
    if (nvm.i == 0) {
        fprintf(stderr, "\tnvm.i == 0");
        ret = LT_FAIL;
        goto exit;
    }
    printf("OK\n");

    // Decrement variable which holds number of tries.
    // Let i = i - 1
    nvm.i--;

    // and store M&D data back to TROPIC01's R-Memory.
    printf("\tWriting back M&D data into R_Mem User slot %d (erase, then write)...", MACANDD_R_MEM_DATA_SLOT);
    ret = lt_r_mem_data_erase(h, MACANDD_R_MEM_DATA_SLOT);
    if (ret != LT_OK) {
        fprintf(stderr, "\n\tFailed to erase User slot, ret=%s\n", lt_ret_verbose(ret));
        goto exit;
    }
    ret = lt_r_mem_data_write(h, MACANDD_R_MEM_DATA_SLOT, (uint8_t *)&nvm, sizeof(nvm));
    if (ret != LT_OK) {
        fprintf(stderr, "\n\tFailed to write User slot, ret=%s\n", lt_ret_verbose(ret));
        goto exit;
    }
    printf("OK\n");

    // Compute v’ = KDF(0, PIN’||A).
    psa_ret = hmac_sha256(zeros, sizeof(zeros), kdf_input_buff, PIN_size + add_size_checked, v_);
    if (psa_ret != PSA_SUCCESS) {
        fprintf(stderr, "\tv' = KDF(0, PIN'||A) failed, psa_status_t=%d\n", psa_ret);
        goto exit;
    }

    // Execute w’ = MACANDD(i, v’).
    printf("\tDoing M&D sequence...");
    ret = lt_mac_and_destroy(h, nvm.i, v_, w_i);
    if (ret != LT_OK) {
        fprintf(stderr, "\tFailed while doing M&D sequence, ret=%s\n", lt_ret_verbose(ret));
        goto exit;
    }
    printf("OK\n");

    // Compute k’_i = KDF(w’, PIN’||A).
    psa_ret = hmac_sha256(w_i, sizeof(w_i), kdf_input_buff, PIN_size + add_size_checked, k_i);
    if (psa_ret != PSA_SUCCESS) {
        fprintf(stderr, "\tk'_i = KDF(w', PIN'||A) failed, psa_status_t=%d\n", psa_ret);
        goto exit;
    }

    // Read the ciphertext c_i and tag t from NVM,
    // decrypt c_i with k’_i as the key and obtain s_.
    decrypt(nvm.ci + (nvm.i * TR01_MAC_AND_DESTROY_DATA_SIZE), k_i, s_);

    // Compute tag t = KDF(s_, "0x00").
    psa_ret = hmac_sha256(s_, sizeof(s_), (uint8_t[]){0x00}, 1, t_);
    if (psa_ret != PSA_SUCCESS) {
        fprintf(stderr, "\tt = KDF(s_, \"0x00\") failed, psa_status_t=%d\n", psa_ret);
        goto exit;
    }

    // If t’ != t: FAIL
    if (memcmp(nvm.t, t_, sizeof(t_)) != 0) {
        ret = LT_FAIL;
        fprintf(stderr, "\tTags do not match!\n");
        goto exit;
    }

    // Pin is correct, now initialize macandd slots again:
    // Compute u = KDF(s’, "0x01").
    psa_ret = hmac_sha256(s_, sizeof(s_), (uint8_t[]){0x01}, 1, u);
    if (psa_ret != PSA_SUCCESS) {
        fprintf(stderr, "\tu = KDF(s', \"0x01\") failed, psa_status_t=%d\n", psa_ret);
        goto exit;
    }

    for (int x = nvm.i; x < MACANDD_ROUNDS - 1; x++) {
        uint8_t ignore[TR01_MAC_AND_DESTROY_DATA_SIZE] = {0};

        printf("\tDoing M&D sequence...");
        ret = lt_mac_and_destroy(h, x, u, ignore);
        if (ret != LT_OK) {
            fprintf(stderr, "\n\tFailed while doing M&D sequence, ret=%s\n", lt_ret_verbose(ret));
            goto exit;
        }
        printf("OK\n");
    }

    // Set variable which holds number of tries back to initial state MACANDD_ROUNDS
    nvm.i = MACANDD_ROUNDS;

    // Store NVM data for future use
    printf("\tWriting M&D data into R_Mem User slot %d for future use (erase, then write)...", MACANDD_R_MEM_DATA_SLOT);
    ret = lt_r_mem_data_erase(h, MACANDD_R_MEM_DATA_SLOT);
    if (ret != LT_OK) {
        fprintf(stderr, "\n\tFailed to erase User slot, ret=%s\n", lt_ret_verbose(ret));
        goto exit;
    }
    ret = lt_r_mem_data_write(h, MACANDD_R_MEM_DATA_SLOT, (uint8_t *)&nvm, sizeof(nvm));
    if (ret != LT_OK) {
        fprintf(stderr, "\n\tFailed to write User slot, ret=%s\n", lt_ret_verbose(ret));
        goto exit;
    }
    printf("OK\n");

    // Calculate final_key and store it into passed array
    psa_ret = hmac_sha256(s_, sizeof(s_), (uint8_t *)"2", 1, final_key);
    if (psa_ret != PSA_SUCCESS) {
        fprintf(stderr, "\tFail during last computation of final_key, psa_status_t=%d\n", psa_ret);
        goto exit;
    }

// Cleanup all sensitive data from memory.
// We recommend using a safe function for this. Refer to the link below for some information.
// https://www.gnu.org/software/libc/manual/html_node/Erasing-Sensitive-Data.html
exit:
    memset(kdf_input_buff, 0, PIN_size + add_size_checked);
    memset(w_i, 0, sizeof(w_i));
    memset(k_i, 0, sizeof(k_i));
    memset(v_, 0, sizeof(v_));
    memset(s_, 0, sizeof(s_));
    memset(t_, 0, sizeof(t_));
    memset(u, 0, sizeof(u));
    memset(&nvm, 0, sizeof(nvm));

    return ret;
}

int main(void)
{
    // Cosmetics: Disable buffering to keep output in order. You do not need to do this in your app if you don't care about
    // stdout/stderr output being shuffled or you use stdout only (or different output mechanism altogether).
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    printf("==========================================\n");
    printf("==== TROPIC01 Mac and Destroy Example ====\n");
    printf("==========================================\n");

    // Cryptographic function provider initialization.
    //
    // In production, this would typically be done only once,
    // usually at the start of the application or before
    // the first use of cryptographic functions but no later than
    // the first occurrence of any Libtropic function
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        fprintf(stderr, "PSA Crypto initialization failed, status=%d (psa_status_t)\n", status);
        return -1;
    }

    // Libtropic handle.
    //
    // It is declared here (on stack) for
    // simplicity. In production, you put it on heap if needed.
    lt_handle_t lt_handle = {0};

    // Initialize device before handing handle to the test.
    lt_dev_posix_tcp_t device;
    device.addr = inet_addr("127.0.0.1");
    device.port = 28992;
    lt_handle.l2.device = &device;

    // Generate seed for the PRNG and seed it.
    // Note: model uses rand(), which is not cryptographically secure. Better alternative should be used in production.
    unsigned int prng_seed;
    if (0 != getentropy(&prng_seed, sizeof(prng_seed))) {
        fprintf(stderr, "main: getentropy() failed (%s)!\n", strerror(errno));
        mbedtls_psa_crypto_free();
        return -1;
    }
    srand(prng_seed);
    printf("PRNG initialized with seed=%u\n", prng_seed);

    // Crypto abstraction layer (CAL) context.
    lt_ctx_mbedtls_v4_t crypto_ctx;
    lt_handle.l3.crypto_ctx = &crypto_ctx;

    printf("Initializing handle...");
    lt_ret_t ret = lt_init(&lt_handle);
    if (ret != LT_OK) {
        fprintf(stderr, "\nFailed to initialize handle, ret=%s\n", lt_ret_verbose(ret));
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("OK\n");

    // We need to ensure we are not in the Start-up Mode, as L3 commands are available only in the Application Firmware.
    printf("Sending reboot request...");
    ret = lt_reboot(&lt_handle, TR01_REBOOT);
    if (ret != LT_OK) {
        fprintf(stderr, "\nlt_reboot() failed, ret=%s\n", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("OK\n");

    printf("Starting Secure Session with key slot %d...", (int)TR01_PAIRING_KEY_SLOT_INDEX_0);
    ret = lt_verify_chip_and_start_secure_session(&lt_handle, DEFAULT_SH0_PRIV, DEFAULT_SH0_PUB,
                                                  TR01_PAIRING_KEY_SLOT_INDEX_0);
    if (ret != LT_OK) {
        fprintf(stderr, "\nFailed to start Secure Session with key %d, ret=%s\n", (int)TR01_PAIRING_KEY_SLOT_INDEX_0,
                lt_ret_verbose(ret));
        fprintf(stderr,
                "Check if you use correct SH0 keys! Hint: if you use an engineering sample chip, compile with "
                "-DLT_SH0_KEYS=eng_sample\n");
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("OK\n");

    // This variable stores final_key which is released to the user after successful PIN check or PIN set.
    uint8_t final_key_initialized[TR01_MAC_AND_DESTROY_DATA_SIZE] = {0};

    // Additional data passed by user besides PIN - this is optional, but recommended.
    uint8_t additional_data[]
        = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
           0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

    // User's PIN.
    uint8_t pin[] = {1, 2, 3, 4};
    uint8_t pin_wrong[] = {2, 2, 3, 4};

    printf("\nWill initialize Mac-And-Destroy:\n");
    printf("Generating random master_secret (using TROPIC01's TRNG)...");
    uint8_t master_secret[TR01_MAC_AND_DESTROY_DATA_SIZE] = {0};
    ret = lt_random_value_get(&lt_handle, master_secret, TR01_MAC_AND_DESTROY_DATA_SIZE);
    if (ret != LT_OK) {
        fprintf(stderr, "\nFailed to get random bytes, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(&lt_handle);
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("OK\n");

    char print_buff[PRINT_BUFF_SIZE];
    ret = lt_print_bytes(master_secret, sizeof(master_secret), print_buff, PRINT_BUFF_SIZE);
    if (ret != LT_OK) {
        fprintf(stderr, "lt_print_bytes failed, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(&lt_handle);
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("Generated master_secret: %s\n", print_buff);

    // Set the PIN and log out the final_key
    printf("Setting the user PIN...\n");
    ret = new_PIN_setup(&lt_handle, master_secret, pin, sizeof(pin), additional_data, sizeof(additional_data),
                        final_key_initialized);
    if (ret != LT_OK) {
        fprintf(stderr, "\nFailed to set the user PIN, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(&lt_handle);
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("PIN was set successfully\n");

    ret = lt_print_bytes(final_key_initialized, sizeof(final_key_initialized), print_buff, PRINT_BUFF_SIZE);
    if (ret != LT_OK) {
        fprintf(stderr, "lt_print_bytes failed, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(&lt_handle);
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("Initialized final_key: %s\n", print_buff);

    uint8_t final_key_exported[TR01_MAC_AND_DESTROY_DATA_SIZE] = {0};
    printf("\nWill do %d PIN check attempts with wrong PIN:\n", MACANDD_ROUNDS);
    for (int i = 1; i < MACANDD_ROUNDS; i++) {
        printf("\tInputting wrong PIN -> slot #%d will be destroyed...\n", i);
        ret = PIN_entry_check(&lt_handle, pin_wrong, sizeof(pin_wrong), additional_data, sizeof(additional_data),
                              final_key_exported);
        if (ret != LT_FAIL) {
            fprintf(stderr, "\nReturn value is not LT_FAIL, ret=%s\n", lt_ret_verbose(ret));
            lt_session_abort(&lt_handle);
            lt_deinit(&lt_handle);
            mbedtls_psa_crypto_free();
            return -1;
        }

        ret = lt_print_bytes(final_key_exported, sizeof(final_key_exported), print_buff, PRINT_BUFF_SIZE);
        if (ret != LT_OK) {
            fprintf(stderr, "lt_print_bytes failed, ret=%s\n", lt_ret_verbose(ret));
            lt_session_abort(&lt_handle);
            lt_deinit(&lt_handle);
            mbedtls_psa_crypto_free();
            return -1;
        }
        printf("\tSecret: %s\n\n", print_buff);
    }

    printf("Doing final PIN attempt with correct PIN, slots are reinitialized again...\n");
    ret = PIN_entry_check(&lt_handle, pin, sizeof(pin), additional_data, sizeof(additional_data), final_key_exported);
    if (ret != LT_OK) {
        fprintf(stderr, "\nAttempt with correct PIN failed, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(&lt_handle);
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("Final PIN attempt was successful\n");

    ret = lt_print_bytes(final_key_exported, sizeof(final_key_exported), print_buff, PRINT_BUFF_SIZE);
    if (ret != LT_OK) {
        fprintf(stderr, "lt_print_bytes failed, ret=%s\n", lt_ret_verbose(ret));
        lt_session_abort(&lt_handle);
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("Exported final_key: %s\n", print_buff);

    printf("Comparing initialized final_key and exported final_key...");
    if (memcmp(final_key_initialized, final_key_exported, sizeof(final_key_initialized))) {
        fprintf(stderr, "The keys do not match!\n");
        lt_session_abort(&lt_handle);
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("OK\n");

    printf("Aborting Secure Session...");
    ret = lt_session_abort(&lt_handle);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to abort Secure Session, ret=%s\n", lt_ret_verbose(ret));
        lt_deinit(&lt_handle);
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("OK\n");

    printf("Deinitializing handle...");
    ret = lt_deinit(&lt_handle);
    if (LT_OK != ret) {
        fprintf(stderr, "\nFailed to deinitialize handle, ret=%s\n", lt_ret_verbose(ret));
        mbedtls_psa_crypto_free();
        return -1;
    }
    printf("OK\n");

    // Cryptographic function provider deinitialization.
    //
    // In production, this would be done only once, typically
    // during termination of the application.
    mbedtls_psa_crypto_free();

    return 0;
}