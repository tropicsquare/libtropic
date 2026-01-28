# 2. FW Update Example

--8<-- "docs/common/examples_descriptions/fw_update.md"

## Build and Run
!!! example "Building and running the example"
    === ":fontawesome-brands-linux: Linux"
        Go to the example's project directory:
        ```bash { .yaml .copy }
        cd examples/linux/spi/fw_update/
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
        ./libtropic_fw_update
        ```

    === ":fontawesome-brands-apple: macOS"
        TBA

    === ":fontawesome-brands-windows: Windows"
        TBA

    The example will prompt you for confirmation. Type `y` to start the update.
    
    After successful execution, your chip will contain the latest firmware and will be compatible with the current Libtropic API.

[Next example :material-arrow-right:](hello_world.md){ .md-button }