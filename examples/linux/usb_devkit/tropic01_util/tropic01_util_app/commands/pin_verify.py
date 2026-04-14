from __future__ import annotations
"""pin-verify command: CLI args, request build, and response formatting."""

import argparse

from ..command_core import AppCommandSender, CliCommandSpec, print_libtropic_res_code
from ..utils import parse_hex_bytes
from protobuf.generated import usb_devkit_messages_pb2 as pb


def add_arguments(parser: argparse.ArgumentParser) -> None:
    """Register pin-verify specific CLI flags."""
    parser.add_argument("--pin", required=True, help="PIN string.")
    parser.add_argument(
        "--additional-data-hex",
        default=None,
        help="Optional additional data as hex, e.g. deadbeef.",
    )


def build_cmd_from_args(args: argparse.Namespace) -> pb.AppCmd:
    """Convert parsed CLI args into a UsbDevkitCmd for pin-verify."""
    # 1. Get user input.
    additional_data = parse_hex_bytes(args.additional_data_hex)

    # 2. Construct command.
    cmd = pb.AppCmd()
    cmd.pin_verify.pin = args.pin
    if additional_data is not None:
        cmd.pin_verify.additional_data = additional_data

    return cmd


def decode_resp(resp: pb.AppResp) -> int:
    """Print pin-verify response summary and optional key on success."""
    print(f"{PIN_VERIFY_SPEC.name} result: {pb.PinVerifyRespCode.Name(resp.pin_verify.res_code)}")
    if resp.pin_verify.res_code == pb.PIN_VERIFY_RESP_CODE_OK:
        print(f"{PIN_VERIFY_SPEC.name} crypto_key: {resp.pin_verify.crypto_key.hex()}")

    return 0


def execute(args: argparse.Namespace, app_cmd_sender: AppCommandSender) -> int:
    """Execute pin-verify command and validate the pin_verify response type."""
    cmd = build_cmd_from_args(args)
    app_resp = app_cmd_sender.send(cmd, expected_resp_type="pin_verify")
    status = decode_resp(app_resp)
    print_libtropic_res_code(app_resp, PIN_VERIFY_SPEC.name)
    return status


PIN_VERIFY_DESC = "Verifies the PIN using the TROPIC01 MAC-And-Destroy feature."\
                  " Upon success, returns the cryptographic key (guarded by the PIN)."\
                  " Note: A successful attempt after aprox. 7 wrong attempts might result in a timeout " \
                  "because the reinitialization of destroyed slots took too long - just increase the timeout " \
                  "with --timeout. For a reference, reinitializing maximum allowed amount of slots (128) takes " \
                  "around 20 seconds."

PIN_VERIFY_SPEC = CliCommandSpec(
    name="pin-verify",
    help_text="Verify PIN (utilizing MAC-And-Destroy feature)",
    description=PIN_VERIFY_DESC,
    add_arguments=add_arguments,
    execute=execute,
)
