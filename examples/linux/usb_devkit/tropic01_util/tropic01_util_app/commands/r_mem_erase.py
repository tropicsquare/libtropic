from __future__ import annotations
"""r-mem-erase command."""

import argparse
import sys

from ..command_core import AppCommandSender, CliCommandSpec, print_libtropic_res_code
from protobuf.generated import usb_devkit_messages_pb2 as pb
from .r_mem_common import R_MEM_SLOT_COUNT

def build_r_mem_erase_cmd(slot: int) -> pb.AppCmd:
    cmd = pb.AppCmd()
    cmd.r_mem_erase.udata_slot = slot
    return cmd

def decode_r_mem_erase_resp(resp: pb.AppResp) -> int:
    code = resp.r_mem_erase.res_code
    print(f"r-mem-erase result: {pb.RMemEraseRespCode.Name(code)}")
    return 0 if code == pb.R_MEM_ERASE_RESP_CODE_OK else 1


def add_arguments(parser: argparse.ArgumentParser) -> None:
    parser.add_argument(
        "--slot",
        type=int,
        default=None,
        help="Optional slot to erase. If omitted, erase all R-Memory user-data slots.",
    )


def execute(args: argparse.Namespace, app_cmd_sender: AppCommandSender) -> int:
    """Execute r_mem_erase in single-slot or full-memory mode."""
    if args.slot is not None:
        cmd = build_r_mem_erase_cmd(args.slot)
        app_resp = app_cmd_sender.send(cmd, expected_resp_type="r_mem_erase")
        status = decode_r_mem_erase_resp(app_resp)
        print_libtropic_res_code(app_resp, R_MEM_ERASE_SPEC.name)
        return status

    print(f"r-mem-erase: erasing all {R_MEM_SLOT_COUNT} user-data slots")
    erased_count = 0
    for slot in range(R_MEM_SLOT_COUNT):
        cmd = build_r_mem_erase_cmd(slot)
        app_resp = app_cmd_sender.send(cmd, expected_resp_type="r_mem_erase")
        code = app_resp.r_mem_erase.res_code
        if code != pb.R_MEM_ERASE_RESP_CODE_OK:
            # Finish the in-place progress line before reporting error details.
            sys.stdout.write("\n")
            print(f"r-mem-erase result: {pb.RMemEraseRespCode.Name(code)}")
            print_libtropic_res_code(app_resp, R_MEM_ERASE_SPEC.name)
            print(f"r-mem-erase failed at slot {slot}")
            print(
                f"r-mem-erase summary: erased={erased_count}/{R_MEM_SLOT_COUNT}, failed_slot={slot}"
            )
            return 1

        erased_count += 1
        sys.stdout.write(
            f"\rr-mem-erase progress: {slot + 1}/{R_MEM_SLOT_COUNT}"
        )
        sys.stdout.flush()

    sys.stdout.write("\n")
    print(f"r-mem-erase summary: erased={erased_count}/{R_MEM_SLOT_COUNT}")
    return 0


R_MEM_ERASE_SPEC = CliCommandSpec(
    name="r-mem-erase",
    help_text="Erase slot(s) from the User Data partition in R-Memory",
    description="Erases one slot (--slot) or all slots (without --slot) in the User Data partition in TROPIC01 R-Memory.",
    add_arguments=add_arguments,
    execute=execute,
)
