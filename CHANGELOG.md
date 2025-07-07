# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Changed
- Enhanced internal test runner logging and error reporting

### Added

### Fixed

### Removed

## [0.1.0]

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
- Add `LT_PRINT_SPI_DATA` - used to debug print low level communication
- Moved logging and assert macros to `libtropic_logging.h` (they were defined twice in
files `libtropic_examples.h`, `libtropic_functional_tests.h`)
- Instead of using `LT_ADD_FUNC_TESTS` and `LT_BUILD_TESTS`, use only `LT_BUILD_TESTS`
- Rename `LT_ADD_EXAMPLES` to `LT_BUILD_EXAMPLES` to be consistent with `LT_BUILD_TESTS`
  and `LT_BUILD_DOCS`
- Always increment host's nonce in `lt_l3_decrypt_response()`
- When `LT_BUILD_EXAMPLES` or `LT_BUILD_TESTS` are set, CMake requires to define `LT_SH0_PRIV_PATH`
- Removed SH0 keys and pkey_index_{0,1,2,3} from `keys/keys.c` and reflected it in examples, tests
- Added writing of pairing keys to slots 1,2,3 in `lt_test_ping` and created `lt_test_engineering_sample_01_ping`
  from the old version of `lt_test_ping`
- Set default value to `LT_SH0_PRIV_PATH`


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
- Added `SESSION_OFF` for indicating that secure session is off and assigned it to `lt_l3_state_t.session` flag in `lt_init()`.
- Added function `lt_l3_invalidate_host_session_data()`, which sets all data in `lt_l3_state_t` to zero and `session` to `SESSION_OFF`; it is called in `lt_session_abort` and when ciphering/deciphering of L3 command fails (wrong TAG)
- Added CMake variable `LT_SH0_PRIV_PATH`, based on which `sh0_keys.c` is generated for examples or tests, so sh0priv and sh0pub are defined
- Added CTest handling for functional tests run against TROPIC01 model

### Fixed

- Return values for L3 API
