from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import x25519
import pathlib
import argparse

def load_x25519_private_key(key_path: pathlib.Path) -> x25519.X25519PrivateKey:
    with key_path.open('rb') as key_file:
        key_data = key_file.read()

    try:
        # Try loading PEM format
        private_key = serialization.load_pem_private_key(key_data, password=None)
    except ValueError:
        try:
            # Try loading DER format
            private_key = serialization.load_der_private_key(key_data, password=None)
        except ValueError:
            raise ValueError("Invalid key format. Must be PEM or DER X25519 private key.")

    if not isinstance(private_key, x25519.X25519PrivateKey):
        raise TypeError("The provided key is not an X25519 private key.")

    return private_key

def extract_raw_keys(private_key: x25519.X25519PrivateKey) -> tuple[bytes, bytes]:
    raw_private = private_key.private_bytes(
        encoding=serialization.Encoding.Raw,
        format=serialization.PrivateFormat.Raw,
        encryption_algorithm=serialization.NoEncryption()
    )
    raw_public = private_key.public_key().public_bytes(
        encoding=serialization.Encoding.Raw,
        format=serialization.PublicFormat.Raw
    )
    return raw_private, raw_public

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog="extract_x25519_key_data.py",
        description="Extracts raw data from X25519 private key in PEM or DER format and also generates public key raw data."
    )

    parser.add_argument(
        "--priv-key",
        help="Path to the X25519 private key in PEM or DER format.",
        type=pathlib.Path,
        required=True
    )

    parser.add_argument(
        "--priv-out",
        help="Path to the file where to put X25519 private key raw data.",
        type=pathlib.Path,
        required=True
    )

    parser.add_argument(
        "--pub-out",
        help="Path to the file where to put X25519 public key raw data.",
        type=pathlib.Path,
        required=True
    )

    args = parser.parse_args()

    priv_key = load_x25519_private_key(args.priv_key)
    raw_private, raw_public = extract_raw_keys(priv_key)

    args.priv_out.write_bytes(raw_private)
    args.pub_out.write_bytes(raw_public)