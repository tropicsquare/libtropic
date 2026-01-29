# TROPIC01 Firmware
TROPIC01 contains the following FW execution engines:

- **RISC-V CPU**,
- **ECC engine** or **SPECT** (these two terms are used interchangeably).

There are multiple kinds of FW running in TROPIC01:

1. *Immutable FW (bootloader)*. Located in ROM, runs on RISC-V CPU from ROM after power-up, updates or boots the mutable FWs.
2. *RISC-V Mutable FW (CPU FW)*. Updatable, located in R-memory, runs on RISC-V CPU from RAM, processes L2/L3 communication.
3. *ECC engine mutable FW (ECC engine FW or SPECT FW)*. Updatable, located in R-memory, runs on ECC engine from RAM, helps the RISC-V CPU FW with processing ECC commands (ECC_Key_*, ECDSA/EDDSA_Sign).

!!! info "More Information About TROPIC01 Firmware"
    For more detailed information about each FW, refer to the [FW Update Application Note](https://github.com/tropicsquare/tropic01?tab=readme-ov-file#application-notes).

## TROPIC01 Firmware in Libtropic
Libtropic provides not only implementation of the FW update L2 commands, but also the necessary files for updating both the RISC-V and SPECT FW. Refer to:

1. [Firmware Update Files](#firmware-update-files) section for more information about the `TROPIC01_fw_update_files/` directory.
2. [Tutorials](../tutorials/index.md), where we demonstrate the firmware update feature. Have a look at, for example, [Firmware Update on TROPIC01 USB Devkit on Linux](../tutorials/linux/usb_devkit/fw_update.md).

### Firmware Update Files
The `TROPIC01_fw_update_files/` directory provides TROPIC01 FW update files in two formats:

1. *C header files (`*.h`)*. These are designed to be included and compiled directly into the Host MCU's firmware/application. See [Compiling into Libtropic](#compiling-into-libtropic) section for more information.
2. *Binary files (`*.bin`)*. These can be stored in the Host MCU's filesystem or external storage, loaded at runtime and used to update TROPIC01's FW.

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
To select which FW version will be compiled together with Libtropic, the user has to set the following CMake variables (both have a default value):

- [LT_SILICON_REV](integrating_libtropic/how_to_configure/index.md#lt_silicon_rev),
- [LT_CPU_FW_UPDATE_DATA_VER](integrating_libtropic/how_to_configure/index.md#lt_cpu_fw_update_data_ver).

## Firmware Hashes
TROPIC01 is able to report hashes of the firmware it is loaded with. Using Libtropic, you can get the value using `lt_get_info_fw_bank` function.

However, for certain old firmware versions, the reported hashes will not match with the hashes found in the public firmware repositories. The reason is that before publication, we cleaned up the git histories. Although the code was not changed, git hashes were affected. Affected version are:

- RISC-V CPU FW: versions older than and including v1.0.1.
- SPECT FW: versions older than and including v1.0.0.

If you want to verify that production binaries match the source code, you can compile the source code and then compare the resulting binary to production binaries provided in the Libtropic repository.