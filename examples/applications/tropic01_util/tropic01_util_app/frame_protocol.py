from __future__ import annotations
"""USB DevKit framing transport helpers (magic, length, CRC16)."""

import serial


FRAME_MAGIC = bytes((0xAA, 0x55))
FRAME_RESPONSE_MAGIC = bytes((0x55, 0xAA))
FRAME_DATA_LEN_SIZE = 2
FRAME_CRC_SIZE = 2
FRAME_DATA_MAX_SIZE = 4093


def crc16_update(crc: int, data: bytes) -> int:
    """Update CRC16 with polynomial 0x8005, init 0x0000, no reflection."""
    for byte in data:
        crc ^= byte << 8
        for _ in range(8):
            if crc & 0x8000:
                crc = ((crc << 1) ^ 0x8005) & 0xFFFF
            else:
                crc = (crc << 1) & 0xFFFF
    return crc


def read_exact(ser: serial.Serial, size: int) -> bytes:
    """Read exactly size bytes or raise on timeout."""
    data = bytearray()
    while len(data) < size:
        chunk = ser.read(size - len(data))
        if not chunk:
            raise TimeoutError(f"Timed out while reading {size} bytes")
        data.extend(chunk)
    return bytes(data)


def build_frame(pb_payload: bytes) -> bytes:
    """Wrap a protobuf payload into the wire frame with CRC."""
    if len(pb_payload) == 0 or len(pb_payload) > FRAME_DATA_MAX_SIZE:
        raise ValueError(f"Invalid protobuf payload size: {len(pb_payload)}")

    data_len_be = len(pb_payload).to_bytes(FRAME_DATA_LEN_SIZE, "big")
    crc = crc16_update(0, data_len_be)
    crc = crc16_update(crc, pb_payload)
    crc_be = crc.to_bytes(FRAME_CRC_SIZE, "big")
    return FRAME_MAGIC + data_len_be + pb_payload + crc_be


def read_response_frame(ser: serial.Serial) -> bytes:
    """Read and validate one response frame, returning protobuf payload bytes."""
    prev = None
    while True:
        b = ser.read(1)
        if not b:
            raise TimeoutError("Timed out waiting for response magic")
        if prev == FRAME_RESPONSE_MAGIC[:1] and b == FRAME_RESPONSE_MAGIC[1:2]:
            break
        prev = b

    data_len_be = read_exact(ser, FRAME_DATA_LEN_SIZE)
    data_len = int.from_bytes(data_len_be, "big")
    if data_len == 0 or data_len > FRAME_DATA_MAX_SIZE:
        raise ValueError(f"Invalid response data length: {data_len}")

    payload = read_exact(ser, data_len)
    crc_be = read_exact(ser, FRAME_CRC_SIZE)
    crc_recv = int.from_bytes(crc_be, "big")

    crc = crc16_update(0, data_len_be)
    crc = crc16_update(crc, payload)
    if crc != crc_recv:
        raise ValueError(
            f"CRC mismatch: received 0x{crc_recv:04x}, calculated 0x{crc:04x}"
        )

    return payload
