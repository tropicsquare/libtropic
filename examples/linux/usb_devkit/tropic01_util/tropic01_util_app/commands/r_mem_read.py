from __future__ import annotations
"""r-mem-read command."""

import argparse
from pathlib import Path
import sys

from ..command_core import AppCommandSender, CliCommandSpec, print_libtropic_res_code
from ..protobuf import usb_devkit_messages_pb2 as pb
from .r_mem_common import MAX_R_MEM_CHUNK_SIZE, R_MEM_SLOT_COUNT

def build_r_mem_read_cmd(slot: int) -> pb.AppCmd:
    cmd = pb.AppCmd()
    cmd.r_mem_read.udata_slot = slot
    return cmd

def decode_r_mem_read_resp_with_output(resp: pb.AppResp, output_file: str | None) -> int:
    code = resp.r_mem_read.res_code
    print(f"r-mem-read result: {pb.RMemReadRespCode.Name(code)}")
    if code != pb.R_MEM_READ_RESP_CODE_OK:
        return 1

    data = bytes(resp.r_mem_read.data)
    print(f"r-mem-read data_len: {len(data)}")
    print(f"r-mem-read data_hex: {data.hex()}")

    if output_file is not None:
        Path(output_file).write_bytes(data)
        print(f"r-mem-read wrote {len(data)} bytes to: {output_file}")

    return 0


def _read_slot(app_cmd_sender: AppCommandSender, slot: int) -> pb.AppResp:
    """Read one slot and return raw AppResp."""
    cmd = build_r_mem_read_cmd(slot)
    return app_cmd_sender.send(cmd, expected_resp_type="r_mem_read")


def _emit_data(data: bytes, output_file: str | None) -> None:
    """Write read data to file or print to stdout."""
    if output_file is not None:
        Path(output_file).write_bytes(data)
        print(f"r-mem-read wrote {len(data)} bytes to: {output_file}")
        return

    print(f"r-mem-read data_len: {len(data)}")
    print(f"r-mem-read data_hex: {data.hex()}")


def add_arguments(parser: argparse.ArgumentParser) -> None:
    parser.add_argument(
        "--slot",
        type=int,
        default=None,
        help="Optional slot to read. If omitted, read starts at slot 0 and continues until no more data available.",
    )
    parser.add_argument(
        "--size",
        type=int,
        default=None,
        help="Optional number of continous bytes to read. If omitted, reads until no more continous data available.",
    )
    parser.add_argument(
        "--data-file",
        default=None,
        help="Optional output file path for read data. If omitted, output is redirected to stdout.",
    )


def execute(args: argparse.Namespace, app_cmd_sender: AppCommandSender) -> int:
    """Execute r_mem_read in single-slot or multi-slot mode."""
    if args.size is not None and args.size <= 0:
        raise ValueError("--size must be > 0")

    # Single-slot mode.
    if args.slot is not None:
        app_resp = _read_slot(app_cmd_sender, args.slot)
        code = app_resp.r_mem_read.res_code
        data = bytes(app_resp.r_mem_read.data)
        print(f"r-mem-read result: {pb.RMemReadRespCode.Name(code)}")
        print_libtropic_res_code(app_resp, R_MEM_READ_SPEC.name)
        if code == pb.R_MEM_READ_RESP_CODE_ERROR:
            return 1
        if code == pb.R_MEM_READ_RESP_CODE_SLOT_EMPTY:
            data = b""

        if args.size is not None:
            if len(data) < args.size:
                print(
                    f"WARNING: Requested --size {args.size}, but slot {args.slot} contains {len(data)} bytes. Returning partial data."
                )
            else:
                data = data[: args.size]

        _emit_data(data, args.data_file)
        return 0

    # Multi-slot mode from slot 0.
    target_size = args.size
    collected = bytearray()
    read_slots = 0
    progress_line_open = False

    result_printed = False
    for slot in range(R_MEM_SLOT_COUNT):
        app_resp = _read_slot(app_cmd_sender, slot)
        code = app_resp.r_mem_read.res_code
        slot_data = bytes(app_resp.r_mem_read.data)

        if not result_printed:
            initial_code = code
            if code == pb.R_MEM_READ_RESP_CODE_OK and len(slot_data) == 0:
                initial_code = pb.R_MEM_READ_RESP_CODE_SLOT_EMPTY
            print(f"r-mem-read result: {pb.RMemReadRespCode.Name(initial_code)}")
            result_printed = True

        if code == pb.R_MEM_READ_RESP_CODE_ERROR:
            sys.stdout.write("\n")
            print_libtropic_res_code(app_resp, R_MEM_READ_SPEC.name)
            print(f"r-mem-read failed at slot {slot}: {pb.RMemReadRespCode.Name(code)}")
            return 1
        if code == pb.R_MEM_READ_RESP_CODE_SLOT_EMPTY or len(slot_data) == 0:
            break

        if target_size is None:
            collected.extend(slot_data)
        else:
            remaining = target_size - len(collected)
            if remaining <= 0:
                break
            collected.extend(slot_data[:remaining])

        read_slots += 1
        sys.stdout.write(
            f"\rr-mem-read progress: slots={read_slots} bytes={len(collected)}"
        )
        sys.stdout.flush()
        progress_line_open = True

        # Without explicit size, continue reading contiguous slots until slot empty.
        if target_size is not None and len(collected) >= target_size:
            break

    if not result_printed:
        print(f"r-mem-read result: {pb.RMemReadRespCode.Name(pb.R_MEM_READ_RESP_CODE_SLOT_EMPTY)}")

    if progress_line_open:
        sys.stdout.write("\n")
        progress_line_open = False

    if target_size is not None and len(collected) < target_size:
        print(
            f"r-mem-read warning: Requested --size {target_size}, but {len(collected)} bytes are available. Returning partial data."
        )

    print(f"r-mem-read summary: slots={read_slots}, bytes={len(collected)}")

    _emit_data(bytes(collected), args.data_file)
    return 0


R_MEM_READ_SPEC = CliCommandSpec(
    name="r-mem-read",
    help_text="Read slot(s) from the User Data partition in R-Memory",
    description="Reads one slot (--slot) or all slots (without --slot) with optional read size (--size) from the User Data partition in TROPIC01 R-Memory.",
    add_arguments=add_arguments,
    execute=execute,
)
