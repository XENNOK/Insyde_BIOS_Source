//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _TXT_ACM_LIB_H_
#define _TXT_ACM_LIB_H_

/*
 * authenticated code (AC) module header (ver 0.0)
 */

typedef union {
    UINT16 _raw;
    struct {
        UINT16  reserved          : 14;
        UINT16  pre_production    : 1;
        UINT16  debug_signed      : 1;
    } r;
} ACM_FLAGS;

typedef struct {
    UINT32     module_type;
    UINT32     header_len;
    UINT32     header_ver;          /* currently 0.0 */
    UINT16     chipset_id;
    ACM_FLAGS  flags;
    UINT32     module_vendor;
    UINT32     date;
    UINT32     size;
    UINT32     reserved1;
    UINT32     code_control;
    UINT32     error_entry_point;
    UINT32     gdt_limit;
    UINT32     gdt_base;
    UINT32     seg_sel;
    UINT32     entry_point;
    UINT8      reserved2[64];
    UINT32     key_size;
    UINT32     scratch_size;
    UINT8      rsa2048_pubkey[256];
    UINT32     pub_exp;
    UINT8      rsa2048_sig[256];
    UINT32     scratch[143];
    UINT8      user_area[1];
} ACM_HDR;

/* value of module_type field */
#define ACM_TYPE_CHIPSET        0x02

/* value of module_vendor field */
#define ACM_VENDOR_INTEL        0x8086

/*
 * SINIT/MLE capabilities
 */
typedef union {
    UINT32  _raw;
    struct {
        UINT32  rlp_wake_getsec     : 1;
        UINT32  rlp_wake_monitor    : 1;
        UINT32  ecx_pgtbl           : 1;
        UINT32  reserved            : 29;
    } r;
} TXT_CAPS;

typedef struct {
    EFI_GUID      uuid;
    UINT8         chipset_acm_type;
    UINT8         version;             /* currently 3 */
    UINT16        length;
    UINT32        chipset_id_list;
    UINT32        os_sinit_data_ver;
    UINT32        min_mle_hdr_ver;
    TXT_CAPS      capabilities;
    UINT8         acm_ver;
    UINT8         reserved[3];
} ACM_INFO_TABLE;

/* ACM UUID value */
#define ACM_UUID_V3 \
  { 0x7fc03aaa, 0x46a7, 0x18db, 0x2e, 0xac, 0x69, 0x8f, 0x8d, 0x41, 0x7f, 0x5a }

/* chipset_acm_type field values */
#define ACM_CHIPSET_TYPE_BIOS         0x00
#define ACM_CHIPSET_TYPE_SINIT        0x01

BOOLEAN
EFIAPI
IsSinitAcm (
  IN VOID         *AcmBase,
  IN UINTN        AcmSize,
  IN BOOLEAN      Quiet
  )
;

UINT16
EFIAPI
GetSinitAcmChipsetId (
  IN VOID         *AcmBase,
  IN UINTN        AcmSize
  )
;

#endif /* _TXT_ACM_LIB_H_ */
