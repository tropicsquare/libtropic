import logging
import subprocess
import shutil
import threading

from typing import List, Union

logger = logging.getLogger(__name__)

# Handle OpenOCD using RAII pattern, so it is automatically terminated
# when leaving a scope.
class lt_openocd_launcher:
    
    @staticmethod
    def __read_stream(stream, logger: logging.Logger, log_level: int):
        """Reads lines from a stream and logs them until the stream closes."""
        for line in iter(stream.readline, b''): # Reads line by line until EOF. Binary mode => b''.
            if line.strip(): # Avoid logging empty lines
                line_text = line.strip().decode('utf-8')
                if log_level == logging.DEBUG:
                    logger.debug(f"OpenOCD output: {line_text}")
                elif log_level == logging.INFO:
                    logger.info(f"OpenOCD output: {line_text}")
                elif log_level == logging.ERROR:
                    logger.error(f"OpenOCD output: {line_text}")

    def cleanup(self):
        if self.openocd_proc.poll() is None:
            logger.info("Terminating OpenOCD...")
            self.openocd_proc.terminate()
            try:
                self.openocd_proc.wait(timeout=10)
            except subprocess.TimeoutExpired:
                logger.warning("OpenOCD did not terminate in time. Killing.")
                self.openocd_proc.kill()
            logger.info("OpenOCD terminated.")

        if self.output_log_thread.is_alive():
            # Wait for logging thread to terminate.
            self.output_log_thread.join()

    def __init__(self, openocd_params: List[str]):
        # This has to be list, hence [ ] (Popen accepts lists as arg).
        openocd_path = [shutil.which("openocd")]
        if openocd_path is None:
            raise FileNotFoundError("Couldn't find OpenOCD! Check if it is installed and available!")

        logger.debug(f"Launching OpenOCD with '{openocd_params}'")
        self.openocd_proc = subprocess.Popen(openocd_path + openocd_params, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

        self.output_log_thread = threading.Thread(
            target = self.__read_stream,
            args   = (self.openocd_proc.stdout, logger, logging.DEBUG)
        )
        self.output_log_thread.start()
    
    def __del__(self):
        self.cleanup()
            
    def is_running(self) -> bool:
        return self.openocd_proc.poll() is None