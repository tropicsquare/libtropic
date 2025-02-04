#include "libtropic.h"

// Default factory pairing keys
int8_t pkey_index_0 =  PAIRING_KEY_SLOT_INDEX_0;
uint8_t sh0priv[] = {0xd0,0x99,0x92,0xb1,0xf1,0x7a,0xbc,0x4d,0xb9,0x37,0x17,0x68,0xa2,0x7d,0xa0,0x5b,0x18,0xfa,0xb8,0x56,0x13,0xa7,0x84,0x2c,0xa6,0x4c,0x79,0x10,0xf2,0x2e,0x71,0x6b};
uint8_t sh0pub[]  = {0xe7,0xf7,0x35,0xba,0x19,0xa3,0x3f,0xd6,0x73,0x23,0xab,0x37,0x26,0x2d,0xe5,0x36,0x08,0xca,0x57,0x85,0x76,0x53,0x43,0x52,0xe1,0x8f,0x64,0xe6,0x13,0xd3,0x8d,0x54};
// Keys with acces to write attestation key in slot 1
uint8_t pkey_index_1 =  PAIRING_KEY_SLOT_INDEX_1;
uint8_t sh1priv[] = {0x58,0xc4,0x81,0x88,0xf8,0xb1,0xcb,0xd4,0x19,0x00,0x2e,0x9c,0x8d,0xf8,0xce,0xea,0xf3,0xa9,0x11,0xde,0xb6,0x6b,0xc8,0x87,0xae,0xe7,0x88,0x10,0xfb,0x48,0xb6,0x74};
uint8_t sh1pub[]  = {0xe1,0xdc,0xf9,0xc3,0x46,0xbc,0xf2,0xe7,0x8b,0xa8,0xf0,0x27,0xd8,0x0a,0x8a,0x33,0xcc,0xf3,0xe9,0xdf,0x6b,0xdf,0x65,0xa2,0xc1,0xae,0xc4,0xd9,0x21,0xe1,0x8d,0x51};
// Keys with access only to read serial number
uint8_t pkey_index_2 =  PAIRING_KEY_SLOT_INDEX_2;
uint8_t sh2priv[] = {0x00,0x40,0x5e,0x19,0x46,0x75,0xab,0xe1,0x5f,0x0b,0x57,0xf2,0x5b,0x12,0x86,0x62,0xab,0xb0,0xe9,0xc6,0xa7,0xc3,0xca,0xdf,0x1c,0xb1,0xd2,0xb7,0xf8,0xcf,0x35,0x47};
uint8_t sh2pub[]  = {0x66,0xb9,0x92,0x5a,0x85,0x66,0xe8,0x09,0x5c,0x56,0x80,0xfb,0x22,0xd4,0xb8,0x4b,0xf8,0xe3,0x12,0xb2,0x7c,0x4b,0xac,0xce,0x26,0x3c,0x78,0x39,0x6d,0x4c,0x16,0x6c};
// Keys for application
uint8_t pkey_index_3 =  PAIRING_KEY_SLOT_INDEX_3;
uint8_t sh3priv[] = {0xb0,0x90,0x9f,0xe1,0xf3,0x1f,0xa1,0x21,0x75,0xef,0x45,0xb1,0x42,0xde,0x0e,0xdd,0xa1,0xf4,0x51,0x01,0x40,0xc2,0xe5,0x2c,0xf4,0x68,0xac,0x96,0xa1,0x0e,0xcb,0x46};
uint8_t sh3pub[]  = {0x22,0x57,0xa8,0x2f,0x85,0x8f,0x13,0x32,0xfa,0x0f,0xf6,0x0c,0x76,0x29,0x42,0x70,0xa9,0x58,0x9d,0xfd,0x47,0xa5,0x23,0x78,0x18,0x4d,0x2d,0x38,0xf0,0xa7,0xc4,0x01};
