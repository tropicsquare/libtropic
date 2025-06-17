/**
 * @file lt_ex_macandd_pin_demo.c
 * @name Mac And Destroy
 * @brief Example usage of TROPIC01 flgship feature - 'Mac And Destroy' PIN verification engine
 *
 * @details This code aims to show an example usage of Mac And Destroy (M&D) feature of TROPIC01.
 *
 *   Value MACANDD_ROUNDS represents a number of possible PIN gueses, this value also affects size of lt_macandd_nvm_t struct.
 *   Technically TROPIC01 is capable to have this set to 128, therefore provide 128 Mac And Destroy tries, which
 *   would require roughly 128*32 bytes in non volatile memory for storing data related to M&D tries.
 *
 *   In this example TROPIC01's R memory is used as a storage for data during power cycle.
 *   For a sake of simplicity, only one R memory slot is used as a storage, which means 444B of storage are available.
 *
 *   Therefore MACANDD_ROUNDS is here limited to 12 -> biggest possible number of tries which fits into 444B one R memory slot.
 *
 * @note Read TROPIC01's datasheet and application note before diving into this example!
 *
 * @author Tropic Square s.r.o.
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "string.h"
#include "inttypes.h"
#include "stdio.h"

#include "libtropic.h"
#include "libtropic_examples.h"

// Needed to access to lt_port_random_bytes()
#include "libtropic_port.h"
// Needed to access HMAC_SHA256
#include "lt_hmac_sha256.h"


// Beginning of TCP stuff
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <ctype.h>
#include <stdbool.h>

#define PORT 12345
#define TCP_ADDR "127.0.0.1"
#define TX_ATTEMPTS 3
#define MAX_PIN_LEN 10
#define BUFFER_SIZE 256

// Global variables for MAC-and-Destroy state
static int current_attempts = 0;
static int max_attempts = 3;  // Default to 3 attempts (matches Python default)
static uint8_t current_pin[MAX_PIN_LEN] = {1,2,3,4};  // Default PIN
static int current_pin_len = 4;
static bool is_locked = false;
/**
 * @brief This function is used for debug print of bytes as hexadecimal string
 *
 * @param data        Pointer to data to be printed
 * @param len         Length in bytes of data to be printed
 */
#define BUFF_SIZE 196
static char bytes_buffer[BUFF_SIZE];
static char* print_bytes(uint8_t *data, uint16_t len) {
    if((len > BUFF_SIZE) || (!data)) {
        memcpy(bytes_buffer, "error_str_decoding", 20);
        return bytes_buffer;
    }
    bytes_buffer[0] = '\0';
    for (uint16_t i = 0; i < len; i++) {
        char byte_str[4];
        snprintf(byte_str, sizeof(byte_str), "%02X", data[i]);
        // Check if appending the byte would exceed the buffer size
        if (strlen(bytes_buffer) + strlen(byte_str) + 1 > sizeof(bytes_buffer)) {
            break; // Stop if the buffer is full
        }
        strncat(bytes_buffer, byte_str, sizeof(bytes_buffer) - strlen(bytes_buffer) - 1);
    }

    return bytes_buffer;
}

#ifndef MACANDD_ROUNDS
#define MACANDD_ROUNDS 12
#endif

#if (MACANDD_ROUNDS > 12)
#error "MACANDD_ROUNDS must be less than 12 here, or generally than MACANDD_ROUNDS_MAX. Read explanation at the beginning of this file"
#endif

/** @brief Minimal size of MAC-and-Destroy additional data */
#define MAC_AND_DESTROY_ADD_SIZE_MIN  0
/** @brief Maximal size of MAC-and-Destroy additional data */
#define MAC_AND_DESTROY_ADD_SIZE_MAX  128u
/** @brief Minimal size of MAC-and-Destroy PIN input */
#define MAC_AND_DESTROY_PIN_SIZE_MIN  4u
/** @brief Maximal size of MAC-and-Destroy PIN input */
#define MAC_AND_DESTROY_PIN_SIZE_MAX  8u

#define PALM_TREE "\U0001F334"
/**
 * @brief This structure holds data used by host during MAC and Destroy sequence
 * Content of this struct must be stored in non-volatile memory, because it is used
 * between power cycles
 */
struct lt_macandd_nvm_t {
    uint8_t i;
    uint8_t ci[MACANDD_ROUNDS*32];
    uint8_t t[32];
}__attribute__((__packed__));

/**
 * @brief Example function how to set PIN with Mac And Destroy
 *
 * @details There are more ways how to implement Mac And Destroy 'PIN set' functionality, differences could be in way of handling nvm data,
 *          number of tries, algorithm used for encryption, etc.
 *          This function is just one of the possible implementations of "PIN set", therefore we do not expose this through official libtropic API.
 *
 *          Take it as an inspiration, copy it into your project and adapt it to your specific hw resources.
 *
 * @param h           Device's handle
 * @param PIN         Array of bytes (size between MAC_AND_DESTROY_PIN_SIZE_MIN and MAC_AND_DESTROY_PIN_SIZE_MAX) representing PIN
 * @param PIN_size    Length of the PIN field
 * @param add         Additional data to be used in M&D sequence (size between MAC_AND_DESTROY_ADD_SIZE_MIN and MAC_AND_DESTROY_ADD_SIZE_MAX)
 * @param add_size    Length of additional data
 * @param secret      Buffer into which secret will be placed when all went successfully
 * @return lt_ret_t   LT_OK if correct, otherwise LT_FAIL
 */
static lt_ret_t lt_PIN_set(lt_handle_t *h, const uint8_t *PIN, const uint8_t PIN_size, const uint8_t *add, const uint8_t add_size, uint8_t *secret)
{
    if(    !h
        || !PIN
        || (PIN_size < MAC_AND_DESTROY_PIN_SIZE_MIN)
        || (PIN_size > MAC_AND_DESTROY_PIN_SIZE_MAX)
        || !add
        || (add_size > MAC_AND_DESTROY_ADD_SIZE_MAX)
        || !secret
    ) {
        return LT_PARAM_ERR;
    }
    if(h->l3.session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Clear variable for released secret so there is known data (zeroes) in case this function ended sooner then secret was prepared
    memset(secret, 0,32);

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
    uint8_t kdf_input_buff[MAC_AND_DESTROY_PIN_SIZE_MAX+MAC_AND_DESTROY_ADD_SIZE_MAX];
    memcpy(kdf_input_buff, PIN, PIN_size);
    memcpy(kdf_input_buff+PIN_size, add, add_size);

    /*
    lt_ret_t ret = lt_port_random_bytes((uint32_t*)s, 8);
    if(ret != LT_OK) {
        goto exit;
    }
    */

    memcpy(s, (uint8_t[]){0x78,0x64,0x4B,0x7D,0x17,0x64,0x98,0xC8,0x58,0xB1,0xDB,0xA0,0x5E,0xD3,0x94,0x70,0x41,0x0F,0xA9,0x1A,0x82,0x85,0xFE,0x9B,0xEE,0xCA,0xE1,0x87,0x42,0x81,0x98,0x8A}, 32);

    // Erase a slot in R memory, which will be used as a storage for NVM data
    lt_ret_t ret = lt_r_mem_data_erase(h, R_MEM_DATA_SLOT_MACANDD);
    if (ret != LT_OK) {
        goto exit;
    }

    // Store number of attempts
    nvm.i = MACANDD_ROUNDS;
    // Compute tag t = KDF(s, "0"), save into nvm struct
    // Tag will be later used during lt_PIN_check() to verify validity of secret
    lt_hmac_sha256(s, 32, (uint8_t*)"0", 1, nvm.t);

    // Compute u = KDF(s, "1")
    // This value will be sent through M&D sequence to initialize a slot
    lt_hmac_sha256(s, 32, (uint8_t*)"1", 1, u);

    // Compute v = KDF(0, PIN||A) where 0 is all zeroes key
    lt_hmac_sha256((uint8_t*)"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 32, kdf_input_buff, PIN_size+add_size, v);

    for(int i=0; i < nvm.i; i++) {
        uint8_t garbage[32] = {0};

        // This call of a M&D sequence results in initialization of one slot
        ret = lt_mac_and_destroy(h, i, u, garbage);
        if(ret != LT_OK) {
            goto exit;
        }
        // This call of a M&D sequence overwrites a previous slot, but key w is returned.
        // This key is later used to derive k_i (used to encrypt precious secret)
        ret = lt_mac_and_destroy(h, i, v, w);
        if(ret != LT_OK) {
            goto exit;
        }
        // Now the slot is initialized again by calling M&S sequence again with 'u'
        ret = lt_mac_and_destroy(h, i, u, garbage);
        if(ret != LT_OK) {
            goto exit;
        }

        // Derive k_i = KDF(w, PIN||A)
        // This key will be used to encrypt secret s
        lt_hmac_sha256(w, 32, kdf_input_buff, PIN_size+add_size, k_i);

        // Encrypt s using k_i as a key
        // TODO implement some better encryption, or discuss if using XOR here is fine
        for (int j=0; j<32; j++) {
            *(nvm.ci+(i*32 + j)) = k_i[j] ^ s[j];
        }
    }

    // Persistently save nvm data into TROPIC01's R memory slot
    ret = lt_r_mem_data_write(h, R_MEM_DATA_SLOT_MACANDD, (uint8_t*)&nvm, sizeof(nvm));
    if (ret != LT_OK) {
        goto exit;
    }

    // Final secret is released to the caller
    lt_hmac_sha256(s, 32, (uint8_t*)"2", 1, secret);

    // Cleanup all sensitive data from memory
    exit:

    memset(kdf_input_buff, 0, PIN_size+add_size);
    memset(u, 0, 32);
    memset(v, 0, 32);
    memset(w, 0, 32);
    memset(k_i, 0, 32);

    return ret;
}

/**
 * @brief Check PIN with Mac And Destroy
 *
 * @details There are more ways how to implement Mac And Destroy 'PIN check' functionality, differences could be in way of handling nvm data,
 *          number of tries, algorithm used for decryption, etc.
 *          This function is just one of the possible implementations of "PIN check", therefore we do not expose this through official libtropic API.
 *
 *          Take it as an inspiration, copy it into your project and adapt it to your specific hw resources.
 *
 * @param h           Device's handle
 * @param PIN         Array of bytes (size between MAC_AND_DESTROY_PIN_SIZE_MIN and MAC_AND_DESTROY_PIN_SIZE_MAX) representing PIN
 * @param PIN_size    Length of the PIN field
 * @param add         Additional data to be used in M&D sequence (size between MAC_AND_DESTROY_ADD_SIZE_MIN and MAC_AND_DESTROY_ADD_SIZE_MAX)
 * @param add_size    Length of additional data
 * @param secret      Buffer ito which secret will be saved
 * @return lt_ret_t   LT_OK if correct, otherwise LT_FAIL
 */
static lt_ret_t lt_PIN_check(lt_handle_t *h, const uint8_t *PIN, const uint8_t PIN_size, const uint8_t *add, const uint8_t add_size, uint8_t *secret)
{
    if(    !h
        || !PIN
        || (PIN_size < MAC_AND_DESTROY_PIN_SIZE_MIN)
        || (PIN_size > MAC_AND_DESTROY_PIN_SIZE_MAX)
        || !add
        || (add_size > MAC_AND_DESTROY_ADD_SIZE_MAX)
        || !secret
    ) {
        return LT_PARAM_ERR;
    }
    if(h->l3.session != SESSION_ON) {
        return LT_HOST_NO_SESSION;
    }

    // Clear variable for released secret so there is known data (zeroes) in case this function ended sooner then secret was prepared
    memset(secret, 0,32);

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
    uint8_t kdf_input_buff[MAC_AND_DESTROY_PIN_SIZE_MAX+MAC_AND_DESTROY_ADD_SIZE_MAX];
    memcpy(kdf_input_buff, PIN, PIN_size);
    memcpy(kdf_input_buff+PIN_size, add, add_size);

    // Load M&D data from TROPIC01's R memory
    lt_ret_t ret = lt_r_mem_data_read(h, R_MEM_DATA_SLOT_MACANDD, (uint8_t*)&nvm, sizeof(nvm));
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
    ret = lt_r_mem_data_write(h, R_MEM_DATA_SLOT_MACANDD, (uint8_t*)&nvm, sizeof(nvm));
    if (ret != LT_OK) {
        goto exit;
    }

    // Compute v' = KDF(0, PIN'||A).
    lt_hmac_sha256((uint8_t*)"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 32, kdf_input_buff, PIN_size + add_size, v_);

    // Execute w' = MACANDD(i, v')
    ret = lt_mac_and_destroy(h, nvm.i, v_, w_);
    if(ret != LT_OK) {
        goto exit;
    }

    // Compute k'_i = KDF(w', PIN'||A)
    lt_hmac_sha256(w_, 32, kdf_input_buff, PIN_size+add_size, k_i);

    // Read the ciphertext c_i and tag t from NVM, decrypt c_i with k'_i as the key and obtain s'
    // TODO figure out if XOR can be used here?
    for (int j=0; j<32; j++) {
        s_[j] = *(nvm.ci+(nvm.i*32 + j)) ^ k_i[j];
    }

    // Compute tag t = KDF(s, "0x00")
    lt_hmac_sha256(s_, 32, (uint8_t*)"0", 1, t_);

    // If t' != t: FAIL
    if(memcmp(nvm.t, t_, 32) != 0) {
        ret = LT_FAIL;
        goto exit;
    }

    // Pin is correct, now initialize macandd slots again:
    // Compute u = KDF(s', "0x01")
    lt_hmac_sha256(s_, 32, (uint8_t*)"1", 1, u);

    for(int x=nvm.i; x < MACANDD_ROUNDS-1; x++) {
        uint8_t garbage[32] = {0};

        ret = lt_mac_and_destroy(h, x, u, garbage);
        if(ret != LT_OK) {
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
    ret = lt_r_mem_data_write(h, R_MEM_DATA_SLOT_MACANDD, (uint8_t*)&nvm, sizeof(nvm));
    if (ret != LT_OK) {
        goto exit;
    }

    // Calculate secret and store it into passed array
    lt_hmac_sha256(s_, 32, (uint8_t*)"2", 1, secret);

    // Cleanup all sensitive data from memory
    exit:

    memset(kdf_input_buff, 0, PIN_size+add_size);
    memset(w_, 0, 32);
    memset(k_i, 0, 32);
    memset(v_, 0, 32);

    return ret;
}

/**
 * @brief Input function
 *
 * @param pin_attemp    Function for pin input
 * @return            pin_attemp if success, otherwise -1
 */


// Function to input and validate a 4-digit PIN
int get_valid_pin(uint8_t pin[4]) {
    LT_LOG("Enter a 4-digit PIN (no Enter key, just digits): ");

    for (int i = 0; i < 4; ++i) {
        int ch = getchar();

        if (ch == EOF) return 0;           // Input error
        if (!isdigit(ch)) return 0;        // Non-digit -> invalid

        pin[i] = ch - '0';
    }
    return 1;
}

void print_palms(int cnt){
    for (size_t i = 0; i < cnt; i++)
    {
        printf(PALM_TREE);
    }
    printf("\n");
}


static int send_response(int socket, uint8_t *buffer, size_t length)
{
    int nb_bytes_sent;
    int nb_bytes_sent_total = 0;
    int nb_bytes_to_send = length;
    uint8_t *ptr = buffer;

    // attempt several times to send the data
    for (int i = 0; i < TX_ATTEMPTS; i++) {
        nb_bytes_sent = send(socket, ptr, nb_bytes_to_send, 0);
        if (nb_bytes_sent <= 0) {
            return 1;
        }

        nb_bytes_to_send -= nb_bytes_sent;
        if (nb_bytes_to_send == 0) {
            return 0;
        }

        ptr += nb_bytes_sent;
        nb_bytes_sent_total += nb_bytes_sent;
    }

    return 1;
}

static int send_status_update(int socket, const char* status_type, int current, int maximum)
{
    char status_msg[BUFFER_SIZE];
    snprintf(status_msg, sizeof(status_msg), "STATUS:%s:%d:%d", status_type, current, maximum);
    
    return send_response(socket, (uint8_t*)status_msg, strlen(status_msg));
}

static int parse_pin_from_ascii(const char* ascii_pin, uint8_t* pin_out, int* pin_len_out)
{
    int len = strlen(ascii_pin);
    if (len < MAC_AND_DESTROY_PIN_SIZE_MIN || len > MAC_AND_DESTROY_PIN_SIZE_MAX) {
        return 0;  // Invalid length
    }
    
    for (int i = 0; i < len; i++) {
        if (!isdigit(ascii_pin[i])) {
            return 0;  // Invalid character
        }
        pin_out[i] = ascii_pin[i] - '0';
    }
    
    *pin_len_out = len;
    return 1;  // Success
}

static int handle_set_pin_command(int socket, const char* new_pin_str, lt_handle_t *h, uint8_t *additional_data, uint8_t additional_data_size)
{
    uint8_t new_pin[MAX_PIN_LEN];
    int new_pin_len;
    uint8_t secret[32];
    
    // Parse the new PIN
    if (!parse_pin_from_ascii(new_pin_str, new_pin, &new_pin_len)) {
        LT_LOG("Invalid PIN format in SET_PIN command: %s", new_pin_str);
        uint8_t fail_response = LT_FAIL;
        return send_response(socket, &fail_response, 1);
    }
    
    // Set the new PIN using MAC-and-Destroy
    lt_ret_t ret = lt_PIN_set(h, new_pin, new_pin_len, additional_data, additional_data_size, secret);
    
    if (ret == LT_OK) {
        // Update global state
        memcpy(current_pin, new_pin, new_pin_len);
        current_pin_len = new_pin_len;
        current_attempts = 0;
        is_locked = false;
        
        //LT_LOG("PIN updated successfully to: %s", new_pin_str);
        LT_LOG("New secret: %s", print_bytes(secret, 32));
        
        // Send success response
        uint8_t success_response = LT_OK;
        return send_response(socket, &success_response, 1);
    } else {
        LT_LOG("Failed to set new PIN: %s", new_pin_str);
        uint8_t fail_response = LT_FAIL;
        return send_response(socket, &fail_response, 1);
    }
}

static int handle_set_attempts_command(int socket, const char* attempts_str)
{
    int new_attempts = atoi(attempts_str);
    
    // Validate attempts value
    if (new_attempts < 1 || new_attempts > MACANDD_ROUNDS) {
        LT_LOG("Invalid attempts value: %s (must be 1-%d)", attempts_str, MACANDD_ROUNDS);
        uint8_t fail_response = LT_FAIL;
        return send_response(socket, &fail_response, 1);
    }
    
    // Update global state
    max_attempts = new_attempts;
    // Reset current attempts and unlock if necessary
    current_attempts = 0;
    is_locked = false;
    
    LT_LOG("Max attempts updated to: %d", new_attempts);
    
    // Send success response
    uint8_t success_response = LT_OK;
    if (send_response(socket, &success_response, 1) != 0) {
        return 1;
    }
    
    // Send status update to confirm the change
    return send_status_update(socket, "RESET", current_attempts, max_attempts);
}

static int handle_pin_verification(int socket, const char* pin_str, lt_handle_t *h, uint8_t *additional_data, uint8_t additional_data_size)
{
    uint8_t pin_attempt[MAX_PIN_LEN];
    int pin_len;
    uint8_t secret[32];
    
    // Check if locked
    if (is_locked) {
        LT_LOG("PIN verification blocked - system is locked");
        uint8_t fail_response = LT_FAIL;
        send_response(socket, &fail_response, 1);
        send_status_update(socket, "LOCKED", current_attempts, max_attempts);
        return 0;
    }
    
    // Parse the PIN attempt
    if (!parse_pin_from_ascii(pin_str, pin_attempt, &pin_len)) {
        LT_LOG("Invalid PIN format: %s", pin_str);
        uint8_t fail_response = LT_FAIL;
        send_response(socket, &fail_response, 1);
        send_status_update(socket, "ATTEMPTS", current_attempts, max_attempts);
        return 0;
    }
    
    // Verify PIN using MAC-and-Destroy
    lt_ret_t ret = lt_PIN_check(h, pin_attempt, pin_len, additional_data, additional_data_size, secret);
    
    if (ret == LT_OK) {
        // PIN correct - reset attempts
        current_attempts = 0;
        is_locked = false;
        
        LT_LOG("PIN verification successful!");
        LT_LOG("Released secret: %s", print_bytes(secret, 32));
        
        // Send success response
        uint8_t success_response = LT_OK;
        return send_response(socket, &success_response, 1);
    } else {
        // PIN incorrect - increment attempts
        current_attempts++;
        
        if (current_attempts >= max_attempts) {
            is_locked = true;
            LT_LOG("Maximum attempts reached - system locked");
            uint8_t fail_response = LT_FAIL;
            send_response(socket, &fail_response, 1);
            send_status_update(socket, "LOCKED", current_attempts, max_attempts);
        } else {
            LT_LOG("PIN verification failed - attempts: %d/%d", current_attempts, max_attempts);
            uint8_t fail_response = LT_FAIL;
            send_response(socket, &fail_response, 1);
            send_status_update(socket, "ATTEMPTS", current_attempts, max_attempts);
        }
        
        return 0;
    }
}

int tcp_server_main(lt_handle_t *h, uint8_t *additional_data, uint8_t additional_data_size)
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    
    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        LT_LOG("Socket creation failed");
        return -1;
    }
    
    // Set socket options to reuse address
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        LT_LOG("Setsockopt failed");
        close(server_fd);
        return -1;
    }
    
    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    // Bind socket
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        LT_LOG("Bind failed");
        close(server_fd);
        return -1;
    }
    
    // Listen for connections
    if (listen(server_fd, 1) < 0) {
        LT_LOG("Listen failed");
        close(server_fd);
        return -1;
    }
    
    LT_LOG("🌴 TCP Server listening on port %d", PORT);
    //LT_LOG("🌴 Current PIN: %s", print_bytes(current_pin, current_pin_len));
    LT_LOG("🌴 Waiting for Python client connection...");
    
    // Accept client connection
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        LT_LOG("Accept failed");
        close(server_fd);
        return -1;
    }
    
    LT_LOG("🌴 Client connected!");
    
    // Handle client communication
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_received <= 0) {
            LT_LOG("Client disconnected");
            break;
        }
        
        buffer[bytes_received] = '\0';  // Null-terminate
        //LT_LOG("Received: %s", buffer);
        
        // Check if it's a SET_PIN command
        if (strncmp(buffer, "SET_PIN:", 8) == 0) {
            char* new_pin = buffer + 8;  // Skip "SET_PIN:"
            handle_set_pin_command(client_fd, new_pin, h, additional_data, additional_data_size);
        } 
        // Check if it's a SET_ATTEMPTS command
        else if (strncmp(buffer, "SET_ATTEMPTS:", 13) == 0) {
            char* attempts_str = buffer + 13;  // Skip "SET_ATTEMPTS:"
            handle_set_attempts_command(client_fd, attempts_str);
        } 
        else {
            // Treat as PIN verification attempt
            handle_pin_verification(client_fd, buffer, h, additional_data, additional_data_size);
        }
    }
    
    close(client_fd);
    close(server_fd);
    
    return 0;
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
    uint8_t l3_buffer[L3_FRAME_MAX_SIZE] __attribute__ ((aligned (16))) = {0};
    h.l3.buff = l3_buffer;
    h.l3.buff_len = sizeof(l3_buffer);
#endif

    lt_init(&h);

    LT_LOG("%s", "Creating secure session with H0 keys");
    LT_ASSERT(LT_OK, verify_chip_and_start_secure_session(&h, sh0priv, sh0pub, pkey_index_0));

    // This variable stores secret which is released to the user after successful PIN check or PIN set
    uint8_t secret[32] = {0};

    // Additional data passed by user besides PIN - this is optional, but recommended
    uint8_t additional_data[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                                 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    uint8_t additional_data_size = sizeof(additional_data);

    // User's PIN (initial PIN for MAC-and-Destroy setup)
    uint8_t pin[] = {1,2,3,4};

    LT_LOG_LINE();

    /*
    // Set the PIN and log out the secret
    LT_LOG("Number of Mac And Destroy tries is set to %d", MACANDD_ROUNDS);
    LT_LOG("%s", "lt_PIN_set(): user sets the PIN");
    LT_ASSERT(LT_OK, lt_PIN_set(&h, pin, 4, additional_data, additional_data_size, secret));
    LT_LOG("Initialized secret: %s", print_bytes(secret, 32));
    LT_LOG_LINE();

    LT_LOG("%s", "  \"i<MACANDD_ROUNDS-1\"  PIN check attempts with wrong PIN");
    for (int i = 0; i<MACANDD_ROUNDS-1; i++) {
        LT_LOG("lt_PIN_check() user inputs wrong PIN - slot %d destroyed", i);
        LT_ASSERT(LT_FAIL, lt_PIN_check(&h, pin_attemp, 4, additional_data, additional_data_size, secret));
        LT_LOG("secret: %s", print_bytes(secret, 32));
    }

    LT_LOG("%s", "Final PIN attempt with correct PIN, slots are reinitialized again - lt_PIN_check() with correct PIN");
    LT_ASSERT(LT_OK, lt_PIN_check(&h, pin, 4, additional_data, additional_data_size, secret));
    LT_LOG("Exported secret: %s", print_bytes(secret, 32));
    LT_LOG_LINE();
    */

    
    // Set the PIN and log out the secret
    LT_LOG("Number of Mac And Destroy tries is set to %d", MACANDD_ROUNDS);
    LT_LOG("%s", "lt_PIN_set(): user sets the PIN");
    
    LT_LOG("\t=======================================================================");
    LT_ASSERT(LT_OK, lt_PIN_set(&h, pin, 4, additional_data, additional_data_size, secret));
    LT_LOG("Initialized secret: %s", print_bytes(secret, 32));
    LT_LOG("\t=======================================================================");



    // Start TCP server to handle PIN verification requests from Python client
    LT_LOG("\t=======================================================================");
    LT_LOG("\t============ Starting TCP Server for PIN Demo ===============");
    LT_LOG("\t=======================================================================");
    
    tcp_server_main(&h, additional_data, additional_data_size);
    
    ////////////////////////////////////////////////////////////////////////////////////////
    LT_LOG("%s", "Aborting session H0");
    LT_ASSERT(LT_OK, lt_session_abort(&h));

    lt_deinit(&h);

    return 0;
}

int lt_ex_macandd_pin_demo(void)
{
    print_palms(60);
    LT_LOG("\t=====  TROPIC01 Hello World FROM PIN DEMO                           ===");
    print_palms(60);
    LT_LOG_LINE();
    if(session_H0() == -1) {
        printf("\r\nError during session_H0()\r\n");
        return -1;
    }

    LT_LOG_LINE();

    return 0;
}
