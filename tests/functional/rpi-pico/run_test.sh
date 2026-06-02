#!/usr/bin/env bash

# --- 1. ARGUMENT VALIDATION ---
if [[ -z "${1:-}" || -z "${2:-}" ]]; then
    echo "libtropic ESP32 script for flashing and getting log from tests"
    echo "usage: ./run_test.sh BUILD_DIR TEST_NAME PICO_SERIAL_NUMBER"
    exit 1
fi

BUILD_DIR="$1"
TEST_NAME="$2"
PICO_SERIAL_NUMBER="$3"

BAUD="115200"
SERIAL_WAIT_TIMEOUT_S="10"
SERIAL_WAIT_POLL_S="0.2"
SENTINEL_OK="TEST FINISHED"
SENTINEL_FAIL_1="ASSERT FAIL"
SENTINEL_FAIL_2="WARNING"
SENTINEL_FAIL_3="ERROR"

set -euo pipefail

# --- 2. FLASH DEVICE ---
picotool load -x "${BUILD_DIR}/${TEST_NAME}.elf" --ser "${PICO_SERIAL_NUMBER}" -f

# --- 3. MONITOR (PTY wrapper, no timeout) ---
# Locate the serial device to get output from (may appear shortly after flashing).
wait_for_serial_device() {
    local start_ts
    start_ts=$(date +%s)

    while true; do
        PICO_SERIAL_DEVICE=$(find /dev/serial/by-id/ \( -type l -o -type c \) -iname "*${PICO_SERIAL_NUMBER}*" | head -n1)

        if [[ -n "${PICO_SERIAL_DEVICE}" ]] && [[ -e "${PICO_SERIAL_DEVICE}" ]]; then
            return 0
        fi

        if (( $(date +%s) - start_ts >= SERIAL_WAIT_TIMEOUT_S )); then
            return 1
        fi

        sleep "$SERIAL_WAIT_POLL_S"
    done
}

if ! wait_for_serial_device; then
    echo "Timed out (${SERIAL_WAIT_TIMEOUT_S}s) waiting for Pico board serial (serial number ${PICO_SERIAL_NUMBER})."
    exit 1
fi

echo "Using serial device: $PICO_SERIAL_DEVICE"

serial_reader() {
    GOT_ERROR=0
    exec 3<"$PICO_SERIAL_DEVICE"
    while IFS= read -t 60 -r -u 3 line; do
        printf '%s\n' "$line"

        if [[ "$line" == *"$SENTINEL_FAIL_1"* ]] \
        || [[ "$line" == *"$SENTINEL_FAIL_2"* ]] \
        || [[ "$line" == *"$SENTINEL_FAIL_3"* ]]; then
            GOT_ERROR=1
        elif [[ "$line" == *"$SENTINEL_OK"* ]]; then
            return $GOT_ERROR
        fi
    done
    return 2 # Timeout or serial read error
}

# Start serial reading in background
serial_reader &
READER_PID=$!

# Configure serial port
stty -F "$PICO_SERIAL_DEVICE" "$BAUD" \
  cs8 -cstopb -parenb \
  -ixon -ixoff -crtscts \
  -icanon -echo -echoe -echok -echoctl -echoke \
  -icrnl -inlcr -igncr -opost min 1 time 0 \
  -hupcl

# Ensure the background serial reader is killed on script termination
cleanup() {
    if [ -n "${READER_PID:-}" ]; then
        if kill -0 "$READER_PID" 2>/dev/null; then
            kill "$READER_PID" 2>/dev/null || true
            wait "$READER_PID" 2>/dev/null || true
        fi
    fi
}

# On Ctrl+C or TERM, kill the reader and exit with 130. Always run cleanup on EXIT.
trap 'cleanup; exit 130' INT TERM
trap 'cleanup' EXIT

# Wait for serial reader to finish.
set +e
wait $READER_PID
EXIT_CODE=$?
set -e

if [ $EXIT_CODE -eq 0 ]; then
    echo "Test finished successfully."
elif [ $EXIT_CODE -eq 1 ]; then
    echo "Test failed."
else
    echo "Serial read error or timeout."
fi

exit $EXIT_CODE