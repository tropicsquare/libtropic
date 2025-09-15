# Adding a New Embedded Platform
!!! bug
    Redo this, add more info, similarly to [Adding a New Cryptographic Backend](adding_crypto_backend.md).

To add a new embedded platform, follow these steps:

1. Implement the necessary HAL (Hardware Abstraction Layer) functions, according to `include/libtropic_port.h`
2. Test the integration by compiling hello world example within your project

For more inspiration check out our Integration examples.