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
  EFI_HII_PROTOCOL                      *Hii;

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

  //
  // There should only be one HII protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiHiiProtocolGuid,
                  NULL,
                  &Hii
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
        GetBootStringFromToken(Hii, &Record->ProducerName, MiscBiosVendor->BiosReleaseDate, &TmpStr);
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
        GetBootStringFromToken(Hii, &Record->ProducerName, *ProcessorVersion, &TmpStr);

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

EFI_STATUS
GetBootStringFromToken (
  IN  EFI_HII_PROTOCOL                  *Hii,
  IN  EFI_GUID                          *ProducerGuid,
  IN  STRING_REF                        Token,
  OUT CHAR16                            **String
  )
/*++

Routine Description:

Arguments:

  ProducerGuid - The Guid to search the HII database for
  Token - The token value of the string to extract
  String - The string that is extracted

Returns:

--*/
{
  EFI_STATUS        Status;
  UINT16            HandleBufferLength;
  EFI_HII_HANDLE    *HiiHandleBuffer;
  UINT16            StringBufferLength;
  UINTN             NumberOfHiiHandles;
  UINTN             Index;
  UINT16            Length;
  EFI_GUID          HiiGuid;

  HandleBufferLength = 0x1000;
  HiiHandleBuffer = NULL;

  //
  // Get all the Hii handles
  //
  HiiHandleBuffer = EfiLibAllocateZeroPool (HandleBufferLength);

  Status = Hii->FindHandles (Hii, &HandleBufferLength, HiiHandleBuffer);
  ASSERT_EFI_ERROR(Status);

  //
  // Get the Hii Handle that matches the StructureNode->ProducerName
  //
  NumberOfHiiHandles = HandleBufferLength / sizeof (EFI_HII_HANDLE);
  for (Index = 0; Index < NumberOfHiiHandles; Index ++) {
    Length = 0;
    Status = ExtractDataFromHiiHandle (
               HiiHandleBuffer[Index],
               &Length,
               NULL,
               &HiiGuid
               );
    if (EfiCompareGuid (ProducerGuid, &HiiGuid)) {
      break;
    }
  }

  //
  // Find the string based on the current language
  //
  StringBufferLength = 0x100;
  *String = EfiLibAllocateZeroPool (0x100);
  Status = Hii->GetString (
                  Hii,
                  HiiHandleBuffer[Index],
                  Token,
                  FALSE,
                  NULL,
                  (VOID *)(UINTN)&StringBufferLength,
                  *String
                  );

  if (EFI_ERROR(Status)) {
    gBS->FreePool(*String);
    *String = L" ";
  }

  gBS->FreePool (HiiHandleBuffer);

  return EFI_SUCCESS;
}





