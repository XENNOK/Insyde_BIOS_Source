/** @file
  Content file contains function definitions for Variable Edit Driver.

;******************************************************************************
;* Copyright (c) 1983 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

#ifndef _VARIABLE_EDIT_H_
#define _VARIABLE_EDIT_H_

#include <ChipsetSetupConfig.h>
#include <Uefi/UefiBaseType.h>
#include <Uefi/UefiInternalFormRepresentation.h>
#include <Csm/LegacyBiosDxe/LegacyBiosInterface.h>

#include <Library/SetupUtilityLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/LoadedImage.h>
#include <Protocol/LegacyRegion2.h>
#include <Protocol/VariableEdit.h>
#include <Protocol/SetupCallbackSync.h>
#include <Protocol/SetupUtilityBrowser.h>
#include <Protocol/SysPasswordService.h>

#include <Guid/VariableEdit.h>
//[-start-140721-IB08620401-add]//
#include <Guid/VariableEditFunctionDisable.h>
//[-end-140721-IB08620401-add]//

#define VARIABLE_EDIT_IDENTIFY_GUID \
  { \
    0x7a633d68, 0x33f8, 0x428d, { 0xa8, 0xb9, 0x87, 0xf8, 0x37, 0x27, 0x06, 0x21 } \
  }

#define VARIABLE_EDIT_PTR_SIGNATURE     SIGNATURE_64 ('$', 'I', 'F', 'R', 'P', 'K', 'G', 1)
#define FILE_VERSION_SIGNATURE          SIGNATURE_64 ('$', 'I', 'F', 'R', 'P', 'K', 'G', 0x21)

#define F0000Region                     0x01
#define E0000Region                     0x02
#define VARIABLE_EDIT_PTR_ALIGNMENT     0x10
#define VARIABLE_EDIT_TABLE_MAX_ADDRESS 0xFFFFFFFF  //4G
#define VARIABLE_EDIT_TABLE_SIGNATURE   VARIABLE_EDIT_PTR_SIGNATURE

#define FIELD_SIZE(s,m)                 (sizeof ((((s *) 0)->m)))
#define EFI_FIELD_OFFSET(TYPE,Field)    ((UINTN)(&(((TYPE *) 0)->Field)))

#define IS_VARIABLE_EDIT_PTR(p)          ((p)->Signature == VARIABLE_EDIT_PTR_SIGNATURE)
#define IS_VARIABLE_EDIT_TABLE(p)        ((p)->Signature == VARIABLE_EDIT_TABLE_SIGNATURE)

#pragma pack(1)

typedef struct _FILE_VERSION {
  UINT64                Signature;
  UINT32                MajorVersion;
  UINT32                MinorVersion;
} FILE_VERSION;

typedef struct _VFR_STR_OFFSET_PAIR {
  UINT32                StrPkgOffset;
  UINT32                VfrBinOffset;
} VFR_STR_OFFSET_PAIR;

typedef struct _VARIABLE_EDIT_PTR {
  UINT64                Signature;
  UINT32                Address;
  UINT32                Size;
} VARIABLE_EDIT_PTR;

typedef struct _VARIABLE_EDIT_TABLE {
  UINT64                Signature;
  UINT8                 SmiNumber;
  UINT8                 BootTypeOrderSize;
  UINT16                BootTypeOrderOffset;
  UINT8                 Reserve0[4];
  UINT8                 Reserve1[60];
  UINT32                PairCount;
} VARIABLE_EDIT_TABLE;

#pragma pack()

#endif

