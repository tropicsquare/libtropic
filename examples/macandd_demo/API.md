# API Documentation

Communication protocol between Python GUI client and C server for MAC-and-Destroy PIN verification.

## Connection Details

- **Protocol**: TCP/IP
- **Host**: 127.0.0.1 (localhost)
- **Port**: 12345
- **Encoding**: ASCII
- **Connection**: Single client, persistent connection

## Message Types

### 1. PIN Verification Request

**Direction**: Client → Server  
**Format**: ASCII string containing exactly 4 digits  
**Example**: `"1234"`

```python
# Python client code
client_socket.sendall("1234".encode('ascii'))
```

**Server Response**: Single byte
- `0x00` (LT_OK) - PIN verification successful
- `0x01` (LT_FAIL) - PIN verification failed

**Behavior**:
- On success: Secret is released, system resets
- On failure: Server sends status update (see below)

### 2. PIN Configuration Request

**Direction**: Client → Server  
**Format**: `"SET_PIN:"` followed by exactly 4 digits  
**Example**: `"SET_PIN:5678"`

```python
# Python client code
sync_command = f"SET_PIN:{new_pin}"
client_socket.sendall(sync_command.encode('ascii'))
```

**Server Response**: Single byte
- `0x00` (LT_OK) - PIN configuration successful
- `0x01` (LT_FAIL) - PIN configuration failed

**Side Effects**:
- Reinitializes MAC-and-Destroy slots
- Resets attempt counter to 0
- Unlocks system if previously locked

### 3. Attempts Configuration Request

**Direction**: Client → Server  
**Format**: `"SET_ATTEMPTS:"` followed by 1-2 digits  
**Example**: `"SET_ATTEMPTS:5"`

```python
# Python client code
sync_command = f"SET_ATTEMPTS:{new_attempts}"
client_socket.sendall(sync_command.encode('ascii'))
```

**Server Response**: 
1. Single byte: `0x00` (LT_OK) or `0x01` (LT_FAIL)
2. Status update message (if successful)

**Constraints**:
- Attempts must be between 1 and MACANDD_ROUNDS (typically 12)
- Invalid values return LT_FAIL

### 4. Status Update Messages

**Direction**: Server → Client  
**Format**: `"STATUS:"` + type + `":"` + current + `":"` + max  
**Encoding**: ASCII string, up to 64 bytes

#### Status Types

##### 4.1 Attempt Update
**Format**: `"STATUS:ATTEMPTS:current:max"`  
**Example**: `"STATUS:ATTEMPTS:2:5"`

**Meaning**: 
- Current failed attempts: 2
- Maximum allowed attempts: 5
- Remaining attempts: 3

##### 4.2 System Locked
**Format**: `"STATUS:LOCKED:current:max"`  
**Example**: `"STATUS:LOCKED:5:5"`

**Meaning**:
- All attempts exhausted
- System is locked
- No further PIN attempts accepted

##### 4.3 System Reset
**Format**: `"STATUS:RESET:0:max"`  
**Example**: `"STATUS:RESET:0:3"`

**Meaning**:
- Attempt counter reset to 0
- Maximum attempts set to 3
- System unlocked and ready

## Protocol Flow Examples

### Successful PIN Verification

```
Client → Server: "1234"
Server → Client: 0x00 (LT_OK)
# No status message needed, system resets internally
```

### Failed PIN Verification

```
Client → Server: "0000"
Server → Client: 0x01 (LT_FAIL)
Server → Client: "STATUS:ATTEMPTS:1:3"
```

### PIN Configuration

```
Client → Server: "SET_PIN:9876"
Server → Client: 0x00 (LT_OK)
# PIN is now set to 9876, attempts reset
```

### Attempts Configuration

```
Client → Server: "SET_ATTEMPTS:7"
Server → Client: 0x00 (LT_OK)
Server → Client: "STATUS:RESET:0:7"
```

### Lockout Scenario

```
# Attempt 1
Client → Server: "0000"
Server → Client: 0x01 (LT_FAIL)
Server → Client: "STATUS:ATTEMPTS:1:3"

# Attempt 2  
Client → Server: "1111"
Server → Client: 0x01 (LT_FAIL)
Server → Client: "STATUS:ATTEMPTS:2:3"

# Attempt 3 (final)
Client → Server: "2222"
Server → Client: 0x01 (LT_FAIL)
Server → Client: "STATUS:LOCKED:3:3"
```

## Error Handling

### Client-Side Errors

| Error | Cause | Resolution |
|-------|-------|------------|
| Connection refused | C server not running | Start C server first |
| Connection reset | Server crashed | Restart both client and server |
| Invalid response | Protocol mismatch | Check server version |

### Server-Side Errors

| Response | Meaning | Possible Causes |
|----------|---------|-----------------|
| LT_FAIL | Operation failed | Invalid PIN format, hardware error |
| No response | Communication error | Network issue, server crash |
| Partial message | Buffer error | Message too long, encoding issue |

## Implementation Notes

### C Server (lt_ex_macandd_pin_demo.c)

```c
// Main communication handler
int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
buffer[bytes_received] = '\0';  // Null-terminate

if (strncmp(buffer, "SET_PIN:", 8) == 0) {
    char* new_pin = buffer + 8;
    handle_set_pin_command(client_fd, new_pin, h, additional_data, additional_data_size);
} else if (strncmp(buffer, "SET_ATTEMPTS:", 13) == 0) {
    char* attempts_str = buffer + 13;
    handle_set_attempts_command(client_fd, attempts_str);
} else {
    handle_pin_verification(client_fd, buffer, h, additional_data, additional_data_size);
}
```

### Python Client (macnd_demo_new.py)

```python
def send_pin_to_c_program(self, pin: str) -> bool:
    """Send PIN to C emulator and get verification response."""
    self.client_socket.sendall(pin.encode('ascii'))
    response = self.client_socket.recv(1)
    is_success = response == b'\x00'
    
    if not is_success:
        self._receive_status_update()
    
    return is_success
```

## Security Considerations

### Protocol Security
- **Plaintext Communication**: Current implementation uses unencrypted TCP
- **Local Only**: Designed for localhost communication only
- **No Authentication**: No client authentication required

### Production Recommendations
1. **Encryption**: Implement TLS/SSL for network security
2. **Authentication**: Add client certificate verification  
3. **Rate Limiting**: Implement connection rate limiting
4. **Audit Logging**: Log all communication events
5. **Input Validation**: Enhanced input sanitization

## Testing

### Unit Tests

```python
def test_pin_verification():
    assert send_pin("1234") == True  # Correct PIN
    assert send_pin("0000") == False # Wrong PIN

def test_configuration():
    assert send_config("SET_PIN:5678") == True
    assert send_config("SET_ATTEMPTS:5") == True
    assert send_config("SET_ATTEMPTS:100") == False  # Invalid
```

### Integration Tests

```bash
# Test complete flow
./test_scripts/integration_test.sh
```

## Debugging

### Enable Debug Logging

**C Server**:
```c
#define DEBUG_COMMUNICATION 1
```

**Python Client**:
```python
logging.basicConfig(level=logging.DEBUG)
```

### Packet Capture

```bash
# Monitor TCP traffic
tcpdump -i lo -n port 12345
```

### Common Debug Commands

```bash
# Check connection status
netstat -an | grep 12345

# Monitor server logs
tail -f server.log

# Test connection manually
telnet 127.0.0.1 12345
```

---

**Note**: This API is designed for demonstration purposes. Production implementations should include additional security and error handling mechanisms. 