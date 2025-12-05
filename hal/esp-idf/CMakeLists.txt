cmake_minimum_required(VERSION 3.21.0)

set(LT_HAL_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/libtropic_port_esp_idf.c
)

set(LT_HAL_INC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# export generic names for parent to consume
set(LT_HAL_SRCS ${LT_HAL_SRCS} PARENT_SCOPE)
set(LT_HAL_INC_DIRS ${LT_HAL_INC_DIRS} PARENT_SCOPE)