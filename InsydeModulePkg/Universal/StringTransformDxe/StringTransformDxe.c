/** @file
 Multi Config DXE module implement code.

 This c file contains driver entry function for DXE phase.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/
#include <IndustryStandard/SmBios.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/GenericBdsLib/String.h>
#include <Library/PrintLib.h>
#include <Library/BvdtLib.h>
#include <Protocol/Smbios.h>
#include <Protocol/StringTransform.h>


STRING_TRANSFORM_PROTOCOL             mStringTransform;

#define GET_ARRAY_COUNT(a)            (sizeof (a)/ sizeof ((a)[0]))

CHAR16 *Supporttype[] = {
  L"%BIOSVER%",
  L"%BUILDDATE%",
  L"%CPUTYPE%",
  L"%MEMSPEED%",  
  L"%CPUID%"
};

VOID
ReplaceString (
  IN CONST CHAR16                                 *InputString,
  IN       CHAR16                                 *ReplaceStrPtr,
  IN       UINTN                                  ReplaceStrSize,
  IN       CHAR16                                 *DataStr,
  OUT      CHAR16                                 **OutputString
  )
{
  UINTN                                           BeforeStrSize;
  CHAR16                                          *RemainStr;
  UINTN                                           StringSize;
  CHAR16                                          *FinalStr;

  BeforeStrSize = 0;
  StringSize = 0;
  FinalStr = NULL;
  
  StringSize = StrSize (InputString) - ReplaceStrSize + StrSize (DataStr);
  FinalStr = (CHAR16 *)AllocateZeroPool ((StringSize + 1) * sizeof (CHAR16));
  BeforeStrSize = ((UINTN)ReplaceStrPtr - (UINTN)InputString);
  //
  //Copy the string before matched string.
  //
  CopyMem (FinalStr, InputString, BeforeStrSize);
  //
  //Copy the string which replace the the original string.
  //
  StrCat (FinalStr, DataStr);
  //
  //Copy the string after match String.
  //
  RemainStr = (CHAR16 *)((UINTN)ReplaceStrPtr + ReplaceStrSize);
  StrCat (FinalStr, RemainStr);

  *OutputString = FinalStr;
}

EFI_STATUS
GetOptionalStringByIndex (
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
    *String = L"Missing String";
  } else {
    *String = AllocatePool (StrSize * sizeof (CHAR16));
    if (*String == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    AsciiStrToUnicodeStr (OptionalStrStart, *String);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
GetSystemInfoFromSMBios (
  CHAR16                                          **BuildDate,
  CHAR16                                          **CpuType,
  CHAR16                                          **MemSpee,
  CHAR16                                          **CpuID  
  )
{
  EFI_SMBIOS_HANDLE                               SmbiosHandle;
  BOOLEAN                                         Find[3];
  UINT8                                           StrIndex;
  EFI_SMBIOS_TABLE_HEADER                         *Record;
  SMBIOS_TABLE_TYPE0                              *Type0Record;
  SMBIOS_TABLE_TYPE4                              *Type4Record;
  SMBIOS_TABLE_TYPE17                             *Type17cord;
  EFI_SMBIOS_PROTOCOL                             *Smbios;
  EFI_STATUS                                      Status;
  CHAR16                                          *StrPtr;

  ZeroMem (Find, sizeof (Find));

  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **)&Smbios
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  do {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, NULL, &Record, NULL);
    if (EFI_ERROR (Status)) {
      break;
    }

    //
    // BIOS Release Date
    //
    if (Record->Type == EFI_SMBIOS_TYPE_BIOS_INFORMATION) {
      Type0Record = (SMBIOS_TABLE_TYPE0 *)Record;
      StrIndex = Type0Record->BiosReleaseDate;
      Status = GetOptionalStringByIndex ((CHAR8*)((UINT8*)Type0Record + Type0Record->Hdr.Length), StrIndex, &StrPtr);
      if (Status != EFI_SUCCESS) {
        continue;
      }
      *BuildDate = StrPtr;
      Find[0] = TRUE;
    }  

    //
    // CPU Type
    //
    if (Record->Type == EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION) {
      Type4Record = (SMBIOS_TABLE_TYPE4 *)Record;
      StrIndex = Type4Record->ProcessorVersion;
      Status = GetOptionalStringByIndex ((CHAR8*)((UINT8*)Type4Record + Type4Record->Hdr.Length), StrIndex, &StrPtr);
      if (Status != EFI_SUCCESS) {
        continue;
      }
      *CpuType = StrPtr;
      //
      //  CPU ID
      //
      StrPtr = AllocateZeroPool (0x100);
      if (StrPtr == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      UnicodeSPrint (StrPtr, 0x100, L"%x", Type4Record->ProcessorId.Signature);
      *CpuID = StrPtr;      
      Find[1] = TRUE;
    }
    //
    // Memory Bus Speed
    //
    if (Record->Type == EFI_SMBIOS_TYPE_MEMORY_DEVICE) {
      Type17cord = (SMBIOS_TABLE_TYPE17 *)Record;
      
      if (Type17cord->ConfiguredMemoryClockSpeed != 0) {
        StrPtr = AllocateZeroPool (0x100);
        if (StrPtr == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
        //
        // need to have 4 digits to show 1067 MHz
        //
        UnicodeValueToString (StrPtr, PREFIX_ZERO, Type17cord->ConfiguredMemoryClockSpeed, 4);
        StrCat (StrPtr, L" MHz");
        *MemSpee = StrPtr;
        Find[2] = TRUE;
      }
    }    
  } while (!(Find[0] && Find[1] && Find[2]));

  if (!(Find[0] && Find[1] && Find[2])) {
    return EFI_NOT_FOUND;
  }
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
Transform (
  IN       STRING_TRANSFORM_PROTOCOL                *This,
  IN CONST CHAR16                                   *String,
  OUT      CHAR16                                   **OutputString
  )
{
  UINTN                                           Index;
  CHAR16                                          *MatchStr;
  UINTN                                           StringSize;
  CHAR16                                          Str[BVDT_MAX_STR_SIZE];
  EFI_STATUS                                      Status;
  CHAR16                                          *DataStr;  
  static CHAR16                                   *BuildDate;
  static CHAR16                                   *CpuType;
  static CHAR16                                   *MemSpeed;
  static CHAR16                                   *CpuID;
  static BOOLEAN                                  IsFirst;

  MatchStr = NULL;
  DataStr = NULL;
  Status = EFI_SUCCESS;
  //
  // OutputString default is same as input srting
  //
  *OutputString = (CHAR16*)String;
  
  for (Index = 0; Index < GET_ARRAY_COUNT (Supporttype); Index++) {
    MatchStr = StrStr (String, Supporttype[Index]);
    if (MatchStr != NULL) {
      break;
    }
  }

  if (MatchStr == NULL) {
    return EFI_NOT_FOUND;
  }
  
  if (!IsFirst) {
    GetSystemInfoFromSMBios (&BuildDate, &CpuType, &MemSpeed, &CpuID);
    IsFirst = TRUE;
  }


  switch (Index) {
  //
  //  BIOSVER
  //
  case 0: 
    StringSize = BVDT_MAX_STR_SIZE;
    Status = GetBvdtInfo ((BVDT_TYPE)BvdtBiosVer, &StringSize, Str);
    DataStr = Str;
    if (DataStr == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
    }
    
    break;
  //
  //  BUILDDATE
  //  
  case 1: 
    if (BuildDate == NULL) {
      Status = EFI_UNSUPPORTED;
    } else {
      DataStr = BuildDate;
    }
    break;
  //
  //  CPUTYPE
  //
  case 2: 
    if (CpuType == NULL) {
      Status = EFI_UNSUPPORTED;
    } else {
      DataStr = CpuType;
    }
    break;
  //
  //  MEMSPEED
  //
  case 3: 
    if (MemSpeed == NULL) {
      Status = EFI_UNSUPPORTED;
    } else {
      DataStr = MemSpeed;
    }
    break;
  //
  //  PROCESSORID
  //
  case 4: 
    if (CpuID == NULL) {
      Status = EFI_UNSUPPORTED;
    } else {
      DataStr = CpuID;
    }
    break;    
  default:
    Status = EFI_UNSUPPORTED;
  }

  if (EFI_ERROR (Status)) {    
    DataStr = L"N/A";  
  }
  ReplaceString (String, MatchStr, StrLen (Supporttype[Index]) * sizeof (CHAR16), DataStr, OutputString);
  return Status;
}

/**
 Entrypoint of this module.

 This function is the entry point of this module. It installs Multi Config Interface Protocol
 in DXE phase.

 @param[in]         ImageHandle         The firmware allocated handle for the EFI image.
 @param[in]         SystemTable         A pointer to the EFI System Table.

 @retval EFI_SUCCESS                    Install protocol success.
 @retval !EFI_SUCCESS                   Install protocol fail.
*/
EFI_STATUS
EFIAPI
StringTransformDxeEntry (
  IN  EFI_HANDLE                        ImageHandle,
  IN  EFI_SYSTEM_TABLE                  *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            Handle;

  mStringTransform.Transform = Transform;

  //
  // Install protocol
  //
  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gStringTransformProtocolGuid,
                  &mStringTransform,
                  NULL
                  );

  return Status;

}

