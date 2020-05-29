/** @file

  Header file of BIOS Storage PEI implementation.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _EFI_PPI_BIOS_STORAGE_PEIM_H_
#define _EFI_PPI_BIOS_STORAGE_PEIM_H_

//
// Statements that include other files.
//
#include <PiPei.h>
#include <Library/HobLib.h>
#include <Library/CmosLib.h>

#include <Ppi/BiosStorage.h>
#include <Guid/BiosEventLogHob.h>


EFI_STATUS
EFIAPI
PeiBiosStorageInit (
  IN EFI_PEI_FILE_HANDLE              FfsHeader,
  IN CONST EFI_PEI_SERVICES           **PeiServices
  );

EFI_STATUS
EFIAPI
PeiWriteEvent (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN  EFI_PEI_EVENT_LOG_PPI             *This,
  IN  UINT8                             BiosStorageType,
  IN BIOS_EVENT_LOG_POST_ERROR_BITMAP_FIRST_DWORD PostBitmap1,
  IN BIOS_EVENT_LOG_POST_ERROR_BITMAP_FIRST_DWORD PostBitmap2
  );

#endif

