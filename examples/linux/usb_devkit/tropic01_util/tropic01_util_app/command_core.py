from __future__ import annotations
"""Shared command abstraction and generic protobuf response handling."""

import argparse
from dataclasses import dataclass
from typing import Callable
from .protobuf import usb_devkit_messages_pb2 as pb


@dataclass(frozen=True)
class ApplicationCommandSpec:
    """Per-command hooks used by the generic CLI flow."""
    name: str # Command name used in CLI by the user.
    help_text: str # Help text do display in CLI.
    response_type: str # Identifier of the specific oneof field in the protobuf message.
    add_arguments: Callable[[argparse.ArgumentParser], None] # Function to add command arguments.
    build_app_cmd_from_args: Callable[[argparse.Namespace], pb.AppCmd] # Function to build the command from CLI args.
    decode_app_resp: Callable[[pb.AppResp], int] # Function to decode the response.


def process_and_print_resp(payload: bytes, app_cmd_spec: ApplicationCommandSpec) -> int:
    """Decode UsbDevkitResp envelope, then delegate command-specific decoding."""
    resp = pb.UsbDevkitResp()
    resp.ParseFromString(payload)

    top = resp.WhichOneof("type")
    if top == "error":
        err_name = pb.ErrorRespCode.Name(resp.error.res_code)
        print(f"Device error response: {err_name}")
        return 1

    if top != "app":
        print(f"Unexpected top-level response type: {top}")
        return 1

    app_type = resp.app.WhichOneof("type")
    if app_type != app_cmd_spec.response_type:
        print(f"Unexpected app response type: {app_type}")
        return 1

    status = app_cmd_spec.decode_app_resp(resp.app)

    if resp.app.HasField("libtropic_error_code"):
        print(f"libtropic_error_code: {resp.app.libtropic_error_code}")

    return status
