import logging
import fcntl
from pathlib import Path

logger = logging.getLogger(__name__)

class lt_lock_device:

    def __init__(self, lock_file: Path):
        self.lock_file = lock_file

    def acquire_lock(self) -> bool:
        self.lock_fd = open(self.lock_file, "w")
        try:
            fcntl.flock(self.lock_fd, fcntl.LOCK_EX | fcntl.LOCK_NB)
            return True
        except BlockingIOError:
            logging.error("Cannot acquire lock!")
            self.lock_fd.close()
            return False

    def release_lock(self) -> None:
        if not self.lock_fd:
            logging.error("Lock not aquired! Can't release.")
            return
        
        fcntl.flock(self.lock_fd, fcntl.LOCK_UN)
        self.lock_fd.close()