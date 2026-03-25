"""Registry of available CLI commands."""

# Import new commands here:
from .pin_set import PIN_SET_SPEC
from .pin_verify import PIN_VERIFY_SPEC

# Add new commands here:
COMMAND_SPECS = {
    PIN_SET_SPEC.name: PIN_SET_SPEC,
    PIN_VERIFY_SPEC.name: PIN_VERIFY_SPEC,
}
