# This file defines an INTERFACE library that carries all of our
# project-wide compile and link flags for static ASan.

# Create the INTERFACE library with the requested name.
add_library(libtropic_static_asan_flags INTERFACE)

# Create the desired namespaced ALIAS for it.
# Consumers will link against this clean name.
add_library(libtropic::static_asan_flags ALIAS libtropic_static_asan_flags)

# Add the desired compile options to its INTERFACE.
# Any target that links to this will inherit these flags.
target_compile_options(libtropic_static_asan_flags INTERFACE
    -fsanitize=address,leak,undefined
    -fno-omit-frame-pointer
    -g
)

target_link_options(libtropic_static_asan_flags INTERFACE
    -fsanitize=address,leak,undefined
    -static-libasan
)