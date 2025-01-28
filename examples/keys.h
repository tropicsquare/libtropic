#include "libtropic.h"

// Default factory pairing keys
extern uint8_t pkey_index_0;
extern uint8_t sh0priv[];
extern uint8_t sh0pub[];
// Keys with acces to write attestation key in slot 1
extern uint8_t pkey_index_1;
extern uint8_t sh1priv[];
extern uint8_t sh1pub[];
// Keys with access only to read serial number
extern uint8_t pkey_index_2;
extern uint8_t sh2priv[];
extern uint8_t sh2pub[];
// Keys for application
extern uint8_t pkey_index_3;
extern uint8_t sh3priv[];
extern uint8_t sh3pub[];
