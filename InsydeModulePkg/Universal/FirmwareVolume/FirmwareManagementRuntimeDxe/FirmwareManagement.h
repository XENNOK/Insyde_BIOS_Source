/** @file
  Type definitions for System Firmware Management RuntimeDxe driver

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

#ifndef _FIRMWARE_MANAGEMENT_RUNTIME_DXE_H_
#define _FIRMWARE_MANAGEMENT_RUNTIME_DXE_H_

#include <Uefi.h>

#define MAX_RETRY_CNT                         3
#define MAX_STRING_LEN                        32
#define MAX_FVB_BLOCK_SIZE                    0x10000
#define GET_IMAGE_PROGRESS_NAME               L"GetImageProgress"
#define FIRMWARE_IMAGE_SIGNATURE              SIGNATURE_32('F','I','M','G')
#define FIRMWARE_IMAGE_ENTRY_FROM_LINK(a)     CR(a, FIRMWARE_IMAGE_ENTRY, Link, FIRMWARE_IMAGE_SIGNATURE)


typedef struct {
  EFI_GUID    ImageTypeGuid;
  CHAR16      ImageName[MAX_STRING_LEN];
} IMAGE_MAP;

typedef struct {
  UINT32                             Signature;
  LIST_ENTRY                         Link;
  UINT8                              ImageIndex;
  EFI_GUID                           ImageTypeGuid;
  UINTN                              ImageOffset;
  UINTN                              ImageSize;
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL *Fvb;
} FIRMWARE_IMAGE_ENTRY;

#endif