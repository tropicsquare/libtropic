# Trezor Crypto
Trezor Crypto is a cryptography library developed by [Trezor](https://trezor.io/) and available as a part of the [Trezor Firmware repository](https://github.com/trezor/trezor-firmware).

CAL files of this port are available in the `libtropic/cal/trezor_crypto/` directory.

Due to historical reasons and testing purposes, we have the Trezor Crypto copied into our `vendor/` directory. Because Trezor Crypto does not have its own repository, does not use CMake and Trezor Firmware would be a very big dependency, we decided to copy it into our `vendor/` and create our own CMakeLists.txt to easily integrate it.

!!! warning
    We strongly advice users that want to use Trezor Crypto in their application to **not** use our out-of-date copy of Trezor Crypto inside `vendor/`, but use the version found in the [Trezor Firmware repository](https://github.com/trezor/trezor-firmware) instead and handle the dependency themselves.

!!! warning
    TROPIC01 PKI chain validation cannot be done using the Trezor Crypto only - additional crypto libraries would have to be used. If you need to validate the chain, use other supported libraries that provide the needed functionality.