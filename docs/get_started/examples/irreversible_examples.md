# Irreversible Examples

To provide better orientation, we highlight specific properties of each example, namely:

- Reversibility,
- Model compatibility,
- Level of complexity (🐣 -> 🐤 -> 🐓).

The source code for the examples can be found in the `examples/` directory and also within this document inside "Source Code" boxes.

!!! tip
    You can copy the example code using the :material-content-copy: icon located in the top-right corner of each source code listing.

## lt_ex_hw_wallet.c
- ⚠️ Irreversible
- ✅ Compatible with the model
- 🐓 Level: Advanced

This example demonstrates how to use configuration objects and different pairing keys to manage access to TROPIC01 features. A hardware wallet device scenario is used as a model for this example.

In this example, you will:

- Understand how the R-config is structured and how permissions are managed using the R-config.
- Learn how the R-config can be modified using the libtropic API:
    - `lt_r_config_erase()`: L3 command to erase the R-config.
    - `lt_write_whole_R_config()`: helper function to write the whole R-config with an instance of `struct lt_config_t`.
    - `lt_read_whole_R_config()`: helper function to read the whole R-config into an instance of `struct lt_config_t`.

        !!! tip "Tip: Modifying Only One R-Config Register"
            If you need to modify only one register in the R-config, you can use `lt_r_config_write()` or `lt_r_config_read()`.
            
- Learn how to manage pairing keys:
    - `lt_pairing_key_write()`: L3 command to write a pairing key.
    - `lt_pairing_key_invalidate()`: L3 command to invalidate a pairing key.
- Learn how to work with keys based on elliptic curves (ECC) on TROPIC01:
    - `lt_ecc_key_store()`: L3 command to store an ECC key.
    - `lt_ecc_key_read()`: L3 command to read an ECC key.
    - `lt_ecc_key_generate()`: L3 command to generate an ECC key.
- Learn how to use EDDSA to sign messages and verify signatures.
- Learn how to use a monotonic counter.

??? example "Source code"
    ```c { .copy }
    --8<-- "examples/lt_ex_hw_wallet.c"
    ```

## lt_ex_fw_update.c
- ⚠️ Irreversible
- ❌ Incompatible with the model
- 🐤 Level: Moderate

This example explains the firmware update process for both ABAB and ACAB silicon revisions. Use this example as a reference for integrating TROPIC01 firmware updates into your application. You will learn:

- How to read the current firmware versions.
- How to update the firmware using `lt_do_mutable_fw_update()`.

!!! info "TROPIC01 Firmware"
    For more information about the firmware itself, refer to the [TROPIC01 Firmware](../tropic01_fw.md) section.

!!! warning "Firmware Update Precautions" 
    Use a stable power source and avoid disconnecting the TROPIC01 (devkit) or rebooting your host device (computer or microcontroller) during the update. Interrupting the firmware update can brick the device.

??? example "Source code"
    ```c { .copy }
    --8<-- "examples/lt_ex_fw_update.c"
    ```