# 2. Hardware Wallet Example
This example demonstrates how to use configuration objects and different pairing keys to manage access to TROPIC01 features. A hardware wallet device scenario is used as a model for this example.

!!! success "Prerequisites"
    It is assumed that you have already completed the previous TROPIC01 Model tutorials. If not, start [here](../model/index.md).

In this example, you will:

- Understand how the R-config is structured and how permissions are managed using the R-config.
- Learn how the R-config can be modified using the libtropic API:
    - `lt_r_config_erase()`: L3 command to erase the R-config.
    - `lt_write_whole_R_config()`: helper function to write the whole R-config with an instance of `struct lt_config_t`.
    - `lt_read_whole_R_config()`: helper function to read the whole R-config into an instance of `struct lt_config_t`.

        !!! tip "Tip: Modifying Only One R-Config Register"
            If you need to modify only one register in the R-config, you can use `lt_r_config_write()` or `lt_r_config_read()`.
            
- Learn how to manage pairing keys:
    - `lt_pairing_key_write()`: L3 command to write a pairing key.
    - `lt_pairing_key_invalidate()`: L3 command to invalidate a pairing key.
- Learn how to work with keys based on elliptic curves (ECC) on TROPIC01:
    - `lt_ecc_key_store()`: L3 command to store an ECC key.
    - `lt_ecc_key_read()`: L3 command to read an ECC key.
    - `lt_ecc_key_generate()`: L3 command to generate an ECC key.
- Learn how to use EDDSA to sign messages and verify signatures.
- Learn how to use a monotonic counter.

## Build and Run
Before proceeding, make sure you have activated the virtual environment you installed the TROPIC01 Model in and started it. If you're lost, see [First Steps](first_steps.md).

Now, you can build and run the example:

!!! example "Building and running the example"
    === ":fontawesome-brands-linux: Linux"
        Go to the example's project directory:
        ```bash { .copy }
        cd examples/model/hw_wallet/
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
        ./libtropic_hw_wallet
        ```

    === ":fontawesome-brands-apple: macOS"
        TBA

    === ":fontawesome-brands-windows: Windows"
        TBA

!!! warning "Irreversible Changes"
    Because this example performs some irreversible changes, the model has to be terminated and started again before running the example binary again. This action results in making the model behave like a fresh TROPIC01 chip again.

[Next tutorial :material-arrow-right:](macandd_example.md){ .md-button }