cmake_minimum_required(VERSION 3.21.0)


###########################################################################
#                                                                         #
#   Define project's name                                                 #
#                                                                         #
###########################################################################

set(TARGET stm32_example)


###########################################################################
#                                                                         #
#   Define toolchaion file and linker script                              #
#                                                                         #
###########################################################################

set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/STM32F429ZITX_FLASH.ld)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/toolchain.cmake)


###########################################################################
#                                                                         #
#   Define project                                                        #
#                                                                         #
###########################################################################

project(${TARGET} LANGUAGES C ASM)
SET_PROPERTY(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS true)

###########################################################################
#                                                                         #
#   Add and configure libtropic for this exmaple                          #
#                                                                         #
###########################################################################

# This example is compiled for stm32F439, UART transport layer is set as default
set(TS_HAL_STM32_SPI ON)
#set(TS_HAL_STM32_UART ON)
# Use trezor crypto as a source of backend cryptography code
set(USE_TREZOR_CRYPTO ON)
# Provide path to a root of libtropic repository
add_subdirectory(../../ "libtropic")


###########################################################################
#                                                                         #
#   Example's source files                                                #
#                                                                         #
###########################################################################

# Path to cloned STM32CubeF4 repository
set(PATH_TO_STM32CUBEF4 "../../vendor" )
# Openocd config file for used nucleo board
set(OPENOCD_CFG "/usr/share/openocd/scripts/board/stm32f429discovery.cfg")

# Fill in your source files here (duh).
set(SOURCES
    Src/main.c
    Src/stm32f4xx_it.c
    Src/stm32f4xx_hal_msp.c
    Src/system_stm32f4xx.c
    System/Startup/startup_stm32f429zitx.s
    System/User/syscalls.c
    System/User/sysmem.c
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
)
if(TS_HAL_STM32_SPI)
    add_definitions(-DHAL_SPI)
    message(INFO "Using SPI as a transport layer")
    set(SOURCES ${SOURCES} ../../hal/spi/ts_l1_stm32_spi.c)
    set(SOURCES ${SOURCES} ../../hal/system/ts_random_stm32.c)
elseif(TS_HAL_STM32_UART)
    add_definitions(-DHAL_UART)
    message(WARNING "Using UART as a transport layer")
    set(SOURCES ${SOURCES} ../../hal/spi/ts_l1_stm32_uart.c)
else()
    message(FATAL_ERROR "No transport layer selected")
endif()

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
#   Example's compilation                                                 #
#                                                                         #
###########################################################################

# Add this so HAL knows what target MCU we use
add_definitions(-DSTM32F439xx)
add_definitions(-DUSE_TREZOR_CRYPTO) # TODO unify and fix this definition with option

# Compile the sources to an .elf
add_executable(${TARGET}.elf ${SOURCES} ${HEADERS})

# Link with libtropic
target_link_options(${TARGET}.elf PRIVATE -Wl,--gc-sections -Wl,--strip-all)
target_link_libraries(${TARGET}.elf PRIVATE tropic)

# Create a binary from the elf
add_custom_command(TARGET ${TARGET}.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -Oihex ${TARGET}.elf ${TARGET}.hex
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_command(TARGET ${TARGET}.elf
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ARGS -O binary ${TARGET}.elf ${TARGET}.bin
)

add_custom_target(flash
    COMMAND openocd -f ${OPENOCD_CFG} -c 'program ${TARGET}.elf verify reset exit'
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${TARGET}.elf
)
