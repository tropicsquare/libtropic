This example explains the firmware update process for the ABAB silicon revision as well as ACAB and newer (e.g. BDBB). Use this example as a reference for integrating TROPIC01 firmware updates into your application. You will learn:

- How to read the current firmware versions.
- How to update the firmware using `lt_do_mutable_fw_update()`, which performs the recommended FW update procedure.
- How to handle TROPIC01 Maintenance Mode to **reduce the attack surface**.

!!! info "TROPIC01 Firmware"
    For more information about the firmware itself, refer to the [TROPIC01 Firmware](/reference/tropic01_fw.md) section.

!!! warning "Firmware Update Precautions" 
    Use a stable power source and avoid disconnecting the TROPIC01 (devkit) or rebooting your host device (computer or microcontroller) during the update. Interrupting the firmware update can brick the device.