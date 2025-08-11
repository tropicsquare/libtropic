# Utility macro to convert DER to C array
function(generate_c_array FROM_HEX_VAR TO_C_ARRAY_VAR)
    string(REGEX MATCHALL ".." BYTE_LIST "${${FROM_HEX_VAR}}")
    string(REPLACE ";" ", 0x" HEX_STRING "0x${BYTE_LIST}")
    set(${TO_C_ARRAY_VAR} "${HEX_STRING}" PARENT_SCOPE)
endfunction()

# === Private Key ===
file(READ "${SH0_PRIV_RAW_PATH}" PRIV_BINARY HEX)
generate_c_array(PRIV_BINARY PRIV_C_ARRAY)

file(WRITE "${OUTPUT_C_FILE}" "#include <stdint.h>\n\nuint8_t sh0priv[] = {\n    ${PRIV_C_ARRAY}\n};\n")

# === Public Key ===
file(READ "${SH0_PUB_RAW_PATH}" PUB_BINARY HEX)
generate_c_array(PUB_BINARY PUB_C_ARRAY)

file(APPEND "${OUTPUT_C_FILE}" "\nuint8_t sh0pub[] = {\n    ${PUB_C_ARRAY}\n};\n")
