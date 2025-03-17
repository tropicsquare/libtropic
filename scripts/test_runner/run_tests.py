from pathlib import Path
import asyncio
import logging

from lt_test_runner.lt_test_runner import lt_test_runner

def build_test():
    pass

# Very basic demonstration for now.
async def main():
    work_dir       = Path.cwd() / "build" / "system_tests"
    mapping_config = Path(__file__).parent / "lt_test_runner" / "adapter_mapping.csv"
    adapter_config = Path(__file__).parent / "lt_test_runner" / "ts11-jtag.cfg"
    tests_dir      = Path.cwd() / "tests" / "platforms" / "libtropic-stm32"
    
    logging.basicConfig(level=logging.INFO)

    tr = lt_test_runner(work_dir, "stm32_f439zi", mapping_config, adapter_config)
    await tr.run(tests_dir / "NUCLEO_F439ZI" / "build" / "stm32_example.elf")

if __name__ == "__main__":
    asyncio.run(main())