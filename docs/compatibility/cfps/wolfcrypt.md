# WolfCrypt
WolfCrypt is a cryptography library which is part of [WolfSSL](https://www.wolfssl.com/products/wolfssl/) embedded SSL/TLS library.

!!! warning "WolfSSL Licensing"
    WolfSSL is licensed under GPLv3, meaning that when you use Libtropic with WolfCrypt (e.g. when building our tests with the WolfCrypt CAL), you are creating a GPLv3 licensed binary. If you require a non-GPL binary, you must obtain a commercial license from wolfSSL Inc.

CAL files of this port are available in the `libtropic/cal/wolfcrypt/` directory.

## Configuration
WolfSSL must be configured with the following features (CMake options):

- `WOLFSSL_AESGCM`,
- `WOLFSSL_SHA256`,
- `WOLFSSL_CURVE25519`.

## Initialization and Deinitialization
Libtropic does not handle initialization and deinitialization of WolfCrypt, this is the user's responsibility. Specifically, it is assumed that:

1. [wolfCrypt_Init](https://www.wolfssl.com/documentation/manuals/wolfssl/group__wolfCrypt.html#function-wolfcrypt_init) is called before the instance of the CAL is handed to Libtropic's `lt_init` function. See the [Libtropic Bare-Bone Example](../../reference/integrating_libtropic/how_to_use/index.md#libtropic-bare-bone-example) for more information about the CAL instance.
2. [wolfCrypt_Cleanup](https://www.wolfssl.com/documentation/manuals/wolfssl/group__wolfCrypt.html#function-wolfcrypt_cleanup) is called in the user's application cleanup logic. Although freeing the WolfCrypt's resources is not required by Libtropic, it **cannot** be called sooner than the last call of Libtropic's `lt_deinit` function, otherwise all Secure Channel Session related commands will return with errors.

## Cryptographic Callbacks
The [Cryptographic callbacks](https://www.wolfssl.com/wolfcrypt-support-cryptographic-callbacks/) are currently not supported by the CAL.