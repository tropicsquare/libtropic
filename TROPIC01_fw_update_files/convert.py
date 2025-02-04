#!/usr/bin/env python3

import sys
import os

def binary_to_c_array(binary_file):
    with open(binary_file, 'rb') as f:
        data = f.read()

    print("const uint8_t binary_data[] = {")
    for i, byte in enumerate(data):
        if i % 16 == 0:
            print("    ", end="")
        print(f"0x{byte:02x}", end="")
        if i < len(data) - 1:
            print(", ", end="")
        if (i + 1) % 16 == 0:
            print()
    print("\n};")
    print(f"\nconst size_t binary_data_len = {len(data)};")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <binary_file>")
        sys.exit(1)

    binary_to_c_array(sys.argv[1])
