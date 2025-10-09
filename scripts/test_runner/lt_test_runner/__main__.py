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
import time

from .lt_test_runner import lt_test_runner
from .lt_platform_factory import lt_platform_factory
from .lt_lock_device import lt_lock_device
from .lt_openocd_launcher import lt_openocd_launcher
from .lt_environment_tools import lt_environment_tools

logger = logging.getLogger(__name__)

LOCK_FILE_PATH = Path("/tmp") / "ts11_test_runner.lock"

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
        default = Path.cwd() / "build"
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
        "--adapter_shutdown_config",
        help    = "Path to the adapter OpenOCD config file used when shutting down.",
        type    = Path,
        default = Path(__file__).parent / "ts11-jtag-shutdown.cfg"
    )

    parser.add_argument(
        "-v", "--verbose",
        help    = "Enable debug logging.",
        action  = "store_true"
    )

    parser.add_argument(
        "--message-timeout",
        help    = "Max time in seconds to wait between messages before timing out. Must be >=0. Default zero (no timeout).",
        type    = int,
        default = 0
    )

    parser.add_argument(
        "--total-timeout",
        help    = "Total max test time regardless of message activity. Ideal to mitigate cases where the platform outputs garbage. Default zero (no timeout). Must be >=0. Note that without message_timeout is this timeout ineffective in case of the platform stops sending messages.",
        type    = int,
        default = 0
    )

    args = parser.parse_args()
    if args.message_timeout < 0:
        parser.error("Message timeout has to be >= 0.")
    if args.total_timeout < 0:
        parser.error("Total timeout has to be >= 0.")

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
    if not args.adapter_shutdown_config.is_file():
        logger.error("Invalid path to dapter shutdown config!")
        return

    if args.verbose:
        logging.basicConfig(level=logging.DEBUG)
    else:
        logging.basicConfig(level=logging.INFO)

    args.work_dir.mkdir(exist_ok = True, parents = True)

    with lt_lock_device(LOCK_FILE_PATH) as locked:
        if not locked:
            logger.error(f"Lock couldn't be acquired, the TS11 is used by another user. If you are sure that this is an error and no one is using the device, remove {LOCK_FILE_PATH.absolute()} and try again.")
            return lt_test_runner.lt_test_result.TEST_FAILED
        
        try:
            tr = lt_test_runner(args.work_dir, args.platform_id, args.mapping_config, args.adapter_config)
        except (ValueError):
            logger.error("Failed to initialize test runner.")
            return lt_test_runner.lt_test_result.TEST_FAILED

        test_result = lt_test_runner.lt_test_result.TEST_FAILED # The default is failure in case an exception is thrown.

        try:
            test_result = await tr.run(args.firmware, args.message_timeout, args.total_timeout)
        except serial.SerialException as e:
            logger.error(f"Platform serial interface communication error: {str(e)}")
            return lt_test_runner.lt_test_result.TEST_FAILED
        except FileNotFoundError:
            logger.error("Couldn't initialize OpenOCD.")
            return lt_test_runner.lt_test_result.TEST_FAILED
        except asyncio.exceptions.TimeoutError:
            logger.error(f"Received timeout!")
            return lt_test_runner.lt_test_result.TEST_FAILED
        except:
            logger.info("Received unexpected exception or termination request. Shutting down.")
            raise
        else:
            return test_result
        finally: # Always run proper shutdown configuration on OpenOCD to set ports to correct values.
            logger.info("Connecting to FTDI to shut down platform power gracefully...")
            adapter_id = lt_environment_tools.get_adapter_id_from_mapping(args.platform_id, args.mapping_config)
            if adapter_id is None:
                logger.error(f"Selected platform has not its adapter ID assigned yet in the config file. Assign the ID in {args.mapping_config} (or select correct config file) and try again.")
                raise ValueError
            with lt_openocd_launcher(["-f", args.adapter_shutdown_config, "-c", f"ftdi vid_pid {adapter_id.vid:#x} {adapter_id.pid:#x}"]) as oocd:
                time.sleep(5)
                if oocd.is_running():
                    logger.info("Platform board shut down.")

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