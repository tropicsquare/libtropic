# TROPIC01 Firmware
TROPIC01 contains the following FW execution engines:

- **RISC-V CPU**,
- **ECC engine** or **SPECT** (these two terms are used interchangeably).

There are multiple kinds of FW running in TROPIC01:

1. *Immutable FW (bootloader)*. Located in ROM, runs on RISC-V CPU from ROM after power-up, updates or boots the mutable FWs.
2. *RISC-V Mutable FW (CPU FW)*. Updatable, located in R-memory, runs on RISC-V CPU from RAM, processes L2/L3 communication.
3. *ECC engine mutable FW (ECC engine FW)*. Updatable, located in R-memory, runs on ECC engine from RAM, helps the RISC-V CPU FW with processing ECC commands (ECC_Key_*, ECDSA/EDDSA_Sign).

!!! tip
    For more detailed information about each FW, refer to the [FW Update Application Note](https://github.com/tropicsquare/tropic01?tab=readme-ov-file#application-notes).

## TROPIC01 Firmware in Libtropic
Libtropic provides not only implementation of the FW update L2 commands, but also the necessary files for updating both the RISC-V and SPECT FW. Refer to:

1. [Firmware Update Files](#firmware-update-files) section for more information about the `TROPIC01_fw_update_files/` directory.
2. [lt_ex_fw_update.c](./examples/irreversible_examples.md#lt_ex_fw_updatec) section (in the [Irreversible Examples](./examples/irreversible_examples.md) documentation page) for a practical example, showing how to update TROPIC01's FW using Libtropic.

### Firmware Update Files
The `TROPIC01_fw_update_files/` directory provides TROPIC01 FW update files in two formats:

1. *C header files (`*.h`)*. These are designed to be included and compiled directly into the Host MCU's firmware/application. See [Compiling into Libtropic](#compiling-into-libtropic) section for more information.
2. *Binary files (`*.bin`)*. These can be stored in the Host MCU, loaded at runtime and used to update TROPIC01's FW.

The general structure of the `TROPIC01_fw_update_files/` directory is the following:
```text
TROPIC01_fw_update_files/
├── boot_v_<X_Y_Z>/
│   └── fw_v_<A_B_C>/
│       ├── fw_CPU.h
│       ├── fw_SPECT.h
│       ├── fw_v<A_B_C>.hex32_signed_chunks.bin
│       └── spect_app-v<D_E_F>_signed_chunks.bin
└── convert.py
```

- `boot_v_<X_Y_Z>/`: directories of available FW update files for a given bootloader version `<X_Y_Z>`.
- `fw_v_<A_B_C>/`: directory with RISC-V CPU and SPECT FW update files (in both formats) for a given FW version `<A_B_C>`. Note that the RISC-V CPU FW and SPECT FW versions can be different.
- `convert.py`: Python script for converting firmware binary files into C header files.

#### Compiling into Libtropic
To select which FW version will be compiled together with Libtropic, the user has to set the following CMake options (both have a default value):

1. `LT_SILICON_REV`: Defines the TROPIC01 silicon revision (e.g. `"ACAB"`), based on which the correct bootloader version is selected. Refer to the [Available Parts](https://github.com/tropicsquare/tropic01?tab=readme-ov-file#available-parts) section (in the [TROPIC01 GitHub repository](https://github.com/tropicsquare/tropic01)) to find out the silicon revision of your TROPIC01 chip.
2. `LT_CPU_FW_VERSION`: Defines the TROPIC01 FW version (e.g. `"1_0_1"`), based on which the correct FW update files for both RISC-V CPU and SPECT are selected.