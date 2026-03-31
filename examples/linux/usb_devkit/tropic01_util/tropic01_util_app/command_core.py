from __future__ import annotations
"""Common CLI-command interface and app-command sender utilities."""

import argparse
from dataclasses import dataclass
import serial
from typing import Callable

from .frame_protocol import build_frame, read_response_frame
from .protobuf import usb_devkit_messages_pb2 as pb


@dataclass(frozen=True)
class CliCommandSpec:
    """Common command contract used by all CLI command implementations."""

    name: str
    help_text: str
    description: str
    add_arguments: Callable[[argparse.ArgumentParser], None]
    execute: Callable[[argparse.Namespace, "AppCommandSender"], int]


def decode_usb_devkit_app_resp(payload: bytes, expected_resp_type: str) -> pb.AppResp:
    """Decode UsbDevkitResp payload, validate the response type and return the enclosed AppResp."""
    resp = pb.UsbDevkitResp()
    resp.ParseFromString(payload)

    top = resp.WhichOneof("type")
    if top == "error":
        err_name = pb.ErrorRespCode.Name(resp.error.res_code)
        raise ValueError(f"Device error response: {err_name}")

    if top != "app":
        raise ValueError(f"Unexpected top-level response type: {top}")
    
    actual_response_type = resp.app.WhichOneof("type")
    if actual_response_type != expected_resp_type:
        raise ValueError(f"Unexpected app response type: {actual_response_type}")

    return resp.app


def print_libtropic_res_code(app_resp: pb.AppResp, command_name: str) -> None:
    """Print optional libtropic error code when present in AppResp."""
    if app_resp.HasField("libtropic_res_code"):
        print(f"{command_name} libtropic_res_code: {app_resp.libtropic_res_code}")


class AppCommandSender:
    """Sends one or many AppCmd requests within one CLI command."""

    def __init__(self, ser: serial.Serial):
        self._ser = ser

    def _send_and_receive_on_serial(self, payload: bytes) -> bytes:
        """Send one framed request using the configured open serial instance."""
        frame = build_frame(payload)
        self._ser.reset_input_buffer()
        self._ser.reset_output_buffer()
        self._ser.write(frame)
        self._ser.flush()
        return read_response_frame(self._ser)

    def send(self, app_cmd: pb.AppCmd, expected_resp_type: str) -> pb.AppResp:
        """Send one AppCmd and verify response oneof type."""
        payload = pb.UsbDevkitCmd(app=app_cmd).SerializeToString()
        response_payload = self._send_and_receive_on_serial(payload)
        app_resp = decode_usb_devkit_app_resp(response_payload, expected_resp_type)
        return app_resp