/** @file
  Get BIOS version, product name and CCB version from BVDT region.

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

#include <Library/BvdtLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>

/**
  Converts ASCII characters to Unicode.

  @param UnicodeStr             The Unicode string to be written to.
                                The buffer must be large enough.
  @param AsciiStr               The ASCII string to be converted.


  @return  The address to the Unicode string - same as UnicodeStr.

**/
STATIC
CHAR16 *
BvdtLibAscii2Unicode (
  OUT CHAR16         *UnicodeStr,
  IN  CHAR8          *AsciiStr
  )
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

/**
  Compare the ASCII strings in length.

  @param String  - Compare to String2
  @param String2 - Compare to String
  @param Length  - Number of ASCII characters to compare

  @return    == 0     - The substring of String and String2 is identical.
             > 0   - The substring of String sorts lexicographically after String2
             < 0   - The substring of String sorts lexicographically before String2
**/
STATIC
INTN
BvdtLibAsciiStrnCmp (
  IN CHAR8    *String,
  IN CHAR8    *String2,
  IN UINTN    Length
  )
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

/**
  Converts an 8-bit BCD value to an 8-bit value.

  @param[in] Value   The 8-bit BCD value to convert to an 8-bit value.

  @return The 8-bit value is returned.
**/
STATIC
UINT8
BcdToDecimal (
  IN UINT8           Value
  )
{
  ASSERT (Value < 0xa0);
  ASSERT ((Value & 0xf) < 0xa);
  return (Value >> 4) * 10 + (Value & 0xf);
}

/**
  Check if it is a leap year

  @param[in] Year   Year value

  @retval TRUE      It is a leap year
  @retval FALSE     It is NOT a leap year
**/
BOOLEAN
IsLeapYear (
  IN UINT16          Year
  )
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

/**
  Check if Day field of input EFI time is valid

  @param[in] EfiTime   The Day field of input EFI time is to be checked

  @retval TRUE         Day field of input EFI time is valid.
  @retval FALSE        Day field of input EFI time is NOT valid.
**/
BOOLEAN
IsDayValid (
  IN EFI_TIME        *EfiTime
  )
{
  UINT8              DayOfMonth[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  if (EfiTime->Day < 1 ||
      EfiTime->Day > DayOfMonth[EfiTime->Month - 1] ||
      (EfiTime->Month == 2 && (!IsLeapYear (EfiTime->Year) && EfiTime->Day > 28))) {
    return FALSE;
  }

  return TRUE;
}

/**
  Check if input EFI time is valid

  @param[in] EfiTime   The EFI time is to be checked

  @retval TRUE         Input EFI time is valid.
  @retval FALSE        Input EFI time is NOT valid.
**/
EFI_STATUS
IsEfiTimeValid (
  IN EFI_TIME        *EfiTime
  )
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

/**
  Get BVDT date data

  @param[in]  BvdtDatePtr   Pointer to BVDT date data
  @param[out] EfiTime       A pointer to storage BVDT date in EFI time structure

  @retval EFI_SUCCESS            Successfully get BVDT date data
  @retval EFI_INVALID_PARAMETER  Input parameter is NULL
**/
EFI_STATUS
GetBvdtDate (
  IN  UINT8      *BvdtDatePtr,
  OUT EFI_TIME   *EfiTime
  )
{
  if (BvdtDatePtr == NULL || EfiTime == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  EfiTime->Year  = (UINT16) BcdToDecimal (BvdtDatePtr[0]) + 2000;
  EfiTime->Month = BcdToDecimal (BvdtDatePtr[1]);
  EfiTime->Day   = BcdToDecimal (BvdtDatePtr[2]);

  return EFI_SUCCESS;
}

/**
  Get BVDT time data

  @param[in]  BvdtTimePtr   Pointer to BVDT time data
  @param[out] EfiTime       A pointer to storage BVDT time in EFI time structure

  @retval EFI_SUCCESS            Successfully get BVDT time data
  @retval EFI_INVALID_PARAMETER  Input parameter is NULL
**/
EFI_STATUS
GetBvdtTime (
  IN  UINT8      *BvdtTimePtr,
  OUT EFI_TIME   *EfiTime
  )
{
  if (BvdtTimePtr == NULL || EfiTime == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  EfiTime->Hour   = BcdToDecimal (BvdtTimePtr[0]);
  EfiTime->Minute = BcdToDecimal (BvdtTimePtr[1]);
  EfiTime->Second = BcdToDecimal (BvdtTimePtr[2]);

  return EFI_SUCCESS;
}

/**
  Get the pointer of BIOS release date infomation from BVDT

  @return The pointer of BIOS release date infomation or NULL if not found
**/
UINT8 *
GetBiosReleaseDatePtr (
  VOID
  )
{
  UINT8       RelaseDateTag[] = RELEASE_DATE_TAG;
  UINT8       *Bvdt;
  UINTN       Index;

  Bvdt = (UINT8 *)(UINTN)(BVDT_BASE_ADDRESS);
  //
  // Search for "$RDATE" from BVDT dynamic signature start
  //
  for (Index = MULTI_BIOS_VERSION_OFFSET; Index < BVDT_SIZE; Index++) {
    if (CompareMem(Bvdt + Index, RelaseDateTag, sizeof(RelaseDateTag)) == 0) {
      return (Bvdt + Index + sizeof (RelaseDateTag));
    }
  }

  return NULL;
}

/**
  Get BIOS version, product name, CCB version, multiple BIOS version, multiple product name
  or multiple CCB verion from BVDT region.

  @param[in]      Type          Information type of BVDT.
  @param[in, out] StrBufferLen  Input : string buffer length
                                Output: length of BVDT information string.
  @param[out]     StrBuffer     BVDT information string.

  @retval EFI_SUCCESS           Successly get string.
  @retval EFI_BUFFER_TOO_SMALL  Buffer was too small. The current length of information string
                                needed to hold the string is returned in BufferSize.
  @retval EFI_INVALID_PARAMETER Input invalid type of BVDT information.
                                BufferSize or Buffer is NULL.
  @retval EFI_NOT_FOUND         Can not find information of multiple version, multiple product name
                                or multiple CCB verion or BVDT build time is invalid

**/
EFI_STATUS
EFIAPI
GetBvdtInfo (
  IN     BVDT_TYPE           Type,
  IN OUT UINTN               *StrBufferLen,
  OUT    CHAR16              *StrBuffer
  )
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
    ZeroMem (&EfiTime, sizeof (EfiTime));
    GetBvdtDate ((UINT8 *) (UINTN) (BVDT_BASE_ADDRESS + BIOS_BUILD_DATE_OFFSET), &EfiTime);
    GetBvdtTime ((UINT8 *) (UINTN) (BVDT_BASE_ADDRESS + BIOS_BUILD_TIME_OFFSET), &EfiTime);
    if (!IsEfiTimeValid (&EfiTime)) {
      return EFI_NOT_FOUND;
    }

    if (Type == BvdtBuildDate) {
     AsciiSPrint (AsciiStrBuffer, sizeof (AsciiStrBuffer), "%02d/%02d/%04d", EfiTime.Month, EfiTime.Day, EfiTime.Year);
    } else {
     AsciiSPrint (AsciiStrBuffer, sizeof (AsciiStrBuffer), "%02d:%02d:%02d", EfiTime.Hour, EfiTime.Minute, EfiTime.Second);
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
      StrPtr = (CHAR8 *) ((UINTN) StrPtr + AsciiStrSize (StrPtr));
    }

    if (BvdtLibAsciiStrnCmp (StrPtr, MultiProductNameSignature, sizeof (MultiProductNameSignature))) {
      return EFI_NOT_FOUND;
    }
    StrPtr = (CHAR8 *) ((UINTN) StrPtr + sizeof (MultiProductNameSignature));
    break;

  case BvdtMultiCcbVer:
    StrPtr = (CHAR8 *) (UINTN) (BVDT_BASE_ADDRESS + MULTI_BIOS_VERSION_OFFSET);
    if (!BvdtLibAsciiStrnCmp (StrPtr, MultiBiosVerSignature, sizeof (MultiBiosVerSignature))) {
      StrPtr = (CHAR8 *) ((UINTN) StrPtr + AsciiStrSize (StrPtr));
    }

    if (!BvdtLibAsciiStrnCmp (StrPtr, MultiProductNameSignature, sizeof (MultiProductNameSignature))) {
      StrPtr = (CHAR8 *) ((UINTN) StrPtr + AsciiStrSize (StrPtr));
    }

    if (BvdtLibAsciiStrnCmp (StrPtr, MultiCcbVerSignature, sizeof (MultiCcbVerSignature))) {
      return EFI_NOT_FOUND;
    }
    StrPtr = (CHAR8 *) ((UINTN) StrPtr + sizeof (MultiCcbVerSignature));
    break;

  case BvdtReleaseDate:
    ZeroMem (&EfiTime, sizeof (EfiTime));
    GetBvdtDate (GetBiosReleaseDatePtr (), &EfiTime);
    if (!IsEfiTimeValid (&EfiTime)) {
      return EFI_NOT_FOUND;
    }
    AsciiSPrint (AsciiStrBuffer, sizeof (AsciiStrBuffer), "%02d/%02d/%04d", EfiTime.Month, EfiTime.Day, EfiTime.Year);
    StrPtr = AsciiStrBuffer;
    break;

  default:
    return EFI_INVALID_PARAMETER;
  }

  StrLen = AsciiStrSize (StrPtr);

  if (StrLen > *StrBufferLen) {
    *StrBufferLen = StrLen;
    return EFI_BUFFER_TOO_SMALL;
  }

  *StrBufferLen = StrLen;
  BvdtLibAscii2Unicode (StrBuffer, StrPtr);

  return EFI_SUCCESS;
}

/**
  Get ESRT System Firmware GUID and Version information from BVDT $ESRT tag

  @param[out]  FirmwareGuid     Pointer to the system firmware version GUID
  @param[out]  FirmwareVersion  Pointer to the system firmware version

  @retval      EFI_SUCCESS      System firmware GUID and system firmware version
                                are successfully retrieved
               EFI_NOT_FOUND    Unable to find system firmware GUID or system firmware
                                version in the BVDT table
**/
EFI_STATUS
EFIAPI
GetEsrtFirmwareInfo (
  OUT EFI_GUID          *FirmwareGuid,
  OUT UINT32            *FirmwareVersion
  )
{
  UINTN      Index;
  UINT8      *Bvdt;
  UINT8      *EsrtTagPtr;
  UINT8      EsrtTag[] = ESRT_TAG;

  if (FirmwareGuid == NULL || FirmwareVersion == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Bvdt = (UINT8 *)(UINTN)(BVDT_BASE_ADDRESS);
  //
  // Search for "$ESRT" from BVDT dynamic signature start
  //
  for (Index = MULTI_BIOS_VERSION_OFFSET; Index < BVDT_SIZE; Index++) {
    if (CompareMem(Bvdt + Index, EsrtTag, sizeof(EsrtTag)) == 0) {
      EsrtTagPtr = Bvdt + Index;
      *FirmwareVersion = *(UINT32 *)(EsrtTagPtr + sizeof(EsrtTag));
      *FirmwareGuid =  *(EFI_GUID *)(EsrtTagPtr + sizeof(EsrtTag) + sizeof(UINT32));
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

