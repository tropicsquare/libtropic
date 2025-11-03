cmake_minimum_required(VERSION 3.21.0)

# Used for choosing the correct CAL
set(LT_CAL "" CACHE STRING "Choose CAL files of the specified cryptographic functionality provider")
set_property(CACHE LT_CAL PROPERTY STRINGS "trezor_crypto" "mbedtls_v4")

# Validate LT_CAL
get_property(lt_cal_choices CACHE LT_CAL PROPERTY STRINGS)
set(cal_valid FALSE)
foreach(cal_choice IN LISTS lt_cal_choices)
    if(LT_CAL STREQUAL ${cal_choice})
        set(cal_valid TRUE)
        break()
    endif()
endforeach()
if (NOT cal_valid)
    message(FATAL_ERROR "Incorrect CAL set to LT_CAL!\nAvailable CALs: ${lt_cal_choices}")
endif()

# Add the chosen backend directory
add_subdirectory(${LT_CAL})

# Forward generic names to the consumer
set(LT_CAL_SRCS ${LT_CAL_SRCS} PARENT_SCOPE)
set(LT_CAL_INC_DIRS ${LT_CAL_INC_DIRS} PARENT_SCOPE)