import logging
import fcntl
from pathlib import Path

logger = logging.getLogger(__name__)

class lt_lock_device:

    def __init__(self, lock_file: Path):
        self.lock_file = lock_file

    def __enter__(self) -> bool:
        try:
            self.lock_fd = self.lock_file.open("w")
        except PermissionError:
            self.lock_fd = None
            logger.error(f"Can't access {self.lock_file}! Check permissions.")
            return False
        
        try:
            fcntl.flock(self.lock_fd, fcntl.LOCK_EX | fcntl.LOCK_NB)
            return True
        except BlockingIOError:
            logger.error("Cannot acquire lock!")
            self.lock_fd.close()
            self.lock_fd = None
            try:
                self.lock_file.unlink(missing_ok = True)
            except PermissionError:
                logger.error("Cannot delete lockfile! Check permissions.")
            return False

    def __exit__(self, exc_type, exc_value, traceback):
        if not self.lock_fd:
            logger.info("Lock not aquired! Nothing to release.")
            return

        try:
            fcntl.flock(self.lock_fd, fcntl.LOCK_UN)
        except ValueError:
            logger.error("Lock is aquired, but can't release.")

        self.lock_fd.close()
        self.lock_fd = None

        try:
            self.lock_file.unlink(missing_ok = True)
        except PermissionError:
            logger.error("Couldn't delete lockfile! Check permissions.")