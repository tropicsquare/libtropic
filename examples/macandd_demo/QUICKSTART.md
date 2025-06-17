# Quick Start Guide

Get the MAC-and-Destroy PIN demo running in 5 minutes!

## 🚀 Fast Setup

### 1. Prerequisites Check
```bash
# Check if you have the required tools
gcc --version     # Should work
cmake --version   # Should be 3.10+
python3 --version # Should be 3.8+
```

### 2. Install Python Dependencies
```bash
pip install textual
```

### 3. Build C Server
```bash
# Navigate to libtropic project
cd <path>/libtropic/tests/model_based_project

# Quick build
rm -rf build/ && mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DLT_EX_MACANDD_PIN_DEMO=1 -DLT_SEPARATE_L3_BUFF=0 .. && make
cd ../
```

### 4. Run the Demo

**Terminal 1 - Start C Server:**
```bash
cd <path>/libtropic/tests/model_based_project
./build/run
```

**Terminal 2 - Start Python GUI:**
```bash
cd <path>/BTC_Prague/from_scratch
python3 macnd_demo_new.py
```

## 🎮 Quick Demo

1. **Default PIN**: `1234` (try entering it correctly)
2. **Test Wrong PIN**: Enter `0000` to see failure handling
3. **Configuration**: Go to "Configuration" tab, try PIN `5678` with 5 attempts
4. **Lock Test**: Enter wrong PIN until locked, watch countdown

## 📝 Default Settings

- **PIN**: 1234
- **Max Attempts**: 3
- **Port**: 12345
- **PIN Length**: 4 digits (fixed)

## 🔍 What You'll See

**C Server Output:**
```
🌴 TCP Server listening on port 12345
🌴 Waiting for Python client connection...
🌴 Client connected!
```

For detailed documentation, see [README.md](README.md) and [TECHNICAL.md](TECHNICAL.md).

---
**🌴 TropicSquare MAC-and-Destroy Demo** - Ready to explore secure PIN verification! 