# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Changed
- Changed prefixes of all platform HAL files to `libtropic_`.
- `lt_l2.h`, `lt_l2.c`, `lt_l3.h`, `lt_l3.c`: change prefix to `libtropic_`.

### Added
- CMake option for setting logging verbosity level: `LT_LOG_LVL`.
- Compiler and linker flags to delete unused sections.
- Macro `MCOUNTER_VALUE_MAX` for the maximal allowed value of monotonic counter.

### Fixed
- `lt_r_mem_data_write()`, `lt_out__r_mem_data_write()`: Mark `data` as `const`.
- `lt_mcounter_init()`: Allow `mcounter_value` only from range 0-`MCOUNTER_VALUE_MAX`.

### Removed

## [1.0.0]

### Changed
- Enhanced internal test runner logging and error reporting.
- Renamed `LT_L2_LAST_RET` to `LT_RET_T_LAST_VALUE` for clarity in `lt_ret_t`.
- Refactored `lt_ex_macandd` example.
- Refactored `lt_ex_hello_world_separate_API` example.
- Refactored `lt_ex_hello_world` example.
- Refactored `lt_ex_hw_wallet` example.
- Use `LIBT_DEBUG` instead of `NDEBUG` in `lt_port_{unix_tcp,raspberrypi_wiringpi}.c`.
- Renamed `config_description_table` to `cfg_desc_table`.
- Functions `lt_ecc_eddsa_sign()`, `lt_ecc_ecdsa_sign()`, `lt_ecc_eddsa_sig_verify()` and `lt_ecc_ecdsa_sig_verify()` accept zero length of the message to be signed.
- Function `lt_ecc_ecdsa_sign()` does not restrict message length - type of `msg_len` changed to `size_t`.
- Renamed `lt_random_get()` to `lt_random_value_get()`, along with the separate API, to match the command name in the User API.
- Renamed `L3_PACKET_MAX_SIZE` to `L3_CYPHERTEXT_MAX_SIZE`.
- Renamed `L3_FRAME_MAX_SIZE` to `L3_PACKET_MAX_SIZE`.
- Set `L3_RES_SIZE_SIZE` directly to value 2.
- Renamed `STATIC` to `LT_STATIC` due to naming conflicts.
- Moved `lt_uart_def_unix_t` to `libtropic_port.h`.
- Updated `enum CONFIGURATION_OBJECTS_REGS` with values from `include/tropic01_application_co.h` and `include/tropic01_bootloader_co.h` to be compatible with User API v1.3.0.
- Use strict format string types from `inttypes.h` or `%zu` for `size_t` (increases good portability).
- Moved `model_based_project/` from `tests/` to repo root and renamed to `tropic01_model/`.
- Added prefix `lt_` to all helper functions.
- Renamed Generic Linux SPI port to `lt_port_unix_spi.c`.
- Runtime state of Generic Linux ports (SPI, USB) is now kept in device structure in handle. This structure
  is also used for port configuration (device mapping, speeds...).
- Strict compile flags can now be activated with `LT_STRICT_COMP_FLAGS` CMake variable, which is automatically set when examples or tests are built.
- API change in `lt_random_bytes()`, `lt_port_random_bytes()`: changed `buff` parameter to `void*`, renamed parameter `len` to `count`
- API change in `lt_random_bytes()`, `lt_port_random_bytes()`: added `s2` parameter.
- API change in `lt_get_log()`: renamed to `lt_get_log_req()`, renamed `msg_len_max` to `log_msg_len` and made an output parameter.
- API change in `lt_port_spi_transfer()`, `lt_l1_spi_transfer()`, `lt_l1_read()`, `lt_l1_write()`: renamed `timeout` to `timeout_ms`

### Added
- Macro `LT_CONFIG_OBJ_CNT` for number of objects in the configuration structure.
- Parameter checks to LT_HELPERS functions.
- Added `LT_NONCE_OVERFLOW` to `lt_ret_t`.
- Value `LT_L3_R_MEM_DATA_READ_SLOT_EMPTY` to `lt_ret_t` to indicate that the user slot is empty.
- Functions `lt_ecc_ecdsa_sig_verify()` and `lt_ecdsa_verify()`.
- Strict compilation flags for examples and tests.
- Generic Unix SPI device support.
- Helper functions `lt_print_bytes()` and `lt_print_chip_id()`.
- Macros for chip package IDs and FAB IDs.
- Header files `include/tropic01_application_co.h` and `include/tropic01_bootloader_co.h` with address enums and mask defines.
- Model configuration is automatically created when tests are run in `tropic01_model/`.
- Enum `CONFIGURATION_OBJECTS_REGS_IDX` with indexes of `lt_config_t.obj`.
- Macros `TO_MACANDD_SLOT_0_31`, `TO_MACANDD_SLOT_32_63`, `TO_MACANDD_SLOT_64_95`, `TO_MACANDD_SLOT_96_127`.
- Model project: ability to run tests with AddressSanitizer or Valgrind with `LT_ASAN` or `LT_VALGRIND`.
- Support for `RESP_DISABLED` value of STATUS field in L2 frame.

### Fixed
- Assigned value to `LT_L1_CHIP_BUSY` and `LT_L1_INT_TIMEOUT` in `lt_ret_t`, incremented all values starting from `LT_L3_R_MEM_DATA_WRITE_WRITE_FAIL`.
- Added missing `LT_L1_INT_TIMEOUT` in `lt_ret_strs`.
- Fixed nonce overflow vulnerability in lt_l3_nonce_increase, the function now returns `LT_NONCE_OVERFLOW` if the nonce cannot be increased anymore.
- Calculation of number of chunks in the function `lt_l2_send_encrypted_cmd()`.
- Chip id parsing in `tests/model_based_project/create_model_cfg.py`.
- Function `lt_r_mem_data_read` -  change `size` to output parameter and its handling.
- Set value of macro `L3_CMD_DATA_SIZE_MAX` to 4111.
- Added 13 of maximal padding to macro `L3_FRAME_MAX_SIZE`.
- Set value of macro `PING_LEN_MAX` to 4096.
- Allowed mcounter index 0 in libtropic API (`lt_mcounter_init`).
- Bug in `lt_random_value_get` which caused stack smashing.

### Removed
- Macro `LT_USE_ASSERT`, `assert()` is always inserted.
- Functions `get_conf_addr()`, `get_conf_desc()`, accessing the configuration table directly is advised instead.
- Removed `keylen` parameter from `lt_ecc_key_read()` and `lt_in__ecc_key_read()`.
- Removed `rs_len` parameter from `lt_ecc_eddsa_sign()` and `lt_ecc_ecdsa_sign()`.
- Removed macro `LT_L3_EDDSA_SIGN_CMD_MSG_LEN_MIN`.
- Header file `include/TROPIC01_configuration_objects.h`.
- Checks with `<` for enums (fixes compilation errors on STM32 and shouldn't be needed anyway).
- File `tests/model_based_project/compile_tests.sh` (refer to `tropic01_model/README.md` for instructions about compiling tests).
- File `tests/model_based_project/model_logging_cfg.py`.
- WiringPi port (`lt_port_raspberrypi_wiringpi.c`), was replaced with Generic Linux SPI port (`lt_port_unix_spi.c`).
- Parameter `max_len` from functions `lt_get_info_riscv_fw_ver()` and `lt_get_info_spect_fw_ver()`.
- Attribute `__attribute__((aligned(4)))` for `struct header_boot_v2_t`.

## [0.1.0]

### Changed

- `ECC_SLOT1-32` numbering changed to `ECC_SLOT_0-31`.
- `lt_handshake()` function renamed to `lt_session_start()`.
- Unit tests and platform tests organization was updated.
- Included `lt_l2_api_structs.h` and `lt_l3_api_structs.h` are automatically generated.
- `SH0` pairing keypair in `keys/keys.c` now reflects engineering samples of TROPIC01 devices.
- Changed logging format in examples.
- Renamed cmake switches: `LT_USE_TREZOR_CRYPTO`, LT_BUILD_DOCS.
- Platform examples are removed - now they are standalone repositories.
- Separated libtropic API calls introduced (session_start and ECC_ECDSA functions):
  - lt_l3.* contain interfaces for encoding l3 data going out and decoding l3 data going in,
  - libtropic.* then wraps l3 functions and does the whole l3 exchange in one round,
  - this change allows user to send data through a tunnel between host and target MCU,
    which then behaves as a relay.
- Separated l2 API calls introduced:
  - Now user can use separate function for sending and receiving data.
- Refactoring of `lt_handle_t` struct:
  - contains separate structs for l2 and l3,
  - allows to split all l2 and l3 calls into separate actions.
- Option to define l3 buffer introduced (`LT_SEPARATE_L3_BUFF`).
- Introduced a new folder `keys/` where user might specify SH0-SH3 keys.
- Remove `lt_serial_code_get()`.
- Rename `LT_L2_STARTUP_ID_MAINTENANCE` to `LT_MODE_MAINTENANCE`.
- Rename `LT_L2_STARTUP_ID_REBOOT` to `LT_MODE_APP`.
- Code formatted with `clang-format` version 16 is now enforced on push to branch.
- Add `LT_PRINT_SPI_DATA` - used to debug print low level communication.
- Moved logging and assert macros to `libtropic_logging.h` (they were defined twice in
files `libtropic_examples.h`, `libtropic_functional_tests.h`).
- Instead of using `LT_ADD_FUNC_TESTS` and `LT_BUILD_TESTS`, use only `LT_BUILD_TESTS`.
- Rename `LT_ADD_EXAMPLES` to `LT_BUILD_EXAMPLES` to be consistent with `LT_BUILD_TESTS`
  and `LT_BUILD_DOCS`.
- Always increment host's nonce in `lt_l3_decrypt_response()`.
- When `LT_BUILD_EXAMPLES` or `LT_BUILD_TESTS` are set, CMake requires to define `LT_SH0_PRIV_PATH`.
- Removed SH0 keys and pkey_index_{0,1,2,3} from `keys/keys.c` and reflected it in examples, tests.
- Added writing of pairing keys to slots 1,2,3 in `lt_test_ping` and created `lt_test_engineering_sample_01_ping` from the old version of `lt_test_ping`.
- Set default value to `LT_SH0_PRIV_PATH`.

### Added

- L3 commands: MCounter (init, update and get), MAC-and-destroy.
- L2 commands: Resend, Sleep and Log req.
- libtropic API: lt_update_mode().
- Example folder and test folder is reworked.
- Option LT_ADD_EXAMPLES controls if example's folder code is a part of compilation.
- Option LT_ADD_FUNC_TESTS controls if code from tests/functional/ folder will be compiled-in.
- Port/ support and tests/platform/ example for Raspberry Pi.
- Added macro `LT_USE_ASSERT`, based on which `assert()` is inserted into `LT_ASSERT` and `LT_ASSERT_COND`.
- Added CTest handling for functional tests.
- Added `SESSION_OFF` for indicating that secure session is off and assigned it to `lt_l3_state_t.session` flag in `lt_init()`.
- Added function `lt_l3_invalidate_host_session_data()`, which sets all data in `lt_l3_state_t` to zero and `session` to `SESSION_OFF`; it is called in `lt_session_abort` and when ciphering/deciphering of L3 command fails (wrong TAG).
- Added CMake variable `LT_SH0_PRIV_PATH`, based on which `sh0_keys.c` is generated for examples or tests, so sh0priv and sh0pub are defined.
- Added CTest handling for functional tests run against TROPIC01 model.

### Fixed

- Return values for L3 API
