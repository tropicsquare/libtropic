#!/usr/bin/env python3
"""TROPIC01 utility application for communication with the USB DevKit via application commands."""

from __future__ import annotations

import argparse
import sys
import serial

from tropic01_util_app.command_core import AppCommandSender
from tropic01_util_app.commands import COMMAND_SPECS

def main() -> int:
	"""CLI entrypoint: parse args, dispatch command, transport frame, decode response."""
	parser = argparse.ArgumentParser(
		description="TROPIC01 USB DevKit App-command host utility. WARNING: This utility is for basic demonstration purposes and shall not be used in production.",
		formatter_class=argparse.ArgumentDefaultsHelpFormatter
	)

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
		subparser = subparsers.add_parser(spec.name, help=spec.help_text, description=spec.description)
		spec.add_arguments(subparser)

	args = parser.parse_args()
	spec = COMMAND_SPECS[args.command]

	try:
		with serial.Serial(port=args.port, baudrate=args.baudrate, timeout=args.timeout) as ser:
			app_cmd_sender = AppCommandSender(ser)
			return spec.execute(args, app_cmd_sender)
	except ValueError as exc:
		print(str(exc), file=sys.stderr)
		return 2
	except Exception as exc:
		print(f"Error: {exc}", file=sys.stderr)
		return 1


if __name__ == "__main__":
	raise SystemExit(main())
