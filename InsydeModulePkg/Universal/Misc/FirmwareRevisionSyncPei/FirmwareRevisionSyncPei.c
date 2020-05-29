/** @file
  Firmware Revision Synchronization with BVDT table

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/BvdtLib.h>

#define MAX_STRING_LEN    64


/**
 Firmware Revision Sync. Entry Point

 @param [in]   FileHandle
 @param [in]   PeiServices      General purpose services available to every PEIM

 @retval EFI_SUCCESS            Success

**/
EFI_STATUS
FirmwareRevisionSyncEntry (
  IN EFI_PEI_FILE_HANDLE             FileHandle,
  IN CONST EFI_PEI_SERVICES          **PeiServices
  )
{
  EFI_STATUS                      Status;
  UINT32                          FirmwareRevision;
  EFI_GUID                        EsrtGuid;
  UINTN                           StringSize;
  CHAR16                          FirmwareRevisionStr[MAX_STRING_LEN];
  CHAR16                          FirmwareReleaseDateStr[MAX_STRING_LEN];


  Status = GetEsrtFirmwareInfo (&EsrtGuid, &FirmwareRevision);
  if (Status == EFI_SUCCESS) {
    PcdSet32 (PcdFirmwareRevision, FirmwareRevision);
  }

  StringSize = MAX_STRING_LEN;
  Status = GetBvdtInfo (BvdtBiosVer, &StringSize, FirmwareRevisionStr);
  if (Status == EFI_SUCCESS) {
    StringSize *= sizeof (CHAR16);
    PcdSetPtr (PcdFirmwareVersionString, &StringSize, FirmwareRevisionStr);
  }

  StringSize = MAX_STRING_LEN;
  Status = GetBvdtInfo (BvdtReleaseDate, &StringSize, FirmwareReleaseDateStr);
  if (Status == EFI_SUCCESS) {
    StringSize *= sizeof (CHAR16);
    PcdSetPtr (PcdFirmwareReleaseDateString, &StringSize, FirmwareReleaseDateStr);
  }


  return EFI_SUCCESS;
}

