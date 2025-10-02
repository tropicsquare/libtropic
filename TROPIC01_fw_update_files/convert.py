#!/usr/bin/env python3
# This script converts a binary file into a C array format.

# searches for .bin inside of a given folder
# creates header file with the name of that bianry file


import sys
import os
import re

def binary_to_c_array(path_to_fw_folder, filename, type):
    print(path_to_fw_folder)
    with open(path_to_fw_folder + filename, 'rb') as f:
        data = f.read()

    header_file_name = path_to_fw_folder + "fw_" + type + ".h"
    with open(header_file_name, 'w') as header:
        header.write("#pragma once\n")
        header.write("/**\n")
        header.write(f" * @brief {type} firmware version {filename} for bootloader v1.0.1\n")
        header.write(" */\n")
        header.write("const uint8_t " + "fw_" + type + "[] = {\n")
        for i, byte in enumerate(data):
            if i % 16 == 0:
                header.write("    ")
            header.write(f"0x{byte:02x}")
            if i < len(data) - 1:
                header.write(", ")
            if (i + 1) % 16 == 0:
                header.write("\n")
        header.write("\n};")
        #header.write(f"\nconst uint16_t {version_name}_len = {len(data)};")

def list_files_in_directory(path):
    try:
        # List all files in the given directory
        files = os.listdir(path)

        # Filter out directories, keep only files
        files = [f for f in files if os.path.isfile(os.path.join(path, f))]

        # Check if there are exactly two files
        if len(files) != 2:
            print(f"Expected 2 files, but found {len(files)}.")
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
            # Format the version as 0_3_1
            version = f"{match.group(1)}_{match.group(2)}_{match.group(3)}"
            return version
        else:
            print(f"No version found in filename: {filename}")
            return None
    except Exception as e:
        print(f"An error occurred while parsing the version: {e}")
        return None

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

        spect_version = parse_version_from_filename(spect_fw_filename)
        if spect_version:
            print(f"Parsed SPECT FW version: {spect_version}")
            binary_to_c_array(sys.argv[1], spect_fw_filename, "SPECT")

        cpu_version = parse_version_from_filename(cpu_fw_filename)
        if cpu_version:
            print(f"Parsed CPU FW version: {cpu_version}")
            binary_to_c_array(sys.argv[1], cpu_fw_filename, "CPU")

