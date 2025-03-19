# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Changed

- ECC_SLOT1-32 numbering chabged to ECC_SLOT_0-31
- lt_handshake() function renamed to lt_session_start()
- Unit tests and platform tests organization was updated
- Included lt_l2_api_structs.h and lt_l3_api_structs.h are automatically generated
- SH0 pairing keypair now reflects first batch of TROPIC01 devices
- Changed logging format in examples.
- Renamed cmake switches: LT_USE_TREZOR_CRYPTO, LT_BUILD_DOCS, LT_EXPERIMENTAL_SPI_UART
- Platform examples are removed - now they are standalone repositories

### Added

- L3 commands: MCounter (init, update and get), MAC-and-destroy,
- L2 commands: Resend, Sleep and Log req
- libtropic API: lt_update_mode()
- Example folder and test folder is reworked
- option LT_ADD_EXAMPLES controls if example's folder code is a part of compilation
- port/ support and tests/platform/ example for Raspberry Pi

### Fixed

- Return values for L3 API
