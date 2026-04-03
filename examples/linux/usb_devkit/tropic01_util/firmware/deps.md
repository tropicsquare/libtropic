Default firmware build:
- no protobuf tools required
- uses committed files in `../protobuf/generated/`

Development mode (regenerate protobuf from `.proto`):
- `protobuf-compiler` (system install)
- `mypy-protobuf` (required for Python `.pyi` generation)
- outputs generated files into `../protobuf/generated/`

Enable development mode with:
- `-DGEN_PROTO_FILES=ON`