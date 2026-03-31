from __future__ import annotations
"""get-rand-bytes command."""

import argparse
from pathlib import Path

from ..command_core import AppCommandSender, CliCommandSpec, print_libtropic_res_code
from ..protobuf import usb_devkit_messages_pb2 as pb

MAX_GET_RAND_BYTES_COUNT = 255


def build_get_rand_bytes_cmd(count: int) -> pb.AppCmd:
    cmd = pb.AppCmd()
    cmd.get_random_bytes.count = count
    return cmd


def _emit_data(data: bytes, data_file: str | None) -> None:
    if data_file is not None:
        Path(data_file).write_bytes(data)
        print(f"get-rand-bytes wrote {len(data)} bytes to: {data_file}")
        return

    print(f"get-rand-bytes data_len: {len(data)}")
    print(f"get-rand-bytes data_hex: {data.hex()}")


def add_arguments(parser: argparse.ArgumentParser) -> None:
    parser.add_argument(
        "--count",
        type=int,
        required=True,
        help="Number of random bytes to read",
    )
    parser.add_argument(
        "--output-file",
        default=None,
        help="Optional output file path for random bytes. If omitted, output is redirected to stdout.",
    )


def execute(args: argparse.Namespace, app_cmd_sender: AppCommandSender) -> int:
    if args.count <= 0:
        raise ValueError("--count must be > 0")

    remaining = args.count
    collected = bytearray()

    while remaining > 0:
        chunk_count = min(remaining, MAX_GET_RAND_BYTES_COUNT)
        app_resp = app_cmd_sender.send(
            build_get_rand_bytes_cmd(chunk_count),
            expected_resp_type="get_random_bytes",
        )
        code = app_resp.get_random_bytes.res_code

        if code != pb.GET_RAND_BYTES_RESP_CODE_OK:
            print(f"get-rand-bytes result: {pb.GetRandBytesRespCode.Name(code)}")
            print_libtropic_res_code(app_resp, GET_RAND_BYTES_SPEC.name)
            print(
                f"get-rand-bytes summary: requested={args.count}, received={len(collected)}"
            )
            return 1

        chunk_data = bytes(app_resp.get_random_bytes.random_bytes)
        collected.extend(chunk_data)
        remaining -= chunk_count

    print(f"get-rand-bytes result: {pb.GetRandBytesRespCode.Name(pb.GET_RAND_BYTES_RESP_CODE_OK)}")
    print(
        f"get-rand-bytes summary: requested={args.count}, received={len(collected)}"
    )
    _emit_data(bytes(collected), args.output_file)
    return 0


GET_RAND_BYTES_SPEC = CliCommandSpec(
    name="get-rand-bytes",
    help_text="Get random bytes from TROPIC01 TRNG2",
    description="Gets random bytes from TROPIC01 True Random Number Generator 2.",
    add_arguments=add_arguments,
    execute=execute,
)
