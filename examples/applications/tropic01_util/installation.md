# USB DevKit FW
## Required tools/packages
- `cmake`,
- `make` or `ninja-build` (build instructions use `ninja` for faster builds),
- GCC cross-compiler for ARM:
    - `gcc-arm-none-eabi` (Ubuntu/Debian),
    - `arm-none-eabi-gcc` (Fedora),
- `dfu-util` for flashing the FW.

If the Protocol Buffers files have to be re-generated (needed when either `protobuf/usb_devkit_messages.proto` or `protobuf/usb_devkit_messages.options` changes), then these are needed:
- `protobuf-compiler` (system install),
- `mypy-protobuf` (Python package; complete [these](#tropic01-util-python-cli-application) steps first).

## Build
1. `cd firmware/`
2. `mkdir build/`
3. `cd build/`
4. `cmake -G Ninja ..` (add `-DGEN_PROTO_FILES` if the Protocol Buffers files have to be re-generated - needed when either `protobuf/usb_devkit_messages.proto` or `protobuf/usb_devkit_messages.options` changes)
5. `ninja`

## Upload
1. Eject USB DevKit
2. Press its button and hold
3. Inject USB DevKit
4. Release button (LED should be off)
5. `dfu-util -a 0 -s 0x08000000:leave -D firmware/build/tropic01_util.bin`
6. LED should turn on after a few seconds


# Tropic01 Util (Python CLI application)
1. Create Python Virtual Environment (don't use Python 3.14, there are problems with some packages): `python3 -m venv .venv`
2. Activate `.venv`: `source .venv/bin/activate`
3. Upgrade pip: `pip install --upgrade pip`
4. Install dependencies: `pip install -r tropic01_util_app/requirements.txt`
5. Start using Tropic01 Util: `./tropic01_util.py -h`
