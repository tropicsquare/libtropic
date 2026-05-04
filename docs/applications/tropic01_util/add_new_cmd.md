# Add a New Command
The process of adding a new command is done in three phases:

1. [Declare Protobuf Messages](#declare-protobuf-messages) (Protobuf stands for [Protocol Buffers](https://protobuf.dev/))
2. [Extend USB DevKit Firmware](#extend-usb-devkit-firmware)
3. [Extend TROPIC01 Util](#extend-tropic01-util)

All of the following steps are done relative to the `libtropic/examples/applications/tropic01_util/` directory.

Before you start, complete [Install and Use](install_and_use.md) and then follow these steps to install the `protobuf-compiler`, which is needed when adding new Protobuf messages, as the Protobuf files have to be re-generated:
!!! example "Install Protobuf compiler"
    === ":fontawesome-brands-linux: Linux"
        1. Install:
            - Ubuntu/Debian: `sudo apt update && sudo apt install protobuf-compiler`
            - Fedora: `sudo dnf install protobuf-compiler`
        2. Ensure the version is 3+:
            - `protoc --version`
        3. Try to run it:
            - Build the USB DevKit firmware with additional CMake option — follow [these instructions](install_and_use.md#build-usb-devkit-firmware) and add the `-DGEN_PROTO_FILES=1` switch to `cmake`.
            - CMake automatically runs `protobuf-compiler`, generates Protobuf files in C (for the firmware) and Python (for the CLI application), and places them at the expected location — Check that it builds without any errors.
            - Because we haven't changed the Protobuf messages yet, `git` should not report any new Protobuf files.

    === ":fontawesome-brands-apple: macOS"
        1. Install:
            - [Homebrew](https://brew.sh/): `brew install protobuf`
        2. Ensure the version is 3+:
            - `protoc --version`
        3. Try to run it. TBA

    === ":fontawesome-brands-windows: Windows"
        1. Install:
            - [Winget](https://learn.microsoft.com/en-us/windows/package-manager/winget/): `winget install protobuf`
        2. Ensure the version is 3+:
            - `protoc --version`
        3. Try to run it. TBA

## Declare Protobuf Messages
We need to define two new Protobuf messages: one for the **command** (raw or application), and one for the **response**. This is done in the `protobuf/usb_devkit_messages.proto` file:

1. Decide whether the new command is **raw** (RawCmd) or **application** (AppCmd). Because the approach is same for both types, we will show how to add a new application command and application response.
2. Come up with a new name in format `<cmd_name_in_camel_case>Cmd`, for example `AddNumsAndPingCmd`. It will add two numbers and send the result to TROPIC01 using the Ping L3 command to showcase how to work with Libtropic in the TROPIC01 Util ecosystem.
3. Look for the `AppCmd` message declaration and add the new command:
    ```proto { hl_lines="6" }
    // =============================== AppCmd ================================
    message AppCmd {
        oneof type {
            PinSetCmd pin_set = 1;
            // <other commands>
            AddNumsAndPingCmd add_nums_and_ping = 2;
        }
    }
    ```
    - The number assigned to `add_nums_and_ping` is a [field number](https://protobuf.dev/programming-guides/proto3/#assigning), **not** a value. This number must be unique.
    - Always assign a field number of the previous command (in our case `pin_set`) plus one.
4. Declare `AddNumsAndPingCmd` below `AppCmd` and all other application commands:
    ```protobuf { .copy }
    // ------------------------------- AddNumsAndPingCmd
    message AddNumsAndPingCmd {
        int32 a_in = 1;
        int32 b_in = 2;
    }
    ```
    - `AddNumsAndPingCmd` may have some fields (e.g. the `a_in`) — see [available types](https://protobuf.dev/programming-guides/proto3/#scalar) in the Protobuf documentation (it is also valid to have no fields, like in `GetGpoCmd`). Again, use appropriate [field number](https://protobuf.dev/programming-guides/proto3/#assigning).
5. Look for the `AppResp` message declaration and add the new response `AddNumsAndPingResp`:
    ```proto { hl_lines="7" }
    // =============================== AppResp ===============================
    message AppResp {
        optional uint32 libtropic_res_code = 1;
        oneof type {
            PinSetResp pin_set = 2;
            // <other responses>
            AddNumsAndPingResp add_nums_and_ping = 3;
        }
    }
    ```
    - The number assigned to `add_nums_and_ping` is a [field number](https://protobuf.dev/programming-guides/proto3/#assigning), **not** a value. This number must be unique.
    - Always assign a field number of the previous response (in our case `pin_set`) plus one.
6. Declare `AddNumsAndPingResp` below `AppResp` and all other application responses, along with `AddNumsAndPingRespCode` enum (for result codes):
    ```proto { .copy }
    // ------------------------------- AddNumsAndPingResp
    message AddNumsAndPingResp {
        AddNumsAndPingRespCode res_code = 1;
        int32 c_out = 2;
    }
    enum AddNumsAndPingRespCode {
        ADD_NUMS_AND_PING_RESP_CODE_UNSPECIFIED = 0;
        ADD_NUMS_AND_PING_RESP_CODE_OK = 1;
        ADD_NUMS_AND_PING_RESP_CODE_ERROR = 2;
        ADD_NUMS_AND_PING_RESP_CODE_MSG_MISMATCH = 3;
        // other optional result codes
    }
    ```
    - Always declare the `res_code` field — that way, error reporting is unified across all responses.
    - `AddNumsAndPingRespCode`: always declare the `*_UNSPECIFIED` value (required by Protobuf) and the `*_OK` value to indicate success. Other values for the errors are not mandatory, but recommended (see other result code enums for inspiration).
    - `AddNumsAndPingResp` may have some fields (e.g. the `c_out`) — see [available types](https://protobuf.dev/programming-guides/proto3/#scalar) in the Protobuf documentation (it is also valid to have no fields, like in `GetGpoCmd`). Again, use appropriate [field number](https://protobuf.dev/programming-guides/proto3/#assigning).
7. Depending on the types you used for `AddNumsAndPingCmd` or `AddNumsAndPingResp` fields, you may need to add some constraints in the `protobuf/usb_devkit_messages.options` file:
    - This file is specific to [Nanopb](https://jpa.kapsi.fi/nanopb/), a plain-C implementation of Protobuf, targeted at embedded systems. We are using Nanopb in the USB DevKit Firmware because Protobuf does not support the C language.
    - See the [available options](https://jpa.kapsi.fi/nanopb/docs/reference.html#generator-options) in the Nanopb documentation. Typically, some size constraints should be used — feel free to get inspired by other command/responses.

## Extend USB DevKit Firmware
Before adding any new changes to the USB DevKit firmware, run the `protobuf-compiler` to validate the changes from above. As already mentioned, the firmware's CMake runs `protobuf-compiler` automatically when the `-DGEN_PROTO_FILES=1` switch is passed to it; follow [these instructions](install_and_use.md#build-usb-devkit-firmware) and pass the switch to `cmake`. After this, the files inside `protobuf/generated/` should be updated with your changes from above.

Now, modify the firmware:

1. Create a new header file `add_nums_and_ping.h` for the command callback function in:
    - `firmware/Inc/app_cmd/`, if it's an application command (our case),
    - `firmware/Inc/raw_cmd/`, if it's a raw command.
2. The contents of `add_nums_and_ping.h` should look like this:
    ```c { .copy }
    #ifndef ADD_NUMS_AND_PING_H
    #define ADD_NUMS_AND_PING_H

    // This is where the Protobuf messages are declared.
    #include "usb_devkit_messages.pb.h"

    void add_nums_and_ping(const AddNumsAndPingCmd *cmd, AppResp *resp);

    #endif  // ADD_NUMS_AND_PING_H
    ```
3. Create a new source file `add_nums_and_ping.c` for the command callback function in: 
    - `firmware/Src/app_cmd/`, if it's an application command (our case),
    - `firmware/Src/raw_cmd/`, if it's a raw command.

    After that, add the file to `firmware/CMakeLists.txt`:
    ```cmake { hl_lines="5" }
    # Add sources to executable
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        # Project sources
        # <other sources in app_cmd/>
        ${CMAKE_CURRENT_SOURCE_DIR}/Src/app_cmd/add_nums_and_ping.c
    )
    ```
4. Implement the command callback function — the contents of `add_nums_and_ping.c` should look like this:
    ```c { .copy }
    #include "app_cmd/add_nums_and_ping.h"

    #include "libtropic.h"
    #include "libtropic_common.h"
    #include "main.h"
    #include "usb_devkit_messages.pb.h"

    void add_nums_and_ping(const AddNumsAndPingCmd *cmd, AppResp *resp)
    {
        // Add the numbers.
        resp->type.add_nums_and_ping.c_out = cmd->a_in + cmd->b_in;

        // Ping TROPIC01.
        uint8_t msg_out[sizeof(resp->type.add_nums_and_ping.c_out)];
        uint8_t msg_in[sizeof(resp->type.add_nums_and_ping.c_out)];
        memcpy(msg_out, &resp->type.add_nums_and_ping.c_out, sizeof(msg_out));
        lt_ret_t lt_ret = lt_ping(&lt_handle, msg_out, msg_in, sizeof(msg_out));

        // Check libtropic result code.
        if (lt_ret != LT_OK) {
            resp->type.add_nums_and_ping.res_code = ADD_NUMS_AND_PING_RESP_CODE_ERROR;
            resp->has_libtropic_res_code = true;
            resp->libtropic_res_code = lt_ret;
            return;
        }
        // Check if the incoming message is the same as the outgoing.
        if (0 != memcmp(msg_out, msg_in, sizeof(msg_out))) {
            resp->type.add_nums_and_ping.res_code = ADD_NUMS_AND_PING_RESP_CODE_MSG_MISMATCH;
            return;
        }

        resp->type.add_nums_and_ping.res_code = ADD_NUMS_AND_PING_RESP_CODE_OK;
    }
    ```

    - `resp->has_libtropic_res_code` is initialized to `false` before `add_nums_and_ping()` is called. Other fields in `resp` are also initialized to their default values.

5. Call `add_nums_and_ping()` in:

    - `process_app_cmd()` function inside `firmware/Src/app_cmd/app_cmd_common.c`, if it's an application command (our case),
    - `process_raw_cmd()` function inside `firmware/Src/raw_cmd/raw_cmd_common.c`, if it's a raw command.
    
    To do that, add a new `case` to the `switch()` (before the `default` case):
    ```c { hl_lines="1 7-11" }
    #include "app_cmd/add_nums_and_ping.h" // Put it to the top of the file.

    void process_app_cmd(const UsbDevkitCmd *cmd, UsbDevkitResp *resp)
    {
        switch (cmd->type.app.which_type) {
            // <other cases>
            case AppCmd_add_nums_and_ping_tag:
                resp->type.app.which_type = AppResp_add_nums_and_ping_tag;
                add_nums_and_ping(&cmd->type.app.type.add_nums_and_ping,
                                  &resp->type.app);
                break;

            default:
                resp->which_type = UsbDevkitResp_error_tag;
                resp->type.error.res_code = ERROR_RESP_CODE_UNKNOWN_CMD;
                break;
        }
    }
    ```

6. Re-build the firmware in `firmware/build/` and check there are no errors (no need to run `cmake` again, just run the generator, e.g. `make`).
7. Flash the modified firmware into the USB DevKit — follow the instructions in [Flash USB DevKit Firmware](install_and_use.md#flash-usb-devkit-firmware).

## Extend TROPIC01 Util
At this moment, TROPIC01 Util supports only application commands, so this step applies only to them.

Add support for the new application command:

1. Create a new file `add_nums_and_ping.py` in `tropic01_util_app/commands/`. In it, we will implement the CLI command that will issue the `AddNumsAndPingCmd` USB DevKit application command.
2. At the top of `add_nums_and_ping.py`, handle imports:
    ```py { .copy }
    from __future__ import annotations
    """add-nums-and-ping command."""

    import argparse

    from ..command_core import AppCommandSender, CliCommandSpec, print_libtropic_res_code
    from protobuf.generated import usb_devkit_messages_pb2 as pb
    ```
3. In the end of `add_nums_and_ping.py`, create a new instance of `CliCommandSpec` — this defines some basic properties of the CLI command:
    ```py { .copy }
    ADD_NUMS_AND_PING_SPEC = CliCommandSpec(
        name="add-nums-and-ping",
        help_text="Adds two integers and pings TROPIC01.",  # Short help text.
        description="Adds two 32-bit integers and sends them to TROPIC01 via the Ping L3 command.",  # Detailed description.
        add_arguments=add_arguments,  # Callback to add arguments to argparse.
        execute=execute  # Callback to execute the command.
    )
    ```
    - For more details about the `CliCommandSpec` class, look into `tropic01_util_app/command_core.py`.
4. In the middle of `add_nums_and_ping.py`, implement `add_arguments()` callback:
    ```py { .copy }
    def add_arguments(parser: argparse.ArgumentParser) -> None:
        parser.add_argument(
            "--a",
            type=int,
            required=True,
            help="First addend A.",
        )
        parser.add_argument(
            "--b",
            type=int,
            required=True,
            help="Second addend B.",
        )
    ```
5. In the middle of `add_nums_and_ping.py`, implement `execute()` callback:
    ```py { .copy }
    def execute(args: argparse.Namespace,
                app_cmd_sender: AppCommandSender) -> int:
        # Construct the AddNumsAndPingCmd to send.
        app_cmd = pb.AppCmd()
        app_cmd.add_nums_and_ping.a_in = args.a
        app_cmd.add_nums_and_ping.b_in = args.b
        
        # Send the command.
        app_resp = app_cmd_sender.send(
            app_cmd=app_cmd,
            expected_resp_type="add_nums_and_ping"
        )

        # Check the result code and print result.
        res_code = app_resp.add_nums_and_ping.res_code
        print(f"{ADD_NUMS_AND_PING_SPEC.name} result: {pb.AddNumsAndPingRespCode.Name(res_code)}")
        print_libtropic_res_code(app_resp, ADD_NUMS_AND_PING_SPEC.name) # Prints only if there is some
        if res_code == pb.ADD_NUMS_AND_PING_RESP_CODE_OK:
            print(f"{ADD_NUMS_AND_PING_SPEC.name} addition: {app_resp.add_nums_and_ping.c_out}")
            return 0
        return 1
    ```
6. In `tropic01_util_app/commands/__init__.py`, register the new CLI command:
    ```py { hl_lines="5 10" }
    """Registry of available CLI commands."""

    # Import new commands here:
    # <other command imports>
    from .add_nums_and_ping import ADD_NUMS_AND_PING_SPEC

    # Add new commands here:
    COMMAND_SPECS = {
        # <other commands>
        ADD_NUMS_AND_PING_SPEC.name: ADD_NUMS_AND_PING_SPEC
    }
    ```

After these steps, you should be able to execute the new command using `tropic01_util.py`.