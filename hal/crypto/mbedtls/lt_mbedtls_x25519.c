/**
 * @file lt_mbedtls_x25519.c
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <stdint.h>

#include "mbedtls/ecdh.h"
#include "mbedtls/ecp.h"
#include "libtropic_common.h"
#include "lt_x25519.h"

lt_ret_t lt_X25519(const uint8_t *privkey, const uint8_t *pubkey, uint8_t *secret)
{
    int ret;
    mbedtls_ecp_group grp;
    mbedtls_ecp_point Q;
    mbedtls_mpi d, z;

    // Initialize structures
    mbedtls_ecp_group_init(&grp);
    mbedtls_ecp_point_init(&Q);
    mbedtls_mpi_init(&d);
    mbedtls_mpi_init(&z);

    // Load Curve25519 parameters
    ret = mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_CURVE25519);
    if (ret != 0) {
        goto cleanup;
    }

    // Import private key (32 bytes)
    ret = mbedtls_mpi_read_binary(&d, privkey, 32);
    if (ret != 0) {
        goto cleanup;
    }

    // Import public key (32 bytes) as X coordinate (Curve25519 uses x-only coordinates)
    ret = mbedtls_mpi_read_binary(&Q.X, pubkey, 32);
    if (ret != 0) {
        goto cleanup;
    }
    
    // For Curve25519, we only use the X coordinate
    ret = mbedtls_mpi_lset(&Q.Z, 1);
    if (ret != 0) {
        goto cleanup;
    }

    // Perform scalar multiplication: z = d * Q
    ret = mbedtls_ecdh_compute_shared(&grp, &z, &Q, &d, NULL, NULL);
    if (ret != 0) {
        goto cleanup;
    }

    // Export the shared secret (X coordinate of the result)
    ret = mbedtls_mpi_write_binary(&z, secret, 32);

cleanup:
    mbedtls_ecp_group_free(&grp);
    mbedtls_ecp_point_free(&Q);
    mbedtls_mpi_free(&d);
    mbedtls_mpi_free(&z);

    if (ret != 0) {
        return LT_CRYPTO_ERR;
    }
    return LT_OK;
}

lt_ret_t lt_X25519_scalarmult(const uint8_t *sk, uint8_t *pk)
{
    int ret;
    mbedtls_ecp_group grp;
    mbedtls_ecp_point R;
    mbedtls_mpi d;

    // Initialize structures
    mbedtls_ecp_group_init(&grp);
    mbedtls_ecp_point_init(&R);
    mbedtls_mpi_init(&d);

    // Load Curve25519 parameters
    ret = mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_CURVE25519);
    if (ret != 0) {
        goto cleanup;
    }

    // Import private key (32 bytes)
    ret = mbedtls_mpi_read_binary(&d, sk, 32);
    if (ret != 0) {
        goto cleanup;
    }

    // Perform scalar multiplication with base point: R = d * G
    ret = mbedtls_ecp_mul(&grp, &R, &d, &grp.G, NULL, NULL);
    if (ret != 0) {
        goto cleanup;
    }

    // Export the public key (X coordinate)
    ret = mbedtls_mpi_write_binary(&R.X, pk, 32);

cleanup:
    mbedtls_ecp_group_free(&grp);
    mbedtls_ecp_point_free(&R);
    mbedtls_mpi_free(&d);

    if (ret != 0) {
        return LT_CRYPTO_ERR;
    }
    return LT_OK;
}
