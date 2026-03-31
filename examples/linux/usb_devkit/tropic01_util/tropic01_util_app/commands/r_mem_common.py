from __future__ import annotations
"""Shared helpers for R-Memory CLI commands."""

from ..protobuf import usb_devkit_messages_pb2 as pb

MAX_R_MEM_CHUNK_SIZE = 475
# TROPIC01 supports user-data slots 0..511 (512 total).
R_MEM_SLOT_COUNT = 512
R_MEM_TOTAL_CAPACITY = R_MEM_SLOT_COUNT * MAX_R_MEM_CHUNK_SIZE
