/** @file
 Function definition for font database support

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

#ifndef _FONT_ENTRY_POINT_JPN_H
#define _FONT_ENTRY_POINT_JPN_H

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/FontDatabase.h>

/**
 Initializes the Font Database Driver

 @param[in] ImageHandle    The firmware allocated handle for the EFI image.
 @param[in] SystemTable    A pointer to the EFI System Table.

 @retval EFI_SUCCESS    The entry point is executed successfully.
 @retval Others         Some error occurs when executing this entry point.
**/
EFI_STATUS
InitializeFontDatabase (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  );

#endif

