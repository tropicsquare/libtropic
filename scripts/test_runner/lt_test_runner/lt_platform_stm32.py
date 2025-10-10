import logging
from pathlib import Path
from .lt_platform import lt_platform

logger = logging.getLogger(__name__)

class lt_platform_stm32(lt_platform):
    def __init__(self):
        super().__init__()

    async def load_elf(self, elf_path: Path) -> bool:
        if not await self.openocd_command(f"program {elf_path.absolute()} verify\r\n"):
            return False
        return await self.openocd_recv_match("** Verified OK **", 30)

    async def reset(self) -> bool:
        return await self.openocd_command(f"reset\r\n")

class lt_platform_stm32_f4(lt_platform_stm32):
    def get_openocd_launch_params(self):
        return ["-f", "target/stm32f4x.cfg"]
    
class lt_platform_stm32_l4(lt_platform_stm32):
    def get_openocd_launch_params(self):
        return ["-f", "target/stm32l4x.cfg", "-c", "transport select swd"]