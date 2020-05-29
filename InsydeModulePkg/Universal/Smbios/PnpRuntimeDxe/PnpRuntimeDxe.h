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

#ifndef _PNP_RUNTIME_DXE_H_
#define _PNP_RUNTIME_DXE_H_

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/IrsiRegistrationLib.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <Guid/IrsiFeature.h>
#include <Guid/EventGroup.h>
#include <Library/BaseMemoryLib.h>

#include "PnpDmi.h"

#define GPNV_SIGNATURE                    SIGNATURE_32 ('G', 'P', 'N', 'V')


#pragma pack(1)

typedef struct {
  UINT16                                Handle;
  UINTN                                 MinGPNVSize;
  UINTN                                 GPNVSize;
  UINTN                                 BaseAddress;
} PLATFORM_GPNV_MAP;

typedef struct {
  PLATFORM_GPNV_MAP                 PlatFormGPNVMap[20];
} PLATFORM_GPNV_MAP_BUFFER;

//
// GPNV Header
//
typedef struct {
  UINT32                            Signature;    //"GPNV"
  UINT32                            TotalLength;
  UINT8                             Reserved[8];
} PLATFORM_GPNV_HEADER;

//
// global variables collection
//
typedef struct {
  PLATFORM_GPNV_MAP_BUFFER              GPNVMapBuffer;
  PLATFORM_GPNV_HEADER                  *GPNVHeader;
  UINTN                                 UpdatableStringCount;
  DMI_UPDATABLE_STRING                  *UpdatableStrings;
  UINTN                                 OemGPNVHandleCount;
  //
  // smbios table runtime address
  //
  EFI_PHYSICAL_ADDRESS                  SMBIOSTableEntryAddress;
  SMBIOS_TABLE_ENTRY_POINT              *SmbiosTableEntryPoint;
  SMBIOS_STRUCTURE                      *SmbiosTable;
  //
  // Runtime buffers
  //
  UINT8                                 *DmiVariableBuf;
  UINT8                                 *SmbiosStoreArea;
  UINT8                                 *TempStoreArea;
} PNP_RUNTIME_PRIVATE_DATA;

#pragma pack()

//
// exported for PnpRuntime services usage
//
extern PNP_RUNTIME_PRIVATE_DATA  *mPrivateData;

#endif