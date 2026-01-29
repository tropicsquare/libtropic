# 1. Chip Identification Example

--8<-- "docs/common/examples_descriptions/identify_chip.md"

## Build and Run
!!! example "Building and running the example"
    === ":fontawesome-brands-linux: Linux"
        Go to the example's project directory:
        ```bash { .copy }
        cd examples/stm32/<your_board>/identify_chip/
        ```

        Create a `build/` directory and switch to it:
        ```bash { .copy }
        mkdir build/
        cd build/
        ```

        Open your STM32's serial port using your preferred serial monitor with configuration 8-N-1 and baudrate set to 115200. By default, the serial port is mapped to `/dev/ttyACM0`. For example, using GTKTerm:
        ```bash { .copy }
        gtkterm -p /dev/ttyACM0 -s 115200
        ```

        !!! warning
            Make sure only one serial monitor has the STM32's serial port open, otherwise your output may appear mangled.

        And finally, build and run the example:
        ```bash { .copy }
        cmake ..
        make
        make flash
        ```

    === ":fontawesome-brands-apple: macOS"
        TBA

    === ":fontawesome-brands-windows: Windows"
        TBA

    After this, you should see a colored output in your serial monitor.

[Next example :material-arrow-right:](fw_update.md){ .md-button }