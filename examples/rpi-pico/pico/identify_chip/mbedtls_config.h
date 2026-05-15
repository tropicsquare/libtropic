#ifndef LIBTROPIC_PICO_MBEDTLS_CONFIG_H
#define LIBTROPIC_PICO_MBEDTLS_CONFIG_H

/* Workaround: some Mbed TLS sources use INT_MAX without including limits.h. */
#include <limits.h>

/* Start from upstream defaults to keep PSA Crypto enabled. */
#include "mbedtls/mbedtls_config.h"

/* RP2040 bare-metal build: avoid Unix/Windows-only platform backends. */
#ifdef MBEDTLS_HAVE_TIME
#undef MBEDTLS_HAVE_TIME
#endif

#ifdef MBEDTLS_HAVE_TIME_DATE
#undef MBEDTLS_HAVE_TIME_DATE
#endif

#ifdef MBEDTLS_TIMING_C
#undef MBEDTLS_TIMING_C
#endif

#ifdef MBEDTLS_FS_IO
#undef MBEDTLS_FS_IO
#endif

#ifndef MBEDTLS_NO_PLATFORM_ENTROPY
#define MBEDTLS_NO_PLATFORM_ENTROPY
#endif

#ifndef MBEDTLS_ENTROPY_HARDWARE_ALT
#define MBEDTLS_ENTROPY_HARDWARE_ALT
#endif

#ifdef MBEDTLS_NET_C
#undef MBEDTLS_NET_C
#endif

#ifdef MBEDTLS_PSA_CRYPTO_STORAGE_C
#undef MBEDTLS_PSA_CRYPTO_STORAGE_C
#endif

#ifdef MBEDTLS_PSA_ITS_FILE_C
#undef MBEDTLS_PSA_ITS_FILE_C
#endif

#endif /* LIBTROPIC_PICO_MBEDTLS_CONFIG_H */
