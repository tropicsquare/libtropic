import logging
from pathlib import Path
from .lt_platform import lt_platform

logger = logging.getLogger(__name__)

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
