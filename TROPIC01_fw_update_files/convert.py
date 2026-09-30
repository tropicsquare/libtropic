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

def list_files_in_directory(path):
    try:
        # List all files in the given directory
        files = os.listdir(path)

        # Keep only .bin files (ignores previously generated headers)
        files = [f for f in files if f.endswith(".bin") and os.path.isfile(os.path.join(path, f))]

        # Check if there are exactly two files
        if len(files) != 2:
            print(f"Expected 2 .bin files, but found {len(files)}.")
            return None, None

        # Return the two filenames
        return files[0], files[1]
    except FileNotFoundError:
        print(f"Error: The directory '{path}' does not exist.")
        return None, None
    except Exception as e:
        print(f"An error occurred: {e}")
        return None, None


def parse_version_from_filename(filename):
    try:
        # Use regex to find the version pattern vX.X.X
        match = re.search(r'v(\d+)\.(\d+)\.(\d+)', filename)
        if match:
            # Return the version as (major, minor, patch)
            return tuple(int(g) for g in match.groups())
        else:
            print(f"No version found in filename: {filename}")
            return None
    except Exception as e:
        print(f"An error occurred while parsing the version: {e}")
        return None

def parse_boot_version_from_path(path):
    # Bootloader version is encoded in the parent folder name as boot_v_X_Y_Z
    match = re.search(r'boot_v_(\d+)_(\d+)_(\d+)', os.path.abspath(path))
    if match:
        return ".".join(match.groups())
    print(f"No bootloader version (boot_v_X_Y_Z) found in path: {path}")
    return None

# Each bootloader version expects firmware binaries in a different format
BOOT_VERSION_TO_BIN_SUFFIX = {
    "1.0.1": "_signed.bin",
    "2.0.1": "_signed_chunks.bin",
}

def check_bin_format(filename, boot_version):
    expected_suffix = BOOT_VERSION_TO_BIN_SUFFIX.get(boot_version)
    if expected_suffix is None:
        print(f"Unknown bootloader version: {boot_version}")
        return False
    if not filename.endswith(expected_suffix):
        print(f"Wrong binary format for bootloader v{boot_version}: {filename} (expected *{expected_suffix})")
        return False
    return True

if __name__ == "__main__":

    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <path_to_fw_folder>")
        print()
        print("Please provide the path to the folder containing signed firmware bin files for APP and SPECT..")
        print("Example: ./convert.py /path/to/firmwares/")
        print("Note: The folder shall contain exactly two bin files with version numbers in their names.")
        sys.exit(1)

    filename1, filename2 = list_files_in_directory(sys.argv[1])

    if filename1 and filename2:

        if "spect" in filename1 and "fw" in filename2:
            spect_fw_filename = filename1
            cpu_fw_filename = filename2
        elif "spect" in filename2 and "fw" in filename1:
            spect_fw_filename = filename2
            cpu_fw_filename = filename1
        else:
            print("Can't locate SPECT and CPU FWs")

        print(f"Located SPECT FW: {spect_fw_filename}")
        print(f"Located CPU FW: {cpu_fw_filename}")

        boot_version = parse_boot_version_from_path(sys.argv[1])
        if not boot_version:
            sys.exit(1)
        print(f"Parsed bootloader version: {boot_version}")

        if not (check_bin_format(spect_fw_filename, boot_version) and
                check_bin_format(cpu_fw_filename, boot_version)):
            sys.exit(1)

        spect_version = parse_version_from_filename(spect_fw_filename)
        if spect_version:
            print(f"Parsed SPECT FW version: {spect_version}")
            binary_to_c_array(sys.argv[1], spect_fw_filename, "SPECT", spect_version, boot_version)

        cpu_version = parse_version_from_filename(cpu_fw_filename)
        if cpu_version:
            print(f"Parsed CPU FW version: {cpu_version}")
            binary_to_c_array(sys.argv[1], cpu_fw_filename, "CPU", cpu_version, boot_version)

