# Linux
Libtropic support on Linux is implemented with:

- [SPI and GPIO Linux Userspace API](#spi-and-gpio-linux-userspace-api)

HALs for these ports are available in the `libtropic/hal/linux/` directory.

Libtropic example usage with these ports is currently available in our [libtropic-linux](https://github.com/tropicsquare/libtropic-linux) repository.

## SPI and GPIO Linux Userspace API
This port uses the [SPI](https://docs.kernel.org/spi/spidev.html) and [GPIO](https://docs.kernel.org/userspace-api/gpio/chardev.html) Linux Userspace API. It was tested on:

- [Raspberry Pi 4](https://www.raspberrypi.com/products/raspberry-pi-4-model-b/),
- [Raspberry Pi 5](https://www.raspberrypi.com/products/raspberry-pi-5/).