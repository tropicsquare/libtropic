# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Changed

- ECC_SLOT1-32 numbering chabged to ECC_SLOT_0-31
- lt_handshake() function renamed to lt_start_session()
- TROPIC01 hw wallet example updated - compiles for 32 and 64 bit system
- Unit tests organization was updated
- Included lt_l2_api_structs.h and lt_l3_api_structs.h are automatically generated

### Added

- L3 commands: MCounter init, update and get
- L2 commands: Resend, Sleep and Log req

### Fixed

- Return values for L3 API
