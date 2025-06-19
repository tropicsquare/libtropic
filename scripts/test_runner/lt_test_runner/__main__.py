from dataclasses import dataclass
import logging
import subprocess
import shutil
import sys
import asyncio
import serial.tools
import serial.tools.list_ports
import telnetlib3
import serial
from enum import Enum, IntEnum
from abc import ABC, abstractmethod
from pathlib import Path
from argparse import ArgumentParser

from .lt_test_runner import lt_test_runner
from .lt_platform_factory import lt_platform_factory

logger = logging.getLogger(__name__)

async def main() -> lt_test_runner.lt_test_result:

    parser = ArgumentParser(
        prog = "test_runner.py",
        description = "This script loads a test firmware to a selected platform and checks the results via serial port.",
        epilog = "Copyright 2025 Tropic Square s.r.o."
    )

    parser.add_argument(
        "platform_id",
        help    = "A platform to run the test firmware on.",
        choices = lt_platform_factory.LT_PLATFORM_STRING_ID_MAPPING.keys(),
        type    = str
    )

    parser.add_argument(
        "firmware",
        help    = "Firmware binary to load to a selected platform.",
        type    = Path
    )

    parser.add_argument(
        "--work_dir",
        help    = "Working directory.",
        type    = Path,
        default = Path.cwd() / "build" / "system_tests"
    )
    
    parser.add_argument(
        "--mapping_config",
        help    = "Path to the platform-adapter mapping config CSV file.",
        type    = Path,
        default = Path(__file__).parent / "adapter_mapping.csv"
    )

    parser.add_argument(
        "--adapter_config",
        help    = "Path to the adapter OpenOCD config file.",
        type    = Path,
        default = Path(__file__).parent / "ts11-jtag.cfg"
    )

    parser.add_argument(
        "-v", "--verbose",
        help    = "Enable debug logging.",
        action  = "store_true"
    )

    args = parser.parse_args()

    if not args.firmware.is_file():
        logger.error("Please provide correct path to firmware.")
        return
    if args.work_dir.is_file():
        logger.error("Invalid build directory!")
        return
    if not args.mapping_config.is_file():
        logger.error("Invalid path to mapping config!")
        return
    if not args.adapter_config.is_file():
        logger.error("Invalid path to adapter config!")
        return
    
    if args.verbose:
        logging.basicConfig(level=logging.DEBUG)
    else:
        logging.basicConfig(level=logging.INFO)

    test_result = lt_test_runner.lt_test_result.TEST_FAILED # The default is failure in case an exception is thrown.

    try:
        tr = lt_test_runner(args.work_dir, args.platform_id, args.mapping_config, args.adapter_config)
    except (ValueError, FileNotFoundError):
        logger.error("Failed to initialize test runner.")
        return lt_test_runner.lt_test_result.TEST_FAILED

    try:
        test_result = await tr.run(args.firmware)
    except serial.SerialException as e:
        logger.error(f"Platform serial interface communication error: {str(e)}")
        return
    except:
        logger.info("Received unexpected exception or termination request. Shutting down.")
        raise

    # We have to return here, not terminate (using sys.exit), so destructors are correctly called
    # and no processes/threads are left hanging.
    return test_result

if __name__ == "__main__":
    try:
        test_result = asyncio.run(main())
        if test_result == lt_test_runner.lt_test_result.TEST_PASSED:
            sys.exit(0) # Test OK
        else:
            sys.exit(1) # Test failed
    except KeyboardInterrupt:
        logger.info("Interrupted by SIGINT.")

    sys.exit(2) # Exception