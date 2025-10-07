
import logging
from abc import ABC, abstractmethod
from enum import IntEnum
import telnetlib3
import asyncio

logger = logging.getLogger(__name__)

class lt_platform(ABC):
    class lt_led_color(IntEnum):
        BLACK = 0x0
        RED = 0x1,
        GREEN = 0x2,
        BLUE = 0x4,
        YELLOW = 0x3,
        CYAN = 0x6,
        MAGENTA = 0x5,
        WHITE = 0x7

    def __init__(self):
        pass
    
    def __del__(self):
        pass

    async def openocd_connect(self, timeout: float = 10) -> bool:
        logger.info("Opening connection to OpenOCD...")

        try:
            self.ocd_reader, self.ocd_writer = await telnetlib3.open_connection('localhost', 4444)
        except ConnectionRefusedError:
            return False

        await asyncio.wait_for(self.ocd_reader.readuntil("> ".encode('ascii')), timeout)
        logger.info("OpenOCD ready!")
        return True

    def openocd_disconnect(self):
        logger.info("Disconnecting from OpenOCD...")
        self.ocd_reader.close()
        self.ocd_writer.close()

    async def openocd_send(self, cmd: str, timeout: float = 5):
        self.ocd_writer.write(cmd)
        await asyncio.wait_for(self.ocd_writer.drain(), timeout)
    
    async def openocd_recv(self, timeout: float = 5) -> str:
        ocd_line = await asyncio.wait_for(self.ocd_reader.readline(), timeout)
        return ocd_line

    async def openocd_recv_match(self, match_str: str, timeout: float = 5) -> str:
        ocd_line = ""
        while not match_str in ocd_line:
            ocd_line = await asyncio.wait_for(self.ocd_reader.readline(), timeout)
        return ocd_line
    
    async def openocd_command(self, command_str : str) -> bool:
        try:
            await self.openocd_send(command_str)
        except:
            logging.error("Timeout when sending command!")
            return False
        
        try:
            output = await self.openocd_recv_match(command_str)
        except asyncio.exceptions.TimeoutError:
            logging.debug(f"Command response check failed, response: '{output}'")
            return False
        
        return True

    ####################################################
    # Common OpenOCD functions
    ####################################################
    async def set_disco_led(self, color: lt_led_color):
        # Get state of each LED for requested color and convert to 0 (off) or 1 (on).
        # This int(bool()) stuff is just to get 0 or 1 from the masked value (instead of shifting).
        await self.openocd_command(f"ftdi set_signal LED_R {int(bool(color.value & 0x1))}\r\n")
        await self.openocd_command(f"ftdi set_signal LED_G {int(bool(color.value & 0x2))}\r\n")
        await self.openocd_command(f"ftdi set_signal LED_B {int(bool(color.value & 0x4))}\r\n")

    async def blink_disco_led(self, color: lt_led_color, count = 3):
        for _ in range(count):
            await self.set_disco_led(color)
            await asyncio.sleep(0.1)
            await self.set_disco_led(self.lt_led_color.BLACK)
            await asyncio.sleep(0.1)        

    async def initialize(self) -> bool:
        return await self.openocd_command("init\r\n")

    async def set_spi_en(self, state: bool) -> bool:
        return await self.openocd_command(f"ftdi set_signal SPI_EN {int(state)}\r\n")

    async def is_spi_bus_free(self):
        return await self.openocd_command(f"ftdi get_signal SPI_EN_BUS\r\n")

    async def set_platform_power(self, state: bool):
        return await self.openocd_command(f"ftdi set_signal PLTF_PWR_EN {int(state)}\r\n")
            
    ####################################################
    # Platform-specific OpenOCD functions
    # 
    # These functions shall be specified in
    # child classes, as they differ for each platform.
    ####################################################
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