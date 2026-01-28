# 3. Mac-And-Destroy Example
This example illustrates MAC-And-Destroy, the flagship feature of TROPIC01.

!!! success "Prerequisites"
    It is assumed that you have already completed the previous TROPIC01 Model tutorials. If not, start [here](../model/index.md).

You will learn about the following functions:

- `lt_mac_and_destroy()`: L3 command to process MAC-and-Destroy operation,
- `lt_r_mem_data_erase()`: L3 command to erase R-memory data slot,
- `lt_r_mem_data_write()`: L3 command to write R-memory data slot,
- `lt_r_mem_data_read()`: L3 command to read R-memory data slot,
- `lt_random_value_get()`: function to generate random number using TROPIC01's TRNG.

In this example, we also define two functions to implement PIN verification functionality:

- `new_PIN_setup()`: setups the PIN,
- `PIN_entry_check()`: checks the PIN.

You can use these functions as an inspiration for your project.

!!! info "More Information"
    For more information about Mac-And-Destroy, we recommend checking out the [Pin Verification Application Note](https://github.com/tropicsquare/tropic01?tab=readme-ov-file#application-notes) or the example's source code in `examples/model/mac_and_destroy/`.

## Build and Run
Before proceeding, make sure you have activated the virtual environment you installed the TROPIC01 Model in and started it. If you're lost, see [First Steps](first_steps.md).

Now, you can build and run the example:

!!! example "Building and running the example"
    === ":fontawesome-brands-linux: Linux"
        Go to the example's project directory:
        ```bash { .copy }
        cd examples/model/mac_and_destroy/
        ```
        Create a `build/` directory and switch to it:
        ```bash { .copy }
        mkdir build/
        cd build/
        ```
        And finally, build and run the example:
        ```bash { .copy }
        cmake ..
        make
        ./libtropic_mac_and_destroy
        ```

    === ":fontawesome-brands-apple: macOS"
        TBA

    === ":fontawesome-brands-windows: Windows"
        TBA

[Next tutorial :material-arrow-right:](separate_api.md){ .md-button }