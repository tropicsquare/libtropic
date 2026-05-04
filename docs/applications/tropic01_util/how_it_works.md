# How It Works
This section provide more detailed information about the TROPIC01 Util ecosystem — the new TROPIC01 USB DevKit firmware and the Python CLI application.

## Old USB DevKit Firmware
Before diving into the new firmware, let's see how the old firmware works to get an idea what has changed:
<figure style="text-align: center;">
<img src="../../../img/usb_devkit_old_fw.svg" alt="Old USB DevKit Firmware" width="900"/>
<figcaption style="font-size: 0.9em; color: #555; margin-top: 0.5em; margin-bottom: -1em;">
    Old USB DevKit Firmware
  </figcaption>
</figure>

- The USB DevKit accepts ASCII (human-readable) commands and returns ASCII responses. The commands are not wrapped in any frame protocol except the USB-CDC transfer protocol.
- The USB DevKit acts only as a USB-to-SPI master converter (STM32U5 talks to the TROPIC01 slave SPI interface) or as an executor of low-level operations (e.g. changing the SPI frequency or reading TROPIC01 GPO pin).

## New USB DevKit Firmware
The new firmware was developed to leverage the rich hardware of the STM32U5 microcontroller (used in Trezor HW wallets, among others), which offers up to 512 KiB of Flash memory and extensive cryptographic and security features. While the current version may not yet utilize every capability, it lays the groundwork for future expansion.

<figure style="text-align: center;">
<img src="../../../img/usb_devkit_new_fw.svg" alt="New USB DevKit Firmware" width="900"/>
<figcaption style="font-size: 0.9em; color: #555; margin-top: 0.5em; margin-bottom: -1em;">
    New USB DevKit Firmware
  </figcaption>
</figure>

- The USB DevKit implements a command-response protocol — it accepts either **raw commands** (*RawCmd*) or **application commands** (*AppCmd*), and returns **raw** (*RawResp*), **application** (*AppResp*), or **error** responses (*ErrorResp*). Both types of commands use binary format rather than ASCII.
    - Error responses (omitted from the image for the sake of clarity) are returned when some error occures before the USB DevKit can know which command (raw or application) it received.
- **Raw commands** are designed to provide the same capabilities as the old firmware did.
- **Application commands** are issued through the Python CLI (TROPIC01 Util) and allow us to perform more complex operations:
    - Libtropic with STM32U5xx HAL is built into the STM32U5 firmware, exposing the entire Libtropic API for communication with TROPIC01.
    - Using the STM32Cube HAL, the STM32U5's hardware can be interacted with.

### Frame Protocol
The frames exchanged between the Host PC and the USB DevKit have the following format:
<figure style="text-align: center;">
<img src="../../../img/usb_devkit_new_fw_frame_protocol.svg" alt="New USB DevKit Firmware Frames" width="500"/>
<figcaption style="font-size: 0.9em; color: #555; margin-top: 0.5em; margin-bottom: -1em;">
    New USB DevKit Firmware Frames
  </figcaption>
</figure>

- **Magic bytes** are used to indicate where the frame starts. Their order depends on the direction of the frame (**from** or **to** the Host PC) to easily separate them when inspecting the raw communication.
- **DATA_LEN** field contains the length of the **DATA** field.
- **DATA** field contains *RawCmd*/*AppCmd* (Host PC -> USB DevKit) or *RawResp*/*AppResp*/*ErrorResp* (USB DevKit -> Host PC). All of these are encoded using Google's [Protocol Buffers](https://protobuf.dev/) message format.
    - Commands are wrapped as *UsbDevkitCmd* messages and responses as *UsbDevkitResp* messages. That way, a command ID field, that would tell e.g. if the incoming message is *AppCmd* or *RawCmd*, is not needed — this information is contained in the Protocol Buffers payload using the [oneof](https://protobuf.dev/programming-guides/proto3/#oneof) feature.
- **CRC16** field includes both **DATA_LEN** and **DATA** fields.

!!! question "Why Protocol Buffers?"
    Google's [Protocol Buffers](https://protobuf.dev/) is a widely known and used format for serializing structured data. All messages are described in one place (a `.proto` file) using a high-level language. Protocol Buffers come with a `protobuf-compiler` that generates source code in most known programming languages for encoding and decoding the messages. This allows us to quickly add new commands.

## TROPIC01 Util
TROPIC01 Util is a Python CLI user application used to issue application commands to the USB DevKit. Raw commands are not supported, because they currently have no added value in the application context — they are primarily used by the Libtropic USB DevKit HAL.

TROPIC01 Util has its own commands like `pin-set`, `pin-verify` etc. These CLI commands can directly issue one application command to the USB DevKit, but can also issue multiple different application commands, along with other actions on the Host PC side. In other words, one CLI command (invoked by the user) can do many other things besides sending the application commands.

See [Use TROPIC01 Util](install_and_use.md#use-tropic01-util) for more information about how TROPIC01 Util is used.