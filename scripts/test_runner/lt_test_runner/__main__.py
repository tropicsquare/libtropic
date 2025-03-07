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

from .lt_environment_tools import lt_environment_tools
from .lt_test_runner import lt_test_runner
from .lt_platform_factory import lt_platform_factory

logger = logging.getLogger(__name__)

async def main():

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

    platform        = lt_platform_factory.create_from_str_id(args.platform_id)
    adapter_id      = lt_environment_tools.get_adapter_id_from_mapping(args.platform_id, args.mapping_config)
    adapter_serial  = lt_environment_tools.get_serial_device_from_vidpid(adapter_id.vid, adapter_id.pid, 1) # TS11 adapter uses second interface for serial port.
    test_result     = lt_test_runner.lt_test_result.TEST_FAILED # The default is failure in case an exception is thrown.

    if platform is None:
        logger.error(f"Platform '{platform}' not found!")
        return

    if adapter_id is None:
        logger.error(f"Selected platform has not its adapter ID assigned yet in the config file. Assign the ID in {args.mapping_config} (or select correct config file) and try again.")
        return

    if adapter_serial is None:
        logger.error("Serial adapter not found. Check if adapter is correctly connected to the computer and correct interface is selected.")
        return

    try:
        openocd_launch_params = ["-f", args.adapter_config] + ["-c", f"ftdi vid_pid {adapter_id.vid:#x} {adapter_id.pid:#x}"] + platform.get_openocd_launch_params() 
        logger.info(f"Launching OpenOCD with '{openocd_launch_params}'")
        openocd_proc = subprocess.Popen([shutil.which("openocd")] + openocd_launch_params, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

        await asyncio.sleep(2)

        if openocd_proc.poll() is not None:
            logger.info("Error starting OpenOCD. Check if computer-adapter-platform connection is functional or if OpenOCD is not already running.")
            return

        tr = lt_test_runner(args.work_dir, platform, adapter_serial)
        test_result = await tr.run(args.firmware)
    except serial.SerialException as e:
        logger.error(f"Platform serial interface communication error: {str(e)}")
        lt_environment_tools.shutdown_openocd(openocd_proc)
        return
    except:
        logger.info("Received unexpected exception or termination request. Shutting down.")
        lt_environment_tools.shutdown_openocd(openocd_proc)
        raise

    lt_environment_tools.shutdown_openocd(openocd_proc)

    if test_result == lt_test_runner.lt_test_result.TEST_PASSED:
        sys.exit(0) # Test OK
    else:
        sys.exit(1) # Test failed

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        logger.info("Interrupted by SIGINT.")
    sys.exit(2) # Exception