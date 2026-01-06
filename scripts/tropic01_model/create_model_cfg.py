from __future__ import annotations
import argparse
import pathlib
import yaml
from cryptography.x509 import load_pem_x509_certificate, load_der_x509_certificate
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.backends import default_backend

def parse_byte_string(s: str) -> bytes:
    if s.startswith("b'") and s.endswith("'"):
        s = s[2:-1]
    return s.encode('latin1').decode('unicode_escape').encode('latin1')

def load_cert(cert_path: pathlib.Path) -> bytes:
    """Load x509 certificate and convert to ASN.1 DER-TLV."""
    if cert_path.suffix == ".pem":
        return load_pem_x509_certificate(cert_path.read_bytes()).public_bytes(
            serialization.Encoding.DER
        )
    if cert_path.suffix == ".der":
        return cert_path.read_bytes()
    raise argparse.ArgumentTypeError("Certificate not in DER nor PEM format.")

def parse_and_complete_chip_id(pkg_yml: dict) -> bytes:
    res = parse_byte_string(pkg_yml["chip_id"]["chip_id_version"])
    res += 16 * b'\x00'  # Wafer level test info (source: Programmer)
    res += parse_byte_string(pkg_yml["chip_id"]["manufacturing_test"])
    res += parse_byte_string(pkg_yml["chip_id"]["provisioning_info"])
    res += parse_byte_string(pkg_yml["chip_id"]["serial_number_v2"])
    res += parse_byte_string(pkg_yml["chip_id"]["part_number"])
    res += parse_byte_string(pkg_yml["chip_id"]["provisioning_data"])
    res += 24 * b'\xff'  # Padding
    return res

def validate_and_parse_riscv_fw_ver(riscv_fw_ver: str) -> list[int]:
    try:
        # 1. Split the string
        parts = riscv_fw_ver.split('.')
        
        # 2. Check if there are exactly 3 parts
        if len(parts) != 3:
            # Raise the specific error argparse expects
            raise argparse.ArgumentTypeError(
                f"'{riscv_fw_ver}' is not in the required 'X.Y.Z' format."
            )
            
        # 3. Try to convert all parts to integers
        # This will raise a ValueError if any part is not a number
        numbers = [int(part) for part in parts]
        
        # 4. Return the successfully parsed list of numbers
        return numbers
        
    except ValueError:
        # Catch errors from int() conversion
        raise argparse.ArgumentTypeError(
            f"'{riscv_fw_ver}' contains non-numeric parts."
        )


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog="create_model_cfg.py",
        description="Creates YAML configuration file for the TROPIC01 model from lab_batch_package."
    )

    parser.add_argument(
        "--pkg-dir",
        help="Path to the batch package directory. You can use batch packages inside 'tropic01_model/provisioning_data/'.",
        type=pathlib.Path,
        required=True
    )

    parser.add_argument(
        "--riscv-fw-ver",
        help="RISC-V FW version in format [0-9].[0-9].[0-9] (e.g. 1.0.0). Note that the configured FW version does not affect the model behavior; Libtropic requires valid FW version to configure some properties at runtime.",
        type=validate_and_parse_riscv_fw_ver,
        required=True
    )

    parser.add_argument(
        "--model-cfg",
        help="Path to the file where to write the created YAML model configuration (default: 'model_cfg.yml').",
        type=pathlib.Path,
        default="model_cfg.yml"
    )

    # Parse and save arguments
    args = parser.parse_args()
    pkg_dir_path: pathlib.Path = args.pkg_dir
    model_cfg_path: pathlib.Path = args.model_cfg
    riscv_fw_major, riscv_fw_minor, riscv_fw_patch = args.riscv_fw_ver

    # Load batch package YAML
    with pkg_dir_path.joinpath("tropic01_lab_batch_package.yml").open("r") as f:
        pkg_yml = yaml.safe_load(f)

    ese_cert_path = pkg_dir_path.joinpath(pkg_yml["tropic01_ese_certificate"])
    ese_priv_path = pkg_dir_path.joinpath(pkg_yml["tropic01_ese_private_key"])
    model_cfg = {}

    # Save raw eSE priv key to model configuration
    ese_priv = serialization.load_pem_private_key(
        ese_priv_path.read_bytes(),
        password=None
    )
    model_cfg["s_t_priv"] = ese_priv.private_bytes(
        encoding=serialization.Encoding.Raw,
        format=serialization.PrivateFormat.Raw,
        encryption_algorithm=serialization.NoEncryption()
    )

    # Save raw eSE pub key to model configuration
    ese_cert = load_der_x509_certificate(
        load_cert(ese_cert_path),
        default_backend()
    )
    model_cfg["s_t_pub"] = ese_cert.public_key().public_bytes(
        encoding=serialization.Encoding.Raw,
        format=serialization.PublicFormat.Raw
    )

    # Get chip id from batch package YAML, add missing data
    model_cfg["chip_id"] = parse_and_complete_chip_id(pkg_yml)

    # Generate certificate store and save it to model configuration
    cert_store_header = b'\x01'  # Store version
    cert_store_header += b'\x04'  # Number of certificates
    cert_store_body = b''
    for cert_path in [ese_cert_path,
                      pkg_dir_path.joinpath(pkg_yml["tropic01_xxxx_ca_certificate"]),
                      pkg_dir_path.joinpath(pkg_yml["tropic01_ca_certificate"]),
                      pkg_dir_path.joinpath(pkg_yml["tropicsquare_root_ca_certificate"])]:
        cert_der = load_cert(cert_path)
        cert_store_header += len(cert_der).to_bytes(2, 'big')
        cert_store_body += cert_der
    model_cfg["x509_certificate"] = cert_store_header+cert_store_body

    # Get raw sh0pub key and save it to model configuration
    sh0_pub_key = serialization.load_pem_public_key(
        pkg_dir_path.joinpath(pkg_yml["s_h0pub_key"]).read_bytes()
    )
    sh0_pub_raw = sh0_pub_key.public_bytes(
        encoding=serialization.Encoding.Raw,
        format=serialization.PublicFormat.Raw
    )
    model_cfg["i_pairing_keys"] = {
        0: {
            "value": sh0_pub_raw,
            "state": "written"
        }
    }

    # Set RISC-V FW version
    model_cfg["riscv_fw_version"] = b'\x00' + riscv_fw_patch.to_bytes(1, 'little') + \
                                    riscv_fw_minor.to_bytes(1, 'little') + riscv_fw_major.to_bytes(1, 'little')

    print("Warning: Following variables are not configured (model will set them to default values):")
    print("\t- r_config")
    print("\t- r_ecc_keys")
    print("\t- r_user_data")
    print("\t- r_mcounters")
    print("\t- r_macandd_data")
    print("\t- i_config")
    print("\t- spect_fw_version")
    print("\t- debug_random_value")
    print("\t- activate_encryption")
    print("\t- init_byte")
    print("\t- busy_iter")

    # Generate the model configuration YAML
    with model_cfg_path.open("w") as f:
        yaml.dump(model_cfg, f, default_flow_style=False)