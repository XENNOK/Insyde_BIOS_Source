//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "BootSystemInformation.h"
#include "OemDataHubSubClassMemory.h"


EFI_STATUS
ShowSystemInfo (
    IN OUT UINT8                        *PrintLine
  )
/*++

Routine Description:

  Show System Infomation, ie BIOS Version, BIOS Release Date,
  CPU Type, and Memory Speed.

Arguments:

Returns:


--*/
{

  EFI_STATUS                            Status;
  CHAR16                                *TmpStr;
  CHAR16                                *CpuTypeStr;
  CHAR16                                *BiosVersionStr;
  CHAR16                                *MiscBiosVersion;
  UINT64                                MonotonicCount;
  CHAR16                                *MemSpeedStr;
  CHAR16                                *BiosReleaseDateStr;
  BOOLEAN                               PrinteMemBusSpeed;
  EFI_DATA_HUB_PROTOCOL                 *DataHub;
  EFI_DATA_RECORD_HEADER                *Record;
  EFI_SUBCLASS_TYPE1_HEADER             *DataHeader;
  EFI_MISC_BIOS_VENDOR                  *MiscBiosVendor;
  EFI_PROCESSOR_VERSION_DATA            *ProcessorVersion;
  EFI_MEMORY_ARRAY_LINK_PLUS            *MemoryArrayLink;

  //
  // Update Front Page strings
  //
  Status = gBS->LocateProtocol (
                  &gEfiDataHubProtocolGuid,
                  NULL,
                  &DataHub
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  MonotonicCount = 0;
  Record = NULL;
  PrinteMemBusSpeed = FALSE;

  gST->ConOut->EnableCursor (gST->ConOut, FALSE);
  gST->ConOut->SetAttribute (gST->ConOut, EFI_WHITE);

  //
  // BIOS Version
  //
  BiosVersionStr = (VOID *)(UINTN)GetStringById (STRING_TOKEN (STR_BIOS_VERSION_STRING));
  MiscBiosVersion = (VOID *)(UINTN)GetStringById (STRING_TOKEN (STR_MISC_BIOS_VERSION));
  BdsLibOutputStrings (gST->ConOut, L"\n\r", BiosVersionStr, L" : ", MiscBiosVersion, NULL);
  (*PrintLine)++;

  do {
    Status = DataHub->GetNextRecord (DataHub, &MonotonicCount, NULL, &Record);
    if (Record->DataRecordClass == EFI_DATA_RECORD_CLASS_DATA) {
      DataHeader  = (EFI_SUBCLASS_TYPE1_HEADER *) (Record + 1);
      //
      // BIOS Release Date
      //
      if (EfiCompareGuid(&Record->DataRecordGuid, &gMiscSubClassName) &&
          (DataHeader->RecordType == EFI_MISC_BIOS_VENDOR_RECORD_NUMBER)) {
        BiosReleaseDateStr = (VOID *)(UINTN)GetStringById (STRING_TOKEN(STR_BIOS_BUILD_DATE));
        MiscBiosVendor = (EFI_MISC_BIOS_VENDOR *)(DataHeader + 1);
        GetStringFromToken (&Record->ProducerName, MiscBiosVendor->BiosReleaseDate, &TmpStr);
        if (TmpStr != NULL) {
          BdsLibOutputStrings (gST->ConOut, L"\n\r", BiosReleaseDateStr, L" : ", TmpStr, NULL);
          (*PrintLine)++;
          gBS->FreePool(TmpStr);
        }
      }
      //
      // CPU Type
      //
      if (EfiCompareGuid(&Record->DataRecordGuid, &gProcessorSubClassName) &&
          (DataHeader->RecordType == ProcessorVersionRecordType)) {
        ProcessorVersion = (EFI_PROCESSOR_VERSION_DATA *)(DataHeader + 1);
        GetStringFromToken (&Record->ProducerName, *ProcessorVersion, &TmpStr);

        if (TmpStr != NULL) {
          CpuTypeStr = (VOID *)(UINTN)GetStringById (STRING_TOKEN (STR_CPU_TYPE_STRING));
          BdsLibOutputStrings (gST->ConOut, L"\n\n\r", CpuTypeStr, L" : ", TmpStr, NULL);
          (*PrintLine) += 2;
          gBS->FreePool(TmpStr);
        }
      }

      //
      // Memory Bus Speed
      //
      if (EfiCompareGuid(&Record->DataRecordGuid, &gEfiMemorySubClassGuid) &&
          (DataHeader->RecordType == EFI_MEMORY_ARRAY_LINK_PLUS_RECORD_NUMBER) &&
          (!PrinteMemBusSpeed)) {
        MemoryArrayLink = (EFI_MEMORY_ARRAY_LINK_PLUS *)(DataHeader + 1);
        if (DataHeader->SubInstance != 0 && MemoryArrayLink->ConfiguredMemoryClockSpeed != 0) {
          TmpStr = EfiLibAllocateZeroPool (0x100);
          //
          // need to have 4 digits to show 1067 MHz
          //
          EfiValueToString (TmpStr, MemoryArrayLink->ConfiguredMemoryClockSpeed, PREFIX_ZERO, 4);
          EfiStrCat (TmpStr, L" MHz");

          MemSpeedStr = (VOID *)(UINTN)GetStringById (STRING_TOKEN (STR_SYSTEM_MEMORY_SPEED_STRING));
          BdsLibOutputStrings (gST->ConOut, L"\n\n\r", MemSpeedStr, L" : ", TmpStr, NULL);
          (*PrintLine) += 2;
          PrinteMemBusSpeed = TRUE;
          gBS->FreePool(TmpStr);
        }
      }
    }
  } while (!EFI_ERROR(Status) && (MonotonicCount != 0));

  return  EFI_SUCCESS;
}







