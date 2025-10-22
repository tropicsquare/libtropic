/**
 * @file lt_mbedtls_ecdsa.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdint.h>
#include <string.h>

#include "mbedtls/ecdsa.h"
#include "mbedtls/ecp.h"
#include "mbedtls/sha256.h"
#include "libtropic_common.h"
#include "lt_ecdsa.h"

lt_ret_t lt_ecdsa_sign_verify(const uint8_t *msg, const uint32_t msg_len, const uint8_t *pubkey, const uint8_t *rs)
{
    int ret;
    mbedtls_ecp_group grp;
    mbedtls_ecp_point Q;
    mbedtls_mpi r, s;
    uint8_t hash[32];

    // Initialize structures
    mbedtls_ecp_group_init(&grp);
    mbedtls_ecp_point_init(&Q);
    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);

    // Load NIST P-256 curve parameters
    ret = mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1);
    if (ret != 0) {
        goto cleanup;
    }

    // Import public key (64 bytes: 32 bytes X, 32 bytes Y)
    ret = mbedtls_mpi_read_binary(&Q.X, pubkey, 32);
    if (ret != 0) {
        goto cleanup;
    }
    
    ret = mbedtls_mpi_read_binary(&Q.Y, pubkey + 32, 32);
    if (ret != 0) {
        goto cleanup;
    }
    
    ret = mbedtls_mpi_lset(&Q.Z, 1);
    if (ret != 0) {
        goto cleanup;
    }

    // Import signature components (r and s, each 32 bytes)
    ret = mbedtls_mpi_read_binary(&r, rs, 32);
    if (ret != 0) {
        goto cleanup;
    }
    
    ret = mbedtls_mpi_read_binary(&s, rs + 32, 32);
    if (ret != 0) {
        goto cleanup;
    }

    // Compute SHA-256 hash of the message
    ret = mbedtls_sha256(msg, msg_len, hash, 0);
    if (ret != 0) {
        goto cleanup;
    }

    // Verify the ECDSA signature
    ret = mbedtls_ecdsa_verify(&grp, hash, 32, &Q, &r, &s);

cleanup:
    mbedtls_ecp_group_free(&grp);
    mbedtls_ecp_point_free(&Q);
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);

    if (ret != 0) {
        return LT_CRYPTO_ERR;
    }
    return LT_OK;
}
