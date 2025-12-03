/**
 * @file lt_trezor_crypto_common.c
 * @copyright Copyright (c) 2020-2025 Tropic Square s.r.o.
 *
 * @license For the license see LICENSE.md file in the root directory of this source tree.
 */

#include "aes/aesgcm.h"
#include "libtropic_trezor_crypto.h"
#include "lt_aesgcm.h"
#include "lt_crypto_common.h"

lt_ret_t lt_crypto_ctx_init(void *ctx)
{
    LT_UNUSED(ctx);
    return LT_OK;
}

lt_ret_t lt_crypto_ctx_deinit(void *ctx)
{
    lt_ret_t ret1 = lt_aesgcm_encrypt_deinit(ctx);
    lt_ret_t ret2 = lt_aesgcm_decrypt_deinit(ctx);

    if (ret1 != LT_OK) {
        return ret1;
    }
    if (ret2 != LT_OK) {
        return ret2;
    }

    return LT_OK;
}