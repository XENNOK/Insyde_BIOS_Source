/** @file

  BIOS Storage PEI Implementation.

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


#include <BiosStoragePei.h>

static EFI_PEI_EVENT_LOG_PPI mBiosStoragePpi = {
  PeiWriteEvent
};

static EFI_PEI_PPI_DESCRIPTOR mPpiList = {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiBiosStorageGuid,
    &mBiosStoragePpi
};

/**
  Install the PEIM.  Initialize listeners, publish the PPI and HOB for PEI and
  DXE use respectively.

  @param [in] FfsHeader            Points to the FFS file header to be checked.
  @param [in] PeiServices          General purpose services available to every PEIM.

  @retval EFI Status            
**/
EFI_STATUS
EFIAPI
PeiBiosStorageInit (
  IN EFI_PEI_FILE_HANDLE             FfsHeader,
  IN CONST EFI_PEI_SERVICES          **PeiServices
  )
{
  EFI_STATUS            Status;

  Status = (**PeiServices).InstallPpi (PeiServices, &mPpiList);

  return Status;
}

/**
  Write event to BIOS Storage by creating HOB.

  @param [in] PeiServices          General purpose services available to every PEIM.
  @param [in] This                 This PPI interface.
  @param [in] BiosStorageType      Event log type.
  @param [in] PostBitmap1          Post bitmap 1 which will be stored in data area of POST error type log.
  @param [in] PostBitmap2          Post bitmap 2 which will be stored in data area of POST error type log.

  @retval EFI Status            
**/
EFI_STATUS
EFIAPI
PeiWriteEvent (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN  EFI_PEI_EVENT_LOG_PPI             *This,
  IN  UINT8                             BiosStorageType,
  IN BIOS_EVENT_LOG_POST_ERROR_BITMAP_FIRST_DWORD PostBitmap1,
  IN BIOS_EVENT_LOG_POST_ERROR_BITMAP_FIRST_DWORD PostBitmap2
  )
{
  EFI_STATUS                          Status=EFI_SUCCESS;
  BIOS_EVENT_LOG_ORGANIZATION         Log;
  UINT32                              NumBytes;
  UINTN                               DataFormatTypeLength;
  VOID                                *Hob;

  DataFormatTypeLength = 0;
  switch (BiosStorageType) {
  case EfiEventLogTypePostError:
    DataFormatTypeLength = BIOS_EVENT_LOG_DATA_FORMAT_POST_RESULT_BITMAP_LENGTH;
    break;

  default:
    DataFormatTypeLength = BIOS_EVENT_LOG_DATA_FORMAT_NONE_LENGTH;
    break;
  }
  NumBytes = BIOS_EVENT_LOG_BASE_LENGTH + DataFormatTypeLength;

  Log.Year   = ReadCmos8(0x09);
  Log.Month  = ReadCmos8(0x08);
  Log.Day    = ReadCmos8(0x07);
  Log.Hour   = ReadCmos8(0x04);
  Log.Minute = ReadCmos8(0x02);
  Log.Second = ReadCmos8(0x00);

  Log.Type   = BiosStorageType;
  Log.Length = (UINT8)NumBytes;

  if (BiosStorageType == EfiEventLogTypePostError) {
    *(UINT32*)(Log.Data) = PostBitmap1;
    *(UINT32*)(&Log.Data[4]) = PostBitmap2;
  }

  Hob = BuildGuidDataHob (
             &gBiosEventLogHobGuid,
             &Log,
             NumBytes
             );
  if (Hob == NULL) {
    Status = EFI_UNSUPPORTED;
  }

  return Status;
}
