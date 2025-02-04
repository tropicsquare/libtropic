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
from enum import Enum
from abc import ABC, abstractmethod
from pathlib import Path
from argparse import ArgumentParser

logger = logging.getLogger(__name__)

class lt_platform(ABC):
    def __init__(self):
        pass
    
    def __del__(self):
        pass

    async def openocd_connect(self):
        logger.info("Opening connection to OpenOCD...")
        self.ocd_reader, self.ocd_writer = await telnetlib3.open_connection('localhost', 4444)

    def openocd_disconnect(self):
        logger.info("Disconnecting from OpenOCD...")
        self.ocd_reader.close()
        self.ocd_writer.close()

    async def openocd_send(self, cmd: str, timeout: float = 5):
        self.ocd_writer.write(cmd)
        await asyncio.wait_for(self.ocd_writer.drain(), timeout)

    async def openocd_recv_match(self, match_str: str, timeout: float = 5):
        ocd_line = ""
        while not match_str in ocd_line:
            ocd_line = await asyncio.wait_for(self.ocd_reader.readline(), timeout)
            logger.info(f"OpenOCD:{ocd_line[:-1]}")

    @abstractmethod
    def get_openocd_launch_params(self):
        pass

    @abstractmethod
    def load_elf(self):
        pass

    @abstractmethod
    def reset(self):
        pass

    # @abstractmethod
    # def reset(self):
    #     pass

    # @abstractmethod
    # def read_console(self):
    #     pass

class lt_platform_stm32(lt_platform):
    def __init__(self):
        super().__init__()

    def get_openocd_launch_params(self):
        return ["-f", "target/stm32f4x.cfg"]

    async def load_elf(self, elf_path: Path):
        logger.info("Programming...")
        await self.openocd_send(f"program {elf_path.absolute()} verify\n")
        await self.openocd_recv_match("** Verified OK **", 30)

    async def reset(self):
        logger.info("Resetting...")
        self.ocd_writer.write(f"reset\n")
        await self.ocd_writer.drain()
        logger.info("Reset issued.")

class lt_platform_factory:
    LT_PLATFORM_STRING_ID_MAPPING = {
        "stm32": lt_platform_stm32
    }
    
    @staticmethod
    def create_from_str_id(platform_string_id: str) -> lt_platform | None:
        try:
            return lt_platform_factory.LT_PLATFORM_STRING_ID_MAPPING[platform_string_id]()
        except KeyError:
            return None

class lt_test_runner:
    class lt_test_result(Enum):
        TEST_FAILED = -1
        TEST_PASSED = 0

    class OpenOCDConnectionError:
        pass

    def __init__(self, working_dir: Path, platform: lt_platform, serial_port: str):
        self.working_dir = working_dir
        logger.info("Preparing environment...")
        self.working_dir.mkdir(exist_ok = True, parents = True)
        self.platform = platform
        self.serial_port = serial_port

    def __del__(self):
        pass

    async def run(self, elf_path: Path) -> lt_test_result:
        await self.platform.openocd_connect()

        try:
            await self.platform.load_elf(elf_path)
        except TimeoutError:
            logger.error("Communication with OpenOCD timed out while loading firmware!")

        err_count = 0
        warn_count = 0
        assert_fail_count = 0
        try:
            with serial.Serial(self.serial_port, baudrate = 115200, timeout=10, inter_byte_timeout=10) as s:
                await self.platform.reset()
            
                while True:
                    line = s.read_until(expected=b"\r\n").decode('ascii', errors="backslashreplace")
                    logger.debug(f"Received from serial: {line}")
                    line = line.split(";")
                    if (len(line) < 3):
                        logger.error("Line malformed!")
                        continue
                    
                    code_line_number = line[0].strip()
                    msg_type = line[1].strip()
                    msg_content = line[2].strip()

                    if (msg_type == "INFO"):
                        logger.info(f"[PLATFORM][{code_line_number}] {msg_content}")
                    elif (msg_type == "WARNING"):
                        logger.warning(f"[PLATFORM][{code_line_number}] {msg_content}")
                        warn_count += 1
                    elif (msg_type == "ERROR"):
                        logger.error(f"[PLATFORM][{code_line_number}] {msg_content}")
                        err_count += 1
                    elif (msg_type == "SYSTEM"):
                        if msg_content == "ASSERT_OK":
                            logger.info(f"Assertion at [{code_line_number}] passed!")
                        elif msg_content == "ASSERT_FAIL":
                            logger.error(f"Assertion at [{code_line_number}] failed!")
                            assert_fail_count += 1
                        elif msg_content == "TEST_FINISH":
                            logger.info("Received TEST_FINISH, wrapping up.")
                            break
        except TimeoutError:
            logger.error("Serial timeout! Did not receive any message in time. Check if the test output is correct and if TEST_FINISH is issued at the end of the test.")

        self.platform.openocd_disconnect()

        logger.info("------ Stats ------")
        logger.info(f"Errors: {err_count}")
        logger.info(f"Warnings: {warn_count}")
        logger.info(f"Failed assertions: {assert_fail_count}")

        if err_count > 0 or warn_count > 0 or assert_fail_count > 0:
            logger.error("There were errors or warnings, test unsuccessful!")
            return self.lt_test_result.TEST_FAILED

        return self.lt_test_result.TEST_PASSED

def cleanup(openocd_proc: subprocess.Popen):
    logger.info("Cleaning up.")

    if openocd_proc.poll() is None:
        logger.info("Terminating OpenOCD...")
        openocd_proc.terminate()
        try:
            openocd_proc.wait(timeout=10)
        except subprocess.TimeoutExpired:
            logger.warning("OpenOCD did not terminate in time. Killing.")
            openocd_proc.kill()
        logger.info("Done. Bye.")

class lt_environment_tools:    
    @dataclass
    class lt_adapter_id:
        vid: int
        pid: int
    
    @staticmethod
    def get_adapter_id_from_mapping(platform_str: str, map_file_path: Path) -> lt_adapter_id | None:
        if not map_file_path.exists() or not map_file_path.is_file():
            logger.error("Config file not found!")
            return None
        with map_file_path.open() as f:
            if f.readline().rstrip("\n") != "platform;vid;pid":
                logger.error("Incorrect config file header!")
                return None
            while line := f.readline().rstrip("\n"):
                if len(line_separated := line.split(';')) != 3:
                    logger.error("Incorrect config file line format!")
                    return None
                if line_separated[0] == platform_str:
                    try:
                        return lt_environment_tools.lt_adapter_id(vid = int(line_separated[1], 16), pid = int(line_separated[2], 16))
                    except ValueError:
                        logger.error("Config file VID and PID have to be integer!")
                        return None
            logger.error("Platform not found in config file!")
            return None

    @staticmethod
    def get_serial_device_from_vidpid(vid: int, pid: int, interface: int = None) -> str | None:
        found_devices = []
        for port in serial.tools.list_ports.comports():
            if port.vid == vid and port.pid == pid:
                if interface == None: # If no specific interface required, return the first one.
                    logger.info(f"Found serial USB device with VID={vid:#x} PID={pid:#x}: {port.device}")
                    return port.device
                else:
                    found_devices.append(port)

        if len(found_devices) == 0:
            logger.error(f"No serial USB devices found with VID={vid:#x} PID={pid:#x}!")
            return None

        for _ in found_devices:
            try:
                if int(port.location.split(".")[-1]) == interface:
                    logger.info(f"Found serial USB device with VID={vid:#x} PID={pid:#x} INTERFACE={interface}: {port.device}")
                    return port.device
            except (ValueError, IndexError):
                pass

        logger.error(f"The selected USB device does not have INTERFACE={interface}!")
        return None

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
        cleanup(openocd_proc)
        return
    except:
        logger.info("Received unexpected exception or termination request. Shutting down.")
        cleanup(openocd_proc)
        raise

    cleanup(openocd_proc)

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