/** @file
  Header file for ImageRelocationLib

  Image relocation related functions. In current design, only support relocate
  BS driver to runtime driver.

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

#ifndef _IMAGE_RELOCATION_LIB_H_
#define _IMAGE_RELOCATION_LIB_H_

#include <Uefi.h>
#include <PiDxe.h>

/**
  Function uses image handle to check this driver is runtime driver or not

  @param[in] ImageHandle   Input Image handle.

  @retval TRUE             This is a runtime driver.
  @retval FALSE            This isn't a runtime driver.
**/
BOOLEAN
IsRuntimeDriver (
  IN EFI_HANDLE       ImageHandle
  );

/**
  Relocation this driver to RuntimeService from DXE
  It will read image from FV, then LoadImage and StartImage to relocation driver

  @param[in] ParentImageHandle     Parent image to load runtime image

  @retval EFI_SUCCESS              Relocation success
  @retval others                   Failed from another driver
**/
EFI_STATUS
RelocateImageToRuntimeDriver (
  IN  EFI_HANDLE                        ParentImageHandle
  );


#endif