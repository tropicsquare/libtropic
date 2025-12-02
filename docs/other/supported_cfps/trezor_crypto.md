# Trezor Crypto
Trezor Crypto is a cryptography library developed by [Trezor](https://trezor.io/) and available as a part of the [Trezor Firmware repository](https://github.com/trezor/trezor-firmware).

CAL files of this port are available in the `libtropic/cal/trezor_crypto/` directory.

Due to historical reasons and testing purposes, we have our own copy of the Trezor Crypto in the `vendor/` directory, as the Trezor Crypto is a part of a Trezor Firmware repository and does not use CMake.

!!! danger "Trezor Crypto Version"
    We strongly advise users that want to use Trezor Crypto in production applications to **not** use our out-of-date copy of Trezor Crypto inside `vendor/`, but use the version found in the [Trezor Firmware repository](https://github.com/trezor/trezor-firmware) instead and handle the dependency themselves.

!!! failure "TROPIC01 PKI Chain Validation"
    TROPIC01 PKI chain validation cannot be done using the Trezor Crypto only, additional crypto libraries have to be used. If you need to validate the chain, use other supported libraries that provide the required functionality.