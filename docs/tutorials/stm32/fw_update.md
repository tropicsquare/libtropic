# 2. FW Update Example

--8<-- "docs/common/examples_descriptions/fw_update.md"

## Build and Run
!!! example "Building and running the example"
    === ":fontawesome-brands-linux: Linux"
        Go to the example's project directory:
        ```bash { .yaml .copy }
        cd examples/stm32/<your_board>/fw_update/
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

    After this, you should see a colored output in your terminal.

[Next example :material-arrow-right:](hello_world.md){ .md-button }