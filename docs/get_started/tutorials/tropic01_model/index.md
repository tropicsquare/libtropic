# TROPIC01 Model Tutorial
Using TROPIC01 Model is a great way of evaluating the Libtropic SDK on your computer and discovering amazing TROPIC01 functionality without a need of a physical chip!

!!! info "OS Support"
    Currently we support Linux only. Support for macOS and Windows is being prepared.

This tutorial assumes you are comfortable with using command line.

First, we will prepare your environment by installing dependencies. Then, you can continue with the next steps.

## Install Dependencies and Prepare the Repository
See below for instructions based on your OS.

??? example "Installation Instructions"
    === ":fontawesome-brands-linux: Linux"
        1. [Install Python](https://www.python.org/downloads/)
            - You can also use your distribution's package manager.
                - Fedora: `sudo dnf install python3`
                - Debian/Ubuntu: `sudo apt update && sudo apt install python3`
        2. [Install CMake](https://cmake.org/download/)
            - You can also use your distribution's package manager.
                - Fedora: `sudo dnf install cmake`
                - Debian/Ubuntu: `sudo apt update && sudo apt install cmake`
        3. Install GCC and Make via your distribution's package manager
            - Some distributions offer complete development environment (including headers), so it's recommended to install this.
            - Fedora: `sudo dnf group install development-tools`
            - Debian/Ubuntu: `sudo apt update && sudo apt install build-essential`
        4. Get the Libtropic repository:
            - Using git: `git clone https://github.com/tropicsquare/libtropic.git`
            - Or you can [download latest release](https://github.com/tropicsquare/libtropic/releases/latest).
        5. Install TROPIC01 Model
            - Use our install script: `scripts/tropic01_model/install_linux.sh`
            - The script will create a virtual environment.
            - Activate the environment: `source scripts/tropic01_model/.venv/bin/activate`
        6. Install Python dependencies: jinja2 and jsonschema
            - It's recommended to use virtual environment created in the step 4.
            - Run: `pip3 install jinja2 jsonschema`

    === ":fontawesome-brands-apple: macOS"
        TBA

    === ":fontawesome-brands-windows: Windows"
        TBA

## Let's Get Started!

!!! warning "Paths"
    All commands in all tutorials assume running from the repository root, unless
    they are preceded with `cd` (or similar) command. If you are running commands from
    elsewhere, make sure to correct paths in arguments.

1. 👣 [First Steps with Libtropic](./first_steps_with_libtropic.md)
2. 💡 [Understanding Libtropic](./understanding_libtropic.md)

---

## TROPIC01 Model Details
This section provides more details about the TROPIC01 model for those interested.

The model is provided by [TROPIC Verification Library](https://github.com/tropicsquare/ts-tvl). See the repository for more details about the model and the source code. Below we discuss specifics of the TROPIC01 Model usage with Libtropic.

### How it works with Libtropic?
The Libtropic uses the TCP HAL implemented in `hal/posix/tcp/libtropic_port_posix_tcp.c`, so both processes (the compiled binary and the model) communicate through a TCP socket at 127.0.0.1:28992. The SPI layer between Libtropic and the model is emulated through this TCP connection. The model responses match those of the physical TROPIC01 chip.

### Configuration
!!! warning "Custom Configuration"
    Custom configuration of the TROPIC01 Model is for advanced users only and it is not supported by our examples and tests, as modifications are required.

The TROPIC01 Model can be configured. To configure the model, pass a YAML configuration file to it — see the [Model Configuration](https://github.com/tropicsquare/ts-tvl?tab=readme-ov-file#model-configuration) section in the [TROPIC Verification Library](https://github.com/tropicsquare/ts-tvl).

For convenience, we provide a default model configuration (`scripts/tropic01_model/model_cfg.yml`), suitable for both examples and tests. If you want to customize the configuration, you can take this file as template and then modify the values according to the [model's documentation](https://github.com/tropicsquare/ts-tvl?tab=readme-ov-file#model-configuration). Internally, we generate the configuration from our testing [Provisioning Data](../../../other/provisioning_data.md).

??? note "Advanced: Generating Configuration from Provisioning Data"
    We provide both testing provisioning data and a Python script that can generate configuration for the model from such data. This is useful mainly for internal purposes, we describe it here for reference.

    To create a model configuration that will initialize the model to the state which is almost identical to a provisioned chip, use the `scripts/tropic01_model/create_model_cfg.py` script. Run `--help` to see available options and their explanation:
    ```shell
    cd scripts/tropic01_model/
    python3 create_model_cfg.py --help
    ```
    !!! info "The `--pkg-dir` Option"
        The script expects a path to one of the lab batch packages inside `scripts/tropic01_model/provisioning_data/` - see [Provisioning Data](../../../other/provisioning_data.md) for more information.

!!! warning "Using Custom Pairing Keys"   
    If you change the pairing keys in the model's configuration, you will not be able to run our examples without modification, as they use default pairing keys. Each example contains `LT_EX_SH0_PRIV` and `LT_EX_SH0_PUB` at the beginning. Simply modify these constants to use arrays with your own keys to make examples work with custom keys.