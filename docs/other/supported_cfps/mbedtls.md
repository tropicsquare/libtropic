# MbedTLS
Currently, we support the [MbedTLS v4.0.0](https://github.com/Mbed-TLS/mbedtls/tree/v4.0.0) and use the [PSA Crypto API](https://mbed-tls.readthedocs.io/en/latest/getting_started/psa/) in our `mbedtls_v4` CAL (Crypto Abstraction Layer).

CAL files of this port are available in the `libtropic/cal/mbedtls_v4/` directory.

!!! warning
    Note that Libtropic does not handle initialization and deinitialization of MbedTLS, this is the user's responsibility. Specifically, it is assumed that:
    
    1. [psa_crypto_init()](https://mbed-tls.readthedocs.io/projects/api/en/development/api/group/group__initialization/#_CPPv415psa_crypto_initv) is called before the instance of the CAL is handed to Libtropic's `lt_init()` (see the [Libtropic Bare-Bone Example](../../get_started/integrating_libtropic/how_to_use/index.md#libtropic-bare-bone-example) for more information),
    2. [mbedtls_psa_crypto_free()](https://mbed-tls.readthedocs.io/projects/api/en/development/api/file/crypto__extra_8h/#_CPPv423mbedtls_psa_crypto_freev) is called at the user's application cleanup logic. Although freeing the MbedTLS's resources is not required by Libtropic, it **cannot** be called sooner than the last call of Libtropic's `lt_deinit()`, otherwise all Secure Channel Session related commands will return with errors.