# Raspberry Pi Pico Tutorials
These tutorials will help you get started with TROPIC01 on Raspberry Pi Pico platforms using Libtropic. We will go through our examples in the `examples/rpi-pico/` directory. In this directory, there are multiple subdirectories for each supported Raspberry Pi Pico board. Most of the instructions in this tutorial are common for all of the boards.

## Hardware Setup
### TROPIC01
!!! info "TROPIC01 Devkit for ESP32 boards"
    Currently, we don't offer a devkit for ESP32 boards. However, you can use any of our devkits that use SPI, except the USB DevKit.

For the purpose of these tutorials, we will use our TROPIC01 Arduino Shield:
<figure style="text-align: center;">
<img src="../../img/arduino-shield-pinout.svg" alt="TROPIC01 Arduino Shield pinout" width="500"/>
<figcaption style="font-size: 0.9em; color: #555; margin-top: 0.5em;">
    TROPIC01 Arduino Shield pinout
  </figcaption>
</figure>
You can get TROPIC01 Arduino Shield and other devkits [here](https://www.tropicsquare.com/order-devkit).

### Your Raspberry Pi Pico Board
Unfortunately, Raspberry Pi Pico boards and our Arduino shield are not plug-and-play, so please prepare some jump wires and use the figure above with Arduino Shield Pinout to help you during the setup. Follow the connection instructions for your Pico board below:
!!! example "Connection Instructions"
    === "Raspberry Pi Pico"
        Raspberry Pi Pico pin layout [here](https://www.raspberrypi.com/documentation/microcontrollers/images/pico-pinout.svg).
        <div style="text-align:center" markdown="1">

        | TROPIC01 Arduino Shield Pin | Raspberry Pi Pico Pin |
        |:---------------------------:|:---------------------:|
        | IOREF                       | 3V3(OUT)              |
        | +3V3                        | 3V3(OUT)              |
        | GND                         | GND                   |
        | SCK                         | GP14                  |
        | MISO                        | GP12                  |
        | MOSI                        | GP15                  |
        | CS                          | GP13                  |
        | GPO                         | GP22                  |
        
        </div>
    
    === "Raspberry Pi Pico 2"
        Raspberry Pi Pico 2 pin layout [here](https://www.raspberrypi.com/documentation/microcontrollers/images/pico-2-r4-pinout.svg).
        <div style="text-align:center" markdown="1">

        | TROPIC01 Arduino Shield Pin | Raspberry Pi Pico 2 Pin |
        |:---------------------------:|:-----------------------:|
        | IOREF                       | 3V3(OUT)                |
        | +3V3                        | 3V3(OUT)                |
        | GND                         | GND                     |
        | SCK                         | GP14                    |
        | MISO                        | GP12                    |
        | MOSI                        | GP15                    |
        | CS                          | GP13                    |
        | GPO                         | GP22                    |
        
        </div>

!!! question "How to Use Different Pins?"
    The pin connections above are used in our examples by default. The pins can be changed in each example's `main.c` — look for the `main()` function and adjust the initialization of the `lt_dev_rpi_pico_t` structure.

## Software Setup
See below for instructions based on your OS:

!!! example "Installation Instructions"
    === ":fontawesome-brands-linux: Linux"
        1. Install build dependencies:
            - Ubuntu/Debian:
              ```bash { .copy }
              sudo apt install -y cmake ninja-build gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential git python3 libusb-1.0-0-dev
              ```
            - Fedora:
              ```bash { .copy }
              sudo dnf groupinstall "Development Tools"
              sudo dnf install cmake ninja-build arm-none-eabi-gcc-cs-c++ arm-none-eabi-gcc arm-none-eabi-newlib python3 libusb1-devel
              ```
        2. Install Pico SDK:
            ```bash { .copy }
            git clone https://github.com/raspberrypi/pico-sdk.git
            cd pico-sdk/
            git submodule update --init --recursive
            ```
        3. Export `PICO_SDK_PATH`:
            ```bash { .copy }
            export PICO_SDK_PATH=<path_to_pico-sdk>
            ```
        4. Install picotool:
            ```bash { .copy }
            git clone https://github.com/raspberrypi/picotool.git
            cd picotool/
            mkdir build/
            cd build/
            cmake -G Ninja ..
            ninja
            sudo cmake --install .
            ```
        5. Install a serial monitor of your choice (`minicom`, `screen`, `GTKTerm`).
            - For beginners we recommend GUI-based GTKTerm.
                - Ubuntu/Debian: `sudo apt update && sudo apt install gtkterm`
                - Fedora: `sudo dnf install gtkterm`
            !!! warning "DTR signal support"
                Make sure the serial monitor asserts the [DTR](https://en.wikipedia.org/wiki/Data_Terminal_Ready) signal (all of the mentioned serial monitors should do it by default).
        6. Get the Libtropic repository:
            - Using git: `git clone https://github.com/tropicsquare/libtropic.git`
            - Or you can download the [latest release](https://github.com/tropicsquare/libtropic/releases/latest).

    === ":fontawesome-brands-apple: macOS"
        TBA

    === ":fontawesome-brands-windows: Windows"
        TBA

## Start with our Tutorials!
!!! warning "Do not skip!"
    We strongly recommend going through each tutorial in this specific order without skipping. You will gather basic information about the chip and update your TROPIC01's firmware, which will guarantee compatibility with the latest Libtropic API.

1. [Chip Identification](identify_chip.md)
3. [Hello, World!](hello_world.md)

## FAQ
If you encounter any issues, please check the [FAQ](../../faq.md) before filing an issue or reaching out to our [support](https://support.tropicsquare.com/).
