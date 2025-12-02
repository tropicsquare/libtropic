# Arduino
We provide a HAL which utilizes only the [Arduino API](https://github.com/arduino/ArduinoCore-API), making it compatible with all Arduino platforms. The HAL is implemented in the `libtropic/hal/arduino/` directory.

We also provide the [libtropic-arduino](https://github.com/tropicsquare/libtropic-arduino) repository, which follows the directory structure of Arduino libraries and implements support for [PlatformIO](https://platformio.org/). Refer to the repository for more information.

!!! warning "Disclaimer"
    The Arduino HAL is not suitable for production use. We strongly recommend using it for demo projects only.