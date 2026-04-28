from __future__ import annotations
"""r-mem-write command."""

import argparse
from pathlib import Path
import sys

from ..command_core import AppCommandSender, CliCommandSpec, print_libtropic_res_code
from protobuf.generated import usb_devkit_messages_pb2 as pb
from ..utils import parse_hex_bytes
from .r_mem_common import (
    MAX_R_MEM_CHUNK_SIZE,
    R_MEM_SLOT_COUNT,
    R_MEM_TOTAL_CAPACITY
)

def build_r_mem_write_cmd(slot: int, data: bytes) -> pb.AppCmd:
    if len(data) == 0:
        raise ValueError("r-mem-write data must not be empty")
    if len(data) > MAX_R_MEM_CHUNK_SIZE:
        raise ValueError(
            f"r-mem-write data too long: {len(data)} bytes (max {MAX_R_MEM_CHUNK_SIZE})"
        )

    cmd = pb.AppCmd()
    cmd.r_mem_write.udata_slot = slot
    cmd.r_mem_write.data = data
    return cmd

def parse_r_mem_write_data_args(args: argparse.Namespace) -> bytes:
    if args.data_hex is not None:
        data = parse_hex_bytes(args.data_hex)
        if data is None:
            raise ValueError("--data-hex parsing failed")
        return data

    return Path(args.data_file).read_bytes()

def decode_r_mem_write_resp(resp: pb.AppResp) -> int:
    code = resp.r_mem_write.res_code
    print(f"r-mem-write result: {pb.RMemWriteRespCode.Name(code)}")
    return 0 if code == pb.R_MEM_WRITE_RESP_CODE_OK else 1


def add_arguments(parser: argparse.ArgumentParser) -> None:
    parser.add_argument(
        "--slot",
        type=int,
        default=None,
        help="Optional R-Memory user-data slot. If omitted, write starts at slot 0 and spans multiple slots if needed.",
    )

    data_group = parser.add_mutually_exclusive_group(required=True)
    data_group.add_argument(
        "--data-hex",
        default=None,
        help="Data to write as hex bytes, e.g. deadbeef.",
    )
    data_group.add_argument(
        "--data-file",
        default=None,
        help="Path to file to write.",
    )


def execute(args: argparse.Namespace, app_cmd_sender: AppCommandSender) -> int:
    """Execute r_mem_write command in single-slot or multi-slot mode."""
    data = parse_r_mem_write_data_args(args)

    if args.slot is not None:
        # Explicit slot mode: write exactly one slot.
        cmd = build_r_mem_write_cmd(args.slot, data)
        app_resp = app_cmd_sender.send(cmd, expected_resp_type="r_mem_write")
        status = decode_r_mem_write_resp(app_resp)
        print_libtropic_res_code(app_resp, R_MEM_WRITE_SPEC.name)
        return status

    # Slot omitted: write from slot 0 and use as many slots as needed.
    if len(data) > R_MEM_TOTAL_CAPACITY:
        raise ValueError(
            f"Input data too large: {len(data)} bytes, "
            f"R-Memory capacity is {R_MEM_TOTAL_CAPACITY} bytes "
            f"({R_MEM_SLOT_COUNT} slots x {MAX_R_MEM_CHUNK_SIZE} bytes)"
        )

    chunks = [data[i : i + MAX_R_MEM_CHUNK_SIZE] for i in range(0, len(data), MAX_R_MEM_CHUNK_SIZE)]
    end_slot = len(chunks) - 1

    print(
        f"r-mem-write: {len(data)} bytes -> {len(chunks)} chunks, "
        f"slots 0..{end_slot}"
    )

    written_count = 0
    for idx, chunk in enumerate(chunks):
        slot = idx

        write_resp = app_cmd_sender.send(
            build_r_mem_write_cmd(slot, chunk),
            expected_resp_type="r_mem_write",
        )
        write_code = write_resp.r_mem_write.res_code
        if write_code != pb.R_MEM_WRITE_RESP_CODE_OK:
            sys.stdout.write("\n")
            print(f"r-mem-write result: {pb.RMemWriteRespCode.Name(write_code)}")
            print_libtropic_res_code(write_resp, R_MEM_WRITE_SPEC.name)
            print(f"r-mem-write failed at slot {slot}")
            print(
                f"r-mem-write summary: written_chunks={written_count}/{len(chunks)}, failed_slot={slot}"
            )
            return 1

        written_count += 1
        sys.stdout.write(f"\rr-mem-write progress: {idx + 1}/{len(chunks)}")
        sys.stdout.flush()

    sys.stdout.write("\n")
    print(f"r-mem-write summary: written_chunks={written_count}/{len(chunks)}, bytes={len(data)}")

    return 0


R_MEM_WRITE_SPEC = CliCommandSpec(
    name="r-mem-write",
    help_text="Write slot(s) in the User Data partition in R-Memory",
    description="Write one slot (--slot) or starting from slot 0 across multiple slots (without --slot) in the User Data partition in TROPIC01 R-Memory.",
    add_arguments=add_arguments,
    execute=execute,
)
