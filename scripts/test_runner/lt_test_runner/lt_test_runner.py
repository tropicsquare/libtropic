import logging
from enum import Enum
from pathlib import Path
import serial

from .lt_platform import lt_platform

logger = logging.getLogger(__name__)

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

        await self.platform.initialize()
        await self.platform.set_platform_power(True)

        if not await self.platform.is_spi_bus_free():
            logger.error("SPI bus is already occupied! Check if all other platform boards disabled SPI access!")
            self.platform.openocd_disconnect()
            return self.lt_test_result.TEST_FAILED

        await self.platform.set_spi_en(True)

        await self.platform.blink_disco_led(lt_platform.lt_led_color.WHITE)
        await self.platform.set_disco_led(lt_platform.lt_led_color.WHITE)

        try:
            await self.platform.load_elf(elf_path)
        except TimeoutError:
            logger.error("Communication with OpenOCD timed out while loading firmware!")
            self.platform.openocd_disconnect()
            return self.lt_test_result.TEST_FAILED

        err_count = 0
        warn_count = 0
        assert_fail_count = 0
        try:
            with serial.Serial(self.serial_port, baudrate = 115200, timeout=10, inter_byte_timeout=10, exclusive=True) as s:
                await self.platform.reset()
            
                while True:
                    if not s.is_open:
                        logger.error("Serial port closed! Check if it is not used by other application (screen, GTKTerm...).")
                        break

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

        logger.info("------ Stats ------")
        logger.info(f"Errors: {err_count}")
        logger.info(f"Warnings: {warn_count}")
        logger.info(f"Failed assertions: {assert_fail_count}")

        if err_count > 0 or warn_count > 0 or assert_fail_count > 0:
            logger.error("There were errors or warnings, test unsuccessful!")
            await self.platform.set_disco_led(lt_platform.lt_led_color.RED)
            self.platform.openocd_disconnect()
            return self.lt_test_result.TEST_FAILED

        await self.platform.set_disco_led(lt_platform.lt_led_color.GREEN)
        self.platform.openocd_disconnect()
        return self.lt_test_result.TEST_PASSED