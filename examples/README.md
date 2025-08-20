# Examples
This directory contains multiple examples demonstrating how to use libtropic. Some examples may cause irreversible changes to the chip, so they are organized into two categories:

## Reversible examples

List of reversible examples.

#### lt_ex_hello_world.c

A basic example to verify chip functionality and establish a secured session.

#### lt_ex_hello_world_separate_API.c

Functionally similar to `lt_ex_hello_world.c`, but uses distinct API calls for incoming and outgoing data. This approach is useful for secure, tunneled communication, used for example during chip provisioning in a factory.

#### lt_ex_show_chip_id_and_fw_ver.c

Demonstrates how to read and display the chip’s unique ID and firmware version information (bootloader, application and SPECT firmware versions).

#### lt_ex_macandd.c

Illustrates the MAC-and-destroy feature. Refer to the application note and code comments for details.


## WARNING: Irreversible examples

List of irreversible examples.

#### lt_ex_hw_wallet.c

Shows how to use configuration objects and different pairing keys to manage access to TROPIC01 features. We used a hardware wallet device scenario as a model for this example.

#### lt_ex_fw_update.c

Explains the firmware update process for both ABAB and ACAB silicon revisions. Use this example as a reference for integrating TROPIC01 firmware updates into your application.