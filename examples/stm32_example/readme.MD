# About

This is a minimalistic example for NUCLEO-F439ZI devboard:

* It expects SPI connection to a running TROPIC01 - chip, model or FPGA
* used SPI pins are: PE2 SCK, PE4 CS, PE5 MISO, PE6 MOSI
* Debug messages are transmitted over USB ACM, they can be received by opening a serial port

## Dependencies

* cmake
* arm-none-eabi-gcc
* openocd (check OPENOCD_CFG in CMakeLists.txt o set path to config file for your devboard)

# How to compile and flash?

 Connect devboard and run in this folder:
```
mkdir build
cd build
cmake ..
make flash
```

# Debugging

## VS Code

Dependencies:

* cortex-debug extension
* arm-none-eabi-gdb

Place this launch.json file into .vscode folder:

```
{
    "version": "0.2.0",
    "configurations": [
        {
            "type": "cortex-debug",
            "request": "attach",
            "name": "Debug (OpenOCD)",
            "servertype": "openocd",
            "cwd": "${workspaceRoot}",
            //"preLaunchTask": "cargo build",
            //"runToEntryPoint": "true",
            "executable": "./examples/stm32_example/build/stm32_example.elf",
            "device": "STM32F429ZI",
            "configFiles": [
                "/usr/share/openocd/scripts/board/stm32f429discovery.cfg"
            ],
            // Commented setup is not working at the moment, but lets have it here anyway
            //"svdFile": "${workspaceRoot}/.vscode/STM32F303.svd",
            //"swoConfig": {
            //    "enabled": true,
            //    "cpuFrequency": 8000000,
            //    "swoFrequency": 2000000,
            //    "source": "probe",
            //    "decoders": [
            //        { "type": "console", "label": "ITM", "port": 0 }
            //    ]
            //}
        }
    ]
}

```
