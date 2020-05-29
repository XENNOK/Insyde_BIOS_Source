/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/*++

Module Name:

  BiosRegionLock.h

Abstract:

  This driver registers event to enable BIOS region protection before boot to OS

--*/
#ifndef _BIOS_REGION_LOCK_H_
#define _BIOS_REGION_LOCK_H_

typedef struct _BIOS_REGION_LOCK_PROTOCOL BIOS_REGION_LOCK_PROTOCOL;

// {E687E50B-C98D-4780-A7B0-E4C7C4AF7F69}
#define BIOS_REGION_LOCK_PROTOCOL_GUID \
 {\
  0xe687e50b, 0xc98d, 0x4780, 0xa7, 0xb0, 0xe4, 0xc7, 0xc4, 0xaf, 0x7f, 0x69\
 }

extern EFI_GUID gEfiBiosRegionLockProtocolGuid;

typedef enum {
  FVMAIN = 0,
  NV_COMMON_STORE,
  NV_COMMON_STORE_MICROCODE,
  NV_COMMON_STORE_OEM_DMI_STORE,
  NV_COMMON_STORE_RESERVED1,
  NV_COMMON_STORE_NV_BVDT,
  NV_COMMON_STORE_RESERVED2,
  NVSTORAGE,
  NVSTORAGE_NV_VARIABLE_STORE,
  NVSTORAGE_NV_FTW_WORKING,
  NVSTORAGE_NV_FTW_SPARE,
  FV_RECOVERY
} BIOS_REGION_TYPE;

typedef struct {
  UINT32                 Base;
  UINT32                 Size;
} BIOS_PROTECT_REGION;

typedef
EFI_STATUS
(EFIAPI *SET_REGION_BY_TYPE) (
  IN BIOS_REGION_LOCK_PROTOCOL  *This,
  IN BIOS_REGION_TYPE            Type
);

typedef
EFI_STATUS
(EFIAPI *SET_REGION_BY_ADDRESS) (
  IN BIOS_REGION_LOCK_PROTOCOL  *This,
  IN UINTN                       BaseAddress,
  IN UINTN                       Length 
);

typedef
EFI_STATUS
(EFIAPI *CLEAR_REGION_BY_TYPE) (
  IN BIOS_REGION_LOCK_PROTOCOL  *This,
  IN BIOS_REGION_TYPE            Type
);

typedef
EFI_STATUS
(EFIAPI *CLEAR_REGION_BY_ADDRESS) (
  IN BIOS_REGION_LOCK_PROTOCOL  *This,
  IN UINTN                       BaseAddress,
  IN UINTN                       Length 
);

typedef
EFI_STATUS
(EFIAPI *LOCK) (
  IN BIOS_REGION_LOCK_PROTOCOL  *This
);

struct _BIOS_REGION_LOCK_PROTOCOL {
  SET_REGION_BY_TYPE       SetRegionByType;
  SET_REGION_BY_ADDRESS    SetRegionByAddress;
  CLEAR_REGION_BY_TYPE     ClearRegionByType;
  CLEAR_REGION_BY_ADDRESS  ClearRegionByAddress;
  LOCK                     Lock;
};

#endif //_BIOS_REGION_LOCK_H_
