# Examples
The `examples/` directory contains multiple examples demonstrating how to use libtropic.

Some examples may cause irreversible changes to the chip, so they are organized into two categories:

- [Reversible Examples](reversible_examples.md),
- [Irreversible Examples](irreversible_examples.md).
!!! note
    Examples are not compiled by default. To compile examples, either

    - pass `-DLT_BUILD_EXAMPLES=1` to `cmake` during compilation, or
    - in your CMake file, switch this option on: `set(LT_BUILD_EXAMPLES ON)`.

!!! note
    During build, SH0 keypair is automatically chosen from `libtropic/provisioning_data/<lab_batch_package_directory>/sh0_key_pair/`, this SH0 key is present in the majority of distributed TROPIC01 chips. In certain cases (first engineering samples) it might be necessary to manually set it (in PEM or DER format) with following cmake switch: `-DLT_SH0_PRIV_PATH=<path to sh0_priv_engineering_sample01.pem>`.