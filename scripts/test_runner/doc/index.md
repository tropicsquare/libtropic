# TS Test Runner
The test runner is a script for running libtropic tests on platform in a batch using the TS11
platform board.

The script basically automates flashing via JTAG/SWD and reading the test status from
the serial line.


## Usage
There are two interfaces:
- Native Python interface
- CLI wrapper over Python interface

The CLI wrapper is mainly used for testing and debugging. In automation scripts, the native
interface is used directly.

### CLI
To use the CLI interface:
```sh
cd scripts/test_runner
python3 -m lt_test_runner <platform_id> <firmware_path>
```

Platform ID is an internal identifier of a platform. The identifiers are defined in `lt_test_runner/adapter_mapping.csv`, together with mapping to corresponding VID/PID on the testbench. This mapping is designed to be used for batch platform testing on the testbench in a lab, where each platform is connected to a TS11 board with a specific VID/PID.

The last argument is a path to the firmware you want to flash.

There are some optional arguments, see help for more information. Usually they are not needed.

### Native
You can use the `lt_test_runner` class directly, which is preffered in Python scripts (e.g. test automations).

The `lt_test_runner` takes 4 arguments:
- working directory (for logs),
- platform ID,
- path to mapping config,
- path to adapter config.

After the test runner is successfully created, you can call `run` method with a path to the firmware
as an argument. It will return result in a form of enum (`lt_test_runner.lt_test_result`).

An example how to use the native interface is in the `run_tests.py` script.

## Structure and extensibility
The test runner consists of several main parts:
- `lt_platform`: JTAG/SWD platform communication.
- `lt_openocd_launcher`: OpenOCD subprocess handler.
- `lt_environment_tools`: various helper functions.
- `lt_test_runner`: main class which actually do all the test steps (flashing, reading from serial...).

If you want to add a new platform, you:
1. Create a new file named `lt_platform_name.py`. If a file for the platform family already exists, use that. For example, all STM32-based platforms should be in `lt_platform_stm32.py`.
2. Derive from the `lt_platform` class and override all neccessary functions.
3. Create a new entry in the `adapter_mapping.csv` file.
4. Add ID-class mapping to the `lt_platform_factory`. Use the string you defined in the `adapter_mapping`.

The platform is an universal interface to access platforms, so the same code in the `lt_test_runner` can be used for as many platforms as possible (classic polymorphism). For example, if `lt_test_runner` needs to flash the target, it calls the `load_elf` method of the `lt_platform` and depending on which platform is actually hidden behind the abstract class, different commands are sent to the OpenOCD.

If you want to modify what steps are done in the test routine, you need to create a new class which
derives from the `lt_test_runner`. In the derived class, override the `run` method with your own code.
You can use any methods the `lt_platform` class provides: flashing, restarting, LED blinking...

