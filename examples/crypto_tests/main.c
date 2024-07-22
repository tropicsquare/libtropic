#include <stdio.h>
//#include "crypto_tests.h"

// Host cryptography
#include "ts_hkdf.h"
#include "ts_aesgcm.h"
#include "ts_sha256.h"
#include "ts_x25519.h"


int test_tropic_host_crypto(void);
int test_aesgcm();
int test_hkdf();
int test_sha256(void);
int test_X25519();

#define FROMHEX_MAXLEN 512

#define ASSERT(x)  if(x) { \
                        printf("OK"); \
                    } else { \
                        printf("ERROR"); \
                        return 1; \
                    }

int test_tropic_host_crypto(void) 
{
    printf("\n\tTest hash:            ");
    ASSERT(!test_sha256())

    printf("\n\tTest hkdf:            ");
    ASSERT(!test_hkdf())

    printf("\n\tTest AESGCM:          ");
    ASSERT(!test_aesgcm())

    printf("\n\tTest X25519:          ");
    ASSERT(!test_X25519())

    return 0;
}

const uint8_t *fromhex(const char *str) {
  static uint8_t buf[FROMHEX_MAXLEN];
  size_t len = strlen(str) / 2;
  if (len > FROMHEX_MAXLEN) len = FROMHEX_MAXLEN;
  for (size_t i = 0; i < len; i++) {
    uint8_t c = 0;
    if (str[i * 2] >= '0' && str[i * 2] <= '9') c += (str[i * 2] - '0') << 4;
    if ((str[i * 2] & ~0x20) >= 'A' && (str[i * 2] & ~0x20) <= 'F')
      c += (10 + (str[i * 2] & ~0x20) - 'A') << 4;
    if (str[i * 2 + 1] >= '0' && str[i * 2 + 1] <= '9')
      c += (str[i * 2 + 1] - '0');
    if ((str[i * 2 + 1] & ~0x20) >= 'A' && (str[i * 2 + 1] & ~0x20) <= 'F')
      c += (10 + (str[i * 2 + 1] & ~0x20) - 'A');
    buf[i] = c;
  }
  return buf;
}

struct {
      char *key;
      char *iv;
      char *aad;
      char *plaintext;
      char *ciphertext;
      char *tag;
    } vectors[] = {
        // Test case 1
        {
            "00000000000000000000000000000000",
            "000000000000000000000000",
            "",
            "",
            "",
            "58e2fccefa7e3061367f1d57a4e7455a",
        },
        // Test case 2
        {
            "00000000000000000000000000000000",
            "000000000000000000000000",
            "",
            "00000000000000000000000000000000",
            "0388dace60b6a392f328c2b971b2fe78",
            "ab6e47d42cec13bdf53a67b21257bddf",
        },
        // Test case 3
        {
            "feffe9928665731c6d6a8f9467308308",
            "cafebabefacedbaddecaf888",
            "",
            "d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c"
            "0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b391aafd255",
            "42831ec2217774244b7221b784d0d49ce3aa212f2c02a4e035c17e2329aca12e21d5"
            "14b25466931c7d8f6a5aac84aa051ba30b396a0aac973d58e091473f5985",
            "4d5c2af327cd64a62cf35abd2ba6fab4",
        },
        // Test case 4
        {
            "feffe9928665731c6d6a8f9467308308",
            "cafebabefacedbaddecaf888",
            "feedfacedeadbeeffeedfacedeadbeefabaddad2",
            "d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c"
            "0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b39",
            "42831ec2217774244b7221b784d0d49ce3aa212f2c02a4e035c17e2329aca12e21d5"
            "14b25466931c7d8f6a5aac84aa051ba30b396a0aac973d58e091",
            "5bc94fbc3221a5db94fae95ae7121a47",
        },
        // Test case 5
        {
            "feffe9928665731c6d6a8f9467308308",
            "cafebabefacedbad",
            "feedfacedeadbeeffeedfacedeadbeefabaddad2",
            "d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c"
            "0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b39",
            "61353b4c2806934a777ff51fa22a4755699b2a714fcdc6f83766e5f97b6c74237380"
            "6900e49f24b22b097544d4896b424989b5e1ebac0f07c23f4598",
            "3612d2e79e3b0785561be14aaca2fccb",
        },
        // Test case 6
        {
            "feffe9928665731c6d6a8f9467308308",
            "9313225df88406e555909c5aff5269aa6a7a9538534f7da1e4c303d2a318a728c3c0"
            "c95156809539fcf0e2429a6b525416aedbf5a0de6a57a637b39b",
            "feedfacedeadbeeffeedfacedeadbeefabaddad2",
            "d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c"
            "0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b39",
            "8ce24998625615b603a033aca13fb894be9112a5c3a211a8ba262a3cca7e2ca701e4"
            "a9a4fba43c90ccdcb281d48c7c6fd62875d2aca417034c34aee5",
            "619cc5aefffe0bfa462af43c1699d050",
        },
        // Test case 7
        {
            "000000000000000000000000000000000000000000000000",
            "000000000000000000000000",
            "",
            "",
            "",
            "cd33b28ac773f74ba00ed1f312572435",
        },
        // Test case 8
        {
            "000000000000000000000000000000000000000000000000",
            "000000000000000000000000",
            "",
            "00000000000000000000000000000000",
            "98e7247c07f0fe411c267e4384b0f600",
            "2ff58d80033927ab8ef4d4587514f0fb",
        },
        // Test case 9
        {
            "feffe9928665731c6d6a8f9467308308feffe9928665731c",
            "cafebabefacedbaddecaf888",
            "",
            "d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c"
            "0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b391aafd255",
            "3980ca0b3c00e841eb06fac4872a2757859e1ceaa6efd984628593b40ca1e19c7d77"
            "3d00c144c525ac619d18c84a3f4718e2448b2fe324d9ccda2710acade256",
            "9924a7c8587336bfb118024db8674a14",
        },
        // Test case 10
        {
            "feffe9928665731c6d6a8f9467308308feffe9928665731c",
            "cafebabefacedbaddecaf888",
            "feedfacedeadbeeffeedfacedeadbeefabaddad2",
            "d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c"
            "0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b39",
            "3980ca0b3c00e841eb06fac4872a2757859e1ceaa6efd984628593b40ca1e19c7d77"
            "3d00c144c525ac619d18c84a3f4718e2448b2fe324d9ccda2710",
            "2519498e80f1478f37ba55bd6d27618c",
        },
        // Test case 11
        {
            "feffe9928665731c6d6a8f9467308308feffe9928665731c",
            "cafebabefacedbad",
            "feedfacedeadbeeffeedfacedeadbeefabaddad2",
            "d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c"
            "0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b39",
            "0f10f599ae14a154ed24b36e25324db8c566632ef2bbb34f8347280fc4507057fddc"
            "29df9a471f75c66541d4d4dad1c9e93a19a58e8b473fa0f062f7",
            "65dcc57fcf623a24094fcca40d3533f8",
        },
        // Test case 12
        {
            "feffe9928665731c6d6a8f9467308308feffe9928665731c",
            "9313225df88406e555909c5aff5269aa6a7a9538534f7da1e4c303d2a318a728c3c0"
            "c95156809539fcf0e2429a6b525416aedbf5a0de6a57a637b39b",
            "feedfacedeadbeeffeedfacedeadbeefabaddad2",
            "d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c"
            "0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b39",
            "d27e88681ce3243c4830165a8fdcf9ff1de9a1d8e6b447ef6ef7b79828666e4581e7"
            "9012af34ddd9e2f037589b292db3e67c036745fa22e7e9b7373b",
            "dcf566ff291c25bbb8568fc3d376a6d9",
        },
        // Test case 13
        {
            "0000000000000000000000000000000000000000000000000000000000000000",
            "000000000000000000000000",
            "",
            "",
            "",
            "530f8afbc74536b9a963b4f1c4cb738b",
        },
        // Test case 14
        {
            "0000000000000000000000000000000000000000000000000000000000000000",
            "000000000000000000000000",
            "",
            "00000000000000000000000000000000",
            "cea7403d4d606b6e074ec5d3baf39d18",
            "d0d1c8a799996bf0265b98b5d48ab919",
        },
        // Test case 15
        {
            "feffe9928665731c6d6a8f9467308308feffe9928665731c6d6a8f9467308308",
            "cafebabefacedbaddecaf888",
            "",
            "d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c"
            "0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b391aafd255",
            "522dc1f099567d07f47f37a32a84427d643a8cdcbfe5c0c97598a2bd2555d1aa8cb0"
            "8e48590dbb3da7b08b1056828838c5f61e6393ba7a0abcc9f662898015ad",
            "b094dac5d93471bdec1a502270e3cc6c",
        },
        // Test case 16
        {
            "feffe9928665731c6d6a8f9467308308feffe9928665731c6d6a8f9467308308",
            "cafebabefacedbaddecaf888",
            "feedfacedeadbeeffeedfacedeadbeefabaddad2",
            "d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c"
            "0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b39",
            "522dc1f099567d07f47f37a32a84427d643a8cdcbfe5c0c97598a2bd2555d1aa8cb0"
            "8e48590dbb3da7b08b1056828838c5f61e6393ba7a0abcc9f662",
            "76fc6ece0f4e1768cddf8853bb2d551b",
        },
        // Test case 17
        {
            "feffe9928665731c6d6a8f9467308308feffe9928665731c6d6a8f9467308308",
            "cafebabefacedbad",
            "feedfacedeadbeeffeedfacedeadbeefabaddad2",
            "d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c"
            "0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b39",
            "c3762df1ca787d32ae47c13bf19844cbaf1ae14d0b976afac52ff7d79bba9de0feb5"
            "82d33934a4f0954cc2363bc73f7862ac430e64abe499f47c9b1f",
            "3a337dbf46a792c45e454913fe2ea8f2",
        },
        // Test case 18
        {
            "feffe9928665731c6d6a8f9467308308feffe9928665731c6d6a8f9467308308",
            "9313225df88406e555909c5aff5269aa6a7a9538534f7da1e4c303d2a318a728c3c0"
            "c95156809539fcf0e2429a6b525416aedbf5a0de6a57a637b39b",
            "feedfacedeadbeeffeedfacedeadbeefabaddad2",
            "d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a721c3c"
            "0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b39",
            "5a8def2f0c9e53f1f75d7853659e2a20eeb2b22aafde6419a058ab4f6f746bf40fc0"
            "c3b780f244452da3ebf1c5d82cdea2418997200ef82e44ae7e3f",
            "a44a8266ee1c8eb0c8b5d4cf5ae9f19a",
        },
    };

int test_aesgcm()
{
    uint8_t iv[60] = {0};
    uint8_t aad[20] = {0};
    uint8_t msg[64] = {0};
    uint8_t tag[16] = {0};

    for (uint32_t i=0; i < (sizeof(vectors)/sizeof(vectors[0])); i++) {
    
        size_t iv_len = strlen(vectors[i].iv) / 2;
        memcpy(iv, fromhex(vectors[i].iv), iv_len);
        size_t aad_len = strlen(vectors[i].aad) / 2;
        memcpy(aad, fromhex(vectors[i].aad), aad_len);
        size_t msg_len = strlen(vectors[i].plaintext) / 2;
        memcpy(msg, fromhex(vectors[i].plaintext), msg_len);
        size_t tag_len = strlen(vectors[i].tag) / 2;
    
        // Set key.
        ts_aes_gcm_ctx_t ctx_enc = {0};
        int ret = ts_aesgcm_init_and_key(
                    &ctx_enc,                   /* the mode context             */
                    fromhex(vectors[i].key),    /* the key value                */
                    strlen(vectors[i].key) / 2  /* and its length in bytes      */
                    );

        ret = ts_aesgcm_encrypt( 
                    &ctx_enc,                   /* the mode context             */
                    iv,                         /* the initialisation vector    */
                    iv_len,                     /* and its length in bytes      */
                    aad,                        /* the header buffer            */
                    aad_len,                    /* and its length in bytes      */
                    msg,                        /* the message buffer           */
                    msg_len,                    /* and its length in bytes      */
                    tag,                        /* the buffer for the tag       */
                    tag_len                     /* and its length in bytes      */
                    );

        //memset(&ctx, 0, sizeof(ts_aes_gcm_ctx_t));
        if(RETURN_GOOD != ret) {
                return 1;
        }
        //TEST_ASSERT_EQUAL(RETURN_GOOD, ret);
        if ((msg_len > 0))
        {
            if(memcmp(msg, fromhex(vectors[i].ciphertext), msg_len) == 1) {
                return 1;
            }
            //TEST_ASSERT_EQUAL_HEX8_ARRAY(msg, fromhex(vectors[i].ciphertext), msg_len);
        }
        if ((tag_len > 0))
        {
            if(memcmp(tag, fromhex(vectors[i].tag), tag_len) == 1) {
                return 1;
            }
            //TEST_ASSERT_EQUAL_HEX8_ARRAY(tag, fromhex(vectors[i].tag), tag_len);
        }
        
        ts_aes_gcm_ctx_t ctx_dec = {0};

        ret = ts_aesgcm_init_and_key(&ctx_dec, 
                    fromhex(vectors[i].key),    /* the key value                */
                    strlen(vectors[i].key) / 2  /* and its length in bytes      */
                    );

        ret = ts_aesgcm_decrypt( &ctx_dec,      /* the mode context             */   
                    iv,                         /* the initialisation vector    */
                    iv_len,                     /* and its length in bytes      */
                    aad,                        /* the header buffer            */
                    aad_len,                    /* and its length in bytes      */
                    msg,                        /* the message buffer           */
                    msg_len,                    /* and its length in bytes      */
                    tag,                        /* the buffer for the tag       */
                    tag_len);                   /* and its length in bytes      */


        if(msg_len > 0) {
            if(memcmp(fromhex(vectors[i].plaintext), msg, msg_len) == 1) {
                return 1;
            }
            //TEST_ASSERT_EQUAL_HEX8_ARRAY(fromhex(vectors[i].plaintext), msg, msg_len);
        }
    }
    return 0;
}

int test_hkdf() {

    uint8_t output_1[33] = {0};
    uint8_t output_2[32] = {0};
    // round 1, output_2 is not used
    uint8_t protocol_name[32] = {'N','o','i','s','e','_','K','K','1','_','2','5','5','1','9','_','A','E','S','G','C','M','_','S','H','A','2','5','6',0x00,0x00,0x00};
    uint8_t s_s_1[32] = {0x3d,0x9c,0xf1,0x6d,0x28,0x96,0x11,0x5e,0x4e,0x6a,0x9f,0xce,0x07,0x10,0x39,0xd9,0xa2,0x06,0x80,0xd3,0x24,0xa6,0xd1,0x6d,0xcb,0x83,0x3c,0xd6,0xd5,0x85,0xdf,0x3b};
    ts_hkdf(protocol_name, 32, s_s_1, 32, 1, output_1, output_2);
    uint8_t ck1[32] = {0x8e,0x5d,0xae,0xbc,0xbc,0xe7,0xa3,0x0d,0xd6,0x6e,0xb7,0x23,0x01,0x53,0x50,0xd9,0x05,0x7f,0x53,0xaf,0x0f,0x81,0xce,0x0d,0xae,0x51,0xda,0xf8,0x70,0x4e,0xca,0x1b};
    
    if(memcmp(ck1, output_1, 32) == 1) {
        return 1;
    }
    //TEST_ASSERT_EQUAL_HEX8_ARRAY(ck1, output_1, 32);
    // round 2, output_2 is not used
    uint8_t s_s_2[32] = {0xdf,0xf8,0x1a,0xa2,0x04,0x78,0x5e,0x0b,0x53,0x28,0xfe,0xed,0x8d,0xec,0x02,0x0d,0xbb,0xcb,0xda,0xb3,0x07,0x67,0xdd,0x3b,0xb1,0xa6,0xc5,0x6c,0x93,0xcb,0x3a,0x13};
    ts_hkdf(ck1, 32, s_s_2, 32, 1, output_1, output_2);
    uint8_t ck2[32] = {0x2f,0x5b,0xc4,0xcb,0xae,0xe1,0x00,0x4d,0x57,0x58,0x53,0x16,0xbc,0xa4,0x06,0x6e,0xac,0x66,0xa8,0xc0,0xe6,0x68,0x0a,0x46,0x97,0x2c,0xe3,0x91,0xa4,0x34,0x48,0x63};
    if(memcmp(ck2, output_1, 32) == 1) {
        return 1;
    }
    //TEST_ASSERT_EQUAL_HEX8_ARRAY(ck2, output_1, 32);
    // round 3, output_2 is also used
    uint8_t s_s_3[32] = {0xb9,0x1c,0x94,0x87,0x9c,0x54,0x09,0x09,0x6d,0x79,0x2d,0x2b,0x98,0xba,0xb0,0x11,0xbf,0x8e,0x49,0xbf,0x8f,0x8d,0x26,0x0b,0x90,0x85,0x1c,0x7e,0x75,0xa9,0xfb,0x6a};
    ts_hkdf(ck2, 32, s_s_3, 32, 2, output_1, output_2);
    uint8_t ck3[32] = {0xa3,0xb3,0x38,0x0a,0x6f,0xf2,0xbb,0x64,0x03,0xc3,0x4f,0xec,0xa4,0x4b,0xf5,0xde,0xb2,0xed,0xc9,0x2f,0xfb,0x4c,0x1b,0xa6,0xcb,0x20,0xdc,0x97,0xb6,0xe2,0xc8,0x34};
    uint8_t kauth[32] = {0xa1,0x97,0x09,0x67,0x11,0xf5,0x8e,0x5d,0x10,0x4c,0xbf,0xfb,0xa7,0x16,0xa0,0x57,0x38,0x28,0x09,0x65,0x6f,0xa5,0xd7,0xfd,0x56,0x93,0x08,0x41,0xf8,0xef,0xf6,0x29};
    if(memcmp(ck3, output_1, 32) == 1) {
        return 1;
    }
    if(memcmp(kauth, output_2, 32) == 1) {
        return 1;
    }
    
    //TEST_ASSERT_EQUAL_HEX8_ARRAY(ck3, output_1, 32);
    //TEST_ASSERT_EQUAL_HEX8_ARRAY(kauth, output_2, 32);
    // round 4, output_2 is also used
    ts_hkdf(ck3, 32, (uint8_t *)"", 0, 2, output_1, output_2);
    uint8_t kcmd[32] = {0x10,0xf3,0x29,0xf6,0x85,0xc3,0x44,0x25,0xa3,0x79,0x0e,0xdb,0xcf,0x42,0xa5,0x88,0xe8,0x87,0x64,0x07,0x73,0x87,0xe3,0x0e,0x57,0x58,0xbd,0xb1,0x9d,0x68,0x45,0xcb};
    uint8_t kres[32] = {0x32,0xb3,0x1d,0x65,0xad,0xf4,0xab,0x0e,0x2c,0xba,0xd8,0x0d,0xe9,0x1f,0xa4,0x66,0xf5,0x76,0x26,0x71,0x81,0x5a,0x75,0x04,0xdf,0x1c,0x32,0xaf,0xfe,0xba,0xaf,0x15};
    //TEST_ASSERT_EQUAL_HEX8_ARRAY(kcmd, output_1, 32);
    //TEST_ASSERT_EQUAL_HEX8_ARRAY(kres, output_2, 32);

    if(memcmp(kcmd, output_1, 32) == 1) {
        return 1;
    }
    if(memcmp(kres, output_2, 32) == 1) {
        return 1;
    }

    return 0;
}

int test_sha256(void) {

    uint8_t protocol_name[32] = {'N','o','i','s','e','_','K','K','1','_','2','5','5','1','9','_','A','E','S','G','C','M','_','S','H','A','2','5','6'};
    uint8_t hash[SHA256_DIGEST_LENGTH] = {0};
    ts_sha256_ctx_t ctx = {0};
    ts_sha256_init(&ctx);
    ts_sha256_start(&ctx);
    ts_sha256_update(&ctx, protocol_name, 29);
    ts_sha256_finish(&ctx, hash);
    uint8_t expected[] = {0x92,0x5b,0x44,0xac,0xed,0xc4,0x53,0x3b,0x48,0x42,0xa0,0x66,0x0d,0x81,0x5e,0x16,0x84,0x0a,0xee,0x64,0x63,0x28,0x0f,0x07,0xa8,0x28,0xe1,0x5d,0x07,0x0c,0x73,0x78};

    if(memcmp(expected, hash, 32)) {
        return 1;
    }
    return 0;
}

// vectors taken from python model
int test_X25519()
{
    uint8_t priv[32] = {0x38,0xc9,0xd9,0xb1,0x79,0x11,0xde,0x26,0xed,0x81,0x2f,0x5c,0xc1,0x9c,0x00,0x29,0xe8,0xd0,0x16,0xbc,0xbc,0x60,0x78,0xbc,0x9d,0xb2,0xaf,0x33,0xf1,0x76,0x1e,0x4a};
    uint8_t pub[32]  = {0x31,0x1b,0x62,0x48,0xaf,0x8d,0xab,0xec,0x5c,0xc8,0x1e,0xac,0x5b,0xf2,0x29,0x92,0x5f,0x6d,0x21,0x8a,0x12,0xe0,0x54,0x7f,0xb1,0x85,0x6e,0x01,0x5c,0xc7,0x6f,0x5d};
    uint8_t out[32] = {0};
    ts_X25519(priv, pub, out);
    uint8_t expected[32]  = {0xa9,0x3d,0xbd,0xb2,0x3e,0x5c,0x99,0xda,0x74,0x3e,0x20,0x3b,0xd3,0x91,0xaf,0x79,0xf2,0xb8,0x3f,0xb8,0xd0,0xfd,0x6e,0xc8,0x13,0x37,0x1c,0x71,0xf0,0x8f,0x2d,0x4d};
    if(memcmp(expected, out, 32) == 1) {
        return 1;
    }
    //TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, out, 32);

    uint8_t priv2[32] = {0x38,0xc9,0xd9,0xb1,0x79,0x11,0xde,0x26,0xed,0x81,0x2f,0x5c,0xc1,0x9c,0x00,0x29,0xe8,0xd0,0x16,0xbc,0xbc,0x60,0x78,0xbc,0x9d,0xb2,0xaf,0x33,0xf1,0x76,0x1e,0x4a};
    uint8_t pub2[32]  = {0x31,0x1b,0x62,0x48,0xaf,0x8d,0xab,0xec,0x5c,0xc8,0x1e,0xac,0x5b,0xf2,0x29,0x92,0x5f,0x6d,0x21,0x8a,0x12,0xe0,0x54,0x7f,0xb1,0x85,0x6e,0x01,0x5c,0xc7,0x6f,0x5d};
    uint8_t out2[32] = {0};
    ts_X25519(priv2, pub2, out2);
    uint8_t expected2[32]  = {0xa9,0x3d,0xbd,0xb2,0x3e,0x5c,0x99,0xda,0x74,0x3e,0x20,0x3b,0xd3,0x91,0xaf,0x79,0xf2,0xb8,0x3f,0xb8,0xd0,0xfd,0x6e,0xc8,0x13,0x37,0x1c,0x71,0xf0,0x8f,0x2d,0x4d};
    if(memcmp(expected2, out2, 32) == 1) {
        return 1;
    }
    return 0;
    //TEST_ASSERT_EQUAL_HEX8_ARRAY(expected2, out2, 32);
}


//--------------------------------------------------------------------------------------------------//
int main(void) {
    printf("\nRunning tests for implemented crypto functions: \n");
    if (test_tropic_host_crypto() == 1) {
        printf("\n\n!!! CRYPTO ERROR !!!\n");
        return 1;
    }
    printf("\n");
}
