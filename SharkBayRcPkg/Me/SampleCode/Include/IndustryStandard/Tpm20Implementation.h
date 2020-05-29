/** @file

  Definitions for Tpm 2.0 implementation

@copyright
  Copyright (c) 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/

#ifndef    _IMPLEMENTATION_H
#define    _IMPLEMENTATION_H

// Table 205 -- Hash Algorithm Digest and Block Size Values
#define    SHA1_DIGEST_SIZE            20
#define    SHA1_BLOCK_SIZE             64
#define    SHA256_DIGEST_SIZE          32
#define    SHA256_BLOCK_SIZE           64
#define    SM3_256_DIGEST_SIZE         32
#define    SM3_256_BLOCK_SIZE          64
#define    SHA384_DIGEST_SIZE          48
#define    SHA384_BLOCK_SIZE           128
#define    SHA512_DIGEST_SIZE          64
#define    SHA512_BLOCK_SIZE           128
#define    WHIRLPOOL512_DIGEST_SIZE    64
#define    WHIRLPOOL512_BLOCK_SIZE     64

// Table 206 -- DER Values
#define    SHA1_DER_SIZE      15
#define    SHA1_DER           {0x30,0x21,0x30,0x09,0x06,0x05,0x2B,0x0E,0x03,0x02,0x1A,0x05,0x00,0x04,0x14}
#define    SHA256_DER_SIZE    19
#define    SHA256_DER         {0x30,0x31,0x30,0x0d,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x01,0x05,0x00,0x04,0x20}
#define    SHA384_DER_SIZE    19
#define    SHA384_DER         {0x30,0x41,0x30,0x0d,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x02,0x05,0x00,0x04,0x30}
#define    SHA512_DER_SIZE    19
#define    SHA512_DER         {0x30,0x51,0x30,0x0d,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x03,0x05,0x00,0x04,0x40}

// Table 207 -- Architectural Limits Values
#define    MAX_SESSION_NUMBER    3

// Table 208 -- Minimum and Maximum Values
#ifndef UINT8_MAX
#define    UINT8_MAX     255
#endif
#define    BYTE_MAX      255
#define    INT8_MIN      -128
#define    INT8_MAX      127
#define    UINT16_MAX    65535
#define    INT16_MIN     -32768
#define    INT16_MAX     32767
#define    UINT32_MAX    4294967295
#define    INT32_MIN     -2147483648
#define    INT32_MAX     2147483647
#define    UINT64_MAX    18446744073709551615    // 1.84467440737096e+019
#define    INT64_MIN     -9223372036854775808    // 1
#define    INT64_MAX     9223372036854775807    // 9.22337203685478e+018

// Table 209 -- Logic Values
#define    YES      1
#define    NO       0
//
// BUGBUG: Comment to to resolve duplicated definition
//
//#define    TRUE     1
//#define    FALSE    0
#define    SET      1
#define    CLEAR    0

// Table 210 -- Processor Values
#define    BIG_ENDIAN       NO    // 0
#define    LITTLE_ENDIAN    YES    // 1
#define    NO_AUTO_ALIGN    NO    // 0

/* Table 211 -- Implemented Algorithms
#define    RSA               YES    // 1
#define    DES               YES    // 1
#define    _3DES             YES    // 1
#define    SHA1              YES    // 1
#define    HMAC              YES    // 1
#define    AES               YES    // 1
#define    MGF1              YES    // 1
#define    XOR               YES    // 1
#define    KEYEDHASH         YES    // 1
#define    SHA256            YES    // 1
#define    SHA384            YES    // 1
#define    SHA512            YES    // 1
#define    WHIRLPOOL512      YES    // 1
#define    SM3_256           YES    // 1
#define    SMS4              YES    // 1
#define    RSASSA            RSA    // 1
#define    RSAES             RSA    // 1
#define    RSAPSS            RSA    // 1
#define    OAEP              RSA    // 1
#define    ECC               YES    // 1
#define    ECDH              YES    // 1
#define    ECDSA             ECC    // 1
#define    ECDAA             ECC    // 1
#define    ECSCHNORR         ECC    // 1
#define    SYMCIPHER         YES    // 1
#define    KDF1_SP800_56a    ECC    // 1
#define    KDF2              YES    // 1
#define    KDF1_SP800_108    YES    // 1
#define    SP800_56a_C1_1    ECC    // 1
#define    CTR               YES    // 1
#define    OFB               YES    // 1
#define    CBC               YES    // 1
#define    CFB               YES    // 1
#define    ECB               YES    // 1

*/

// Table 212 -- Implemented Algorithm Constants
#define    RSA_KEY_SIZES_BITS    {1024,2048}
#define    MAX_RSA_KEY_BITS      2048
#define    MAX_RSA_KEY_BYTES     ((MAX_RSA_KEY_BITS+7)/8)    // 256
#define    ECC_CURVES            {TPM_ECC_NIST_P256,TPM_ECC_BN_P256}
#define    ECC_KEY_SIZES_BITS    {256}
#define    MAX_ECC_KEY_BITS      256
#define    MAX_ECC_KEY_BYTES     ((MAX_ECC_KEY_BITS+7)/8)    // 32
#define    AES_KEY_SIZES_BITS    {128}
#define    MAX_AES_KEY_BITS      128
#define    MAX_AES_KEY_BYTES     ((MAX_AES_KEY_BITS+7)/8)    // 16
#define    MAX_SYM_KEY_BITS      128
#define    MAX_SYM_KEY_BYTES     ((MAX_SYM_KEY_BITS+7)/8)    // 16
#define    MAX_SYM_BLOCK_SIZE    16

// Table 213 -- Implementation Values
#define    FIELD_UPGRADE_IMPLEMENTED      NO    // 0
typedef    UINT16                         BSIZE;
#define    IMPLEMENTATION_PCR             24
#define    PLATFORM_PCR                   24
#define    DRTM_PCR                       17
#define    NUM_LOCALITIES                 5
#define    MAX_HANDLE_NUM                 3
#define    MAX_ACTIVE_SESSIONS            64
typedef    UINT16                         CONTEXT_SLOT;
typedef    UINT64                         CONTEXT_COUNTER;
#define    MAX_LOADED_SESSIONS            3
#define    MAX_SESSION_NUM                3
#define    MAX_LOADED_OBJECTS             3
#define    MIN_EVICT_OBJECTS              2
#define    PCR_SELECT_MIN                 ((PLATFORM_PCR+7)/8)    // 3
#define    PCR_SELECT_MAX                 ((IMPLEMENTATION_PCR+7)/8)    // 3
#define    NUM_POLICY_PCR_GROUP           1
#define    NUM_AUTHVALUE_PCR_GROUP        1
#define    MAX_CONTEXT_SIZE               4000
#define    MAX_DIGEST_BUFFER              1024
#define    MAX_NV_INDEX_SIZE              1024
#define    MAX_CAP_BUFFER                 1024
#define    NV_MEMORY_SIZE                 16384
#define    NUM_STATIC_PCR                 16
#define    MAX_ALG_LIST_SIZE              64
#define    TIMER_PRESCALE                 100000
#define    PRIMARY_SEED_SIZE              32
#define    CONTEXT_ENCRYPT_ALG            TPM_ALG_AES
#define    CONTEXT_ENCRYPT_KEY_BITS       MAX_SYM_KEY_BITS    // 128
#define    CONTEXT_ENCRYPT_KEY_BYTES      ((CONTEXT_ENCRYPT_KEY_BITS+7)/8)    // 16
#define    CONTEXT_INTEGRITY_HASH_ALG     TPM_ALG_SHA256
#define    CONTEXT_INTEGRITY_HASH_SIZE    SHA256_DIGEST_SIZE    // 32
#define    PROOF_SIZE                     CONTEXT_INTEGRITY_HASH_SIZE    // 32
#define    NV_CLOCK_UPDATE_INTERVAL       12
#define    NUM_POLICY_PCR                 1
#define    MAX_COMMAND_SIZE               4096
#define    MAX_RESPONSE_SIZE              4096
#define    ORDERLY_BITS                   8
#define    MAX_ORDERLY_COUNT              ((1<<ORDERLY_BITS)-1)    // 255
#define    ALG_ID_FIRST                   TPM_ALG_FIRST
#define    ALG_ID_LAST                    TPM_ALG_LAST
#define    MAX_SYM_DATA                   128
#define    MAX_HASH_STATE_SIZE            512
#define    MAX_RNG_ENTROPY_SIZE           64
#define    RAM_INDEX_SPACE                512
#define    RSA_DEFAULT_PUBLIC_EXPONENT    0x00010001

///

/*(auto)

    Automatically Generated by DoImplemented.pl

    Date: Mar  5, 2012
    Time: 11:14:55 PM

*/

// Table 6 -- TPM_ALG_ID Constants <I/O,S>
typedef UINT16 TPM_ALG_ID;
//
// BUGBUG: Comment some algo which has same name as TPM1.2 (value is same, so not runtime issue)
//
#define    TPM_ALG_ERROR             (TPM_ALG_ID)(0x0000)        // a: ; D:
#define    TPM_ALG_FIRST             (TPM_ALG_ID)(0x0001)        // a: ; D:
//#define    TPM_ALG_RSA               (TPM_ALG_ID)(0x0001)        // a: A O; D:
//#define    TPM_ALG_DES               (TPM_ALG_ID)(0x0002)        // a: S; D:
#define    TPM_ALG__3DES             (TPM_ALG_ID)(0x0003)        // a: S; D:
//#define    TPM_ALG_SHA               (TPM_ALG_ID)(0x0004)        // a: H; D:
#define    TPM_ALG_SHA1              (TPM_ALG_ID)(0x0004)        // a: H; D:
//#define    TPM_ALG_HMAC              (TPM_ALG_ID)(0x0005)        // a: H X; D:
#define    TPM_ALG_AES               (TPM_ALG_ID)(0x0006)        // a: S; D:
//#define    TPM_ALG_MGF1              (TPM_ALG_ID)(0x0007)        // a: H M; D:
#define    TPM_ALG_KEYEDHASH         (TPM_ALG_ID)(0x0008)        // a: H E X O; D:
//#define    TPM_ALG_XOR               (TPM_ALG_ID)(0x000A)        // a: H S; D:
#define    TPM_ALG_SHA256            (TPM_ALG_ID)(0x000B)        // a: H; D:
#define    TPM_ALG_SHA384            (TPM_ALG_ID)(0x000C)        // a: H; D:
#define    TPM_ALG_SHA512            (TPM_ALG_ID)(0x000D)        // a: H; D:
#define    TPM_ALG_WHIRLPOOL512      (TPM_ALG_ID)(0x000E)        // a: H; D:
#define    TPM_ALG_NULL              (TPM_ALG_ID)(0x0010)        // a: ; D:
#define    TPM_ALG_SM3_256           (TPM_ALG_ID)(0x0012)        // a: H; D:
#define    TPM_ALG_SMS4              (TPM_ALG_ID)(0x0013)        // a: S; D:
#define    TPM_ALG_RSASSA            (TPM_ALG_ID)(0x0014)        // a: X; D: RSA
#define    TPM_ALG_RSAES             (TPM_ALG_ID)(0x0015)        // a: E; D: RSA
#define    TPM_ALG_RSAPSS            (TPM_ALG_ID)(0x0016)        // a: X; D: RSA
#define    TPM_ALG_OAEP              (TPM_ALG_ID)(0x0017)        // a: E; D: RSA
#define    TPM_ALG_ECDSA             (TPM_ALG_ID)(0x0018)        // a: X; D: ECC
#define    TPM_ALG_ECDH              (TPM_ALG_ID)(0x0019)        // a: M; D: ECC
#define    TPM_ALG_ECDAA             (TPM_ALG_ID)(0x001A)        // a: A X; D: ECC
#define    TPM_ALG_ECSCHNORR         (TPM_ALG_ID)(0x001C)        // a:  A X; D: ECC
#define    TPM_ALG_KDF1_SP800_56a    (TPM_ALG_ID)(0x0020)        // a: H M; D: ECC
#define    TPM_ALG_KDF2              (TPM_ALG_ID)(0x0021)        // a: H M; D:
#define    TPM_ALG_KDF1_SP800_108    (TPM_ALG_ID)(0x0022)        // a: H M; D:
#define    TPM_ALG_ECC               (TPM_ALG_ID)(0x0023)        // a: A O; D:
#define    TPM_ALG_SYMCIPHER         (TPM_ALG_ID)(0x0025)        // a: O; D:
#define    TPM_ALG_CTR               (TPM_ALG_ID)(0x0040)        // a: S E; D:
#define    TPM_ALG_OFB               (TPM_ALG_ID)(0x0041)        // a: S E; D:
#define    TPM_ALG_CBC               (TPM_ALG_ID)(0x0042)        // a: S E; D:
#define    TPM_ALG_CFB               (TPM_ALG_ID)(0x0043)        // a: S E; D:
#define    TPM_ALG_ECB               (TPM_ALG_ID)(0x0044)        // a: S E; D:
#define    TPM_ALG_LAST              (TPM_ALG_ID)(0x0044)        // a: ; D:

// Table 7 -- TPM_ECC_CURVE Constants <I/O,S>
typedef UINT16 TPM_ECC_CURVE;

#define    TPM_ECC_NONE         (TPM_ECC_CURVE)(0x0000)
#define    TPM_ECC_NIST_P192    (TPM_ECC_CURVE)(0x0001)
#define    TPM_ECC_NIST_P224    (TPM_ECC_CURVE)(0x0002)
#define    TPM_ECC_NIST_P256    (TPM_ECC_CURVE)(0x0003)
#define    TPM_ECC_NIST_P384    (TPM_ECC_CURVE)(0x0004)
#define    TPM_ECC_NIST_P521    (TPM_ECC_CURVE)(0x0005)
#define    TPM_ECC_BN_P256      (TPM_ECC_CURVE)(0x0010)
#define    TPM_ECC_BN_P638      (TPM_ECC_CURVE)(0x0011)

//#define      MAX_DIGEST_SIZE        32
//#define      MAX_HASH_BLOCK_SIZE    64
//
// BUGBUG: Always set 6 here, because we want to support all hash algo in BIOS.
//
#define      HASH_COUNT             6

#endif
