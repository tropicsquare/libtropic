# TROPIC01 Model Tutorials
Using TROPIC01 Model is a great way of evaluating the Libtropic SDK on your computer and discovering amazing TROPIC01 functionality without a need of a physical chip!

Currently, we support following desktop platforms:

- :fontawesome-brands-linux: Linux ✅
- :fontawesome-brands-apple: macOS ❌
- :fontawesome-brands-windows: Windows ❌

!!! info "macOS and Windows"
    Support for macOS and Windows is being prepared.

This tutorial assumes you are comfortable with using command line.

First, we will prepare your environment by installing dependencies. Then, you can continue with our tutorials.

## Install Dependencies and Prepare the Repository
The tutorials with TROPIC01 Model require following dependencies:

- Python >3.8
    - Python 3.8 is preferred, but newer versions may work as well.
- CMake
- GCC

After you had prepared the dependencies, clone the Libtropic repository:

```shell
git clone https://github.com/tropicsquare/libtropic.git
```

Or you can [download latest release](https://github.com/tropicsquare/libtropic/releases/latest).

## Start Experimenting with our Tutorials!
1. [Your First Steps with Libtropic](./first_steps.md)

## Details
This section provides more details about the TROPIC01 model for those interested.

The model is provided by [TROPIC Verification Library](https://github.com/tropicsquare/ts-tvl). See the repository for more details about the model and the source code. Below we discuss specifics of the TROPIC01 Model usage with Libtropic.

### How it works?
The Libtropic uses the TCP HAL implemented in `hal/posix/tcp/libtropic_port_posix_tcp.c`, so both processes (the compiled binary and the model) communicate through a TCP socket at 127.0.0.1:28992. The SPI layer between libtropic and the model is emulated through this TCP connection. The model responses match those of the physical TROPIC01 chip.

### Model Configuration
!!! warning "Custom model configuration"
    Custom configuration is for advanced users only and it is not supported by our examples and tests, as modifications are required.

The TROPIC01 Model can be configured. To configure the model, pass a YAML configuration file to the model — see the [Model Configuration](https://github.com/tropicsquare/ts-tvl?tab=readme-ov-file#model-configuration) section in the [TROPIC Verification Library](https://github.com/tropicsquare/ts-tvl).

For convenience, we provide a default model configuration (`scripts/tropic01_model/model_cfg.yml`), suitable for both examples and tests. If you want to modify the configuration, you can take this file as template and then modify the values according to the [documentation](https://github.com/tropicsquare/ts-tvl?tab=readme-ov-file#model-configuration). Internally, we generate the configuration from our [provisioning data](../../../other/provisioning_data.md).

??? note "Advanced: Generating config from provisioning data"
    We provide both provisioning data and a Python script that can generate configuration for the model from such data. This is useful mainly for internal purposes, we describe it here for reference.

    To create a model configuration that will initialize the model to the state which is almost identical to a provisioned chip, use the `tropic01_model/create_model_cfg.py` script. Run `--help` to see available options and their explanation:
    ```shell
    cd tropic01_model/
    python3 create_model_cfg.py --help
    ```
    !!! info "The `--pkg-dir` Option"
        The script expects a path to one of the lab batch packages inside `tropic01_model/provisioning_data/` - see [Provisioning Data](provisioning_data.md) for more information.

!!! warning "Using custom pairing keys"   
    If you change the pairing keys in the model's configuration, you will not be able to run our examples without modification, as they use default pairing keys. Each example contains `LT_EX_SH0_PRIV` and `LT_EX_SH0_PUB` at the beginning. Simply modify these constants to use arrays with your own keys to make examples work with custom keys.