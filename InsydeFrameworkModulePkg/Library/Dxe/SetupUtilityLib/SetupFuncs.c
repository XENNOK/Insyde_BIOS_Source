//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "SetupUtilityLibCommon.h"
#include "ChipsetLib.h"

CHAR16 *
SetupUtilityLibGetTokenString (
  IN EFI_HII_HANDLE                             HiiHandle,
  IN STRING_REF                                 Token
  )
/*++

Routine Description:

  Given a token, return the string.

Arguments:

  Token - the string reference
  HiiHandle - the handle the token is located

Returns:

  *CHAR16 - Returns the string corresponding to the token

--*/
{
  CHAR16                                      *Buffer;
  UINTN                                       BufferLength;
  EFI_STATUS                                  Status;
  EFI_HII_PROTOCOL                            *Hii;
  SETUP_UTILITY_BROWSER_DATA                  *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  Hii          = SuBrowser->Hii;

  BufferLength = 0x200;
  Buffer = EfiLibAllocateZeroPool (BufferLength);
  if (Buffer == NULL) {
    return NULL;
  }
  Status = Hii->GetString (
                  Hii,
                  HiiHandle,
                  Token,
                  TRUE,
                  NULL,
                  &BufferLength,
                  Buffer
                  );

  if (Status == EFI_BUFFER_TOO_SMALL) {
    //
    // Free the old pool
    //
    gBS->FreePool (Buffer);

    //
    // Allocate new pool with correct value
    //
    Buffer = EfiLibAllocateZeroPool (BufferLength);
    if (Buffer == NULL) {
      return NULL;
    }

    Status = Hii->GetString (
                    Hii,
                    HiiHandle,
                    Token,
                    TRUE,
                    NULL,
                    &BufferLength,
                    Buffer
                    );
  }

  return Buffer;
}


CHAR16 *
SetupUtilityLibGetTokenStringByLanguage (
  IN EFI_HII_HANDLE                             HiiHandle,
  IN STRING_REF                                 Token,
  IN CHAR16                                     *LanguageString
  )
/*++

Routine Description:

  Given a token, return the string.

Arguments:

  Token           - the string reference
  HiiHandle       - the handle the token is located
  LanguageString  - indicate what language string we want to get. if this is a
                    NULL pointer, using the current language setting to get string

Returns:

  *CHAR16 - Returns the string corresponding to the token
  NULL    - Cannot get string from Hii database

--*/
{
  CHAR16                                      *Buffer;
  UINTN                                       BufferLength;
  EFI_STATUS                                  Status;
  EFI_HII_PROTOCOL                            *Hii;
  SETUP_UTILITY_BROWSER_DATA                  *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  Hii          = SuBrowser->Hii;

  BufferLength = 0x200;
  Buffer = EfiLibAllocateZeroPool (BufferLength);
  if (Buffer == NULL) {
    return NULL;
  }
  Status = Hii->GetString (
                  Hii,
                  HiiHandle,
                  Token,
                  TRUE,
                  LanguageString,
                  &BufferLength,
                  Buffer
                  );

  if (Status == EFI_BUFFER_TOO_SMALL) {
    //
    // Free the old pool
    //
    gBS->FreePool (Buffer);

    //
    // Allocate new pool with correct value
    //
    Buffer = EfiLibAllocateZeroPool (BufferLength);
    if (Buffer == NULL) {
      return NULL;
    }

    Status = Hii->GetString (
                    Hii,
                    HiiHandle,
                    Token,
                    TRUE,
                    LanguageString,
                    &BufferLength,
                    Buffer
                    );
  }

  return Buffer;
}

VOID *
SetupUtilityLibGetVariableAndSize (
  IN  CHAR16              *Name,
  IN  EFI_GUID            *VendorGuid,
  OUT UINTN               *VariableSize
  )
/*++

Routine Description:

  Read the EFI variable (VendorGuid/Name) and return a dynamically allocated
  buffer, and the size of the buffer. If failure return NULL.

Arguments:

  Name       - String part of EFI variable name

  VendorGuid - GUID part of EFI variable name

  VariableSize - Returns the size of the EFI variable that was read

Returns:

  Dynamically allocated memory that contains a copy of the EFI variable.
  Caller is responsible freeing the buffer.

  NULL - Variable was not read

--*/
{
  EFI_STATUS  Status;
  UINTN       BufferSize;
  VOID        *Buffer;

  Buffer = NULL;

  //
  // Pass in a zero size buffer to find the required buffer size.
  //
  BufferSize  = 0;

  Status      = gRT->GetVariable (Name, VendorGuid, NULL, &BufferSize, Buffer);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    //
    // Allocate the buffer to return
    //

    Buffer = EfiLibAllocateZeroPool (BufferSize);
    if (Buffer == NULL) {
      return NULL;
    }

    //
    // Read variable into the allocated buffer.
    //

    Status = gRT->GetVariable (Name, VendorGuid, NULL, &BufferSize, Buffer);
    if (EFI_ERROR (Status)) {
      BufferSize = 0;
      gBS->FreePool (Buffer);
      Buffer = NULL;
    }
  }

  *VariableSize = BufferSize;

  return Buffer;
}



EFI_STATUS
SetupUtilityLibGetLangDatabase (
  OUT UINTN                             *LangNumber,
  OUT UINT16                            **LanguageString
  )
/*++

Routine Description:

  Get supported language database

Arguments:

  LangNumber       - Supported language number

  LanguageString   - Supported language strings


Returns:

  EFI_SUCCESS           - Get language data base successful
  EFI_NOT_FOUND         - Cannot find L"LangCodes" variable
  EFI_OUT_OF_RESOURCES  - Locate memory failed

--*/
{
  UINTN                                 BufferSize;
  UINTN                                 Index;
  UINT8                                 *LangBuffer;

  LangBuffer = NULL;

  //
  // Collect the languages from what our current Language support is based on our VFR
  //
  *LangNumber = 0;
  *LanguageString = NULL;
  BufferSize = 0;
  LangBuffer = SetupUtilityLibGetVariableAndSize (
                 L"LangCodes",
                 &gEfiGlobalVariableGuid,
                 &BufferSize
                 );

  if (LangBuffer == NULL) {
    return EFI_NOT_FOUND;
  }

  *LanguageString = EfiLibAllocateZeroPool ((BufferSize + 1) * 2);
  if (*LanguageString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < BufferSize; Index++) {
    (*LanguageString)[Index] = LangBuffer[Index];
  }
  (*LanguageString)[Index] = 0;

  *LangNumber = (UINTN) (BufferSize / 3);
  if (LangBuffer != NULL) {
    gBS->FreePool(LangBuffer);
  }
  return  EFI_SUCCESS;
}

EFI_STATUS
GetSetupUtilityBrowserData (
  OUT SETUP_UTILITY_BROWSER_DATA    **SuBrowser
  )
/*++

Routine Description:

  Get SETUP_UTILITY_BROWSER_DATA instance from gEfiSetupUtilityBrowserProtocolGuid

Arguments:

  SuBrowser       - double pointer point to SETUP_UTILITY_BROWSER_DATA instance

Returns:

  EFI_SUCCESS      - Get SETUP_UTILITY_BROWSER_DATA instance successful
  Other            - Cannot locate gEfiSetupUtilityBrowserProtocolGuid

--*/
{
  EFI_STATUS                                Status;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL        *Interface;


  Status = gBS->LocateProtocol (
               &gEfiSetupUtilityBrowserProtocolGuid,
               NULL,
               &Interface
               );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  *SuBrowser = EFI_SETUP_UTILITY_BROWSER_FROM_THIS (Interface);

  return Status;;
}

EFI_STATUS
SetupUtilityLibUpdateDeviceString (
  IN EFI_HII_HANDLE                         HiiHandle,
  IN SYSTEM_CONFIGURATION                   *SetupVariable
  )
/*++

Routine Description:

  Update all of device relative stirngs

Arguments:

  HiiHandle       - Specific HII handle for Boot menu
  SetupVariable   - Pointer to SYSTEM_CONFIGURATION instance

Returns:

  EFI_SUCCESS    - Update device relative strings successful
  Other          - Update device relative strings failed

--*/
{
  EFI_STATUS                                Status;
  UINTN                                     Index;
  UINTN                                     Index1;
  STRING_REF                                Token;
  UINT16                                    AdvBootDevCount;
  UINT16                                    LegacyAdvBootDevCount;
  UINT16                                    EfiBootDevCount;
  STRING_PTR                                *UpdateAdvBootString;
  STRING_PTR                                *UpdateLegacyAdvBootString;
  STRING_PTR                                *UpdateEfiBootString;
  STRING_PTR                                *UpdateBootTypeOrderString;
  STRING_PTR                                *TempString;
  UINT16                                    TableTypeCount;
  UINT16                                    *TempBootType;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  EFI_HII_PROTOCOL                          *Hii;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (SuBrowser->SUCInfo->AdvBootDeviceNum == 0) {
    return EFI_SUCCESS;
  }

  Status = GetBbsName (
             HiiHandle,
             SetupVariable,
             &UpdateBootTypeOrderString,
             &UpdateAdvBootString,
             &UpdateLegacyAdvBootString,
             &UpdateEfiBootString
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Hii                   = SuBrowser->Hii;
  SUCInfo               = SuBrowser->SUCInfo;
  TableTypeCount        = SUCInfo->LegacyBootDevTypeCount;
  TempBootType          = SUCInfo->LegacyBootDevType;
  LegacyAdvBootDevCount = SUCInfo->LegacyBootDeviceNum;
  EfiBootDevCount       = SUCInfo->EfiBootDeviceNum;
  AdvBootDevCount       = SUCInfo->AdvBootDeviceNum;

  BootOptionStrAppendDeviceName (EfiBootDevCount, UpdateEfiBootString); 
  if (TableTypeCount != 0) {
    SetupVariable->NoBootTypeOrder = 1;
  }
  Token = SUCInfo->BootTypeTokenRecord;
  for (Index = 0; Index < TableTypeCount; Index++, Token++) {
    Hii->NewString(
           Hii,
           NULL,
           HiiHandle,
           &Token,
           UpdateBootTypeOrderString[Index].pString
           );
  }

  if (LegacyAdvBootDevCount != 0) {
    SetupVariable->NoBootTypeOrder = 1;
  }
  Token = SUCInfo->LegacyAdvanceTokenRecord;
  for (Index = 0; Index < LegacyAdvBootDevCount; Index++, Token++) {
    Hii->NewString(
           Hii,
           NULL,
           HiiHandle,
           &Token,
           UpdateLegacyAdvBootString[Index].pString
           );
  }

  Token = SUCInfo->EfiTokenRecord;
  for (Index = 0; Index < EfiBootDevCount; Index++, Token++) {
    Hii->NewString(
           Hii,
           NULL,
           HiiHandle,
           &Token,
           UpdateEfiBootString[Index].pString
           );
  }

  if (AdvBootDevCount != 0) {
    SetupVariable->NoAdvBootDev = 1;
  }
  Token = SUCInfo->AdvanceTokenRecord;
  for (Index = 0; Index < AdvBootDevCount; Index++, Token++) {
    Hii->NewString(
           Hii,
           NULL,
           HiiHandle,
           &Token,
           UpdateAdvBootString[Index].pString
           );
  }

  for(Index = 0; Index < TableTypeCount; Index++) {
    if (mBbsTypeTable[TempBootType[Index]].DeviceTypeCount != 0) {
      SetupVariable->NoBootDevs[TempBootType[Index]]= 1;
    }
    Token = SUCInfo->LegacyNormalTokenRecord[TempBootType[Index]];
    TempString = (STRING_PTR *) mBbsTypeTable[TempBootType[Index]].StringPtr;
    for (Index1 = 0; Index1 < (UINT16) mBbsTypeTable[TempBootType[Index]].DeviceTypeCount; Index1++, Token++) {
      Hii->NewString (
             Hii,
             NULL,
             HiiHandle,
             &Token,
             TempString[Index1].pString
             );
    }
  }
  for (Index = 0; Index < MAX_BOOT_ORDER_NUMBER; Index++) {
    if (UpdateBootTypeOrderString[Index].pString != NULL) {
      gBS->FreePool (UpdateBootTypeOrderString[Index].pString);
    }
  }
  gBS->FreePool (UpdateBootTypeOrderString);
  gBS->FreePool (UpdateLegacyAdvBootString);
  gBS->FreePool (UpdateEfiBootString);
  for (Index = 0; Index < SuBrowser->SUCInfo->AdvBootDeviceNum; Index++) {
    if (UpdateAdvBootString[Index].BbsEntry != NULL) {
      gBS->FreePool (UpdateAdvBootString[Index].BbsEntry);
    }
    if (UpdateAdvBootString[Index].pString != NULL) {
      gBS->FreePool (UpdateAdvBootString[Index].pString);
    }
  }
  gBS->FreePool (UpdateAdvBootString);

  return EFI_SUCCESS;
}

EFI_STATUS
SetupUtilityLibAsciiToUnicode (
  IN    CHAR8     *AsciiString,
  IN    CHAR16    *UnicodeString,
  IN    UINTN     Length
  )
/*++

Routine Description:

  Convert ASCII string to Unicode string in fixed length.

Arguments:

  AsciiStr        - Input ASCII string
  UnicodeStr      - Output Unicode string
  AsciiStrLen     - The string length of ASCII

Returns:

--*/
{
  UINT8           Index;

  for (Index = 0; Index < Length; Index++) {
    UnicodeString[Index] = (CHAR16) AsciiString[Index];
  }

  return EFI_SUCCESS;
}

EFI_STATUS
SetupUtilityLibUpdateAtaString(
  IN      EFI_IDENTIFY_DATA                 *IdentifyDriveInfo,
  IN      UINTN                             BufSize,
  IN OUT  CHAR16                            **BootString
  )
/*++

Routine Description:
  Function to update the ATA strings into Model Name -- Size

Arguments:

Returns:
  Will return model name and size (or ATAPI if non-ATA)
--*/
{
  CHAR8                                     *TempString;
  UINT16                                    Index;
  CHAR8                                     Temp8;

  TempString = EfiLibAllocateZeroPool (0x100);

  EfiCopyMem (
    TempString,
    IdentifyDriveInfo->AtapiData.ModelName,
    sizeof(IdentifyDriveInfo->AtapiData.ModelName)
    );

  //
  // Swap the IDE string since Identify Drive format is inverted
  //
  Index = 0;
  while (TempString[Index] != 0 && TempString[Index+1] != 0) {
    Temp8 = TempString[Index];
    TempString[Index] = TempString[Index+1];
    TempString[Index+1] = Temp8;
    Index +=2;
  }
  SetupUtilityLibAsciiToUnicode (TempString, *BootString, (UINTN) Index);
  if (TempString != NULL) {
    gBS->FreePool(TempString);
  }

  return EFI_SUCCESS;
}
