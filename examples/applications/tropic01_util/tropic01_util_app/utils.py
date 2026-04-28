from __future__ import annotations
"""Small helpers shared by multiple commands."""


def parse_hex_bytes(hex_text: str | None) -> bytes | None:
    """Parse optional hex text to bytes with a clearer odd-length error."""
    if hex_text is None:
        return None
    compact = hex_text.replace(" ", "")
    if len(compact) % 2 != 0:
        raise ValueError("Hex string must have an even number of digits")
    return bytes.fromhex(compact)