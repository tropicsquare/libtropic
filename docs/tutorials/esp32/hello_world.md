# Hello, World! Example

--8<-- "docs/common/examples_descriptions/hello_world.md"

## Build and Run the Example
!!! example "Building and running the example"
    === ":fontawesome-brands-linux: Linux"
        Go to the example's project directory:
        ```bash
        cd examples/esp32/<your_board>/hello_world/
        ```

        Build, flash and run the serial monitor using this command:
        ```bash
        idf.py build flash monitor
        ```

    === ":fontawesome-brands-apple: macOS"
        TBA

    === ":fontawesome-brands-windows: Windows"
        TBA

    After this, you should see a colored output in your terminal.

If your TROPIC01 has engineering sample pairing keys, you can switch to them using the `LT_SH0_KEYS` CMake option:
!!! example "Switching to engineering sample pairing keys"
    === ":fontawesome-brands-linux: Linux"
        You can pass any CMake option to `idf.py` as follows:
        ```bash
        idf.py -DLT_SH0_KEYS="eng_sample" build flash monitor
        ```

    === ":fontawesome-brands-apple: macOS"
        TBA

    === ":fontawesome-brands-windows: Windows"
        TBA

Additionally, see [Default Pairing Keys for a Secure Channel Handshake](../../reference/default_pairing_keys.md) for more information.