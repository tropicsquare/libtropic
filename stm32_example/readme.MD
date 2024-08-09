# About

This is a minimalistic example for NUCLEO-F439ZI devboard:

* Based on used transport layer, example works with TROPIC01 model or FPGA(physical silicon)
* Debug messages are transmitted over devboard's USB ACM serial port, they can be received by opening a serial port


## Dependencies

* cmake
* arm-none-eabi-gcc
* openocd (check OPENOCD_CFG in CMakeLists.txt. If needed, define own path to config file for your devboard)


## Transport layer

Transport layer must be defined by uncommenting apropriate option in example's CMakeLists.txt.

If SPI is enabled, hardware uses folowing pins:

|       |          |
|-------|----------|
|  MISO | GPIOE_5  |
|  MOSI | GPIOE_6  |
|  SCK  | GPIOE_2  |
|  CS   | GPIOE_4  |


If UART is enabled, hardware uses folowing pins:

|       |           |
|-------|-----------|
|  TX   | GPIOC_12  |
|  RX   | GPIOD_2   |



# How to compile and flash?

Check if a transport layer is defined in CMakeLists.txt. Then connect devboard and run in this folder:
```
mkdir build
cd build
cmake ..
make flash
```
Alternatively, definition for transport layer `-DUSE_UART=1` or `-DUSE_SPI=1` can be passed to define transport layer without modifying CMakeFile.txt:

```
mkdir build
cd build
cmake -DUSE_SPI=1 ..
make flash
```

# Debugging in VS Code

Install dependencies:

* cortex-debug extension
* arm-none-eabi-gdb

Then place this launch.json file into .vscode folder:

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
            "executable": "./build/stm32_example.elf",
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

Debugging should be enabled after restarting your VS Code.

# STM32CubeIDE

* Open `File>New>STM32 Cmake Project`
* Set 'Project with existing CMake sources'
* Choose apropriate name, set source directory to `PATH_TO_THIS_EXAMPLE/stm32_example` and set the name of build directory
* Define used MCU (Supported device is `STM32F439ZITx`)
* Click on finish
* Define transport layer by uncommenting an option in CMakeLists.txt
* Click on Build, then on the left side click on `binaries`, right click on `stm32_example.elf` and choose 'run as a stm32 Application'
