import logging
import subprocess
import shutil

from typing import List, Union

logger = logging.getLogger(__name__)

# Handle OpenOCD using RAII pattern, so it is automatically terminated
# when leaving a scope.
class lt_openocd_launcher:
    def __init__(self, openocd_params: List[str]):
        # This has to be list, hence [ ] (Popen accepts lists as arg).
        openocd_path = [shutil.which("openocd")]
        if openocd_path is None:
            raise FileNotFoundError("Couldn't find OpenOCD! Check if it is installed and available!")

        logger.debug(f"Launching OpenOCD with '{openocd_params}'")
        self.openocd_proc = subprocess.Popen(openocd_path + openocd_params, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    
    def __del__(self):
        if self.openocd_proc.poll() is None:
            logger.info("Terminating OpenOCD...")
            self.openocd_proc.terminate()
            try:
                self.openocd_proc.wait(timeout=10)
            except subprocess.TimeoutExpired:
                logger.warning("OpenOCD did not terminate in time. Killing.")
                self.openocd_proc.kill()
            logger.info("OpenOCD terminated.")
            
    def is_running(self) -> bool:
        return self.openocd_proc.poll() is None