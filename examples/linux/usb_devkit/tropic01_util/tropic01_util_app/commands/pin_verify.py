from __future__ import annotations
"""pin-verify command: CLI args, request build, and response formatting."""

import argparse

from ..command_core import ApplicationCommandSpec
from ..utils import parse_hex_bytes
from ..protobuf import usb_devkit_messages_pb2 as pb


def add_arguments(parser: argparse.ArgumentParser) -> None:
    """Register pin-verify specific CLI flags."""
    parser.add_argument("--pin", required=True, help="PIN string")
    parser.add_argument(
        "--additional-data-hex",
        default=None,
        help="Optional additional data as hex, e.g. deadbeef",
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


PIN_VERIFY_SPEC = ApplicationCommandSpec(
    name="pin-verify",
    help_text="Verify PIN (utilizing MAC-And-Destroy feature)",
    response_type="pin_verify",
    add_arguments=add_arguments,
    build_app_cmd_from_args=build_cmd_from_args,
    decode_app_resp=decode_resp,
)
