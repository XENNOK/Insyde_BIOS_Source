/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  The header file for TPM physical presence driver.

Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __PHYSICAL_PRESENCE_H__
#define __PHYSICAL_PRESENCE_H__

#include <PiDxe.h>

#include <Protocol/TcgService.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/TpmPhysicalPresence.h>
#include <Protocol/VariableLock.h>
#include <Protocol/EndOfBdsBootSelection.h>

//
// Work around for OemFormBrowser2
//
#ifdef MDE_CPU_ARM
typedef UINT16 STRING_REF;
#endif
#include <Protocol/H2ODialog.h>

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/HiiLib.h>
#include <Library/OemGraphicsLib.h>
#include <Library/DxeOemSvcKernelLib.h>

#include <Guid/EventGroup.h>
#include <Guid/PhysicalPresenceData.h>
#include <Guid/UsbEnumeration.h>

#include <KernelSetupConfig.h>

#define TPM_PP_USER_ABORT                     ((TPM_RESULT)(-0x10))
#define TPM_PP_BIOS_FAILURE                   ((TPM_RESULT)(-0x0f))

#define CONFIRM_BUFFER_SIZE                   4096
#define TPM_MANG_FLAG_READ                    (BOOLEAN)TRUE
#define TPM_MANG_FLAG_WRITE                   (BOOLEAN)FALSE

#define TPM_NV_INDEX_BIOS_MANG_FLAG_BASE      (UINT32)0x50010000
#define PHYSICAL_PRESENCE_NO_ACTION_MAX       22
#pragma pack (push)
#pragma pack (1)
//
// TPM_SetCapability for STCLEAR_DATA
//
typedef struct {
  TPM_CAPABILITY_AREA               CapArea;
  UINT32                            SdSubCapSize;
  TPM_CAPABILITY_AREA               SdSubCap;
  UINT32                            ValueSize;
  UINT8                             Value[1];
} TPM_SET_CAPABILITY_SD_IN;

//
// TPM_NVDefine
//
typedef struct _TPM_PCR_SELECTION_24PCR {
  UINT16                            sizeOfSelect;
  UINT8                             pcrSelect[3];
} TPM_PCR_SELECTION_24PCR;

typedef struct _TPM_PCR_INFO_SHORT_24PCR {
  TPM_PCR_SELECTION_24PCR           pcrSelection;
  TPM_LOCALITY_SELECTION            localityAtRelease;
  TPM_COMPOSITE_HASH                digestAtRelease;
} TPM_PCR_INFO_SHORT_24PCR;

typedef struct _TPM_NV_DATA_PUBLIC_24PCR {
    TPM_STRUCTURE_TAG               tag;
    TPM_NV_INDEX                    nvIndex;
    TPM_PCR_INFO_SHORT_24PCR        pcrInfoRead;
    TPM_PCR_INFO_SHORT_24PCR        pcrInfoWrite;
    TPM_NV_ATTRIBUTES               permission;
    BOOLEAN                         bReadSTClear;
    BOOLEAN                         bWriteSTClear;
    BOOLEAN                         bWriteDefine;
    UINT32                          dataSize;
} TPM_NV_DATA_PUBLIC_24PCR;

typedef struct {
  TPM_NV_DATA_PUBLIC_24PCR      PubInfo;
  TPM_ENCAUTH                   EncAuth;
} TPM_NV_DEFINE_IN_24PCR;

//
// TPM_NVWrite
//
typedef struct {
  TPM_NV_INDEX    NvIndex;
  UINT32          Offset;
  UINT32          DataSize;
  UINT8           Data;
} TPM_NV_WRITEVALUE_IN_ONE_BYTE;

//
// TPM_NVRead
//
typedef struct {
  TPM_NV_INDEX    NvIndex;
  UINT32          Offset;
  UINT32          DataSize;
} TPM_NV_READVALUE_IN;

typedef struct {
  UINT32    DataSize;
  UINT8     Data;
} TPM_NV_READVALUE_OUT_ONE_BYTE;

typedef struct {
  TPM_RQU_COMMAND_HDR               TpmRqu;
  TPM_NV_READVALUE_IN               NvReadValueIn;
} TPM_NV_READ_IN_WITH_HDR;

typedef struct {
  TPM_RSP_COMMAND_HDR               TpmRsp;
  TPM_NV_READVALUE_OUT_ONE_BYTE     NvReadValueOut;
} TPM_NV_READ_OUT_ONE_BYTE_WITH_HDR;
#pragma pack (pop)

typedef
TPM_RESULT
(EFIAPI *TPM_NV_READ_WRITE_ONE_BYTE) (
  IN      EFI_TCG_PROTOCOL          *TcgProtocol,
  IN      TPM_NV_INDEX              NvIndex,
  IN      UINT32                    Offset,
  IN OUT  UINT8                     *Data
  );

EFI_STATUS
EFIAPI
ExecuteTpmPhysicalPresence (
  IN      EFI_TCG_PROTOCOL          *TcgProtocol,
  IN      UINT8                     CommandCode,
  OUT     TPM_RESULT                *Result
  );

EFI_STATUS
EFIAPI
TcgGetTpmManagementFlags (
  IN      EFI_TCG_PROTOCOL               *TcgProtocol,
  OUT     UINT8                          *TpmMangFlagPtr
  );

EFI_STATUS
EFIAPI
ManipulatePhysicalPresence (
  IN      EFI_TCG_PROTOCOL          *TcgProtocol,
  IN      TPM_PHYSICAL_PRESENCE     PhysicalPresence
  );

TPM_RESULT
EFIAPI
TpmNvLock (
  IN      EFI_TCG_PROTOCOL          *TcgProtocol
  );

#endif
