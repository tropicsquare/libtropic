#!/usr/bin/env python3
# This script converts a binary file into a C array format.

# searches for .bin inside of a given folder
# creates header file with the name of that bianry file


import sys
import os
import re

def binary_to_c_array(path_to_fw_folder, filename, type, version, boot_version):
    print(path_to_fw_folder)
    with open(path_to_fw_folder + filename, 'rb') as f:
        data = f.read()

    major, minor, patch = version

    # 16 bytes per line
    data_lines = ",\n".join(
        "    " + ", ".join(f"0x{byte:02x}" for byte in data[i:i + 16])
        for i in range(0, len(data), 16)
    )

    header_content = f"""\
#pragma once

#include <stdint.h>

/**
 * @brief {type} firmware version array {{reserved, patch, minor, major}}
 */
const uint8_t fw_{type}_ver[4] = {{0, {patch}, {minor}, {major}}};

/**
 * @brief {type} firmware version {filename} for bootloader v{boot_version}
 */
const uint8_t fw_{type}[] = {{
{data_lines}
}};
"""

    header_file_name = path_to_fw_folder + "fw_" + type + ".h"
    with open(header_file_name, 'w') as header:
        header.write(header_content)

# Each bootloader version expects firmware binaries in a different format
BOOT_VERSION_TO_BIN_SUFFIX = {
    "1.0.1": "_signed.bin",
    "2.0.1": "_signed_chunks.bin",
}

# Filename prefix of each firmware type, followed by vX.Y.Z
FW_TYPE_TO_PREFIX = {
    "SPECT": "spect_app-",
    "CPU": "fw_",
}

def parse_boot_version_from_path(path):
    """Parse the bootloader version from path. Exits if it is missing or unknown."""
    # Bootloader version is encoded in the parent folder name as boot_v_X_Y_Z
    match = re.search(r'boot_v_(\d+)_(\d+)_(\d+)', os.path.abspath(path))
    if not match:
        sys.exit(f"No bootloader version (boot_v_X_Y_Z) found in path: {path}")

    boot_version = ".".join(match.groups())
    if boot_version not in BOOT_VERSION_TO_BIN_SUFFIX:
        sys.exit(f"Unknown bootloader version: {boot_version}")

    print(f"Parsed bootloader version: {boot_version}")
    return boot_version

def find_fw_binary(path, fw_type, boot_version):
    """Find the only fw_type binary in path that is in the format expected by boot_version.

    Returns (filename, (major, minor, patch)). Exits if there is not exactly one match.
    """
    prefix = FW_TYPE_TO_PREFIX[fw_type]
    suffix = BOOT_VERSION_TO_BIN_SUFFIX[boot_version]
    # Anything may follow the version (e.g. ".hex32" in CPU FW binaries for bootloader 2.0.1)
    pattern = re.compile(re.escape(prefix) + r"v(\d+)\.(\d+)\.(\d+).*" + re.escape(suffix))

    bin_files = sorted(f for f in os.listdir(path) if f.endswith(".bin") and os.path.isfile(os.path.join(path, f)))
    matches = [f for f in bin_files if pattern.fullmatch(f)]

    if len(matches) != 1:
        sys.exit(f"Expected exactly one {fw_type} FW named {prefix}vX.Y.Z*{suffix} "
                 f"for bootloader v{boot_version}, but found {len(matches)}.\n"
                 f"Present .bin files: {', '.join(bin_files) or 'none'}")

    filename = matches[0]
    version = tuple(int(g) for g in pattern.fullmatch(filename).groups())
    print(f"Located {fw_type} FW: {filename} (version {version})")
    return filename, version

if __name__ == "__main__":

    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <path_to_fw_folder>")
        print()
        print("Please provide the path to the folder containing signed firmware bin files for APP and SPECT..")
        print("Example: ./convert.py /path/to/firmwares/")
        print("Note: The folder shall be inside a boot_v_X_Y_Z folder and contain one SPECT and one CPU FW bin file")
        print("      in the format expected by that bootloader version.")
        sys.exit(1)

    # check if provided directory exists
    fw_dir = sys.argv[1]
    if not os.path.isdir(fw_dir):
        sys.exit(f"Error: The directory '{fw_dir}' does not exist.")

    # get bootloader version from parent dir name
    boot_version = parse_boot_version_from_path(fw_dir)

    # locate fw binaries
    spect_bin, spect_version = find_fw_binary(fw_dir, "SPECT", boot_version)
    cpu_bin, cpu_version = find_fw_binary(fw_dir, "CPU", boot_version)

    binary_to_c_array(fw_dir, spect_bin, "SPECT", spect_version, boot_version)
    binary_to_c_array(fw_dir, cpu_bin, "CPU", cpu_version, boot_version)

