# 3. Hello, World! Example Tutorial

--8<-- "docs/common/examples_descriptions/hello_world.md"

## Build
!!! example "Building the example"
    === ":fontawesome-brands-linux: Linux"
        Go to the example's project directory:
        ```bash { .copy }
        cd examples/rpi-pico/<your_board>/hello_world/
        ```

        Create a `build/` directory and switch to it:
        ```bash { .copy }
        mkdir build/
        cd build/
        ```

        Build the example:
        ```bash { .copy }
        cmake ..
        make
        ```
    === ":fontawesome-brands-apple: macOS"
        TBA

    === ":fontawesome-brands-windows: Windows"
        TBA

## Flash
There are two ways to flash firmware to a Pico board: with and without `picotool`. We recommend using the `picotool` we installed previously.

!!! question "When can `picotool` be used?"
    [picotool](https://github.com/raspberrypi/picotool) is a tool used to interact with RP2040/RP2350 devices in BOOTSEL mode, but it is also possible to interact with devices not in BOOTSEL mode. However, the running firmware must use the USB stdio from [Pico SDK](https://github.com/raspberrypi/pico-sdk) and keep the USB stack alive. Our examples are written in a way that `picotool` can be used even if the device is not in BOOTSEL mode, but this might not be the case for your current firmware. We address all possible cases below.

!!! warning "DTR signal support"
    The example program will wait until a serial monitor is connected, i.e. after the DTR signal is asserted — make sure your serial monitor asserts it! The program waits for the DTR signal so no output is lost before you connect to the serial (the serial port is created only after flashing the program).

### A. With `picotool`
!!! example "Flashing the example with `picotool`"
    === ":fontawesome-brands-linux: Linux"
        Make sure `picotool` can communicate with the board:
        ```bash { .copy }
        picotool info
        ```
        If you see an error, reboot into **BOOTSEL** manually:
        ??? tip "Rebooting into BOOTSEL mode manually"
            1. Unplug the board.
            2. Hold the **BOOTSEL** button.
            3. Plug USB back in while still holding the **BOOTSEL** button.
            4. Release the **BOOTSEL** button.
            5. Execute `lsusb`. The board should be listed as *Raspberry Pi RP2 Boot*, *Raspberry Pi RP2350 Boot* or similar (the *Boot* is important).

        After this, the output of `picotool info` should not contain errors. If there are still errors, follow the [flashing instructions without picotool](#b-without-picotool).

        Flash the example:
        ```bash { .copy }
        picotool load libtropic_hello_world.elf -x -f
        ```

        Open your Pico's serial port using your preferred serial monitor, with configuration 8-n-1 and a baud rate of 115200. By default, the port is mapped to `/dev/ttyACM0`. For example, using GTKTerm:
        ```bash { .copy }
        gtkterm -p /dev/ttyACM0 -s 115200
        ```

        After this, you should see output in your serial terminal.
    === ":fontawesome-brands-apple: macOS"
        TBA

    === ":fontawesome-brands-windows: Windows"
        TBA

### B. Without `picotool`
!!! example "Flashing the example without `picotool`"
    === ":fontawesome-brands-linux: Linux"
        Reboot into **BOOTSEL** mode:
        ??? tip "Rebooting into BOOTSEL mode manually"
            1. Unplug the board.
            2. Hold the **BOOTSEL** button.
            3. Plug USB back in while still holding the **BOOTSEL** button.
            4. Release the **BOOTSEL** button.
            5. Execute `lsusb`. The board should be listed as *Raspberry Pi RP2 Boot*, *Raspberry Pi RP2350 Boot* or similar (the *Boot* is important).

        When the board is in **BOOTSEL** mode, it should appear as a mass-storage `vfat` device with labels like `RP2040`, `RP2350` or `RPI-RP2`; you can check it with:
        ```bash { .copy }
        lsblk -f  # look into the LABEL column
        ```
        The board should also be automatically mounted in `/media/<your_username>/`, but this depends on your system settings. Use the instructions below to mount it manually if needed:
        ??? tip "Mounting the Pico board manually"
            1. Get device node of the board from the NAME column (e.g. `/dev/sda1`):
            ```bash { .copy }
            lsblk -f  # look into the NAME column
            ```
            2. Mount it e.g. into your current directory (should still be `build/`):
            ```bash { .copy }
            mkdir -p pico_mnt/
            sudo mount /dev/sd<xy> pico_mnt/  # <xy> is part of the found device node above
            ```
            3. Verify you can read from the mounted device (you should see files `INDEX.HTM` and `INFO_UF2.TXT` or similar):
            ```bash { .copy }
            ls pico_mnt/
            ```

        Copy the previously built `uf2` file to the mounted device (`<mount_location>` is either `/media/<your_username>/<RP*>/` or `pico_mnt/` if mounted manually):
        ```bash { .copy }
        cp libtropic_hello_world.uf2 <mount_location>
        ```
        Open your Pico's serial port using your preferred serial monitor, with configuration 8-n-1 and a baud rate of 115200. By default, the port is mapped to `/dev/ttyACM0`. For example, using GTKTerm:
        ```bash { .copy }
        gtkterm -p /dev/ttyACM0 -s 115200
        ```

        After this, you should see output in your serial terminal.
    === ":fontawesome-brands-apple: macOS"
        TBA

    === ":fontawesome-brands-windows: Windows"
        TBA

## Configuration
In addition to the [Libtropic CMake options](../../reference/integrating_libtropic/how_to_configure/index.md) used to configure Libtropic, this example provides the following CMake option:

- `LT_SH0_KEYS` (default: `"prod0"`) selects which pairing keys in slot 0 are used. Switch to engineering-sample pairing keys if your TROPIC01 is provisioned with them:

    ??? example "Building with engineering sample pairing keys"
        === ":fontawesome-brands-linux: Linux"
            ```bash { .copy }
            cmake -DLT_SH0_KEYS="eng_sample" ..
            make
            ```

        === ":fontawesome-brands-apple: macOS"
            TBA

        === ":fontawesome-brands-windows: Windows"
            TBA
    Additionally, see [Default Pairing Keys for a Secure Channel Handshake](../../reference/default_pairing_keys.md) for more information.