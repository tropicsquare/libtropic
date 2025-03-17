import logging
import subprocess
from dataclasses import dataclass
from pathlib import Path
import serial
import serial.tools.list_ports

logger = logging.getLogger(__name__)

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
    
    @staticmethod
    def shutdown_openocd(openocd_proc: subprocess.Popen):
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