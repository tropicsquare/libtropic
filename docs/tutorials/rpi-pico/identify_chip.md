# 1. Chip Identification Example Tutorial

--8<-- "docs/common/examples_descriptions/identify_chip.md"

## Build and Run
!!! example "Building, flashing and running the example"
    === ":fontawesome-brands-linux: Linux"
        Go to the example's project directory:
        ```bash { .copy }
        cd examples/rpi-pico/<your_board>/identify_chip/
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

        Now, we need to get the board into **USB Mass Storage Mode**:

        1. Unplug the board.
        2. Hold the `BOOTSEL` button.
        3. Plug USB back in while still holding `BOOTSEL`.
        4. Release `BOOTSEL`. The board should appear as a mass-storage `vfat` device with labels like `RP2040`, `RP2350` or `RPI-RP2`; you can check it with:
            ```bash { .copy }
            lsblk -f  # look into the LABEL column
            ```
            The board should also be automatically mounted in `/media/<your_username>/`, but this depends on your system settings. Use the instructions below to mount it manually if needed:
            
            ??? example "Mounting the Pico board manually"
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

        5. Copy generated `uf2` file to the mounted device (`<mount_location>` is either `/media/<username>/<RP*>/` or `pico_mnt/` if mounted manually in step 4):
            ```bash { .copy }
            cp *.uf2 <mount_location>
            ```
        6. If you mounted the device manually in step 4, unmount:
            ```bash { .copy }
            sudo umount pico_mnt/
            ```
        7. Open your Pico's serial port using your preffered serial monitor with configuration 8-n-1 and baudrate set to 115200. By default, the port is mapped to `/dev/ttyACM0`. For example, using GTKTerm:
            ```bash { .copy }
            gtkterm -p /dev/ttyACM0 -s 115200
            ```

            !!! warning "DTR signal support"
                The program will wait until a serial monitor is connected, i.e. after the DTR signal is asserted — make sure your serial monitor asserts it! The program waits for the DTR signal so no output is lost before you connect to the serial (the serial port is created only after flashing the program).
        
        After this, you should see output in your serial terminal.
        
        Optional next program uploads can be done with `picotool`. This means the BOOTSEL button does not have to be manually pressed. There are two options:

        1. Reboot and upload `uf2` (works from host regardless of device state):
        ```bash { .copy }
        picotool reboot -f -u
        cp *.uf2 <mount_location>
        ```

        2. Direct upload (no mount required):
        ```bash { .copy }
        picotool load libtropic_identify_chip.elf -f
        ```

            !!! note
                `picotool load` talks to a running device (your firmware) over the USB protocol and writes the image without exposing the ROM mass-storage. Use the ELF produced in `build/` so `picotool` can find symbols; this only works when the board is not in the ROM bootloader. If `picotool load` reports the device is absent or unreachable, fall back to the first method above (or press BOOTSEL and copy the UF2 manually).

    === ":fontawesome-brands-apple: macOS"
        TBA

    === ":fontawesome-brands-windows: Windows"
        TBA
