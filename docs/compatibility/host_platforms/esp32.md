# ESP32
To support the widest range of Espressif SoCs possible, we provide a HAL directly for the [Espressif IoT Development Framework](https://github.com/espressif/esp-idf) (ESP-IDF). The HAL is available in the `libtropic/hal/esp-idf/` directory.

!!! info "ESP-IDF Version"
    The ESP-IDF version tested with Libtropic is 5.5.1.

Currently tested ESP32 boards are:

- [ESP32-DevKitC-V4](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitc/user_guide.html)
- [ESP32-S3-DevKitC-1](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitc-1/index.html)
- [ESP32-C3-DevKit-RUST-1](https://cz.mouser.com/ProductDetail/Espressif-Systems/ESP32-C3-DevKit-RUST-1?qs=4ASt3YYao0WvXOj9TGjU2A%3D%3D)

See our [ESP32 Tutorial](../../tutorials/esp32/index.md) to quickly get started.

## Initialization
If Libtropic's [LT_USE_INT_PIN](../../reference/integrating_libtropic/how_to_configure/index.md#lt_use_int_pin) CMake option is used, the ESP-IDF HAL will use GPIO interrupts. This puts a requirement on your application — in your code, call the [`gpio_install_isr_service`](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/gpio.html#_CPPv424gpio_install_isr_servicei) function with parameter `0` before calling `lt_init`. This function has to be called **exactly once** in your application. See the ESP32 examples in the `examples/esp32/` directory for inspiration.