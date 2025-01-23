import logging
import subprocess
import shutil
import sys
import asyncio
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

async def main():
    print("libtropic SLT start")
    logging.basicConfig(level=logging.INFO)

    # TODO dependency check

    platform_string_id = "stm32"
    firmware_path = Path("tests/system/stm32_example.elf")
    work_dir = Path("tests/system/build")
    adapter_config_path = Path(__file__).parent / "ts11-jtag.cfg"
    platform = lt_platform_factory(platform_string_id)

    try:
        logger.info("Launching OpenOCD...")
        openocd_launch_params = ["-f", adapter_config_path] + platform.get_openocd_launch_params()
        openocd_proc = subprocess.Popen([shutil.which("openocd")] + openocd_launch_params, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

        await asyncio.sleep(2)

        if openocd_proc.poll() is not None:
            logger.info("Error starting OpenOCD. Check if computer-adapter-platform connection is functional or if OpenOCD is not already running.")
            return

        tr = lt_test_runner(work_dir, platform, "/dev/ttyUSB1")
        await tr.run(firmware_path)
    except:
        logger.info("Received exception or termination request. Shutting down.")
        cleanup(openocd_proc)
        raise

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        logger.info("Interrupted by SIGINT.")
    sys.exit()