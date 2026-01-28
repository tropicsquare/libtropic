# 3. Hello, World! Example

--8<-- "docs/common/examples_descriptions/hello_world.md"

## Build and Run
!!! example "Building and running the example"
    === ":fontawesome-brands-linux: Linux"
        Go to the example's project directory:
        ```bash { .yaml .copy }
        cd examples/linux/spi/hello_world/
        ```

        Create a `build/` directory and switch to it:
        ```bash { .yaml .copy }
        mkdir build/
        cd build/
        ```

        And finally, build and run the example:
        ```bash { .yaml .copy }
        cmake ..
        make -j
        ./libtropic_hello_world
        ```

    === ":fontawesome-brands-apple: macOS"
        TBA

    === ":fontawesome-brands-windows: Windows"
        TBA

    After this, you should see an output in your terminal.

If your TROPIC01 has engineering sample pairing keys, you can switch to them using the `LT_SH0_KEYS` CMake option:
!!! example "Switching to engineering sample pairing keys"
    === ":fontawesome-brands-linux: Linux"
        You can pass `LT_SH0_KEYS` to `cmake` as follows:
        ```bash { .yaml .copy }
        cmake -DLT_SH0_KEYS="eng_sample" ..
        make -j
        ```

    === ":fontawesome-brands-apple: macOS"
        TBA

    === ":fontawesome-brands-windows: Windows"
        TBA

Additionally, see [Default Pairing Keys for a Secure Channel Handshake](../../../reference/default_pairing_keys.md) for more information.