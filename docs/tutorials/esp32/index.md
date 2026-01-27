# ESP32 Tutorial
This tutorial will guide you through the building of the examples in the `examples/esp32/` directory. In this directory, there are multiple subdirectories for each supported ESP32 board. Instructions in this tutorial are common for all of the boards.

## Install Dependencies and Prepare the Repository
See below for instructions based on your OS.

!!! example "Installation instructions"
    === ":fontawesome-brands-linux: Linux"
        1. Setup ESP-IDF and its dependencies:
            - Complete the first 4 steps in the [official ESP-IDF setup guide](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/linux-macos-setup.html).
            - We recommend getting the 5.5.1 version, but any 5.x.x version should work.
        2. Get the Libtropic repository:
            - Using git: `git clone https://github.com/tropicsquare/libtropic.git`
            - Or you can download the [latest release](https://github.com/tropicsquare/libtropic/releases/latest).

    === ":fontawesome-brands-apple: macOS"
        TBA

    === ":fontawesome-brands-windows: Windows"
        TBA

## Start with our Examples!
!!! warning "Do not skip!"
    We strongly recommend going through each example in this specific order without skipping. You will gather basic information about the chip and update your TROPIC01's firmware, which will guarantee compatibility with the latest Libtropic API.

1. [Chip Identification](identify_chip.md)
2. [FW Update](fw_update.md)
3. [Hello, World!](hello_world.md)

## FAQ
If you encounter any issues, please check the [FAQ](../../faq.md) before filing an issue or reaching out to our [support](https://support.desk.tropicsquare.com/).