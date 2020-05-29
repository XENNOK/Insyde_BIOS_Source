/** @file
 H2O IPMI common library implement code.

 This c file contains common functions that are used in H2O IPMI modules.

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


#include <Uefi.h>
#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IpmiCommonLib.h>
#include <BaseIpmiCommonLibInternal.h>


UINT32 mDaysOfMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


/**
 This function verifies the leap year.

 @param[in]         Year                Year in YYYY format.

 @retval TRUE                           The year is a leap year.
 @retval FALSE                          The year is not a leap year.
*/
BOOLEAN
IpmiIsLeapYear (
  IN  UINT16                            Year
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
 Count the number of the leap years between 1970 and CurYear.

 @param[in]         CurYear             The Current year.

 @retuen The count of leapyears.
*/
UINTN
CountNumOfLeapYears (
  IN  UINT16                            CurYear
  )
{
  UINT16                                NumOfYear;
  UINT16                                BaseYear;
  UINT16                                Index;
  UINTN                                 Count;


  Count     = 0;
  BaseYear  = BASE_YEAR;
  NumOfYear = (UINT16) (CurYear - BASE_YEAR);
  for (Index = 0; Index <= NumOfYear; Index++) {
    if (IpmiIsLeapYear ((UINT16) (BaseYear + Index))) {
      Count += 1;
    }
  }

  return Count;

}


/**
 This function converts EFI_TIME structure to a TimeStamp.

 @param[in]         Time                EFI_TIME structure to be converted.
 @param[out]        TimeStamp           TimeStamp converted from EFI_TIME structure.
*/
VOID
EfiTimeToTimeStamp (
  IN  EFI_TIME                          *Time,
  OUT UINT32                            *TimeStamp
  )
{
  UINT16                                Year;
  UINT16                                AddedDays;
  UINT8                                 Month;

  //
  // Find number of leap years
  //
  AddedDays = 0;
  for (Year = BASE_YEAR; Year < Time->Year; ++Year) {
    if (IpmiIsLeapYear (Year)) {
      ++AddedDays;
    }
  }

  //
  // Number of days of complete years (include all leap years)
  //
  *TimeStamp = (Time->Year - BASE_YEAR) * DAYS_PER_YEEAR;
  *TimeStamp += AddedDays;

  //
  // Number of days from 1970/1/1 to now
  //
  for (Month = 0; Month < Time->Month - BASE_MONTH; ++Month) {
    *TimeStamp += mDaysOfMonth[Month];
  }
  *TimeStamp += Time->Day - BASE_DAY;

  //
  // Check this Feb. is 28 days or 29 days
  //
  if (IpmiIsLeapYear (Time->Year) && Time->Month > 2) {
    *TimeStamp += 1;
  }

  //
  // Convert days to seconds
  //
  *TimeStamp *= SECONDS_PER_DAY;

  //
  // Add rest seconds
  //
  *TimeStamp += (Time->Hour * SECONDS_PER_HOUR) +
                (Time->Minute * SECONDS_PER_MINUTE) +
                Time->Second;

}


/**
 Convert timestamp to EFI_TIME structure.

 @param[in]         TimeStamp           Timestamp to be converted.
 @param[out]        EfiTime             EFI_TIME structure use to store the result converting from TimeStamp.
*/
VOID
TimeStampToEfiTime (
  IN  UINT32                            TimeStamp,
  OUT EFI_TIME                          *Time
  )
{
  UINT16                                NumOfYears;
  UINTN                                 NumOfLeapYears;
  UINTN                                 NumOfDays;
  UINTN                                 DayOfYear;
  UINT16                                Years;
  UINT32                                Mouths;

  //
  // Caculate second;
  //
  Time->Second = (UINT8) (TimeStamp % SECONDS_PER_MINUTE);

  //
  // Caculate minitunes;
  //
  TimeStamp /= SECONDS_PER_MINUTE;
  Time->Minute = (UINT8) (TimeStamp % MINUTES_PER_HOUR);

  //
  // Caculate Hour;
  //
  TimeStamp /= MINUTES_PER_HOUR;
  Time->Hour = (UINT8) (TimeStamp % HOURS_PER_DAY);
  NumOfDays = TimeStamp / HOURS_PER_DAY;

  //
  // Caculate Year
  //
  NumOfYears = (UINT16) (NumOfDays / DAYS_PER_YEEAR);
  Years = BASE_YEAR + NumOfYears;

  if (Years != BASE_YEAR) {
    NumOfLeapYears = CountNumOfLeapYears (Years - 1);
  } else {
    NumOfLeapYears = 0;
  }

  NumOfDays -= NumOfLeapYears;

  //
  // Year = 1970 + NumOfYear
  //
  Time->Year = (UINT16) (NumOfDays / DAYS_PER_YEEAR + BASE_YEAR);

  DayOfYear = NumOfDays % DAYS_PER_YEEAR + 1;

  if (IpmiIsLeapYear (Time->Year)) {
    mDaysOfMonth[1] = 29;
  } else {
    mDaysOfMonth[1] = 28;
  }

  for (Mouths = 0; DayOfYear > mDaysOfMonth[Mouths]; Mouths++) {
      DayOfYear -= mDaysOfMonth[Mouths];
  }

  Time->Month = (UINT8) Mouths + 1;
  Time->Day = (UINT8) DayOfYear;

  //
  // Rollback days of Feb. since we may change it
  //
  mDaysOfMonth[1] = 28;

}


/**
 Check if the input string is valid IPv4 format and return the IPv4 number.

 @param[in]         InStr               The input string in IPv4 format. This can be IPv4 IP Address,
                                        IPv4 Subnet Mask and IPv4 Gateway Address.
 @param[in]         IPv4                The buffer to store IPv4 value.

 @retval TRUE       The input IPv4 string is valid IPv4 format.
 @retval FALSE      The input IPv4 string is invalid IPv4 format.
*/
BOOLEAN
IsValidIpv4 (
  IN  CHAR16                            *InStr,
  OUT UINT8                             *Ipv4
  )
{
  UINTN                                 Index;
  UINTN                                 DotCount;
  CHAR16                                TempStr[0x20];
  UINTN                                 Value;
  UINT8                                 TempBuffer[4];
  CHAR16                                *TempChar;

  Index = 0;
  DotCount = 0;
  Value = 0;
  TempChar = InStr;

  while (*TempChar != L'\0') {

    if (*TempChar == L'.') {

      Value = StrDecimalToUintn (TempStr);

      if (Index > 3 || Value > 255) {
        return FALSE;
      } else {
        TempBuffer[DotCount] = (UINT8)Value;
        Index = 0;
        DotCount++;
      }

    } else {

      //
      // Check if valid IPv4 digits
      //
      if (*TempChar >= '0' && *TempChar <= '9') {
        TempStr[Index] = *TempChar;
        Index++;
      } else {
        return FALSE;
      }

    }

    ++TempChar;
    TempStr[Index] = L'\0';

  }

  if (DotCount != 3) {
    return FALSE;
  } else {
    Value = StrDecimalToUintn (TempStr);

    if (Value > 255) {
      return FALSE;
    } else {
      TempBuffer[DotCount] = (UINT8)Value;
      CopyMem (Ipv4, TempBuffer, 4);
      return TRUE;
    }
  }

}


/**
 Check if the input string is valid IPv6 format.

 @param[in]         InStr               The input string in IPv6 format. This can be IPv4 IP Address
                                        IPv6 Gateway Address.
 @param[in]         IPv6                The buffer to store IPv6 value.

 @retval TRUE       The input IPv6 string is valid IPv6 format.
 @retval FALSE      The input IPv6 string is invalid IPv6 format.
*/
BOOLEAN
IsValidIpv6 (
  IN  CHAR16                            *InStr,
  OUT UINT8                             *Ipv6
  )
{
  //    # NOTE: 2001:0db8:0000:0000:0000:0000:1428:57ab
  //    #       2001:0db8:0000:0000:0000::1428:57ab
  //    #       2001:0db8:0:0:0:0:1428:57ab
  //    #       2001:0db8:0:0::1428:57ab
  //    #       2001:0db8::1428:57ab
  //    #       2001:db8::1428:57ab
  //    #       2001:0db8:0000:0000:0000:0000:<IPv4>
  //    #       ::<IPv4>
  //    #       ::
  UINTN                                 Index;
  UINTN                                 LastColonIndex;
  UINT16                                Ipv6Buffer1[8];
  UINTN                                 Ipv6Buffer1Index;
  UINT16                                Ipv6Buffer2[8];
  UINTN                                 Ipv6Buffer2Index;
  UINT8                                 Ipv4Buffer[4];
  UINT16                                Ipv6String[0x80];
  CHAR16                                SegmentBuffer[0x10];
  UINTN                                 SegmentBufferIndex;
  UINTN                                 Length;
  UINTN                                 DotCount;
  UINTN                                 ColonCount;
  BOOLEAN                               HasIpv4;
  BOOLEAN                               HasDoubleColon;
  CHAR16                                *TempPtr;
  BOOLEAN                               UseIpv6Buffer1;

  //
  // Quickly check first and last character
  //
  Length = StrLen (InStr);
  if (Length < 2) {
    return FALSE;
  }

  if ((InStr[0] == L':' && InStr[1] != L':') ||
      (InStr[Length - 1] == L':' && InStr[Length - 2] != L':')) {
    return FALSE;
  }

  //
  // Quick check more than one "::"
  //
  HasDoubleColon = FALSE;
  TempPtr = StrStr (InStr, L"::");
  if (TempPtr != NULL) {
    HasDoubleColon = TRUE;
    TempPtr++;
    if (StrStr (TempPtr, L"::") != NULL) {
      return FALSE;
    }
  }

  //
  // 1. Count '.' number to check if it contains IPv4 string
  // 2. Count ':' number to check if it contains enough segments of IPv6
  //
  LastColonIndex = 0;
  DotCount = 0;
  ColonCount = 0;
  Index = Length - 1;
  while (TRUE) {

    if (InStr[Index] == L'.') {
      DotCount++;
    } else if (InStr[Index] == L':') {
      if (ColonCount == 0) {
        LastColonIndex = Index;
      }
      ColonCount++;
    }

    if (Index == 0) {
      break;
    }

    Index--;

  }

  //
  // Check if it contains valid IPv4 string
  //
  ZeroMem (Ipv4Buffer, sizeof (Ipv4Buffer));
  HasIpv4 = FALSE;
  if (DotCount == 3 && IsValidIpv4 (&InStr[LastColonIndex + 1], Ipv4Buffer)) {
    HasIpv4 = TRUE;
  } else if (DotCount != 0) {
    return FALSE;
  }

  //
  // Use colon number to check if the IPv6 string has enough segments
  // 1. IPv4 = TRUE,  DoubleColon = TRUE,  Colon number shoule be 2~6
  // 2. IPv4 = TRUE,  DoubleColon = FALSE, Colon number shoule be 6
  // 3. IPv4 = FALSE, DoubleColon = TRUE,  Colon number shoule be 2~7
  // 4. IPv4 = FALSE, DoubleColon = FALSE, Colon number shoule be 7
  //
  if ((HasIpv4 && HasDoubleColon && ColonCount > 6) ||
      (HasIpv4 && !HasDoubleColon && ColonCount != 6) ||
      (!HasIpv4 && HasDoubleColon && ColonCount > 7) ||
      (!HasIpv4 && !HasDoubleColon && ColonCount != 7)) {
    return FALSE;
  }

  //
  // Copy IPv6 string to another buffer, and append a ':' to the end of buffer.
  // This can make us easier to handle it.
  //
  ZeroMem (Ipv6String, sizeof (Ipv6String));
  if (HasIpv4) {
    StrnCpy (Ipv6String, InStr, LastColonIndex);
  } else {
    StrCpy (Ipv6String, InStr);
  }
  StrCat (Ipv6String, L":");

  //
  // Go throught whole string
  //
  ZeroMem (Ipv6Buffer1, sizeof (Ipv6Buffer1));
  ZeroMem (Ipv6Buffer2, sizeof (Ipv6Buffer2));
  ZeroMem (SegmentBuffer, sizeof (SegmentBuffer));
  SegmentBufferIndex = 0;
  Ipv6Buffer1Index = 0;
  Ipv6Buffer2Index = 0;
  UseIpv6Buffer1 = TRUE;
  Length = StrLen (Ipv6String);
  for (Index = 0; Index < Length; ++Index) {

    switch (Ipv6String[Index]) {

    case L':':
      SegmentBuffer[SegmentBufferIndex] = 0;
      SegmentBufferIndex = 0;

      if (Index != 0 && Ipv6String[Index - 1] == L':') {
        UseIpv6Buffer1 = FALSE;
      } else {
        if (UseIpv6Buffer1) {
          Ipv6Buffer1[Ipv6Buffer1Index] = (UINT16)StrHexToUintn (SegmentBuffer);
          Ipv6Buffer1Index++;
        } else {
          Ipv6Buffer2[Ipv6Buffer2Index] = (UINT16)StrHexToUintn (SegmentBuffer);
          Ipv6Buffer2Index++;
        }
      }
      break;

    case L'.':
      //
      // Should not appear '.' character.
      //
      return FALSE;
      break;

    default:
      //
      // Each segment only has max four character.
      //
      if (SegmentBufferIndex == 4) {
        return FALSE;
      }

      SegmentBuffer[SegmentBufferIndex] = Ipv6String[Index];
      SegmentBufferIndex++;
      break;

    }

  }

  //
  // Copy to final buffer
  //
  if (HasIpv4) {
    CopyMem (&Ipv6Buffer1[6], Ipv4Buffer, 4);
  }

  if (Ipv6Buffer2Index != 0) {
    if (HasIpv4) {
      CopyMem (
        &Ipv6Buffer1[6 - Ipv6Buffer2Index],
        Ipv6Buffer2,
        Ipv6Buffer2Index * sizeof (UINT16)
        );
    } else {
      CopyMem (
        &Ipv6Buffer1[8 - Ipv6Buffer2Index],
        Ipv6Buffer2,
        Ipv6Buffer2Index * sizeof (UINT16)
        );
    }
  }

  for (Index = 0; Index < 8; ++Index) {
    Ipv6[Index * 2] = (UINT8)(Ipv6Buffer1[Index] >> 8);
    Ipv6[Index * 2 + 1] = (UINT8)(Ipv6Buffer1[Index] & 0xFF);
  }

  return TRUE;

}


//
// The Microsoft compiler inlines memset and we can not stop it.
// These routines allow the code to link!
//
//void * memcpy (void *dest, const void *src, unsigned int count)
//{
//  return CopyMem (dest, src, (UINTN)count);
//}
//
//void * memset (void *dest, char ch, unsigned int count)
//{
//  return SetMem(dest, (UINTN)count, (UINT8)ch);
//}

