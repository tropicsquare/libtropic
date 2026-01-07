#!/usr/bin/env bash
set -euo pipefail

URL="https://github.com/tropicsquare/ts-tvl/releases/download/2.3/tvl-2.3-py3-none-any.whl"

# Resolve script directory and venv path
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VENV_DIR="$SCRIPT_DIR/.venv"

echo "Using script dir: $SCRIPT_DIR"

echo "Checking for download tool (curl or wget)..."
if command -v curl >/dev/null 2>&1; then
    DL_TOOL=curl
elif command -v wget >/dev/null 2>&1; then
    DL_TOOL=wget
else
    echo "Error: neither curl nor wget is installed. Please install curl or wget and retry." >&2
    exit 1
fi

echo "Selecting Python interpreter (prefer python3.8)..."
if command -v python3.8 >/dev/null 2>&1; then
    PYTHON=python3.8
elif command -v python3 >/dev/null 2>&1; then
    PYTHON=python3
elif command -v python >/dev/null 2>&1; then
    PYTHON=python
else
    echo "Error: no Python interpreter found (python3.8 / python3 / python)." >&2
    exit 1
fi

echo "Using Python: $PYTHON"

# Verify selected Python is Python 3
if ! $PYTHON -c "import sys
if sys.version_info.major != 3:
    raise SystemExit(2)"; then
    echo "Error: selected Python is not Python 3." >&2
    exit 1
fi

PY_VER=$($PYTHON -c 'import sys;print("{}.{}".format(sys.version_info.major, sys.version_info.minor))')
echo "Python version: $PY_VER"

echo "Creating virtual environment at $VENV_DIR (if missing)..."
if [ -d "$VENV_DIR" ]; then
    echo "Virtual environment already exists. Re-using it." 
else
    $PYTHON -m venv "$VENV_DIR"
fi

echo "Activating virtual environment..."
source "$VENV_DIR/bin/activate"

echo "Upgrading pip, setuptools and wheel in venv..."
pip install --upgrade pip setuptools wheel

TMP_WHEEL="$SCRIPT_DIR/${URL##*/}"
echo "Downloading wheel to $TMP_WHEEL..."
if [ "$DL_TOOL" = "curl" ]; then
    curl -fL "$URL" -o "$TMP_WHEEL"
else
    wget -O "$TMP_WHEEL" "$URL"
fi

echo "Installing wheel into virtualenv..."
pip install "$TMP_WHEEL"

echo "Cleaning up..."
rm -f "$TMP_WHEEL"

echo -e "\033[0;32mTROPIC01 Model Installation complete."
echo -e "Virtual environment is at: $VENV_DIR"
echo -e "\033[1;32mTo activate it: \033[0msource $VENV_DIR/bin/activate"

exit 0
