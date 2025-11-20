# FAQ

This list might help you resolve some issues.

- [I received an error](#i-received-an-error)
- [I cannot establish a Secure Session](#i-cannot-establish-a-secure-session)

## I received an error
Description of all return values is in the `libtropic_common.h` (`lt_ret_t` enum). However, some errors may have a seemingly unrelated cause; see the following paragraphs.

### `LT_L1_CHIP_BUSY`
Normally, this means that the chip is busy processing an operation and is unable to respond. However, it can also mean that the SPI lines (mainly `MISO`) are tied to ground (causing the host to receive all zeroes). Check your connections.

The reason is that we detect the status of the TROPIC01 using a single flag; if all data are zero, we cannot distinguish between the TROPIC01 being truly busy and the host receiving only zeroes.

### `LT_L1_CHIP_ALARM_MODE`
Normally, this means the TROPIC01 entered Alarm Mode. However, it can also mean — similarly to `LT_L1_CHIP_BUSY` — that all ones are received on `MISO`. Check your connections.

### `LT_L3_DATA_LEN_ERROR`
This error normally means that the L3 packet size we sent to the TROPIC01 is incorrect, which can be caused by a bug or an attack. However, it can also mean that the chip select is connected incorrectly. Check your connections and GPIO assignments.

!!! warning "Chip Select Handling"
    We use a GPIO to handle chip select, not the SPI peripheral's native chip select output.

### `LT_L3_INVALID_CMD` or `LT_L2_UNKNOWN_REQ`
This error means that the TROPIC01 does not recognize the L3 command or L2 request it received. However, this behavior can be caused by the TROPIC01 being in Maintenance Mode. Maintenance Mode does not implement the entire API — it does not implement `Handshake_Req` nor any L3 commands, so handshake attempts will always fail.

A TROPIC01 will be in Maintenance Mode after a user-triggered reboot (calling `lt_reboot` with `TR01_MAINTENANCE_REBOOT` as `startup_id`). In that case, reboot the chip back to Application Mode by calling `lt_reboot` with `TR01_REBOOT`. During evaluation you can also use the [lt_ex_show_chip_id_and_fwver.c](get_started/examples/reversible_examples.md#lt_ex_show_chip_id_and_fwverc) example, which reboots to Application Mode at the end.

However, a TROPIC01 can also enter Maintenance Mode automatically after an unsuccessful update or if firmware banks are empty or corrupted. In that case, a simple reboot will not help; you must run the firmware update again, either using the [lt_ex_fw_update.c](get_started/examples/irreversible_examples.md#lt_ex_fw_updatec) example or from your application code.

## I cannot establish a Secure Session
There are two main causes:

1. You are using incorrect pairing keys.
   All new TROPIC01s use production pairing keys, which are used by default in Libtropic. Some devkits still contain preview chips (engineering samples). For those, you need to use different keys. Refer to the [Default Pairing Keys for a Secure Channel Handshake](get_started/default_pairing_keys.md).

2. Your TROPIC01 is in Maintenance Mode.
   Reboot to Application Mode by calling `lt_reboot` with `TR01_REBOOT`, or during evaluation use the [lt_ex_show_chip_id_and_fwver.c](get_started/examples/reversible_examples.md#lt_ex_show_chip_id_and_fwverc) example, which reboots to Application Mode at the end.
