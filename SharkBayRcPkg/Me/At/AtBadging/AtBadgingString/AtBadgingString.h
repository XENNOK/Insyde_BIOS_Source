/** @file



;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _EFI_AT_BADGING_STRING_H
#define _EFI_AT_BADGING_STRING_H

#include <Uefi.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/HiiLib.h>

#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiString.h>

#include <At/Library/AtGraphicsLite/AtGraphicsLib.h>
#include <At/Library/AtGraphicsLite/AtGraphicsHook.h>

#include <Guid/Pcx.h>
#include <Protocol/ExitAtAuth.h>

#define EFI_INSYDE_BADGING_GUID \
  { 0x96DCE314, 0x3628, 0x4256, { 0xA0, 0x2E, 0x3B, 0x10, 0xC6, 0x1D, 0x85, 0xFD }}

#define OEM_STRING_LOCATION_X         50
#define OEM_STRING1_LOCATION_Y        50
#define THEFT_TRIGGER_STRINGS_COUNT   5
#define TOP_SCREEN_STRINGS_COUNT      sizeof(mClearTopScreenString)/sizeof(OEM_BADGING_STRING) 
#define BOTTOM_SCREEN_STRINGS_COUNT   sizeof(mClearBottomScreenString)/sizeof(OEM_BADGING_STRING) 
#define MAX_STRING_LANE               120

typedef struct _LANGUAGE_COORDINATE_OFFSET {
  UINTN ShowTimeOffsetX;
  UINTN ShowPasswordOffsetX1;
  UINTN ShowPasswordOffsetX2;
  UINTN ShowPasswordOffsetX3;
} LANGUAGE_COORDINATE_OFFSET;

/**
  Get string by string id from HII Interface

  @param [in] Id                  String ID.

  @retval String from ID.
  
**/
CHAR16 *
GetStringById (
  IN  EFI_STRING_ID   Id
  );

/**
  Get Number of string that should be shown on screen

  @param none

  @retval EFI_SUCCESS            Always return EFI_SUCCESS.
  
**/
EFI_STATUS
InitializeStringSupport (
  VOID
  );

#endif
