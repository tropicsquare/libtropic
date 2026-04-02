cmake_minimum_required(VERSION 3.21.0)
include(FetchContent)

# Pull nanopb runtime sources for host-side USB dongle HAL build.
FetchContent_Declare(
    nanopb
    URL https://github.com/nanopb/nanopb/releases/download/nanopb-0.4.9.1/nanopb-0.4.9.1.tar.gz
    URL_HASH SHA256=882cd8473ad932b24787e676a808e4fb29c12e086d20bcbfbacc66c183094b5c
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# This HAL consumes only nanopb runtime C sources, not protoc generation.
set(nanopb_BUILD_GENERATOR OFF CACHE BOOL "Build nanopb generator" FORCE)
set(nanopb_PROTOC_PATH ${CMAKE_COMMAND} CACHE FILEPATH "Path to protoc used by nanopb" FORCE)
FetchContent_MakeAvailable(nanopb)

set(LT_HAL_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/libtropic_port_posix_usb_devkit.c

    ${nanopb_SOURCE_DIR}/pb_common.c
    ${nanopb_SOURCE_DIR}/pb_encode.c
    ${nanopb_SOURCE_DIR}/pb_decode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../examples/linux/usb_devkit/tropic01_util/protobuf/generated/usb_devkit_messages.pb.c
)

set(LT_HAL_INC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}

    ${CMAKE_CURRENT_SOURCE_DIR}/../../../examples/linux/usb_devkit/tropic01_util/protobuf/generated/
    ${nanopb_SOURCE_DIR}
)

# export generic names for parent to consume
set(LT_HAL_SRCS ${LT_HAL_SRCS} PARENT_SCOPE)
set(LT_HAL_INC_DIRS ${LT_HAL_INC_DIRS} PARENT_SCOPE)