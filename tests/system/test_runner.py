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
from abc import ABC, abstractmethod
from pathlib import Path

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

def lt_platform_factory(platform_string_id: str, *args) -> lt_platform:
    match platform_string_id:
        case "stm32":
            return lt_platform_stm32()

class lt_test_runner:
    class OpenOCDConnectionError:
        pass

    def __init__(self, working_dir: Path, platform: lt_platform, serial_port: str):
        self.working_dir = working_dir
        logger.info("Preparing environment...")
        self.working_dir.mkdir(exist_ok = True)
        self.platform = platform
        self.serial_port = serial_port

    def __del__(self):
        pass

    async def run(self, elf_path: Path):
        await self.platform.openocd_connect()

        # try:
        #     await self.platform.load_elf(elf_path)
        # except TimeoutError:
        #     logger.error("Communication with OpenOCD timed out while loading firmware!")

        with serial.Serial(self.serial_port, baudrate = 115200, timeout=10) as s:
            await self.platform.reset()
            # dummy code for now, just copies values from uart
            for i in range(30):
                logger.info(f"platform:{s.read_until(expected=b"\r\n")}") # readline, but we use CRLF (EOL), instead of just LF
            
        self.platform.openocd_disconnect()

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
    print("libtropic SLT start")
    logging.basicConfig(level=logging.INFO)

    # TODO dependency check

    platform_string_id = "stm32"
    firmware_path = Path("tests/system/stm32_example.elf")
    work_dir = Path("tests/system/build") # default should be 'build' in current dir
    adapter_config_path = Path(__file__).parent / "ts11-jtag.cfg"
    platform = lt_platform_factory(platform_string_id)
    adapter_mapping_path = Path("tests/system/adapter_mapping.csv") 
    adapter_id = lt_environment_tools.get_adapter_id_from_mapping(platform_string_id, adapter_mapping_path)
    adapter_serial = lt_environment_tools.get_serial_device_from_vidpid(adapter_id.vid, adapter_id.pid, 1) # TS11 adapter uses second interface for serial port.

    if adapter_id is None:
        logger.error(f"Selected platform has not its adapter ID assigned yet in the config file. Assign the ID in {adapter_mapping_path} (or select correct config file) and try again.")
        return

    if adapter_serial is None:
        logger.error("Serial adapter not found. Check if adapter is correctly connected to the computer and correct interface is selected.")
        return

    try:
        openocd_launch_params = ["-f", adapter_config_path] + ["-c", f"ftdi vid_pid {adapter_id.vid:#x} {adapter_id.pid:#x}"] + platform.get_openocd_launch_params() 
        logger.info(f"Launching OpenOCD with '{openocd_launch_params}'")
        openocd_proc = subprocess.Popen([shutil.which("openocd")] + openocd_launch_params, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

        await asyncio.sleep(2)

        if openocd_proc.poll() is not None:
            logger.info("Error starting OpenOCD. Check if computer-adapter-platform connection is functional or if OpenOCD is not already running.")
            return

        tr = lt_test_runner(work_dir, platform, adapter_serial)
        await tr.run(firmware_path)
    except serial.SerialException as e:
        logger.error(f"Platform serial interface communication error: {str(e)}")
        cleanup(openocd_proc)
        return
    except:
        logger.info("Received unexpected exception or termination request. Shutting down.")
        cleanup(openocd_proc)
        raise

    cleanup(openocd_proc)

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        logger.info("Interrupted by SIGINT.")
    sys.exit()