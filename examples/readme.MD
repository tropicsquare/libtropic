# Examples

Each example can be built with the same commands, for example:

```
$ cd crypto_tests/
$ mkdir build
$ cd build
$ cmake ..
$ make

```

## Crypto tests

This code does not communicate with Tropic01. Its only purpose is to run cryptographic test vectors through library's crypto interfaces and check if they calculate correct results. Might be practical to keep this for the future, when another cryptography backend is introduced.


## Unix example

This code communicates with running Tropic01 model over TCP.

It expects Tropic01's model running within a reach of example's TCP client. Tropic's L1 data transfers are over TCP.


## STM32 example

This example is prepared for stm32 target, therefore it expects **arm-none-eabi-gcc** installed on a system.

Once compiled, it is meant to be flashed on NUCLEO-F439ZI devboard. There are two options for L1 transport layer:

### SPI - default

Set it in CMakeLists.txt:

```
set(TS_HAL_STM32_SPI ON)
```

Used pins are:

|       |          |
|-------|----------|
|  MISO | GPIOE_5  |
|  MOSI | GPIOE_6  |
|  SCK  | GPIOE_2  |
|  CS   | GPIOE_4  |


### UART

This option is meant to be used when Libtropic application runs on stm32 devboard and instead of a physical chip (or FPGA) it is talking to the model, which is running on the PC.

Usb-to-serial converter is used.


Set it in CMakeLists.txt:

```
set(TS_HAL_STM32_UART ON)
```

Used pins are:

|       |           |
|-------|-----------|
|  TX   | GPIOC_12  |
|  RX   | GPIOD_2   |


