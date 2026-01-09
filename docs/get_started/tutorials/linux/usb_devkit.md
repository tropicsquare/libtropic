# Linux USB Devkit Tutorial
This project is compatible with Linux-based systems (and possibly other POSIX-compatibles) and our [USB devkit](https://github.com/tropicsquare/tropic01-stm32u5-usb-devkit-hw). Follow the link to get more details about this devkit, including schematics, design files, and manufacturing data.

It is recommended to read through the [Libtropic SDK documentation](https://tropicsquare.github.io/libtropic/latest/) before proceeding.

## Install Dependencies
Make sure to have these dependencies installed:

- CMake
    - Raspbian/Debian/Ubuntu: `sudo apt install cmake`
    - Fedora: `sudo dnf install cmake`
- GCC
    - Raspbian/Debian/Ubuntu: `sudo apt install gcc`
    - Fedora: `sudo dnf install gcc`

MbedTLS 4.0.0 which we use in this repository requires:

- Recent Python
- The following Python packages:
    - jinja2
    - jsonschema

The recommended method is to use Python virtual environment to install those packages. Instructions for Linux:
```shell
python3 -m venv .venv
source .venv/bin/activate
pip3 install --upgrade pip
pip3 install jinja2 jsonschema
```

## System Setup
Make sure you have access to a USB UART interface. Usually, your user account has to be a member of a certain group, usually the `dialout` group.

```bash
# Check if you are in the dialout group
groups
# If not, add yourself to the dialout group
sudo usermod -aG dialout "$USER"
# Log out and log in again to reflect changes.
```

## Clone the Libtropic Repository

```bash
git clone https://github.com/tropicsquare/libtropic.git
cd libtropic
git submodule update --init --recursive
cd examples/linux/usb_devkit
```

## Build and Run a Basic Hello World Example

This basic example demonstrates basic usage of the Libtropic SDK. In the example, the Secure Session is established and a Ping L3 Command is sent to verify that the Secure Session works.
This example is also useful as a template for your project, as it contains minimal `CMakeLists.txt` to build Libtropic including dependencies.

```bash
cd hello_world
mkdir build
cd build
cmake ..
make
./libtropic_hello_world
```

## Build and Run a Chip Identification Example

Next, we will read important identification data from the chip.

```bash
cd identify_chip
mkdir build
cd build
cmake ..
make
./libtropic_identify_chip | tee chip_info.txt # We will backup the info to a file. 
```

> [!IMPORTANT]
> Do not forget to store the output of this example! It is crucial for providing any support by Tropic Square.

## Build and Run a Firmware Update Example
After trying out communication, we will update the TROPIC01's firmware using our firmware update example, as new firmware versions fix bugs and ensure compatibility with the latest Libtropic SDK.

> [!IMPORTANT]
> - Using outdated firmware is not recommended. Outdated firmware may not be compatible with the latest version of the Libtropic SDK.
> - Make sure you stored chip identification from the previous step.
> - Use a stable power source and avoid disconnecting the devkit or rebooting your computer during the update. Interrupting a firmware update can brick the device.

To update both internal firmware to the latest versions, execute the following example:

```bash
cd fw_update
mkdir build
cd build
cmake ..
make
./libtropic_fw_update
```

The example will prompt you for confirmation. Type `y` to start the update.

After successful execution, your chip will contain the latest firmware and will be compatible with the current Libtropic API.

## FAQ

If you encounter any issues, please check the [FAQ](../../../faq.md) before filing an issue or reaching out to our [support](https://support.desk.tropicsquare.com/).
