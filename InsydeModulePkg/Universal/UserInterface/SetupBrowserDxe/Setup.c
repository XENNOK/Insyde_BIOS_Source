/** @file
 Entry and initial functions for SetupBrowserDxe driver

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

#include "Setup.h"
#include "Ui.h"
#include "SetupUtility.h"

#include <Protocol/ConsoleRedirectionService.h>
#include <Protocol/EndOfDisableQuietBoot.h>

#include <Library/OemGraphicsLib.h>

#define EFI_CURRENT_VGA_MODE_ADDRESS  0x4A3
#define CURRENT_VGA_MODE_ADDRESS      0x449
#define DEFAULT_HORIZONTAL_RESOLUTION 800
#define DEFAULT_VERTICAL_RESOLUTION   600

#pragma pack(push, 1)
typedef struct {
  UINT8  Id[4];
  UINT8  HIIVersion;
  UINT32 EZH2OVersion;
  UINT32 StringPackageoffset;
  UINT32 Reserved1;
  UINT32 StringPackageTotalSizeOffset;
  UINT32 Reserved2;
} EFI_OBS_HEAD;
#pragma pack(pop)

#ifndef __GNUC__
EFI_OBS_HEAD     OBS_HEADER =
                   { {'$', 'O', 'B', 'S'},
                     0x21,
                     0x02,
                     (UINT32)(UINTN) SetupBrowserStrings,
                     0,
                     (UINT32)(UINTN) &SetupBrowserStringsTotalSize,
                     0
                   };
#endif

SETUP_DRIVER_PRIVATE_DATA  mPrivateData = {
  SETUP_DRIVER_SIGNATURE,
  NULL,
  {
    SendForm,
    BrowserCallback
  },
  {
    SetScope,
    RegisterHotKey,
    RegiserExitHandler,
    SaveReminder
  }
};

EFI_HII_DATABASE_PROTOCOL         *mHiiDatabase;
EFI_HII_STRING_PROTOCOL           *mHiiString;
EFI_HII_CONFIG_ROUTING_PROTOCOL   *mHiiConfigRouting;


UINTN           gBrowserContextCount = 0;
LIST_ENTRY      gBrowserContextList = INITIALIZE_LIST_HEAD_VARIABLE (gBrowserContextList);
LIST_ENTRY      gBrowserFormSetList = INITIALIZE_LIST_HEAD_VARIABLE (gBrowserFormSetList);
LIST_ENTRY      gBrowserHotKeyList  = INITIALIZE_LIST_HEAD_VARIABLE (gBrowserHotKeyList);

BANNER_DATA           *gBannerData;
EFI_HII_HANDLE        gFrontPageHandle;
UINTN                 gClassOfVfr;
UINTN                 gFunctionKeySetting;
BOOLEAN               gResetRequired;
BOOLEAN               gExitRequired;
BOOLEAN               gNvUpdateRequired;
EFI_HII_HANDLE        gHiiHandle;
UINT16                gDirection;
EFI_SCREEN_DESCRIPTOR gScreenDimensions;
EFI_HII_ANIMATION_PACKAGE_HDR *gAnimationPackage;
BOOLEAN               gGraphicsEnabled;
UINTN                 gFooterHeight;
BROWSER_SETTING_SCOPE gBrowserSettingScope  = FormSetLevel;
BOOLEAN               mBrowserScopeFirstSet = TRUE;
EXIT_HANDLER          ExitHandlerFunction   = NULL;

//
// Browser Global Strings
//
CHAR16            *gFunctionNineString;
CHAR16            *gFunctionTenString;
CHAR16            *gEnterString;
CHAR16            *gEnterCommitString;
CHAR16            *gEnterEscapeString;
CHAR16            *gEscapeString;
CHAR16            *gSaveFailed;
CHAR16            *gMoveHighlight;
CHAR16            *gMakeSelection;
CHAR16            *gDecNumericInput;
CHAR16            *gHexNumericInput;
CHAR16            *gToggleCheckBox;
CHAR16            *gPromptForData;
CHAR16            *gPromptForPassword;
CHAR16            *gPromptForNewPassword;
CHAR16            *gConfirmPassword;
CHAR16            *gConfirmError;
CHAR16            *gPassowordInvalid;
CHAR16            *gSamePasswordError;
CHAR16            *gPressEnter;
CHAR16            *gEmptyString;
CHAR16            *gAreYouSure;
CHAR16            *gYesResponse;
CHAR16            *gNoResponse;
CHAR16            *gMiniString;
CHAR16            *gPlusString;
CHAR16            *gMinusString;
CHAR16            *gAdjustNumber;
CHAR16            *gSaveChanges;
CHAR16            *gOptionMismatch;
CHAR16            *gSetupWarning;
CHAR16            *gSetupNotice;
CHAR16            *gChangesSaved;
CHAR16            *gContinue;
CHAR16            *gPasswordChangesSaved;
CHAR16            gPromptBlockWidth;
CHAR16            gOptionBlockWidth;
CHAR16            gHelpBlockWidth;
CHAR16            *mUnknownString = L"!";

EFI_GUID  gZeroGuid               = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
EFI_GUID  gSetupBrowserGuid       = {0xab368524, 0xb60c, 0x495b, 0xa0, 0x9, 0x12, 0xe8, 0x5b, 0x1a, 0xea, 0x32};
EFI_GUID  gPlatformSetupClassGuid = EFI_HII_PLATFORM_SETUP_FORMSET_GUID;
EFI_GUID  gFrontPageClassGuid     = {0x9e0c30bc, 0x3f06, 0x4ba6, 0x82, 0x88, 0x9 , 0x17, 0x9b, 0x85, 0x5d, 0xbe};
EFI_GUID  gFrontPageFormsetGuid   = {0x9e0c30bc, 0x3f06, 0x4ba6, 0x82, 0x88, 0x9 , 0x17, 0x9b, 0x85, 0x5d, 0xbe};
EFI_GUID  gSecureBootFormsetGuid  = {0xaa1305b9, 0x1f3 , 0x4afb, 0x92, 0xe , 0xc9, 0xb9, 0x79, 0xa8, 0x52, 0xfd};
BOOLEAN   gSecureBootMgr;

FORM_BROWSER_FORMSET  *gOldFormSet;

FUNCTIION_KEY_SETTING gFunctionKeySettingTable[] = {
  //
  // Boot Manager
  //
  {
    {
      0x847bc3fe,
      0xb974,
      0x446d,
      0x94,
      0x49,
      0x5a,
      0xd5,
      0x41,
      0x2e,
      0x99,
      0x3b
    },
    NONE_FUNCTION_KEY_SETTING
  },
  //
  // Device Manager
  //
  {
    {
      0x3ebfa8e6,
      0x511d,
      0x4b5b,
      0xa9,
      0x5f,
      0xfb,
      0x38,
      0x26,
      0xf,
      0x1c,
      0x27
    },
    NONE_FUNCTION_KEY_SETTING
  },
  //
  // BMM FormSet.
  //
  {
    {
      0x642237c7,
      0x35d4,
      0x472d,
      0x83,
      0x65,
      0x12,
      0xe0,
      0xcc,
      0xf2,
      0x7a,
      0x22
    },
    NONE_FUNCTION_KEY_SETTING
  },
  //
  // BMM File Explorer FormSet.
  //
  {
    {
      0x1f2d63e1,
      0xfebd,
      0x4dc7,
      0x9c,
      0xc5,
      0xba,
      0x2b,
      0x1c,
      0xef,
      0x9c,
      0x5b
    },
    NONE_FUNCTION_KEY_SETTING
  },
};

HII_VENDOR_DEVICE_PATH  mSetupBrowserHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    { 0x2a3db560, 0xd8ea, 0x4cf6, { 0xb7, 0x4b, 0xd0, 0x69, 0x49, 0x86, 0xfd, 0x6d } }
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};

/**
 Determines if a Unicode character is a hexadecimal digit.
 The test is case insensitive.

 @param[out] Digit              Pointer to byte that receives the value of the hex character.
 @param[in]  Char               Unicode character to test.

 @retval TRUE                   If the character is a hexadecimal digit.
 @retval FALSE                  Otherwise.
**/
BOOLEAN
IsHexDigit (
  OUT UINT8      *Digit,
  IN  CHAR16      Char
  )
{
  if ((Char >= L'0') && (Char <= L'9')) {
    *Digit = (UINT8) (Char - L'0');
    return TRUE;
  }

  if ((Char >= L'A') && (Char <= L'F')) {
    *Digit = (UINT8) (Char - L'A' + 0x0A);
    return TRUE;
  }

  if ((Char >= L'a') && (Char <= L'f')) {
    *Digit = (UINT8) (Char - L'a' + 0x0A);
    return TRUE;
  }

  return FALSE;
}

/**
 Converts Unicode string to binary buffer.
 The conversion may be partial.
 The first character in the string that is not hex digit stops the conversion.
 At a minimum, any blob of data could be represented as a hex string.

 @param[in, out] Buf               Pointer to buffer that receives the data.
 @param[in, out] Len               Length in bytes of the buffer to hold converted data.
                                   If routine return with EFI_SUCCESS, containing length of converted data.
                                   If routine return with EFI_BUFFER_TOO_SMALL, containg length of buffer desired.
 @param[in]      Str               String to be converted from.
 @param[out]     ConvertedStrLen   Length of the Hex String consumed.

 @retval EFI_SUCCESS               Routine Success.
 @retval EFI_BUFFER_TOO_SMALL      The buffer is too small to hold converted data.
**/
EFI_STATUS
HexStringToBuf (
  IN OUT UINT8                     *Buf,
  IN OUT UINTN                    *Len,
  IN     CHAR16                    *Str,
  OUT    UINTN                     *ConvertedStrLen  OPTIONAL
  )
{
  UINTN       HexCnt;
  UINTN       Idx;
  UINTN       BufferLength;
  UINT8       Digit;
  UINT8       Byte;

  //
  // Find out how many hex characters the string has.
  //
  for (Idx = 0, HexCnt = 0; IsHexDigit (&Digit, Str[Idx]); Idx++, HexCnt++);

  if (HexCnt == 0) {
    *Len = 0;
    return EFI_SUCCESS;
  }
  //
  // Two Unicode characters make up 1 buffer byte. Round up.
  //
  BufferLength = (HexCnt + 1) / 2;

  //
  // Test if  buffer is passed enough.
  //
  if (BufferLength > (*Len)) {
    *Len = BufferLength;
    return EFI_BUFFER_TOO_SMALL;
  }

  *Len = BufferLength;

  for (Idx = 0; Idx < HexCnt; Idx++) {
    IsHexDigit (&Digit, Str[HexCnt - 1 - Idx]);

    //
    // For odd charaters, write the lower nibble for each buffer byte,
    // and for even characters, the upper nibble.
    //
    if ((Idx & 1) == 0) {
      Byte = Digit;
    } else {
      Byte = Buf[Idx / 2];
      Byte &= 0x0F;
      Byte = (UINT8)(Byte | (Digit << 4));
    }

    Buf[Idx / 2] = Byte;
  }

  if (ConvertedStrLen != NULL) {
    *ConvertedStrLen = HexCnt;
  }

  return EFI_SUCCESS;
}

/**
 Convert binary representation Config string (e.g. "0041004200430044") to the
 original string (e.g. "ABCD"). Config string appears in <ConfigHdr> (i.e.
 "&NAME=<string>"), or Name/Value pair in <ConfigBody> (i.e. "label=<string>").

 @param[in, out] UnicodeString    Original Unicode string.
 @param[in, out] StrBufferLen     On input: Length in bytes of buffer to hold the Unicode string.
                                            Includes tailing '\0' character.
                                  On output: If return EFI_SUCCESS, containing length of Unicode string buffer.
                                             If return EFI_BUFFER_TOO_SMALL, containg length of string buffer desired.
 @param[in]      ConfigString     Binary representation of Unicode String, <string> := (<HexCh>4)+

 @retval  EFI_SUCCESS            Routine Success.
 @retval  EFI_BUFFER_TOO_SMALL   The string buffer is too small.
**/
EFI_STATUS
ConfigStringToUnicode (
  IN OUT CHAR16                *UnicodeString,
  IN OUT UINTN                 *StrBufferLen,
  IN CHAR16                    *ConfigString
  )
{
  UINTN       Index;
  UINTN       Len;
  UINTN       BufferSize;
  CHAR16      BackupChar;

  Len = StrLen (ConfigString) / 4;
  BufferSize = (Len + 1) * sizeof (CHAR16);

  if (*StrBufferLen < BufferSize) {
    *StrBufferLen = BufferSize;
    return EFI_BUFFER_TOO_SMALL;
  }

  *StrBufferLen = BufferSize;

  for (Index = 0; Index < Len; Index++) {
    BackupChar = ConfigString[4];
    ConfigString[4] = L'\0';

    HexStringToBuf ((UINT8 *) UnicodeString, &BufferSize, ConfigString, NULL);

    ConfigString[4] = BackupChar;

    ConfigString += 4;
    UnicodeString += 1;
  }

  //
  // Add tailing '\0' character
  //
  *UnicodeString = L'\0';

  return EFI_SUCCESS;
}

/**
 Converts the low nibble of a byte  to hex unicode character.

 @param[in] Nibble                 lower nibble of a byte.

 @return Hex unicode character.
**/
CHAR16
NibbleToHexChar (
  IN UINT8      Nibble
  )
{
  Nibble &= 0x0F;
  if (Nibble <= 0x9) {
    return (CHAR16)(Nibble + L'0');
  }

  return (CHAR16)(Nibble - 0xA + L'A');
}

/**
 Converts binary buffer to Unicode string.
 At a minimum, any blob of data could be represented as a hex string.

 @param[in, out] Str                     Pointer to the string.
 @param[in, out] HexStringBufferLength   Length in bytes of buffer to hold the hex string.
                                         Includes tailing '\0' character.
                                         If routine return with EFI_SUCCESS, containing length of hex string buffer.
                                         If routine return with EFI_BUFFER_TOO_SMALL, containg length of hex string
                                         buffer desired.
 @param[in]      Buf                     Buffer to be converted from.
 @param[in]      Len                     Length in bytes of the buffer to be converted.

 @retval EFI_SUCCESS            Routine Success.
 @retval EFI_BUFFER_TOO_SMALL   The string buffer is too small.
**/
EFI_STATUS
BufToHexString (
  IN OUT CHAR16                    *Str,
  IN OUT UINTN                     *HexStringBufferLength,
  IN     UINT8                     *Buf,
  IN     UINTN                      Len
  )
{
  UINTN       Idx;
  UINT8       Byte;
  UINTN       StrLen;

  //
  // Make sure string is either passed or allocate enough.
  // It takes 2 Unicode characters (4 bytes) to represent 1 byte of the binary buffer.
  // Plus the Unicode termination character.
  //
  StrLen = Len * 2;
  if (StrLen > ((*HexStringBufferLength) - 1)) {
    *HexStringBufferLength = StrLen + 1;
    return EFI_BUFFER_TOO_SMALL;
  }

  *HexStringBufferLength = StrLen + 1;
  //
  // Ends the string.
  //
  Str[StrLen] = L'\0';

  for (Idx = 0; Idx < Len; Idx++) {

    Byte = Buf[Idx];
    Str[StrLen - 1 - Idx * 2] = NibbleToHexChar (Byte);
    Str[StrLen - 2 - Idx * 2] = NibbleToHexChar ((UINT8)(Byte >> 4));
  }

  return EFI_SUCCESS;
}

/**
 Converts the unicode character of the string from uppercase to lowercase.

 @param[in, out] Str                     String to be converted
**/
VOID
ToLower (
  IN OUT CHAR16    *Str
  )
{
  CHAR16      *Ptr;

  for (Ptr = Str; *Ptr != L'\0'; Ptr++) {
    if (*Ptr >= L'A' && *Ptr <= L'Z') {
      *Ptr = (CHAR16) (*Ptr - L'A' + L'a');
    }
  }
}

/**
 Convert Unicode string to binary representation Config string, e.g.
 "ABCD" => "0041004200430044". Config string appears in <ConfigHdr> (i.e.
 "&NAME=<string>"), or Name/Value pair in <ConfigBody> (i.e. "label=<string>").

 @param[in, out] ConfigString   Binary representation of Unicode String, <string> := (<HexCh>4)+
 @param[in, out] StrBufferLen   On input: Length in bytes of buffer to hold the Unicode string.
                                          Includes tailing '\0' character.
                                On output:
                                  If return EFI_SUCCESS, containing length of Unicode string buffer.
                                  If return EFI_BUFFER_TOO_SMALL, containg length of string buffer desired.
 @param[in]      UnicodeString  Original Unicode string.

 @retval EFI_SUCCESS            Routine Success.
 @retval EFI_BUFFER_TOO_SMALL   The string buffer is too small.
**/
EFI_STATUS
UnicodeToConfigString (
  IN OUT CHAR16                *ConfigString,
  IN OUT UINTN                 *StrBufferLen,
  IN CHAR16                    *UnicodeString
  )
{
  UINTN       Index;
  UINTN       Len;
  UINTN       BufferSize;
  CHAR16      *String;

  Len = StrLen (UnicodeString);
  BufferSize = (Len * 4 + 1) * sizeof (CHAR16);

  if (*StrBufferLen < BufferSize) {
    *StrBufferLen = BufferSize;
    return EFI_BUFFER_TOO_SMALL;
  }

  *StrBufferLen = BufferSize;
  String        = ConfigString;

  for (Index = 0; Index < Len; Index++) {
    BufToHexString (ConfigString, &BufferSize, (UINT8 *) UnicodeString, 2);

    ConfigString += 4;
    UnicodeString += 1;
  }

  //
  // Add tailing '\0' character
  //
  *ConfigString = L'\0';

  //
  // Convert the uppercase to lowercase since <HexAf> is defined in lowercase format.
  //
  ToLower (String);
  return EFI_SUCCESS;
}

/**
 This is the routine which an external caller uses to direct the browser
 where to obtain it's information.

 @param[in]  This                The Form Browser protocol instanse.
 @param[in]  Handles             A pointer to an array of Handles.  If HandleCount > 1 we
                                 display a list of the formsets for the handles specified.
 @param[in]  HandleCount         The number of Handles specified in Handle.
 @param[in]  FormSetGuid         This field points to the EFI_GUID which must match the Guid
                                 field in the EFI_IFR_FORM_SET op-code for the specified
                                 forms-based package. If FormSetGuid is NULL, then this
                                 function will display the first found forms package.
 @param[in]  FormId              This field specifies which EFI_IFR_FORM to render as the first
                                 displayable page. If this field has a value of 0x0000, then
                                 the forms browser will render the specified forms in their encoded order.
 @param[in]  ScreenDimensions    Points to recommended form dimensions, including any non-content area, in characters.
 @param[out] ActionRequest       Points to the action recommended by the form.

 @retval EFI_SUCCESS             The function completed successfully.
 @retval EFI_INVALID_PARAMETER   One of the parameters has an invalid value.
 @retval EFI_NOT_FOUND           No valid forms could be found to display.
**/
EFI_STATUS
EFIAPI
SendForm (
  IN  CONST EFI_FORM_BROWSER2_PROTOCOL *This,
  IN  EFI_HII_HANDLE                   *Handles,
  IN  UINTN                            HandleCount,
  IN  EFI_GUID                         *FormSetGuid, OPTIONAL
  IN  UINT16                           FormId, OPTIONAL
  IN  CONST EFI_SCREEN_DESCRIPTOR      *ScreenDimensions, OPTIONAL
  OUT EFI_BROWSER_ACTION_REQUEST       *ActionRequest  OPTIONAL
  )
{
  EFI_STATUS                               Status;
  UI_MENU_SELECTION                        *Selection;
  UINTN                                    Index;
  FORM_BROWSER_FORMSET                     *FormSet;
  LIST_ENTRY                               *Link;
  EFI_HII_CONFIG_ACCESS_PROTOCOL           *ConfigAccess;
  EFI_BROWSER_ACTION_REQUEST               DummyActionRequest;
  EFI_BROWSER_ACTION_REQUEST               WorkingActionRequest;
  EFI_CONSOLE_CONTROL_PROTOCOL             *ConsoleControl;
  EFI_GRAPHICS_OUTPUT_PROTOCOL             *Gop;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL            BackgroundColor = { 0, 0, 0, 0 };
  EFI_SETUP_MOUSE_PROTOCOL                 *LocalSetupMouse;
  EFI_CONSOLE_REDIRECTION_SERVICE_PROTOCOL *CRService;

  mBootManager  = FALSE;
  mBootFromFile = FALSE;
  LocalSetupMouse = NULL;
  ConsoleControl = NULL;

  //
  // Calculate total number of Register HotKeys.
  //
  Index = 0;
  Link  = GetFirstNode (&gBrowserHotKeyList);
  while (!IsNull (&gBrowserHotKeyList, Link)) {
    Link = GetNextNode (&gBrowserHotKeyList, Link);
    Index ++;
  }

  //
  // Show three HotKeys help information on one ROW.
  //
  gFooterHeight = FOOTER_HEIGHT + (Index / 3);

  CheckDeviceManagerSetup ();

  //
  // Save globals used by SendForm()
  //
  SaveBrowserContext ();

  //
  // Ensure we are in Text mode
  //
  if ((gDeviceManagerSetup == NULL) ||
      ((gDeviceManagerSetup != NULL) && (gDeviceManagerSetup->Firstin == TRUE))) {
    gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_LIGHTGRAY, EFI_BLACK));
    DisableQuietBoot ();
  }

  gResetRequired = FALSE;
  Status = EFI_SUCCESS;
  ZeroMem (&gScreenDimensions, sizeof (EFI_SCREEN_DESCRIPTOR));

  //
  // Seed the dimensions in the global
  //
  gST->ConOut->QueryMode (
                 gST->ConOut,
                 gST->ConOut->Mode->Mode,
                 &gScreenDimensions.RightColumn,
                 &gScreenDimensions.BottomRow
                 );

  if (ScreenDimensions != NULL) {
    //
    // Check local dimension vs. global dimension.
    //
    if ((gScreenDimensions.RightColumn < ScreenDimensions->RightColumn) ||
        (gScreenDimensions.BottomRow < ScreenDimensions->BottomRow)
        ) {
      Status = EFI_INVALID_PARAMETER;
      goto Done;
    } else {
      //
      // Local dimension validation.
      //
      if ((ScreenDimensions->RightColumn > ScreenDimensions->LeftColumn) &&
          (ScreenDimensions->BottomRow > ScreenDimensions->TopRow) &&
          ((ScreenDimensions->RightColumn - ScreenDimensions->LeftColumn) > 2) &&
          (
            (ScreenDimensions->BottomRow - ScreenDimensions->TopRow) > STATUS_BAR_HEIGHT +
            SCROLL_ARROW_HEIGHT *
            2 +
            FRONT_PAGE_HEADER_HEIGHT +
            FOOTER_HEIGHT +
            1
          )
        ) {
        CopyMem (&gScreenDimensions, (VOID *) ScreenDimensions, sizeof (EFI_SCREEN_DESCRIPTOR));
      } else {
        Status = EFI_INVALID_PARAMETER;
        goto Done;
      }
    }
  }

  gOptionBlockWidth = (CHAR16) ((gScreenDimensions.RightColumn - gScreenDimensions.LeftColumn) / 3);
  gHelpBlockWidth   = gOptionBlockWidth;
  gPromptBlockWidth = gOptionBlockWidth;
  gHelpBlockWidth = gHelpBlockWidth + ((CHAR16) ((gScreenDimensions.RightColumn - gScreenDimensions.LeftColumn) % 3));
  gHelpBlockWidth = gHelpBlockWidth - (80 % 3);
  //
  // Initialize the strings for the browser, upon exit of the browser, the strings will be freed
  //
  InitializeBrowserStrings ();

  gFunctionKeySetting = DEFAULT_FUNCTION_KEY_SETTING;


  for (Index = 0; Index < HandleCount; Index++) {
    Selection = AllocateZeroPool (sizeof (UI_MENU_SELECTION));
    ASSERT (Selection != NULL);
    if  (Selection == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    Selection->Handle = Handles[Index];
    if (FormSetGuid != NULL) {
      CopyMem (&Selection->FormSetGuid, FormSetGuid, sizeof (EFI_GUID));
      Selection->FormId = FormId;
    } else {
      CopyMem (&Selection->FormSetGuid, &gEfiHiiPlatformSetupFormsetGuid, sizeof (EFI_GUID));
    }

    gOldFormSet = NULL;
    gNvUpdateRequired = FALSE;

    do {
      //
      // locate animation package
      //
      Status = GetAnimationPackage (Selection->Handle, &gAnimationPackage);
      if (EFI_ERROR (Status)) {
        gAnimationPackage = NULL;
      }

      FormSet = AllocateZeroPool (sizeof (FORM_BROWSER_FORMSET));
      ASSERT (FormSet != NULL);
      if (FormSet == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      //
      // Initialize internal data structures of FormSet
      //
      Status = InitializeFormSet (Selection->Handle, &Selection->FormSetGuid, FormSet);
      if (EFI_ERROR (Status) || IsListEmpty (&FormSet->FormListHead)) {
        DestroyFormSet (FormSet);
        break;
      }
      Selection->FormSet = FormSet;

      gGraphicsEnabled = FALSE;
      Status = gBS->LocateProtocol (&gConsoleRedirectionServiceProtocolGuid, NULL, (VOID **)&CRService);
      if (EFI_ERROR (Status) &&
          FormSetGuid != NULL &&
          CompareGuid (FormSetGuid, &gFrontPageFormsetGuid) &&
          gAnimationPackage != NULL) {
        gGraphicsEnabled = TRUE;
      }

      Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, NULL, (VOID **)&ConsoleControl);
      if (EFI_ERROR (Status)) {
        ConsoleControl = NULL;
      }

      if (gGraphicsEnabled) {
        ASSERT (gAnimationPackage != NULL);
        gST->ConOut->EnableCursor (gST->ConOut, FALSE);
        if (ConsoleControl != NULL) {
          ConsoleControl->SetMode (ConsoleControl, EfiConsoleControlScreenGraphics);
        }
        gBS->HandleProtocol (
               gST->ConsoleOutHandle,
               &gEfiGraphicsOutputProtocolGuid,
               (VOID **)&Gop
               );
        Gop->Blt (
               Gop,
               &BackgroundColor,
               EfiBltVideoFill,
               0, 0,
               0, 0,
               Gop->Mode->Info->HorizontalResolution,
               Gop->Mode->Info->VerticalResolution,
               0
               );
        Status = gBS->LocateProtocol (&gSetupMouseProtocolGuid, NULL, (VOID **)&LocalSetupMouse);
        if (!EFI_ERROR (Status)) {
          LocalSetupMouse->Start (LocalSetupMouse);
          LocalSetupMouse->SetMode (LocalSetupMouse, EfiSetupMouseScreenGraphics);
        } else {
          LocalSetupMouse = NULL;
        }
      }

      //
      // Initialize current settings of Questions in this FormSet
      //
      Status = InitializeCurrentSetting (FormSet);
      if (EFI_ERROR (Status)) {
        DestroyFormSet (FormSet);
        break;
      }
      //
      // Display this formset
      //
      gCurrentSelection = Selection;
      ConfigAccess = Selection->FormSet->ConfigAccess;
      if(gDeviceManagerSetup) {
        ConfigAccess->Callback(ConfigAccess, EFI_BROWSER_ACTION_CHANGING, SET_SETUP_CONFIG, 0, NULL, &DummyActionRequest);
      }
      Status = SetupBrowser (Selection);
      if(gDeviceManagerSetup) {
        ConfigAccess->Callback(ConfigAccess, EFI_BROWSER_ACTION_CHANGING, GET_SETUP_CONFIG, 0, NULL, &DummyActionRequest);
      }
      gCurrentSelection = NULL;
      if (gOldFormSet != NULL) {
        DestroyFormSet (gOldFormSet);
      }
      gOldFormSet = FormSet;

      if (EFI_ERROR (Status)) {
        break;
      }
    } while (Selection->Action == UI_ACTION_REFRESH_FORMSET);

    if (gOldFormSet != NULL) {
      DestroyFormSet (gOldFormSet);
      gOldFormSet = NULL;
    }


    gBS->FreePool (Selection);
  }


  WorkingActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  if (gResetRequired) {
    WorkingActionRequest = EFI_BROWSER_ACTION_REQUEST_RESET;
  } else if (gExitRequired) {
    WorkingActionRequest = EFI_BROWSER_ACTION_REQUEST_EXIT;
  }

  if (ActionRequest != NULL) {
    *ActionRequest = WorkingActionRequest;
  }

  FreeBrowserStrings ();

  if (gAnimationPackage != NULL) {
    gBS->FreePool (gAnimationPackage);
    gAnimationPackage = NULL;
  }

  gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_LIGHTGRAY, EFI_BLACK));
  if (gDeviceManagerSetup) {
    if (WorkingActionRequest != EFI_BROWSER_ACTION_REQUEST_NONE) {
      gST->ConOut->ClearScreen (gST->ConOut);
      gDeviceManagerSetup->Firstin = TRUE;
    }
  } else {
    gST->ConOut->ClearScreen (gST->ConOut);

  }
Done:
  gGraphicsEnabled = FALSE;
  gSetupMouse = NULL;
  gDeviceManagerSetup = NULL;

  if (LocalSetupMouse != NULL) {
    LocalSetupMouse->Close  (LocalSetupMouse);
    LocalSetupMouse = NULL;
  }
  if (ConsoleControl != NULL) {
    ConsoleControl->SetMode (ConsoleControl, EfiConsoleControlScreenText);
  }
  //
  // Restore globals used by SendForm()
  //
  RestoreBrowserContext ();

  return Status;
}

/**
 This function is called by a callback handler to retrieve uncommitted state
 data from the browser.

 @param[in]       This              A pointer to the EFI_FORM_BROWSER2_PROTOCOL instance.
 @param[in, out]  ResultsDataSize   A pointer to the size of the buffer associated with ResultsData.
 @param[in, out]  ResultsData       A string returned from an IFR browser or
                                    equivalent. The results string will have no routing information in them.
 @param[in]       RetrieveData      A BOOLEAN field which allows an agent to retrieve
                                    (if RetrieveData = TRUE) data from the uncommitted
                                    browser state information or set (if RetrieveData
                                    = FALSE) data in the uncommitted browser state information.
 @param[in]       VariableGuid      An optional field to indicate the target variable GUID name to use.
 @param[in]       VariableName      An optional field to indicate the target human-readable variable name.

 @retval EFI_SUCCESS            The results have been distributed or are awaiting distribution.
 @retval EFI_BUFFER_TOO_SMALL   The ResultsDataSize specified was too small to contain the results data.
**/
EFI_STATUS
EFIAPI
BrowserCallback (
  IN CONST EFI_FORM_BROWSER2_PROTOCOL  *This,
  IN OUT UINTN                         *ResultsDataSize,
  IN OUT EFI_STRING                    ResultsData,
  IN BOOLEAN                           RetrieveData,
  IN CONST EFI_GUID                    *VariableGuid, OPTIONAL
  IN CONST CHAR16                      *VariableName  OPTIONAL
  )
{
  EFI_STATUS            Status;
  LIST_ENTRY            *Link;
  FORMSET_STORAGE       *Storage;
  FORM_BROWSER_FORMSET  *FormSet;
  BOOLEAN               Found;
  CHAR16                *ConfigResp;
  CHAR16                *StrPtr;
  UINTN                 BufferSize;

  if (ResultsDataSize == NULL || ResultsData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (gCurrentSelection == NULL) {
    return EFI_NOT_READY;
  }

  Storage = NULL;
  ConfigResp = NULL;
  FormSet = gCurrentSelection->FormSet;

  //
  // Find target storage
  //
  Link = GetFirstNode (&FormSet->StorageListHead);
  if (IsNull (&FormSet->StorageListHead, Link)) {
    return EFI_UNSUPPORTED;
  }

  if (VariableGuid != NULL) {
    //
    // Try to find target storage
    //
    Found = FALSE;
    while (!IsNull (&FormSet->StorageListHead, Link)) {
      Storage = FORMSET_STORAGE_FROM_LINK (Link);
      Link = GetNextNode (&FormSet->StorageListHead, Link);

      if (CompareGuid (&Storage->Guid, (EFI_GUID *) VariableGuid)) {
        if (Storage->Type == EFI_HII_VARSTORE_BUFFER) {
          //
          // Buffer storage require both GUID and Name
          //
          if (VariableName == NULL) {
            return EFI_NOT_FOUND;
          }

          if (StrCmp (Storage->Name, (CHAR16 *) VariableName) != 0) {
            continue;
          }
        }
        Found = TRUE;
        break;
      }
    }

    if (!Found) {
      return EFI_NOT_FOUND;
    }
  } else {
    //
    // GUID/Name is not specified, take the first storage in FormSet
    //
    Storage = FORMSET_STORAGE_FROM_LINK (Link);
  }

  if (RetrieveData) {
    //
    // Skip if there is no RequestElement
    //
    if (Storage->ElementCount == 0) {
      return EFI_SUCCESS;
    }

    //
    // Generate <ConfigResp>
    //
    Status = StorageToConfigResp (Storage, &ConfigResp);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Skip <ConfigHdr> and '&' to point to <ConfigBody>
    //
    StrPtr = ConfigResp + StrLen (Storage->ConfigHdr) + 1;

    BufferSize = StrSize (StrPtr);
    if (*ResultsDataSize < BufferSize) {
      *ResultsDataSize = BufferSize;

      gBS->FreePool (ConfigResp);
      return EFI_BUFFER_TOO_SMALL;
    }

    *ResultsDataSize = BufferSize;
    CopyMem (ResultsData, StrPtr, BufferSize);

    gBS->FreePool (ConfigResp);
  } else {
    //
    // Prepare <ConfigResp>
    //
    BufferSize = (StrLen (ResultsData) + StrLen (Storage->ConfigHdr) + 2) * sizeof (CHAR16);
    ConfigResp = AllocateZeroPool (BufferSize);
    ASSERT (ConfigResp != NULL);
    if (ConfigResp == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    StrCpy (ConfigResp, Storage->ConfigHdr);
    StrCat (ConfigResp, L"&");
    StrCat (ConfigResp, ResultsData);

    //
    // Update Browser uncommited data
    //
    Status = ConfigRespToStorage (Storage, ConfigResp);
    ScuSafeFreePool ((VOID **)&ConfigResp);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}

/**
 Notify function will remove the formset in the maintain list
 once this formset is removed.

 Functions which are registered to receive notification of
 database events have this prototype. The actual event is encoded
 in NotifyType. The following table describes how PackageType,
 PackageGuid, Handle, and Package are used for each of the
 notification types.

 @param[in] PackageType  Package type of the notification.
 @param[in] PackageGuid  If PackageType is
                         EFI_HII_PACKAGE_TYPE_GUID, then this is
                         the pointer to the GUID from the Guid
                         field of EFI_HII_PACKAGE_GUID_HEADER.
                         Otherwise, it must be NULL.
 @param[in] Package      Points to the package referred to by the
                         notification Handle The handle of the package
                         list which contains the specified package.
 @param[in] Handle       The HII handle.
 @param[in] NotifyType   The type of change concerning the database.
                         See EFI_HII_DATABASE_NOTIFY_TYPE.
**/
EFI_STATUS
EFIAPI
FormsetRemoveNotify (
  IN UINT8                              PackageType,
  IN CONST EFI_GUID                     *PackageGuid,
  IN CONST EFI_HII_PACKAGE_HEADER       *Package,
  IN EFI_HII_HANDLE                     Handle,
  IN EFI_HII_DATABASE_NOTIFY_TYPE       NotifyType
  )
{
  FORM_BROWSER_FORMSET *FormSet;

  //
  // Ignore the update for current using formset, which is handled by another notify function.
  //
  if (IsHiiHandleInBrowserContext (Handle)) {
    return EFI_SUCCESS;
  }

  //
  // Remove the backup FormSet data when the Form Package is removed.
  //
  FormSet = GetFormSetFromHiiHandle (Handle);
  if (FormSet != NULL) {
    RemoveEntryList (&FormSet->Link);
    DestroyFormSet (FormSet);
  }

  return EFI_SUCCESS;
}

/**
 Initialize Setup Browser driver.

 @param[in] ImageHandle     The image handle.
 @param[in] SystemTable     The system table.

 @retval EFI_SUCCESS        The Setup Browser module is initialized correctly..
 @return Other value if failed to initialize the Setup Browser module.
**/
EFI_STATUS
EFIAPI
InitializeSetup (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                  Status;
  EFI_HANDLE                  DriverHandle;
  EFI_HANDLE                  NotifyHandle;
  EFI_INPUT_KEY               DefaultHotKey;
  EFI_STRING                  HelpString;

  //
  // Do not remove, it is for compiler optimization
  //
#ifndef __GNUC__
  UINT32                      DummyVersion;
  DummyVersion = OBS_HEADER.EZH2OVersion;
#endif
  //
  // Initialize the global for DeviceMangerSetup protocol
  //
  gDeviceManagerSetup = NULL;

  //
  // Locate required Hii relative protocols
  //
  Status = gBS->LocateProtocol (
                  &gEfiHiiDatabaseProtocolGuid,
                  NULL,
                  (VOID **) &mHiiDatabase
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (
                  &gEfiHiiStringProtocolGuid,
                  NULL,
                  (VOID **) &mHiiString
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (
                  &gEfiHiiConfigRoutingProtocolGuid,
                  NULL,
                  (VOID **) &mHiiConfigRouting
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Publish our HII data
  //
  DriverHandle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mSetupBrowserHiiVendorDevicePath,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  gHiiHandle = HiiAddPackages (
                 &gSetupBrowserGuid,
                 DriverHandle,
                 SetupBrowserStrings,
                 NULL
                 );
  ASSERT (gHiiHandle != NULL);

  //
  // Initialize Driver private data
  //
  gBannerData = AllocateZeroPool (sizeof (BANNER_DATA));
  ASSERT (gBannerData != NULL);

  //
  // Install FormBrowser2 protocol
  //
  mPrivateData.Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &mPrivateData.Handle,
                  &gEfiFormBrowser2ProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mPrivateData.FormBrowser2
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Install default HotKey F10 for Save
  //
  DefaultHotKey.UnicodeChar = CHAR_NULL;
  HelpString                = GetToken (STRING_TOKEN (FUNCTION_TEN_STRING), gHiiHandle);
  DefaultHotKey.ScanCode    = SCAN_F10;
  Status = RegisterHotKey (&DefaultHotKey, BROWSER_ACTION_SUBMIT, 0, HelpString);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  FreePool (HelpString);
  //
  // Install default HotKey F9 for Reset To Defaults
  //
  DefaultHotKey.ScanCode    = SCAN_F9;
  HelpString                = GetToken (STRING_TOKEN (FUNCTION_NINE_STRING), gHiiHandle);
  Status = RegisterHotKey (&DefaultHotKey, BROWSER_ACTION_DEFAULT, EFI_HII_DEFAULT_CLASS_STANDARD, HelpString);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  FreePool (HelpString);

  //
  // Install FormBrowserEx protocol
  //
  mPrivateData.Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &mPrivateData.Handle,
                  &gEfiFormBrowserExProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mPrivateData.FormBrowserEx
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Register notify for Form package remove
  //
  Status = mHiiDatabase->RegisterPackageNotify (
                           mHiiDatabase,
                           EFI_HII_PACKAGE_FORMS,
                           NULL,
                           FormsetRemoveNotify,
                           EFI_HII_DATABASE_NOTIFY_REMOVE_PACK,
                           &NotifyHandle
                           );
  ASSERT_EFI_ERROR (Status);

  Status = InstallH2ODialogProtocol (mPrivateData.Handle);
  ASSERT_EFI_ERROR (Status);

#ifdef GRAPHICS_SETUP_SUPPORT
    InitializeGraphicsUi (ImageHandle);
#endif
  return Status;
}


/**
 Create a new string in HII Package List.

 @param[in] String              The String to be added
 @param[in] HiiHandle           The package list in the HII database to insert the specified string.

 @return The output string.
**/
EFI_STRING_ID
NewString (
  IN  CHAR16                   *String,
  IN  EFI_HII_HANDLE           HiiHandle
  )
{
  EFI_STRING_ID  StringId;

  StringId = HiiSetString (HiiHandle, 0, String, NULL);
  ASSERT (StringId != 0);

  return StringId;
}

/**
 Modify a string from HII Package List.

 @param[in] StringId            Id of the string in HII database.
 @param[in] String              The String to be set
 @param[in] HiiHandle           The HII package list handle.

 @retval EFI_SUCCESS            The string was set successfully.
 @retval EFI_ABORTED            Fail to set string.
**/
EFI_STATUS
SetString (
  IN  EFI_STRING_ID            StringId,
  IN  CHAR16                   *String,
  IN  EFI_HII_HANDLE           HiiHandle
  )
{
  StringId = HiiSetString (HiiHandle, StringId, String, NULL);

  if (StringId == 0) {
    return EFI_ABORTED;
  }

  return EFI_SUCCESS;
}

/**
 Delete a string from HII Package List.

 @param[in] StringId            Id of the string in HII database.
 @param[in] HiiHandle           The HII package list handle.

 @retval EFI_SUCCESS            The string was deleted successfully.
**/
EFI_STATUS
DeleteString (
  IN  EFI_STRING_ID            StringId,
  IN  EFI_HII_HANDLE           HiiHandle
  )
{
  CHAR16  NullChar;

  NullChar = CHAR_NULL;
  HiiSetString (HiiHandle, StringId, &NullChar, NULL);

  return EFI_SUCCESS;
}

/**
 Get the string based on the StringId and HII Package List Handle.

 @param[in] Token               The String's ID.
 @param[in] HiiHandle           The package list in the HII database to search for
                                the specified string.

 @return The output string.
**/
CHAR16 *
GetToken (
  IN  EFI_STRING_ID                Token,
  IN  EFI_HII_HANDLE               HiiHandle
  )
{
  EFI_STRING  String;

  if (HiiHandle == NULL) {
    return NULL;
  }

  String = HiiGetString (HiiHandle, Token, NULL);
  if (String == NULL) {
    String = AllocateCopyPool (sizeof (mUnknownString), mUnknownString);
    ASSERT (String != NULL);
  }
  return (CHAR16 *) String;
}

/**
 Allocate new memory and then copy the Unicode string Source to Destination.

 @param[in, out] Dest                   Location to copy string
 @param[in]      Src                    String to copy
**/
VOID
NewStringCpy (
  IN OUT CHAR16       **Dest,
  IN CHAR16           *Src
  )
{
  ScuSafeFreePool ((VOID **)Dest);
  *Dest = AllocateCopyPool (StrSize (Src), Src);
  ASSERT (*Dest != NULL);
}

/**
 Allocate new memory and concatinate Source on the end of Destination.

 @param[in, out] Dest                   String to added to the end of.
 @param[in]      Src                    String to concatinate.
**/
VOID
NewStringCat (
  IN OUT CHAR16       **Dest,
  IN CHAR16           *Src
  )
{
  CHAR16  *NewString;

  if (*Dest == NULL) {
    NewStringCpy (Dest, Src);
    return;
  }

  NewString = AllocateZeroPool (StrSize (*Dest) + StrSize (Src) - 1);
  ASSERT (NewString != NULL);
  if (NewString == NULL) {
    return ;
  }

  StrCpy (NewString, *Dest);
  StrCat (NewString, Src);

  gBS->FreePool (*Dest);
  *Dest = NewString;
}

/**
 Synchronize or restore Storage's Edit copy and Shadow copy.

 @param[in] Storage          The Storage to be synchronized.
**/
VOID
SynchronizeStorage (
  IN FORMSET_STORAGE         *Storage
  )
{
  LIST_ENTRY              *Link;
  NAME_VALUE_NODE         *Node;

  switch (Storage->Type) {
  case EFI_HII_VARSTORE_BUFFER:
    CopyMem (Storage->Buffer, Storage->EditBuffer, Storage->Size);
    break;

  case EFI_HII_VARSTORE_NAME_VALUE:
    Link = GetFirstNode (&Storage->NameValueListHead);
    while (!IsNull (&Storage->NameValueListHead, Link)) {
      Node = NAME_VALUE_NODE_FROM_LINK (Link);

      NewStringCpy (&Node->Value, Node->EditValue);

      Link = GetNextNode (&Storage->NameValueListHead, Link);
    }
    break;

  case EFI_HII_VARSTORE_EFI_VARIABLE:
  default:
    break;
  }
}

/**
 Get Value for given Name from a NameValue Storage.

 @param[in]      Storage        The NameValue Storage.
 @param[in]      Name           The Name.
 @param[in, out] Value          The retured Value.

 @retval EFI_SUCCESS            Value found for given Name.
 @retval EFI_NOT_FOUND          No such Name found in NameValue storage.
**/
EFI_STATUS
GetValueByName (
  IN FORMSET_STORAGE         *Storage,
  IN CHAR16                  *Name,
  IN OUT CHAR16              **Value
  )
{
  LIST_ENTRY              *Link;
  NAME_VALUE_NODE         *Node;

  *Value = NULL;

  Link = GetFirstNode (&Storage->NameValueListHead);
  while (!IsNull (&Storage->NameValueListHead, Link)) {
    Node = NAME_VALUE_NODE_FROM_LINK (Link);

    if (StrCmp (Name, Node->Name) == 0) {
      NewStringCpy (Value, Node->EditValue);
      if (*Value == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      return EFI_SUCCESS;
    }

    Link = GetNextNode (&Storage->NameValueListHead, Link);
  }

  return EFI_NOT_FOUND;
}

/**
 Set Value of given Name in a NameValue Storage.

 @param[in] Storage             The NameValue Storage.
 @param[in] Name                The Name.
 @param[in] Value               The Value to set.

 @retval EFI_SUCCESS            Value found for given Name.
 @retval EFI_NOT_FOUND          No such Name found in NameValue storage.
**/
EFI_STATUS
SetValueByName (
  IN FORMSET_STORAGE         *Storage,
  IN CHAR16                  *Name,
  IN CHAR16                  *Value
  )
{
  LIST_ENTRY              *Link;
  NAME_VALUE_NODE         *Node;

  Link = GetFirstNode (&Storage->NameValueListHead);
  while (!IsNull (&Storage->NameValueListHead, Link)) {
    Node = NAME_VALUE_NODE_FROM_LINK (Link);

    if (StrCmp (Name, Node->Name) == 0) {
      ScuSafeFreePool ((VOID **)&Node->EditValue);
      Node->EditValue = AllocateCopyPool (StrSize (Value), Value);
      ASSERT (Node->EditValue != NULL);
      return EFI_SUCCESS;
    }

    Link = GetNextNode (&Storage->NameValueListHead, Link);
  }

  return EFI_NOT_FOUND;
}

/**
 Convert setting of Buffer Storage or NameValue Storage to <ConfigResp>.

 @param[in] Buffer              The Storage to be conveted.
 @param[in] ConfigResp          The returned <ConfigResp>.

 @retval EFI_SUCCESS            Convert success.
 @retval EFI_INVALID_PARAMETER  Incorrect storage type.
**/
EFI_STATUS
StorageToConfigResp (
  IN FORMSET_STORAGE         *Storage,
  IN CHAR16                  **ConfigResp
  )
{
  EFI_STATUS  Status;
  EFI_STRING  Progress;
  LIST_ENTRY              *Link;
  NAME_VALUE_NODE         *Node;

  Status = EFI_SUCCESS;

  switch (Storage->Type) {
  case EFI_HII_VARSTORE_BUFFER:
    Status = mHiiConfigRouting->BlockToConfig (
                                  mHiiConfigRouting,
                                  Storage->ConfigRequest,
                                  Storage->EditBuffer,
                                  Storage->Size,
                                  ConfigResp,
                                  &Progress
                                  );
    break;

  case EFI_HII_VARSTORE_NAME_VALUE:
    *ConfigResp = NULL;
    NewStringCat (ConfigResp, Storage->ConfigHdr);

    Link = GetFirstNode (&Storage->NameValueListHead);
    while (!IsNull (&Storage->NameValueListHead, Link)) {
      Node = NAME_VALUE_NODE_FROM_LINK (Link);

      NewStringCat (ConfigResp, L"&");
      NewStringCat (ConfigResp, Node->Name);
      NewStringCat (ConfigResp, L"=");
      NewStringCat (ConfigResp, Node->EditValue);

      Link = GetNextNode (&Storage->NameValueListHead, Link);
    }
    break;

  case EFI_HII_VARSTORE_EFI_VARIABLE:
  default:
    Status = EFI_INVALID_PARAMETER;
    break;
  }

  return Status;
}

/**
 Convert <ConfigResp> to settings in Buffer Storage or NameValue Storage.

 @param[in] Storage             The Storage to receive the settings.
 @param[in] ConfigResp          The <ConfigResp> to be converted.

 @retval EFI_SUCCESS            Convert success.
 @retval EFI_INVALID_PARAMETER  Incorrect storage type.
**/
EFI_STATUS
ConfigRespToStorage (
  IN FORMSET_STORAGE         *Storage,
  IN CHAR16                  *ConfigResp
  )
{
  EFI_STATUS  Status;
  EFI_STRING  Progress;
  UINTN       BufferSize;
  CHAR16      *StrPtr;
  CHAR16      *Name;
  CHAR16      *Value;

  Status = EFI_SUCCESS;

  switch (Storage->Type) {

  case EFI_HII_VARSTORE_BUFFER:
    BufferSize = Storage->Size;
    Status = mHiiConfigRouting->ConfigToBlock (
                                  mHiiConfigRouting,
                                  ConfigResp,
                                  Storage->EditBuffer,
                                  &BufferSize,
                                  &Progress
                                  );
    break;

  case EFI_HII_VARSTORE_NAME_VALUE:
    StrPtr = StrStr (ConfigResp, L"PATH");
    if (StrPtr == NULL) {
      break;
    }
    StrPtr = StrStr (StrPtr, L"&");
    while (StrPtr != NULL) {
      //
      // Skip '&'
      //
      StrPtr = StrPtr + 1;
      Name = StrPtr;
      StrPtr = StrStr (StrPtr, L"=");
      if (StrPtr == NULL) {
        break;
      }
      *StrPtr = 0;

      //
      // Skip '='
      //
      StrPtr = StrPtr + 1;
      Value = StrPtr;
      StrPtr = StrStr (StrPtr, L"&");
      if (StrPtr != NULL) {
        *StrPtr = 0;
      }
      SetValueByName (Storage, Name, Value);
    }
    break;

  case EFI_HII_VARSTORE_EFI_VARIABLE:
  default:
    Status = EFI_INVALID_PARAMETER;
    break;
  }

  return Status;
}

/**
 Get Question's current Value.

 @param[in]      FormSet        FormSet data structure.
 @param[in]      Form           Form data structure.
 @param[in, out] Question       Question to be initialized.
 @param[in]      Cached         TRUE:  get from Edit copy
                                FALSE: get from original Storage

 @retval EFI_SUCCESS            The function completed successfully.
**/
EFI_STATUS
GetQuestionValue (
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN FORM_BROWSER_FORM                *Form,
  IN OUT FORM_BROWSER_STATEMENT       *Question,
  IN BOOLEAN                          Cached
  )
{
  EFI_STATUS          Status;
  BOOLEAN             Enabled;
  BOOLEAN             Pending;
  UINT8               *Dst;
  UINTN               StorageWidth;
  EFI_TIME            EfiTime;
  FORMSET_STORAGE     *Storage;
  EFI_IFR_TYPE_VALUE  *QuestionValue;
  CHAR16              *ConfigRequest;
  CHAR16              *Progress;
  CHAR16              *Result;
  CHAR16              *Value;
  CHAR16              *StringPtr;
  UINTN               Length;
  BOOLEAN             IsBufferStorage;
  BOOLEAN             IsString;

  Status = EFI_SUCCESS;

  //
  // Statement don't have storage, skip them
  //
  if (Question->QuestionId == 0) {
    return Status;
  }

  //
  // Question value is provided by an Expression, evaluate it
  //
  if (Question->ValueExpression != NULL) {
    Status = EvaluateExpression (FormSet, Form, Question->ValueExpression);
    if (!EFI_ERROR (Status)) {
      CopyMem (&Question->HiiValue, &Question->ValueExpression->Result, sizeof (EFI_HII_VALUE));
    }
    return Status;
  }

  //
  // Question value is provided by RTC
  //
  Storage = Question->Storage;
  QuestionValue = &Question->HiiValue.Value;
  if (Storage == NULL) {
    //
    // It's a Question without storage, or RTC date/time
    //
    if (Question->Operand == EFI_IFR_DATE_OP || Question->Operand == EFI_IFR_TIME_OP) {
      //
      // Date and time define the same Flags bit
      //
      switch (Question->Flags & EFI_QF_DATE_STORAGE) {
      case QF_DATE_STORAGE_TIME:
        Status = gRT->GetTime (&EfiTime, NULL);
        break;

      case QF_DATE_STORAGE_WAKEUP:
        Status = gRT->GetWakeupTime (&Enabled, &Pending, &EfiTime);
        break;

      case QF_DATE_STORAGE_NORMAL:
      default:
        //
        // For date/time without storage
        //
        return EFI_SUCCESS;
      }

      if (EFI_ERROR (Status)) {
        return Status;
      }

      if (Question->Operand == EFI_IFR_DATE_OP) {
        QuestionValue->date.Year  = EfiTime.Year;
        QuestionValue->date.Month = EfiTime.Month;
        QuestionValue->date.Day   = EfiTime.Day;
      } else {
        QuestionValue->time.Hour   = EfiTime.Hour;
        QuestionValue->time.Minute = EfiTime.Minute;
        QuestionValue->time.Second = EfiTime.Second;
      }
    }

    return EFI_SUCCESS;
  }

  //
  // Question value is provided by EFI variable
  //
  StorageWidth = Question->StorageWidth;
  if (Storage->Type == EFI_HII_VARSTORE_EFI_VARIABLE) {
    if (Question->BufferValue != NULL) {
      Dst = Question->BufferValue;
    } else {
      Dst = (UINT8 *) QuestionValue;
    }

    Status = gRT->GetVariable (
                     Question->VariableName,
                     &Storage->Guid,
                     NULL,
                     &StorageWidth,
                     Dst
                     );
    //
    // Always return success, even this EFI variable doesn't exist
    //
    return EFI_SUCCESS;
  }

  //
  // Question Value is provided by Buffer Storage or NameValue Storage
  //
  if (Question->BufferValue != NULL) {
    //
    // This Question is password or orderedlist
    //
    Dst = Question->BufferValue;
  } else {
    //
    // Other type of Questions
    //
    Dst = (UINT8 *) &Question->HiiValue.Value;
  }

  IsBufferStorage = (Storage->Type == EFI_HII_VARSTORE_BUFFER) ? TRUE : FALSE;
  IsString = (Question->HiiValue.Type == EFI_IFR_TYPE_STRING) ?  TRUE : FALSE;
  if (Cached) {
    if (IsBufferStorage) {
      //
      // Copy from storage Edit buffer
      //
      CopyMem (Dst, Storage->EditBuffer + Question->VarStoreInfo.VarOffset, StorageWidth);
    } else {
      Status = GetValueByName (Storage, Question->VariableName, &Value);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      if (Value == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      if (IsString) {
        //
        // Convert Config String to Unicode String, e.g "0041004200430044" => "ABCD"
        //
        Length = StorageWidth + sizeof (CHAR16);
        Status = ConfigStringToUnicode ((CHAR16 *) Dst, &Length, Value);
      } else {
        Status = HexStringToBuf (Dst, &StorageWidth, Value, NULL);
      }

      gBS->FreePool (Value);
    }
  } else {
    //
    // Request current settings from Configuration Driver
    //
    if (FormSet->ConfigAccess == NULL) {
      return EFI_NOT_FOUND;
    }

    //
    // <ConfigRequest> ::= <ConfigHdr> + <BlockName> ||
    //                   <ConfigHdr> + "&" + <VariableName>
    //
    if (IsBufferStorage) {
      Length = StrLen (Storage->ConfigHdr) + StrLen (Question->BlockName);
    } else {
      Length = StrLen (Storage->ConfigHdr) + StrLen (Question->VariableName) + 1;
    }
    ConfigRequest = AllocateZeroPool ((Length + 1) * sizeof (CHAR16));
    ASSERT (ConfigRequest != NULL);
    if (ConfigRequest == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    StrCpy (ConfigRequest, Storage->ConfigHdr);
    if (IsBufferStorage) {
      StrCat (ConfigRequest, Question->BlockName);
    } else {
      StrCat (ConfigRequest, L"&");
      StrCat (ConfigRequest, Question->VariableName);
    }

    Status = FormSet->ConfigAccess->ExtractConfig (
                                      FormSet->ConfigAccess,
                                      ConfigRequest,
                                      &Progress,
                                      &Result
                                      );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Skip <ConfigRequest>
    //
    Value = Result + Length;
    if (IsBufferStorage) {
      //
      // Skip "&VALUE"
      //
      Value = Value + 6;
    }
    if (*Value != '=') {
      gBS->FreePool (Result);
      return EFI_NOT_FOUND;
    }
    //
    // Skip '=', point to value
    //
    Value = Value + 1;

    //
    // Suppress <AltResp> if any
    //
    StringPtr = Value;
    while (*StringPtr != L'\0' && *StringPtr != L'&') {
      StringPtr++;
    }
    *StringPtr = L'\0';

    if (!IsBufferStorage && IsString) {
      //
      // Convert Config String to Unicode String, e.g "0041004200430044" => "ABCD"
      //
      Length = StorageWidth + sizeof (CHAR16);
      Status = ConfigStringToUnicode ((CHAR16 *) Dst, &Length, Value);
    } else {
      Status = HexStringToBuf (Dst, &StorageWidth, Value, NULL);
      if (EFI_ERROR (Status)) {
        gBS->FreePool (Result);
        return Status;
      }
    }

    //
    // Synchronize Edit Buffer
    //
    if (IsBufferStorage) {
      CopyMem (Storage->EditBuffer + Question->VarStoreInfo.VarOffset, Dst, StorageWidth);
    } else {
      SetValueByName (Storage, Question->VariableName, Value);
    }
    gBS->FreePool (Result);
  }

  return Status;
}

/**
 Save Question Value to edit copy(cached) or Storage(uncached).

 @param[in]      FormSet        FormSet data structure.
 @param[in]      Form           Form data structure.
 @param[in, out] Question       Pointer to the Question.
 @param[in]      Cached         TRUE:  set to Edit copy
                                FALSE: set to original Storage

 @retval EFI_SUCCESS            The function completed successfully.
**/
EFI_STATUS
SetQuestionValue (
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN FORM_BROWSER_FORM                *Form,
  IN OUT FORM_BROWSER_STATEMENT       *Question,
  IN BOOLEAN                          Cached
  )
{
  EFI_STATUS          Status;
  BOOLEAN             Enabled;
  BOOLEAN             Pending;
  UINT8               *Src;
  EFI_TIME            EfiTime;
  UINTN               BufferLen;
  UINTN               StorageWidth;
  FORMSET_STORAGE     *Storage;
  EFI_IFR_TYPE_VALUE  *QuestionValue;
  CHAR16              *ConfigResp;
  CHAR16              *Progress;
  CHAR16              *Value;
  UINTN               Length;
  BOOLEAN             IsBufferStorage;
  BOOLEAN             IsString;

  Status = EFI_SUCCESS;

  //
  // Statement don't have storage, skip them
  //
  if (Question->QuestionId == 0) {
    return Status;
  }

  //
  // If Question value is provided by an Expression, then it is read only
  //
  if (Question->ValueExpression != NULL) {
    return Status;
  }

  //
  // Question value is provided by RTC
  //
  Storage = Question->Storage;
  QuestionValue = &Question->HiiValue.Value;
  if (Storage == NULL) {
    //
    // It's a Question without storage, or RTC date/time
    //
    if (Question->Operand == EFI_IFR_DATE_OP || Question->Operand == EFI_IFR_TIME_OP) {
      //
      // Date and time define the same Flags bit
      //
      switch (Question->Flags & EFI_QF_DATE_STORAGE) {

      case QF_DATE_STORAGE_TIME:
        Status = gRT->GetTime (&EfiTime, NULL);
        break;

      case QF_DATE_STORAGE_WAKEUP:
        Status = gRT->GetWakeupTime (&Enabled, &Pending, &EfiTime);
        break;

      case QF_DATE_STORAGE_NORMAL:
      default:
        //
        // For date/time without storage
        //
        return EFI_SUCCESS;
      }

      if (EFI_ERROR (Status)) {
        return Status;
      }

      if (Question->Operand == EFI_IFR_DATE_OP) {
        EfiTime.Year  = QuestionValue->date.Year;
        EfiTime.Month = QuestionValue->date.Month;
        EfiTime.Day   = QuestionValue->date.Day;
      } else {
        EfiTime.Hour   = QuestionValue->time.Hour;
        EfiTime.Minute = QuestionValue->time.Minute;
        EfiTime.Second = QuestionValue->time.Second;
      }

      if ((Question->Flags & EFI_QF_DATE_STORAGE) == QF_DATE_STORAGE_TIME) {
        do {
          Status = gRT->SetTime(&EfiTime);
          if (Status == EFI_INVALID_PARAMETER) {
            EfiTime.Day--;
          }
        } while (Status == EFI_INVALID_PARAMETER);
      } else {
        Status = gRT->SetWakeupTime (TRUE, &EfiTime);
      }
    }

    return Status;
  }

  //
  // Question value is provided by EFI variable
  //
  StorageWidth = Question->StorageWidth;
  if (Storage->Type == EFI_HII_VARSTORE_EFI_VARIABLE) {
    if (Question->BufferValue != NULL) {
      Src = Question->BufferValue;
    } else {
      Src = (UINT8 *) QuestionValue;
    }

    Status = gRT->SetVariable (
                     Question->VariableName,
                     &Storage->Guid,
                     Storage->Attributes,
                     StorageWidth,
                     Src
                     );
    return Status;
  }

  //
  // Question Value is provided by Buffer Storage or NameValue Storage
  //
  if (Question->BufferValue != NULL) {
    Src = Question->BufferValue;
  } else {
    Src = (UINT8 *) &Question->HiiValue.Value;
  }

  IsBufferStorage = (Storage->Type == EFI_HII_VARSTORE_BUFFER) ? TRUE : FALSE;
  IsString = (Question->HiiValue.Type == EFI_IFR_TYPE_STRING) ?  TRUE : FALSE;
  if (IsBufferStorage) {
    //
    // Copy to storage edit buffer
    //
    CopyMem (Storage->EditBuffer + Question->VarStoreInfo.VarOffset, Src, StorageWidth);
  } else {
    if (IsString) {
      //
      // Convert Unicode String to Config String, e.g. "ABCD" => "0041004200430044"
      //
      Value = NULL;
      BufferLen = ((StrLen ((CHAR16 *) Src) * 4) + 1) * sizeof (CHAR16);
      Value = AllocateZeroPool (BufferLen);
      ASSERT (Value != NULL);
      Status = UnicodeToConfigString (Value, &BufferLen, (CHAR16 *) Src);
      ASSERT_EFI_ERROR (Status);
    } else {
      BufferLen = StorageWidth * 2 + 1;
      Value = AllocateZeroPool (BufferLen * sizeof (CHAR16));
      ASSERT (Value != NULL);
      if (Value == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      BufToHexString (Value, &BufferLen, Src, StorageWidth);
      ToLower (Value);
    }

    Status = SetValueByName (Storage, Question->VariableName, Value);
    gBS->FreePool (Value);
  }

  if (!Cached) {
    //
    // <ConfigResp> ::= <ConfigHdr> + <BlockName> + "&VALUE=" + "<HexCh>StorageWidth * 2" ||
    //                <ConfigHdr> + "&" + <VariableName> + "=" + "<string>"
    //
    if (IsBufferStorage) {
      Length = StrLen (Question->BlockName) + 7;
    } else {
      Length = StrLen (Question->VariableName) + 2;
    }
    if (!IsBufferStorage && IsString) {
      Length += (StrLen ((CHAR16 *) Src) * 4);
    } else {
      Length += (StorageWidth * 2);
    }
    ConfigResp = AllocateZeroPool ((StrLen (Storage->ConfigHdr) + Length + 1) * sizeof (CHAR16));
    ASSERT (ConfigResp != NULL);
    if (ConfigResp == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    StrCpy (ConfigResp, Storage->ConfigHdr);
    if (IsBufferStorage) {
      StrCat (ConfigResp, Question->BlockName);
      StrCat (ConfigResp, L"&VALUE=");
    } else {
      StrCat (ConfigResp, L"&");
      StrCat (ConfigResp, Question->VariableName);
      StrCat (ConfigResp, L"=");
    }

    Value = ConfigResp + StrLen (ConfigResp);
    if (!IsBufferStorage && IsString) {
      //
      // Convert Unicode String to Config String, e.g. "ABCD" => "0041004200430044"
      //
      BufferLen = ((StrLen ((CHAR16 *) Src) * 4) + 1) * sizeof (CHAR16);
      Status = UnicodeToConfigString (Value, &BufferLen, (CHAR16 *) Src);
      ASSERT_EFI_ERROR (Status);
    } else {
      BufferLen = StorageWidth * 2 + 1;
      BufToHexString (Value, &BufferLen, Src, StorageWidth);
      ToLower (Value);
    }

    //
    // Submit Question Value to Configuration Driver
    //
    if (FormSet->ConfigAccess != NULL) {
      Status = FormSet->ConfigAccess->RouteConfig (
                                        FormSet->ConfigAccess,
                                        ConfigResp,
                                        &Progress
                                        );
      if (EFI_ERROR (Status)) {
        gBS->FreePool (ConfigResp);
        return Status;
      }
    }
    gBS->FreePool (ConfigResp);

    //
    // Synchronize shadow Buffer
    //
    SynchronizeStorage (Storage);
  }

  return Status;
}

/**
 Perform inconsistent check for a Form.

 @param[in] FormSet             FormSet data structure.
 @param[in] Form                Form data structure.
 @param[in] Question            The Question to be validated.
 @param[in] Type                Validation type: InConsistent or NoSubmit

 @retval EFI_SUCCESS            Form validation pass.
 @retval other                  Form validation failed.
**/
EFI_STATUS
ValidateQuestion (
  IN  FORM_BROWSER_FORMSET            *FormSet,
  IN  FORM_BROWSER_FORM               *Form,
  IN  FORM_BROWSER_STATEMENT          *Question,
  IN  UINTN                           Type
  )
{
  EFI_STATUS              Status;
  LIST_ENTRY              *Link;
  LIST_ENTRY              *ListHead;
  EFI_STRING              PopUp;
  EFI_INPUT_KEY           Key;
  FORM_EXPRESSION         *Expression;

  if (Type == EFI_HII_EXPRESSION_INCONSISTENT_IF) {
    ListHead = &Question->InconsistentListHead;
  } else if (Type == EFI_HII_EXPRESSION_NO_SUBMIT_IF) {
    ListHead = &Question->NoSubmitListHead;
  } else {
    return EFI_UNSUPPORTED;
  }

  Link = GetFirstNode (ListHead);
  while (!IsNull (ListHead, Link)) {
    Expression = FORM_EXPRESSION_FROM_LINK (Link);

    //
    // Evaluate the expression
    //
    Status = EvaluateExpression (FormSet, Form, Expression);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (Expression->Result.Value.b) {
      //
      // Condition meet, show up error message
      //
      if (Expression->Error != 0) {
        PopUp = GetToken (Expression->Error, FormSet->HiiHandle);
        do {
          CreateDialog (4, TRUE, 0, NULL, &Key, gEmptyString, PopUp, gPressEnter, gEmptyString);
        } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);
        gBS->FreePool (PopUp);
      }

      return EFI_NOT_READY;
    }

    Link = GetNextNode (ListHead, Link);
  }

  return EFI_SUCCESS;
}

/**
 Perform NoSubmit check for a Form.

 @param[in] FormSet             FormSet data structure.
 @param[in] CurrentForm         Form data structure.

 @retval EFI_SUCCESS            Form validation pass.
 @retval other                  Form validation failed.
**/
EFI_STATUS
NoSubmitCheck (
  IN  FORM_BROWSER_FORMSET            *FormSet,
  IN  FORM_BROWSER_FORM               *Form
  )
{
  EFI_STATUS              Status;
  LIST_ENTRY              *Link;
  FORM_BROWSER_STATEMENT  *Question;

  Link = GetFirstNode (&Form->StatementListHead);
  while (!IsNull (&Form->StatementListHead, Link)) {
    Question = FORM_BROWSER_STATEMENT_FROM_LINK (Link);

    Status = ValidateQuestion (FormSet, Form, Question, EFI_HII_EXPRESSION_NO_SUBMIT_IF);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Link = GetNextNode (&Form->StatementListHead, Link);
  }

  return EFI_SUCCESS;
}

/**
 Submit a Form.

 @param[in] FormSet             FormSet data structure.
 @param[in] Form                Form data structure.

 @retval EFI_SUCCESS            The function completed successfully.
 @retval EFI_UNSUPPORTED        Unsupport SettingScope.
**/
EFI_STATUS
SubmitForm (
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN FORM_BROWSER_FORM                *Form
  )
{
  EFI_STATUS              Status;
  LIST_ENTRY              *Link;
  EFI_STRING              ConfigResp;
  EFI_STRING              Progress;
  FORMSET_STORAGE         *Storage;

  //
  // Validate the Form by NoSubmit check
  //
  Status = NoSubmitCheck (FormSet, Form);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Submit Buffer storage or Name/Value storage
  //
  Link = GetFirstNode (&FormSet->StorageListHead);
  while (!IsNull (&FormSet->StorageListHead, Link)) {
    Storage = FORMSET_STORAGE_FROM_LINK (Link);
    Link = GetNextNode (&FormSet->StorageListHead, Link);

    if (Storage->Type == EFI_HII_VARSTORE_EFI_VARIABLE) {
      continue;
    }

    //
    // Skip if there is no RequestElement
    //
    if (Storage->ElementCount == 0) {
      continue;
    }

    //
    // Prepare <ConfigResp>
    //
    Status = StorageToConfigResp (Storage, &ConfigResp);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Send <ConfigResp> to Configuration Driver
    //
    if (FormSet->ConfigAccess != NULL) {
      Status = FormSet->ConfigAccess->RouteConfig (
                                        FormSet->ConfigAccess,
                                        ConfigResp,
                                        &Progress
                                        );
      if (EFI_ERROR (Status)) {
        gBS->FreePool (ConfigResp);
        return Status;
      }
    }
    gBS->FreePool (ConfigResp);

    //
    // Config success, update storage shadow Buffer
    //
    SynchronizeStorage (Storage);
  }

  gNvUpdateRequired = FALSE;

  return EFI_SUCCESS;
}

/**
 Reset Question to its default value.

 @param[in] FormSet             The form set.
 @param[in] Form                The form.
 @param[in] Question            The question.
 @param[in] DefaultId           The Class of the default.

 @retval EFI_SUCCESS            Question is reset to default value.
**/
EFI_STATUS
GetQuestionDefault (
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN FORM_BROWSER_FORM                *Form,
  IN FORM_BROWSER_STATEMENT           *Question,
  IN UINT16                           DefaultId
  )
{
  EFI_STATUS              Status;
  LIST_ENTRY              *Link;
  QUESTION_DEFAULT        *Default;
  QUESTION_OPTION         *Option;
  EFI_HII_VALUE           *HiiValue;
  UINT8                   Index;

  Status = EFI_SUCCESS;

  //
  // Statement don't have storage, skip them
  //
  if (Question->QuestionId == 0) {
    return Status;
  }

  //
  // There are three ways to specify default value for a Question:
  //  1, use nested EFI_IFR_DEFAULT (highest priority)
  //  2, set flags of EFI_ONE_OF_OPTION (provide Standard and Manufacturing default)
  //  3, set flags of EFI_IFR_CHECKBOX (provide Standard and Manufacturing default) (lowest priority)
  //
  HiiValue = &Question->HiiValue;

  //
  // EFI_IFR_DEFAULT has highest priority
  //
  if (!IsListEmpty (&Question->DefaultListHead)) {
    Link = GetFirstNode (&Question->DefaultListHead);
    while (!IsNull (&Question->DefaultListHead, Link)) {
      Default = QUESTION_DEFAULT_FROM_LINK (Link);

      if (Default->DefaultId == DefaultId) {
        if (Default->ValueExpression != NULL) {
          //
          // Default is provided by an Expression, evaluate it
          //
          Status = EvaluateExpression (FormSet, Form, Default->ValueExpression);
          if (EFI_ERROR (Status)) {
            return Status;
          }

          CopyMem (HiiValue, &Default->ValueExpression->Result, sizeof (EFI_HII_VALUE));
        } else {
          //
          // Default value is embedded in EFI_IFR_DEFAULT
          //
          CopyMem (HiiValue, &Default->Value, sizeof (EFI_HII_VALUE));
        }

        return EFI_SUCCESS;
      }

      Link = GetNextNode (&Question->DefaultListHead, Link);
    }
  }

  //
  // EFI_ONE_OF_OPTION
  //
  if ((Question->Operand == EFI_IFR_ONE_OF_OP) && !IsListEmpty (&Question->OptionListHead)) {
    if (DefaultId <= EFI_HII_DEFAULT_CLASS_MANUFACTURING)  {
      //
      // OneOfOption could only provide Standard and Manufacturing default
      //
      Link = GetFirstNode (&Question->OptionListHead);
      while (!IsNull (&Question->OptionListHead, Link)) {
        Option = QUESTION_OPTION_FROM_LINK (Link);

        if (((DefaultId == EFI_HII_DEFAULT_CLASS_STANDARD) && (Option->Flags & EFI_IFR_OPTION_DEFAULT)) ||
            ((DefaultId == EFI_HII_DEFAULT_CLASS_MANUFACTURING) && (Option->Flags & EFI_IFR_OPTION_DEFAULT_MFG))
           ) {
          CopyMem (HiiValue, &Option->Value, sizeof (EFI_HII_VALUE));

          return EFI_SUCCESS;
        }

        Link = GetNextNode (&Question->OptionListHead, Link);
      }
    }
  }

  //
  // EFI_IFR_CHECKBOX - lowest priority
  //
  if (Question->Operand == EFI_IFR_CHECKBOX_OP) {
    if (DefaultId <= EFI_HII_DEFAULT_CLASS_MANUFACTURING)  {
      //
      // Checkbox could only provide Standard and Manufacturing default
      //
      if (((DefaultId == EFI_HII_DEFAULT_CLASS_STANDARD) && (Question->Flags & EFI_IFR_CHECKBOX_DEFAULT)) ||
          ((DefaultId == EFI_HII_DEFAULT_CLASS_MANUFACTURING) && (Question->Flags & EFI_IFR_CHECKBOX_DEFAULT_MFG))
         ) {
        HiiValue->Value.b = TRUE;
      } else {
        HiiValue->Value.b = FALSE;
      }

      return EFI_SUCCESS;
    }
  }

  //
  // For Questions without default
  //
  switch (Question->Operand) {

  case EFI_IFR_ONE_OF_OP:
    //
    // Take first oneof option as oneof's default value
    //
    if (ValueToOption (Question, HiiValue) == NULL) {
      Link = GetFirstNode (&Question->OptionListHead);
      if (!IsNull (&Question->OptionListHead, Link)) {
        Option = QUESTION_OPTION_FROM_LINK (Link);
        CopyMem (HiiValue, &Option->Value, sizeof (EFI_HII_VALUE));
      }
    }
    break;

  case EFI_IFR_ORDERED_LIST_OP:
    //
    // Take option sequence in IFR as ordered list's default value
    //
    Index = 0;
    Link = GetFirstNode (&Question->OptionListHead);
    while (!IsNull (&Question->OptionListHead, Link)) {
      Option = QUESTION_OPTION_FROM_LINK (Link);

      SetArrayData (Question->BufferValue, Question->ValueType, Index, Option->Value.Value.u64);

      Index++;
      if (Index >= Question->MaxContainers) {
        break;
      }

      Link = GetNextNode (&Question->OptionListHead, Link);
    }
    break;

  default:
    Status = EFI_NOT_FOUND;
    break;
  }

  return Status;
}

/**
 Reset Questions in a Form to their default value.

 @param[in] FormSet             FormSet data structure.
 @param[in] Form                Form data structure.
 @param[in] DefaultId           The Class of the default.

 @retval EFI_SUCCESS            The function completed successfully.
**/
EFI_STATUS
ExtractFormDefault (
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN FORM_BROWSER_FORM                *Form,
  IN UINT16                           DefaultId
  )
{
  EFI_STATUS              Status;
  LIST_ENTRY              *Link;
  FORM_BROWSER_STATEMENT  *Question;

  Link = GetFirstNode (&Form->StatementListHead);
  while (!IsNull (&Form->StatementListHead, Link)) {
    Question = FORM_BROWSER_STATEMENT_FROM_LINK (Link);
    Link = GetNextNode (&Form->StatementListHead, Link);

    //
    // If Question is disabled, don't reset it to default
    //
    if (Question->DisableExpression != NULL) {
      Status = EvaluateExpression (FormSet, Form, Question->DisableExpression);
      if (!EFI_ERROR (Status) && Question->DisableExpression->Result.Value.b) {
        continue;
      }
    }

    //
    // Reset Question to its default value
    //
    Status = GetQuestionDefault (FormSet, Form, Question, DefaultId);
    if (EFI_ERROR (Status)) {
      continue;
    }

    //
    // Synchronize Buffer storage's Edit buffer
    //
    if ((Question->Storage != NULL) &&
        (Question->Storage->Type != EFI_HII_VARSTORE_EFI_VARIABLE)) {
      SetQuestionValue (FormSet, Form, Question, TRUE);
    }
  }

  return EFI_SUCCESS;
}

/**
 Initialize Question's Edit copy from Storage.

 @param[in] FormSet             FormSet data structure.
 @param[in] Form                Form data structure.

 @retval EFI_SUCCESS            The function completed successfully.
**/
EFI_STATUS
LoadFormConfig (
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN FORM_BROWSER_FORM                *Form
  )
{
  EFI_STATUS              Status;
  LIST_ENTRY              *Link;
  FORM_BROWSER_STATEMENT  *Question;

  Link = GetFirstNode (&Form->StatementListHead);
  while (!IsNull (&Form->StatementListHead, Link)) {
    Question = FORM_BROWSER_STATEMENT_FROM_LINK (Link);

    //
    // Initialize local copy of Value for each Question
    //
    Status = GetQuestionValue (FormSet, Form, Question, TRUE);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Link = GetNextNode (&Form->StatementListHead, Link);
  }

  return EFI_SUCCESS;
}

/**
 Initialize Question's Edit copy from Storage for the whole Formset.

 @param[in] FormSet             FormSet data structure.

 @retval EFI_SUCCESS            The function completed successfully.
**/
EFI_STATUS
LoadFormSetConfig (
  IN FORM_BROWSER_FORMSET             *FormSet
  )
{
  EFI_STATUS              Status;
  LIST_ENTRY              *Link;
  FORM_BROWSER_FORM       *Form;

  Link = GetFirstNode (&FormSet->FormListHead);
  while (!IsNull (&FormSet->FormListHead, Link)) {
    Form = FORM_BROWSER_FORM_FROM_LINK (Link);

    //
    // Initialize local copy of Value for each Form
    //
    Status = LoadFormConfig (FormSet, Form);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Link = GetNextNode (&FormSet->FormListHead, Link);
  }

  return EFI_SUCCESS;
}

/**
 Fill storage's edit copy with settings requested from Configuration Driver.

 @param[in] FormSet             FormSet data structure.
 @param[in] Storage             Buffer Storage.

 @retval EFI_SUCCESS            The function completed successfully.
**/
EFI_STATUS
LoadStorage (
  IN FORM_BROWSER_FORMSET    *FormSet,
  IN FORMSET_STORAGE         *Storage
  )
{
  EFI_STATUS  Status;
  EFI_STRING  Progress;
  EFI_STRING  Result;
  CHAR16      *StrPtr;

  if (Storage->Type == EFI_HII_VARSTORE_EFI_VARIABLE) {
    return EFI_SUCCESS;
  }

  if (FormSet->ConfigAccess == NULL) {
    return EFI_NOT_FOUND;
  }

  if (Storage->ElementCount == 0) {
    //
    // Skip if there is no RequestElement
    //
    return EFI_SUCCESS;
  }

  //
  // Request current settings from Configuration Driver
  //
  Result = NULL;
  Status = FormSet->ConfigAccess->ExtractConfig (
                                    FormSet->ConfigAccess,
                                    Storage->ConfigRequest,
                                    &Progress,
                                    &Result
                                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Convert Result from <ConfigAltResp> to <ConfigResp>
  //
  StrPtr = StrStr (Result, L"ALTCFG");
  if (StrPtr != NULL) {
    *StrPtr = L'\0';
  }

  Status = ConfigRespToStorage (Storage, Result);
  ScuSafeFreePool ((VOID **)&Result);
  return Status;
}

/**
 Copy uncommitted data from source Storage to destination Storage.

 @param[in, out] Dst            Target Storage for uncommitted data.
 @param[in]      Src            Source Storage for uncommitted data.

 @retval EFI_SUCCESS            The function completed successfully.
 @retval EFI_INVALID_PARAMETER  Source and destination Storage is not the same type.
**/
EFI_STATUS
CopyStorage (
  IN OUT FORMSET_STORAGE     *Dst,
  IN FORMSET_STORAGE         *Src
  )
{
  LIST_ENTRY              *Link;
  NAME_VALUE_NODE         *Node;

  if ((Dst->Type != Src->Type) || (Dst->Size != Src->Size)) {
    return EFI_INVALID_PARAMETER;
  }

  switch (Src->Type) {
  case EFI_HII_VARSTORE_BUFFER:
    CopyMem (Dst->EditBuffer, Src->EditBuffer, Src->Size);
    break;

  case EFI_HII_VARSTORE_NAME_VALUE:
    Link = GetFirstNode (&Src->NameValueListHead);
    while (!IsNull (&Src->NameValueListHead, Link)) {
      Node = NAME_VALUE_NODE_FROM_LINK (Link);

      SetValueByName (Dst, Node->Name, Node->EditValue);

      Link = GetNextNode (&Src->NameValueListHead, Link);
    }
    break;

  case EFI_HII_VARSTORE_EFI_VARIABLE:
  default:
    break;
  }

  return EFI_SUCCESS;
}

/**
 Get current setting of Questions.

 @param[in, out] FormSet        FormSet data structure.

 @retval EFI_SUCCESS            The function completed successfully.
**/
EFI_STATUS
InitializeCurrentSetting (
  IN OUT FORM_BROWSER_FORMSET             *FormSet
  )
{
  EFI_STATUS              Status;
  LIST_ENTRY              *Link;
  LIST_ENTRY              *Link2;
  FORMSET_STORAGE         *Storage;
  FORMSET_STORAGE         *StorageSrc;
  FORMSET_STORAGE         *OldStorage;
  FORM_BROWSER_FORM       *Form;

  Status = EFI_SUCCESS;

  //
  // Extract default from IFR binary
  //
  Link = GetFirstNode (&FormSet->FormListHead);
  while (!IsNull (&FormSet->FormListHead, Link)) {
    Form = FORM_BROWSER_FORM_FROM_LINK (Link);

    Status = ExtractFormDefault (FormSet, Form, EFI_HII_DEFAULT_CLASS_STANDARD);

    Link = GetNextNode (&FormSet->FormListHead, Link);
  }

  //
  // Request current settings from Configuration Driver
  //
  Link = GetFirstNode (&FormSet->StorageListHead);
  while (!IsNull (&FormSet->StorageListHead, Link)) {
    Storage = FORMSET_STORAGE_FROM_LINK (Link);

    OldStorage = NULL;
    if (gOldFormSet != NULL) {
      //
      // Try to find the Storage in backup formset gOldFormSet
      //
      Link2 = GetFirstNode (&gOldFormSet->StorageListHead);
      while (!IsNull (&gOldFormSet->StorageListHead, Link2)) {
        StorageSrc = FORMSET_STORAGE_FROM_LINK (Link2);

        if (StorageSrc->VarStoreId == Storage->VarStoreId) {
          OldStorage = StorageSrc;
          break;
        }

        Link2 = GetNextNode (&gOldFormSet->StorageListHead, Link2);
      }
    }

    if (OldStorage == NULL) {
      //
      // Storage is not found in backup formset, request it from ConfigDriver
      //
      Status = LoadStorage (FormSet, Storage);
    } else {
      //
      // Storage found in backup formset, use it
      //
      Status = CopyStorage (Storage, OldStorage);
    }

    //
    // Now Edit Buffer is filled with default values(lower priority) and current
    // settings(higher priority), sychronize it to shadow Buffer
    //
    SynchronizeStorage (Storage);

    Link = GetNextNode (&FormSet->StorageListHead, Link);
  }

  return EFI_SUCCESS;
}

/**
 Fetch the Ifr binary data of a FormSet.

 @param[in]      Handle         PackageList Handle
 @param[in, out] FormSetGuid    On input, GUID or class GUID of a formset. If not
                                specified (NULL or zero GUID), take the first
                                FormSet with class GUID EFI_HII_PLATFORM_SETUP_FORMSET_GUID
                                found in package list.
                                On output, GUID of the formset found(if not NULL).
 @param[out]     BinaryLength   The length of the FormSet IFR binary.
 @param[out]     BinaryData     The buffer designed to receive the FormSet.

 @retval EFI_SUCCESS            Buffer filled with the requested FormSet.
                                BufferLength was updated.
 @retval EFI_INVALID_PARAMETER  The handle is unknown.
 @retval EFI_NOT_FOUND          A form or FormSet on the requested handle cannot
                                be found with the requested FormId.
**/
EFI_STATUS
GetIfrBinaryData (
  IN  EFI_HII_HANDLE   Handle,
  IN OUT EFI_GUID      *FormSetGuid,
  OUT UINTN            *BinaryLength,
  OUT UINT8            **BinaryData
  )
{
  EFI_STATUS                   Status;
  EFI_HII_PACKAGE_LIST_HEADER  *HiiPackageList;
  UINTN                        BufferSize;
  UINT8                        *Package;
  UINT8                        *OpCodeData;
  UINT32                       Offset;
  UINT32                       Offset2;
  UINT32                       PackageListLength;
  EFI_HII_PACKAGE_HEADER       PackageHeader;
  UINT8                        Index;
  UINT8                        NumberOfClassGuid;
  BOOLEAN                      ClassGuidMatch;
  EFI_GUID                     *ClassGuid;
  EFI_GUID                     *ComparingGuid;

  OpCodeData = NULL;
  Package = NULL;
  ZeroMem (&PackageHeader, sizeof (EFI_HII_PACKAGE_HEADER));

  //
  // if FormSetGuid is NULL or zero GUID, return first Setup FormSet in the package list
  //
  if (FormSetGuid == NULL || CompareGuid (FormSetGuid, &gZeroGuid)) {
    ComparingGuid = &gEfiHiiPlatformSetupFormsetGuid;
  } else {
    ComparingGuid = FormSetGuid;
  }

  //
  // Get HII PackageList
  //
  BufferSize = 0;
  HiiPackageList = NULL;
  Status = mHiiDatabase->ExportPackageLists (mHiiDatabase, Handle, &BufferSize, HiiPackageList);
  if (EFI_ERROR (Status) && Status != EFI_BUFFER_TOO_SMALL) {
    return Status;
  }
  HiiPackageList = AllocatePool (BufferSize);
  ASSERT (HiiPackageList != NULL);
  if (HiiPackageList == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Status = mHiiDatabase->ExportPackageLists (mHiiDatabase, Handle, &BufferSize, HiiPackageList);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Get Form package from this HII package List
  //
  Offset = sizeof (EFI_HII_PACKAGE_LIST_HEADER);
  Offset2 = 0;
  CopyMem (&PackageListLength, &HiiPackageList->PackageLength, sizeof (UINT32));

  ClassGuidMatch = FALSE;
  while (Offset < PackageListLength) {
    Package = ((UINT8 *) HiiPackageList) + Offset;
    CopyMem (&PackageHeader, Package, sizeof (EFI_HII_PACKAGE_HEADER));

    if (PackageHeader.Type == EFI_HII_PACKAGE_FORMS) {
      //
      // Search FormSet in this Form Package
      //
      Offset2 = sizeof (EFI_HII_PACKAGE_HEADER);
      while (Offset2 < PackageHeader.Length) {
        OpCodeData = Package + Offset2;

        if (((EFI_IFR_OP_HEADER *) OpCodeData)->OpCode == EFI_IFR_FORM_SET_OP) {
          //
          // Try to compare against formset GUID
          //
          if (CompareGuid (ComparingGuid, (EFI_GUID *)(OpCodeData + sizeof (EFI_IFR_OP_HEADER)))) {
            break;
          }

          if (((EFI_IFR_OP_HEADER *) OpCodeData)->Length > OFFSET_OF (EFI_IFR_FORM_SET, Flags)) {
            //
            // Try to compare against formset class GUID
            //
            NumberOfClassGuid = (UINT8) (((EFI_IFR_FORM_SET *) OpCodeData)->Flags & 0x3);
            ClassGuid         = (EFI_GUID *) (OpCodeData + sizeof (EFI_IFR_FORM_SET));
            for (Index = 0; Index < NumberOfClassGuid; Index++) {
              if (CompareGuid (ComparingGuid, ClassGuid + Index)) {
                ClassGuidMatch = TRUE;
                break;
              }
            }
            if (ClassGuidMatch) {
              break;
            }
          } else if (ComparingGuid == &gEfiHiiPlatformSetupFormsetGuid) {
            ClassGuidMatch = TRUE;
            break;
          }
        }

        Offset2 += ((EFI_IFR_OP_HEADER *) OpCodeData)->Length;
      }

      if (Offset2 < PackageHeader.Length) {
        //
        // Target formset found
        //
        break;
      }
    }

    Offset += PackageHeader.Length;
  }

  if (Offset >= PackageListLength) {
    //
    // Form package not found in this Package List
    //
    FreePool (HiiPackageList);
    return EFI_NOT_FOUND;
  }

  if (FormSetGuid != NULL) {
    //
    // Return the FormSet GUID
    //
    CopyMem (FormSetGuid, &((EFI_IFR_FORM_SET *) OpCodeData)->Guid, sizeof (EFI_GUID));
  }

  //
  // To determine the length of a whole FormSet IFR binary, one have to parse all the Opcodes
  // in this FormSet; So, here just simply copy the data from start of a FormSet to the end
  // of the Form Package.
  //
  *BinaryLength = PackageHeader.Length - Offset2;
  *BinaryData = AllocateCopyPool (*BinaryLength, OpCodeData);

  FreePool (HiiPackageList);

  if (*BinaryData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  return EFI_SUCCESS;
}

/**
 Initialize the internal data structure of a FormSet.

 @param[in]      Handle         PackageList Handle
 @param[in, out] FormSetGuid    On input, GUID or class GUID of a formset. If not
                                specified (NULL or zero GUID), take the first
                                FormSet with class GUID EFI_HII_PLATFORM_SETUP_FORMSET_GUID
                                found in package list.
                                On output, GUID of the formset found(if not NULL).
 @param[out]     FormSet        FormSet data structure.

 @retval EFI_SUCCESS            The function completed successfully.
 @retval EFI_NOT_FOUND          The specified FormSet could not be found.
**/
EFI_STATUS
InitializeFormSet (
  IN  EFI_HII_HANDLE                   Handle,
  IN OUT EFI_GUID                      *FormSetGuid,
  OUT FORM_BROWSER_FORMSET             *FormSet
  )
{
  EFI_STATUS                Status;
  EFI_HANDLE                DriverHandle;
  UINT16                    Index;

  Status = GetIfrBinaryData (Handle, FormSetGuid, &FormSet->IfrBinaryLength, &FormSet->IfrBinaryData);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  FormSet->HiiHandle = Handle;
  CopyMem (&FormSet->Guid, FormSetGuid, sizeof (EFI_GUID));

  //
  // Retrieve ConfigAccess Protocol associated with this HiiPackageList
  //
  Status = mHiiDatabase->GetPackageListHandle (mHiiDatabase, Handle, &DriverHandle);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  FormSet->DriverHandle = DriverHandle;
  Status = gBS->HandleProtocol (
                  DriverHandle,
                  &gEfiHiiConfigAccessProtocolGuid,
                  (VOID **) &FormSet->ConfigAccess
                  );
  if (EFI_ERROR (Status)) {
    //
    // Configuration Driver don't attach ConfigAccess protocol to its HII package
    // list, then there will be no configuration action required
    //
    FormSet->ConfigAccess = NULL;
  }

  //
  // Parse the IFR binary OpCodes
  //
  FormSet->SubClass = 0xffff;
  Status = ParseOpCodes (FormSet);
  if (EFI_ERROR (Status)) {
    return Status;
  }


  gSecureBootMgr = FALSE;
  if (CompareGuid (&FormSet->Guid, &gSecureBootFormsetGuid)) {
    gSecureBootMgr = TRUE;
  }
  mBootManager = FALSE;
  mBootFromFile = FALSE;
  gClassOfVfr = FormSet->SubClass;
  switch (FormSet->SubClass) {

  case EFI_FRONT_PAGE_SUBCLASS:
    gFrontPageHandle= FormSet->HiiHandle;
    break;
  case EFI_GENERAL_APPLICATION_SUBCLASS :
    mBootManager = TRUE;
    break;

  case EFI_SETUP_APPLICATION_SUBCLASS :
    if (FormSet->Class == EFI_NON_DEVICE_CLASS) {
      mBootFromFile = TRUE;
    }
    break;

  default:
    if (FormSet->SubClass == EFI_USER_ACCESS_TWO || FormSet->SubClass == EFI_USER_ACCESS_THREE) {
      gClassOfVfr = EFI_SETUP_APPLICATION_SUBCLASS;
    }
    break;
  }


  //
  // Match GUID to find out the function key setting. If match fail, use the default setting.
  //
  for (Index = 0; Index < sizeof (gFunctionKeySettingTable) / sizeof (FUNCTIION_KEY_SETTING); Index++) {
    if (CompareGuid (&FormSet->Guid, &(gFunctionKeySettingTable[Index].FormSetGuid))) {
      //
      // Update the function key setting.
      //
      gFunctionKeySetting = gFunctionKeySettingTable[Index].KeySetting;
      //
      // Function key prompt can not be displayed if the function key has been disabled.
      //
      if ((gFunctionKeySetting & FUNCTION_NINE) != FUNCTION_NINE) {
        gFunctionNineString = GetToken (STRING_TOKEN (EMPTY_STRING), gHiiHandle);
      }

      if ((gFunctionKeySetting & FUNCTION_TEN) != FUNCTION_TEN) {
        gFunctionTenString = GetToken (STRING_TOKEN (EMPTY_STRING), gHiiHandle);
      }
    }
  }

  return Status;
}

/**
 Save globals used by previous call to SendForm(). SendForm() may be called from
 HiiConfigAccess.Callback(), this will cause SendForm() be reentried.
 So, save globals of previous call to SendForm() and restore them upon exit.
**/
VOID
SaveBrowserContext (
  VOID
  )
{
  BROWSER_CONTEXT  *Context;

  gBrowserContextCount++;
  if (gBrowserContextCount == 1) {
    //
    // This is not reentry of SendForm(), no context to save
    //
    return;
  }

  Context = AllocatePool (sizeof (BROWSER_CONTEXT));
  ASSERT (Context != NULL);
  if (Context == NULL) {
    return ;
  }

  Context->Signature = BROWSER_CONTEXT_SIGNATURE;

  //
  // Save FormBrowser context
  //
  Context->BannerData           = gBannerData;
  Context->ClassOfVfr           = gClassOfVfr;
  Context->FunctionKeySetting   = gFunctionKeySetting;
  Context->ResetRequired        = gResetRequired;
  Context->NvUpdateRequired     = gNvUpdateRequired;
  Context->Direction            = gDirection;
  Context->FunctionNineString   = gFunctionNineString;
  Context->FunctionTenString    = gFunctionTenString;
  Context->EnterString          = gEnterString;
  Context->EnterCommitString    = gEnterCommitString;
  Context->EnterEscapeString    = gEnterEscapeString;
  Context->EscapeString         = gEscapeString;
  Context->SaveFailed           = gSaveFailed;
  Context->MoveHighlight        = gMoveHighlight;
  Context->MakeSelection        = gMakeSelection;
  Context->DecNumericInput      = gDecNumericInput;
  Context->HexNumericInput      = gHexNumericInput;
  Context->ToggleCheckBox       = gToggleCheckBox;
  Context->PromptForData        = gPromptForData;
  Context->PromptForPassword    = gPromptForPassword;
  Context->PromptForNewPassword = gPromptForNewPassword;
  Context->ConfirmPassword      = gConfirmPassword;
  Context->ConfirmError         = gConfirmError;
  Context->PassowordInvalid     = gPassowordInvalid;
  Context->PressEnter           = gPressEnter;
  Context->EmptyString          = gEmptyString;
  Context->AreYouSure           = gAreYouSure;
  Context->YesResponse          = gYesResponse;
  Context->NoResponse           = gNoResponse;
  Context->MiniString           = gMiniString;
  Context->PlusString           = gPlusString;
  Context->MinusString          = gMinusString;
  Context->AdjustNumber         = gAdjustNumber;
  Context->SaveChanges          = gSaveChanges;
  Context->OptionMismatch       = gOptionMismatch;
  Context->PromptBlockWidth     = gPromptBlockWidth;
  Context->OptionBlockWidth     = gOptionBlockWidth;
  Context->HelpBlockWidth       = gHelpBlockWidth;
  Context->OldFormSet           = gOldFormSet;
  Context->MenuRefreshHead      = gMenuRefreshHead;

  CopyMem (&Context->ScreenDimensions, &gScreenDimensions, sizeof (gScreenDimensions));
  CopyMem (&Context->Menu, &Menu, sizeof (Menu));

  //
  // Insert to FormBrowser context list
  //
  InsertHeadList (&gBrowserContextList, &Context->Link);
}

/**
 Restore globals used by previous call to SendForm().
**/
VOID
RestoreBrowserContext (
  VOID
  )
{
  LIST_ENTRY       *Link;
  BROWSER_CONTEXT  *Context;

  ASSERT (gBrowserContextCount != 0);
  gBrowserContextCount--;
  if (gBrowserContextCount == 0) {
    //
    // This is not reentry of SendForm(), no context to restore
    //
    return;
  }

  ASSERT (!IsListEmpty (&gBrowserContextList));

  Link = GetFirstNode (&gBrowserContextList);
  Context = BROWSER_CONTEXT_FROM_LINK (Link);

  //
  // Restore FormBrowser context
  //
  gBannerData           = Context->BannerData;
  gClassOfVfr           = Context->ClassOfVfr;
  gFunctionKeySetting   = Context->FunctionKeySetting;
  gResetRequired        = Context->ResetRequired;
  gNvUpdateRequired     = Context->NvUpdateRequired;
  gDirection            = Context->Direction;
  gFunctionNineString   = Context->FunctionNineString;
  gFunctionTenString    = Context->FunctionTenString;
  gEnterString          = Context->EnterString;
  gEnterCommitString    = Context->EnterCommitString;
  gEnterEscapeString    = Context->EnterEscapeString;
  gEscapeString         = Context->EscapeString;
  gSaveFailed           = Context->SaveFailed;
  gMoveHighlight        = Context->MoveHighlight;
  gMakeSelection        = Context->MakeSelection;
  gDecNumericInput      = Context->DecNumericInput;
  gHexNumericInput      = Context->HexNumericInput;
  gToggleCheckBox       = Context->ToggleCheckBox;
  gPromptForData        = Context->PromptForData;
  gPromptForPassword    = Context->PromptForPassword;
  gPromptForNewPassword = Context->PromptForNewPassword;
  gConfirmPassword      = Context->ConfirmPassword;
  gConfirmError         = Context->ConfirmError;
  gPassowordInvalid     = Context->PassowordInvalid;
  gPressEnter           = Context->PressEnter;
  gEmptyString          = Context->EmptyString;
  gAreYouSure           = Context->AreYouSure;
  gYesResponse          = Context->YesResponse;
  gNoResponse           = Context->NoResponse;
  gMiniString           = Context->MiniString;
  gPlusString           = Context->PlusString;
  gMinusString          = Context->MinusString;
  gAdjustNumber         = Context->AdjustNumber;
  gSaveChanges          = Context->SaveChanges;
  gOptionMismatch       = Context->OptionMismatch;
  gPromptBlockWidth     = Context->PromptBlockWidth;
  gOptionBlockWidth     = Context->OptionBlockWidth;
  gHelpBlockWidth       = Context->HelpBlockWidth;
  gOldFormSet           = Context->OldFormSet;
  gMenuRefreshHead      = Context->MenuRefreshHead;

  CopyMem (&gScreenDimensions, &Context->ScreenDimensions, sizeof (gScreenDimensions));
  CopyMem (&Menu, &Context->Menu, sizeof (Menu));

  //
  // Remove from FormBrowser context list
  //
  RemoveEntryList (&Context->Link);
  gBS->FreePool (Context);
}

/**
 This function will return animation package duplication

 @param[in]  Handle             Hii Handle
 @param[out] AnimationPackage   Animation Package duplication

 @retval EFI_SUCCESS            Get animation success
 @retval EFI_INVALID_PARAMETER  Invalid parameter
 @retval EFI_BUFFER_TOO_SMALL   Buffer too small
 @retval EFI_NOT_FOUND          Can't found animation package by handle
**/
EFI_STATUS
GetAnimationPackage (
  IN  EFI_HII_HANDLE                  Handle,
  OUT EFI_HII_ANIMATION_PACKAGE_HDR   **AnimationPackage
  )
{
  EFI_STATUS                    Status;
  EFI_HII_PACKAGE_LIST_HEADER   *HiiPackageList;
  EFI_HII_PACKAGE_HEADER        *Package;
  UINT32                        PackageLength;
  UINTN                         BufferSize;
  UINTN                         Offset;

  if (AnimationPackage == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *AnimationPackage = NULL;

  //
  // Get the orginal package list
  //
  BufferSize = 0;
  HiiPackageList   = NULL;
  Status = mHiiDatabase->ExportPackageLists (mHiiDatabase, Handle, &BufferSize, HiiPackageList);
  if (EFI_ERROR (Status) && Status != EFI_BUFFER_TOO_SMALL) {
    return Status;
  }
  HiiPackageList = AllocatePool (BufferSize);
  ASSERT (HiiPackageList != NULL);
  if (HiiPackageList == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = mHiiDatabase->ExportPackageLists (mHiiDatabase, Handle, &BufferSize, HiiPackageList);
  if (EFI_ERROR (Status)) {
    gBS->FreePool (HiiPackageList);
    return Status;
  }

  Status = EFI_NOT_FOUND;
  Offset = sizeof (EFI_HII_PACKAGE_LIST_HEADER);
  PackageLength = HiiPackageList->PackageLength;
  while (Offset < PackageLength) {
    Package = (EFI_HII_PACKAGE_HEADER *) (((UINT8 *) HiiPackageList) + Offset);
    //
    // Only one animation package in one hiipackagelist
    //
    if (Package->Type == EFI_HII_PACKAGE_ANIMATIONS) {
      *AnimationPackage = AllocatePool (PackageLength);
      if (*AnimationPackage != NULL) {
        CopyMem (*AnimationPackage, Package, PackageLength);
        Status = EFI_SUCCESS;
      } else {
        Status = EFI_BUFFER_TOO_SMALL;
      }
      break;
    }
    Offset += Package->Length;
  }

  if (Offset >= PackageLength) {
    Status = EFI_NOT_FOUND;
  }

  gBS->FreePool (HiiPackageList);
  return Status;
}

/**
 Find the matched FormSet context in the backup maintain list based on HiiHandle.

 @param[in] Handle  The Hii Handle.

 @return the found FormSet context. If no found, NULL will return.
**/
FORM_BROWSER_FORMSET *
GetFormSetFromHiiHandle (
  IN EFI_HII_HANDLE Handle
  )
{
  LIST_ENTRY           *Link;
  FORM_BROWSER_FORMSET *FormSet;

  Link = GetFirstNode (&gBrowserFormSetList);
  while (!IsNull (&gBrowserFormSetList, Link)) {
    FormSet = FORM_BROWSER_FORMSET_FROM_LINK (Link);
    if (FormSet->HiiHandle == Handle) {
      return FormSet;
    }
    Link = GetNextNode (&gBrowserFormSetList, Link);
  }

  return NULL;
}

/**
 Check whether the input HII handle is the FormSet that is being used.

 @param[in] Handle  The Hii Handle.

 @retval TRUE   HII handle is being used.
 @retval FALSE  HII handle is not being used.
**/
BOOLEAN
IsHiiHandleInBrowserContext (
  IN EFI_HII_HANDLE Handle
  )
{
  LIST_ENTRY       *Link;
  BROWSER_CONTEXT  *Context;

  //
  // HiiHandle is Current FormSet.
  //
  if ((gOldFormSet != NULL) && (gOldFormSet->HiiHandle == Handle)) {
    return TRUE;
  }

  //
  // Check whether HiiHandle is in BrowserContext.
  //
  Link = GetFirstNode (&gBrowserContextList);
  while (!IsNull (&gBrowserContextList, Link)) {
    Context = BROWSER_CONTEXT_FROM_LINK (Link);
    if (Context->OldFormSet->HiiHandle == Handle) {
      //
      // HiiHandle is in BrowserContext
      //
      return TRUE;
    }
    Link = GetNextNode (&gBrowserContextList, Link);
  }

  return FALSE;
}

/**
 Find the registered HotKey based on KeyData.

 @param[in] KeyData     A pointer to a buffer that describes the keystroke
                        information for the hot key.

 @return The registered HotKey context. If no found, NULL will return.
**/
BROWSER_HOT_KEY *
GetHotKeyFromRegisterList (
  IN EFI_INPUT_KEY *KeyData
  )
{
  LIST_ENTRY       *Link;
  BROWSER_HOT_KEY  *HotKey;

  Link = GetFirstNode (&gBrowserHotKeyList);
  while (!IsNull (&gBrowserHotKeyList, Link)) {
    HotKey = BROWSER_HOT_KEY_FROM_LINK (Link);
    if (HotKey->KeyData->ScanCode == KeyData->ScanCode) {
      return HotKey;
    }
    Link = GetNextNode (&gBrowserHotKeyList, Link);
  }

  return NULL;
}

/**
 Configure what scope the hot key will impact.
 All hot keys have the same scope. The mixed hot keys with the different level are not supported.
 If no scope is set, the default scope will be FormSet level.
 After all registered hot keys are removed, previous Scope can reset to another level.

 @param[in] Scope               Scope level to be set.

 @retval EFI_SUCCESS            Scope is set correctly.
 @retval EFI_INVALID_PARAMETER  Scope is not the valid value specified in BROWSER_SETTING_SCOPE.
 @retval EFI_UNSPPORTED         Scope level is different from current one that the registered hot keys have.
**/
EFI_STATUS
EFIAPI
SetScope (
  IN BROWSER_SETTING_SCOPE Scope
  )
{
  if (Scope >= MaxLevel) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // When no hot key registered in system or on the first setting,
  // Scope can be set.
  //
  if (mBrowserScopeFirstSet || IsListEmpty (&gBrowserHotKeyList)) {
    gBrowserSettingScope  = Scope;
    mBrowserScopeFirstSet = FALSE;
  } else if (Scope != gBrowserSettingScope) {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

/**
 Register the hot key with its browser action, or unregistered the hot key.
 Only support hot key that is not printable character (control key, function key, etc.).
 If the action value is zero, the hot key will be unregistered if it has been registered.
 If the same hot key has been registered, the new action and help string will override the previous ones.

 @param[in] KeyData     A pointer to a buffer that describes the keystroke
                        information for the hot key. Its type is EFI_INPUT_KEY to
                        be supported by all ConsoleIn devices.
 @param[in] Action      Action value that describes what action will be trigged when the hot key is pressed.
 @param[in] DefaultId   Specifies the type of defaults to retrieve, which is only for DEFAULT action.
 @param[in] HelpString  Help string that describes the hot key information.
                        Its value may be NULL for the unregistered hot key.

 @retval EFI_SUCCESS            Hot key is registered or unregistered.
 @retval EFI_INVALID_PARAMETER  KeyData is NULL or HelpString is NULL on register.
 @retval EFI_NOT_FOUND          KeyData is not found to be unregistered.
 @retval EFI_UNSUPPORTED        Key represents a printable character. It is conflicted with Browser.
**/
EFI_STATUS
EFIAPI
RegisterHotKey (
  IN EFI_INPUT_KEY *KeyData,
  IN UINT32        Action,
  IN UINT16        DefaultId,
  IN EFI_STRING    HelpString OPTIONAL
  )
{
  BROWSER_HOT_KEY  *HotKey;

  //
  // Check input parameters.
  //
  if (KeyData == NULL || KeyData->UnicodeChar != CHAR_NULL ||
     (Action != BROWSER_ACTION_UNREGISTER && HelpString == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check whether the input KeyData is in BrowserHotKeyList.
  //
  HotKey = GetHotKeyFromRegisterList (KeyData);

  //
  // Unregister HotKey
  //
  if (Action == BROWSER_ACTION_UNREGISTER) {
    if (HotKey != NULL) {
      //
      // The registered HotKey is found.
      // Remove it from List, and free its resource.
      //
      RemoveEntryList (&HotKey->Link);
      FreePool (HotKey->KeyData);
      FreePool (HotKey->HelpString);
      return EFI_SUCCESS;
    } else {
      //
      // The registered HotKey is not found.
      //
      return EFI_NOT_FOUND;
    }
  }

  //
  // Register HotKey into List.
  //
  if (HotKey == NULL) {
    //
    // Create new Key, and add it into List.
    //
    HotKey = AllocateZeroPool (sizeof (BROWSER_HOT_KEY));
    ASSERT (HotKey != NULL);
    if (HotKey == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    HotKey->Signature = BROWSER_HOT_KEY_SIGNATURE;
    HotKey->KeyData   = AllocateCopyPool (sizeof (EFI_INPUT_KEY), KeyData);
    InsertTailList (&gBrowserHotKeyList, &HotKey->Link);
  }

  //
  // Fill HotKey information.
  //
  HotKey->Action     = Action;
  HotKey->DefaultId  = DefaultId;
  if (HotKey->HelpString != NULL) {
    FreePool (HotKey->HelpString);
  }
  HotKey->HelpString = AllocateCopyPool (StrSize (HelpString), HelpString);

  return EFI_SUCCESS;
}

/**
 Register Exit handler function.
 When more than one handler function is registered, the latter one will override the previous one.
 When NULL handler is specified, the previous Exit handler will be unregistered.

 @param[in] Handler      Pointer to handler function.
**/
VOID
EFIAPI
RegiserExitHandler (
  IN EXIT_HANDLER Handler
  )
{
  ExitHandlerFunction = Handler;
  return;
}

/**
 Create reminder to let user to choose save or discard the changed browser data.
 Caller can use it to actively check the changed browser data.

 @retval BROWSER_NO_CHANGES       No browser data is changed.
 @retval BROWSER_SAVE_CHANGES     The changed browser data is saved.
 @retval BROWSER_DISCARD_CHANGES  The changed browser data is discard.
**/
UINT32
EFIAPI
SaveReminder (
  VOID
  )
{
  LIST_ENTRY              *Link;
  FORM_BROWSER_FORMSET    *FormSet;
  BOOLEAN                 IsDataChanged;
  UINT32                  DataSavedAction;
  CHAR16                  *YesResponse;
  CHAR16                  *NoResponse;
  CHAR16                  *EmptyString;
  CHAR16                  *ChangeReminderString;
  CHAR16                  *SaveConfirmString;
  EFI_INPUT_KEY           Key;

  DataSavedAction  = BROWSER_NO_CHANGES;
  IsDataChanged    = FALSE;
  Link = GetFirstNode (&gBrowserFormSetList);
  while (!IsNull (&gBrowserFormSetList, Link)) {
    FormSet = FORM_BROWSER_FORMSET_FROM_LINK (Link);
    if (IsNvUpdateRequired (FormSet)) {
      IsDataChanged = TRUE;
      break;
    }
    Link = GetNextNode (&gBrowserFormSetList, Link);
  }

  //
  // No data is changed. No save is required.
  //
  if (!IsDataChanged) {
    return DataSavedAction;
  }

  //
  // If data is changed, prompt user
  //
  gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);

  YesResponse          = GetToken (STRING_TOKEN (ARE_YOU_SURE_YES), gHiiHandle);
  ASSERT (YesResponse != NULL);
  NoResponse           = GetToken (STRING_TOKEN (ARE_YOU_SURE_NO), gHiiHandle);
  ASSERT (NoResponse  != NULL);
  EmptyString          = GetToken (STRING_TOKEN (EMPTY_STRING), gHiiHandle);
  ChangeReminderString = GetToken (STRING_TOKEN (CHANGE_REMINDER), gHiiHandle);
  SaveConfirmString    = GetToken (STRING_TOKEN (SAVE_CONFIRM), gHiiHandle);

  if (YesResponse == NULL || NoResponse == NULL || EmptyString == NULL || SaveConfirmString == NULL ||
      ChangeReminderString == NULL) {
    return DataSavedAction;
  }
  do {
    CreateDialog (4, TRUE, 0, NULL, &Key, EmptyString, ChangeReminderString, SaveConfirmString, EmptyString);
  } while
  (((Key.UnicodeChar | UPPER_LOWER_CASE_OFFSET) != (NoResponse[0] | UPPER_LOWER_CASE_OFFSET)) &&
   ((Key.UnicodeChar | UPPER_LOWER_CASE_OFFSET) != (YesResponse[0] | UPPER_LOWER_CASE_OFFSET))
  );

  //
  // If the user hits the YesResponse key
  //
  if ((Key.UnicodeChar | UPPER_LOWER_CASE_OFFSET) == (YesResponse[0] | UPPER_LOWER_CASE_OFFSET)) {
    SubmitForm (NULL, NULL);
    DataSavedAction = BROWSER_SAVE_CHANGES;
  } else {
    DataSavedAction = BROWSER_DISCARD_CHANGES;
    gResetRequired  = FALSE;
  }

  FreePool (YesResponse);
  FreePool (NoResponse);
  FreePool (EmptyString);
  FreePool (SaveConfirmString);
  FreePool (ChangeReminderString);

  return DataSavedAction;
}
