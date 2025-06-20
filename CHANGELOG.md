# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Changed

- `ECC_SLOT1-32` numbering changed to `ECC_SLOT_0-31`
- `lt_handshake()` function renamed to `lt_session_start()`
- Unit tests and platform tests organization was updated
- Included `lt_l2_api_structs.h` and `lt_l3_api_structs.h` are automatically generated
- `SH0` pairing keypair in `keys/keys.c` now reflects engineering samples of TROPIC01 devices
- Changed logging format in examples
- Renamed cmake switches: `LT_USE_TREZOR_CRYPTO`, LT_BUILD_DOCS
- Platform examples are removed - now they are standalone repositories
- Separated libtropic API calls introduced (session_start and ECC_ECDSA functions):
  - lt_l3.* contain interfaces for encoding l3 data going out and decoding l3 data going in
  - libtropic.* then wraps l3 functions and does the whole l3 exchange in one round
  - This change allows user to send data through a tunnel between host and target MCU,
    which then behaves as a relay
- Separated l2 API calls introduced
  - Now user can use separate function for sending and receiving data
- Refactoring of `lt_handle_t` struct:
  - contains separate structs for l2 and l3
  - allows to split all l2 and l3 calls into separate actions
- Option to define l3 buffer introduced (`LT_SEPARATE_L3_BUFF`)
- Introduced a new folder `keys/` where user might specify SH0-SH3 keys
- remove `lt_serial_code_get()`
- rename `LT_L2_STARTUP_ID_MAINTENANCE` to `LT_MODE_MAINTENANCE`
- rename `LT_L2_STARTUP_ID_REBOOT` to `LT_MODE_APP`
- Code formatted with `clang-format` version 16 is now enforced on push to branch
- Moved logging and assert macros to `libtropic_logging.h` (they were defined twice in
files `libtropic_examples.h`, `libtropic_functional_tests.h`)


### Added

- L3 commands: MCounter (init, update and get), MAC-and-destroy,
- L2 commands: Resend, Sleep and Log req
- libtropic API: lt_update_mode()
- Example folder and test folder is reworked
- option LT_ADD_EXAMPLES controls if example's folder code is a part of compilation
- option LT_ADD_FUNC_TESTS controls if code from tests/functional/ folder will be compiled-in
- port/ support and tests/platform/ example for Raspberry Pi
- Added macro `LT_USE_ASSERT`, based on which `assert()` is inserted into `LT_ASSERT` and `LT_ASSERT_COND`
- added CTest handling for functional tests

### Fixed

- Return values for L3 API
