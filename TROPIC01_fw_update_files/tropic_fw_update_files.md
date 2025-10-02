# TROPIC01 Firmware Update Files

We provide TROPIC01 firmware update files for two main types of devices:
- Devices with bare-metal firmware (no operating system).
- Devices running an operating system (OS).

To accommodate these different environments, we provide firmware update files in two formats.

For **bare-metal devices**, we provide C header files (`.h`). These are designed to be included and compiled directly into the device's firmware. The TROPIC01 firmware update then becomes an integral part of the device's own firmware image.

For **devices running an OS**, which typically have a filesystem, we provide the firmware update as binary files (`.bin`). These can be stored on the device and loaded at runtime to perform the update.

### Example of firmware update procedure

- **Header files (`.h`):** `examples/lt_ex_fw_update.c`
- **Binary files (`.bin`):** `TBD`
