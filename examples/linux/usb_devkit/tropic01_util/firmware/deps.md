Default firmware build:
- no protobuf tools required

Development mode (regenerate protobuf from `.proto`):
- `protobuf-compiler` (system install)
- optional: `mypy-protobuf` (for Python `.pyi` generation)

Enable development mode with:
- `-DLT_PROTO_GENERATE=ON`