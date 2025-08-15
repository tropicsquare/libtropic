# This file defines an INTERFACE library that carries all of our
# project-wide strict compile flags.

# Create the INTERFACE library with the requested name.
add_library(libtropic_strict_comp_flags INTERFACE)

# Create the desired namespaced ALIAS for it.
# Consumers will link against this clean name.
add_library(libtropic::strict_comp_flags ALIAS libtropic_strict_comp_flags)

# Add the desired compile options to its INTERFACE.
# Any target that links to this will inherit these flags.
target_compile_options(libtropic_strict_comp_flags INTERFACE
    -Wall
    -Wextra
    -Werror
    -Wpedantic
    -Wshadow
    -Wformat=2
    -Wcast-align
    -Wfloat-equal
    -Wnull-dereference
    -Wredundant-decls
    -Wlogical-op
    -Wstrict-prototypes
    -Wunused-result
    -Wmissing-prototypes
    -fstack-protector-strong
)