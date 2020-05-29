/** @file
  System informaiton relative code

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "SystemInformation.h"
#include <IndustryStandard/SmBios.h>
#include <Protocol/Smbios.h>
#include <Library/MultiConfigBaseLib.h>
#include <Library/VariableSupportLib.h>

CHAR16                      mMemFormFactorStr[][20] = {L"Other",
                                                       L"Unknown",
                                                       L"SIMM",
                                                       L"SIP",
                                                       L"Chip",
                                                       L"DIP",
                                                       L"ZIP",
                                                       L"Proprietary Card",
                                                       L"DIMM",
                                                       L"TSOP",
                                                       L"Row of chips",
                                                       L"RIMM",
                                                       L"SODIMM",
                                                       L"SRIMM",
                                                       L"FB-DIMM"
                                                       };

/**
  Acquire the string associated with the Index from smbios structure and return it.
  The caller is responsible for free the string buffer.

  @param    OptionalStrStart  The start position to search the string
  @param    Index             The index of the string to extract
  @param    String            The string that is extracted

  @retval   EFI_SUCCESS            Get index string successfully.
  @retval   EFI_INVALID_PARAMETER  Index is zero. It is invalid value.
  @retval   EFI_ABORTED            Get missing string fail .
  @retval   EFI_OUT_OF_RESOURCES   Allocate memory fail.
  @retval   Other                  Get setup browser data fail.

**/
EFI_STATUS
GetOptionalStringByIndex (
  IN  CHAR8                        *OptionalStrStart,
  IN  UINT8                        Index,
  OUT CHAR16                       **String
  )
{
  UINTN                                 StrSize;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  EFI_STATUS                            Status;

  if (Index == 0) {
    return EFI_INVALID_PARAMETER;
  }

  StrSize = 0;
  do {
    Index--;
    OptionalStrStart += StrSize;
    StrSize           = AsciiStrSize (OptionalStrStart);
  } while (OptionalStrStart[StrSize] != 0 && Index != 0);

  if ((Index != 0) || (StrSize == 1)) {
    //
    // Meet the end of strings set but Index is non-zero, or
    // Find an empty string
    //
    Status = GetSetupUtilityBrowserData (&SuBrowser);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    SUCInfo = SuBrowser->SUCInfo;
    *String = HiiGetString (SUCInfo->MapTable[MainHiiHandle].HiiHandle, STRING_TOKEN (STR_MISSING_STRING), NULL);
     if (*String == NULL) {
       return EFI_ABORTED;
     }
  } else {
    *String = AllocatePool (StrSize * sizeof (CHAR16));
     if (*String == NULL) {
       return EFI_OUT_OF_RESOURCES;
     }
    AsciiStrToUnicodeStr (OptionalStrStart, *String);
  }

  return EFI_SUCCESS;
}

/**
  Convert Cache Size to a string

  @param SmbiosType7             Pointer to smbios table type 7
  @param String                  The string that is created

  @retval EFI_SUCCESS            convert cache size value to string successful.

**/
STATIC
EFI_STATUS
ConvertCacheSizeToString (
  IN  SMBIOS_TABLE_TYPE7                *SmbiosType7,
  OUT CHAR16                            **String
  )
{
  CHAR16        *StringBuffer;

  StringBuffer = AllocateZeroPool (0x20);
  if (StringBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (SmbiosType7->InstalledSize & 0x8000) {
    UnicodeValueToString (StringBuffer, 0, ((SmbiosType7->InstalledSize & ~0x8000) >> 4), 0);
    StrCat (StringBuffer, L" MB");
  } else {
    UnicodeValueToString (StringBuffer, 0, SmbiosType7->InstalledSize, 0);
    StrCat (StringBuffer, L" KB");
  }

  *String = StringBuffer;

  return EFI_SUCCESS;
}


/**
  Convert Processor Frequency Data to a string

  @param  HiiHandle              Input Hii handle
  @param  NumSlot                Number of memory slots
  @param  RamSlotInfo            Pointer to the start of Ram slot information arrary

  @retval EFI_SUCCESS            convert value to string successful.

**/
STATIC
EFI_STATUS
CreateRamSlotInfo (
  IN  EFI_HII_HANDLE                        HiiHandle,
  IN  UINTN                                 NumSlot,
  IN  UPDATE_INFO_STRUCT                    *RamSlotInfo
  )
{
  VOID                                  *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL                    *StartLabel;
  UINTN                                 Index;
  EFI_STATUS                            Status;

  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = UPDATE_INFO_RAM_SLOT_LABEL;

  //
  //Channel A
  //
  HiiCreateTextOpCode (
    StartOpCodeHandle,
    STRING_TOKEN(STR_CHANNELA_STRING),
    0,
    STRING_TOKEN(STR_BLANK_STRING)
    );
  for (Index = 0; Index < NumSlot / 2; Index++) {
    HiiCreateTextOpCode (
      StartOpCodeHandle,
      (EFI_STRING_ID) RamSlotInfo[Index].TokenToUpdate1,
      0,
      (EFI_STRING_ID) RamSlotInfo[Index].TokenToUpdate2
      );
  }
  //
  //Channel B
  //
  HiiCreateTextOpCode (
    StartOpCodeHandle,
    STRING_TOKEN(STR_CHANNELB_STRING),
    0,
    STRING_TOKEN(STR_BLANK_STRING)
    );
  for (Index = NumSlot / 2; Index < NumSlot; Index++) {
    HiiCreateTextOpCode (
      StartOpCodeHandle,
      (EFI_STRING_ID) RamSlotInfo[Index].TokenToUpdate1,
      0,
      (EFI_STRING_ID) RamSlotInfo[Index].TokenToUpdate2
      );
  }
  //
  // Add Text op-code
  //
  Status = HiiUpdateForm (
             HiiHandle,
             NULL,
             ROOT_FORM_ID,
             StartOpCodeHandle,
             NULL
             );

  HiiFreeOpCodeHandle (StartOpCodeHandle);

  return EFI_SUCCESS;
}

/**
  Update CPU Type information in VFR

  @param  HiiHandle              Input Hii handle

  @retval EFI_SUCCESS            update successfully.

**/
STATIC
EFI_STATUS
CreateCpuTypeInfo (
  IN  EFI_HII_HANDLE                        HiiHandle
  )
{
  VOID                                  *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL                    *StartLabel;
  EFI_STATUS                            Status;

  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = UPDATE_CPU_TYPE_LABEL;

  HiiCreateTextOpCode (
    StartOpCodeHandle,
    STRING_TOKEN(STR_CPU_TYPE_STRING),
    0,
    STRING_TOKEN(STR_CPU_TYPE_STRING2)
    );

  Status = HiiUpdateForm (
             HiiHandle,
             NULL,
             ROOT_FORM_ID,
             StartOpCodeHandle,
             NULL
             );

  HiiFreeOpCodeHandle (StartOpCodeHandle);

  return EFI_SUCCESS;
}

/**
  Update System Bus Speed information in VFR

  @param  HiiHandle              Input Hii handle

  @retval EFI_SUCCESS            update successfully.

**/
STATIC
EFI_STATUS
CreateSystemBusSpeedInfo (
  IN  EFI_HII_HANDLE                        HiiHandle
  )
{
  VOID                                  *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL                    *StartLabel;
  EFI_STATUS                            Status;

  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = UPDATE_SYSTEM_BUS_SPEED_LABEL;

  HiiCreateTextOpCode (
    StartOpCodeHandle,
    STRING_TOKEN(STR_SYSTEM_BUS_SPEED_STRING),
    0,
    STRING_TOKEN(STR_SYSTEM_BUS_SPEED_STRING2)
    );

  Status = HiiUpdateForm (
             HiiHandle,
             NULL,
             ROOT_FORM_ID,
             StartOpCodeHandle,
             NULL
             );

  HiiFreeOpCodeHandle (StartOpCodeHandle);

  return EFI_SUCCESS;
}

/**
  Update Cache RAM information in VFR

  @param  HiiHandle              Input Hii handle

  @retval EFI_SUCCESS            update successfully.

**/
STATIC
EFI_STATUS
CreateCacheRamInfo (
  IN  EFI_HII_HANDLE                        HiiHandle
  )
{
  VOID                                  *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL                    *StartLabel;
  EFI_STATUS                            Status;

  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = UPDATE_CACHE_RAM_LABEL;

  HiiCreateTextOpCode (
    StartOpCodeHandle,
    STRING_TOKEN(STR_CACHE_RAM_STRING),
    0,
    STRING_TOKEN(STR_CACHE_RAM_STRING2)
    );

  Status = HiiUpdateForm (
             HiiHandle,
             NULL,
             ROOT_FORM_ID,
             StartOpCodeHandle,
             NULL
             );

  HiiFreeOpCodeHandle (StartOpCodeHandle);

  return EFI_SUCCESS;
}

/**
  Update Config Data ID in VFR

  @param  HiiHandle              Input Hii handle

  @retval EFI_SUCCESS            update successfully.

**/
STATIC
EFI_STATUS
CreateConfigIdInfo (
  IN  EFI_HII_HANDLE                        HiiHandle
  )
{
  VOID                                  *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL                    *StartLabel;
  EFI_STATUS                            Status;

  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = UPDATE_CONFIG_ID_LABEL;

  HiiCreateTextOpCode (
    StartOpCodeHandle,
    STRING_TOKEN(STR_CONFIG_DATA_ID_STRING),
    0,
    STRING_TOKEN(STR_CONFIG_DATA_ID_STRING2)
    );

  Status = HiiUpdateForm (
             HiiHandle,
             NULL,
             ROOT_FORM_ID,
             StartOpCodeHandle,
             NULL
             );

  HiiFreeOpCodeHandle (StartOpCodeHandle);

  return EFI_SUCCESS;
}

/**
  Update Config Data Name in VFR

  @param  HiiHandle              Input Hii handle

  @retval EFI_SUCCESS            update successfully.

**/
STATIC
EFI_STATUS
CreateConfigNameInfo (
  IN  EFI_HII_HANDLE                        HiiHandle
  )
{
  VOID                                  *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL                    *StartLabel;
  EFI_STATUS                            Status;

  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = UPDATE_CONFIG_NAME_LABEL;

  HiiCreateTextOpCode (
    StartOpCodeHandle,
    STRING_TOKEN(STR_CONFIG_DATA_NAME_STRING),
    0,
    STRING_TOKEN(STR_CONFIG_DATA_NAME_STRING2)
    );

  Status = HiiUpdateForm (
             HiiHandle,
             NULL,
             ROOT_FORM_ID,
             StartOpCodeHandle,
             NULL
             );

  HiiFreeOpCodeHandle (StartOpCodeHandle);

  return EFI_SUCCESS;
}

/**
  Update System Infomation, ie CPU Type & Speed, Bus Speed, System Memory Speed, cache, and RAM.

  @param  HiiHandle              Input Hii handle

  @retval EFI_SUCCESS            System info update successful.

**/
EFI_STATUS
UpdateSystemInfo (
  IN  EFI_HII_HANDLE                      HiiHandle
  )
{
  EFI_SMBIOS_TABLE_HEADER     *Record;
  EFI_SMBIOS_HANDLE           SmbiosHandle;
  EFI_SMBIOS_PROTOCOL         *Smbios;
  EFI_STATUS                  Status;
  UINT8                       StrIndex;
  CHAR16                      *NewString;
  CHAR16                      StrBuffer[40];
  EFI_STRING_ID               TokenToUpdate;
  UINT64                      MemorySize;
  UINT16                      CacheLevel;
  SMBIOS_TABLE_TYPE4          *Type4Record;
  SMBIOS_TABLE_TYPE7          *Type7Record;
  SMBIOS_TABLE_TYPE17         *Type17Record;
  SMBIOS_TABLE_TYPE19         *Type19Record;
  UINTN                       Index;
  UINTN                       NumSlot;
  UINTN                       SlotIndex;
  UINT8                       SlotNumChar;
  UINT8                       ChannelBase;
  UPDATE_INFO_STRUCT          UpdateRamSlotInfo[] = {
                                {1, STRING_TOKEN(STR_CHA_DIMM0_STRING), STRING_TOKEN(STR_CHA_DIMM0_STRING2)},
                                {2, STRING_TOKEN(STR_CHA_DIMM1_STRING), STRING_TOKEN(STR_CHA_DIMM1_STRING2)},
                                {3, STRING_TOKEN(STR_CHB_DIMM0_STRING), STRING_TOKEN(STR_CHB_DIMM0_STRING2)},
                                {4, STRING_TOKEN(STR_CHB_DIMM1_STRING), STRING_TOKEN(STR_CHB_DIMM1_STRING2)}
                                };

  NumSlot = sizeof (UpdateRamSlotInfo) /sizeof (UPDATE_INFO_STRUCT);
  CreateRamSlotInfo (
    HiiHandle,
    NumSlot,
    UpdateRamSlotInfo
    );

  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **) &Smbios
                  );
  ASSERT_EFI_ERROR (Status);

  Index = 0;
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  do {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, NULL, &Record, NULL);
    if (EFI_ERROR(Status)) {
      break;
    }

    if (Record->Type == EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION) {
      Type4Record = (SMBIOS_TABLE_TYPE4 *) Record;

      StrIndex = Type4Record->ProcessorVersion;
      Status = GetOptionalStringByIndex ((CHAR8*)((UINT8*)Type4Record + Type4Record->Hdr.Length), StrIndex, &NewString);
      if (!EFI_ERROR (Status)) {
        TokenToUpdate = STRING_TOKEN (STR_CPU_TYPE_STRING2);
        HiiSetString (HiiHandle, TokenToUpdate, NewString, NULL);
        gBS->FreePool (NewString);
      } else {
        Status = EFI_SUCCESS;
      }
      ZeroMem (StrBuffer, sizeof (StrBuffer));
      UnicodeValueToString (StrBuffer, 0, (INT64) Type4Record->ExternalClock, 0);
      StrCat (StrBuffer, L" MHz");
      TokenToUpdate = STRING_TOKEN (STR_SYSTEM_BUS_SPEED_STRING2);
      HiiSetString (HiiHandle, TokenToUpdate, StrBuffer, NULL);
    }

    if (Record->Type == EFI_SMBIOS_TYPE_MEMORY_DEVICE) {
      Type17Record = (SMBIOS_TABLE_TYPE17 *) Record;

      if (Type17Record->ConfiguredMemoryClockSpeed != 0) {
        ZeroMem (StrBuffer, sizeof (StrBuffer));
        UnicodeValueToString (StrBuffer, 0, (INT64) Type17Record->ConfiguredMemoryClockSpeed, 0);
        StrCat (StrBuffer, L" MHz");
        TokenToUpdate = STRING_TOKEN (STR_SYSTEM_MEMORY_SPEED_STRING2);
        HiiSetString (HiiHandle, TokenToUpdate, StrBuffer, NULL);
       }

      //
      // Update memory device slot string
      //
      SlotIndex = Index;
      if (Type17Record->FormFactor >= MemoryFormFactorOther &&
          Type17Record->FormFactor <= MemoryFormFactorFbDimm) {
        StrIndex = Type17Record->DeviceLocator;
        Status = GetOptionalStringByIndex ((CHAR8*) ((UINT8*) Type17Record + Type17Record->Hdr.Length), StrIndex, &NewString);
        if (Status == EFI_SUCCESS) {
          if (!StrnCmp (NewString, L"Channel", StrLen (L"Channel"))) {
            ChannelBase = (UINT8) ((NewString[StrLen (L"Channel")] - 'A') * 2);
          } else {
            ChannelBase = 0;
          }

          SlotNumChar = *(UINT8 *) (NewString + StrLen (NewString) - 1);
          SlotNumChar += ChannelBase;
          if (SlotNumChar < '0' || SlotNumChar >= NumSlot + '0') {
            SlotIndex = Index;
          } else {
            SlotIndex = SlotNumChar - '0';
          }

          gBS->FreePool (NewString);
        }

        ZeroMem (StrBuffer, sizeof (StrBuffer));
        StrCat (StrBuffer, mMemFormFactorStr[Type17Record->FormFactor - 1]);
        if (SlotIndex % 2 == 0) {
          StrCat (StrBuffer, L" 0");
        } else {
          StrCat (StrBuffer, L" 1");
        }
        TokenToUpdate = (STRING_REF) (UpdateRamSlotInfo[SlotIndex].TokenToUpdate1);
        HiiSetString (HiiHandle, TokenToUpdate, StrBuffer, NULL);
      }

      if (Type17Record->Size != 0) {
        ZeroMem (StrBuffer, sizeof (StrBuffer));
        if (Type17Record->Size == 0xFFFF) {
          StrCat (StrBuffer, L"Unknown");
        } else if (Type17Record->Size == 0x7FFFF) {
          UnicodeValueToString (StrBuffer, 0, (UINT64)(Type17Record->ExtendedSize & ~BIT31), 0);
          StrCat (StrBuffer, L" MB");
        } else {
          UnicodeValueToString (StrBuffer, 0, (UINT64)(Type17Record->Size & ~BIT15), 0);
          if (Type17Record->Size & BIT15) {
            StrCat (StrBuffer, L" KB");
          } else {
            StrCat (StrBuffer, L" MB");
          }
        }
        TokenToUpdate = (STRING_REF) (UpdateRamSlotInfo[SlotIndex].TokenToUpdate2);
        HiiSetString (HiiHandle, TokenToUpdate, StrBuffer, NULL);
      }
      Index++;
    }


    if (Record->Type == EFI_SMBIOS_TYPE_CACHE_INFORMATION) {
      Type7Record = (SMBIOS_TABLE_TYPE7 *) Record;
      CacheLevel = (UINT16) (Type7Record->CacheConfiguration & 0x7) + 1;
      if (CacheLevel == EFI_CACHE_L2) {
        ConvertCacheSizeToString (Type7Record, &NewString);
        TokenToUpdate = STRING_TOKEN (STR_CACHE_RAM_STRING2);
        HiiSetString (HiiHandle, TokenToUpdate, NewString, NULL);
        gBS->FreePool (NewString);
      }
    }

    if (Record->Type == EFI_SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS) {
      Type19Record = (SMBIOS_TABLE_TYPE19 *) Record;
      ZeroMem (StrBuffer, sizeof (StrBuffer));

      if (Type19Record->StartingAddress == 0xFFFFFFFF && Type19Record->EndingAddress == 0xFFFFFFFF) {
        MemorySize = (RShiftU64((Type19Record->ExtendedEndingAddress - Type19Record->ExtendedStartingAddress), 30) + 1);
        UnicodeValueToString (StrBuffer, 0, MemorySize, 0);
        StrCat (StrBuffer, L" GB");
      } else {
        MemorySize = (RShiftU64((Type19Record->EndingAddress - Type19Record->StartingAddress), 10) + 1);
        UnicodeValueToString (StrBuffer, 0, MemorySize, 0);
        StrCat (StrBuffer, L" MB");
      }

      TokenToUpdate = STRING_TOKEN (STR_TOTAL_MEMORY_STRING2);
      HiiSetString (HiiHandle, TokenToUpdate, StrBuffer, NULL);
    }
  } while(Status == EFI_SUCCESS);

  if (FeaturePcdGet (PcdMultiConfigSupported)) {
    UINT16                      Index2;
    UINT16                      ConfigCount;
    VOID                        *NamePtr;
    UINTN                       MultiConfigDataSize;
    VOID                        *MultiConfigData;
    UINT8                       ConfigId;
    CONFIG_HEADER               *TempHeader;
    CHAR16                      *TempStrBuffer;
    BOOLEAN                     SettingModify;
    VOID                        *SetupData;
    UINTN                       DataSize;
    UINTN                       NameSize;

    TempStrBuffer = NULL;
    TempHeader = NULL;
    SettingModify = FALSE;
    Index2 = 0;
    ConfigId = 0;
    MultiConfigDataSize = 0;
    ConfigCount = GetConfigCount();
    ZeroMem (StrBuffer, sizeof (StrBuffer));

    CommonGetVariableDataAndSize (L"Setup", &gSystemConfigurationGuid, &DataSize, &SetupData);
    if (SetupData == NULL) {
      return EFI_NOT_FOUND;
    }

    for ( ; Index2 < ConfigCount; Index2 ++) {
      //
      // Get Active (Attribute: ACTIVE & LOAD_DEFAULT) Config Data from Multi Config Region
      //
      Status = GetFullSetupSetting (
                SETUP_FOR_BIOS_POST,
                &Index2,
                MultiConfigDataSize,
                &MultiConfigData,
                &ConfigId,
                NULL
                 );
      if (!EFI_ERROR (Status)) {
        NamePtr = (VOID *) GET_VARIABLE_NAME_PTR (MultiConfigData);
        if (CompareMem (L"Setup", NamePtr, NameSizeOfVariable (MultiConfigData)) == 0) {
          if (CompareMem (SetupData, GetVariableDataPtr (MultiConfigData), DataSize) != 0) {
            SettingModify = TRUE;
          }
          break;
        }
      }
    }

    if (Status == EFI_NOT_FOUND) {
      Index2 = 0;
      for ( ; Index2 < ConfigCount; Index2 ++) {
        //
        // Get Active (Attribute: ACTIVE & LOAD_DEFAULT) Config Data from Multi Config Region
        //
        Status = GetFullSetupSetting (
                  SETUP_FOR_LOAD_DEFAULT,
                  &Index2,
                  MultiConfigDataSize,
                  &MultiConfigData,
                  &ConfigId,
                  NULL
                   );
        if (!EFI_ERROR (Status)) {
          NamePtr = (VOID *) GET_VARIABLE_NAME_PTR (MultiConfigData);
          if (CompareMem (L"Setup", NamePtr, NameSizeOfVariable (MultiConfigData)) == 0) {
            if (CompareMem (SetupData, GetVariableDataPtr (MultiConfigData), DataSize) != 0) {
              SettingModify = TRUE;
            }
            break;
          }
        }
      }
    }

    if (!EFI_ERROR (Status)) {
      //
      // Display Config ID and Setting is modified or not
      //
      UnicodeValueToString (StrBuffer, PREFIX_ZERO | RADIX_HEX, (INT64) ConfigId, 2);
      StrCat (StrBuffer, L"h");
      if (SettingModify) {
        StrCat (StrBuffer, L" (Setting Modified)");
      }
      TokenToUpdate = STRING_TOKEN (STR_CONFIG_DATA_ID_STRING2);
      HiiSetString (HiiHandle, TokenToUpdate, StrBuffer, NULL);

      CreateConfigIdInfo (HiiHandle);

      //
      //  Display Config Name
      //
      ZeroMem (StrBuffer, sizeof (StrBuffer));

      TempHeader = GetConfigHeaderByOrder (Index2);
      NameSize = AsciiStrSize (TempHeader->Name);
      if (NameSize > 1) {
        //
        // Config Name is not NULL
        //
        TempStrBuffer = (CHAR16 *)AllocateZeroPool (NameSize * sizeof (CHAR16));
        if (TempStrBuffer == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
        AsciiStrToUnicodeStr (TempHeader->Name, TempStrBuffer);
        StrCat (StrBuffer, TempStrBuffer);
        FreePool (TempStrBuffer);
      }

      TokenToUpdate = STRING_TOKEN (STR_CONFIG_DATA_NAME_STRING2);
      HiiSetString (HiiHandle, TokenToUpdate, StrBuffer, NULL);

      CreateConfigNameInfo (HiiHandle);
    }
  }
  CreateCpuTypeInfo (HiiHandle);
  CreateSystemBusSpeedInfo (HiiHandle);
  CreateCacheRamInfo (HiiHandle);

  return  EFI_SUCCESS;
}
