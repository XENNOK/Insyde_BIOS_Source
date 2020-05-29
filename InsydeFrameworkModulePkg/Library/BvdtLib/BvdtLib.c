//;******************************************************************************
//;* Copyright (c) 1983-2014, Insyde Software Corporation. All Rights Reserved.
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
//;   BvdtLib.c
//;
//; Abstract:
//;
//;   Get BIOS version, product name and CCB version from BVDT region.
//;
//;

#include "BvdtLib.h"
#include "EfiPrintLib.h"

STATIC
CHAR16 *
BvdtLibAscii2Unicode (
  OUT CHAR16         *UnicodeStr,
  IN  CHAR8          *AsciiStr
  )
/*++

Routine Description:

  Converts ASCII characters to Unicode.

Arguments:

  UnicodeStr - the Unicode string to be written to. The buffer must be large enough.
  AsciiStr   - The ASCII string to be converted.

Returns:
  The address to the Unicode string - same as UnicodeStr.

--*/
{
  CHAR16  *Str;

  Str = UnicodeStr;

  while (TRUE) {
    *(UnicodeStr++) = (CHAR16) *AsciiStr;
    if (*(AsciiStr++) == '\0') {
      return Str;
    }
  }
}

STATIC
INTN
BvdtLibAsciiStrnCmp (
  IN CHAR8    *String,
  IN CHAR8    *String2,
  IN UINTN    Length
  )
/*++

Routine Description:

  Compare the ASCII strings in length.

Arguments:

  String  - Compare to String2
  String2 - Compare to String
  Length  - Number of ASCII characters to compare

Returns:

  0     - The substring of String and String2 is identical.
  > 0   - The substring of String sorts lexicographically after String2
  < 0   - The substring of String sorts lexicographically before String2

--*/
{
  if (Length == 0) {
    return 0;
  }

  while ((*String != '\0') && (*String == *String2) && (Length > 1)) {
    String++;
    String2++;
    Length--;
  }

  return *String - *String2;
}

STATIC
UINT8
BcdToDecimal (
  IN UINT8           Value
  )
/*++

Routine Description:

  Converts an 8-bit BCD value to an 8-bit value.

Arguments:

  Value   - The 8-bit BCD value to convert to an 8-bit value.

Returns:

  The 8-bit value is returned.

--*/
{
  ASSERT (Value < 0xa0);
  ASSERT ((Value & 0xf) < 0xa);
  return (Value >> 4) * 10 + (Value & 0xf);
}

BOOLEAN
IsLeapYear (
  IN UINT16          Year
  )
/*++

Routine Description:

  Check if it is a leap year

Arguments:

  Year    - Year value

Returns:

  TRUE    - It is a leap year
  FALSE   - It is NOT a leap year

--*/
{
  if (Year % 4 == 0) {
    if (Year % 100 == 0) {
      if (Year % 400 == 0) {
        return TRUE;
      } else {
        return FALSE;
      }
    } else {
      return TRUE;
    }
  } else {
    return FALSE;
  }
}

BOOLEAN
IsDayValid (
  IN EFI_TIME        *EfiTime
  )
/*++

Routine Description:

  Check if Day field of input EFI time is valid

Arguments:

  EfiTime   - The Day field of input EFI time is to be checked

Returns:

  TRUE      - Day field of input EFI time is valid.
  FALSE     - Day field of input EFI time is NOT valid.

--*/
{
  UINT8              DayOfMonth[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  if (EfiTime->Day < 1 ||
      EfiTime->Day > DayOfMonth[EfiTime->Month - 1] ||
      (EfiTime->Month == 2 && (!IsLeapYear (EfiTime->Year) && EfiTime->Day > 28))) {
    return FALSE;
  }

  return TRUE;
}

EFI_STATUS
IsEfiTimeValid (
  IN EFI_TIME        *EfiTime
  )
/*++

Routine Description:

  Check if input EFI time is valid

Arguments:

  EfiTime   - The EFI time is to be checked

Returns:

  TRUE      - Input EFI time is valid.
  FALSE     - Input EFI time is NOT valid.

--*/
{
  if (EfiTime->Year  < 1900   ||
      EfiTime->Year  > 9999   ||
      EfiTime->Month < 1      ||
      EfiTime->Month > 12     ||
      (!IsDayValid (EfiTime)) ||
      EfiTime->Hour   > 23    ||
      EfiTime->Minute > 59    ||
      EfiTime->Second > 59) {
    return FALSE;
  }

  return TRUE;
}

EFI_STATUS
GetBvdtDate (
  IN  UINT8      *BvdtDatePtr,
  OUT EFI_TIME   *EfiTime
  )
/*++

Routine Description:

  Get BVDT date data

Arguments:

  BvdtDatePtr - Pointer to BVDT date data
  EfiTime     - A pointer to storage BVDT date in EFI time structure.

Returns:

  EFI_SUCCESS             - Successfully get BVDT date data
  EFI_INVALID_PARAMETER   - Input parameter is NULL pointer

--*/
{
  if (BvdtDatePtr == NULL || EfiTime == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  EfiTime->Year  = (UINT16) BcdToDecimal (BvdtDatePtr[0]) + 2000;
  EfiTime->Month = BcdToDecimal (BvdtDatePtr[1]);
  EfiTime->Day   = BcdToDecimal (BvdtDatePtr[2]);

  return EFI_SUCCESS;
}

EFI_STATUS
GetBvdtTime (
  IN  UINT8      *BvdtTimePtr,
  OUT EFI_TIME   *EfiTime
  )
/*++

Routine Description:

  Get BVDT time data

Arguments:

  BvdtTimePtr - Pointer to BVDT time data
  EfiTime     - A pointer to storage BVDT time in EFI time structure.

Returns:

  EFI_SUCCESS             - Successfully get BVDT time data
  EFI_INVALID_PARAMETER   - Input parameter is NULL pointer

--*/
{
  if (BvdtTimePtr == NULL || EfiTime == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  EfiTime->Hour   = BcdToDecimal (BvdtTimePtr[0]);
  EfiTime->Minute = BcdToDecimal (BvdtTimePtr[1]);
  EfiTime->Second = BcdToDecimal (BvdtTimePtr[2]);

  return EFI_SUCCESS;
}

UINT8 *
GetBiosReleaseDatePtr (
  VOID
  )
/*++

Routine Description:

  Get the pointer of BIOS release date infomation from BVDT

Arguments:

Returns:

  The pointer of BIOS release date infomation or NULL if not found

--*/
{
  UINT8       RelaseDateTag[] = RELEASE_DATE_TAG;
  UINT8       *Bvdt;
  UINTN       Index;

  Bvdt = (UINT8 *)(UINTN)(BVDT_BASE_ADDRESS);
  //
  // Search for "$RDATE" from BVDT dynamic signature start
  //
  for (Index = MULTI_BIOS_VERSION_OFFSET; Index < BVDT_SIZE; Index++) {
    if (EfiCompareMem (Bvdt + Index, RelaseDateTag, sizeof(RelaseDateTag)) == 0) {
      return (Bvdt + Index + sizeof (RelaseDateTag));
    }
  }

  return NULL;
}

EFI_STATUS
GetBvdtInfo (
  IN     BVDT_TYPE           Type,
  IN OUT UINTN               *StrBufferLen,
  OUT    CHAR16              *StrBuffer
  )
/*++

Routine Description:

  Get BIOS version, product name, CCB version, multiple BIOS version, multiple product name
  or multiple CCB verion from BVDT region.

Arguments:

  Type           - Information type of BVDT.
  StrBufferLen   - Input : string buffer length
                   Output: length of BVDT information string.
  StrBuffer      - BVDT information string.

Returns:

  EFI_SUCCESS           - Successly get string.
  EFI_BUFFER_TOO_SMALL  - Buffer was too small. The current length of information string
                          needed to hold the string is returned in BufferSize.
  EFI_INVALID_PARAMETER - Input invalid type of BVDT information.
                          StrBufferLen or StrBuffer is NULL.
  EFI_NOT_FOUND         - Can not find information of multiple version, multiple product name
                          or multiple CCB verion or BVDT build time is invalid

--*/
{
  UINTN          StrLen;
  CHAR8          *StrPtr;
  CHAR8          MultiBiosVerSignature[]     = {'$', 'V', 'E', 'R', 'E', 'X'};
  CHAR8          MultiProductNameSignature[] = {'$', 'P', 'R', 'O', 'D', 'E', 'X'};
  CHAR8          MultiCcbVerSignature[]      = {'$', 'C', 'C', 'B', 'V', 'E', 'X'};
  CHAR8          AsciiStrBuffer[BVDT_MAX_STR_SIZE];
  EFI_TIME       EfiTime;

  if ((StrBuffer == NULL) || (StrBufferLen == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  switch (Type) {

  case BvdtBuildDate:
  case BvdtBuildTime:
    EfiCommonLibZeroMem (&EfiTime, sizeof (EfiTime));
    GetBvdtDate ((UINT8 *) (UINTN) (BVDT_BASE_ADDRESS + BIOS_BUILD_DATE_OFFSET), &EfiTime);
    GetBvdtTime ((UINT8 *) (UINTN) (BVDT_BASE_ADDRESS + BIOS_BUILD_TIME_OFFSET), &EfiTime);
    if (!IsEfiTimeValid (&EfiTime)) {
      return EFI_NOT_FOUND;
    }

    if (Type == BvdtBuildDate) {
     ASPrint (AsciiStrBuffer, sizeof (AsciiStrBuffer), "%02d/%02d/%04d", EfiTime.Month, EfiTime.Day, EfiTime.Year);
    } else {
     ASPrint (AsciiStrBuffer, sizeof (AsciiStrBuffer), "%02d:%02d:%02d", EfiTime.Hour, EfiTime.Minute, EfiTime.Second);
    }
    StrPtr = AsciiStrBuffer;
    break;

  case BvdtBiosVer:
    StrPtr = (CHAR8 *) (UINTN) (BVDT_BASE_ADDRESS + BIOS_VERSION_OFFSET);
    break;

  case BvdtProductName:
    StrPtr = (CHAR8 *) (UINTN) (BVDT_BASE_ADDRESS + PRODUCT_NAME_OFFSET);
    break;

  case BvdtCcbVer:
    StrPtr = (CHAR8 *) (UINTN) (BVDT_BASE_ADDRESS + CCB_VERSION_OFFSET);
    break;

  case BvdtMultiBiosVer:
    StrPtr = (CHAR8 *) (UINTN) (BVDT_BASE_ADDRESS + MULTI_BIOS_VERSION_OFFSET);
    if (BvdtLibAsciiStrnCmp (StrPtr, MultiBiosVerSignature, sizeof (MultiBiosVerSignature))) {
      return EFI_NOT_FOUND;
    }
    StrPtr = (CHAR8 *) ((UINTN) StrPtr + sizeof (MultiBiosVerSignature));
    break;

  case BvdtMultiProductName:
    StrPtr = (CHAR8 *) (UINTN) (BVDT_BASE_ADDRESS + MULTI_BIOS_VERSION_OFFSET);
    if (!BvdtLibAsciiStrnCmp (StrPtr, MultiBiosVerSignature, sizeof (MultiBiosVerSignature))) {
      StrPtr = (CHAR8 *) ((UINTN) StrPtr + EfiAsciiStrSize (StrPtr));
    }

    if (BvdtLibAsciiStrnCmp (StrPtr, MultiProductNameSignature, sizeof (MultiProductNameSignature))) {
      return EFI_NOT_FOUND;
    }
    StrPtr = (CHAR8 *) ((UINTN) StrPtr + sizeof (MultiProductNameSignature));
    break;

  case BvdtMultiCcbVer:
    StrPtr = (CHAR8 *) (UINTN) (BVDT_BASE_ADDRESS + MULTI_BIOS_VERSION_OFFSET);
    if (!BvdtLibAsciiStrnCmp (StrPtr, MultiBiosVerSignature, sizeof (MultiBiosVerSignature))) {
      StrPtr = (CHAR8 *) ((UINTN) StrPtr + EfiAsciiStrSize (StrPtr));
    }

    if (!BvdtLibAsciiStrnCmp (StrPtr, MultiProductNameSignature, sizeof (MultiProductNameSignature))) {
      StrPtr = (CHAR8 *) ((UINTN) StrPtr + EfiAsciiStrSize (StrPtr));
    }

    if (BvdtLibAsciiStrnCmp (StrPtr, MultiCcbVerSignature, sizeof (MultiCcbVerSignature))) {
      return EFI_NOT_FOUND;
    }
    StrPtr = (CHAR8 *) ((UINTN) StrPtr + sizeof (MultiCcbVerSignature));
    break;

  case BvdtReleaseDate:
    EfiCommonLibZeroMem (&EfiTime, sizeof (EfiTime));
    GetBvdtDate (GetBiosReleaseDatePtr (), &EfiTime);
    if (!IsEfiTimeValid (&EfiTime)) {
      return EFI_NOT_FOUND;
    }
    ASPrint (AsciiStrBuffer, sizeof (AsciiStrBuffer), "%02d/%02d/%04d", EfiTime.Month, EfiTime.Day, EfiTime.Year);
    StrPtr = AsciiStrBuffer;
    break;

  default:
    return EFI_INVALID_PARAMETER;
  }

  StrLen = EfiAsciiStrSize (StrPtr);

  if (StrLen > *StrBufferLen) {
    *StrBufferLen = StrLen;
    return EFI_BUFFER_TOO_SMALL;
  }

  *StrBufferLen = StrLen;
  BvdtLibAscii2Unicode (StrBuffer, StrPtr);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
GetEsrtFirmwareInfo (
  OUT EFI_GUID          *FirmwareGuid,
  OUT UINT32            *FirmwareVersion
  )
/*++

Routine Description:

  Get ESRT System Firmware GUID and Version information from BVDT $ESRT tag

Arguments:

  FirmwareGuid     - Pointer to the system firmware version GUID
  FirmwareVersion  - Pointer to the system firmware version

Returns:

  EFI_SUCCESS           - Successfully retrieve system firmware GUID and system firmware version
  EFI_NOT_FOUND         - Unable to find system firmware GUID or system firmware version in the BVDT table
  EFI_INVALID_PARAMETER - Input parameter is NULL.

--*/
{
  UINTN       Index;
  UINT8       *Bvdt;
  UINT8       *EsrtTagPtr;
  UINT8       EsrtTag[] = ESRT_TAG;

  if (FirmwareGuid == NULL || FirmwareVersion == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Bvdt = (UINT8 *)(UINTN)(BVDT_BASE_ADDRESS);
  //
  // Search for "$ESRT" from BVDT dynamic signature start
  //
  for (Index = MULTI_BIOS_VERSION_OFFSET; Index < BVDT_SIZE; Index++) {
    if (EfiCompareMem(Bvdt + Index, EsrtTag, sizeof(EsrtTag)) == 0) {
      EsrtTagPtr = Bvdt + Index;
      *FirmwareVersion = *(UINT32 *)(EsrtTagPtr + sizeof(EsrtTag));
      EfiCommonLibCopyMem (
        FirmwareGuid,
        (EsrtTagPtr + sizeof(EsrtTag) + sizeof(UINT32)),
        sizeof (EFI_GUID)
        );
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

