//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   SystemInformation.c
//;
//; Abstract:
//;
//;   System informaiton relative code
//;

#include "SystemInformation.h"
#include "OemDataHubSubClassMemory.h"

STATIC
EFI_STATUS
ConvertMemorySizeToString (
  IN  UINT64          MemorySize,
  OUT CHAR16          **String
  )
/*++

Routine Description:

  Convert Memory Size to a string

Arguments:

  MemorySize - The size of the memory to process
  String     - The string that is created

Returns:

  EFI_SUCCESS  -  convert memory size value to string successful.

--*/
{
  CHAR16        *StringBuffer;

  StringBuffer = EfiLibAllocateZeroPool (0x20);
  EfiValueToString (StringBuffer, MemorySize, 0, 0);
  EfiStrCat (StringBuffer, L" MB");

  *String = (CHAR16 *) StringBuffer;

  return EFI_SUCCESS;
}


STATIC
EFI_STATUS
GetStringFromToken (
  IN  EFI_HII_PROTOCOL                  *Hii,
  IN  EFI_GUID                          *ProducerGuid,
  IN  STRING_REF                        Token,
  OUT CHAR16                            **String
  )
/*++

Routine Description:

  Acquire the string associated with the ProducerGuid and return it.

Arguments:

  Hii          - Pointer to EFI HII protocol instance
  ProducerGuid - The Guid to search the HII database for
  Token        - The token value of the string to extract
  String       - The string that is extracted

Returns:

  EFI_SUCCESS  - Get string successful

--*/
{
  EFI_STATUS        Status;
  UINT16            HandleBufferLength;
  EFI_HII_HANDLE    *HiiHandleBuffer;
  UINTN             StringBufferLength;
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
  ASSERT_EFI_ERROR (Status);

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
  StringBufferLength = 0x200;
  *String = EfiLibAllocateZeroPool (StringBufferLength);
  if (String == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Status = Hii->GetString (
                  Hii,
                  HiiHandleBuffer[Index],
                  Token,
                  FALSE,
                  NULL,
                  &StringBufferLength,
                  *String
                  );

  if (Status == EFI_BUFFER_TOO_SMALL) {
    gBS->FreePool (*String);
    *String = EfiLibAllocateZeroPool (StringBufferLength);
    if (*String == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    Status = Hii->GetString (
                    Hii,
                    HiiHandleBuffer[Index],
                    Token,
                    FALSE,
                    NULL,
                    &StringBufferLength,
                    *String
                    );
  }

  gBS->FreePool (HiiHandleBuffer);
  return Status;
}

STATIC
EFI_STATUS
ConvertCacheSizeToString (
  IN  EFI_CACHE_SIZE_DATA               *CacheSize,
  OUT CHAR16                            **String
  )
/*++

Routine Description:

  Convert Cache Size to a string

Arguments:

  CacheSize - The size of the Cache to process
  String    - The string that is created

Returns:

  EFI_SUCCESS  -  convert cache size value to string successful.
--*/
{
  CHAR16        *StringBuffer;

  StringBuffer = EfiLibAllocateZeroPool (0x20);

  if (CacheSize->Value & 0x8000) {
    EfiValueToString (StringBuffer, ((CacheSize->Value & ~0x8000) >> 4), 0, 0);
    EfiStrCat (StringBuffer, L" MB");
  } else {
    EfiValueToString (StringBuffer, CacheSize->Value, 0, 0);
    EfiStrCat (StringBuffer, L" KB");
  }

  *String = StringBuffer;

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
ConvertFsbFrequencyToString (
  IN  EFI_PROCESSOR_FSB_FREQUENCY_DATA  *ProcessorFsbFrequency,
  OUT CHAR16                            **String
  )
/*++

Routine Description:

  Convert Processor Frequency Data to a string

Arguments:

  ProcessorFrequency - The frequency data to process
  String - The string that is created

Returns:

  EFI_SUCCESS  -  convert value to string successful.

--*/
{
  CHAR16        *StringBuffer;
  UINTN         Index;
  UINT32        FreqMhz;

  if (ProcessorFsbFrequency->Exponent >= 6) {
    FreqMhz = ProcessorFsbFrequency->Value;
    for (Index = 0; Index < (UINTN) (ProcessorFsbFrequency->Exponent - 6); Index++) {
      FreqMhz *= 10;
    }
  } else {
    FreqMhz = 0;
  }

  StringBuffer = EfiLibAllocateZeroPool (0x20);
  Index = EfiValueToString (StringBuffer, FreqMhz, 0, 0);
  EfiStrCat (StringBuffer, L" MHz");

  *String = (CHAR16 *) StringBuffer;

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
CreateRamSlotInfo (
  IN  EFI_HII_PROTOCOL                      *Hii,
  IN  EFI_HII_HANDLE                        HiiHandle,
  IN  UINTN                                 NumSlot,
  IN  UPDATE_INFO_STRUCT                    *RamSlotInfo
  )
/*++

Routine Description:

  Convert Processor Frequency Data to a string

Arguments:

  Hii         -  Pointer to EFI Hii protocol instance.
  HiiHandle   -  Input Hii handle
  NumSlot     -  Number of memory slots
  RamSlotInfo -  Pointer to the start of Ram slot information arrary

Returns:

  EFI_SUCCESS  -  convert value to string successful.

--*/
{
  EFI_HII_UPDATE_DATA                   *UpdateData;
  UINT8                                 *Location;
  UINTN                                 Index;
  UINTN                                 SlotPerChannel;

  //
  // Allocate space for creation of Buffer
  //
  UpdateData = EfiLibAllocateZeroPool (0x1000);
  Location = (UINT8 *) &UpdateData->Data;

  UpdateData->DataCount = (UINT16) NumSlot + 2;
  //
  //Channel A
  //
  CreateTextOpCode(
    (STRING_REF) STRING_TOKEN (STR_CHANNELA_STRING),
    (STRING_REF) STRING_TOKEN (STR_BLANK_STRING),
    0,
    0,
    0,
    Location
    );
  Location += ((EFI_IFR_OP_HEADER *) Location)->Length;
  SlotPerChannel = NumSlot / 2;
  for (Index = 0; Index < SlotPerChannel; Index++) {
    CreateTextOpCode(
      (STRING_REF)RamSlotInfo[Index].TokenToUpdate1,
      (STRING_REF)RamSlotInfo[Index].TokenToUpdate2,
      0,
      0,
      0,
      Location
      );

    Location += ((EFI_IFR_OP_HEADER *) Location)->Length;
  }
  //
  //Channel B
  //
  CreateTextOpCode(
    (STRING_REF) STRING_TOKEN (STR_CHANNELB_STRING),
    (STRING_REF) STRING_TOKEN (STR_BLANK_STRING),
      0,
      0,
      0,
      Location
      );
  Location += ((EFI_IFR_OP_HEADER *) Location)->Length;
  for (Index = SlotPerChannel; Index < NumSlot; Index++) {
    CreateTextOpCode(
      (STRING_REF)RamSlotInfo[Index].TokenToUpdate1,
      (STRING_REF)RamSlotInfo[Index].TokenToUpdate2,
      0,
      0,
      0,
      Location
      );

    Location += ((EFI_IFR_OP_HEADER *) Location)->Length;
  }
  //
  // Add Text op-code
  //
  Hii->UpdateForm (
         Hii,
         HiiHandle,
         (EFI_FORM_LABEL) UPDATE_INFO_RAM_SLOT_LABEL,
         TRUE,
         UpdateData
         );

  gBS->FreePool (UpdateData);
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
CreateCpuTypeInfo (
  IN  EFI_HII_PROTOCOL                      *Hii,
  IN  EFI_HII_HANDLE                        HiiHandle
  )
/*++

Routine Description:

  Update CPU type information in VFR

Arguments:

  Hii         -  Pointer to EFI Hii protocol instance.
  HiiHandle   -  Input Hii handle

Returns:

  EFI_SUCCESS  -  update successfully.

--*/
{
  EFI_HII_UPDATE_DATA                   *UpdateData;
  UINT8                                 *Location;
  
  UpdateData = EfiLibAllocateZeroPool (0x1000);
  Location = (UINT8 *) &UpdateData->Data;

  UpdateData->DataCount =  1;
  
  CreateTextOpCode(
    (STRING_REF) STRING_TOKEN (STR_CPU_TYPE_STRING),
    (STRING_REF) STRING_TOKEN (STR_CPU_TYPE_STRING2),
    0,
    0,
    0,
    Location
    );
  
  Hii->UpdateForm (
         Hii,
         HiiHandle,
         (EFI_FORM_LABEL) UPDATE_CPU_TYPE_LABEL,
         TRUE,
         UpdateData
         );
  
  gBS->FreePool (UpdateData);
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
CreateSystemBusSpeedInfo (
  IN  EFI_HII_PROTOCOL                      *Hii,
  IN  EFI_HII_HANDLE                        HiiHandle
  )
/*++

Routine Description:

  Update system bus speed information in VFR

Arguments:

  Hii         -  Pointer to EFI Hii protocol instance.
  HiiHandle   -  Input Hii handle

Returns:

  EFI_SUCCESS  -  update successfully.

--*/
{
  EFI_HII_UPDATE_DATA                   *UpdateData;
  UINT8                                 *Location;
  
  UpdateData = EfiLibAllocateZeroPool (0x1000);
  Location = (UINT8 *) &UpdateData->Data;

  UpdateData->DataCount =  1;

  CreateTextOpCode(
    (STRING_REF) STRING_TOKEN (STR_SYSTEM_BUS_SPEED_STRING),
    (STRING_REF) STRING_TOKEN (STR_SYSTEM_BUS_SPEED_STRING2),
    0,
    0,
    0,
    Location
    );
  
  Hii->UpdateForm (
         Hii,
         HiiHandle,
         (EFI_FORM_LABEL) UPDATE_SYSTEM_BUS_SPEED_LABEL,
         TRUE,
         UpdateData
         );
  
  gBS->FreePool (UpdateData);
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
CreateCacheRamInfo (
  IN  EFI_HII_PROTOCOL                      *Hii,
  IN  EFI_HII_HANDLE                        HiiHandle
  )
/*++

Routine Description:

  Update Cache RAM information in VFR

Arguments:

  Hii         -  Pointer to EFI Hii protocol instance.
  HiiHandle   -  Input Hii handle

Returns:

  EFI_SUCCESS  -  update successfully.

--*/
{
  EFI_HII_UPDATE_DATA                   *UpdateData;
  UINT8                                 *Location;

  UpdateData = EfiLibAllocateZeroPool (0x1000);
  Location = (UINT8 *) &UpdateData->Data;

  UpdateData->DataCount = 1;

  CreateTextOpCode(
    (STRING_REF) STRING_TOKEN (STR_CACHE_RAM_STRING),
    (STRING_REF) STRING_TOKEN (STR_CACHE_RAM_STRING2),
    0,
    0,
    0,
    Location
    );

  Hii->UpdateForm (
         Hii,
         HiiHandle,
         (EFI_FORM_LABEL) UPDATE_CACHE_RAM_LABEL,
         TRUE,
         UpdateData
         );
  
  gBS->FreePool (UpdateData);  
  
  return EFI_SUCCESS;
}

EFI_STATUS
UpdateSystemInfo (
  IN  EFI_HII_PROTOCOL                    *Hii,
  IN  EFI_HII_HANDLE                      HiiHandle
  )
/*++

Routine Description:

  Update System Infomation, ie CPU Type & Speed, Bus Speed, System Memory Speed,
  cache, and RAM.
Arguments:

  Hii        -  Pointer to EFI Hii protocol instance.
  HiiHandle  -  Input Hii handle

Returns:

  EFI_SUCCESS  -  System info update successful.

--*/
{

  EFI_STATUS                            Status;
  STRING_REF                            TokenToUpdate;
  CHAR16                                *NewString;
  UINT64                                MonotonicCount;
  EFI_DATA_HUB_PROTOCOL                 *DataHub;
  EFI_DATA_RECORD_HEADER                *Record;
  EFI_SUBCLASS_TYPE1_HEADER             *DataHeader;
  EFI_PROCESSOR_VERSION_DATA            *ProcessorVersion;
  EFI_PROCESSOR_FSB_FREQUENCY_DATA      *ProcessorFsbFrequency;
  EFI_MEMORY_ARRAY_START_ADDRESS        *MemoryArray;
  EFI_MEMORY_DEVICE_START_ADDRESS       *MemoryDevice;
  EFI_MEMORY_ARRAY_LINK_PLUS            *MemoryArrayLink;
  EFI_CACHE_SIZE_DATA                   *CacheSizeData;
  EFI_GUID                              ProcessorSubClass = EFI_PROCESSOR_SUBCLASS_GUID;
  EFI_GUID                              MemorySubClass    = EFI_MEMORY_SUBCLASS_GUID;
  EFI_GUID                              CacheSubClass     = EFI_CACHE_SUBCLASS_GUID;
  UINTN                                 NumSlot;
  UINTN                                 Index;
  UPDATE_INFO_STRUCT                    UpdateRamSlotInfo[] = {
                                                {1, STRING_TOKEN (STR_CHA_DIMM0_STRING), STRING_TOKEN (STR_CHA_DIMM0_STRING2)},
                                                {2, STRING_TOKEN (STR_CHA_DIMM1_STRING), STRING_TOKEN (STR_CHA_DIMM1_STRING2)},
                                                {3, STRING_TOKEN (STR_CHB_DIMM0_STRING), STRING_TOKEN (STR_CHB_DIMM0_STRING2)},
                                                {4, STRING_TOKEN (STR_CHB_DIMM1_STRING), STRING_TOKEN (STR_CHB_DIMM1_STRING2)}
                                        };

  //
  // Update Front Page strings
  //
  Status = gBS->LocateProtocol (
                  &gEfiDataHubProtocolGuid,
                  NULL,
                  &DataHub
                  );

  MonotonicCount = 0;
  Record = NULL;
  NumSlot = sizeof (UpdateRamSlotInfo) / sizeof (UPDATE_INFO_STRUCT);
  CreateRamSlotInfo (
    Hii,
    HiiHandle,
    NumSlot,
    UpdateRamSlotInfo
    );

  do {
    Status = DataHub->GetNextRecord (DataHub, &MonotonicCount, NULL, &Record);
    if (Record->DataRecordClass == EFI_DATA_RECORD_CLASS_DATA) {
      DataHeader  = (EFI_SUBCLASS_TYPE1_HEADER *) (Record + 1);
      //
      // CPU Type
      //
      if (EfiCompareGuid (&Record->DataRecordGuid, &ProcessorSubClass) &&
          (DataHeader->RecordType == ProcessorVersionRecordType)) {
        ProcessorVersion = (EFI_PROCESSOR_VERSION_DATA *) (DataHeader + 1);
        GetStringFromToken (Hii, &Record->ProducerName, *ProcessorVersion, &NewString);
        TokenToUpdate = STRING_TOKEN (STR_CPU_TYPE_STRING2);
        Hii->NewString (Hii, NULL, HiiHandle, &TokenToUpdate, NewString);
        gBS->FreePool (NewString);
      }

      //
      // System Bus Speed
      //
      if (EfiCompareGuid (&Record->DataRecordGuid, &ProcessorSubClass) &&
          (DataHeader->RecordType == ProcessorFsbFrequencyRecordType)) {
        ProcessorFsbFrequency = (EFI_PROCESSOR_FSB_FREQUENCY_DATA *) (DataHeader + 1);
        ConvertFsbFrequencyToString (ProcessorFsbFrequency, &NewString);
        TokenToUpdate = STRING_TOKEN (STR_SYSTEM_BUS_SPEED_STRING2);
        Hii->NewString (Hii, NULL, HiiHandle, &TokenToUpdate, NewString);
        gBS->FreePool (NewString);
      }

      //
      // Memory Bus Speed
      //
      if (EfiCompareGuid (&Record->DataRecordGuid, &MemorySubClass) &&
          (DataHeader->RecordType == EFI_MEMORY_ARRAY_LINK_PLUS_RECORD_NUMBER)) {
        MemoryArrayLink = (EFI_MEMORY_ARRAY_LINK_PLUS *) (DataHeader + 1);
        if (DataHeader->SubInstance != 0 && MemoryArrayLink->ConfiguredMemoryClockSpeed != 0) {
          gBS->AllocatePool (EfiBootServicesData, 0x40, &NewString);
          EfiValueToString (NewString, MemoryArrayLink->ConfiguredMemoryClockSpeed, 0, 0);
          EfiStrCat (NewString, L" MHz");
          TokenToUpdate = STRING_TOKEN (STR_SYSTEM_MEMORY_SPEED_STRING2);
          Hii->NewString (Hii, NULL, HiiHandle, &TokenToUpdate, NewString);
          gBS->FreePool (NewString);
        }
      }

      //
      // Cache Size
      //
      if (EfiCompareGuid (&Record->DataRecordGuid, &CacheSubClass) &&
          (DataHeader->RecordType == CacheSizeRecordType) &&
          (DataHeader->SubInstance == EFI_CACHE_L2)) {
        CacheSizeData = (EFI_CACHE_SIZE_DATA *) (DataHeader + 1);
        ConvertCacheSizeToString (CacheSizeData, &NewString);
        TokenToUpdate = STRING_TOKEN (STR_CACHE_RAM_STRING2);
        Hii->NewString (Hii, NULL, HiiHandle, &TokenToUpdate, NewString);
        gBS->FreePool (NewString);
      }

      //
      // Total Memory Size
      //
      if (EfiCompareGuid (&Record->DataRecordGuid, &MemorySubClass) &&
          (DataHeader->RecordType == EFI_MEMORY_ARRAY_START_ADDRESS_RECORD_NUMBER)) {
        MemoryArray = (EFI_MEMORY_ARRAY_START_ADDRESS *) (DataHeader + 1);
        ConvertMemorySizeToString (
          (RShiftU64 ((MemoryArray->MemoryArrayEndAddress - MemoryArray->MemoryArrayStartAddress), 20) + 1),
          &NewString
          );
        TokenToUpdate = STRING_TOKEN (STR_TOTAL_MEMORY_STRING2);
        Hii->NewString (Hii, NULL, HiiHandle, &TokenToUpdate, NewString);
        gBS->FreePool (NewString);
      }

      //
      // Memory Bank size
      //
      if (EfiCompareGuid (&Record->DataRecordGuid, &MemorySubClass) &&
          (DataHeader->RecordType == EFI_MEMORY_DEVICE_START_ADDRESS_RECORD_NUMBER)) {
        MemoryDevice = (EFI_MEMORY_DEVICE_START_ADDRESS *) (DataHeader + 1);

        for (Index = 0; Index < NumSlot; Index++) {
          if (DataHeader->SubInstance == UpdateRamSlotInfo[Index].SubInstance) {
            ConvertMemorySizeToString (
              (RShiftU64 ((MemoryDevice->MemoryDeviceEndAddress - MemoryDevice->MemoryDeviceStartAddress), 20) + 1),
              &NewString
              );
            TokenToUpdate = (STRING_REF)UpdateRamSlotInfo[Index].TokenToUpdate2;
            Hii->NewString (Hii, NULL, HiiHandle, &TokenToUpdate, NewString);
            gBS->FreePool (NewString);
            break;
          }
        }
      }
    }
  } while (!EFI_ERROR(Status) && (MonotonicCount != 0));

  CreateCpuTypeInfo (
    Hii,
    HiiHandle
    );
  CreateSystemBusSpeedInfo (
    Hii,
    HiiHandle
    );
  CreateCacheRamInfo (
    Hii,
    HiiHandle
    );

  return  EFI_SUCCESS;
}
