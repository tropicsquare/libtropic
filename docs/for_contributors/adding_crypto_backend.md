# Adding a New Cryptographic Backend
As libtropic was designed with extensibility in mind, it is possible to add a new backend for the cryptographic operations.

## Requirements
The new cryptographic backend has to support the following schemes:

- **AES (GCM)**
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

### Create and Implement the Necessary Files
1. Inside `hal/crypto/`, create a new directory called `mycrypto`.
2. Inside `hal/crypto/mycrypto/`, create the following source files:
    - `lt_crypto_mycrypto_aesgcm.c`,
    - `lt_crypto_mycrypto_ecdsa.c`,
    - `lt_crypto_mycrypto_ed25519.c`,
    - `lt_crypto_mycrypto_sha256.c`,
    - `lt_crypto_mycrypto_hmac_sha256.c`,
    - `lt_crypto_mycrypto_x25519.c`.
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
        To implement Curve25519 functions, copy declarations from `lt_x25519.h` to `lt_crypto_mycrypto_x25519.c` and provide implementations.
        You can use existing ports inside `hal/crypto/` for inspiration.

### Edit the Main CMakeLists.txt
The cryptographic provider is selected using the `LT_CRYPTO` CMake option - a string, specifying the provider, is expected. Currently, `trezor_crypto` is the only available provider.

The following steps describe how the `CMakeLists.txt` has to be edited to add support for the new cryptographic provider:

1. In the `Collect files` section, add the new source files to the compilation process. Use the following template, modify it accordingly, and paste it above the `# --- add new crypto sources above this line ---` comment:
```cmake
set(SDK_SRCS ${SDK_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/mycrypto/lt_crypto_mycrypto_aesgcm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/mycrypto/lt_crypto_mycrypto_ed25519.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/mycrypto/lt_crypto_mycrypto_ecdsa.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/mycrypto/lt_crypto_mycrypto_sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/mycrypto/lt_crypto_mycrypto_hmac_sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/crypto/mycrypto/lt_crypto_mycrypto_x25519.c
)
```

At the end of `CMakeLists.txt`, in the section `# LT_CRYPTO handling` do the following:

1. Add the new provider to the `CRYPTO_PROVIDERS` list:
```cmake
# Supported crypto providers
set(CRYPTO_PROVIDERS "trezor_crypto" "mycrypto")
```
2. Define new internal macro:
```cmake
# These are internal macros for crypto providers
set(LT_CRYPTO_TREZOR   0 CACHE INTERNAL "")
set(LT_CRYPTO_MYCRYPTO 0 CACHE INTERNAL "") # <-- new addition
```
3. Add new `elseif()` in the `LT_CRYPTO` handling:
```cmake
# Handle LT_CRYPTO
if(LT_CRYPTO STREQUAL "trezor_crypto")

    # ... trezor_crypto handling ...

# --- start of new additions ---
elseif(LT_CRYPTO STREQUAL "mycrypto")

    message(STATUS "Crypto provider set to mycrypto")
    set(LT_CRYPTO_MYCRYPTO 1)

    add_subdirectory(vendor/mycrypto/ "mycrypto")
    target_compile_definitions(mycrypto PRIVATE <edit_me>) # Edit or remove.
    target_link_libraries(tropic PRIVATE mycrypto)
    # Add more lines if necessary.

# --- end of new additions ---
else()
    # ...
endif()
```
4. Propagate the new internal macro `LT_CRYPTO_MYCRYPTO`:
```cmake
# Propagate crypto provider internal switches
target_compile_definitions(tropic PUBLIC
    LT_CRYPTO_TREZOR=${LT_CRYPTO_TREZOR}
    LT_CRYPTO_MYCRYPTO=${LT_CRYPTO_MYCRYPTO}
)
```