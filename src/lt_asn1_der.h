#ifndef LT_ASN1_DER_H
#define LT_ASN1_DER_H

/**
 * @file lt_asn1_der.h
 * @brief ASN1 DER parser
 * @author Tropic Square s.r.o.
 *
 * @license For the license see file LICENSE.txt file in the root directory of this source tree.
 */

#include "libtropic_common.h"

typedef enum lt_asn1der_obj_kind_t {
    LT_ASN1DER_BOOLEAN = 0x01,
    LT_ASN1DER_INTEGER = 0x02,
    LT_ASN1DER_STRING_BIT = 0x03,
    LT_ASN1DER_STRING_OCTET = 0x04,
    LT_ASN1DER_STRING_NULL = 0x05,
    LT_ASN1DER_OBJECT_IDENTIFIER = 0x06,
    LT_ASN1DER_STRING_UTF8 = 0x0C,
    LT_ASN1DER_STRING_PRINTABLE = 0x13,
    LT_ASN1DER_UTC_TIME = 0x17,
    LT_ASN1DER_SEQUENCE = 0x30,
} lt_asn1der_obj_kind_t;

typedef enum lt_asn1der_crop_kind_t { LT_ASN1DER_CROP_SUFFIX, LT_ASN1DER_CROP_PREFIX } lt_asn1der_crop_kind_t;

#define LT_OBJ_ID_CURVEX25519 0x2B656E

/**
 * @brief Parse ASN1 DER encoded stream and find certain OBJECT. Return data from primitve type
 *        right after the OBJECT_IDENTIFIER. If multiple objects of the searched OBJECT_KIND are
 *        present, return only first one.
 *
 * @param stream        Byte stream with X509 certificate to be parsed
 * @param len           Length of the certificate in the byte-stream
 * @param obj_id        3-byte OBJECT_IDENTIFIER to be searched for
 * @param buf           Buffer where to copy the found object value
 * @param buf_len       Size of the buffer pointed to by "buf"
 * @param crop_kind     If size of the found object is bigger than "buf" buffer, the only
 *                      part of the object is copied to "buf".
 *                          LT_ASN1DER_CROP_SUFFIX - Crop Suffix , copy to "buf" from 0 to "buf_len" - 1
 *                          LT_ASN1DER_CROP_PREFIX - Crop Prefix, copy to "buf" from N - buf_len to N - 1
 *                                                where N is length of the object being sampled.
 * @return lt_ret_t     LT_OK if sucessfull (and the object was found)
 *                      LT_CERT_STORE_INVALID if the stream does not contain valid ASN1 syntax
 *                      LT_CERT_UNSUPPORTED if the ASN1 stream contains features unsupported by this parser
 *                      LT_CERT_ITEM_NOT_FOUND if OBJECT_IDENTIFIER with "obj_id" value was not found!
 */
lt_ret_t asn1der_find_object(const uint8_t *stream, uint16_t len, int32_t obj_id, uint8_t *buf, int buf_len,
                             enum lt_asn1der_crop_kind_t crop_kind) __attribute__((warn_unused_result));

#endif