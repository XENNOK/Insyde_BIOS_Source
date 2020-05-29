/** @file
 DXE IPMI Setup Utility library implement code - IPMI Config.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/


#include <Library/DxeIpmiSetupUtilityLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/HiiLib.h>
#include <Library/SetupUtilityLib.h>
#include <Library/PcdLib.h>
#include <Library/IpmiHobLib.h>
#include <Library/IpmiCommonLib.h>

#include <ChipsetSetupConfig.h>
#include <H2OIpmi.h>

#include <Guid/MdeModuleHii.h>
#include <Guid/H2OIpmiHob.h>

#include <Protocol/H2OIpmiInterfaceProtocol.h>
#include <Protocol/H2OIpmiSdrProtocol.h>


#define IPMI_CONFIG_TEMP_STR_LEN        0x40
//
// IPv4 maximun length format: 192.168.100.100
//
#define BMC_IPV4_MAX_STRING_LENGTH      15
#define BMC_IPV4_POPUP_DIALOG_WIDTH     30
//
// IPv6 maximun length format: 0000:0000:0000:0000:0000:0000:192.168.100.100
//
#define BMC_IPV6_MAX_STRING_LENGTH      45
#define BMC_IPV6_POPUP_DIALOG_WIDTH     52
#define BMC_TEXT_INPUT_BOX_HEIGHT       7

#define INPUT_FORMAT_IPV4               1
#define INPUT_FORMAT_IPV6               2

#define SDR_ID_FIELD_WIDTH              3
#define SDR_NAME_FIELD_WIDTH            20
#define SDR_VALUE_FIELD_WIDTH           10


UINT8       mBmcStatus = 0;
UINT8       mBmcLanChannel = 0;
UINT8       mBmcPrevLanChannel;
UINT16      mBmcLanChannelBitmap;

//
// The first time get necessary Ipmi Hii string ID is setuputility init advance menu.
// So store the Ipmi Hii string ID when OemIpmiConfigUpdate be called for after required.
//
IPMI_STR_ID mIpmiStrIdStorage;


/**
 Wait for a given event to fire, or for an optional timeout to expire.

 @param[in]         Event               The event to wait for.
 @param[in]         Timeout OPTIONAL    An optional timeout value in 100 ns units.

 @retval EFI_SUCCESS     Event fired before Timeout expired.
 @retval EFI_TIME_OUT    Timout expired before Event fired.
*/
EFI_STATUS
WaitForKeyEvent (
  IN  EFI_EVENT                         Event,
  IN  UINT64                            Timeout OPTIONAL
  )
{
  EFI_STATUS                            Status;
  UINTN                                 Index;
  EFI_EVENT                             TimerEvent;
  EFI_EVENT                             WaitList[2];

  //
  // Update screen every second
  //
  Timeout = 10000000;

  do {

    Status = gBS->CreateEvent (EVT_TIMER, 0, NULL, NULL, &TimerEvent);

    //
    // Set the timer event
    //
    gBS->SetTimer (
           TimerEvent,
           TimerRelative,
           Timeout
           );

    //
    // Wait for the original event or the timer
    //
    WaitList[0] = Event;
    WaitList[1] = TimerEvent;
    Status      = gBS->WaitForEvent (2, WaitList, &Index);

    gBS->CloseEvent (TimerEvent);

  } while (Status == EFI_TIMEOUT);

  return Status;

}


/**
 Wait for a given event to fire, or for an optional timeout to expire.

 @param[in]         Key                 The input char.

 @retval EFI_SUCCESS     Event fired before Timeout expired.
 @retval EFI_TIME_OUT    Timout expired before Event fired.
*/
EFI_STATUS
WaitForKeyStroke (
  OUT  EFI_INPUT_KEY                    *Key
  )
{
  EFI_STATUS                            Status;

  do {

    WaitForKeyEvent (gST->ConIn->WaitForKey, 0);
    Status = gST->ConIn->ReadKeyStroke (gST->ConIn, Key);

  } while (EFI_ERROR(Status));

  return Status;

}


/**
 Check if the character is a valid character or not.

 @param[in]         InputChar           The character to be checked.
 @param[in]         Flag                IPv4 format or IPv6 format.

 @retval TRUE       The character is a valid character in given flag format.
 @retval FALSE      The character is an invalid character in given flag format.
*/
BOOLEAN
IsValidChar (
  IN  CHAR16                            InputChar,
  IN  UINT8                             Flag
  )
{

  if (Flag & INPUT_FORMAT_IPV4) {
    if (InputChar == '.' || (InputChar >= '0' && InputChar <= '9')) {
      return TRUE;
    }
  } else {
    if (InputChar == '.' ||
        InputChar == ':' ||
        (InputChar >= '0' && InputChar <= '9') ||
        (InputChar >= 'a' && InputChar <= 'f') ||
        (InputChar >= 'A' && InputChar <= 'F')) {
      return TRUE;
    }
  }

  return FALSE;

}


/**
 Pop-up a text input box and let user input text. The input text will be stored
 in an allocated buffer, and it is caller's reponsibility to free it.

 @param[in]         Title               Title for the text input box.
 @param[in]         Flag                Currently only for IPv4 format and IPv6 format.
 @param[in]         StringBuffer        An allocated buffer to store user's input. It is caller's
                                        reponsibility to free it.

 @retval EFI_SUCCESS       User press Enter to finish input.
 @retval EFI_UNSUPPORTED   User press ESC to cancel input.
*/
EFI_STATUS
TextInputBox (
  IN  CHAR16                            *Title,
  IN  UINT8                             Flag,
  OUT CHAR16                            **StringBuffer
  )
{
  UINTN                                 Columns;
  UINTN                                 Rows;
  CHAR16                                CleanLine[80];
  BOOLEAN                               SelectionComplete;
  UINTN                                 CurrentAttribute;
  BOOLEAN                               NeedUpdate;
  UINTN                                 StrPos;
  UINTN                                 StrLength;
  CHAR16                                *InStr;
  EFI_INPUT_KEY                         Key;
  EFI_STATUS                            Status;
  UINTN                                 BoxColumns;
  UINTN                                 BoxRows;
  UINTN                                 BoxLeft;
  UINTN                                 BoxRight;
  UINTN                                 BoxTop;
  UINTN                                 BoxBottom;
  UINTN                                 Index;
  UINTN                                 TitleLength;
  CHAR16                                TempChar[2];


  //
  // Variable initial
  //
  BoxRows = BMC_TEXT_INPUT_BOX_HEIGHT;
  if (Flag & INPUT_FORMAT_IPV4) {
    BoxColumns = BMC_IPV4_POPUP_DIALOG_WIDTH;
    StrLength = BMC_IPV4_MAX_STRING_LENGTH;
  } else {
    BoxColumns = BMC_IPV6_POPUP_DIALOG_WIDTH;
    StrLength = BMC_IPV6_MAX_STRING_LENGTH;
  }

  for (Index = 0; Index < BoxColumns; ++Index) {
    CleanLine[Index] = L' ';
  }
  CleanLine[Index] = L'\0';
  TempChar[1] = L'\0';

 
  //
  // Disable cursor
  //
  CurrentAttribute  = gST->ConOut->Mode->Attribute;
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);

  //
  // Get screen size and calculate left, right, top and bottom of this box
  //
  gST->ConOut->QueryMode (gST->ConOut, gST->ConOut->Mode->Mode, &Columns, &Rows);
  BoxLeft = (Columns - BoxColumns) / 2;
  BoxRight = BoxLeft + BoxColumns - 1;
  BoxTop = (Rows - BoxRows) / 2;
  BoxBottom = BoxTop + BoxRows - 1;

  //
  // Set color attribute and draw it
  //
  gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_LIGHTGRAY, EFI_BLUE));
  for (Index = BoxTop; Index <= BoxBottom; ++Index) {
    gST->ConOut->SetCursorPosition (gST->ConOut, BoxLeft, Index);
    gST->ConOut->OutputString (gST->ConOut, CleanLine);
  }


  //
  // Print  border line
  // +-------------------------------------------+
  // |                                           |
  // |-------------------------------------------|
  // |                                           |
  // |                                           |
  // |                                           |
  // +-------------------------------------------+
  //
  // 1. Four corner
  //
  TempChar[0] = BOXDRAW_DOWN_RIGHT;
  gST->ConOut->SetCursorPosition (gST->ConOut, BoxLeft, BoxTop);
  gST->ConOut->OutputString (gST->ConOut, TempChar);
  TempChar[0] = BOXDRAW_UP_RIGHT;
  gST->ConOut->SetCursorPosition (gST->ConOut, BoxLeft, BoxBottom);
  gST->ConOut->OutputString (gST->ConOut, TempChar);
  TempChar[0] = BOXDRAW_DOWN_LEFT;
  gST->ConOut->SetCursorPosition (gST->ConOut, BoxRight, BoxTop);
  gST->ConOut->OutputString (gST->ConOut, TempChar);
  TempChar[0] = BOXDRAW_UP_LEFT;
  gST->ConOut->SetCursorPosition (gST->ConOut, BoxRight, BoxBottom);
  gST->ConOut->OutputString (gST->ConOut, TempChar);


  //
  // 2. Three horizontal line
  //
  for (Index = BoxLeft + 1; Index < BoxRight; Index++) {
    TempChar[0] = BOXDRAW_HORIZONTAL;
    gST->ConOut->SetCursorPosition (gST->ConOut, Index, BoxTop);
    gST->ConOut->OutputString (gST->ConOut, TempChar);
    TempChar[0] = BOXDRAW_HORIZONTAL;
    gST->ConOut->SetCursorPosition (gST->ConOut, Index, BoxTop + 2);
    gST->ConOut->OutputString (gST->ConOut, TempChar);
    TempChar[0] = BOXDRAW_HORIZONTAL;
    gST->ConOut->SetCursorPosition (gST->ConOut, Index, BoxBottom);
    gST->ConOut->OutputString (gST->ConOut, TempChar);
  }


  //
  // 3. Two vertical line
  //
  for (Index = BoxTop + 1; Index < BoxBottom; Index++) {
    TempChar[0] = BOXDRAW_VERTICAL;
    gST->ConOut->SetCursorPosition (gST->ConOut, BoxLeft, Index);
    gST->ConOut->OutputString (gST->ConOut, TempChar);
    TempChar[0] = BOXDRAW_VERTICAL;
    gST->ConOut->SetCursorPosition (gST->ConOut, BoxRight, Index);
    gST->ConOut->OutputString (gST->ConOut, TempChar);
  }


  //
  // 4. Vertical right and left
  //
  TempChar[0] = BOXDRAW_VERTICAL_RIGHT;
  gST->ConOut->SetCursorPosition (gST->ConOut, BoxLeft, BoxTop + 2);
  gST->ConOut->OutputString (gST->ConOut, TempChar);
  TempChar[0] = BOXDRAW_VERTICAL_LEFT;
  gST->ConOut->SetCursorPosition (gST->ConOut, BoxRight, BoxTop + 2);
  gST->ConOut->OutputString (gST->ConOut, TempChar);


  //
  // 5. Input area
  //
  gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_BLACK, EFI_LIGHTGRAY));
  for (Index = BoxLeft + 2; Index < BoxRight - 1; ++Index) {
    gST->ConOut->SetCursorPosition (gST->ConOut, Index, BoxTop + 4);
    gST->ConOut->OutputString (gST->ConOut, L" ");
  }


  //
  // 6. Title
  //
  gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_LIGHTGRAY, EFI_BLUE));
  TitleLength = StrLen (Title);
  gST->ConOut->SetCursorPosition (gST->ConOut, BoxLeft + ((BoxColumns - TitleLength) / 2), BoxTop + 1);
  gST->ConOut->OutputString (gST->ConOut, Title);
  gST->ConOut->SetAttribute (gST->ConOut, CurrentAttribute);


  //
  // Allocate big enough buffer
  //
  InStr = AllocateZeroPool ((StrLength + 0x10) * sizeof (CHAR16));
  if (InStr == NULL) {
    return EFI_UNSUPPORTED;
  }


  //
  // Start to process user input
  //
  StrPos = 0;
  NeedUpdate = FALSE;
  SelectionComplete = FALSE;
  Key.UnicodeChar = 0;
  Key.ScanCode = 0;
  Status = EFI_UNSUPPORTED;

  while (!SelectionComplete) {
    Status = WaitForKeyStroke (&Key);

    switch (Key.UnicodeChar) {

    case CHAR_NULL:
      switch (Key.ScanCode) {

      case SCAN_ESC:
        gST->ConOut->SetAttribute (gST->ConOut, CurrentAttribute);
        FreePool (InStr);

        *StringBuffer = NULL;
        Status = EFI_UNSUPPORTED;
        SelectionComplete = TRUE;

      default:
        break;
      }

      break;

    case CHAR_CARRIAGE_RETURN:
      *StringBuffer = InStr;
      Status = EFI_SUCCESS;
      SelectionComplete = TRUE;

      break;

    case CHAR_BACKSPACE:
      if (StrPos) {
        //
        // If not move back beyond string beginning, move all characters behind
        // the current position one character forward
        //
        --StrPos;
        InStr[StrPos] = L'\0';
        NeedUpdate = TRUE;
      }
      break;

    default:
      if (IsValidChar (Key.UnicodeChar, Flag)) {
        //
        // If we are at the buffer's end, drop the key
        //
        if (StrPos < StrLength) {
          InStr[StrPos] = Key.UnicodeChar;
          NeedUpdate = TRUE;
          ++StrPos;
        }
      }
      break;

    }

    //
    // Check if we need update add/remove character
    //
    if (NeedUpdate) {
      gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_BLACK, EFI_LIGHTGRAY));
      gST->ConOut->SetCursorPosition (gST->ConOut, BoxLeft + 2, BoxBottom - 2);
      CleanLine[StrLength] = L'\0';
      gST->ConOut->OutputString (gST->ConOut, CleanLine);
      CleanLine[StrLength] = L' ';
      gST->ConOut->SetCursorPosition (gST->ConOut, BoxLeft + 2, BoxBottom - 2);
      gST->ConOut->OutputString (gST->ConOut, InStr);
      gST->ConOut->SetAttribute (gST->ConOut, CurrentAttribute);

      NeedUpdate = FALSE;
    }

  };

  return Status;

}


/**
 Get SDRs information from BMC.

 @param[in]         HiiHandle           EFI_HII_HANDLE.

*/
VOID
UpdateBmcSdrList (
  IN  EFI_HII_HANDLE                    HiiHandle
  )
{
  EFI_STATUS                            Status;
  H2O_IPMI_SDR_PROTOCOL                 *Sdr;
  SDR_DATA_STURCT                       *SdrBuffer;
  UINT16                                SdrCount;
  UINT16                                Index;
  CHAR16                                HelpStr[0x1000];
  CHAR16                                TempStr[0x40];
  CHAR16                                SpaceStr[0x20];
  EFI_STRING_ID                         TokenToUpdate;
  EFI_STRING_ID                         HelpTokenToUpdate;
  UINTN                                 Length;
  VOID						                      *StartOpCodeHandle;
  VOID                                  *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL                    *StartLabel;
  EFI_IFR_GUID_LABEL                    *EndLabel;

  Status = gBS->LocateProtocol (&gH2OIpmiSdrProtocolGuid, NULL, (VOID **)&Sdr);
  if (EFI_ERROR (Status)) {
    return;
  }

  Status = Sdr->GetSdrData (Sdr, SDR_ID_ALL, 0x0300, &SdrCount, &SdrBuffer);
  if (EFI_ERROR (Status)) {
    return;
  }

  //
  // Start to update SDR list
  //
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  EndOpCodeHandle = HiiAllocateOpCodeHandle ();

  StartLabel = (EFI_IFR_GUID_LABEL*) HiiCreateGuidOpCode (
                                       StartOpCodeHandle,
                                       &gEfiIfrTianoGuid,
                                       NULL,
                                       sizeof (EFI_IFR_GUID_LABEL)
                                       );

  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = IPMI_SDR_LIST_LABEL;

  EndLabel = (EFI_IFR_GUID_LABEL*) HiiCreateGuidOpCode (
                                     EndOpCodeHandle,
                                     &gEfiIfrTianoGuid,
                                     NULL,
                                     sizeof (EFI_IFR_GUID_LABEL)
                                     );

  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = IPMI_SDR_LIST_END_LABEL;

  ZeroMem (HelpStr, sizeof (HelpStr));
  ZeroMem (TempStr, sizeof (TempStr));
  StrCpy (SpaceStr, L"                              ");
  for (Index = 0; Index < SdrCount; ++Index) {
    TempStr[0] = 0x20;
    UnicodeValueToString (
      &TempStr[1],
      PREFIX_ZERO,
      (UINT64)SdrBuffer[Index].RecordId,
      SDR_ID_FIELD_WIDTH
      );
    StrnCat (TempStr, SpaceStr, SDR_ID_FIELD_WIDTH);
    Length = StrLen (SdrBuffer[Index].Name);
    StrCat (TempStr, SdrBuffer[Index].Name);
    if (Length < SDR_NAME_FIELD_WIDTH) {
      StrnCat (TempStr, SpaceStr, (UINTN)(SDR_NAME_FIELD_WIDTH - Length));
    }
    Length = StrLen (SdrBuffer[Index].ValueStr);
    StrCat (TempStr, SdrBuffer[Index].ValueStr);
    if (Length < SDR_VALUE_FIELD_WIDTH) {
      StrnCat (TempStr, SpaceStr, (UINTN)(SDR_VALUE_FIELD_WIDTH - Length));
    }
    StrCat (TempStr, SdrBuffer[Index].Unit);

    //
    // Print other information in help field
    //
    UnicodeSPrint (HelpStr, sizeof (HelpStr), L"Sensor Number: 0x%02x\n", SdrBuffer[Index].SensorNumber);
    if (SdrBuffer[Index].DataStatus & SDR_SENSOR_TYPE_ANALOG) {
      StrCat (HelpStr, L"Lower Non-Critical Threshold: ");
      StrCat (HelpStr, SdrBuffer[Index].LowNonCriticalThresStr);
      StrCat (HelpStr, L"\n");
      StrCat (HelpStr, L"Lower Critical Threshold: ");
      StrCat (HelpStr, SdrBuffer[Index].LowCriticalThresStr);
      StrCat (HelpStr, L"\n");
      StrCat (HelpStr, L"Lower Non-Recoverable Threshold: ");
      StrCat (HelpStr, SdrBuffer[Index].LowNonRecoverThresStr);
      StrCat (HelpStr, L"\n");
      StrCat (HelpStr, L"Upper Non-Critical Threshold: ");
      StrCat (HelpStr, SdrBuffer[Index].UpNonCriticalThresStr);
      StrCat (HelpStr, L"\n");
      StrCat (HelpStr, L"Upper Critical Threshold: ");
      StrCat (HelpStr, SdrBuffer[Index].UpCriticalThresStr);
      StrCat (HelpStr, L"\n");
      StrCat (HelpStr, L"Upper Non-Recoverable Threshold: ");
      StrCat (HelpStr, SdrBuffer[Index].UpNonRecoverThresStr);
    } else {
      StrCat (HelpStr, L"This is not an analog sensor, no threshold data.");
    }

    TokenToUpdate = HiiSetString (HiiHandle, 0, TempStr, NULL);
    HelpTokenToUpdate = HiiSetString (HiiHandle, 0, HelpStr, NULL);

    HiiCreateGotoOpCode (
      StartOpCodeHandle,
      0,
      TokenToUpdate,
      HelpTokenToUpdate,
      0,
      0
      );

    ZeroMem (TempStr, 0x40 * sizeof (CHAR16));
    ZeroMem (HelpStr, 0x1000 * sizeof (CHAR16));

  }

  HiiUpdateForm (
    HiiHandle,
    NULL,
    IPMI_SDR_LIST_FORM_ID,
    StartOpCodeHandle,
    EndOpCodeHandle
    );

  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (EndOpCodeHandle);

  FreePool (SdrBuffer);

}


/**
 IPMI configuration menu callback function.

 @param[in]         HiiHandle           EFI_HII_HANDLE.
 @param[in]         QuestionId          The Callback question ID.
 @param[in]         ConfigBuffer        A pointer to CHIPSET_CONFIGURATION struct.
 @param[in]         BrowserData         A pointer to SETUP_UTILITY_BROWSER_DATA struct.

 @retval EFI_UNSUPPORTED                Platform does not implement this function.
 @return Status                         Callback execute Status.
*/
EFI_STATUS
IpmiConfigCallback (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  EFI_QUESTION_ID                   QuestionId,
  IN  VOID                              *ConfigBuffer,
  IN  VOID                              *BrowserData
  )
{
  EFI_STRING_ID                         TokenToUpdate;
  CHAR16                                UpdateString[0x100];
  EFI_STATUS                            Status;
  CHAR16                                *StringBuffer;
  UINT8                                 *DestBuffer;
  EFI_INPUT_KEY                         Key;
  UINT8                                 IPv4[4];
  UINT8                                 IPv6[16];
  CHIPSET_CONFIGURATION                 *SetupConfig;
  SETUP_UTILITY_BROWSER_DATA            *SUBrowser;

  ZeroMem (UpdateString, sizeof (UpdateString));
  Status = EFI_UNSUPPORTED;
  StringBuffer = NULL;
  DestBuffer = NULL;
  ZeroMem (&Key, sizeof (EFI_INPUT_KEY));
  ZeroMem (IPv4, sizeof (IPv4));
  ZeroMem (IPv6, sizeof (IPv6));
  SetupConfig = NULL;
  SetupConfig = (CHIPSET_CONFIGURATION*)ConfigBuffer;
  SUBrowser = NULL;
  SUBrowser = (SETUP_UTILITY_BROWSER_DATA*)BrowserData;

  switch (QuestionId) {

  case KEY_IPMI_SDR_LIST_SUPPORT:
    Status = EFI_SUCCESS;
    if (SetupConfig->IpmiSdrListEnable) {
      UpdateBmcSdrList (HiiHandle);
    }
    break;

  case KEY_SET_BMC_IPV6_IP_ADDRESS:
  case KEY_SET_BMC_IPV6_GATEWAY_ADDRESS:
    if (QuestionId == KEY_SET_BMC_IPV6_IP_ADDRESS) {
      Status = TextInputBox (L"Set IPv6 IP Address", INPUT_FORMAT_IPV6, &StringBuffer);
      TokenToUpdate = STRING_TOKEN (mIpmiStrIdStorage.CurrentBmcIpv6IpAddress);
      DestBuffer = SetupConfig->BmcIPv6IpAddress;
    } else {
      Status = TextInputBox (L"Set IPv6 Gateway Address", INPUT_FORMAT_IPV6, &StringBuffer);
      TokenToUpdate = STRING_TOKEN (mIpmiStrIdStorage.CurrentBmcIpv6GatewayAddress);
      DestBuffer = SetupConfig->BmcIPv6GatewayAddress;
    }

    if (!EFI_ERROR (Status)) {
      if (IsValidIpv6 (StringBuffer, IPv6)) {
        //
        // Store IP address
        //
        CopyMem (DestBuffer, IPv6, 16);

        //
        // Update string
        //
        UnicodeSPrint (
          UpdateString,
          sizeof (UpdateString),
          L"%x:%x:%x:%x:%x:%x:%x:%x",
          (UINTN)((IPv6[0] << 8) | IPv6[1]),
          (UINTN)((IPv6[2] << 8) | IPv6[3]),
          (UINTN)((IPv6[4] << 8) | IPv6[5]),
          (UINTN)((IPv6[6] << 8) | IPv6[7]),
          (UINTN)((IPv6[8] << 8) | IPv6[9]),
          (UINTN)((IPv6[10] << 8) | IPv6[11]),
          (UINTN)((IPv6[12] << 8) | IPv6[13]),
          (UINTN)((IPv6[14] << 8) | IPv6[15])
          );

        HiiSetString (HiiHandle, TokenToUpdate, UpdateString, NULL);

      } else {
        Status = EFI_UNSUPPORTED;
        SUBrowser->H2ODialog->ConfirmDialog (
                                  2,
                                  FALSE,
                                  0,
                                  NULL,
                                  &Key,
                                  L"Invalid Input! Please check it again!"
                                  );
      }

      FreePool (StringBuffer);

    }

    break;

  case KEY_SET_BMC_LAN_CHANNEL_NUM:
    Status = EFI_SUCCESS;
    if  (((mBmcLanChannelBitmap >> (SetupConfig->BmcLanChannel)) & 0x01) != 0x01) {
      SetupConfig->BmcLanChannel = mBmcPrevLanChannel;
      SUBrowser->H2ODialog->ConfirmDialog (
                                2,
                                FALSE,
                                0,
                                NULL,
                                &Key,
                                L"Please Look at HELP! Invalid Input BMC LAN Channel Number!"
                                );
    } else {
      mBmcPrevLanChannel = SetupConfig->BmcLanChannel;
    }

    break;

  case KEY_SET_BMC_IPV4_IP_ADDRESS:
  case KEY_SET_BMC_IPV4_SUBNET_MASK:
  case KEY_SET_BMC_IPV4_GATEWAY_ADDRESS:
    if (QuestionId == KEY_SET_BMC_IPV4_IP_ADDRESS) {
      Status = TextInputBox (L"Set IPv4 IP Address", INPUT_FORMAT_IPV4, &StringBuffer);
      TokenToUpdate = STRING_TOKEN (mIpmiStrIdStorage.CurrentBmcIpv4IpAddress);
      DestBuffer = SetupConfig->BmcIPv4IpAddress;
    } else if (QuestionId == KEY_SET_BMC_IPV4_SUBNET_MASK) {
      Status = TextInputBox (L"Set IPv4 Subnet Mask", INPUT_FORMAT_IPV4, &StringBuffer);
      TokenToUpdate = STRING_TOKEN (mIpmiStrIdStorage.CurrentBmcIpv4SubnetMask);
      DestBuffer = SetupConfig->BmcIPv4SubnetMask;
    } else {
      Status = TextInputBox (L"Set IPv4 Gateway Address", INPUT_FORMAT_IPV4, &StringBuffer);
      TokenToUpdate = STRING_TOKEN (mIpmiStrIdStorage.CurrentBmcIpv4GatewayAddress);
      DestBuffer = SetupConfig->BmcIPv4GatewayAddress;
    }

    if (!EFI_ERROR (Status)) {
      if (IsValidIpv4 (StringBuffer, IPv4)) {

        //
        // Store IP address
        //
        CopyMem (DestBuffer, IPv4, 4);

        //
        // Update string
        //
        UnicodeSPrint (
          UpdateString,
          sizeof (UpdateString),
          L"%d.%d.%d.%d",
          (UINTN)(IPv4[0]),
          (UINTN)(IPv4[1]),
          (UINTN)(IPv4[2]),
          (UINTN)(IPv4[3])
          );

        HiiSetString (HiiHandle, TokenToUpdate, UpdateString, NULL);

      } else {
        Status = EFI_UNSUPPORTED;
        SUBrowser->H2ODialog->ConfirmDialog (
                                  2,
                                  FALSE,
                                  0,
                                  NULL,
                                  &Key,
                                  L"Invalid Input! Please check again!"
                                  );
      }

      FreePool (StringBuffer);

    }

    break;

  }

  return Status;

}


/**
 Create IPMI configuration goto option dynamically.

 @param[in]         HiiHandle           EFI_HII_HANDLE.

*/
VOID
CreateIpmiConfigOption (
  IN  EFI_HII_HANDLE                    HiiHandle
  )
{
  VOID						                      *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL                    *StartLabel;
  CHAR16                                HelpStr[0x100];
  EFI_STRING_ID                         HelpTokenToUpdate;

  StartOpCodeHandle = NULL;
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();

  StartLabel = (EFI_IFR_GUID_LABEL*) HiiCreateGuidOpCode (
                                       StartOpCodeHandle,
                                       &gEfiIfrTianoGuid,
                                       NULL,
                                       sizeof (EFI_IFR_GUID_LABEL)
                                       );

  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = IPMI_CONFIG_LABEL;

  UnicodeSPrint (HelpStr, sizeof (HelpStr), L"Displays H2O IPMI and BMC information");
  HelpTokenToUpdate = HiiSetString (HiiHandle, 0, HelpStr, NULL);

  HiiCreateGotoOpCode (
    StartOpCodeHandle,
    IPMI_CONFIG_FORM_ID,
    STRING_TOKEN(mIpmiStrIdStorage.IpmiConfigFormTitle),
    HelpTokenToUpdate,
    EFI_IFR_FLAG_CALLBACK,
    KEY_IPMI_CONFIG
    );

  HiiUpdateForm (
    HiiHandle,
    NULL,
    ROOT_FORM_ID,
    StartOpCodeHandle,
    NULL
    );

  HiiFreeOpCodeHandle (StartOpCodeHandle);

}


/**
 Update "BMC Status" string in IPMI configuration.

 @param[in]         BmcStatus           BMC Status.
 @param[in]         HiiHandle           EFI_HII_HANDLE.

*/
VOID
UpdateBmcStatusString (
  IN  H2O_IPMI_BMC_STATUS               BmcStatus,
  IN  EFI_HII_HANDLE                    HiiHandle
  )
{
  CHAR16                                TempString[IPMI_CONFIG_TEMP_STR_LEN];
  EFI_STRING_ID                         TokenToUpdate;

  ZeroMem (TempString, sizeof (TempString));

  switch (BmcStatus) {
  case BMC_STATUS_OK:
    UnicodeSPrint (TempString, sizeof (TempString), L"OK");
    break;

  case BMC_STATUS_ERROR:
    UnicodeSPrint (TempString, sizeof (TempString), L"Error");
    break;

  case BMC_STATUS_NOT_READY:
    UnicodeSPrint (TempString, sizeof (TempString), L"Not Ready");
    break;

  case BMC_STATUS_NOT_FOUND:
    UnicodeSPrint (TempString, sizeof (TempString), L"Not Found");
    break;

  case BMC_STATUS_UNKNOWN:
    UnicodeSPrint (TempString, sizeof (TempString), L"Unknown");
    break;
  }

  TokenToUpdate = STRING_TOKEN (mIpmiStrIdStorage.CurrentBmcStatus);
  HiiSetString (HiiHandle, TokenToUpdate, TempString, NULL);

}


/**
 Update "IPMI Base Address for OS/POST/SMM" string in IPMI configuration.

 @param[in]         BaseAddressTable    BMC address table, including OS, SMM and FW base address.
 @param[in]         InterfaceType       BMC interface type.
 @param[in]         HiiHandle           EFI_HII_HANDLE.

*/
VOID
UpdateIpmiBaseAddressString (
  IN  H2O_IPMI_INTERFACE_ADDRESS        *BaseAddressTable,
  IN  H2O_IPMI_INTERFACE_TYPE           InterfaceType,
  IN  EFI_HII_HANDLE                    HiiHandle
  )
{
  CHAR16                                TempString[IPMI_CONFIG_TEMP_STR_LEN];
  EFI_STRING_ID                         TokenToUpdate;

  ZeroMem (TempString, sizeof (TempString));

  if (InterfaceType == H2O_IPMI_KCS) {

    UnicodeSPrint (
      TempString,
      sizeof (TempString),
      L"%x/%x",
      (UINTN)BaseAddressTable[H2O_IPMI_OS_BASE_ADDRESS_INDEX].Addr,
      (UINTN)(BaseAddressTable[H2O_IPMI_OS_BASE_ADDRESS_INDEX].Addr +
              BaseAddressTable[H2O_IPMI_OS_BASE_ADDRESS_INDEX].Offset)
      );

    TokenToUpdate = STRING_TOKEN (mIpmiStrIdStorage.CurrentIpmiOsBaseAddress);
    HiiSetString (HiiHandle, TokenToUpdate, TempString, NULL);

    ZeroMem (TempString, sizeof (TempString));
    UnicodeSPrint (
      TempString,
      sizeof (TempString),
      L"%x/%x",
      (UINTN)BaseAddressTable[H2O_IPMI_POST_BASE_ADDRESS_INDEX].Addr,
      (UINTN)(BaseAddressTable[H2O_IPMI_POST_BASE_ADDRESS_INDEX].Addr +
              BaseAddressTable[H2O_IPMI_POST_BASE_ADDRESS_INDEX].Offset)
      );

    TokenToUpdate = STRING_TOKEN (mIpmiStrIdStorage.CurrentIpmiPostBaseAddress);
    HiiSetString (HiiHandle, TokenToUpdate, TempString, NULL);

    ZeroMem (TempString, sizeof (TempString));
    UnicodeSPrint (
      TempString,
      sizeof (TempString),
      L"%x/%x",
      (UINTN)BaseAddressTable[H2O_IPMI_SMM_BASE_ADDRESS_INDEX].Addr,
      (UINTN)(BaseAddressTable[H2O_IPMI_SMM_BASE_ADDRESS_INDEX].Addr +
              BaseAddressTable[H2O_IPMI_SMM_BASE_ADDRESS_INDEX].Offset)
      );

    TokenToUpdate = STRING_TOKEN (mIpmiStrIdStorage.CurrentIpmiSmmBaseAddress);
    HiiSetString (HiiHandle, TokenToUpdate, TempString, NULL);

  } else {

    UnicodeSPrint (
      TempString,
      sizeof (TempString),
      L"%x",
      (UINTN)BaseAddressTable[H2O_IPMI_OS_BASE_ADDRESS_INDEX].Addr
      );

    TokenToUpdate = STRING_TOKEN (mIpmiStrIdStorage.CurrentIpmiOsBaseAddress);
    HiiSetString (HiiHandle, TokenToUpdate, TempString, NULL);

    ZeroMem (TempString, sizeof (TempString));
    UnicodeSPrint (
      TempString,
      sizeof (TempString),
      L"%x",
      (UINTN)BaseAddressTable[H2O_IPMI_POST_BASE_ADDRESS_INDEX].Addr
      );

    TokenToUpdate = STRING_TOKEN (mIpmiStrIdStorage.CurrentIpmiPostBaseAddress);
    HiiSetString (HiiHandle, TokenToUpdate, TempString, NULL);

    ZeroMem (TempString, sizeof (TempString));

    UnicodeSPrint (
      TempString,
      sizeof (TempString),
      L"%x",
      (UINTN)BaseAddressTable[H2O_IPMI_SMM_BASE_ADDRESS_INDEX].Addr
      );

    TokenToUpdate = STRING_TOKEN (mIpmiStrIdStorage.CurrentIpmiSmmBaseAddress);
    HiiSetString (HiiHandle, TokenToUpdate, TempString, NULL);
    
  }

}


/**
 Update "System Interface type" string in IPMI BMC Config.

 @param[in]         InterfaceType       H2O_IPMI_INTERFACE_TYPE enum.
 @param[in]         HiiHandle           EFI_HII_HANDLE.

*/
VOID
UpdateIpmiInterfaceTypeString (
  IN  H2O_IPMI_INTERFACE_TYPE           InterfaceType,
  IN  EFI_HII_HANDLE                    HiiHandle
  )
{
  CHAR16                                TempString[IPMI_CONFIG_TEMP_STR_LEN];
  EFI_STRING_ID                         TokenToUpdate;

  switch (InterfaceType) {
  case H2O_IPMI_KCS:
    UnicodeSPrint (TempString, sizeof (TempString), L"KCS");
    break;

  case H2O_IPMI_SMIC:
    UnicodeSPrint (TempString, sizeof (TempString), L"SMIC");
    break;

  case H2O_IPMI_BT:
    UnicodeSPrint (TempString, sizeof (TempString), L"BT");
    break;

  case H2O_IPMI_SSIF:
    UnicodeSPrint (TempString, sizeof (TempString), L"SSIF");
    break;

  default:
    UnicodeSPrint (TempString, sizeof (TempString), L"Unknown");
    break;
  }

  TokenToUpdate = STRING_TOKEN (mIpmiStrIdStorage.CurrentIpmiInterfaceType);
  HiiSetString (HiiHandle, TokenToUpdate, TempString, NULL);

}


/**
 Update "BMC Firmware Version" string in IPMI configuration.

 @param[in]         FwVersion           BMC firmware version.
 @param[in]         HiiHandle           EFI_HII_HANDLE.

*/
VOID
UpdateBmcFirmwareVersionString (
  IN  UINT16                            FwVersion,
  IN  EFI_HII_HANDLE                    HiiHandle
  )
{
  CHAR16                                TempString[IPMI_CONFIG_TEMP_STR_LEN];
  EFI_STRING_ID                         TokenToUpdate;

  ZeroMem (TempString, sizeof (TempString));

  UnicodeSPrint (
    TempString,
    sizeof (TempString),
    L"%d.%02x",
    (UINTN)(FwVersion >> 8),
    (UINTN)(FwVersion & 0xFF)
    );

  TokenToUpdate = STRING_TOKEN (mIpmiStrIdStorage.CurrentBmcFirmwareVersion);
  HiiSetString (HiiHandle, TokenToUpdate, TempString, NULL);

}


/**
 Update "IPMI Specification Version" string in IPMI configuration.

 @param[in]         IpmiVersion         IPMI version that BMC supported.
 @param[in]         HiiHandle           EFI_HII_HANDLE.

*/
VOID
UpdateIpmiSpecVersionString (
  IN  UINT8                             IpmiVersion,
  IN  EFI_HII_HANDLE                    HiiHandle
  )
{
  CHAR16                                TempString[IPMI_CONFIG_TEMP_STR_LEN];
  EFI_STRING_ID                         TokenToUpdate;

  ZeroMem (TempString, sizeof (TempString));

  UnicodeSPrint (
    TempString,
    sizeof (TempString),
    L"%d.%d",
    (UINTN)(IpmiVersion >> 4),
    (UINTN)(IpmiVersion & 0x0F)
    );

  TokenToUpdate = STRING_TOKEN (mIpmiStrIdStorage.CurrentIpmiSpecVersion);
  HiiSetString (HiiHandle, TokenToUpdate, TempString, NULL);

}


/**
 Update BMC Lan Channel Help, show available BMC lan channels.

 @param[in]         HiiHandle           EFI_HII_HANDLE.

*/
VOID
UpdateBmcLanChannelHelp (
  IN  EFI_HII_HANDLE                    HiiHandle
  )
{
  CHAR16                                TempString[0x100];
  CHAR16                                Temp[0x10];
  UINT8                                 Index;
  EFI_STRING_ID                         TokenToUpdate;

  ZeroMem (TempString, sizeof (TempString));
  StrCpy (TempString, L"This is Channel Number for BMC, Set/Get LAN information according to it. ");
  StrCat (TempString, L"The Valid Channel Number :");

  ZeroMem (Temp, sizeof (Temp));

  for (Index = 0; Index < LAN_CHANNEL_MAX; Index++) {

    if (((mBmcLanChannelBitmap >> Index) & 0x01) == 0x01) {
      UnicodeSPrint (Temp, sizeof (Temp), L" %02d", Index);
      StrCat (TempString, Temp);
    }

  }

  TokenToUpdate = STRING_TOKEN (mIpmiStrIdStorage.BmcLanChannelHelp);
  HiiSetString (HiiHandle, TokenToUpdate, TempString, NULL);

}


/**
 Update "BMC MAC Address" string in IPMI configuration.

 @param[in]         Ipmi                Pointer to H2O_IPMI_INTERFACE_PROTOCOL.
 @param[in]         HiiHandle           EFI_HII_HANDLE.
 @param[in]         SetupConfig         Pointer to setup variable structure.

*/
VOID
UpdateBmcMacAddressString (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *Ipmi,
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  CHIPSET_CONFIGURATION             *SetupConfig
  )
{
  CHAR16                                TempString[IPMI_CONFIG_TEMP_STR_LEN];
  EFI_STRING_ID                         TokenToUpdate;
  EFI_STATUS                            Status;
  UINT8                                 RequestData[4];
  UINT8                                 RecvSize;
  UINT8                                 RecvData[7];
  H2O_IPMI_CMD_HEADER                   Request;

  ZeroMem (RequestData, sizeof (RequestData));
  ZeroMem (TempString, sizeof (TempString));

  //
  // Channel number
  //
  RequestData[0] = SetupConfig->BmcLanChannel;

  //
  // Parameter Selector
  //
  RequestData[1] = LAN_MAC_ADDRESS;

  Request.NetFn = H2O_IPMI_NETFN_TRANSPORT;
  Request.Cmd = H2O_IPMI_CMD_GET_LAN_PARAMETERS;
  Request.Lun = H2O_IPMI_BMC_LUN;

  Status = Ipmi->ExecuteIpmiCmd (
                   Ipmi,
                   Request,
                   RequestData,
                   4,
                   RecvData,
                   &RecvSize,
                   NULL
                   );

  if (!EFI_ERROR (Status)) {

    UnicodeSPrint (
      TempString,
      sizeof (TempString),
      L"%02x:%02x:%02x:%02x:%02x:%02x",
      (UINTN)RecvData[1],
      (UINTN)RecvData[2],
      (UINTN)RecvData[3],
      (UINTN)RecvData[4],
      (UINTN)RecvData[5],
      (UINTN)RecvData[6]
      );

    TokenToUpdate = STRING_TOKEN (mIpmiStrIdStorage.CurrentBmcMacAddress);
    HiiSetString (HiiHandle, TokenToUpdate, TempString, NULL);

  }

}


/**
 Update "IPv4 IP Address" string in IPMI configuration.

 @param[in]         HiiHandle           EFI_HII_HANDLE.
 @param[in]         SetupConfig         Pointer to setup variable structure.

*/
VOID
UpdateBmcIPv4IpAddressString (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  CHIPSET_CONFIGURATION             *SetupConfig
  )
{
  CHAR16                                TempString[IPMI_CONFIG_TEMP_STR_LEN];
  EFI_STRING_ID                         TokenToUpdate;

  ZeroMem (TempString, sizeof (TempString));

  UnicodeSPrint (
    TempString,
    sizeof (TempString),
    L"%d.%d.%d.%d",
    (UINTN)SetupConfig->BmcIPv4IpAddress[0],
    (UINTN)SetupConfig->BmcIPv4IpAddress[1],
    (UINTN)SetupConfig->BmcIPv4IpAddress[2],
    (UINTN)SetupConfig->BmcIPv4IpAddress[3]
    );

  TokenToUpdate = STRING_TOKEN (mIpmiStrIdStorage.CurrentBmcIpv4IpAddress);
  HiiSetString (HiiHandle, TokenToUpdate, TempString, NULL);

}


/**
 Update "IPv4 Subnet Mask" string in IPMI configuration.

 @param[in]         HiiHandle           EFI_HII_HANDLE.
 @param[in]         SetupConfig         Pointer to setup variable structure.

*/
VOID
UpdateBmcIPv4SubnetMaskString (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  CHIPSET_CONFIGURATION             *SetupConfig
  )
{
  CHAR16                                TempString[IPMI_CONFIG_TEMP_STR_LEN];
  EFI_STRING_ID                         TokenToUpdate;

  ZeroMem (TempString, sizeof (TempString));

  UnicodeSPrint (
    TempString,
    sizeof (TempString),
    L"%d.%d.%d.%d",
    (UINTN)SetupConfig->BmcIPv4SubnetMask[0],
    (UINTN)SetupConfig->BmcIPv4SubnetMask[1],
    (UINTN)SetupConfig->BmcIPv4SubnetMask[2],
    (UINTN)SetupConfig->BmcIPv4SubnetMask[3]
    );

  TokenToUpdate = STRING_TOKEN (mIpmiStrIdStorage.CurrentBmcIpv4SubnetMask);
  HiiSetString (HiiHandle, TokenToUpdate, TempString, NULL);

}


/**
 Update "IPv4 Gateway Address" string in IPMI configuration.

 @param[in]         HiiHandle           EFI_HII_HANDLE.
 @param[in]         SetupConfig         Pointer to setup variable structure.

*/
VOID
UpdateBmcIPv4GatewayAddressString (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  CHIPSET_CONFIGURATION             *SetupConfig
  )
{
  CHAR16                                TempString[IPMI_CONFIG_TEMP_STR_LEN];
  EFI_STRING_ID                         TokenToUpdate;

  ZeroMem (TempString, sizeof (TempString));

  UnicodeSPrint (
    TempString,
    sizeof (TempString),
    L"%d.%d.%d.%d",
    (UINTN)SetupConfig->BmcIPv4GatewayAddress[0],
    (UINTN)SetupConfig->BmcIPv4GatewayAddress[1],
    (UINTN)SetupConfig->BmcIPv4GatewayAddress[2],
    (UINTN)SetupConfig->BmcIPv4GatewayAddress[3]
    );

  TokenToUpdate = STRING_TOKEN (mIpmiStrIdStorage.CurrentBmcIpv4GatewayAddress);
  HiiSetString (HiiHandle, TokenToUpdate, TempString, NULL);

}


/**
 Update "IPv6 IP Address" string in IPMI configuration.

 @param[in]         HiiHandle           EFI_HII_HANDLE.
 @param[in]         SetupConfig         Pointer to setup variable structure.

*/
VOID
UpdateBmcIPv6IpAddressString (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  CHIPSET_CONFIGURATION             *SetupConfig
  )
{
  CHAR16                                TempString[40];
  EFI_STRING_ID                         TokenToUpdate;

  ZeroMem (TempString, sizeof (TempString));

  UnicodeSPrint (
    TempString,
    sizeof (TempString),
    L"%x:%x:%x:%x:%x:%x:%x:%x",
    (UINTN)((SetupConfig->BmcIPv6IpAddress[0] << 8) | SetupConfig->BmcIPv6IpAddress[1]),
    (UINTN)((SetupConfig->BmcIPv6IpAddress[2] << 8) | SetupConfig->BmcIPv6IpAddress[3]),
    (UINTN)((SetupConfig->BmcIPv6IpAddress[4] << 8) | SetupConfig->BmcIPv6IpAddress[5]),
    (UINTN)((SetupConfig->BmcIPv6IpAddress[6] << 8) | SetupConfig->BmcIPv6IpAddress[7]),
    (UINTN)((SetupConfig->BmcIPv6IpAddress[8] << 8) | SetupConfig->BmcIPv6IpAddress[9]),
    (UINTN)((SetupConfig->BmcIPv6IpAddress[10] << 8) | SetupConfig->BmcIPv6IpAddress[11]),
    (UINTN)((SetupConfig->BmcIPv6IpAddress[12] << 8) | SetupConfig->BmcIPv6IpAddress[13]),
    (UINTN)((SetupConfig->BmcIPv6IpAddress[14] << 8) | SetupConfig->BmcIPv6IpAddress[15])
    );

  TokenToUpdate = STRING_TOKEN (mIpmiStrIdStorage.CurrentBmcIpv6IpAddress);
  HiiSetString (HiiHandle, TokenToUpdate, TempString, NULL);

}


/**
 Update "IPv6 Gateway Address" string in IPMI configuration.

 @param[in]         HiiHandle           EFI_HII_HANDLE.
 @param[in]         SetupConfig         Pointer to setup variable structure.

*/
VOID
UpdateBmcIPv6GatewayAddressString (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  CHIPSET_CONFIGURATION             *SetupConfig
  )
{
  CHAR16                                TempString[40];
  EFI_STRING_ID                         TokenToUpdate;

  ZeroMem (TempString, sizeof (TempString));

  UnicodeSPrint (
    TempString,
    sizeof (TempString),
    L"%x:%x:%x:%x:%x:%x:%x:%x",
    (UINTN)((SetupConfig->BmcIPv6GatewayAddress[0] << 8) | SetupConfig->BmcIPv6GatewayAddress[1]),
    (UINTN)((SetupConfig->BmcIPv6GatewayAddress[2] << 8) | SetupConfig->BmcIPv6GatewayAddress[3]),
    (UINTN)((SetupConfig->BmcIPv6GatewayAddress[4] << 8) | SetupConfig->BmcIPv6GatewayAddress[5]),
    (UINTN)((SetupConfig->BmcIPv6GatewayAddress[6] << 8) | SetupConfig->BmcIPv6GatewayAddress[7]),
    (UINTN)((SetupConfig->BmcIPv6GatewayAddress[8] << 8) | SetupConfig->BmcIPv6GatewayAddress[9]),
    (UINTN)((SetupConfig->BmcIPv6GatewayAddress[10] << 8) | SetupConfig->BmcIPv6GatewayAddress[11]),
    (UINTN)((SetupConfig->BmcIPv6GatewayAddress[12] << 8) | SetupConfig->BmcIPv6GatewayAddress[13]),
    (UINTN)((SetupConfig->BmcIPv6GatewayAddress[14] << 8) | SetupConfig->BmcIPv6GatewayAddress[15])
    );

  TokenToUpdate = STRING_TOKEN (mIpmiStrIdStorage.CurrentBmcIpv6GatewayAddress);
  HiiSetString (HiiHandle, TokenToUpdate, TempString, NULL);

}


/**
 Update all information in IPMI configuration menu.

 @param[in]         HiiHandle           EFI_HII_HANDLE.
 @param[in]         ConfigBuffer        A pointer to CHIPSET_CONFIGURATION struct.
 @param[in]         StrIdBuffer         A pointer to IPMI_STR_ID struct.

 @retval EFI_SUCCESS                    Platform implement this function return EFI_SUCCESS.
 @retval EFI_UNSUPPORTED                Platform does not implement this function.
*/
EFI_STATUS
IpmiConfigUpdate (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  VOID                              *ConfigBuffer,
  IN  IPMI_STR_ID                       *StrIdBuffer
  )
{
  H2O_IPMI_INTERFACE_PROTOCOL           *Ipmi;
  EFI_STATUS                            Status;
  H2O_IPMI_HOB                          *IpmiHob;
  H2O_IPMI_BMC_STATUS                   BmcStatus;
  H2O_IPMI_INTERFACE_ADDRESS            BaseAddressTable[H2O_IPMI_MAX_BASE_ADDRESS_NUM];
  UINT16                                FwVersion;
  UINT8                                 IpmiVersion;
  BOOLEAN                               UpdateBaseAddress;
  UINT8                                 InterfaceType;
  CHIPSET_CONFIGURATION                 *SetupConfig;

  FwVersion = 0;
  IpmiVersion = 0;
  InterfaceType = 0;
  CopyMem (&mIpmiStrIdStorage, StrIdBuffer, sizeof (IPMI_STR_ID));

  //
  // Create goto for IPMI Configuration
  //
  CreateIpmiConfigOption (HiiHandle);

  //
  // Locate H2OIpmiInterfaceProtocol first
  //
  BmcStatus = BMC_STATUS_UNKNOWN;
  UpdateBaseAddress = FALSE;
  Status = gBS->LocateProtocol (&gH2OIpmiInterfaceProtocolGuid, NULL, (VOID **)&Ipmi);

  if (!EFI_ERROR (Status) && H2O_IPMI_VERSION_DEFAULT != Ipmi->GetIpmiVersion (Ipmi)) {

    //
    // Locate IPMI protocol success and can communicate with BMC
    // Get information from IPMI protocol
    //
    BmcStatus = BMC_STATUS_OK;

    BaseAddressTable[H2O_IPMI_OS_BASE_ADDRESS_INDEX].Addr =
      Ipmi->GetIpmiBaseAddress (Ipmi, H2O_IPMI_OS_BASE_ADDRESS_INDEX);
    BaseAddressTable[H2O_IPMI_OS_BASE_ADDRESS_INDEX].Offset =
      Ipmi->GetIpmiBaseAddressOffset (Ipmi, H2O_IPMI_OS_BASE_ADDRESS_INDEX);

    BaseAddressTable[H2O_IPMI_SMM_BASE_ADDRESS_INDEX].Addr =
      Ipmi->GetIpmiBaseAddress (Ipmi, H2O_IPMI_SMM_BASE_ADDRESS_INDEX);
    BaseAddressTable[H2O_IPMI_SMM_BASE_ADDRESS_INDEX].Offset =
      Ipmi->GetIpmiBaseAddressOffset (Ipmi, H2O_IPMI_SMM_BASE_ADDRESS_INDEX);

    BaseAddressTable[H2O_IPMI_POST_BASE_ADDRESS_INDEX].Addr =
      Ipmi->GetIpmiBaseAddress (Ipmi, H2O_IPMI_POST_BASE_ADDRESS_INDEX);
    BaseAddressTable[H2O_IPMI_POST_BASE_ADDRESS_INDEX].Offset =
      Ipmi->GetIpmiBaseAddressOffset (Ipmi, H2O_IPMI_POST_BASE_ADDRESS_INDEX);

    FwVersion = Ipmi->GetBmcFirmwareVersion (Ipmi);

    IpmiVersion = Ipmi->GetIpmiVersion (Ipmi);

    InterfaceType = Ipmi->GetIpmiInterfaceType (Ipmi);

    UpdateBaseAddress = TRUE;

  } else {

    //
    // Since we cannot communicate with BMC, get information from IPMI hob.
    //
    IpmiLibGetIpmiHob (&IpmiHob);

    BmcStatus = IpmiHob->BmcStatus;

    CopyMem (
      BaseAddressTable,
      IpmiHob->InterfaceAddress,
      H2O_IPMI_MAX_BASE_ADDRESS_NUM * sizeof (H2O_IPMI_INTERFACE_ADDRESS)
      );

    InterfaceType = IpmiHob->InterfaceType;

    UpdateBaseAddress = TRUE;

  }

  SetupConfig = NULL;
  SetupConfig = (CHIPSET_CONFIGURATION*)ConfigBuffer;

  //
  // Add BmcStatus variable used in Setup Utility, so we can reduce
  // the usage of label
  //
  SetupConfig->BmcStatus = BmcStatus;
  mBmcStatus = BmcStatus;

  //
  // Always update BMC status
  //
  UpdateBmcStatusString (BmcStatus, HiiHandle);

  //
  // If we can get base address, update it
  //
  if (UpdateBaseAddress) {
    UpdateIpmiInterfaceTypeString (InterfaceType, HiiHandle);
    UpdateIpmiBaseAddressString (BaseAddressTable, InterfaceType, HiiHandle);
  }

  //
  // Some information only need to be updated when BMC is OK
  //
  if (BmcStatus == BMC_STATUS_OK) {

    UpdateBmcFirmwareVersionString (FwVersion, HiiHandle);
    UpdateIpmiSpecVersionString (IpmiVersion, HiiHandle);

    if (SetupConfig->IpmiSdrListEnable) {
      UpdateBmcSdrList (HiiHandle);
    }

    UpdateBmcLanChannelHelp (HiiHandle);
    UpdateBmcMacAddressString (Ipmi, HiiHandle, SetupConfig);

    UpdateBmcIPv4IpAddressString (HiiHandle, SetupConfig);
    UpdateBmcIPv4SubnetMaskString (HiiHandle, SetupConfig);
    UpdateBmcIPv4GatewayAddressString (HiiHandle, SetupConfig);

    UpdateBmcIPv6IpAddressString (HiiHandle, SetupConfig);
    UpdateBmcIPv6GatewayAddressString (HiiHandle, SetupConfig);

  }

  return EFI_SUCCESS;

}


/**
 This function is called when loading default/custom in Setup Utility.
 It will restore default/custom value that related to BMC settings.

 @param[in]         HiiHandle           EFI_HII_HANDLE.
 @param[in]         ConfigBuffer        A pointer to CHIPSET_CONFIGURATION struct.
 @param[in]         LoadDefault         TRUE to load default; FALSE to load custom.

 @retval EFI_SUCCESS                    Platform implement this function return EFI_SUCCESS.
 @retval EFI_UNSUPPORTED                Platform does not implement this function.
*/
EFI_STATUS
IpmiConfigRestore (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  VOID                              *ConfigBuffer,
  IN  BOOLEAN                           LoadDefault
  )
{
  CHIPSET_CONFIGURATION                 *SetupConfig;

  SetupConfig = NULL;
  SetupConfig = (CHIPSET_CONFIGURATION*)ConfigBuffer;

  //
  // Restore BMC status to setup buffer
  //
  SetupConfig->BmcStatus = mBmcStatus;
  SetupConfig->BmcLanChannel = mBmcLanChannel;

  if (LoadDefault) {
    ZeroMem (SetupConfig->BmcIPv4IpAddress, sizeof (SetupConfig->BmcIPv4IpAddress));
    ZeroMem (SetupConfig->BmcIPv4SubnetMask, sizeof (SetupConfig->BmcIPv4SubnetMask));
    ZeroMem (SetupConfig->BmcIPv4GatewayAddress, sizeof (SetupConfig->BmcIPv4GatewayAddress));
    ZeroMem (SetupConfig->BmcIPv6IpAddress, sizeof (SetupConfig->BmcIPv6IpAddress));
    ZeroMem (SetupConfig->BmcIPv6GatewayAddress, sizeof (SetupConfig->BmcIPv6GatewayAddress));
  }

  UpdateBmcIPv4IpAddressString (HiiHandle, SetupConfig);
  UpdateBmcIPv4SubnetMaskString (HiiHandle, SetupConfig);
  UpdateBmcIPv4GatewayAddressString (HiiHandle, SetupConfig);

  UpdateBmcIPv6IpAddressString (HiiHandle, SetupConfig);
  UpdateBmcIPv6GatewayAddressString (HiiHandle, SetupConfig);

  return EFI_SUCCESS;

}


/**
 Pre identify available BMC Lan Channels, If channel number in SetupConfig is invalid, modify it.

 @param[in]         Ipmi                Ipmi protocol.
 @param[in]         SetupConfig         A pointer to CHIPSET_CONFIGURATION structure.

*/
VOID
PreIdentifyAvailableBmcLanChannels (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *Ipmi,
  IN  CHIPSET_CONFIGURATION             *SetupConfig
  )
{
  UINT8                                 Index;
  UINT8                                 RequestData;
  UINT8                                 RecvSize;
  H2O_IPMI_GET_CHANNEL_INFO             Info;
  EFI_STATUS                            Status;
  H2O_IPMI_CMD_HEADER                   Request;

  Request.NetFn = H2O_IPMI_NETFN_APPLICATION;
  Request.Cmd = H2O_IPMI_CMD_GET_CHANNEL_INFO_COMMAND;
  Request.Lun = H2O_IPMI_BMC_LUN;

  mBmcLanChannelBitmap = 0;
  for (Index = 0; Index < LAN_CHANNEL_MAX; Index++) {
    RequestData = Index;
    ZeroMem (&Info, sizeof (Info));
    Status = Ipmi->ExecuteIpmiCmd (
                     Ipmi,
                     Request,
                     &RequestData,
                     1,
                     &Info,
                     &RecvSize,
                     NULL
                     );
    if (!EFI_ERROR (Status)) {
      if (Info.ChannelMidiumType == CHANNEL_MEDIUM_8023LAN) {
        mBmcLanChannelBitmap = mBmcLanChannelBitmap | (0x01 << Index);
        if ((((mBmcLanChannelBitmap >> (SetupConfig->BmcLanChannel)) & 0x01) != 0x01) &&
            (Index > SetupConfig->BmcLanChannel)) {
          SetupConfig->BmcLanChannel = Index;
        }
      }
    }
  }

}


/**
 This function is called in SetupUtilityInit for pre identify available BMC lan channels
 and get some setting from BMC. These settings are platform-dependent.

 @param[in]         ConfigBuffer        Pointer to CHIPSET_CONFIGURATION struct.

 @retval EFI_SUCCESS                    Init IPMI Config data success.
 @retval EFI_NOT_FOUND                  Locate gH2OIpmiInterfaceProtocolGuid Protocol or Ipmi Version error.
 @retval EFI_UNSUPPORTED                Platform does not implement this function.
*/
EFI_STATUS
IpmiConfigInit (
  IN  VOID                              *ConfigBuffer
  )
{
  EFI_STATUS                            Status;
  H2O_IPMI_INTERFACE_PROTOCOL           *Ipmi;
  CHIPSET_CONFIGURATION                 *SetupConfig;
  UINTN                                 BufferSize;

  Status = gBS->LocateProtocol (&gH2OIpmiInterfaceProtocolGuid, NULL, (VOID **)&Ipmi);
  if (!EFI_ERROR (Status) && H2O_IPMI_VERSION_DEFAULT != Ipmi->GetIpmiVersion (Ipmi)) {

    SetupConfig = (CHIPSET_CONFIGURATION*)ConfigBuffer;

    mBmcLanChannel = SetupConfig->BmcLanChannel;
    PreIdentifyAvailableBmcLanChannels (Ipmi, SetupConfig);
    mBmcPrevLanChannel = SetupConfig->BmcLanChannel;

    IpmiGetFromBmc (SetupConfig);

    BufferSize = 0;
    Status = gRT->GetVariable (
                    SETUP_VARIABLE_NAME,
                    &gSystemConfigurationGuid,
                    NULL,
                    &BufferSize,
                    NULL
                    );

    if (Status == EFI_BUFFER_TOO_SMALL) {
      gRT->SetVariable (
              SETUP_VARIABLE_NAME,
              &gSystemConfigurationGuid,
              EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
              BufferSize,
              ConfigBuffer
              );
    }

    return EFI_SUCCESS;

  }

  return EFI_NOT_FOUND;

}

