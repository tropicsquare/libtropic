#!/usr/bin/env python3
# This script converts a binary file into a C array format.

# searches for .bin inside of a given folder
# creates header file with the name of that bianry file


import sys
import os
import re

def write_fw_header(path_to_fw_folder, filename, type, version, is_boot_v_1_0_1):
    BYTES_PER_LINE = 16

    with open(os.path.join(path_to_fw_folder, filename), 'rb') as f:
        data = f.read()

    major, minor, patch = version

    # Format the data as hex bytes, BYTES_PER_LINE per line
    lines = []
    for offset in range(0, len(data), BYTES_PER_LINE):
        chunk = data[offset:offset + BYTES_PER_LINE]  # single line of bytes
        hex_bytes = [f"0x{byte:02x}" for byte in chunk]  # convert bytes in line to str
        lines.append("    " + ", ".join(hex_bytes))

    data_lines = ",\n".join(lines)

    header_content = f"""\
#pragma once

#include <stdint.h>

/**
 * @brief {type} firmware version array {{reserved, patch, minor, major}}
 */
const uint8_t fw_{type}_ver[4] = {{0, {patch}, {minor}, {major}}};

/**
 * @brief {type} firmware version {filename} for bootloader {boot_version_str(is_boot_v_1_0_1)}
 */
const uint8_t fw_{type}[] = {{
{data_lines}}};"""

    header_file_name = os.path.join(path_to_fw_folder, "fw_" + type + ".h")
    with open(header_file_name, 'w') as header:
        header.write(header_content)

# Filename prefix of each firmware type, followed by vX.Y.Z
FW_TYPE_TO_PREFIX = {
    "SPECT": "spect_app-",
    "CPU": "fw_",
}

def boot_version_str(is_boot_v_1_0_1):
    return "v1.0.1" if is_boot_v_1_0_1 else "v2.0.1 and newer"

def get_boot_version_from_path(path):
    """Determine the bootloader version from the parent folder name of path. Exits if it is unknown.

    Returns True for bootloader 1.0.1, False for bootloader 2.0.1 and newer.
    """
    boot_dir = os.path.basename(os.path.dirname(os.path.abspath(path)))
    if boot_dir == "boot_v_1_0_1":
        is_boot_v_1_0_1 = True
    elif boot_dir == "boot_v_2_0_1_and_newer":
        is_boot_v_1_0_1 = False
    else:
        sys.exit(f"Unknown bootloader folder '{boot_dir}', expected boot_v_1_0_1 or boot_v_2_0_1_and_newer")

    print(f"Bootloader version: {boot_version_str(is_boot_v_1_0_1)}")
    return is_boot_v_1_0_1

def find_fw_binary(path, fw_type, is_boot_v_1_0_1):
    """Find the only fw_type binary in path that is in the format expected by the bootloader.

    Returns (filename, (major, minor, patch)). Exits if there is not exactly one match.
    """
    prefix = FW_TYPE_TO_PREFIX[fw_type]
    # Bootloader 2.0.1 and newer expects firmware binaries split into chunks
    suffix = "_signed.bin" if is_boot_v_1_0_1 else "_signed_chunks.bin"
    # Anything may follow the version (e.g. ".hex32" in CPU FW binaries for bootloader 2.0.1 and newer)
    pattern = re.compile(re.escape(prefix) + r"v(\d+)\.(\d+)\.(\d+).*" + re.escape(suffix))

    bin_files = sorted(f for f in os.listdir(path) if f.endswith(".bin") and os.path.isfile(os.path.join(path, f)))
    matches = [f for f in bin_files if pattern.fullmatch(f)]

    if len(matches) != 1:
        sys.exit(f"Expected exactly one {fw_type} FW named {prefix}vX.Y.Z*{suffix} "
                 f"for bootloader {boot_version_str(is_boot_v_1_0_1)}, but found {len(matches)}.\n"
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
        print("Note: The folder shall be inside the boot_v_1_0_1 or boot_v_2_0_1_and_newer folder")
        print("      and contain one SPECT and one CPU FW bin file")
        print("      in the format expected by that bootloader version.")
        sys.exit(1)

    # check if provided directory exists
    fw_dir = sys.argv[1]
    if not os.path.isdir(fw_dir):
        sys.exit(f"Error: The directory '{fw_dir}' does not exist.")

    # get bootloader version from parent dir name
    is_boot_v_1_0_1 = get_boot_version_from_path(fw_dir)

    # locate fw binaries
    spect_bin, spect_version = find_fw_binary(fw_dir, "SPECT", is_boot_v_1_0_1)
    cpu_bin, cpu_version = find_fw_binary(fw_dir, "CPU", is_boot_v_1_0_1)

    write_fw_header(fw_dir, spect_bin, "SPECT", spect_version, is_boot_v_1_0_1)
    write_fw_header(fw_dir, cpu_bin, "CPU", cpu_version, is_boot_v_1_0_1)

