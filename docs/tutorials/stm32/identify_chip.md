# 1. Chip Identification Example

--8<-- "docs/common/examples_descriptions/identify_chip.md"

## Build and Run
!!! example "Building and running the example"
    === ":fontawesome-brands-linux: Linux"
        Go to the example's project directory:
        ```bash { .yaml .copy }
        cd examples/stm32/<your_board>/identify_chip/
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
        ./libtropic_identify_chip
        ```

    === ":fontawesome-brands-apple: macOS"
        TBA

    === ":fontawesome-brands-windows: Windows"
        TBA

    After this, you should see a colored output in your terminal.

[Next example :material-arrow-right:](fw_update.md){ .md-button }