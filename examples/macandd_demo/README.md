# TROPIC01 MAC-and-Destroy PIN Demo

A comprehensive demonstration of the TROPIC01 security chip's MAC-and-Destroy PIN verification feature, consisting of a C-based secure backend and a Python-based graphical user interface.

![Demo Preview](docs/demo-preview.png)

## 🌴 Overview

This project demonstrates the **MAC-and-Destroy (M&D)** cryptographic feature of the TROPIC01 security chip, which provides secure PIN verification with hardware-enforced attempt limits. The system consists of two main components:

1. **C Server (`lt_ex_macandd_pin_demo.c`)** - Secure backend using TROPIC01 hardware
2. **Python Client (`macnd_demo_new.py`)** - Interactive terminal-based GUI

### What is MAC-and-Destroy?

MAC-and-Destroy is a cryptographic technique that provides secure authentication with limited attempts:

- **Initialization**: Multiple cryptographic slots are pre-initialized with encrypted secrets
- **Verification**: Each PIN attempt consumes one slot through a MAC operation
- **Destruction**: Failed attempts permanently "destroy" slots, making them unusable
- **Lockout**: When all slots are consumed, the system locks permanently until reset

This makes brute-force attacks computationally infeasible, as each failed attempt reduces the available attempts.

## 🛠️ System Architecture

```
┌─────────────────┐    TCP/IP     ┌─────────────────┐    Hardware    ┌─────────────────┐
│   Python GUI   │◄─────────────►│   C Server      │◄──────────────►│   TROPIC01      │
│  (macnd_demo    │   Port 12345  │  (libtropic)    │   SPI/I2C      │  Security Chip  │
│   _new.py)      │               │                 │                │                 │
└─────────────────┘               └─────────────────┘                └─────────────────┘
```

### Communication Protocol

| Direction | Command | Description | Response |
|-----------|---------|-------------|----------|
| Client → Server | `"1234"` | PIN verification attempt | `0x00` (success) / `0x01` (fail) |
| Client → Server | `"SET_PIN:5678"` | Change target PIN | `0x00` (success) / `0x01` (fail) |
| Client → Server | `"SET_ATTEMPTS:5"` | Set max attempts | `0x00` (success) / `0x01` (fail) |
| Server → Client | `"STATUS:ATTEMPTS:2:5"` | Current attempt status | N/A |
| Server → Client | `"STATUS:LOCKED:5:5"` | System locked | N/A |
| Server → Client | `"STATUS:RESET:0:5"` | Attempts reset | N/A |

## 📋 Prerequisites

### Hardware Requirements
- TROPIC01 security chip (or compatible emulator)
- Development board with SPI/I2C interface
- Linux development environment

### Software Requirements
- **C Development**: GCC, CMake 3.10+, libtropic library
- **Python GUI**: Python 3.8+, textual library
- **System**: Linux (tested on Ubuntu 20.04+)

## 🔧 Installation & Setup

### 1. Clone Repository
```bash
git clone <repository-url>
cd tropic01-macandd-demo
```

### 2. Install Python Dependencies
```bash
pip install textual
```

### 3. Setup libtropic Library
```bash
# Ensure libtropic is installed and available
# Copy demo files to libtropic examples directory
cp lt_ex_macandd_pin_demo.c /path/to/libtropic/examples/
```

### 4. Build C Server
```bash
cd /path/to/libtropic/tests/model_based_project

# Clean and build
rm -rf build/ && mkdir build && cd build

# Configure with MAC-and-Destroy demo enabled
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DLT_EX_MACANDD_PIN_DEMO=1 \
      -DLT_SEPARATE_L3_BUFF=0 ..

# Build the project
make

cd ../
```

## 🚀 Usage

### Starting the Demo

1. **Start the C Server** (Terminal 1):
```bash
cd /path/to/libtropic/tests/model_based_project
./build/run
```

You should see:
```
🌴🌴🌴 TROPIC01 Hello World FROM PIN DEMO 🌴🌴🌴
Creating secure session with H0 keys
Number of Mac And Destroy tries is set to 12
lt_PIN_set(): user sets the PIN
🌴 TCP Server listening on port 12345
🌴 Waiting for Python client connection...
```

2. **Start the Python GUI** (Terminal 2):
```bash
cd /path/to/demo/files
python3 macnd_demo_new.py
```

### Using the Interface

#### PIN Entry Tab 🌴
- **Keypad**: Click numbers or use keyboard (0-9)
- **Clear**: Clear current input
- **Backspace**: Remove last digit
- **Enter**: Submit 4-digit PIN for verification
- **New Session**: Reset to default settings

#### Configuration Tab ⚙️
- **Set PIN**: Configure target PIN (exactly 4 digits)
- **Max Attempts**: Set attempt limit (1-10)
- **Save Configuration**: Apply settings and sync with C server
- **Connect/Disconnect**: Manage connection to C server

### Demo Scenarios

#### Scenario 1: Successful Authentication
1. Enter correct PIN (default: `1234`)
2. System displays "GRANTED" countdown
3. New random PIN is generated automatically
4. Attempts reset to maximum

#### Scenario 2: Failed Attempts
1. Enter incorrect PIN
2. System shows "WRONG - X LEFT" 
3. Progress bar decreases
4. After max attempts: "LOCKED" countdown

#### Scenario 3: Configuration Changes
1. Go to Configuration tab
2. Set new PIN (e.g., `5678`)
3. Set new attempt limit (e.g., `5`)
4. Click "Save Configuration"
5. Settings sync with C server automatically

## 🔍 Technical Details

### Security Features

- **Hardware-Backed Security**: Uses TROPIC01 cryptographic operations
- **Key Derivation**: HMAC-SHA256 for key generation
- **Slot Management**: Each attempt consumes a cryptographic slot
- **Tamper Resistance**: Failed attempts permanently destroy slots
- **Session Management**: Secure initialization and cleanup

### File Structure
```
📁 tropic01-macandd-demo/
├── 📄 README.md                    # This documentation
├── 📄 lt_ex_macandd_pin_demo.c     # C server implementation
├── 📄 macnd_demo_new.py           # Python GUI client
├── 📄 macnd_demo_new.tcss         # GUI styling
├── 📄 harvested_pins.txt          # PIN attempt log (generated)
└── 📁 docs/
    └── 📄 demo-preview.png         # Screenshot
```

### Key Configuration Parameters

| Parameter | Default | Range | Description |
|-----------|---------|-------|-------------|
| `MACANDD_ROUNDS` | 12 | 1-128 | Maximum cryptographic slots |
| `MAX_ATTEMPTS` | 3 | 1-10 | Default attempt limit |
| `PIN_LENGTH` | 4 | 4 | PIN length (fixed) |
| `TCP_PORT` | 12345 | 1024-65535 | Communication port |

### Memory Usage

- **NVM Storage**: ~32 bytes per slot + metadata
- **R Memory**: 444 bytes (single slot usage)
- **Maximum slots**: Limited by available memory
- **Current demo**: 12 slots = ~400 bytes

## 🐛 Troubleshooting

### Common Issues

#### "Connection failed" Error
```bash
# Check if C server is running
ps aux | grep run

# Check port availability
netstat -ln | grep 12345

# Restart C server
./build/run
```

#### "Invalid PIN format" Error
- Ensure PIN is exactly 4 digits
- No letters or special characters
- Check configuration tab settings

#### Build Errors
```bash
# Clean build directory
rm -rf build/ && mkdir build

# Check CMake configuration
cmake --version  # Should be 3.10+

# Verify libtropic installation
ls /path/to/libtropic/examples/
```

### Known Limitations
- Single client support
- Localhost communication only
- Fixed 4-digit PIN length
- Limited to 12 attempts (demo configuration)

## 🙏 Acknowledgments

- **TropicSquare** for the TROPIC01 security chip
- **Textual** framework for the terminal UI
- **libtropic** library for hardware abstraction

---

**⚠️ Note**: This is a demonstration project. For production use, additional security hardening and testing are required. 