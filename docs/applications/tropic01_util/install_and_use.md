# Install and Use
This section provides instructions for installing dependencies, building and flashing the new TROPIC01 USB DevKit firmware, and using the TROPIC01 Util Python CLI application.

!!! warning "Before you start"
    Before you start with the instructions below, please, complete our [USB DevKit tutorials](../../tutorials/linux/usb_devkit/index.md), specifically:
    
    1. [Hardware](../../tutorials/linux/usb_devkit/index.md#hardware-setup) and [software](../../tutorials/linux/usb_devkit/index.md#software-setup) setup. This will help you prepare Libtropic and the USB DevKit on your machine.
    1. [Chip identification](../../tutorials/linux/usb_devkit/identify_chip.md), [FW Update](../../tutorials/linux/usb_devkit/fw_update.md) and [Hello, World!](../../tutorials/linux/usb_devkit/hello_world.md) tutorials. These will help you verify that you can use the USB DevKit on your machine without any issues and you will update TROPIC01 to the latest firmware.

## Install Dependencies
!!! example "Installation instructions"
    === ":fontawesome-brands-linux: Linux"
        1. Switch to `tropic01_util/` directory:
            - `cd libtropic/examples/applications/tropic01_util/`
        2. Install GCC cross-compiler for ARM:
            - Ubuntu/Debian: `sudo apt update && sudo apt install gcc-arm-none-eabi`
            - Fedora: `sudo dnf install arm-none-eabi-gcc arm-none-eabi-binutils-cs arm-none-eabi-newlib`
        3. Install `dfu-util`:
            - Ubuntu/Debian: `sudo apt update && sudo apt install dfu-util`
            - Fedora: `sudo dnf install dfu-util`
        4. [Install Python](https://www.python.org/downloads/):
            - You can also use your distribution's package manager.
                - Fedora: `sudo dnf install python3`
                - Debian/Ubuntu: `sudo apt update && sudo apt install python3`
            !!! warning "Python version"
                At the time this guide was written, Python 3.14 did not work with our required packages. This might have been already solved, but if not, version 3.13 should work. Older versions were not tested.
        5. Prepare Python Virtual Environment (venv):
            - Create venv: `python3 -m venv tropic01-util-venv`
            - Activate venv: `source tropic01-util-venv/bin/activate`
            - Update pip: `pip install --upgrade pip`
            - Install dependencies: `pip install -r tropic01_util_app/requirements.txt`

    === ":fontawesome-brands-apple: macOS"
        TBA

    === ":fontawesome-brands-windows: Windows"
        TBA

## Build USB DevKit Firmware
First, build the new USB DevKit firmware:
!!! example "Build instructions"
    === ":fontawesome-brands-linux: Linux"
        Switch to the firmware directory:
        ```bash { .copy }
        cd libtropic/examples/applications/tropic01_util/firmware/
        ```

        Create a `build/` directory and switch to it:
        ```bash { .copy }
        mkdir build/
        cd build/
        ```

        And finally, build the firmware. If the revision of your USB DevKit is TS1303, build as:
        ```bash { .copy }
        cmake ..
        make -j
        ```
        However, if the revision of your USB DevKit is TS1302, build as:
        ```bash { .copy }
        cmake -DUSB_DEVKIT_REVISION=TS1302 ..
        make -j
        ```

        !!! question "What is the revision of my USB DevKit?"
            The revision is written on the back side of the PCB.

    === ":fontawesome-brands-apple: macOS"
        TBA

    === ":fontawesome-brands-windows: Windows"
        TBA

## Flash USB DevKit Firmware
Next, flash the new USB DevKit firmware:
!!! example "Flash instructions"
    Before flashing the firmware, the STM32U5 on the USB DevKit must be booted into bootloader with these steps:

    1. press the button on the USB DevKit and hold it,
    2. connect it to USB,
    3. release the button.

    After this, the LED should stay off and the USB DevKit should be registered in your system as "STMicroelectronics STM Device in DFU Mode".
    
    Follow the next steps to flash the firmware.

    === ":fontawesome-brands-linux: Linux"
        Switch to the `build/` directory where you have previously built the firmware:
        ```bash { .copy }
        cd libtropic/examples/applications/tropic01_util/firmware/build/
        ```

        flash the firmware:
        ```bash { .copy }
        dfu-util -a 0 -s 0x08000000:leave -D tropic01_util.bin
        ```

        Now, the USB DevKit LED should turn on and it should be registered in your system as "STMicroelectronics Virtual COM Port".

    === ":fontawesome-brands-apple: macOS"
        TBA

    === ":fontawesome-brands-windows: Windows"
        TBA

    !!! tip "Flashing back the original firmware"
        If you want to flash back the firmware shipped with the USB DevKit, clone our [devboards](https://github.com/tropicsquare/devboards) repository and follow [these instructions](https://github.com/tropicsquare/devboards/tree/main/TROPIC01_USB_Devkit/firmwares/firmware_B#updating-devkits-firmware).

## Use TROPIC01 Util
As previously mentioned, TROPIC01 Util is a Python CLI application used to interact with our USB DevKit running the new firmware. To use it, follow these instructions:

!!! example "Using TROPIC01 Util"
    === ":fontawesome-brands-linux: Linux"
        Switch to `tropic01_util/` directory:
        ```bash { .copy }
        cd libtropic/examples/applications/tropic01_util/
        ```

        Make sure the Python `venv` we have created before is still active. If not, activate it:
        ```bash { .copy }
        source tropic01-util-venv/bin/activate
        ```

        After that, run the `tropic01_util.py` script:
        ```bash { .copy }
        ./tropic01_util.py -h
        ```
        You should see a help menu with available **options** and **commands**. The **options** are used for e.g. setting a read timeout or a port where your USB DevKit is mapped. These have default values, so you do not always have to set them. The **commands** are used for executing operations on the USB DevKit. To see how to use a specific command, e.g. `get-rand-bytes`, run:
        ```bash { .copy }
        ./tropic01_util.py get-rand-bytes -h
        ```
        For example, to get 500 random bytes, run:
        ```bash { .copy }
        ./tropic01_util.py get-rand-bytes --count 500
        ```

    === ":fontawesome-brands-apple: macOS"
        TBA

    === ":fontawesome-brands-windows: Windows"
        TBA

!!! info "Secure Session establishment"
    There is no command for establishing a secure session. When the USB DevKit is connected to USB, during startup, it establishes a Secure Session with TROPIC01 using the default `prod0` keys in Pairing Key slot 0. For more information about the default pairing keys, refer to [Default Pairing Keys for a Secure Channel Handshake](../../reference/default_pairing_keys.md).
    
    Because the usage of `prod0` keys is hardcoded into the USB DevKit firmware, there is no possibility to use other pairing keys without modifying the firmare and flashing it into the USB DevKit.