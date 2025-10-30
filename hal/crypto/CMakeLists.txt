cmake_minimum_required(VERSION 3.21.0)

# Used for choosing the correct crypto HAL
set(LT_CRYPTO_HAL "" CACHE STRING "Choose HAL files of the specified cryptography provider")
set_property(CACHE LT_CRYPTO_HAL PROPERTY STRINGS "trezor_crypto" "mbedtls_v4")

# Validate LT_CRYPTO_HAL
get_property(lt_crypto_hal_choices CACHE LT_CRYPTO_HAL PROPERTY STRINGS)
set(crypto_hal_valid FALSE)
foreach(crypto_hal_choice IN LISTS lt_crypto_hal_choices)
    if(LT_CRYPTO_HAL STREQUAL ${crypto_hal_choice})
        set(crypto_hal_valid TRUE)
        break()
    endif()
endforeach()
if (NOT crypto_hal_valid)
    message(FATAL_ERROR "Incorrect crypto HAL set to LT_CRYPTO_HAL!\nAvailable crypto HALs: ${lt_crypto_hal_choices}")
endif()

# Add the chosen backend directory
add_subdirectory(${LT_CRYPTO_HAL})

# Forward generic names to the consumer
set(LT_CRYPTO_HAL_SRCS ${LT_CRYPTO_HAL_SRCS} PARENT_SCOPE)
set(LT_CRYPTO_HAL_INC_DIRS ${LT_CRYPTO_HAL_INC_DIRS} PARENT_SCOPE)