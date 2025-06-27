/**
 * @file lt_ex_macandd.c
 * @name Mac And Destroy
 * @brief Example usage of TROPIC01 flgship feature - 'Mac And Destroy' PIN verification engine
 *
 * @details This code aims to show an example usage of Mac And Destroy (M&D) feature of TROPIC01.
 *
 *   Value MACANDD_ROUNDS represents a number of possible PIN gueses, this value also affects size of lt_macandd_nvm_t
 * struct. Technically TROPIC01 is capable to have this set to 128, therefore provide 128 Mac And Destroy tries, which
 *   would require roughly 128*32 bytes in non volatile memory for storing data related to M&D tries.
 *
 *   In this example TROPIC01's R memory is used as a storage for data during power cycle.
 *   For a sake of simplicity, only one R memory slot is used as a storage, which means 444B of storage are available.
 *
 *   Therefore MACANDD_ROUNDS is here limited to 12 -> biggest possible number of tries which fits into 444B one R
 * memory slot.
 *
 * @note Read TROPIC01's datasheet and application note before diving into this example!
 *
 * @author Tropic Square s.r.o.
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "inttypes.h"
#include "libtropic.h"
#include "libtropic_examples.h"
#include "libtropic_logging.h"
#include "string.h"

// Needed to access to lt_port_random_bytes()
#include "libtropic_port.h"
// Needed to access HMAC_SHA256
#include "lt_hmac_sha256.h"

/**
 * @brief This function is used for debug print of bytes as hexadecimal string
 *
 * @param data        Pointer to data to be printed
 * @param len         Length in bytes of data to be printed
 */
#define BUFF_SIZE 196
static char bytes_buffer[BUFF_SIZE];
static char *print_bytes(uint8_t *data, uint16_t len)
{
    if ((len > BUFF_SIZE) || (!data)) {
        memcpy(bytes_buffer, "error_str_decoding", 20);
        return bytes_buffer;
    }
    bytes_buffer[0] = '\0';
    for (uint16_t i = 0; i < len; i++) {
        char byte_str[4];
        snprintf(byte_str, sizeof(byte_str), "%02X", data[i]);
        // Check if appending the byte would exceed the buffer size
        if (strlen(bytes_buffer) + strlen(byte_str) + 1 > sizeof(bytes_buffer)) {
            break;  // Stop if the buffer is full
        }
        strncat(bytes_buffer, byte_str, sizeof(bytes_buffer) - strlen(bytes_buffer) - 1);
    }

    return bytes_buffer;
}

#ifndef MACANDD_ROUNDS
#define MACANDD_ROUNDS 12
#endif

#if (MACANDD_ROUNDS > 12)
#error \
    "MACANDD_ROUNDS must be less than 12 here, or generally than MACANDD_ROUNDS_MAX. Read explanation at the beginning of this file"
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
 * @brief Example function how to set PIN with Mac And Destroy
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
    if (h->l3.session != SESSION_ON) {
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

    lt_ret_t ret = lt_port_random_bytes((uint32_t *)s, 8);
    if (ret != LT_OK) {
        goto exit;
    }

    // Erase a slot in R memory, which will be used as a storage for NVM data
    ret = lt_r_mem_data_erase(h, R_MEM_DATA_SLOT_MACANDD);
    if (ret != LT_OK) {
        goto exit;
    }

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
        ret = lt_mac_and_destroy(h, i, u, garbage);
        if (ret != LT_OK) {
            goto exit;
        }
        // This call of a M&D sequence overwrites a previous slot, but key w is returned.
        // This key is later used to derive k_i (used to encrypt precious secret)
        ret = lt_mac_and_destroy(h, i, v, w);
        if (ret != LT_OK) {
            goto exit;
        }
        // Now the slot is initialized again by calling M&S sequence again with 'u'
        ret = lt_mac_and_destroy(h, i, u, garbage);
        if (ret != LT_OK) {
            goto exit;
        }

        // Derive k_i = KDF(w, PIN||A)
        // This key will be used to encrypt secret s
        lt_hmac_sha256(w, 32, kdf_input_buff, PIN_size + add_size, k_i);

        // Encrypt s using k_i as a key
        // TODO implement some better encryption, or discuss if using XOR here is fine
        for (int j = 0; j < 32; j++) {
            *(nvm.ci + (i * 32 + j)) = k_i[j] ^ s[j];
        }
    }

    // Persistently save nvm data into TROPIC01's R memory slot
    ret = lt_r_mem_data_write(h, R_MEM_DATA_SLOT_MACANDD, (uint8_t *)&nvm, sizeof(nvm));
    if (ret != LT_OK) {
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
 * @brief Check PIN with Mac And Destroy
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
    if (h->l3.session != SESSION_ON) {
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
    lt_ret_t ret = lt_r_mem_data_read(h, R_MEM_DATA_SLOT_MACANDD, (uint8_t *)&nvm, sizeof(nvm));
    if (ret != LT_OK) {
        goto exit;
    }

    // if i == 0: FAIL (no attempts remaining)
    if (nvm.i == 0) {
        goto exit;
    }

    // Decrement variable which holds number of tries
    // Let i = i - 1
    nvm.i--;

    // and store M&D data back to TROPIC01's R memory
    ret = lt_r_mem_data_erase(h, R_MEM_DATA_SLOT_MACANDD);
    if (ret != LT_OK) {
        goto exit;
    }
    ret = lt_r_mem_data_write(h, R_MEM_DATA_SLOT_MACANDD, (uint8_t *)&nvm, sizeof(nvm));
    if (ret != LT_OK) {
        goto exit;
    }

    // Compute v’ = KDF(0, PIN’||A).
    lt_hmac_sha256((uint8_t*)"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 32, kdf_input_buff, PIN_size + add_size, v_);

    // Execute w’ = MACANDD(i, v’)
    ret = lt_mac_and_destroy(h, nvm.i, v_, w_);
    if (ret != LT_OK) {
        goto exit;
    }

    // Compute k’_i = KDF(w’, PIN’||A)
    lt_hmac_sha256(w_, 32, kdf_input_buff, PIN_size + add_size, k_i);

    // Read the ciphertext c_i and tag t from NVM, decrypt c_i with k’_i as the key and obtain s_
    // TODO figure out if XOR can be used here?
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

        ret = lt_mac_and_destroy(h, x, u, garbage);
        if (ret != LT_OK) {
            goto exit;
        }
    }

    // Set variable which holds number of tries back to initial state MACANDD_ROUNDS
    nvm.i = MACANDD_ROUNDS;

    // Store NVM data for future use
    ret = lt_r_mem_data_erase(h, R_MEM_DATA_SLOT_MACANDD);
    if (ret != LT_OK) {
        goto exit;
    }
    ret = lt_r_mem_data_write(h, R_MEM_DATA_SLOT_MACANDD, (uint8_t *)&nvm, sizeof(nvm));
    if (ret != LT_OK) {
        goto exit;
    }

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

/**
 * @brief Session with H0 pairing keys
 *
 * @param h           Device's handle
 * @return            0 if success, otherwise -1
 */
static int session_H0(void)
{
    lt_handle_t h = {0};
#if LT_SEPARATE_L3_BUFF
    uint8_t l3_buffer[L3_FRAME_MAX_SIZE] __attribute__((aligned(16))) = {0};
    h.l3.buff = l3_buffer;
    h.l3.buff_len = sizeof(l3_buffer);
#endif

    lt_init(&h);

    LT_LOG("%s", "Creating secure session with H0 keys");
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, PAIRING_KEY_SLOT_INDEX_0));

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
    LT_LOG("Number of Mac And Destroy tries is set to %d", MACANDD_ROUNDS);
    LT_LOG("%s", "lt_PIN_set(): user sets the PIN");
    LT_ASSERT(LT_OK, lt_PIN_set(&h, pin, 4, additional_data, additional_data_size, secret));
    LT_LOG("Initialized secret: %s", print_bytes(secret, 32));
    LT_LOG_LINE();

    LT_LOG("%s", "  \"i<MACANDD_ROUNDS-1\"  PIN check attempts with wrong PIN");
    for (int i = 0; i < MACANDD_ROUNDS - 1; i++) {
        LT_LOG("lt_PIN_check() user inputs wrong PIN - slot %d destroyed", i);
        LT_ASSERT(LT_FAIL, lt_PIN_check(&h, pin_wrong, 4, additional_data, additional_data_size, secret));
        LT_LOG("secret: %s", print_bytes(secret, 32));
    }

    LT_LOG("%s", "Final PIN attempt with correct PIN, slots are reinitialized again - lt_PIN_check() with correct PIN");
    LT_ASSERT(LT_OK, lt_PIN_check(&h, pin, 4, additional_data, additional_data_size, secret));
    LT_LOG("Exported secret: %s", print_bytes(secret, 32));
    LT_LOG_LINE();

    LT_LOG("%s", "Aborting session H0");
    LT_ASSERT(LT_OK, lt_session_abort(&h));

    lt_deinit(&h);

    return LT_OK;
}

int lt_ex_macandd(void)
{
    LT_LOG("");
    LT_LOG("\t=======================================================================");
    LT_LOG("\t=====  TROPIC01 Hello World                                         ===");
    LT_LOG("\t=======================================================================");

    LT_LOG_LINE();
    if (session_H0() == -1) {
        printf("\r\nError during session_H0()\r\n");
        return -1;
    }

    LT_LOG_LINE();

    return 0;
}
