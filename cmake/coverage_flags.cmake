# This file defines an INTERFACE library that carries coverage flags.

# Create the INTERFACE library with the requested name.
add_library(libtropic_coverage_flags INTERFACE)

# Create the desired namespaced ALIAS for it.
# Consumers will link against this clean name.
add_library(libtropic::coverage_flags ALIAS libtropic_coverage_flags)

# Add the desired compile options to its INTERFACE.
# Any target that links to this will inherit these flags.
target_compile_options(libtropic_coverage_flags INTERFACE
    --coverage
    -g
    -O0
)

target_link_options(libtropic_coverage_flags INTERFACE
    --coverage
)