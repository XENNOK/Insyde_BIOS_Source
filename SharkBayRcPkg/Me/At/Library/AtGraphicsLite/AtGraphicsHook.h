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

#ifndef _EFI_AT_GRAPHICS_HOOK_H_
#define _EFI_AT_GRAPHICS_HOOK_H_

#include <Library/BadgingSupportLib.h>

#include <Protocol/OEMBadgingSupport.h>

/**
  Get String information which should be shown on screen

  @param [in]  This               Protocol instance pointer.
  @param [in]  Index              String Index of mOemBadgingString array
  @param [in]  AfterSelect        No use any more
  @param [in]  SelectedStringNum  String number High nibble:Type, Low nibble: Index of string Type
  @param [out] StringData         String data to output on screen
  @param [out] CoordinateX        String data location on screen
  @param [out] CoordinateY        String data location on screen
  @param [out] Foreground         Foreground color information of the string
  @param [out] Background         Background color information of the string
  @param [in]  OemBadgingInfo     point to Oem Badging Info    
  @param [in]  StringCountArray   Point to array count
  @param [in]  StringArray        Point to String array
  @param [in]  PassWordTypeflag1  Point to PassWord Type
  @param [in]  PassWordTypeflag2  Point to PassWord Type

  @retval OEM BADGING STRING
  
**/
OEM_BADGING_STRING *
EFIAPI
GetOemString (
  IN     EFI_OEM_BADGING_SUPPORT_PROTOCOL  *This,
  IN     UINTN                             Index,
  IN     BOOLEAN                           AfterSelect,
  IN     UINT8                             SelectedStringNum,
  OUT    CHAR16                            **StringData,
  OUT    UINTN                             *CoordinateX,
  OUT    UINTN                             *CoordinateY,
  OUT    EFI_UGA_PIXEL                     *Foreground,
  OUT    EFI_UGA_PIXEL                     *Background,
  IN     OEM_BADGING_INFO                  *OemBadgingInfo,  
  IN     UINTN                             *StringCountArray,
  IN     OEM_BADGING_STRING                **StringArray,
  IN     UINTN                             *PassWordTypeflag1,
  IN     UINTN                             *PassWordTypeflag2
  );

#endif
