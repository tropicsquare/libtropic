/**
 * @file asn1_der.c
 * @brief ASN1 DER parser
 * @note Implements subset of ASN1 DER parsing needed to extract STPub from
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include <lt_asn1_der.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Uncomment to enable parser logging
// #define ASNDER_LOG_EN

/**
 * @brief ASN1 DER parsing context
 *
 * @details Holds state of the parser and parsing results
 */
struct parse_ctx_t {
    uint8_t *head;                      /** Next byte to be parse */
    uint16_t len;                       /** Length of the byte stream */
    uint16_t past;                      /** Index of last processed byte */
    uint32_t obj_id;                    /** Target OBJECT_IDENTIFIER (3-byte) to be searched */
    uint8_t *sbuf;                      /** Buffer (and its length) where to copy data after
                                            OBJECT_IDENTIFIER match */
    int sbuf_len;                       /** Length of Buffer pointed to by sbuf. */
    enum asn1der_crop_kind_t crop_kind; /** How to treat objects larger than provided buffer */
    bool sample_next;                   /** Internal context ->
                                            Next ASN1 object is the one to be sampled */
    bool found;                         /** Searched OBJECT_IDENTIFIER was found */
    bool cropped;                       /** Searched object was cropped */
};

#ifdef ASNDER_LOG_EN
#define PARSE_ERR(ctx, msg, ...)                                   \
    do {                                                           \
        LT_LOG_ERROR("ASN1 DER Parsing error:");                   \
        LT_LOG_ERROR("    Byte position:    %d", ctx->past);       \
        LT_LOG_ERROR("    Byte value:       0x%x", *(ctx->head));  \
        LT_LOG_ERROR("    Error:            " msg, ##__VA_ARGS__); \
    } while (0);

#else
#define PARSE_ERR(ctx, msg, ...)
#endif  // ASNDER_LOG_EN

/**
 * @brief Process bytes from byte stream being parsed
 *
 * @param ctx       Parsing context
 * @param buf       Result buffer
 * @param n         Number of bytes to copy
 * @param copy      If true, copy the consumed bytes to "buf"
 *
 * @returns LT_OK if sucessfully, error code otherwise
 */
static lt_ret_t consume_bytes(struct parse_ctx_t *ctx, uint8_t *buf, uint16_t n, bool copy)
{
    if (ctx->past + n > ctx->len) {
        PARSE_ERR(ctx, "Incomplete byte stream. Past: %d, n: %d, len: %d", ctx->past, n, ctx->len);
        return LT_CERT_STORE_INVALID;
    }

    if (copy) {
        if (n == 1) {
            *buf = *(ctx->head);
        }
        else {
            memcpy(buf, ctx->head, n);
        }
    }

    ctx->head += n;
    ctx->past += n;

    return LT_OK;
}

#define GET_BYTES(ctx, buf, n)                          \
    do {                                                \
        lt_ret_t rv = consume_bytes(ctx, buf, n, true); \
        if (rv != LT_OK) return rv;                     \
    } while (0);

#define GET_NEXT_BYTE(ctx, buf) GET_BYTES(ctx, buf, 1)

#define DROP_BYTES(ctx, n)                                \
    do {                                                  \
        lt_ret_t rv = consume_bytes(ctx, NULL, n, false); \
        if (rv != LT_OK) return rv;                       \
    } while (0);

/**
 * @brief Parse ASN1 DER length encoding
 *
 * @param ctx       Parsing context
 * @param len       Pointer to integer where length is returned
 *
 * @returns LT_OK if sucessfully, error code otherwise
 */
static lt_ret_t parse_length(struct parse_ctx_t *ctx, uint16_t *len)
{
    uint8_t b;
    GET_NEXT_BYTE(ctx, &b);

    if (b < 127) {
        *len = (uint16_t)b;
    }
    else {
        uint8_t n_bytes = b ^ 0x80;

        if (n_bytes > 2) {
            PARSE_ERR(ctx, "Unsupported length: More than 2 bytes");
            return LT_CERT_UNSUPPORTED;
        }

        *len = 0;
        for (int i = 0; i < n_bytes; i++) {
            (*len) <<= 8;
            GET_NEXT_BYTE(ctx, &b);
            *len |= b;
        }
    }

    return LT_OK;
}

/**
 * @brief Parse ASN1 object.
 *
 * @details Recursively parses the ASN1/DER stream and searches for Object with
 *          ctx->obj_id 3 byte identifier. If found, samples the follow-up item
 *          in the sequence that shall be of primitive type into ctx.
 *
 * @param   ctx     Parsing context
 * @returns LT_OK if succesfull, false otherwise
 */
static lt_ret_t parse_object(struct parse_ctx_t *ctx)
{
    uint8_t b = 0;
    uint16_t len = 0;
    lt_ret_t rv = LT_OK;

    GET_NEXT_BYTE(ctx, &b);

    rv = parse_length(ctx, &len);
    if (rv != LT_OK) return rv;

    uint16_t start = ctx->past;

#ifdef ASNDER_LOG_EN
    LT_LOG("parse_object:");
    LT_LOG("    Start: %d", start);
    LT_LOG("    Object type: 0x%x", b);
    LT_LOG("    Object len: %d", len);
#endif

    switch (b) {
        case ASN1DER_SEQUENCE:
            while (ctx->past < start + len - 1) {
                lt_ret_t rv = parse_object(ctx);
                if (rv != LT_OK) return rv;
            }

            if (start + len != ctx->past) {
                PARSE_ERR(ctx,
                          "Invalid Sequence length. "
                          "Sequence start: %d. "
                          "Expected length: %d. "
                          "Expected end: %d. "
                          "Real end: %d",
                          start, len, start + len - 1, ctx->past);
                return LT_CERT_STORE_INVALID;
            }
            break;

        case ASN1DER_OBJECT_IDENTIFIER: {
            uint8_t buf[len];
            GET_BYTES(ctx, buf, len);

            // TODO: Can it happend len is smaller than 3 ?
            uint32_t obj_id = (((uint32_t)buf[0]) << 16) | (((uint32_t)buf[1]) << 8) | (((uint32_t)buf[2]));

            if (ctx->obj_id == obj_id) {
#ifdef ASNDER_LOG_EN
                LT_LOG("Found searched object: 0x%x. Next object will be sampled!", ctx->obj_id);
#endif
                ctx->sample_next = true;
            }
            break;
        }

        /* These object types can be searched for */
        case ASN1DER_BOOLEAN:
        case ASN1DER_INTEGER:
        case ASN1DER_STRING_BIT:
        case ASN1DER_STRING_OCTET:
        case ASN1DER_STRING_NULL:
        case ASN1DER_STRING_UTF8:
        case ASN1DER_STRING_PRINTABLE:
        case ASN1DER_UTC_TIME: {
            if (ctx->sample_next & !ctx->found) {
#ifdef ASNDER_LOG_EN
                LT_LOG("Sampling this object!", ctx->obj_id);
#endif
                uint8_t sample_len = len;

                if (ctx->sbuf_len < sample_len) {
                    uint16_t n_crop_bytes = sample_len - ctx->sbuf_len;
                    bool crop_prefix = ctx->crop_kind == ASN1DER_CROP_PREFIX;
                    ctx->cropped = true;

#ifdef ASNDER_LOG_EN
                    LT_LOG(
                        "Sample buffer (%d) is smaller than size of the object to be sampled (%d). "
                        "Cropping %d bytes from %s of the searched object",
                        ctx->sbuf_len, sample_len, n_crop_bytes, crop_prefix ? "prefix" : "suffix");
#endif

                    if (crop_prefix) {
                        DROP_BYTES(ctx, n_crop_bytes)
                    }

                    sample_len = ctx->sbuf_len;
                    GET_BYTES(ctx, ctx->sbuf, sample_len);

                    if (!crop_prefix) {
                        DROP_BYTES(ctx, n_crop_bytes)
                    }
                }
                else {
                    GET_BYTES(ctx, ctx->sbuf, sample_len);
                }

                ctx->sample_next = false;
                ctx->found = true;
            }
            else {
                DROP_BYTES(ctx, len);
            }
            break;
        }

        default:
            DROP_BYTES(ctx, len);
    }

    return rv;
}

/*******************************************************************************
 * Public API
 *******************************************************************************/

lt_ret_t asn1der_find_object(const uint8_t *stream, uint16_t len, int32_t obj_id, uint8_t *buf, int buf_len,
                             enum asn1der_crop_kind_t crop_kind)
{
    struct parse_ctx_t ctx = {.head = (uint8_t *)stream,
                              .len = len,
                              .past = 0,
                              .obj_id = obj_id,
                              .sbuf = buf,
                              .sbuf_len = buf_len,
                              .crop_kind = crop_kind,
                              .sample_next = false,
                              .found = false,
                              .cropped = false};

    while (ctx.past < ctx.len - 1) {
        lt_ret_t rv = parse_object(&ctx);
        if (rv != LT_OK) return rv;
    };

    if (!ctx.found) return LT_CERT_ITEM_NOT_FOUND;

    return LT_OK;
}
