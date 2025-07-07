import logging
from enum import Enum
from pathlib import Path
import serial
import time

from .lt_platform_factory import lt_platform_factory
from .lt_platform import lt_platform
from .lt_environment_tools import lt_environment_tools
from .lt_openocd_launcher import lt_openocd_launcher

logger           = logging.getLogger(__name__)
logger_runner    = logging.getLogger("RUNNER  ")
logger_platform  = logging.getLogger("PLATFORM")

class lt_test_runner:
    class lt_test_result(Enum):
        TEST_FAILED = -1
        TEST_PASSED = 0

    def __init__(self, working_dir: Path, platform_id: str, mapping_config_path: Path, adapter_config_path: Path):
        self.working_dir = working_dir
        logger.info("Preparing environment...")
        self.working_dir.mkdir(exist_ok = True, parents = True)

        self.platform = lt_platform_factory.create_from_str_id(platform_id)
        if self.platform is None:
            logger.error(f"Platform '{self.platform}' not found!")
            raise ValueError
        
        adapter_id = lt_environment_tools.get_adapter_id_from_mapping(platform_id, mapping_config_path)
        if adapter_id is None:
            logger.error(f"Selected platform has not its adapter ID assigned yet in the config file. Assign the ID in {mapping_config_path} (or select correct config file) and try again.")
            raise ValueError
    
        logger.info("Finding serial interface...")
        self.serial_port  = lt_environment_tools.get_serial_device_from_vidpid(adapter_id.vid, adapter_id.pid, 1) # TS11 adapter uses second interface for serial port.
        if self.serial_port is None:
            logger.error("Serial adapter not found. Check if adapter is correctly connected to the computer and correct interface is selected.")
            raise ValueError
        
        self.openocd_launch_params = ["-f", adapter_config_path] + ["-c", f"ftdi vid_pid {adapter_id.vid:#x} {adapter_id.pid:#x}"] + self.platform.get_openocd_launch_params() 

    async def __parse_output(self, message_timeout: int, total_timeout: int) -> lt_test_result:
        err_count = 0
        warn_count = 0
        assert_fail_count = 0
        comm_error_count = 0
        # This flag is needed to check whether there is any assert in the test at all.
        # If there is no assert it is probably a mistake in test design, as the test can't detect
        # errors (and as such, is useless).
        reached_assert_flag = False

        start_time = time.time()
        
        with serial.Serial(self.serial_port, baudrate = 115200, timeout=message_timeout, inter_byte_timeout=None, exclusive=True) as s:
            await self.platform.reset()
        
            ########################################################################################
            # BEGINNING OF TEST OUTPUT
            # In this section, use dedicated logger_runner and logger_platform to distinguish
            # platform and runner output and to make logs easier to read.
            ########################################################################################
            logger_runner.info("!!!!!!!!!!!!!!!! TEST BEGIN !!!!!!!!!!!!!!!!")
            
            while True:
                try:
                    if total_timeout != 0 and time.time() - start_time > total_timeout:
                        logger_runner.error("Total test time exceeded! Aborting.")
                        comm_error_count += 1
                        break

                    if not s.is_open:
                        logger_runner.error("Serial port closed! Check if it is not used by other application (screen, GTKTerm...).")
                        comm_error_count += 1
                        break

                    line = s.read_until(expected=b"\r\n").decode('ascii', errors="backslashreplace")

                    if (len(line) == 0):
                        logger_runner.error("Did not receive message in time (message_timeout exceeded).")
                        comm_error_count += 1
                        break

                    logger_runner.debug(f"Received from serial: {line}")
                    line = line.split(";")

                    if (len(line) < 3):
                        logger_runner.error(f"Line malformed!")
                        comm_error_count += 1
                        continue
                    
                    try:
                        code_line_number = f"{int(line[0].strip()) : 4d}"
                    except ValueError:
                        logger_runner.error(f"Line malformed (line number not an integer)!")
                        comm_error_count += 1

                    msg_type = line[1].strip()
                    msg_content = line[2].strip()

                    if (msg_type == "INFO"):
                        logger_platform.info(f"[{code_line_number}] {msg_content}")
                    elif (msg_type == "WARNING"):
                        logger_platform.warning(f"[{code_line_number}] {msg_content}")
                        warn_count += 1
                    elif (msg_type == "ERROR"):
                        logger_platform.error(f"[{code_line_number}] {msg_content}")
                        err_count += 1
                    elif (msg_type == "SYSTEM"):
                        if msg_content == "ASSERT_OK":
                            logger_runner.info(f"[{code_line_number}] Assertion passed!")
                            reached_assert_flag = True
                        elif msg_content == "ASSERT_FAIL":
                            if (len(line) < 5):
                                logger_runner.error(f"Line malformed!")
                                comm_error_count += 1
                                continue
                            msg_expected = line[4].strip()
                            msg_got = line[3].strip()
                            logger_runner.error(f"[{code_line_number}] Assertion failed! Expected '{msg_expected}', got '{msg_got}'.")
                            assert_fail_count += 1
                            reached_assert_flag = True
                        elif msg_content == "TEST_FINISH":
                            logger_runner.info(f"[{code_line_number}] Received TEST_FINISH, wrapping up.")
                            break
                except TimeoutError:
                    logger_runner.error(f"Serial timeout! Did not receive any message in time. Check if the test output is correct and if TEST_FINISH is issued at the end of the test.")
                    comm_error_count += 1

        if not reached_assert_flag:
            logger_runner.error("There was no assert in the test! Check whether the test is designed correctly! (No assert = useless test!)")
            err_count += 1

        logger_runner.info("!!!!!!!!!!!!!!!! TEST END !!!!!!!!!!!!!!!!")
        ########################################################################################
        # END OF TEST OUTPUT
        # Here you return to generic logger.
        ########################################################################################

        logger.info("------ Stats ------")
        logger.info(f"Errors: {err_count}")
        logger.info(f"Warnings: {warn_count}")
        logger.info(f"Failed assertions: {assert_fail_count}")
        logger.info(f"Communication errors (malformed lines, timeouts): {comm_error_count}")

        if err_count > 0 or warn_count > 0 or assert_fail_count > 0 or comm_error_count > 0:
            logger.error("There were errors or warnings, test unsuccessful!")
            await self.platform.set_disco_led(lt_platform.lt_led_color.RED)
            await self.platform.set_platform_power(False)
            self.platform.openocd_disconnect()
            return self.lt_test_result.TEST_FAILED

        await self.platform.set_disco_led(lt_platform.lt_led_color.GREEN)
        await self.platform.set_platform_power(False)
        self.platform.openocd_disconnect()
        return self.lt_test_result.TEST_PASSED

    async def run(self, elf_path: Path, message_timeout: int, total_timeout: int) -> lt_test_result:
        
        if message_timeout == 0:
            # pyserial expects None for no timeout, 0 sets non-blocking mode
            message_timeout = None

        with lt_openocd_launcher(self.openocd_launch_params) as oocd:
            logger.info("Launching OpenOCD...")
            time.sleep(2)
            if not oocd.is_running():
                logger.error("Couldn't launch OpenOCD. Hint: run with higher verbosity (-v) to check OpenOCD output.")
                return self.lt_test_result.TEST_FAILED

            if not await self.platform.openocd_connect():
                logger.error("Couldn't connect to OpenOCD!")
                return self.lt_test_result.TEST_FAILED

            await self.platform.initialize()

            if not await self.platform.is_spi_bus_free():
                logger.error("SPI bus is already occupied! Check if all other platform boards disabled SPI access!")
                self.platform.openocd_disconnect()
                return self.lt_test_result.TEST_FAILED

            await self.platform.set_spi_en(True)
            await self.platform.set_platform_power(True)

            await self.platform.blink_disco_led(lt_platform.lt_led_color.WHITE)
            await self.platform.set_disco_led(lt_platform.lt_led_color.WHITE)

            try:
                await self.platform.load_elf(elf_path)
            except TimeoutError:
                logger.error("Communication with OpenOCD timed out while loading firmware!")
                await self.platform.set_platform_power(False)
                self.platform.openocd_disconnect()
                return self.lt_test_result.TEST_FAILED

            return await self.__parse_output(message_timeout, total_timeout)