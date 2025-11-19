# FAQ

This list might help you resolve some issues.

- [I received an error](#i-received-an-error)
- [I cannot estabilish a Secure Session](#i-cannot-estabilish-a-secure-session)

## I received an error
Description of all return values is in the `libtropic_common.h` (`lt_ret_t` enum). However, some errors may have a seemingly unrelated cause, see the next paragraphs.

### `LT_L1_CHIP_BUSY`
Normally, it means that the chip is busy processing some operation and is unable to respond. However, it can also mean that the SPI lines (mainly `MISO`) are tied to ground (and the host receives all zeroes). Check your connections.

The reason is that we detect the status of the TROPIC01 using a single flag and if all data are zero, we are unable to distinguish the situation where the TROPIC01 is truly busy and when the host receives only zeroes.

### `LT_L1_CHIP_ALARM_MODE`
Normally, it means the TROPIC01 entered the alarm mode. However, it can also means, similarly to the situation with `LT_L1_CHIP_BUSY`, that all ones are received on `MISO`. Check your connections.

### `LT_L3_DATA_LEN_ERROR`
This error normally means that L3 packet size we sent to the TROPIC01 is incorrect, which can be caused by a bug or an attack. However, it can also mean that the chip select is connected incorrectly. Check your connections and GPIO assignments. Do not forget we use GPIO to handle chip select, not SPI peripheral's native chip select output.

### `LT_L3_INVALID_CMD` or `LT_L2_UNKNOWN_REQ`
This error means that the TROPIC01 does not know the L3 Command or L2 Request it received. However, the this behavior can be caused by the fact that the TROPIC01 is in the Maintenance Mode. The Maintenance Mode does not implement whole API, mainly it does not implement `Handshake_Req` and none of the L3 Commands, so any handshake attempt will always fail.

TROPIC01 will be in the Maintenance Mode after reboot triggered by the user (`lt_reboot` with `TR01_MAINTENANCE_REBOOT` as `startup_id`). In that case, simply reboot the chip back to the Application Mode (in the code using `lt_reboot` with `TR01_REBOOT` or during evaluation you can use `lt_ex_show_chip_id_and_fwver.c` example, which will reboot to the Application Mode at the end of the example).

However, TROPIC01 can enter Maintenance Mode also automatically in the case of unsuccessful update or in the case where firmware banks are empty or corrupted. In that case, simple reboot will not help and you need to run firmware update again, either using example or in your application code.

## I cannot estabilish a Secure Session
There are two main causes:
1. You use incorrect pairing keys.
    All new TROPIC01s use production pairing keys, which are used by default in Libtropic. However, there are still some preview chips (called engineering samples) on some devkits. For those, you need to use different keys. Refer to the [Default Pairing Keys for a Secure Channel Handshake](../get_started/default_pairing_keys.md).

2. Your TROPIC01 is in the Maintenance Mode.
    Reboot to the Application Mode using `lt_reboot` with `TR01_REBOOT` in your code or during evaluation you can use `lt_ex_show_chip_id_and_fwver.c` example, which will reboot to the Application Mode at the end of the example.
