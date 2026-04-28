"""Registry of available CLI commands."""

# Import new commands here:
from .pin_set import PIN_SET_SPEC
from .pin_verify import PIN_VERIFY_SPEC
from .r_mem_erase import R_MEM_ERASE_SPEC
from .r_mem_read import R_MEM_READ_SPEC
from .r_mem_write import R_MEM_WRITE_SPEC
from .get_rand_bytes import GET_RAND_BYTES_SPEC

# Add new commands here:
COMMAND_SPECS = {
    PIN_SET_SPEC.name: PIN_SET_SPEC,
    PIN_VERIFY_SPEC.name: PIN_VERIFY_SPEC,
    R_MEM_READ_SPEC.name: R_MEM_READ_SPEC,
    R_MEM_WRITE_SPEC.name: R_MEM_WRITE_SPEC,
    R_MEM_ERASE_SPEC.name: R_MEM_ERASE_SPEC,
    GET_RAND_BYTES_SPEC.name: GET_RAND_BYTES_SPEC
}
