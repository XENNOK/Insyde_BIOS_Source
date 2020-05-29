//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "PeiEventLog.h"


static EFI_PEI_EVENT_LOG_PPI mEventLogPpi = {
  PeiWriteEvent
};

static EFI_PEI_PPI_DESCRIPTOR mPpiList = {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiEventLogGuid,
    &mEventLogPpi
};


//
// Define PEIM entry point
//
EFI_PEIM_ENTRY_POINT (PeiEventLogInit)

EFI_STATUS
EFIAPI
PeiEventLogInit (
  IN EFI_FFS_FILE_HEADER       *FfsHeader,
  IN EFI_PEI_SERVICES          **PeiServices
  )
/*++

Routine Description:

  Install the PEIM.  Initialize listeners, publish the PPI and HOB for PEI and
  DXE use respectively.

Arguments:

  FfsHeader   - FV this PEIM was loaded from.
  PeiServices - General purpose services available to every PEIM.

Returns:

  EFI_SUCCESS   The function always returns success.

--*/
{
  EFI_STATUS            Status;

  Status = (**PeiServices).InstallPpi (PeiServices, &mPpiList);

  return Status;
}

EFI_STATUS
PeiWriteEvent (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN  EFI_PEI_EVENT_LOG_PPI             *This,
  IN  UINT8                             EventLogType,
  IN EVENT_LOG_POST_ERROR_BITMAP_FIRST_DWORD PostBitmap1,
  IN EVENT_LOG_POST_ERROR_BITMAP_FIRST_DWORD PostBitmap2
  )
{
  EFI_STATUS                          Status;
  EVENT_LOG_ORGANIZATION              Log;
  UINT32                              NumBytes;
  UINTN                               DataFormatTypeLength;

  DataFormatTypeLength = 0;
  switch (EventLogType) {
  case EfiEventLogTypePostError:
    DataFormatTypeLength = EVENT_LOG_DATA_FORMAT_POST_RESULT_BITMAP_LENGTH;
    break;

  default:
    DataFormatTypeLength = EVENT_LOG_DATA_FORMAT_NONE_LENGTH;
    break;
  }
  NumBytes = EVENT_LOG_BASE_LENGTH + DataFormatTypeLength;

  Log.Year   = EfiReadCmos8(0x09);
  Log.Month  = EfiReadCmos8(0x08);
  Log.Day    = EfiReadCmos8(0x07);
  Log.Hour   = EfiReadCmos8(0x04);
  Log.Minute = EfiReadCmos8(0x02);
  Log.Second = EfiReadCmos8(0x00);

  Log.Type   = EventLogType;
  Log.Length = (UINT8)NumBytes;

  if (EventLogType == EfiEventLogTypePostError) {
    *(UINT32*)(Log.Data) = PostBitmap1;
    *(UINT32*)(&Log.Data[4]) = PostBitmap2;
  }

  Status = PeiBuildHobGuidData (
             PeiServices,
             &gEventLogHobGuid,
             &Log,
             NumBytes
             );

  return Status;
}
