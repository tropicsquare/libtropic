# FAQ

This list might help you resolve some issues.

- [FAQ](#faq)
  - [I received an error](#i-received-an-error)
    - [`LT_L1_CHIP_BUSY`](#lt_l1_chip_busy)
    - [`LT_L1_CHIP_ALARM_MODE`](#lt_l1_chip_alarm_mode)
    - [`LT_L2_HSK_ERR`](#lt_l2_hsk_err)
    - [`LT_L3_DATA_LEN_ERROR`](#lt_l3_data_len_error)
    - [`LT_L3_INVALID_CMD` or `LT_L2_UNKNOWN_REQ`](#lt_l3_invalid_cmd-or-lt_l2_unknown_req)
    - [`LT_HAL_ERROR`](#lt_hal_error)
    - [`LT_L2_CRC_ERR` or `LT_L2_IN_CRC_ERR`](#lt_l2_crc_err-or-lt_l2_in_crc_err)
  - [I cannot establish a Secure Session](#i-cannot-establish-a-secure-session)
  - [FW update failed](#fw-update-failed)
  - [What is the part number (P/N) of my TROPIC01?](#what-is-the-part-number-pn-of-my-tropic01)
  - [What is the silicon revision of my TROPIC01?](#what-is-the-silicon-revision-of-my-tropic01)
  - [What FW versions is my TROPIC01 running?](#what-fw-versions-is-my-tropic01-running)

## I received an error
Description of all return values is in the `libtropic_common.h` (`lt_ret_t` enum). However, some errors may have a seemingly unrelated cause; see the following paragraphs.

### `LT_L1_CHIP_BUSY`
Normally, this means that the chip is busy processing an operation and is unable to respond. However, it can also mean that the SPI lines (mainly `MISO`) are tied to ground (causing the host to receive all zeroes). Check your connections.

The reason is that we detect the status of the TROPIC01 using a single flag; if all data are zero, we cannot distinguish between the TROPIC01 being truly busy and the host receiving only zeroes.

### `LT_L1_CHIP_ALARM_MODE`
Normally, this means the TROPIC01 has entered the Alarm Mode. The alarm can be caused by a variety of factors:

- sensors detected an attack,
- TROPIC01 is operating outside of the standard range (voltage, temperature...),
- incorrect or corrupted firmware update data was used,
- hardware failure,
- and more.

Refer to the TROPIC01 Datasheet for more information about the Alarm Mode.

However, `LT_L1_CHIP_ALARM_MODE` can also be returned if all bits received on `MISO` are `1` (`0xFF`...). This happens due to how the Alarm Mode flag is defined, similarly to `LT_L1_CHIP_BUSY`. In that case, check connections between the host and TROPIC01.

### `LT_L2_HSK_ERR`
This error is caused by a problem during Secure Session establishment. See [I cannot establish a Secure Session](#i-cannot-establish-a-secure-session).

### `LT_L3_DATA_LEN_ERROR`
This error normally means that the L3 packet size we sent to the TROPIC01 is incorrect, which can be caused by a bug or an attack. However, it can also mean that the chip select is connected incorrectly. Check your connections and GPIO assignments.

!!! warning "Chip Select Handling"
    We use a GPIO to handle chip select, not the SPI peripheral's native chip select output.

### `LT_L3_INVALID_CMD` or `LT_L2_UNKNOWN_REQ`
This error means that the TROPIC01 does not recognize the L3 command or L2 request it received. However, this behavior can be caused by the TROPIC01 being in Maintenance Mode. Maintenance Mode does not implement the entire API — it does not implement `Handshake_Req` nor any L3 commands, so handshake attempts will always fail.

A TROPIC01 will be in Maintenance Mode after a user-triggered reboot (calling `lt_reboot` with `TR01_MAINTENANCE_REBOOT` as `startup_id`). In that case, reboot the chip back to Application Mode by calling `lt_reboot` with `TR01_REBOOT`.

However, a TROPIC01 can also enter Maintenance Mode automatically after an unsuccessful update or if firmware banks are empty or corrupted. In that case, a simple reboot will not help; you must run the firmware update again, either using the firmware update example (see [Tutorials](./tutorials/index.md)) or from your application code.

### `LT_HAL_ERROR`
This is an error code returned by HAL and can be caused by various issues. See below for the most common causes.

#### Permissions issue on Linux

On Linux, a common cause is an error in opening the device due to insufficient permissions. If you run any example and receive `LT_HAL_ERROR`, enable debug logging (see [Logging](reference/logging.md)). If you see an error similar to the following:

```sh
ERROR   [ 145] Error opening serial at "/dev/ttyACM0", errno=13 (Permission denied)
```

This is likely an issue with permissions. Try adding your user account to the groups that provide access to the device you are using (for example, `dialout` or `plugdev` for USB serial devices, and `spi` for SPI devices). Refer to documentation of your distribution for details.

### `LT_L2_CRC_ERR` or `LT_L2_IN_CRC_ERR`
This error means that either TROPIC01 received a corrupted frame (`LT_L2_CRC_ERR`) or the host received a corrupted frame (`LT_L2_IN_CRC_ERR`).

Libtropic will try to retry the communication by default and return the error only after all retry attempts are depleted.

#### Diagnostics

To diagnose CRC-related errors, we provide two counters in `lt_l2_state_t`: 

- `l2_in_crc_error_count` tracks number of corrupted frames received by the host.
- `l2_crc_error_count` tracks number of corrupted frames received by the TROPIC01.

These counters are reset automatically on Libtropic initialization (in `lt_init`).

!!! important "Counters and separate API"
    If you use separate API, you need to use `lt_l2_transfer` for counters to be updated automatically, not `lt_l2_send` and `lt_l2_receive`.

#### Possible causes and solutions

CRC errors hint that the connection is unstable. You can try the following:

- Use high-quality wiring and firm connectors. Poor DuPont (jump) wires combined with cheap breadboards are common causes of connection issues.
- Minimize the length of the connections as much as possible.
- Try to lower the communication speed.
- Increase the number of retries: see [`LT_CRC_ERR_RETRY_ATTEMPTS` parameter](reference/integrating_libtropic/how_to_configure/index.md#lt_crc_err_retry_attempts). This is not recommended, it is always better to find the cause of the connection instability.

## I cannot establish a Secure Session
There are two main causes:

1. You are using incorrect pairing keys.
   All new TROPIC01s use production pairing keys, which are used by default in Libtropic. Some devkits still contain preview chips (engineering samples). For those, you need to use different keys. Refer to the [Default Pairing Keys for a Secure Channel Handshake](reference/default_pairing_keys.md).

2. Your TROPIC01 is in Maintenance Mode.
   Reboot to Application Mode by calling `lt_reboot` with `TR01_REBOOT`.

## FW update failed
If one of our firmware update API functions or the `lt_do_mutable_fw_update()` helper function failed:

1. Try the suggestions in [I received an error](#i-received-an-error).
2. Make sure you have correct values set in the following CMake options:
    - [LT_SILICON_REV_ABAB](reference/integrating_libtropic/how_to_configure/index.md#lt_silicon_rev_abab),
    - [LT_CPU_FW_UPDATE_DATA_VER](reference/integrating_libtropic/how_to_configure/index.md#lt_cpu_fw_update_data_ver).
3. Make sure you are not attempting a firmware downgrade — TROPIC01 does not allow this.

## What is the part number (P/N) of my TROPIC01?
You have two options:

1. Read it from the packaging you received your TROPIC01 product in.
2. Run our example Identify Chip (see [Tutorials](tutorials/index.md)), which **does not** require the Secure Channel Session.

## What is the silicon revision of my TROPIC01?
You have two options:

1. Read the product number (P/N) from the packaging you received your TROPIC01 product in. After that, refer to the [catalog list](https://github.com/tropicsquare/tropic01/blob/main/doc/pages/part-numbers.md#catalog-list) in the TROPIC01 GitHub repository, which will help you decode the silicon revision based on your P/N.
2. Run our example Identify Chip (see [Tutorials](tutorials/index.md)), which **does not** require the Secure Channel Session.
    - If the silicon revision field says "N/A", silicon revision of the TROPIC01 is **ABAB**. This is because CHIP_ID v0.0.0.1, used in ABAB chips, does not include the silicon revision field.

## What FW versions is my TROPIC01 running?
Run our example Identify Chip (see [Tutorials](tutorials/index.md)), which **does not** require the Secure Channel Session.