#!/usr/bin/env python3
"""TROPIC01 utility application for communication with the USB DevKit via application commands."""

from __future__ import annotations

import argparse
import sys

from tropic01_util_app.command_core import process_and_print_resp
from tropic01_util_app.commands import COMMAND_SPECS
from tropic01_util_app.frame_protocol import send_and_receive
from tropic01_util_app.protobuf import usb_devkit_messages_pb2 as pb

def main() -> int:
	"""CLI entrypoint: parse args, dispatch command, transport frame, decode response."""
	parser = argparse.ArgumentParser(description="TROPIC01 USB DevKit App-command host utility")
	parser.add_argument("--port", default="/dev/ttyACM0", help="TROPIC01 USB Devkit serial port")
	parser.add_argument("--baudrate", type=int, default=115200, help="Serial baudrate")
	parser.add_argument(
		"--timeout", type=float, default=1.0, help="Serial read timeout in seconds"
	)

	subparsers = parser.add_subparsers(
		title="commands",
		description="Application commands to send to the USB Devkit.",
		dest="command",
		required=True
	)
	# Register each command from the central command table.
	for spec in COMMAND_SPECS.values():
		subparser = subparsers.add_parser(spec.name, help=spec.help_text)
		spec.add_arguments(subparser)

	args = parser.parse_args()
	spec = COMMAND_SPECS[args.command]

	try:
		cmd = pb.UsbDevkitCmd(app=spec.build_app_cmd_from_args(args))
		payload = cmd.SerializeToString()
		response_payload = send_and_receive(args.port, args.baudrate, args.timeout, payload)
		return process_and_print_resp(response_payload, spec)
	except ValueError as exc:
		print(str(exc), file=sys.stderr)
		return 2
	except Exception as exc:
		print(f"Error: {exc}", file=sys.stderr)
		return 1


if __name__ == "__main__":
	raise SystemExit(main())
