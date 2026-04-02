from __future__ import annotations
"""pin-set command: CLI args, request build, and response formatting."""

import argparse
from pathlib import Path

from ..command_core import AppCommandSender, CliCommandSpec, print_libtropic_res_code
from ..utils import parse_hex_bytes
from protobuf.generated import usb_devkit_messages_pb2 as pb


def bip32_master_private_key_from_seed(seed: bytes) -> bytes:
    """Derive BIP32 secp256k1 master private key bytes from a BIP39 seed."""
    try:
        from bip_utils import Bip32Secp256k1  # pyright: ignore[reportMissingImports]
    except ModuleNotFoundError as exc:
        raise RuntimeError(
            "Python package 'bip-utils' is required for BIP32 derivation. Install it with: pip install bip-utils"
        ) from exc

    bip32_ctx = Bip32Secp256k1.FromSeed(seed)

    return bip32_ctx.PrivateKey().Raw().ToBytes()


def derive_secret_from_bip39_words(words_text: str, passphrase: str) -> bytes:
    """Validate BIP39 words and derive PinSetCmd secret deterministically."""
    try:
        from bip_utils import (  # pyright: ignore[reportMissingImports]
            Bip39MnemonicValidator,
            Bip39SeedGenerator,
        )
    except ModuleNotFoundError as exc:
        raise RuntimeError(
            "Python package 'bip-utils' is required for BIP39/BIP32 derivation. Install it with: pip install bip-utils"
        ) from exc

    words = [w for w in words_text.strip().split() if w]
    normalized = " ".join(words)

    if not Bip39MnemonicValidator().IsValid(normalized):
        raise ValueError("Invalid BIP39 words/checksum")

    # Full BIP39 seed derivation returns 64 bytes (PBKDF2-HMAC-SHA512, 2048 rounds).
    seed = Bip39SeedGenerator(normalized).Generate(passphrase)

    # PinSetCmd.secret expects exactly 32 bytes. Use BIP32 master private key (secp256k1).
    return bip32_master_private_key_from_seed(seed)


def derive_secret_from_bip39_words_file(file_path: str, passphrase: str) -> bytes:
    """Load BIP39 words from file and derive PinSetCmd secret."""
    text = Path(file_path).read_text(encoding="utf-8")

    return derive_secret_from_bip39_words(text, passphrase)


def add_arguments(parser: argparse.ArgumentParser) -> None:
    """Register pin-set specific CLI flags."""
    parser.add_argument("--pin", required=True, help="New PIN string.")
    parser.add_argument(
        "--attempts", required=True, type=int, help="Allowed attempts before destroy."
    )
    parser.add_argument(
        "--additional-data-hex",
        default=None,
        help="Optional additional data as hex, e.g. deadbeef.",
    )
    secret_group = parser.add_mutually_exclusive_group(required=True)
    secret_group.add_argument(
        "--secret-hex",
        default=None,
        help="32-byte secret as hex (64 hex chars) used as the MAC-And-Destroy secret.",
    )
    secret_group.add_argument(
        "--secret-bip39-words",
        default=None,
        help="BIP39 words (12/15/18/21/24) used to derive the MAC-And-Destroy secret.",
    )
    secret_group.add_argument(
        "--secret-bip39-file",
        default=None,
        help="Path to text file containing BIP39 words used to derive the MAC-And-Destroy secret.",
    )
    parser.add_argument(
        "--secret-bip39-passphrase",
        default="",
        help="Optional BIP39 passphrase used to derive the MAC-And-Destroy secret with --secret-bip39-words/--secret-bip39-file; ignored with --secret-hex.",
    )


def build_cmd_from_args(args: argparse.Namespace) -> pb.AppCmd:
    """Convert parsed CLI args into a AppCmd for pin-set."""
    # 1. Validate and parse user input.
    if args.attempts <= 0:
        raise ValueError("--attempts must be > 0")

    additional_data = parse_hex_bytes(args.additional_data_hex)

    # The secret may be provided directly or derived from BIP39 inputs.
    if args.secret_hex is not None:
        secret = parse_hex_bytes(args.secret_hex)
        if secret is None:
            raise ValueError("--secret-hex parsing failed")
    elif args.secret_bip39_file is not None:
        secret = derive_secret_from_bip39_words_file(
            args.secret_bip39_file, args.secret_bip39_passphrase
        )
    else:
        secret = derive_secret_from_bip39_words(
            args.secret_bip39_words,
            args.secret_bip39_passphrase,
        )

    if len(secret) != 32:
        raise ValueError(f"Secret must be exactly 32 bytes, got {len(secret)}")

    # 2. Construct the command.
    cmd = pb.AppCmd()
    cmd.pin_set.new_pin = args.pin
    cmd.pin_set.attempts = args.attempts
    if additional_data is not None:
        cmd.pin_set.additional_data = additional_data
    cmd.pin_set.secret = secret

    return cmd


def decode_resp(resp: pb.AppResp) -> int:
    """Print pin-set response summary and optional key on success."""
    print(f"{PIN_SET_SPEC.name} result: {pb.PinSetRespCode.Name(resp.pin_set.res_code)}")
    if resp.pin_set.res_code == pb.PIN_SET_RESP_CODE_OK:
        print(f"{PIN_SET_SPEC.name} crypto_key: {resp.pin_set.crypto_key.hex()}")

    return 0


def execute(args: argparse.Namespace, app_cmd_sender: AppCommandSender) -> int:
    """Execute pin-set command and validate the pin_set response type."""
    cmd = build_cmd_from_args(args)
    app_resp = app_cmd_sender.send(cmd, expected_resp_type="pin_set")
    status = decode_resp(app_resp)
    print_libtropic_res_code(app_resp, PIN_SET_SPEC.name)
    return status


PIN_SET_SPEC = CliCommandSpec(
    name="pin-set",
    help_text="Setup new PIN (utilizing MAC-And-Destroy feature)",
    description="Sets up a new PIN using the TROPIC01 MAC-And-Destroy feature. Upon success, returns a cryptographic key (guarded by the PIN).",
    add_arguments=add_arguments,
    execute=execute,
)
