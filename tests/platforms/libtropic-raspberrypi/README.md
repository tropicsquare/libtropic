# Unix-spi example

If our shield is used, put jumper on a CS2 position.

Otherwise wire chip like this:

```
                    Connector J8 on Raspberry Pi:
         3v3  -->        3V3  (1) (2)  5V    
                       GPIO2  (3) (4)  5V    
                       GPIO3  (5) (6)  GND   
                       GPIO4  (7) (8)  GPIO14
         GND  -->        GND  (9) (10) GPIO15
                      GPIO17 (11) (12) GPIO18
                      GPIO27 (13) (14) GND   
                      GPIO22 (15) (16) GPIO23
                         3V3 (17) (18) GPIO24
         MOSI -->     GPIO10 (19) (20) GND   
         MISO -->      GPIO9 (21) (22) GPIO25    <--  CS
          CLK -->     GPIO11 (23) (24) GPIO8 
                         GND (25) (26) GPIO7 
                       GPIO0 (27) (28) GPIO1 
                       GPIO5 (29) (30) GND   
                       GPIO6 (31) (32) GPIO12
                      GPIO13 (33) (34) GND   
                      GPIO19 (35) (36) GPIO16
                      GPIO26 (37) (38) GPIO20
                         GND (39) (40) GPIO21

```

This example is ready to be compiled on Raspberry Pi with official raspbian image. Hardware spi0 is used, but chip select pin is controlled separately, not by spi hw.

# Dependencies

Enable spi:

```
sudo raspi-config  # then go into Interfaces and enable SPI
```

On raspberry pi install wiringPi from released package

```bash
$ wget https://github.com/WiringPi/WiringPi/releases/download/3.14/wiringpi_3.14_arm64.deb

$ sudo apt install ./wiringpi_3.14_arm64.deb
```

Check out installed version:

```bash
$ gpio -v
gpio version: 3.14
Copyright (c) 2012-2025 Gordon Henderson and contributors
This is free software with ABSOLUTELY NO WARRANTY.
For details type: gpio -warranty

Hardware details:
  Type: Pi 3, Revision: 02, Memory: 1024MB, Maker: Sony UK

System details:
  * Device tree present.
      Model: Raspberry Pi 3 Model B Rev 1.2
  * Supports full  user-level GPIO access via memory.
  * Supports basic user-level GPIO access via /dev/gpiomem.
  * Supports basic user-level GPIO access via /dev/gpiochip (slow).

```

# Compile Hello World

Use switches as described here TODO to control what examples will be compiled in.

```bash
$ cd ./libtropic/tests/platforms/libtropic-raspberrypi

$ mkdir build && cd build && cmake  -DCMAKE_BUILD_TYPE=Debug -DLT_EX_HELLO_WORLD .. && make
```

# Execute

```bash
$ ./run
```

# Other examples

Pass compilation switch to compile one of other examples:
```bash
-DLT_EX_HELLO_WORLD
-DLT_EX_TEST_REVERSIBLE
```
