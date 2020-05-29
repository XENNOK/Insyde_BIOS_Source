/** @file
 Definitions for Unicode Collation Protocol Implementation

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

#ifndef _UNICODE_COLLATION_H
#define _UNICODE_COLLATION_H

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include <Library/HiiLib.h>
#include <Library/VariableLib.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/UnicodeCollation.h>
#include <Protocol/FontDatabase.h>
#include <Guid/GlobalVariable.h>

//
// Globals
//
extern CHAR8  *mUpperMap;
extern CHAR8  *mLowerMap;
extern CHAR8  *mInfoMap;
extern CHAR8  mOtherChars[];
//
// Defines
//
#define CHAR_FAT_VALID      0x01
#define TO_UPPER(a)      (CHAR16) (a <= 0xFF ? mUpperMap[a] : a)
#define TO_LOWER(a)      (CHAR16) (a <= 0xFF ? mLowerMap[a] : a)

//
// Prototypes
//

///
/// HII specific Vendor Device Path definition.
///
typedef struct {
  VENDOR_DEVICE_PATH             VendorDevicePath;
  EFI_DEVICE_PATH_PROTOCOL       End;
} HII_VENDOR_DEVICE_PATH;

INTN
EFIAPI
StriColl (
  IN EFI_UNICODE_COLLATION_PROTOCOL           *This,
  IN CHAR16                                   *s1,
  IN CHAR16                                   *s2
  );

BOOLEAN
EFIAPI
MetaiMatch (
  IN EFI_UNICODE_COLLATION_PROTOCOL          *This,
  IN CHAR16                                  *String,
  IN CHAR16                                  *Pattern
  );

VOID
EFIAPI
StrLwr (
  IN EFI_UNICODE_COLLATION_PROTOCOL          *This,
  IN OUT CHAR16                              *Str
  );

VOID
EFIAPI
StrUpr (
  IN EFI_UNICODE_COLLATION_PROTOCOL          *This,
  IN OUT CHAR16                              *Str
  );

VOID
EFIAPI
FatToStr (
  IN EFI_UNICODE_COLLATION_PROTOCOL          *This,
  IN UINTN                                   FatSize,
  IN CHAR8                                   *Fat,
  OUT CHAR16                                 *String
  );

BOOLEAN
EFIAPI
StrToFat (
  IN EFI_UNICODE_COLLATION_PROTOCOL          *This,
  IN CHAR16                                  *String,
  IN UINTN                                   FatSize,
  OUT CHAR8                                  *Fat
  );

EFI_STATUS
EFIAPI
InitializeUnicodeCollation (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  );

EFI_STATUS
SetSupportLanguageFonts (
  VOID
  );

#endif

