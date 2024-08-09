cmake_minimum_required(VERSION 3.21.0)


###########################################################################
#                                                                         #
#   Paths and setup                                                       #
#                                                                         #
###########################################################################

# Specify stm32 device
set(STM32_DEVICE "STM32F439xx")
# Openocd config file for used nucleo board
set(OPENOCD_CFG "/usr/share/openocd/scripts/board/stm32f429discovery.cfg")
# Path to linker script file
set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/STM32F429ZITX_FLASH.ld)
# Path to toolchain file
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/toolchain.cmake)
# Paths to stm32 drivers
set(PATH_TO_STM32CUBEF4 "drivers/" )
# Path to libtropic
set(PATH_TO_LIBTROPIC "../")
# Path to trezor_crypto
set(PATH_TREZOR_CRYPTO "../vendor/trezor_crypto/")

# Libtropic related setup, use trezor crypto library for host side cryptography
set(USE_TREZOR_CRYPTO ON)

# Possibly switch transport layer here:
#option(USE_UART "Use UART as a transport layer" ON)
#option(USE_SPI "Use SPI as a transport layer" OFF)

###########################################################################
#                                                                         #
#   Define project's name                                                 #
#                                                                         #
###########################################################################

project(stm32_example LANGUAGES C ASM)


###########################################################################
#                                                                         #
#   Sources                                                               #
#                                                                         #
###########################################################################

# Specify other source files
set(SOURCES ${SOURCES}
    Src/main.c
    Src/stm32f4xx_it.c
    Src/stm32f4xx_hal_msp.c
    Src/system_stm32f4xx.c
    System/Startup/startup_stm32f429zitx.s
    System/User/syscalls.c
    System/User/sysmem.c
    ${PATH_TO_LIBTROPIC}hal/platform/stm32/ts_platform_stm32.c
    # Sources related to STM32f439ZInucleo board
    ${PATH_TO_STM32CUBEF4}/STM32CubeF4/Drivers/BSP/STM32F4xx_Nucleo_144/stm32f4xx_nucleo_144.c
    # Sources for HAL functionalities used in this example
    ${PATH_TO_STM32CUBEF4}/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
    ${PATH_TO_STM32CUBEF4}/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c
    ${PATH_TO_STM32CUBEF4}/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
    ${PATH_TO_STM32CUBEF4}/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
    ${PATH_TO_STM32CUBEF4}/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
    ${PATH_TO_STM32CUBEF4}/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
    ${PATH_TO_STM32CUBEF4}/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c
    ${PATH_TO_STM32CUBEF4}/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rng.c
)

# Find all HAL header files
file(GLOB_RECURSE HAL_HEADERS ${PATH_TO_STM32CUBEF4}/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Inc/*.h)

set(HEADERS
    Inc/main.h
    Inc/stm32f4xx_hal_conf.h
    Inc/stm32f4xx_it.h
    # BSP header file
    ${PATH_TO_STM32CUBEF4}/STM32CubeF4/Drivers/BSP/STM32F4xx_Nucleo_144/stm32f4xx_nucleo_144.h
    # HAL headers
    ${HAL_HEADERS}
)

# Include path for directories containing header files
include_directories(
    Inc/
    ${PATH_TO_STM32CUBEF4}/STM32CubeF4/Drivers/BSP/STM32F4xx_Nucleo_144/
    ${PATH_TO_STM32CUBEF4}/STM32CubeF4/Drivers/CMSIS/Device/ST/STM32F4xx/Include/
    ${PATH_TO_STM32CUBEF4}/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Inc/
    ${PATH_TO_STM32CUBEF4}/STM32CubeF4/Drivers/CMSIS/Core/Include/
)


###########################################################################
#                                                                         #
#   Add executable and set definitions                                    #
#                                                                         #
###########################################################################

add_executable(stm32_example.elf ${SOURCES} ${HEADERS})

# Add this so HAL knows what target MCU we use
add_definitions(-D${STM32_DEVICE})
# Transport layer (USE_UART or USE_SPI):
if(USE_UART)
    target_compile_definitions(stm32_example.elf PRIVATE USE_UART)
else()
    target_compile_definitions(stm32_example.elf PRIVATE USE_SPI)
endif()

if(USE_TREZOR_CRYPTO)
    target_compile_definitions(stm32_example.elf PRIVATE USE_TREZOR_CRYPTO)
endif()


###########################################################################
#                                                                         #
#   Add libtropic                                                         #
#                                                                         #
###########################################################################

# Provide path to a root of libtropic repository
add_subdirectory(${PATH_TO_LIBTROPIC} "libtropic")

#target_link_options(stm32_example.elf PRIVATE -Wl,--gc-sections -Wl,--strip-all)
target_link_libraries(stm32_example.elf PRIVATE tropic)


###########################################################################
#                                                                         #
#   Compilation outputs                                                   #
#                                                                         #
###########################################################################

# Create a binary from the elf
add_custom_command(TARGET stm32_example.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -Oihex stm32_example.elf stm32_example.hex
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_command(TARGET stm32_example.elf
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ARGS -O binary stm32_example.elf stm32_example.bin
)

add_custom_target(flash
    COMMAND openocd -f ${OPENOCD_CFG} -c 'program stm32_example.elf verify reset exit'
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS stm32_example.elf
)
