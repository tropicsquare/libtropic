This Example demonstrates how to read and display the chip’s unique ID and firmware version information (bootloader, application and SPECT firmware versions). You will learn about following functions:

- `lt_reboot()`: L2 request to reboot to either Application or Maintenance Mode,
- `lt_get_info_riscv_fw_ver()`, `lt_get_info_spect_fw_ver()`: L2 requests to read RISC-V CPU and SPECT firmware versions,
- `lt_get_info_chip_id()`: L2 request to read chip identification (e.g., serial number).