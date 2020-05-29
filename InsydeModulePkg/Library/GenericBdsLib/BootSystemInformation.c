/** @file
  Boot System Information

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

#include "BootSystemInformation.h"
#include <Protocol/DynamicHotKey.h>

EFI_STATUS
BdsLibGetOptionalStringByIndex (
  IN      CHAR8                   *OptionalStrStart,
  IN      UINT8                   Index,
  OUT     CHAR16                  **String
  )
{
  UINTN          StrSize;

  if (Index == 0) {
    *String = AllocateZeroPool (sizeof (CHAR16));
    if (*String == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    return EFI_SUCCESS;
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
    *String = BdsLibGetStringById (STRING_TOKEN (STR_MISSING_STRING));
    if (*String == NULL) {
      return EFI_OUT_OF_RESOURCES;
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
  Show System Infomation, ie BIOS Version, BIOS Release Date,
  CPU Type, and Memory Speed.

  @param  PrintLine         Display line number of system information

  @retval EFI_SUCCESS       Success to show system information

**/
EFI_STATUS
ShowSystemInfo (
  IN OUT UINT8                        *PrintLine
  )
{
  UINT8                             StrIndex;
  BOOLEAN                           Find[3];
  EFI_STATUS                        Status;
  EFI_SMBIOS_HANDLE                 SmbiosHandle;
  EFI_SMBIOS_PROTOCOL               *Smbios;
  SMBIOS_TABLE_TYPE0                *Type0Record;
  SMBIOS_TABLE_TYPE4                *Type4Record;
  SMBIOS_TABLE_TYPE17               *Type17cord;
  EFI_SMBIOS_TABLE_HEADER           *Record;
  CHAR16                            *ItemStr;
  CHAR16                            *DataStr;
  
  ZeroMem (Find, sizeof (Find));

  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **) &Smbios
                  );
  ASSERT_EFI_ERROR (Status);

  gST->ConOut->EnableCursor (gST->ConOut, FALSE);
  gST->ConOut->SetAttribute (gST->ConOut, EFI_WHITE);

  if (FeaturePcdGet (PcdDynamicHotKeySupported)) {
    DYNAMIC_HOTKEY_PROTOCOL           *DynamicHotKey;
	
	DynamicHotKey = NULL;
    Status = gBS->LocateProtocol (&gDynamicHotKeyProtocolGuid, NULL, (VOID **)&DynamicHotKey);
    if (!EFI_ERROR (Status)) {
      Status = DynamicHotKey->ShowSystemInformation (PrintLine);
      if (Status == EFI_SUCCESS) {
        return EFI_SUCCESS;
      }
    }
  }  

  //
  // BIOS Version
  //
  ItemStr = BdsLibGetStringById (STRING_TOKEN (STR_BIOS_VERSION_STRING));
  if (ItemStr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  DataStr = BdsLibGetStringById (STRING_TOKEN (STR_MISC_BIOS_VERSION));
  if (DataStr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  BdsLibOutputStrings (gST->ConOut, L"\n\r", ItemStr, L" : ", DataStr, NULL);
  (*PrintLine)++;
  gBS->FreePool (ItemStr);
  gBS->FreePool (DataStr);

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  do {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, NULL, &Record, NULL);
    if (EFI_ERROR(Status)) {
      break;
    }

    //
    // BIOS Release Date
    //
    if (Record->Type == EFI_SMBIOS_TYPE_BIOS_INFORMATION) {
      Type0Record = (SMBIOS_TABLE_TYPE0 *) Record;
      StrIndex = Type0Record->BiosReleaseDate;
      Status = BdsLibGetOptionalStringByIndex ((CHAR8*)((UINT8*)Type0Record + Type0Record->Hdr.Length), StrIndex, &DataStr);
      if (Status != EFI_SUCCESS) {
        continue;
      }

      ItemStr = BdsLibGetStringById (STRING_TOKEN(STR_BIOS_BUILD_DATE));
      if (ItemStr == NULL) {
        gBS->FreePool (DataStr);
        continue;
      }
      BdsLibOutputStrings (gST->ConOut, L"\n\r", ItemStr, L" : ", DataStr, NULL);
      (*PrintLine)++;
      gBS->FreePool (ItemStr);
      gBS->FreePool (DataStr);
      Find[0] = TRUE;
    }  

    //
    // CPU Type
    //
    if (Record->Type == EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION) {
      Type4Record = (SMBIOS_TABLE_TYPE4 *) Record;
      StrIndex = Type4Record->ProcessorVersion;
      Status = BdsLibGetOptionalStringByIndex ((CHAR8*)((UINT8*)Type4Record + Type4Record->Hdr.Length), StrIndex, &DataStr);
      if (Status != EFI_SUCCESS) {
        continue;
      }
      
      ItemStr = BdsLibGetStringById (STRING_TOKEN(STR_CPU_TYPE_STRING));
      if (ItemStr == NULL) {
        gBS->FreePool (DataStr);
        continue;
      }
      BdsLibOutputStrings (gST->ConOut, L"\n\r", ItemStr, L" : ", DataStr, NULL);
      (*PrintLine)++;
      gBS->FreePool (ItemStr);
      gBS->FreePool (DataStr);
      Find[1] = TRUE;
    }

    //
    // Memory Bus Speed
    //
    if (Record->Type == EFI_SMBIOS_TYPE_MEMORY_DEVICE) {
      Type17cord = (SMBIOS_TABLE_TYPE17 *) Record;
      
      if (Type17cord->ConfiguredMemoryClockSpeed != 0) {
        DataStr = AllocateZeroPool (0x100);
        if (DataStr == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
        //
        // need to have 4 digits to show 1067 MHz
        //
        UnicodeValueToString (DataStr, PREFIX_ZERO, Type17cord->ConfiguredMemoryClockSpeed, 4);
        StrCat (DataStr, L" MHz");
            
        ItemStr = BdsLibGetStringById (STRING_TOKEN(STR_SYSTEM_MEMORY_SPEED_STRING));
        if (ItemStr == NULL) {
          gBS->FreePool (DataStr);
          continue;
        }
        BdsLibOutputStrings (gST->ConOut, L"\n\r", ItemStr, L" : ", DataStr, NULL);
        (*PrintLine)++;
        gBS->FreePool (ItemStr);
        gBS->FreePool (DataStr);
        Find[2] = TRUE;
      }
    }
  } while (!(Find[0] && Find[1] && Find[2]));

  return  EFI_SUCCESS;
}

