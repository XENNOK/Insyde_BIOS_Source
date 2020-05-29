/** @file
  The EFI Legacy BIOS Patform Protocol member function

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "LegacyBiosPlatformDxe.h"

#include <IndustryStandard/SmBios.h>
#include <Library/BvdtLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/PcdLib.h>
#include <Protocol/Smbios.h>

CHAR8       mOemString[] = "0InsydeH2O Version ";
CHAR16      mStrCcbVersion[BVDT_MAX_STR_SIZE] = L"";


static
EFI_STATUS
GetBiosVersionFromSmbios (
  OUT CHAR8        **AsciiString
  );

static
EFI_STATUS
FillOem16String (
  IN EFI_LEGACY_BIOS_PLATFORM_PROTOCOL              *This,
  IN VOID                                           **Table,
  OUT UINTN                                         *TableSize
  );

/**
  Finds the binary data or other platform information.

  @param  This                  The protocol instance pointer.
  @param  Mode                  Specifies what data to return. See See EFI_GET_PLATFORM_INFO_MODE enum.
  @param  Table                 Mode specific.  See EFI_GET_PLATFORM_INFO_MODE enum.
  @param  TableSize            	Mode specific.  See EFI_GET_PLATFORM_INFO_MODE enum.
  @param  Location             	Mode specific.  See EFI_GET_PLATFORM_INFO_MODE enum.
  @param  Alignment             Mode specific.  See EFI_GET_PLATFORM_INFO_MODE enum.
  @param  LegacySegment         Mode specific.  See EFI_GET_PLATFORM_INFO_MODE enum.
  @param  LegacyOffset          Mode specific.  See EFI_GET_PLATFORM_INFO_MODE enum.

  @retval EFI_SUCCESS           Data returned successfully.
  @retval EFI_UNSUPPORTED       Mode is not supported on the platform.
  @retval EFI_NOT_FOUND         Binary image or table not found.

**/
EFI_STATUS
EFIAPI
GetPlatformInfo (
  IN EFI_LEGACY_BIOS_PLATFORM_PROTOCOL            *This,
  IN EFI_GET_PLATFORM_INFO_MODE                   Mode,
  OUT VOID                                        **Table,
  OUT UINTN                                       *TableSize,
  OUT UINTN                                       *Location,
  OUT UINTN                                       *Alignment,
  IN  UINT16                                      LegacySegment,
  IN  UINT16                                      LegacyOffset
  )
{
  EFI_STATUS                        Status;
  UINTN                             Index;
  CHAR8                             *AsciiString;
  UINT16                            OemIntRomNum;
  SYSTEM_ROM_TABLE                  *TpmTable;
  SYSTEM_ROM_TABLE                  *SystemRomTable;
  SYSTEM_ROM_TABLE                  *SystemOemIntTable;
  UINTN                             StrSize;


  AsciiString = NULL;

  switch (Mode) {

  case EfiGetPlatformBinarySystemRom:
    //
    // Get System Rom(CSM16 binary) table from OptionRom Table of OemServices
    //
    SystemRomTable = NULL;
    Status = OemSvcInstallOptionRomTable (
               SYSTEM_ROM,
               (VOID**)&SystemRomTable
               );
    if (!EFI_ERROR (Status) || SystemRomTable == NULL) {
      return EFI_NOT_FOUND;
    }

    //
    // Loop through table of System rom descriptions
    //
    for (Index = 0; SystemRomTable[Index].Valid != 0; Index++) {

      Status = GetSectionFromFv (
                 &SystemRomTable[Index].FileName,
                 EFI_SECTION_RAW,
                 0,
                 Table,
                 (UINTN *) TableSize
                 );
      if (EFI_ERROR (Status)) {
        continue;
      }

      Status = EFI_SUCCESS;
      break;
    }

    gBS->FreePool (SystemRomTable);

    return Status;

  case EfiGetPlatformBinaryOem16Data:
    //
    // First, we need to request a buffer that is located in F-Segment. If LegacySegment and LegacyOffset are 0,
    // returns TableSize, Location and Alignment that we need. LegacyBios calls CSM16 to find a free space and
    // return the free space's address back. Then, we find the BIOS version from DataHub and return the
    // string's address and size. LegacyBios has the responsilbility to copy the string to the free space that
    // is located in F-Segment.
    //
    if ((LegacySegment == 0) && (LegacyOffset == 0)) {
      //
      //The first call, return TableSize, Location and Alignment
      //The string should be put in F-Segment, set bit0 of Location
      //
      *TableSize = AsciiStrLen (mOemString) + 1;

      StrSize = sizeof (mStrCcbVersion);
      Status = GetBvdtInfo ((BVDT_TYPE) BvdtCcbVer, &StrSize, mStrCcbVersion);
      if (!EFI_ERROR (Status)) {
        *TableSize += StrSize;
      }

      Status = GetBiosVersionFromSmbios (&AsciiString);
      if (Status == EFI_SUCCESS && AsciiString != NULL) {
        *TableSize += AsciiStrLen (AsciiString);
        gBS->FreePool (AsciiString);
      } else {
        DEBUG ((EFI_D_INFO, "System get BIOS version failed.\n"));
      }

      *Table = NULL;
      *Location = F0000Region;
      *Alignment = 0;
    } else {
      //
      //The second call, return the address for the string
      //
      *Table = (VOID *) (UINTN) ((LegacySegment << 4) + LegacyOffset);
      ZeroMem (*Table, *TableSize);
      *Table = NULL;
      FillOem16String (This, Table, TableSize);
    }
    return EFI_SUCCESS;

  case EfiGetPlatformBinaryOemIntData:
    //
    // Get System Rom table from OptionRom Table of OemServices
    //
    SystemOemIntTable = NULL;
    Status = OemSvcInstallOptionRomTable (
               SYSTEM_OEM_INT_ROM,
               &SystemOemIntTable
               );
    if (!EFI_ERROR (Status) || SystemOemIntTable == NULL) {
      return EFI_NOT_FOUND;
    }

    OemIntRomNum = LegacySegment;
    if (SystemOemIntTable[OemIntRomNum].Valid == 1) {
      Status = GetSectionFromFv (
                 &SystemOemIntTable[OemIntRomNum].FileName,
                 EFI_SECTION_RAW,
                 0,
                 Table,
                 (UINTN *) TableSize
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "Can not get INT ROM image %g. Status %r.\n", &SystemOemIntTable[OemIntRomNum].FileName,Status));
        ASSERT_EFI_ERROR (Status);
        return Status;
      }
      //
      // Bug Bug: If the OEM INT15 OpROM must locate at F0000 Region , please follow that simple code.
      //
      //   if (SystemOemIntTable[OemIntRomNum].FileName == OEM_INT15CB_FILE_GUID) {
      //     *Location   = E0000Region;
      //   } else {
      //     //
      //     // Search F000Region first.
      //     //
      //     *Location   = 0;
      //   }
      *Location   = E0000Region;
      *Alignment  = 1;
      return Status;
    }
    return EFI_NOT_FOUND;

  case EfiGetPlatformBinaryTpmBinary:
    //
    // Get TPM Rom table from OptionRom Table of OemServices
    //
    TpmTable = NULL;
    Status = OemSvcInstallOptionRomTable (
               TPM_ROM,
               &TpmTable
               );
    if (!EFI_ERROR (Status) || TpmTable == NULL) {
      return EFI_NOT_FOUND;
    }

    for (Index = 0; TpmTable[Index].Valid != 0; Index++) {

      *Table     = NULL;
      *TableSize = 0;

      Status = GetSectionFromFv (
                 &TpmTable[Index].FileName,
                 EFI_SECTION_RAW,
                 0,
                 Table,
                 (UINTN *) TableSize
                 );
      if (EFI_ERROR (Status)) {
        continue;
      }
    }

    //
    // Temp kludge - ETpm uses negative logic since variable defaults are 0
    //
    //
    // Update CMOS to reflect TPM enable/disable for ACPI usage
    //

    if (EFI_ERROR (Status) || ( Table == NULL)) {
      *Table = NULL;
      *TableSize  = 0;
      return EFI_NOT_FOUND;
    }

    return EFI_SUCCESS;
    break;

  case EfiGetPlatformPciExpressBase:
    *Location = (UINTN) PcdGet64 (PcdPciExpressBaseAddress);
    return EFI_SUCCESS;
    break;

  case EfiGetPlatformBinaryMpTable:
  case EfiGetPlatformBinaryOem32Data:
  default:
    return EFI_UNSUPPORTED;
  }
}

EFI_STATUS
GetOptionalStringByIndex (
  IN      CHAR8                   *OptionalStrStart,
  IN      UINT8                   Index,
  OUT     CHAR8                   **String
  )
{
  UINTN          StrSize;

  if (Index == 0) {
    *String = AllocateZeroPool (sizeof (CHAR8));
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
    return EFI_NOT_FOUND;
  } else {
    *String = AllocatePool (StrSize * sizeof (CHAR8));
    AsciiStrCpy (*String, OptionalStrStart);
  }

  return EFI_SUCCESS;
}

/**
  Get BIOS version Ascii string from SMBIOS

  @param  AsciiString           A double pointer which point to the Bios version Ascii string address.

  @retval EFI_SUCCESS           it can find Bios version information from SMBIOS
  @retval EFI_NOT_FOUND         it can't find Bios version information from SMBIOS

**/
static
EFI_STATUS
GetBiosVersionFromSmbios (
  OUT CHAR8        **AsciiString
  )
{
  UINT8                             StrIndex;
  EFI_STATUS                        Status;
  EFI_SMBIOS_HANDLE                 SmbiosHandle;
  EFI_SMBIOS_PROTOCOL               *Smbios;
  SMBIOS_TABLE_TYPE0                *Type0Record;
  EFI_SMBIOS_TABLE_HEADER           *Record;

  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **) &Smbios
                  );
  ASSERT_EFI_ERROR (Status);

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
      Type0Record = (SMBIOS_TABLE_TYPE0 *) Record;
      StrIndex = Type0Record->BiosVersion;
      Status = GetOptionalStringByIndex ((CHAR8*)((UINT8*)Type0Record + Type0Record->Hdr.Length), StrIndex, AsciiString);
      break;
    }
  } while (TRUE);

  return Status;
}

/**
  The function gets Bios version string from FirmwareVolume. After it finds the string, it returns the string's
  address and size.

  @param  This                  Protocol instance pointer.
  @param  Table                 Point to the buffer that saves the BIOS string
  @param  TableSize             Size of the BIOS string

  @retval EFI_SUCCESS           Find the string and return its size and address
  @retval EFI_OUT_OF_RESOURCES  Cannot allocate enough memory

**/
static
EFI_STATUS
FillOem16String (
  IN EFI_LEGACY_BIOS_PLATFORM_PROTOCOL              *This,
  IN VOID                                           **Table,
  OUT UINTN                                         *TableSize
  )
{
  CHAR8                                 *StringTempPtr;
  CHAR8                                 *BiosVersionPtr;
  CHAR8                                 *AsciiString = NULL;
  UINTN                                 Index = 0;
  EFI_STATUS                            Status;

  StringTempPtr = NULL;
  StringTempPtr = AllocateZeroPool (*TableSize);
  if (StringTempPtr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  *Table = StringTempPtr;

  CopyMem (StringTempPtr, mOemString, AsciiStrLen (mOemString));
  StringTempPtr += AsciiStrLen (mOemString);

  while (mStrCcbVersion[Index] != 0) {
    *StringTempPtr++ = (UINT8) mStrCcbVersion[Index++];
  }

  Status = GetBiosVersionFromSmbios (&AsciiString);
  if (Status == EFI_SUCCESS && AsciiString != NULL) {
    BiosVersionPtr = AsciiString;
    CopyMem (StringTempPtr, BiosVersionPtr, AsciiStrLen (BiosVersionPtr));
  } else {
    DEBUG ((EFI_D_INFO, "System get BIOS version failed.\n"));
  }
  //
  // The string must be start with null
  //
  *((CHAR8 *)(*Table)) = 0;
  return EFI_SUCCESS;
}


