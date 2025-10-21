# Adding a New Cryptographic Backend
Because Libtropic is designed to run on the Host MCU, it requires certain cryptographic capabilities - for example, to decrypt incoming L3 packets from TROPIC01. To enable this, Libtropic defines an interface that abstracts the functionality of a chosen cryptographic backend. This interface-based design makes it easy to integrate and support additional cryptographic backends in the future.

## Requirements
The new cryptographic backend has to support the following schemes:

- **AES-GCM**
    - encryption
    - decryption
- **ECDSA**
    - signing
- **ED25519**
    - signing
- **SHA256**
    - hashing
- **Curve25519**
    - multiplication on both arbitrary and base point

## Guide
To add a new cryptographic backend (let's say `mycrypto`):

1. [Create and Implement the Necessary Files](#create-and-implement-the-necessary-files), 
2. [Edit the Main CMakeLists.txt](#edit-the-main-cmakeliststxt).

After these steps, the new cryptographic backend should be selectable during compilation by setting `LT_CRYPTO` CMake variable to `"mycrypto"`.

### Create and Implement the Necessary Files
1. Inside `hal/crypto/`, create a new directory called `mycrypto`.
2. Inside `hal/crypto/mycrypto/`, create the following source files:
    - `lt_mycrypto_aesgcm.c`,
    - `lt_mycrypto_ecdsa.c`,
    - `lt_mycrypto_ed25519.c`,
    - `lt_mycrypto_sha256.c`,
    - `lt_mycrypto_hmac_sha256.c`,
    - `lt_mycrypto_x25519.c`.
3. In each of the source files, implement all required functions - they are declared in respective headers inside the `src/` directory:
    - `lt_aesgcm.h`: AES-GCM functions,
    - `lt_ecdsa.h`: ECDSA functions,
    - `lt_ed25519.h`: ED25519 functions,
    - `lt_sha256.h`: SHA256 functions,
    - `lt_hmac_sha256.h`: HMAC SHA256 functions,
    - `lt_x25519.h`: Curve25519 functions.

    Look into each header - the exact purpose of every function is described in its comment. Copy the function declarations
    from the headers to the source files and implement the functions.
    !!! example
        To implement Curve25519 functions, copy declarations from `lt_x25519.h` to `lt_mycrypto_x25519.c` and provide implementations.
        You can use existing ports inside `hal/crypto/` for inspiration.

4. Inside `hal/crypto/mycrypto/`, create a file `lt_crypto_macros.h`. This file should define the following macros:
    - `LT_CRYPTO_AES_GCM_CTX_T`: type of the AES-GCM context `mycrypto` uses,
    - `LT_CRYPTO_SHA256_CTX_T`: type of the SHA256 context `mycrypto` uses.

    !!! info
        The name of `lt_crypto_macros.h` does not contain the name of the cryptographic backend (in this case `mycrypto`), because it is included in some Libtropic headers that do not know which cryptographic backend will be compiled (this is decided at compile time). The same goes for the aforementioned macros in `lt_crypto_macros.h`, because Libtropic often needs to instantiate a cryptographic context for e.g. AES-GCM functions, but does not have information about which backend is it working with.

### Edit the Main CMakeLists.txt
The cryptographic provider is selected using the `LT_CRYPTO` CMake option - a string, specifying the provider, is expected. In this case, the string would be `mycrypto`.

The following steps describe how the `CMakeLists.txt` has to be edited to add support for the new cryptographic provider:

1. In the `# Setup` section of the `CMakeLists.txt`, add the new string value for the `LT_CRYPTO` variable:
```cmake
# Crypto provider
set(LT_CRYPTO "" CACHE STRING "Set a cryptography provider") # no change
set_property(CACHE LT_CRYPTO PROPERTY STRINGS "cryptoXY" "mycrypto") # add the new string here
```
2. At the end of `CMakeLists.txt`, in the section `# LT_CRYPTO handling`, add a new `elseif()` in the `LT_CRYPTO` handling:
```cmake
# Handle LT_CRYPTO
if(LT_CRYPTO STREQUAL "trezor_crypto")

    # ... trezor_crypto handling ...

# --- start of new additions ---
elseif(LT_CRYPTO STREQUAL "mycrypto")

    message(STATUS "Crypto provider set to mycrypto")

    add_subdirectory(vendor/<path_to_crypto_backend>/ "mycrypto")
    target_compile_definitions(mycrypto PRIVATE <edit_me>) # Edit or remove.
    target_link_libraries(tropic PUBLIC mycrypto)
    # Add more lines if necessary.

# --- end of new additions ---
else()
    # ...
endif()
```