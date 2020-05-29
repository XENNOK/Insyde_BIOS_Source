//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
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
//;   Password.c
//;
//; Abstract:
//;
//;   Password relaitve code, including HDD password and System password
//;

#include "Password.h"
#include "ChipsetLib.h"
#include EFI_PROTOCOL_DEFINITION (OemServices)
#include EFI_PROTOCOL_DEFINITION (SimpleTextInputEx)
#include EFI_PROTOCOL_DEFINITION (IdeControllerInit)
#include EFI_PROTOCOL_DEFINITION (ChipsetLibServices)

BOOLEAN                     mSetHddPassword       = FALSE;
BOOLEAN                     mHDDPswdBootIn        = FALSE;
BOOLEAN                     mUnLockHddPassword    = FALSE;
BOOLEAN                     mSetAllHddPassword    = FALSE;
UINT16                      mSetAllCurrentPassword[HDD_PASSWORD_MAX_NUMBER + 1];
BOOLEAN                     mDisableAllHddFlag  = FALSE;
UINT16                      mSaveItemMapping[0xf][2];

STATIC
BOOLEAN
CheckHarddiskStatus(
  IN  SECURITY_INFO_DATA         *PswHddInfoTablePtr,
  IN  UINTN                      NumOfHarddisk
  );

STATIC
EFI_STATUS
CheckSetupPassword (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL *PswdConsoleService,
  IN  SETUP_UTILITY_CONFIGURATION       *ScuInfo,
  IN  SYSTEM_CONFIGURATION              *SetupVariable
  );

STATIC
EFI_STATUS
ScuLibSearchMatchedPortNum (
  IN     UINT32                              Bus,
  IN     UINT32                              Device,
  IN     UINT32                              Function,
  IN     UINT8                               PrimarySecondary,
  IN     UINT8                               SlaveMaster,
  OUT    UINTN                               *PortNum
  );

STATIC
EFI_STATUS
SetupUtilityLibGetSecurityData (
  IN  SECURITY_INFO_DATA *SecurityDataInfo,
  IN  UINT16             CallBackFormId,
  OUT BOOLEAN            *HaveSecurityData,
  OUT UINTN              *HarddiskInfoIndex
  );

STATIC
EFI_STATUS
Unicode2Ascii (
  IN     VOID   *PasswordPtr,
  IN     UINTN  PasswordLength,
  IN OUT VOID   **EncodePasswordPtr,
  IN OUT UINTN  *EncodePasswordLength
  )
/*++

  Routine Description:

    This function converts Unicode string to ASCII string.

  Arguments:

    PasswordPtr            - Password string address.
    PasswordLength         - Password string length.
    EncodePasswordPtr      - Encode password address.
    EncodePasswordLength   - Encode string length.

  Returns:

    Start of the ASCII ouput string.

--*/
{
  EFI_STATUS  Status;
  UINTN       Index;
  CHAR16      *ApPassPassword;
  UINT8       *EncodePaswd;

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  PasswordLength / 2,
                  EncodePasswordPtr
                  );

  if (EFI_ERROR(Status)) {
    return Status;
  }
  ApPassPassword  = PasswordPtr;
  EncodePaswd     = *EncodePasswordPtr;

  for (Index = 0; Index < PasswordLength / 2; Index++) {
    EncodePaswd[Index] = (UINT8)ApPassPassword[Index];
  }

  *EncodePasswordLength = PasswordLength / 2;
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
Encode (
  IN     VOID   *PasswordPtr,
  IN     UINTN  PasswordLength,
  IN OUT VOID   **EncodePasswordPtr,
  IN OUT UINTN  *EncodePasswordLength
  )
/*++

  Routine Description:
    If you want to Encode password string just edit at here.

  Arguments:
    PasswordPtr            - Password string address.
    PasswordLength         - Password string length.
    EncodePasswordPtr      - Encode password address.
    EncodePasswordLength   - Encode string length.

  Returns:
    EFI_SUCCESS - Always.
--*/
{
  *EncodePasswordLength = PasswordLength;
  *EncodePasswordPtr    = PasswordPtr;

  return EFI_SUCCESS;
}


EFI_STATUS
SereachMachLabelIndex (
  IN  HDD_SECURITY_INFO_DATA     *PswHddInfoTablePtr,
  OUT UINT16                     *CurrentMachLabel,
  OUT STRING_REF                 *SecurityModeToken
  )
/*++

Routine Description:

  Search match Label number of harddisk information

Arguments:

  PswHddInfoTablePtr -   The Simple password descriptor Information of harddisk.
  CurrentMachLabel   -   Return match IDE-Harddisk Label
  SecurityModeToken  -   Return string token of device status.

Returns:

  EFI_SUCCESS   -  Found match harddisk info and Label.
  EFI_NOT_FOUND -  Could not found match harddisk info.

--*/
{
  BOOLEAN                    CheckFoundFlag;
  UINT8                      Channel;
  UINT8                      Device;
  UINTN                      Index;
  UINTN                      PortNum;
  BBS_TABLE                  *LocalBbsTable;
  EFI_LEGACY_BIOS_PROTOCOL   *LegacyBios;
  UINT16                     HddCount;
  UINT16                     BbsTotalCount;
  HDD_INFO                   *LocalHddInfo;
  EFI_STATUS                 Status;
  SETUP_UTILITY_BROWSER_DATA *SuBrowser;
  IDE_CONFIG                 *IdeConfig;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, &LegacyBios);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = LegacyBios->GetBbsInfo(LegacyBios, &HddCount, &LocalHddInfo, &BbsTotalCount, &LocalBbsTable);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CheckFoundFlag = FALSE;
  IdeConfig = SuBrowser->IdeConfig;
  for (Index = 0; Index <= 7; Index++) {
    //
    // Only need check exist device
    //
    if (IdeConfig[Index].IdeDevice != 1) {
      continue;
    }
    Channel = IdeConfig[Index].Channel;
    Device = IdeConfig[Index].Device;

    PortNum = PswHddInfoTablePtr->MappedPort;

    //
    // Check port number is match and is not ODD
    //
    if (PortNum == Index && (LocalHddInfo[Channel].IdentifyDrive[Device].Raw[0] & bit(15)) == 0) {
      CheckFoundFlag = TRUE;
      break;
    }
  }

  if (!CheckFoundFlag) {
    return EFI_NOT_FOUND;
  }

  *CurrentMachLabel  = IdeConfig[Index].Formlabel;
  *SecurityModeToken = IdeConfig[Index].SecurityModeToken;

  return EFI_SUCCESS;
}

EFI_STATUS
UpdateHddPasswordLabel (
  IN  EFI_HII_PROTOCOL                  *Hii,
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  SECURITY_INFO_DATA                *SecurityDataInfo,
  IN  UINTN                             NumOfHddPswd
  )
/*++

Routine Description:

  Update Hdd password information at security form

Arguments:

  Hii              -     Pointer to EFI_HII_PROTOCOL instance.
  HiiHandle        -     Specific HII handle.
  SecurityDataInfo -     Security Info table of Harddisk.
  NumOfHddPswd     -     Number of harddisk.

Returns:

  EFI_SUCCESS -          Update Hdd password information successful.
  Other       -          Locate gEfiSetupUtilityBrowserProtocolGuid failed

--*/
{
  UINTN                                 Index;
  BOOLEAN                               HddPaswdLockFlag;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  EFI_STATUS                            Status;


  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (SecurityDataInfo == NULL) {
    SuBrowser->SCBuffer->SetAllHddPasswordFlag = 0;
    return EFI_SUCCESS;
  }

  if (NumOfHddPswd == 0) {
    SuBrowser->SCBuffer->SetAllHddPasswordFlag = 0;
    return EFI_SUCCESS;
  }

  HddPaswdLockFlag = FALSE;
  for (Index = 0; Index < NumOfHddPswd; Index++) {
    if (SecurityDataInfo[Index].Flag == SECURITY_LOCK) {
      HddPaswdLockFlag = TRUE;
    }
  }
  if (HddPaswdLockFlag == TRUE) {
    SuBrowser->SCBuffer->SetAllHddPasswordFlag = 0;
  } else {
    SuBrowser->SCBuffer->SetAllHddPasswordFlag = 1;
    SuBrowser->SCBuffer->UseMasterPassword = 1;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
UpdateHDDPswForm (
  IN  EFI_HII_PROTOCOL                  *Hii,
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  SECURITY_INFO_DATA                *SecurityDataInfo,
  IN  UINT16                            CurrentHddIndex
  )
/*++

Routine Description:

  Update Harddisk password form information

Arguments:

  Hii              -     Pointer to EFI_HII_PROTOCOL instance.
  HiiHandle        -     Specific HII handle.
  SecurityDataInfo -     Security Info table of Harddisk.
  CurrentHddIndex  -     The harddisk index number.

Returns:

  EFI_SUCCESS -          Update Harddisk password successful
  Other       -          locate gEfiSetupUtilityBrowserProtocolGuid failed
--*/
{
  STRING_REF                            Token;
  CHAR16                                *NewString;
  UINT16                                LabelIndex;
  STRING_REF                            SecurityModeToken;
  UINT16                                LabelIndex1;
  UINT16                                LabelIndex2;
  UINTN                                 Index;
  UINT16                                KeyBase1;
  UINT16                                KeyBase2;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  EFI_STATUS                            Status;
  BOOLEAN                               HaveSecurityData;
  UINTN                                 HarddiskInfoIndex;

  Index = 0;
  Token = 0;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SereachMachLabelIndex (
    SecurityDataInfo->HddDataInfo,
    &LabelIndex,
    &SecurityModeToken
    );

  //
  // Update Information
  //
  if (SecurityDataInfo->Flag == SECURITY_LOCK) {
    //
    // Harddisk Security Enable [Unlock]
    //
    Token = STRING_TOKEN (STR_HDD_PSW_LOCK);
  } else if (SecurityDataInfo->Flag == DISABLE_PASSWORD || SecurityDataInfo->MasterFlag == DISABLE_PASSWORD ) {
    //
    // Harddisk Security Disable
    //
    Token = STRING_TOKEN (STR_HDD_PSW_DISABLE);
  } else if (SecurityDataInfo->Flag == CHANGE_PASSWORD) {
    //
    // Password Change
    //
    Token = STRING_TOKEN (STR_HDD_PSW_CHANGE);
  } else if (SecurityDataInfo->Flag == ENABLE_PASSWORD) {
    //
    // Harddisk Security Unlock
    //
    Token = STRING_TOKEN (STR_HDD_PSW_UNLOCK);
  } else {
    //
    //Harddisk don't access password
    //
    Token = STRING_TOKEN (STR_HDD_PSW_NONACCESS);
  }
  if (Token != 0) {
    NewString = SetupUtilityLibGetTokenString (
                  SuBrowser->SUCInfo->SecurityHiiHandle,
                  Token
                  );
    Hii->NewString (
           Hii,
           NULL,
           SuBrowser->SUCInfo->AdvancedHiiHandle,
           &SecurityModeToken,
           NewString
           );
    gBS->FreePool (NewString);
    SuBrowser->SUCInfo->DoRefresh = TRUE;
  }

  if (LabelIndex < HDD_PASSWORD_CH_1_MASTER_CBM_LABEL) {
    KeyBase1 = KEY_HDD_PSW_DEVICE_BASE;
    KeyBase2 = KEY_HDD_PSW_DEVICE_CBM_BASE;
    LabelIndex1 = LabelIndex;
    Index = LabelIndex - HDD_PASSWORD_CH_1_MASTER_LABEL;
    switch (Index) {

    case 0 :
      LabelIndex2 = HDD_PASSWORD_CH_1_MASTER_CBM_LABEL + 2;
      break;

    case 1 :
      LabelIndex2 = HDD_PASSWORD_CH_1_MASTER_CBM_LABEL + 3;
      break;

    case 2 :
      LabelIndex2 = HDD_PASSWORD_CH_1_MASTER_CBM_LABEL;
      break;

    case 3 :
      LabelIndex2 = HDD_PASSWORD_CH_1_MASTER_CBM_LABEL + 1;
      break;

    default :
      LabelIndex2 = HDD_PASSWORD_CH_1_MASTER_CBM_LABEL + (UINT16) Index;
      break;
    }
  } else {
    KeyBase1 = KEY_HDD_PSW_DEVICE_CBM_BASE;
    KeyBase2 = KEY_HDD_PSW_DEVICE_BASE;
    LabelIndex1 = LabelIndex;
    Index = LabelIndex - HDD_PASSWORD_CH_1_MASTER_CBM_LABEL;
    switch (Index) {

    case 0 :
      LabelIndex2 = HDD_PASSWORD_CH_1_MASTER_LABEL + 2;
      break;

    case 1 :
      LabelIndex2 = HDD_PASSWORD_CH_1_MASTER_LABEL + 3;
      break;

    case 2 :
      LabelIndex2 = HDD_PASSWORD_CH_1_MASTER_LABEL;
      break;

    case 3 :
      LabelIndex2 = HDD_PASSWORD_CH_1_MASTER_LABEL + 1;
      break;

    default :
      LabelIndex2 = HDD_PASSWORD_CH_1_MASTER_LABEL + (UINT16) Index;
      break;
    }
  }
  SetupUtilityLibGetSecurityData (
    SecurityDataInfo,
    LabelIndex1,
    &HaveSecurityData,
    &HarddiskInfoIndex
    );
  if (HaveSecurityData == TRUE) {
    UpdateHdPswLabel (LabelIndex1, HaveSecurityData, HarddiskInfoIndex);
  }
  SetupUtilityLibGetSecurityData (
    SecurityDataInfo,
    LabelIndex2,
    &HaveSecurityData,
    &HarddiskInfoIndex
    );
  if (HaveSecurityData == TRUE) {
    UpdateHdPswLabel (LabelIndex2, HaveSecurityData, HarddiskInfoIndex);
  }

  SuBrowser->Interface.Reload = TRUE;

  return EFI_SUCCESS;
}

EFI_STATUS
HddPasswordCallback (
  IN  EFI_FORM_CALLBACK_PROTOCOL        *FormCallback,
  IN  EFI_HII_PROTOCOL                  *Hii,
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  EFI_IFR_DATA_ARRAY                *Data,
  OUT EFI_HII_CALLBACK_PACKET           **Packet,
  OUT BOOLEAN                           *PState,
  IN  UINT16                            CurrentHddIndex,
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *PswdConsoleService,
  IN  SECURITY_INFO_DATA                *SecurityDataInfo,
  IN  BOOLEAN                           MasterOrUser,
  IN  UINTN                             NumOfHddPswd
  )
/*++

Routine Description:

  The current Harddisk security call back.

Arguments:

  FormCallback       -  Point to EFI_FORM_CALLBACK_PROTOCOL instance.
  Hii                -  Point to HII protocol instance.
  HiiHandle          -  Specific HII handle.
  Data               -  A pointer to the data being sent to the original exporting driver.
  Packet             -  A double pointer points to EFI_HII_CALLBACK_PACKET instance.
  PState             -  Password access is success or not, if access success then return TRUE.
  CurrentHddIndex    -  The hard disk index number.
  PswdConsoleService -  Point to Password console protocol service.
  SecurityDataInfo   -  Point to SECURITY_INFO_DATA instance.
  MasterOrUser       -  Master password (TRUE) or User password (FALSE).
  NumOfHddPswd       -  Number of hard disk password.

Returns:

  EFI_SUCCESS -          Password callback success.
  EFI_ALREADY_STARTED -  Password already exists.
  EFI_NOT_READY -        Password confirm error.

--*/
{
  static UINTN                       PasswordState;
  static UINT16                      CurrentPassword[HDD_PASSWORD_MAX_NUMBER + 1];
  static UINT16                      CurrentPasswordTemp[HDD_PASSWORD_MAX_NUMBER + 1];
  static UINT16                      InputPassword[HDD_PASSWORD_MAX_NUMBER + 1];
  UINT16                             ConfirmPassword;
  CHAR16                             *Buffer;
  EFI_STATUS                         Status;
  UINTN                              PswLength;
  UINT16                             CurrentHddIndexTmp;
  SYSTEM_CONFIGURATION               *MyIfrNVData;
  VOID                               *EncodePasswordPtr;
  VOID                               *EncodePasswordPtrTmp;
  UINTN                              EncodePasswordLength;
  UINTN                              EncodePasswordLengthTmp;

  CurrentHddIndexTmp = CurrentHddIndex;
  CurrentHddIndex = mSaveItemMapping[CurrentHddIndex][0];
  MyIfrNVData = (SYSTEM_CONFIGURATION *) Data->NvRamMap;

  ConfirmPassword = 0;
  *PState = FALSE;
  Buffer = Data->Data->Data;

  if (NumOfHddPswd <= CurrentHddIndex) {
    return EFI_NOT_FOUND;
  }

  if (Data->Data->Length == 3) {
    if (Data->Data->Data == 0) {
      //
      // Check has password or not.
      //
      PasswordState = 0;
    if (MasterOrUser == USER_PSW) {
      if (SecurityDataInfo[CurrentHddIndex].Flag == ENABLE_PASSWORD) {
        mSetHddPassword = TRUE;
        mHDDPswdBootIn  = TRUE;
        return EFI_ALREADY_STARTED;
      }
      if (SecurityDataInfo[CurrentHddIndex].Flag == CHANGE_PASSWORD) {
        return EFI_ALREADY_STARTED;
      }
      if (SecurityDataInfo[CurrentHddIndex].Flag == DISABLE_PASSWORD) {
        return EFI_SUCCESS;
      }
      if (SecurityDataInfo[CurrentHddIndex].Flag == SECURITY_LOCK) {
        mSetHddPassword = TRUE;
        mHDDPswdBootIn  = TRUE;
        return EFI_CRC_ERROR;
      }
      if (SecurityDataInfo[CurrentHddIndex].Flag == NO_ACCESS_PASSWORD) {
        return EFI_SUCCESS;
      }
    } else {
      if (SecurityDataInfo[CurrentHddIndex].MasterFlag == CHANGE_PASSWORD) {
        return EFI_ALREADY_STARTED;
      } else if (SecurityDataInfo[CurrentHddIndex].MasterFlag == DISABLE_PASSWORD) {
        return EFI_SUCCESS;
      } else if (SecurityDataInfo[CurrentHddIndex].Flag != SECURITY_LOCK && \
        SecurityDataInfo[CurrentHddIndex].DisableAllType == NO_ACCESS_PSW || \
        SecurityDataInfo[CurrentHddIndex].DisableAllType == MASTER_PSW) {
        return EFI_SUCCESS;
      } else if (SecurityDataInfo[CurrentHddIndex].Flag == SECURITY_LOCK) {
        return EFI_CRC_ERROR;
      } else {
        return EFI_ALREADY_STARTED;
      }
    }
    } else {
      //
      // Get the Password handle state.
      //
      PasswordState = (UINTN) Data->Data->Data;
    }

    return EFI_SUCCESS;
  }


  switch (PasswordState) {

  case 0:
    return EFI_SUCCESS;
    break;

  case 1:
    //
    // Enter Unlock password to confirmation.
    //
    PasswordState = 0;
    PswLength = EfiStrLen ((CHAR16*) Data->Data->Data);
    if (PswLength == 0) {
      return EFI_NOT_READY;
    }
    if ((mSetHddPassword == TRUE && SecurityDataInfo[CurrentHddIndex].Flag != SECURITY_LOCK) && (MasterOrUser == USER_PSW)) {
      EfiZeroMem (InputPassword, ((HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (UINT16)));

      EfiStrCpy (InputPassword, (CHAR16*) Data->Data->Data);

      if (SecurityDataInfo[CurrentHddIndex].Flag != ENABLE_PASSWORD) {
        if (mSetAllCurrentPassword[0] != 0) {
          EfiZeroMem (CurrentPassword, ((HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (UINT16)));
        }
        if (EfiStrCmp (mSetAllCurrentPassword, InputPassword) == 0) {
          return EFI_SUCCESS;
        }
        if (EfiStrCmp (SecurityDataInfo[CurrentHddIndex].InputString,InputPassword) == 0) {
          return EFI_SUCCESS;
        }
        if (EfiStrCmp (CurrentPassword,InputPassword) != 0) {
          return EFI_NOT_READY;
        }
        if (mHDDPswdBootIn && (mUnLockHddPassword || mSetAllHddPassword)) {
          UpdateHDDPswForm (
            Hii,
            HiiHandle,
            &SecurityDataInfo[CurrentHddIndex],
            CurrentHddIndex
            );
          return EFI_SUCCESS;
        }
      }
      Status = Unicode2Ascii (
                 InputPassword,
                 (HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (UINT16),
                 &EncodePasswordPtrTmp,
                 &EncodePasswordLengthTmp
                 );

      Status = Encode (
                 EncodePasswordPtrTmp,
                 EncodePasswordLengthTmp,
                 &EncodePasswordPtr,
                 &EncodePasswordLength
                 );

      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = PswdConsoleService->UnlockHddPassword (
                                     PswdConsoleService,
                                     SecurityDataInfo[CurrentHddIndex].HddDataInfo,
                                     MasterOrUser,
                                     EncodePasswordPtr,
                                     EncodePasswordLength
                                     );
      if (EFI_ERROR (Status)) {
        return EFI_NOT_READY;
      } else {
        EfiZeroMem (SecurityDataInfo[CurrentHddIndex].DisableInputString, (HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (UINT16));

        EfiStrCpy (SecurityDataInfo[CurrentHddIndex].DisableInputString, InputPassword);
        SecurityDataInfo[CurrentHddIndex].DisableStringLength = (HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (UINT16);
        mUnLockHddPassword = TRUE;

        UpdateHDDPswForm (
          Hii,
          HiiHandle,
          &SecurityDataInfo[CurrentHddIndex],
          CurrentHddIndex
          );
        return EFI_SUCCESS;
      }
    }
    //
    // Clear CurrentPassword buffer
    //
    EfiZeroMem (CurrentPassword, ((HDD_PASSWORD_MAX_NUMBER + 1) * sizeof(UINT16)));
    EfiStrCpy (CurrentPassword, (CHAR16*)Data->Data->Data);

    if (MasterOrUser == MASTER_PSW) {
      if (SecurityDataInfo[CurrentHddIndex].MasterFlag == CHANGE_PASSWORD) {
        if (EfiStrCmp (CurrentPassword, SecurityDataInfo[CurrentHddIndex].MasterInputString) == 0) {
          return EFI_SUCCESS;
        } else {
          return EFI_NOT_READY;
        }
      }
    }

    Status = Unicode2Ascii (
               CurrentPassword,
               (HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (UINT16),
               &EncodePasswordPtrTmp,
               &EncodePasswordLengthTmp
               );

    Status = Encode (
               EncodePasswordPtrTmp,
               EncodePasswordLengthTmp,
               &EncodePasswordPtr,
               &EncodePasswordLength
               );

    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = PswdConsoleService->UnlockHddPassword (
                                   PswdConsoleService,
                                   SecurityDataInfo[CurrentHddIndex].HddDataInfo,
                                   MasterOrUser,
                                   EncodePasswordPtr,
                                   EncodePasswordLength
                                   );
    if (EFI_ERROR (Status)) {
      return EFI_NOT_READY;
    }

    EfiZeroMem (SecurityDataInfo[CurrentHddIndex].DisableInputString, (HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (UINT16));
    EfiStrCpy (SecurityDataInfo[CurrentHddIndex].DisableInputString, CurrentPassword);
    SecurityDataInfo[CurrentHddIndex].DisableStringLength = (HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (UINT16);
    if ((MasterOrUser == MASTER_PSW) && (SecurityDataInfo[CurrentHddIndex].Flag != SECURITY_LOCK)) {
      SecurityDataInfo[CurrentHddIndex].MasterFlag = ENABLE_PASSWORD;
      return EFI_SUCCESS;
    }
    mUnLockHddPassword = TRUE;
    SecurityDataInfo[CurrentHddIndex].Flag = ENABLE_PASSWORD;
    if (!CheckHarddiskStatus (SecurityDataInfo, NumOfHddPswd)) {
      MyIfrNVData->SetAllHddPasswordFlag = TRUE;
    }

    if (mSaveItemMapping[CurrentHddIndexTmp][1] == HDD_PASSWORD_CH_1_MASTER_CBM_LABEL) {
      MyIfrNVData->C1MsCBMSetHddPasswordFlag = 1;
      MyIfrNVData->C1MsCBMUnlockHddPasswordFlag = 0;
    }
    if (mSaveItemMapping[CurrentHddIndexTmp][1] == HDD_PASSWORD_CH_2_MASTER_CBM_LABEL) {
      MyIfrNVData->C2MsCBMSetHddPasswordFlag = 1;
      MyIfrNVData->C2MsCBMUnlockHddPasswordFlag = 0;
    }
    if (mSaveItemMapping[CurrentHddIndexTmp][1] == HDD_PASSWORD_CH_3_MASTER_CBM_LABEL) {
      MyIfrNVData->C3MsCBMSetHddPasswordFlag = 1;
      MyIfrNVData->C3MsCBMUnlockHddPasswordFlag = 0;
    }
    if (mSaveItemMapping[CurrentHddIndexTmp][1] == HDD_PASSWORD_CH_4_MASTER_CBM_LABEL) {
      MyIfrNVData->C4MsCBMSetHddPasswordFlag = 1;
      MyIfrNVData->C4MsCBMUnlockHddPasswordFlag = 0;
    }
    if (mSaveItemMapping[CurrentHddIndexTmp][1] == HDD_PASSWORD_CH_1_SLAVE_CBM_LABEL) {
      MyIfrNVData->C1SlCBMSetHddPasswordFlag = 1;
      MyIfrNVData->C1SlCBMUnlockHddPasswordFlag = 0;
    }
    if (mSaveItemMapping[CurrentHddIndexTmp][1] == HDD_PASSWORD_CH_2_SLAVE_CBM_LABEL) {
      MyIfrNVData->C2SlCBMSetHddPasswordFlag = 1;
      MyIfrNVData->C2SlCBMUnlockHddPasswordFlag = 0;
    }
    if (mSaveItemMapping[CurrentHddIndexTmp][1] == HDD_PASSWORD_CH_3_SLAVE_CBM_LABEL) {
      MyIfrNVData->C3SlCBMSetHddPasswordFlag = 1;
      MyIfrNVData->C3SlCBMUnlockHddPasswordFlag = 0;
    }
    if (mSaveItemMapping[CurrentHddIndexTmp][1] == HDD_PASSWORD_CH_4_SLAVE_CBM_LABEL) {
      MyIfrNVData->C4SlCBMSetHddPasswordFlag = 1;
      MyIfrNVData->C4SlCBMUnlockHddPasswordFlag = 0;
    }
    if (mSaveItemMapping[CurrentHddIndexTmp][1] == HDD_PASSWORD_CH_1_MASTER_LABEL) {
      MyIfrNVData->C1MsSetHddPasswordFlag = 1;
      MyIfrNVData->C1MsUnlockHddPasswordFlag = 0;
    }
    if (mSaveItemMapping[CurrentHddIndexTmp][1] == HDD_PASSWORD_CH_2_MASTER_LABEL) {
      MyIfrNVData->C2MsSetHddPasswordFlag = 1;
      MyIfrNVData->C2MsUnlockHddPasswordFlag = 0;
    }
    if (mSaveItemMapping[CurrentHddIndexTmp][1] == HDD_PASSWORD_CH_3_MASTER_LABEL) {
      MyIfrNVData->C3MsSetHddPasswordFlag = 1;
      MyIfrNVData->C3MsUnlockHddPasswordFlag = 0;
    }
    if (mSaveItemMapping[CurrentHddIndexTmp][1] == HDD_PASSWORD_CH_4_MASTER_LABEL) {
      MyIfrNVData->C4MsSetHddPasswordFlag = 1;
      MyIfrNVData->C4MsUnlockHddPasswordFlag = 0;
    }
    if (mSaveItemMapping[CurrentHddIndexTmp][1] == HDD_PASSWORD_CH_1_SLAVE_LABEL) {
      MyIfrNVData->C1SlSetHddPasswordFlag = 1;
      MyIfrNVData->C1SlUnlockHddPasswordFlag = 0;
    }
    if (mSaveItemMapping[CurrentHddIndexTmp][1] == HDD_PASSWORD_CH_2_SLAVE_LABEL) {
      MyIfrNVData->C2SlSetHddPasswordFlag = 1;
      MyIfrNVData->C2SlUnlockHddPasswordFlag = 0;
    }
    if (mSaveItemMapping[CurrentHddIndexTmp][1] == HDD_PASSWORD_CH_3_SLAVE_LABEL) {
      MyIfrNVData->C3SlSetHddPasswordFlag = 1;
      MyIfrNVData->C3SlUnlockHddPasswordFlag = 0;
    }
    if (mSaveItemMapping[CurrentHddIndexTmp][1] == HDD_PASSWORD_CH_4_SLAVE_LABEL) {
      MyIfrNVData->C4SlSetHddPasswordFlag = 1;
      MyIfrNVData->C4SlUnlockHddPasswordFlag = 0;
    }
    UpdateHDDPswForm(
      Hii,
      HiiHandle,
      &SecurityDataInfo[CurrentHddIndex],
      CurrentHddIndex
      );
    return EFI_NOT_READY;

    break;
  case 2:
    //
    // Set Password Success
    //
    *PState = TRUE;
    return EFI_SUCCESS;
    break;

  case 3:
    //
    // Enter new password.
    //

    //
    // Clear CurrentPassword buffer
    //
    PswLength = EfiStrLen ((CHAR16*) Data->Data->Data);
    if (MIN_PASSWORD_LENGTH > 1) {
      if (PswLength < MIN_PASSWORD_LENGTH && PswLength != 0) {
        PasswordState = 0;
        return EFI_NOT_READY;
      }
    }
    EfiZeroMem(
      CurrentPasswordTemp,
      ((HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (UINT16))
      );

    EfiStrCpy (CurrentPasswordTemp, (UINT16*) Data->Data->Data);
    PasswordState = 0;
    return EFI_SUCCESS;
    break;

  case 4:
    //
    // Confirm new password.
    //
    PasswordState = 2;
    if (EfiStrCmp (CurrentPasswordTemp, (UINT16*) Data->Data->Data) != 0) {
      return EFI_NOT_READY;
    }
    EfiStrCpy (CurrentPassword, CurrentPasswordTemp);
    if (CurrentPassword[0] == 0) {
      if (MasterOrUser == USER_PSW) {
        SecurityDataInfo[CurrentHddIndex].Flag = DISABLE_PASSWORD;
      } else {
        SecurityDataInfo[CurrentHddIndex].MasterFlag = DISABLE_PASSWORD;
      }

    } else {
      if (MasterOrUser == USER_PSW) {
        SecurityDataInfo[CurrentHddIndex].Flag = CHANGE_PASSWORD;
        mSetHddPassword = TRUE;
        mDisableAllHddFlag = FALSE;
      } else {
        SecurityDataInfo[CurrentHddIndex].MasterFlag = CHANGE_PASSWORD;
        mDisableAllHddFlag = FALSE;
      }
      SecurityDataInfo[CurrentHddIndex].StringLength = (HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (UINT16);
      EfiZeroMem (mSetAllCurrentPassword, ((HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (UINT16)));
      if (MasterOrUser == USER_PSW) {
        EfiZeroMem (
          SecurityDataInfo[CurrentHddIndex].InputString,
          SecurityDataInfo[CurrentHddIndex].StringLength
          );

        EfiStrCpy (SecurityDataInfo[CurrentHddIndex].InputString, CurrentPassword);
      } else {
        EfiZeroMem (
          SecurityDataInfo[CurrentHddIndex].MasterInputString,
          SecurityDataInfo[CurrentHddIndex].StringLength
          );

        EfiStrCpy (SecurityDataInfo[CurrentHddIndex].MasterInputString, CurrentPassword);
      }
    }
    UpdateHDDPswForm(
      Hii,
      HiiHandle,
      &SecurityDataInfo[CurrentHddIndex],
      CurrentHddIndex
      );
    break;
  }
  return  EFI_SUCCESS;
}

EFI_STATUS
DisableAllHarddiskPswd (
  IN  EFI_FORM_CALLBACK_PROTOCOL        *FormCallback,
  IN  EFI_HII_PROTOCOL                  *Hii,
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  SECURITY_INFO_DATA                *SecurityDataInfo,
  IN  UINTN                             NumOfHddPswd
  )
/*++

Routine Description:

  Set disable password information to SecurityDataInfo and update string of Security mode.

Arguments:

  FormCallback     -     The form call back protocol.
  Hii              -     Hii protocol.
  HiiHandle        -     Specific HII handle.
  SecurityDataInfo -     Security Info table of Harddisk.
  NumOfHddPswd     -     Number of harddisk.

Returns:

  EFI_SUCCESS -          Always return EFI_SUCCESS.

--*/
{
  UINTN Index;

  for (Index = 0; Index < NumOfHddPswd; Index++) {
    SecurityDataInfo[Index].Flag = DISABLE_PASSWORD;
    UpdateHDDPswForm (
       Hii,
       HiiHandle,
       &SecurityDataInfo[Index],
       (UINT16)Index
       );
  }

  UpdateHddPasswordLabel (
    Hii,
    HiiHandle,
    SecurityDataInfo,
    NumOfHddPswd
    );

  return EFI_SUCCESS;
}

EFI_STATUS
SetAllHarddiskPswd (
  IN  EFI_FORM_CALLBACK_PROTOCOL        *FormCallback,
  IN  BOOLEAN                           MasterOrUser,
  IN  EFI_HII_PROTOCOL                  *Hii,
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  EFI_IFR_DATA_ARRAY                *Data,
  OUT EFI_HII_CALLBACK_PACKET           **Packet,
  OUT BOOLEAN                           *PState,
  IN  SECURITY_INFO_DATA                *SecurityDataInfo,
  IN  UINTN                             NumOfHddPswd
  )
/*++

Routine Description:

  Set password to all harddisk

Arguments:

  FormCallback       -  The form call back protocol.
  Hii                -  Hii protocol.
  MasterOrUser       -  Master password (TRUE) or User password (FALSE)
  HiiHandle          -  Specific HII handle.
  Data               -  A pointer to the data being sent to the original exporting driver.
  Packet             -  Pointer EFI_HII_CALLBACK_PACKET instance
  *PState            -  Password access is success or not, if access success then return TRUE.
  SecurityDataInfo   -  Pointer to SECURITY_INFO_DATA instance
  NumOfHddPswd       -  Number of Harddisk

Returns:

  EFI_SUCCESS -          Password callback success.
  EFI_ALREADY_STARTED -  Already password in SetupUtility.
  EFI_NOT_READY -        Password confirm error.

--*/
{
  static UINTN                          PasswordState;
  static UINT16                         CurrentPassword[HDD_PASSWORD_MAX_NUMBER + 1];
  UINT16                                ConfirmPassword;
  CHAR16                                *Buffer;
  UINTN                                 Index;
  UINTN                                 SetSecurityFlag;
  UINTN                                 PswLength;
  SYSTEM_CONFIGURATION                  *MyIfrNVData;

  ConfirmPassword = 0;
  *PState = FALSE;
  Buffer = Data->Data->Data;
  Index = 0;
  SetSecurityFlag = 0;

  if (Data->Data->Length == 3) {

    if (Data->Data->Data != 0) {
      //
      // Get the Password handle state.
      //
      PasswordState = (UINTN) Data->Data->Data;
    }

    return EFI_SUCCESS;
  }


  switch (PasswordState) {

  case 0:
    return EFI_SUCCESS;
    break;

  case 1:
    //
    // Force set new password
    //
    return EFI_NOT_READY;
    break;

  case 2:
    //
    // Set Password Success
    //
    *PState = TRUE;
    return EFI_SUCCESS;
    break;
  case 3:
    //
    // Enter new password.
    //

    //
    // Clear CurrentPassword buffer
    //
    PswLength = EfiStrLen ((CHAR16*) Data->Data->Data);
    if (MIN_PASSWORD_LENGTH > 1) {
      if (PswLength < MIN_PASSWORD_LENGTH && PswLength != 0) {
        PasswordState = 0;
        return EFI_NOT_READY;
      }
    }
    EfiZeroMem (CurrentPassword, ((HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (UINT16)));
    EfiStrCpy (CurrentPassword,(UINT16*) Data->Data->Data);
    EfiZeroMem (mSetAllCurrentPassword, ((HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (UINT16)));
    EfiStrCpy (mSetAllCurrentPassword, CurrentPassword);
    PasswordState = 0;
    return EFI_SUCCESS;
    break;

  case 4:
    //
    // Confirm new password.
    //
    PasswordState = 2;
    if (EfiStrCmp (CurrentPassword, (UINT16*) Data->Data->Data) != 0) {
      for (Index = 0; Index < NumOfHddPswd; Index++) {
         SecurityDataInfo[Index].Flag = NO_ACCESS_PASSWORD;
      }
      return EFI_NOT_READY;
    }


    if ((CurrentPassword[0] == 0)) {
      SetSecurityFlag = DISABLE_PASSWORD;
      mDisableAllHddFlag = TRUE;
      MyIfrNVData = (SYSTEM_CONFIGURATION *) Data->NvRamMap;
      MyIfrNVData->SetAllMasterHddPasswordFlag = FALSE;
    } else {
      mSetHddPassword = TRUE;
      mSetAllHddPassword = TRUE;
      SetSecurityFlag = CHANGE_PASSWORD;
      mDisableAllHddFlag = FALSE;
      MyIfrNVData = (SYSTEM_CONFIGURATION *) Data->NvRamMap;
      MyIfrNVData->SetAllMasterHddPasswordFlag = TRUE;
    }

   if (MasterOrUser == USER_PSW) {
      for (Index = 0; Index < NumOfHddPswd; Index++) {
         SecurityDataInfo[Index].Flag = SetSecurityFlag;
         SecurityDataInfo[Index].StringLength = (HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (UINT16);
         EfiZeroMem (
           SecurityDataInfo[Index].InputString,
           SecurityDataInfo[Index].StringLength
           );

         EfiStrCpy (
           SecurityDataInfo[Index].InputString,
           CurrentPassword
           );

         UpdateHDDPswForm (
           Hii,
           HiiHandle,
           &SecurityDataInfo[Index],
           (UINT16)Index
           );
      }
    } else {
      for (Index = 0; Index < NumOfHddPswd; Index++) {
         SecurityDataInfo[Index].MasterFlag = SetSecurityFlag;
         SecurityDataInfo[Index].StringLength = (HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (UINT16);

         EfiZeroMem (
           SecurityDataInfo[Index].MasterInputString,
           SecurityDataInfo[Index].StringLength
           );

         EfiStrCpy (
           SecurityDataInfo[Index].MasterInputString,
           CurrentPassword
           );

         UpdateHDDPswForm (
           Hii,
           HiiHandle,
           &SecurityDataInfo[Index],
           (UINT16)Index
           );
      }
    }

    UpdateHddPasswordLabel (
      Hii,
      HiiHandle,
      SecurityDataInfo,
      NumOfHddPswd
      );
    break;
  }
  return  EFI_SUCCESS;
}

EFI_STATUS
PasswordCheck (
  IN  SETUP_UTILITY_CONFIGURATION       *ScuInfo,
  IN  SYSTEM_CONFIGURATION              *SetupVariable
  )
/*++

Routine Description:

  Check system pasword status.

Arguments:

  ScuInfo       -      Point to SETUP_UTILITY_CONFIGURATION instance.
  SetupVariable -      The Setup NV data.

Returns:

  EFI_SUCCESS   -      Check system password success.
  EFI_NOT_FOUND -      Not found system password.

--*/
{
  EFI_STATUS                            Status;
  EFI_HII_PROTOCOL                      *Hii;
  EFI_HII_HANDLE                        HiiHandle;
  CHAR16                                *NewString;
  UINTN                                 NumOfSupervisorPswdInfo;
#ifdef SUPPORT_USER_PASSWORD
  UINTN                                 NumOfUserPswdInfo;
#endif
  SECURITY_INFO_DATA                    *SecurityInfoData;
  OEM_SERVICES_PROTOCOL                 *OemServices;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  VOID                                  *HobList;
  EFI_BOOT_MODE                         BootMode;
  STATIC SYSTEM_PASSWORD_UNLOCK_STATUS  SysPasswordUnlockStatus = BeforeHotKey;

  NumOfSupervisorPswdInfo = 0;
#ifdef SUPPORT_USER_PASSWORD  
  NumOfUserPswdInfo = 0;
#endif  

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }


  Hii              = SuBrowser->Hii;
  HiiHandle        = ScuInfo->SecurityHiiHandle;
  SecurityInfoData = NULL;

  if (ScuInfo->SysPasswordService == NULL) {
    return EFI_UNSUPPORTED;
  }
  //
  // Get Hob list
  //
  Status = EfiLibGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
  Status = GetHobBootMode (HobList, &BootMode);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }
  if (BootMode == BOOT_ON_S4_RESUME) {
    return EFI_SUCCESS;
  }
  //
  // Get Supervisor password
  //
  Status = ScuInfo->SysPasswordService->GetStatus (
                                          ScuInfo->SysPasswordService,
                                          SystemSupervisor
                                          );
  if (!EFI_ERROR (Status)) {
    NumOfSupervisorPswdInfo = 1;
  }

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof (SECURITY_INFO_DATA),
                  &SecurityInfoData
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }
  SecurityInfoData->Flag           = NO_ACCESS_PASSWORD;
  SecurityInfoData->InputString[0] = 0x00;
  SecurityInfoData->StringLength   = 0x00;
  SecurityInfoData->NumOfEntry     = NumOfSupervisorPswdInfo;

  ScuInfo->SupervisorPassword = SecurityInfoData;


  //
  // Get User password
  //
#ifdef SUPPORT_USER_PASSWORD
  Status = ScuInfo->SysPasswordService->GetStatus (
                                          ScuInfo->SysPasswordService,
                                          SystemUser
                                          );
  SecurityInfoData = NULL;

  if (!EFI_ERROR (Status)) {
    NumOfUserPswdInfo = 1;
  }


  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof (SECURITY_INFO_DATA),
                  &SecurityInfoData
                  );

  if (EFI_ERROR (Status)) {
    gBS->FreePool (ScuInfo->SupervisorPassword);
    return Status;
  }
  SecurityInfoData->Flag = NO_ACCESS_PASSWORD;
  SecurityInfoData->InputString[0] = 0x00;
  SecurityInfoData->StringLength = 0x00;
  SecurityInfoData->NumOfEntry = NumOfUserPswdInfo;

  ScuInfo->UserPassword = SecurityInfoData;

  if ((NumOfSupervisorPswdInfo != 0) || (NumOfUserPswdInfo != 0)) {
#else
  if (NumOfSupervisorPswdInfo != 0) {
#endif
    //
    // Locate OemServices Protocol
    //
    OemServices = NULL;
    Status = gBS->LocateProtocol (&gOemServicesProtocolGuid, NULL, &OemServices);
    if (!EFI_ERROR (Status)) {
      Status = OemServices->Funcs[COMMON_PASSWORD_ENTRY_CALL_BACK] (OemServices, COMMON_PASSWORD_ENTRY_CALL_BACK_ARG_COUNT);
    }
  }



  if (NumOfSupervisorPswdInfo == 0) {
    NewString = SetupUtilityLibGetTokenString (
                  ScuInfo->SecurityHiiHandle,
                  STRING_TOKEN(STR_NOT_INSTALLED_TEXT)
                  );
    UpdateSupervisorTags (
      Hii,
      HiiHandle,
      FALSE,
      NewString,
      SetupVariable
      );
    SuBrowser->SCBuffer->SupervisorFlag = 0;
    SuBrowser->SCBuffer->UserFlag = 0;
    SuBrowser->SUCInfo->SupervisorPwdFlag = FALSE;
    SuBrowser->SUCInfo->UserPwdFlag       = FALSE;
    return EFI_SUCCESS;
  }

  if (*((UINT16*)SetupVariable->PowerOnPassword) == POWER_ON_PASSWORD && 
      SysPasswordUnlockStatus == ScuFirst) {
    SysPasswordUnlockStatus = ScuReEnter;
    return EFI_SUCCESS;
  }

  if (SysPasswordUnlockStatus == BeforeHotKey) {
    DisableQuietBoot ();
    gST->ConOut->EnableCursor (gST->ConOut, TRUE);
  }

  Status = CheckSetupPassword (
             ScuInfo->SecurityHiiHandle,
             ScuInfo->SysPasswordService,
             ScuInfo,
             SetupVariable
             );

  if (SysPasswordUnlockStatus == BeforeHotKey) {
    SysPasswordUnlockStatus = ScuFirst;
  }

  NewString = SetupUtilityLibGetTokenString (
                ScuInfo->SecurityHiiHandle,
                STRING_TOKEN (STR_INSTALLED_TEXT)
                );
  UpdateSupervisorTags (
    Hii,
    HiiHandle,
    TRUE,
    NewString,
    SetupVariable
    );
  SuBrowser->SCBuffer->SupervisorFlag = 1;
  SuBrowser->SUCInfo->SupervisorPwdFlag = TRUE;
#ifdef SUPPORT_USER_PASSWORD
  if (NumOfUserPswdInfo == 0){
    SuBrowser->SCBuffer->UserFlag = 0;
    SuBrowser->SUCInfo->UserPwdFlag = FALSE;
  } else {
    SuBrowser->SCBuffer->UserFlag = 1;
    SuBrowser->SUCInfo->UserPwdFlag = TRUE;
  }

  if (NumOfUserPswdInfo != 0) {
    UpdateUserTags (
      Hii,
      HiiHandle,
      TRUE,
      NewString,
      SetupVariable
      );
  }
#endif
  gBS->FreePool (NewString);

  return EFI_SUCCESS;
}

EFI_STATUS
UpdateSupervisorTags (
  IN  EFI_HII_PROTOCOL                  *Hii,
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  BOOLEAN                           Installed,
  IN  CHAR16                            *NewString,
  IN  SYSTEM_CONFIGURATION              *SetupVariable
  )
/*++

Routine Description:

  Update supervisor tag.

Arguments:

  Hii           -     Hii protocol.
  HiiHandle     -     Return string token of device status.
  Installed     -     TRUE installed supervisor password.
  *NewString    -     String bufer.
  SetupVariable -     The Setup NV data.

Returns:

  EFI_SUCCESS   -        Always return.

--*/
{
  STRING_REF                            TokenToUpdate;

  if (Installed) {
    SetupVariable->SupervisorFlag = 1;
  } else {
    SetupVariable->SupervisorFlag = 0;
#ifdef SUPPORT_USER_PASSWORD
    UpdateUserTags (Hii, HiiHandle, FALSE, NewString,SetupVariable);
#endif
  }

  TokenToUpdate = STRING_TOKEN (STR_SUPERVISOR_PASSWORD_STRING2);
  Hii->NewString(Hii, NULL, HiiHandle, &TokenToUpdate, NewString);

  return EFI_SUCCESS;
}

#ifdef SUPPORT_USER_PASSWORD
EFI_STATUS
UpdateUserTags (
  IN  EFI_HII_PROTOCOL                  *Hii,
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  BOOLEAN                           Installed,
  IN  CHAR16                            *NewString,
  IN  SYSTEM_CONFIGURATION              *SetupVariable
  )
/*++

Routine Description:

  Update user password tag.

Arguments:

  Hii           -   Hii protocol.
  HiiHandle     -   Return string token of device status.
  Installed     -   TRUE installed supervisor password.
  *NewString    -   String bufer.
  SetupVariable -   The Setup NV data.

Returns:

  EFI_SUCCESS   -   Always return.

--*/
{
  STRING_REF                            TokenToUpdate;

  if (Installed) {
    SetupVariable->UserFlag = TRUE;
  } else {
    SetupVariable->UserFlag = FALSE;
  }

  TokenToUpdate = STRING_TOKEN (STR_USER_PASSWORD_STRING2);
  Hii->NewString (
         Hii,
         NULL,
         HiiHandle,
         &TokenToUpdate,
         NewString
         );

  return EFI_SUCCESS;
}
#endif

STATIC
EFI_STATUS
CheckSetupPassword (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL *PswdConsoleService,
  IN  SETUP_UTILITY_CONFIGURATION       *ScuInfo,
  IN  SYSTEM_CONFIGURATION              *SetupVariable
  )
/*++

Routine Description:

  Check system pasword status.

Arguments:

  FormCallback            - The form call back protocol.
  PswdConsoleService      - Password console service ptr
  SupervisorPswdInfoPtr   - The simple password protocol of supervisor.
  UserPswdInfoPtr         - The simple password protocol of user.
  SetupVariable           - pointer to SYSTEM_CONFIGURATION  instance

Returns:

  EFI_SUCCESS   -      Check system password success.
  EFI_NOT_FOUND -      Not found system password.

--*/
{
  EFI_STATUS                            Status;
  EFI_INPUT_KEY                         Key;
  CHAR16                                *PasswordString;
  CHAR16                                StringPtr[MAX_PASSWORD_NUMBER];
  CHAR16                                *ErrorStatusMsg;
  CHAR16                                *ThreeIncorrectMsg;
  CHAR16                                *RebootMsg;
  UINTN                                 MaxLength;
  UINTN                                 TryPasswordTimes;
  BOOLEAN                               DrawFlag;
  BOOLEAN                               ReadChar;
  UINT16                                Password;
  EFI_OEM_FORM_BROWSER_PROTOCOL         *OemFormBrowserPtr;
  UINTN                                 PswdInfoIndex;
  OEM_SERVICES_PROTOCOL                 *OemServices;
  CHAR16                                *SysInvalidPassword;

  if (ScuInfo->SupervisorPassword->NumOfEntry == 0) {
    return EFI_SUCCESS;
  }

  ErrorStatusMsg        = SetupUtilityLibGetTokenString (HiiHandle, STRING_TOKEN (STR_HDD_DIALOG_ERROR_STATUS));
  ThreeIncorrectMsg    = SetupUtilityLibGetTokenString (HiiHandle, STRING_TOKEN (STR_HDD_DIALOG_THREE_ERRORS));
  RebootMsg             = SetupUtilityLibGetTokenString (HiiHandle, STRING_TOKEN (STR_HDD_DIALOG_RESTART_MSG));
  SysInvalidPassword    = SetupUtilityLibGetTokenString (HiiHandle, STRING_TOKEN (STR_SYS_DIALOG_INVALID_PASSWORD));

  Password = 0;
  DrawFlag = FALSE;
  ReadChar = TRUE;
  TryPasswordTimes = 3;
  PswdInfoIndex = 0;


  Status = gBS->LocateProtocol (
                  &gEfiOemFormBrowserProtocolGuid,
                 NULL,
                 &OemFormBrowserPtr
                 );

  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (
                  &gOemServicesProtocolGuid,
                  NULL,
                  &OemServices
                  );

  if ( !EFI_ERROR (Status) ) {
    OemServices->Funcs[COMMON_GET_MAX_CHECK_COUNT] (
                   OemServices,
                   COMMON_GET_MAX_CHECK_COUNT_ARG_COUNT,
                   &TryPasswordTimes
                   );

  } else {
  	OemServices = NULL;
  }

  MaxLength = MIN_STRING_LENGTH;


  PasswordString = SetupUtilityLibGetTokenString (
                     HiiHandle,
                     STRING_TOKEN (STR_CHECK_PASSWORD_STRING)
                     );
  //
  // Clear screen before print Harddisk passwork prompt message
  //
  gST->ConOut->ClearScreen (gST->ConOut);                    
  do {
    EfiZeroMem (StringPtr, MAX_PASSWORD_NUMBER * sizeof (CHAR16));
    OemFormBrowserPtr->DialogIcon (
                         0,
                         FALSE,
                         MAX_PASSWORD_NUMBER,
                         StringPtr,
                         &Key,
                         PasswordString
                         );

    if (ScuInfo->SupervisorPassword->NumOfEntry != 0) {
      Status = PswdConsoleService->GetStatus (
                                     PswdConsoleService,
                                     SystemSupervisor
                                     );
      if (Status == EFI_SUCCESS) {
        //
        // Check Supervisor password
        //
        Status = PswdConsoleService->CheckPassword (
                                       PswdConsoleService,
                                       StringPtr,
                                       MAX_PASSWORD_NUMBER * sizeof(CHAR16),
                                       SystemSupervisor
                                       );

        if (Status == EFI_SUCCESS) {
          SetupVariable->SetUserPass = FALSE;
          return EFI_SUCCESS;
        }
      }
    }
#ifdef SUPPORT_USER_PASSWORD
    if (ScuInfo->UserPassword->NumOfEntry != 0) {
      Status = PswdConsoleService->GetStatus(
                                     PswdConsoleService,
                                     SystemUser
                                     );

      if (Status == EFI_SUCCESS) {
        //
        // Check User password
        //
        Status = PswdConsoleService->CheckPassword (
                                       PswdConsoleService,
                                       StringPtr,
                                       MAX_PASSWORD_NUMBER * sizeof(CHAR16),
                                       SystemUser
                                       );

        if (Status == EFI_SUCCESS) {
          SetupVariable->SetUserPass = TRUE;
          return EFI_SUCCESS;
        }
      }
    }
#endif
    TryPasswordTimes--;
    ReadChar = TRUE;
    if (TryPasswordTimes > 0) {
      OemFormBrowserPtr->CreatePopUp (
                           2,
                           FALSE,
                           40,
                           NULL,
                           &Key,
                           SysInvalidPassword
                           );
    }
  } while (TryPasswordTimes > 0);

  if ( OemServices != NULL ) {
    OemServices->Funcs[COMMON_ERROR_PASSWORD_CALL_BACK] (
                   OemServices,
                   COMMON_ERROR_PASSWORD_CALL_BACK_ARG_COUNT
                   );
  }

  OemFormBrowserPtr->CreateMsgPopUp (
                       40,
                       3,
                       ErrorStatusMsg,
                       ThreeIncorrectMsg,
                       RebootMsg
                       );

  gBS->FreePool (PasswordString);
  while (TRUE);

  return EFI_SECURITY_VIOLATION;
}

BOOLEAN
CheckHarddiskStatus (
  IN  SECURITY_INFO_DATA         *PswHddInfoTablePtr,
  IN  UINTN                      NumOfHarddisk
  )
/*++

Routine Description:

  Check Harddisk security ststus.

Arguments:

  PswHddInfoTablePtr      - The Harddisk information array.
  NumOfHarddisk           - Number of simple password protocol of harddisk.

Returns:

  BOOLEAN                 -  If TRUE have harddisk security status is locked.
                             If false could not found any harddisk security status is locked.

--*/
{
  UINT32  Index;
  UINT32  StatusFlag;

  if ((PswHddInfoTablePtr == NULL) || (NumOfHarddisk == 0)) {
    return FALSE;
  }

  StatusFlag = 0;
  for (Index = 0; Index < NumOfHarddisk; Index++) {
    StatusFlag |= PswHddInfoTablePtr[Index].Flag;
  }

  if ((StatusFlag & SECURITY_LOCK) != 0) {
    return TRUE;
  }

  return FALSE;
}

EFI_STATUS
HddPasswordCheck (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  BOOLEAN                           BootOrSCU,
  IN  SYSTEM_CONFIGURATION              *SetupVariable,
  OUT SEC_HDD_PASSWORD_STRUCT           **SavePasswordPoint
  )
/*++

Routine Description:

  Check hard disk password status.

Arguments:

  HiiHandle               - Specific HII handle for Security menu.
  BootOrSCU               - TRUE: In to SCU
                            FALSE: want to boot
  SetupVariable           - Point to SYSTEM_CONFIGURATION instance.
  SavePasswordPoint       - Point to security harddisk password instance

Returns:

  EFI_SUCCESS   -      Check hard disk password success.
  EFI_NOT_FOUND -      Not found system password.

--*/
{
  EFI_STATUS                            Status;
  UINTN                                 Index;
  SECURITY_INFO_DATA                    *SecurityInfoArray;
  EFI_HDD_PASSWORD_SERVICE_PROTOCOL     *HddPswdServiceProtocol;
  HDD_SECURITY_INFO_DATA                *HarddiskStr;
  UINTN                                 NumOfHDDInfo;
  VOID                                  *HobList;
  EFI_BOOT_MODE                         BootMode;
  UINT8                                 IndexTmp;
  CHAR16                                HardDiskPassword[HDD_PASSWORD_MAX_NUMBER];
  UINTN                                 PswdLength;
  UINTN                                 DataSize;
  SEC_HDD_PASSWORD_STRUCT               *HddPasswordTable;
  BOOLEAN                               UnlockPasswordType;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  EFI_CHIPSET_LIB_SERVICES_PROTOCOL     *ChipsetLibServices;

  Status = gBS->LocateProtocol (
                        &gEfiChipsetLibServicesProtocolGuid,
                        NULL,
                        &ChipsetLibServices
                        );
  ASSERT_EFI_ERROR (Status);

  HddPswdServiceProtocol  = NULL;
  DataSize                = 0;
  HddPasswordTable        = NULL;
  HarddiskStr             = NULL;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo = SuBrowser->SUCInfo;
  SecurityInfoArray = SUCInfo->HarddiskPassword;

  //
  // Get Hob list
  //
  Status = EfiLibGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
  Status = GetHobBootMode (HobList, &BootMode);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  Status = gBS->LocateProtocol (
                  &gEfiHddPasswordServiceProtocolGuid,
                  NULL,
                  &HddPswdServiceProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Clear screen before print Harddisk passwork prompt message
  //
  gST->ConOut->ClearScreen (gST->ConOut);
  Status = HddPswdServiceProtocol->GetHddSecurityInfo (
                            HddPswdServiceProtocol,
                            (VOID *)&HarddiskStr
                            );

  if (Status != EFI_SUCCESS) {
    return Status;
  }

  NumOfHDDInfo = HarddiskStr->TotalNumOfHddInfo;

  if (BootMode == BOOT_ON_S4_RESUME) {
    HddPasswordTable = SetupUtilityLibGetVariableAndSize (
                         SAVE_HDD_PASSWORD_VARIABLE_NAME,
                         &gSaveHddPasswordGuid,
                         &DataSize
                         );

    for (Index = 0; Index < NumOfHDDInfo; Index++) {
      if ((HarddiskStr[Index].HddSecurityStatus & HDD_SECURITY_LOCK) != HDD_SECURITY_LOCK) {
        continue;
      }
      if (HarddiskStr[Index].ControllerMode == IDE_MODE) {
        if (HarddiskStr[Index].ControllerNumber == 0) {
          if (HarddiskStr[Index].PortNumber == 0 && HarddiskStr[Index].PortMulNumber == 0) {
            EBDA (EBDA_HDD_LOCKED_FLAG) |= bit (0);
          } else if (HarddiskStr[Index].PortNumber == 0 && HarddiskStr[Index].PortMulNumber == 1) {
            EBDA (EBDA_HDD_LOCKED_FLAG) |= bit (1);
          } else if (HarddiskStr[Index].PortNumber == 1 && HarddiskStr[Index].PortMulNumber == 0) {
            EBDA (EBDA_HDD_LOCKED_FLAG) |= bit (2);
          } else {
            EBDA (EBDA_HDD_LOCKED_FLAG) |= bit (3);
          }
        } else {
          if (HarddiskStr[Index].PortNumber == 0 && HarddiskStr[Index].PortMulNumber == 0) {
            EBDA (EBDA_HDD_LOCKED_FLAG) |= bit (4);
          } else if (HarddiskStr[Index].PortNumber == 0 && HarddiskStr[Index].PortMulNumber == 1) {
            EBDA (EBDA_HDD_LOCKED_FLAG) |= bit (5);
          } else if (HarddiskStr[Index].PortNumber == 1 && HarddiskStr[Index].PortMulNumber == 0) {
            EBDA (EBDA_HDD_LOCKED_FLAG) |= bit (6);
          } else {
            EBDA (EBDA_HDD_LOCKED_FLAG) |= bit (7);
          }
        }
      } else {
        EBDA (EBDA_HDD_LOCKED_FLAG) |= bit (HarddiskStr[Index].PortNumber);
      }
      for (IndexTmp = 0; IndexTmp < NumOfHDDInfo; IndexTmp++ ) {
        if (HarddiskStr[Index].PortNumber == HddPasswordTable[IndexTmp].PortNumber &&
            HarddiskStr[Index].PortMulNumber == HddPasswordTable[IndexTmp].PortMulNumber) {
          if (HddPasswordTable[IndexTmp].PasswordType == USER_PSW && HddPasswordTable[IndexTmp].PasswordStr[0] != 0) {
            EfiCopyMem (HardDiskPassword, HddPasswordTable[IndexTmp].PasswordStr, (HDD_PASSWORD_MAX_NUMBER * sizeof (UINT16)));
            UnlockPasswordType = USER_PSW;
          } else {
            EfiCopyMem (HardDiskPassword, HddPasswordTable[IndexTmp].PasswordStr, (HDD_PASSWORD_MAX_NUMBER * sizeof (UINT16)));
            UnlockPasswordType = MASTER_PSW;
          }

          PswdLength = EfiStrLen (HardDiskPassword);

          HddPswdServiceProtocol->UnlockHddPassword (
                                    HddPswdServiceProtocol,
                                    &HarddiskStr[IndexTmp],
                                    UnlockPasswordType,
                                    (UINT8 *)HardDiskPassword,
                                    (PswdLength * sizeof (UINT16))
                                    );
          //
          //To clean the locked flogs if unlock Hdd success from the EBDA offset 0x3d9
          //
          if (HarddiskStr[Index].ControllerMode == IDE_MODE) {
            if (HarddiskStr[Index].ControllerNumber == 0) {
              if (HarddiskStr[IndexTmp].PortNumber == 0 && HarddiskStr[IndexTmp].PortMulNumber == 0) {
                EBDA (EBDA_HDD_LOCKED_FLAG) &= ~(bit (0));
              } else if (HarddiskStr[IndexTmp].PortNumber == 0 && HarddiskStr[IndexTmp].PortMulNumber == 1) {
                EBDA (EBDA_HDD_LOCKED_FLAG) &= ~(bit (1));
              } else if (HarddiskStr[IndexTmp].PortNumber == 1 && HarddiskStr[IndexTmp].PortMulNumber == 0) {
                EBDA (EBDA_HDD_LOCKED_FLAG) &= ~(bit (2));
              } else {
                EBDA (EBDA_HDD_LOCKED_FLAG) &= ~(bit (3));
              }
            } else {
              if (HarddiskStr[IndexTmp].PortNumber == 0 && HarddiskStr[IndexTmp].PortMulNumber == 0) {
                EBDA (EBDA_HDD_LOCKED_FLAG) &= ~(bit (4));
              } else if (HarddiskStr[IndexTmp].PortNumber == 0 && HarddiskStr[IndexTmp].PortMulNumber == 1) {
                EBDA (EBDA_HDD_LOCKED_FLAG) &= ~(bit (5));
              } else if (HarddiskStr[IndexTmp].PortNumber == 1 && HarddiskStr[IndexTmp].PortMulNumber == 0) {
                EBDA (EBDA_HDD_LOCKED_FLAG) &= ~(bit (6));
              } else {
                EBDA (EBDA_HDD_LOCKED_FLAG) &= ~(bit (7));
              }
            }
          } else {
            EBDA (EBDA_HDD_LOCKED_FLAG) &= ~(bit (HarddiskStr[IndexTmp].PortNumber));
          }
        }
      }
    }
    gBS->FreePool (HddPasswordTable);
    return EFI_NOT_FOUND;
  }

  if (BootOrSCU == INTO_SCU) {
    for (Index = 0; Index < NumOfHDDInfo; Index++) {
      if ((HarddiskStr[Index].HddSecurityStatus & HDD_SECURITY_FROZEN) == HDD_SECURITY_FROZEN) {
        HddPswdServiceProtocol->SetFeaturesCmd (
                                  HddPswdServiceProtocol,
                                  &HarddiskStr[Index],
                                  0x90,
                                  0x06
                                  );
        Status = ChipsetLibServices->SataComreset (HarddiskStr[Index].PortNumber);
        if (EFI_ERROR (Status)) {
          return Status;
        }

        gBS->Stall (100000);

        HddPswdServiceProtocol->SetFeaturesCmd (
                                  HddPswdServiceProtocol,
                                  &HarddiskStr[Index],
                                  0x10,
                                  0x06
                                  );
      }
    }
  }

  if (!CheckHarddiskStatus (SecurityInfoArray, NumOfHDDInfo)) {
    SetupVariable->SetAllMasterHddPasswordFlag = FALSE;
    return EFI_SUCCESS;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
SetSecurityStatus (
  VOID
  )
/*++

Routine Description:

  Set user set password status. This founction could set password status
  when press 'F10' and set 'yes'.

Arguments:

  none

Returns:

  EFI_SUCCESS   -      Set system password and harddisk password success.

--*/
{
  UINTN                                     Index;
  VOID                                      *EncodePasswordPtr;
  VOID                                      *EncodePasswordPtrTmp;
  UINTN                                     EncodePasswordLength;
  UINTN                                     EncodePasswordLengthTmp;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  EFI_STATUS                                Status;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo = SuBrowser->SUCInfo;

#ifdef SUPPORT_USER_PASSWORD
    //
    // Set User Password
    //
  if (SUCInfo->UserPassword->Flag != NO_ACCESS_PASSWORD) {
    SUCInfo->SysPasswordService->DisablePassword (
                                   SUCInfo->SysPasswordService,
                                   SystemUser
                                   );

    if (SUCInfo->UserPassword->Flag == CHANGE_PASSWORD) {
      SUCInfo->SysPasswordService->SetPassword (
                                     SUCInfo->SysPasswordService,
                                     SUCInfo->UserPassword->InputString,
                                     SUCInfo->UserPassword->StringLength,
                                     SystemUser
                                     );
    }
  }
#endif

  //
  // Set Supervisor Password
  //
  if (SUCInfo->SupervisorPassword->Flag != NO_ACCESS_PASSWORD) {
    SUCInfo->SysPasswordService->DisablePassword (
                                   SUCInfo->SysPasswordService,
                                   SystemSupervisor
                                   );

    if (SUCInfo->SupervisorPassword->Flag == CHANGE_PASSWORD) {
      SUCInfo->SysPasswordService->SetPassword (
                                     SUCInfo->SysPasswordService,
                                     SUCInfo->SupervisorPassword->InputString,
                                     SUCInfo->SupervisorPassword->StringLength,
                                     SystemSupervisor
                                     );
    }
  }
    //
    // Harddisk security
    //
  for (Index = 0; Index < SUCInfo->NumOfHarddiskPswd; Index++) {
    if ((SUCInfo->HarddiskPassword[Index].Flag != CHANGE_PASSWORD) && (SUCInfo->HarddiskPassword[Index].Flag != DISABLE_PASSWORD) && (SUCInfo->HarddiskPassword[Index].MasterFlag != CHANGE_PASSWORD) && (SUCInfo->HarddiskPassword[Index].MasterFlag != DISABLE_PASSWORD)) {
      continue;
    }

    if (SUCInfo->HarddiskPassword[Index].Flag == SECURITY_LOCK) {
      continue;
    }

    //
    //Disable all Hdd password
    //
    if (mDisableAllHddFlag == TRUE) {
      Status = Unicode2Ascii (
                 SUCInfo->HarddiskPassword[Index].DisableAllInputString,
                 SUCInfo->HarddiskPassword[Index].DisableStringLength,
                 &EncodePasswordPtrTmp,
                 &EncodePasswordLengthTmp
                 );

      Status = Encode (
                 EncodePasswordPtrTmp,
                 EncodePasswordLengthTmp,
                 &EncodePasswordPtr,
                 &EncodePasswordLength
                 );

      if (EFI_ERROR(Status)) {
        return Status;
      }

      SUCInfo->PswdConsoleService->DisableService (
                                     SUCInfo->PswdConsoleService,
                                     SUCInfo->HarddiskPassword[Index].HddDataInfo,
                                     SUCInfo->HarddiskPassword[Index].DisableAllType,
                                     EncodePasswordPtr,
                                     EncodePasswordLength
                                     );

    }
    //
    // Change HDD user password
    //
    if (SUCInfo->HarddiskPassword[Index].Flag == CHANGE_PASSWORD) {
      //
      // Disable Password
      //
      Status = Unicode2Ascii (
                 SUCInfo->HarddiskPassword[Index].DisableInputString,
                 SUCInfo->HarddiskPassword[Index].DisableStringLength,
                 &EncodePasswordPtrTmp,
                 &EncodePasswordLengthTmp
                 );

      Status = Encode (
                 EncodePasswordPtrTmp,
                 EncodePasswordLengthTmp,
                 &EncodePasswordPtr,
                 &EncodePasswordLength
                 );

      if (EFI_ERROR(Status)) {
        return Status;
      }

      SUCInfo->PswdConsoleService->DisableService (
                                     SUCInfo->PswdConsoleService,
                                     SUCInfo->HarddiskPassword[Index].HddDataInfo,
                                     USER_PSW,
                                     EncodePasswordPtr,
                                     EncodePasswordLength
                                     );
      //
      // Change Password
      //
      Status = Unicode2Ascii (
                 SUCInfo->HarddiskPassword[Index].InputString,
                 SUCInfo->HarddiskPassword[Index].StringLength,
                 &EncodePasswordPtrTmp,
                 &EncodePasswordLengthTmp
                 );

      Status = Encode (
                 EncodePasswordPtrTmp,
                 EncodePasswordLengthTmp,
                 &EncodePasswordPtr,
                 &EncodePasswordLength
                 );

      if (EFI_ERROR(Status)) {
        return Status;
      }
      SUCInfo->PswdConsoleService->SetHddPassword (
                                     SUCInfo->PswdConsoleService,
                                     SUCInfo->HarddiskPassword[Index].HddDataInfo,
                                     USER_PSW,
                                     EncodePasswordPtr,
                                     EncodePasswordLength
                                     );
    }

    //
    // Change HDD Master password
    //
    if (SUCInfo->HarddiskPassword[Index].MasterFlag == CHANGE_PASSWORD) {
      //
      // Change Password
      //
      Status = Unicode2Ascii (
                 SUCInfo->HarddiskPassword[Index].MasterInputString,
                 SUCInfo->HarddiskPassword[Index].StringLength,
                 &EncodePasswordPtrTmp,
                 &EncodePasswordLengthTmp
                 );

      Status = Encode (
                 EncodePasswordPtrTmp,
                 EncodePasswordLengthTmp,
                 &EncodePasswordPtr,
                 &EncodePasswordLength
                 );

      if (EFI_ERROR(Status)) {
        return Status;
      }
      SUCInfo->PswdConsoleService->SetHddPassword (
                                     SUCInfo->PswdConsoleService,
                                     SUCInfo->HarddiskPassword[Index].HddDataInfo,
                                     MASTER_PSW,
                                     EncodePasswordPtr,
                                     EncodePasswordLength
                                     );

    }
    //
    // Disable User Password
    //
    if (SUCInfo->HarddiskPassword[Index].Flag == DISABLE_PASSWORD) {
      Status = Unicode2Ascii (
                 SUCInfo->HarddiskPassword[Index].DisableInputString,
                 SUCInfo->HarddiskPassword[Index].DisableStringLength,
                 &EncodePasswordPtrTmp,
                 &EncodePasswordLengthTmp
                 );

      Status = Encode (
                 EncodePasswordPtrTmp,
                 EncodePasswordLengthTmp,
                 &EncodePasswordPtr,
                 &EncodePasswordLength
                 );

      if (EFI_ERROR(Status)) {
        return Status;
      }

      SUCInfo->PswdConsoleService->DisableService (
                                     SUCInfo->PswdConsoleService,
                                     SUCInfo->HarddiskPassword[Index].HddDataInfo,
                                     USER_PSW,
                                     EncodePasswordPtr,
                                     EncodePasswordLength
                                     );
    }
    //
    // Disable Master Password
    //
    if (SUCInfo->HarddiskPassword[Index].MasterFlag == DISABLE_PASSWORD) {
      Status = Unicode2Ascii (
                 SUCInfo->HarddiskPassword[Index].DisableInputString,
                 SUCInfo->HarddiskPassword[Index].DisableStringLength,
                 &EncodePasswordPtrTmp,
                 &EncodePasswordLengthTmp
                 );

      Status = Encode (
                 EncodePasswordPtrTmp,
                 EncodePasswordLengthTmp,
                 &EncodePasswordPtr,
                 &EncodePasswordLength
                 );

      if (EFI_ERROR(Status)) {
        return Status;
      }

      Status = SUCInfo->PswdConsoleService->DisableService (
                                              SUCInfo->PswdConsoleService,
                                              SUCInfo->HarddiskPassword[Index].HddDataInfo,
                                              MASTER_PSW,
                                              EncodePasswordPtr,
                                              EncodePasswordLength
                                              );
      if (Status != EFI_SUCCESS) {
        Status = Unicode2Ascii (
                   SUCInfo->HarddiskPassword[Index].DisableAllInputString,
                   SUCInfo->HarddiskPassword[Index].DisableStringLength,
                   &EncodePasswordPtrTmp,
                   &EncodePasswordLengthTmp
                   );

        Status = Encode (
                   EncodePasswordPtrTmp,
                   EncodePasswordLengthTmp,
                   &EncodePasswordPtr,
                   &EncodePasswordLength
                   );

        if (EFI_ERROR(Status)) {
          return Status;
        }

        SUCInfo->PswdConsoleService->DisableService (
                                       SUCInfo->PswdConsoleService,
                                       SUCInfo->HarddiskPassword[Index].HddDataInfo,
                                       MASTER_PSW,
                                       EncodePasswordPtr,
                                       EncodePasswordLength
                                       );
      }
    }
  }
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
SetupUtilityLibGetSecurityData (
  IN  SECURITY_INFO_DATA *SecurityDataInfo,
  IN  UINT16             CallBackFormId,
  OUT BOOLEAN            *HaveSecurityData,
  OUT UINTN              *HarddiskInfoIndex
  )
/*++

Routine Description:

  According to Channel and device to get secruity data.

Arguments:

  SecurityDataInfo - Pointer security information data instance
  CallBackFormId   - Callback form ID number
  HaveSecurityData  - TRUE: Have security data
                      FALSE: Doesn't have security data.
  HarddiskInfoIndex - If have Security data, this value is the index in SECURITY_INFO_DATA array.

Returns:

  EFI_SUCCESS - Check have security data successful
  Other       - Cannot get SETUP_UTILITY_BROWSER_DATA instance

--*/
{
  EFI_STATUS                         Status;
  UINTN                              Index;
  UINTN                              NumOfHarddisk;
  SECURITY_INFO_DATA                 *HardDataInfo;
  SYSTEM_CONFIGURATION               *SetupVariable;
  SETUP_UTILITY_CONFIGURATION        *SCUInfo;
  SETUP_UTILITY_BROWSER_DATA         *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SCUInfo       = SuBrowser->SUCInfo;
  SetupVariable = SuBrowser->SCBuffer;
  NumOfHarddisk = SCUInfo->NumOfHarddiskPswd;
  HardDataInfo  = SCUInfo->HarddiskPassword;

  *HaveSecurityData = FALSE;

  if ((NumOfHarddisk == 0) || (HardDataInfo == NULL)) {
    return EFI_UNSUPPORTED;
  }

  for (Index = 0; Index < NumOfHarddisk; Index++) {
    //
    // Search match Harddisk info
    //
    if (HardDataInfo[Index].LabelIndex != CallBackFormId) {
      continue;
    }

    if (&HardDataInfo[Index] != SecurityDataInfo) {
      continue;
    }

    *HaveSecurityData = TRUE;
    *HarddiskInfoIndex = Index;
    break;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
UpdateHdPswLabel (
  IN UINT16              CallBackFormId,
  IN BOOLEAN             HaveSecurityData,
  IN UINTN               HarddiskInfoIndex
  )
/*++

Routine Description:

  To update current hard disk password sate to all labels of SATA devices.

Arguments:

  CallBackFormId    - Callback form ID number
  HaveSecurityData  - TRUE: Have security data
                      FALSE: Doesn't have security data.
  HarddiskInfoIndex - If have Security data, this value is the index in SECURITY_INFO_DATA array.

Returns:

  EFI_SUCCESS - Update the labels of all SATA devices successfully.
  Other       - Update labels of all SATA devices failed.

--*/
{
  EFI_STATUS                      Status;
  SETUP_UTILITY_BROWSER_DATA      *SuBrowser;
  SYSTEM_CONFIGURATION            *SetupVariable;
  SECURITY_INFO_DATA              *HardDataInfo;
  SETUP_UTILITY_CONFIGURATION     *SCUInfo;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SetupVariable = SuBrowser->SCBuffer;
  SCUInfo       = SuBrowser->SUCInfo;
  HardDataInfo  = SCUInfo->HarddiskPassword;

  SetupVariable->UseMasterPassword = 1;

  if (HaveSecurityData == FALSE) {
    if ((CallBackFormId == HDD_PASSWORD_CH_1_MASTER_CBM_LABEL)) {
      SetupVariable->C1MsCBMSetHddPasswordFlag = 0;
      SetupVariable->C1MsCBMUnlockHddPasswordFlag = 0;
      mSaveItemMapping[0][0] = (UINT16) HarddiskInfoIndex;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_2_MASTER_CBM_LABEL)) {
      SetupVariable->C2MsCBMSetHddPasswordFlag = 0;
      SetupVariable->C2MsCBMUnlockHddPasswordFlag = 0;
      mSaveItemMapping[2][0] = (UINT16) HarddiskInfoIndex;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_3_MASTER_CBM_LABEL)){
      SetupVariable->C3MsCBMSetHddPasswordFlag = 0;
      SetupVariable->C3MsCBMUnlockHddPasswordFlag = 0;
      mSaveItemMapping[4][0] = (UINT16) HarddiskInfoIndex;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_4_MASTER_CBM_LABEL)) {
      SetupVariable->C4MsCBMSetHddPasswordFlag = 0;
      SetupVariable->C4MsCBMUnlockHddPasswordFlag = 0;
      mSaveItemMapping[6][0] = (UINT16) HarddiskInfoIndex;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_1_SLAVE_CBM_LABEL)) {
      SetupVariable->C1SlCBMSetHddPasswordFlag = 0;
      SetupVariable->C1SlCBMUnlockHddPasswordFlag = 0;
      mSaveItemMapping[1][0] = (UINT16) HarddiskInfoIndex;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_2_SLAVE_CBM_LABEL)) {
      SetupVariable->C2SlCBMSetHddPasswordFlag = 0;
      SetupVariable->C2SlCBMUnlockHddPasswordFlag = 0;
      mSaveItemMapping[3][0] = (UINT16) HarddiskInfoIndex;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_3_SLAVE_CBM_LABEL)) {
      SetupVariable->C3SlCBMSetHddPasswordFlag = 0;
      SetupVariable->C3SlCBMUnlockHddPasswordFlag = 0;
      mSaveItemMapping[5][0] = (UINT16) HarddiskInfoIndex;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_4_SLAVE_CBM_LABEL)) {
      SetupVariable->C4SlCBMSetHddPasswordFlag = 0;
      SetupVariable->C4SlCBMUnlockHddPasswordFlag = 0;
      mSaveItemMapping[7][0] = (UINT16) HarddiskInfoIndex;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_1_MASTER_LABEL)) {
      SetupVariable->C1MsSetHddPasswordFlag = 0;
      SetupVariable->C1MsUnlockHddPasswordFlag = 0;
      mSaveItemMapping[0][0] = (UINT16) HarddiskInfoIndex;
    }

    if ((CallBackFormId == HDD_PASSWORD_CH_2_MASTER_LABEL)) {
      SetupVariable->C2MsSetHddPasswordFlag = 0;
      SetupVariable->C2MsUnlockHddPasswordFlag = 0;
      mSaveItemMapping[2][0] = (UINT16) HarddiskInfoIndex;
    }

    if ((CallBackFormId == HDD_PASSWORD_CH_3_MASTER_LABEL)) {
      SetupVariable->C3MsSetHddPasswordFlag = 0;
      SetupVariable->C3MsUnlockHddPasswordFlag = 0;
      mSaveItemMapping[4][0] = (UINT16) HarddiskInfoIndex;
    }

    if ((CallBackFormId == HDD_PASSWORD_CH_4_MASTER_LABEL)) {
      SetupVariable->C4MsSetHddPasswordFlag = 0;
      SetupVariable->C4MsUnlockHddPasswordFlag = 0;
      mSaveItemMapping[6][0] = (UINT16) HarddiskInfoIndex;
    }

    if ((CallBackFormId == HDD_PASSWORD_CH_1_SLAVE_LABEL)) {
      SetupVariable->C1SlSetHddPasswordFlag = 0;
      SetupVariable->C1SlUnlockHddPasswordFlag = 0;
      mSaveItemMapping[1][0] = (UINT16) HarddiskInfoIndex;
    }

    if ((CallBackFormId == HDD_PASSWORD_CH_2_SLAVE_LABEL)) {
      SetupVariable->C2SlSetHddPasswordFlag = 0;
      SetupVariable->C2SlUnlockHddPasswordFlag = 0;
      mSaveItemMapping[3][0] = (UINT16) HarddiskInfoIndex;
    }

    if ((CallBackFormId == HDD_PASSWORD_CH_3_SLAVE_LABEL)) {
      SetupVariable->C3SlSetHddPasswordFlag = 0;
      SetupVariable->C3SlUnlockHddPasswordFlag = 0;
      mSaveItemMapping[5][0] = (UINT16) HarddiskInfoIndex;
    }

    if ((CallBackFormId == HDD_PASSWORD_CH_4_SLAVE_LABEL)) {
      SetupVariable->C4SlSetHddPasswordFlag = 0;
      SetupVariable->C4SlUnlockHddPasswordFlag = 0;
      mSaveItemMapping[7][0] = (UINT16) HarddiskInfoIndex;
    }
  } else {
    //
    // Get string token
    //
    if (HardDataInfo[HarddiskInfoIndex].Flag == SECURITY_LOCK) {
      if ((CallBackFormId == HDD_PASSWORD_CH_1_MASTER_CBM_LABEL)) {
        SetupVariable->C1MsCBMUnlockHddPasswordFlag = 1;
        SetupVariable->C1MsCBMSetHddPasswordFlag = 0;
        mSaveItemMapping[0][0] = (UINT16) HarddiskInfoIndex;
        mSaveItemMapping[0][1] = CallBackFormId;
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_2_MASTER_CBM_LABEL)) {
        SetupVariable->C2MsCBMUnlockHddPasswordFlag = 1;
        SetupVariable->C2MsCBMSetHddPasswordFlag = 0;
        mSaveItemMapping[2][0] = (UINT16) HarddiskInfoIndex;
        mSaveItemMapping[2][1] = CallBackFormId;
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_3_MASTER_CBM_LABEL)) {
        SetupVariable->C3MsCBMUnlockHddPasswordFlag = 1;
        SetupVariable->C3MsCBMSetHddPasswordFlag = 0;
        mSaveItemMapping[4][0] = (UINT16) HarddiskInfoIndex;
        mSaveItemMapping[4][1] = CallBackFormId;
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_4_MASTER_CBM_LABEL)) {
        SetupVariable->C4MsCBMUnlockHddPasswordFlag = 1;
        SetupVariable->C4MsCBMSetHddPasswordFlag = 0;
        mSaveItemMapping[6][0] = (UINT16) HarddiskInfoIndex;
        mSaveItemMapping[6][1] = CallBackFormId;
        }
      if ((CallBackFormId == HDD_PASSWORD_CH_1_SLAVE_CBM_LABEL)){
        SetupVariable->C1SlCBMUnlockHddPasswordFlag = 1;
        SetupVariable->C1SlCBMSetHddPasswordFlag = 0;
        mSaveItemMapping[1][0] = (UINT16) HarddiskInfoIndex;
        mSaveItemMapping[1][1] = CallBackFormId;
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_2_SLAVE_CBM_LABEL)) {
        SetupVariable->C2SlCBMUnlockHddPasswordFlag = 1;
        SetupVariable->C2SlCBMSetHddPasswordFlag = 0;
        mSaveItemMapping[3][0] = (UINT16) HarddiskInfoIndex;
        mSaveItemMapping[3][1] = CallBackFormId;
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_3_SLAVE_CBM_LABEL)) {
        SetupVariable->C3SlCBMUnlockHddPasswordFlag = 1;
        SetupVariable->C3SlCBMSetHddPasswordFlag = 0;
        mSaveItemMapping[5][0] = (UINT16) HarddiskInfoIndex;
        mSaveItemMapping[5][1] = CallBackFormId;
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_4_SLAVE_CBM_LABEL)) {
        SetupVariable->C4SlCBMUnlockHddPasswordFlag = 1;
        SetupVariable->C4SlCBMSetHddPasswordFlag = 0;
        mSaveItemMapping[7][0] = (UINT16) HarddiskInfoIndex;
        mSaveItemMapping[7][1] = CallBackFormId;
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_1_MASTER_LABEL)) {
        SetupVariable->C1MsUnlockHddPasswordFlag = 1;
        SetupVariable->C1MsSetHddPasswordFlag = 0;
        mSaveItemMapping[0][0] = (UINT16) HarddiskInfoIndex;
        mSaveItemMapping[0][1] = CallBackFormId;
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_2_MASTER_LABEL)) {
        SetupVariable->C2MsUnlockHddPasswordFlag = 1;
        SetupVariable->C2MsSetHddPasswordFlag = 0;
        mSaveItemMapping[2][0] = (UINT16) HarddiskInfoIndex;
        mSaveItemMapping[2][1] = CallBackFormId;
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_3_MASTER_LABEL)) {
        SetupVariable->C3MsUnlockHddPasswordFlag = 1;
        SetupVariable->C3MsSetHddPasswordFlag = 0;
        mSaveItemMapping[4][0] = (UINT16) HarddiskInfoIndex;
        mSaveItemMapping[4][1] = CallBackFormId;
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_4_MASTER_LABEL)) {
        SetupVariable->C4MsUnlockHddPasswordFlag = 1;
        SetupVariable->C4MsSetHddPasswordFlag = 0;
        mSaveItemMapping[6][0] = (UINT16) HarddiskInfoIndex;
        mSaveItemMapping[6][1] = CallBackFormId;
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_1_SLAVE_LABEL)) {
        SetupVariable->C1SlUnlockHddPasswordFlag = 1;
        SetupVariable->C1SlSetHddPasswordFlag = 0;
        mSaveItemMapping[1][0] = (UINT16) HarddiskInfoIndex;
        mSaveItemMapping[1][1] = CallBackFormId;
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_2_SLAVE_LABEL)) {
        SetupVariable->C2SlUnlockHddPasswordFlag = 1;
        SetupVariable->C2SlSetHddPasswordFlag = 0;
        mSaveItemMapping[3][0] = (UINT16) HarddiskInfoIndex;
        mSaveItemMapping[3][1] = CallBackFormId;
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_3_SLAVE_LABEL)) {
        SetupVariable->C3SlUnlockHddPasswordFlag = 1;
        SetupVariable->C3SlSetHddPasswordFlag = 0;
        mSaveItemMapping[5][0] = (UINT16) HarddiskInfoIndex;
        mSaveItemMapping[5][1] = CallBackFormId;
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_4_SLAVE_LABEL)) {
        SetupVariable->C4SlUnlockHddPasswordFlag = 1;
        SetupVariable->C4SlSetHddPasswordFlag = 0;
        mSaveItemMapping[7][0] = (UINT16) HarddiskInfoIndex;
        mSaveItemMapping[7][1] = CallBackFormId;
      }
    } else {
      if ((CallBackFormId == HDD_PASSWORD_CH_1_MASTER_CBM_LABEL)) {
        SetupVariable->C1MsCBMSetHddPasswordFlag = 1;
        SetupVariable->C1MsCBMUnlockHddPasswordFlag = 0;
        mSaveItemMapping[0][0] = (UINT16) HarddiskInfoIndex;
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_2_MASTER_CBM_LABEL)) {
        SetupVariable->C2MsCBMSetHddPasswordFlag = 1;
        SetupVariable->C2MsCBMUnlockHddPasswordFlag = 0;
        mSaveItemMapping[2][0] = (UINT16) HarddiskInfoIndex;
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_3_MASTER_CBM_LABEL)){
        SetupVariable->C3MsCBMSetHddPasswordFlag = 1;
        SetupVariable->C3MsCBMUnlockHddPasswordFlag = 0;
        mSaveItemMapping[4][0] = (UINT16) HarddiskInfoIndex;
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_4_MASTER_CBM_LABEL)) {
        SetupVariable->C4MsCBMSetHddPasswordFlag = 1;
        SetupVariable->C4MsCBMUnlockHddPasswordFlag = 0;
        mSaveItemMapping[6][0] = (UINT16) HarddiskInfoIndex;
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_1_SLAVE_CBM_LABEL)) {
        SetupVariable->C1SlCBMSetHddPasswordFlag = 1;
        SetupVariable->C1SlCBMUnlockHddPasswordFlag = 0;
        mSaveItemMapping[1][0] = (UINT16) HarddiskInfoIndex;
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_2_SLAVE_CBM_LABEL)) {
        SetupVariable->C2SlCBMSetHddPasswordFlag = 1;
        SetupVariable->C2SlCBMUnlockHddPasswordFlag = 0;
        mSaveItemMapping[3][0] = (UINT16) HarddiskInfoIndex;
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_3_SLAVE_CBM_LABEL)) {
        SetupVariable->C3SlCBMSetHddPasswordFlag = 1;
        SetupVariable->C3SlCBMUnlockHddPasswordFlag = 0;
        mSaveItemMapping[5][0] = (UINT16) HarddiskInfoIndex;
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_4_SLAVE_CBM_LABEL)) {
        SetupVariable->C4SlCBMSetHddPasswordFlag = 1;
        SetupVariable->C4SlCBMUnlockHddPasswordFlag = 0;
        mSaveItemMapping[7][0] = (UINT16) HarddiskInfoIndex;
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_1_MASTER_LABEL)) {
        SetupVariable->C1MsSetHddPasswordFlag = 1;
        SetupVariable->C1MsUnlockHddPasswordFlag = 0;
        mSaveItemMapping[0][0] = (UINT16) HarddiskInfoIndex;
      }

      if ((CallBackFormId == HDD_PASSWORD_CH_2_MASTER_LABEL)) {
        SetupVariable->C2MsSetHddPasswordFlag = 1;
        SetupVariable->C2MsUnlockHddPasswordFlag = 0;
        mSaveItemMapping[2][0] = (UINT16) HarddiskInfoIndex;
      }

      if ((CallBackFormId == HDD_PASSWORD_CH_3_MASTER_LABEL)) {
        SetupVariable->C3MsSetHddPasswordFlag = 1;
        SetupVariable->C3MsUnlockHddPasswordFlag = 0;
        mSaveItemMapping[4][0] = (UINT16) HarddiskInfoIndex;
      }

      if ((CallBackFormId == HDD_PASSWORD_CH_4_MASTER_LABEL)) {
        SetupVariable->C4MsSetHddPasswordFlag = 1;
        SetupVariable->C4MsUnlockHddPasswordFlag = 0;
        mSaveItemMapping[6][0] = (UINT16) HarddiskInfoIndex;
      }

      if ((CallBackFormId == HDD_PASSWORD_CH_1_SLAVE_LABEL)) {
        SetupVariable->C1SlSetHddPasswordFlag = 1;
        SetupVariable->C1SlUnlockHddPasswordFlag = 0;
        mSaveItemMapping[1][0] = (UINT16) HarddiskInfoIndex;
      }

      if ((CallBackFormId == HDD_PASSWORD_CH_2_SLAVE_LABEL)) {
        SetupVariable->C2SlSetHddPasswordFlag = 1;
        SetupVariable->C2SlUnlockHddPasswordFlag = 0;
        mSaveItemMapping[3][0] = (UINT16) HarddiskInfoIndex;
      }

      if ((CallBackFormId == HDD_PASSWORD_CH_3_SLAVE_LABEL)) {
        SetupVariable->C3SlSetHddPasswordFlag = 1;
        SetupVariable->C3SlUnlockHddPasswordFlag = 0;
        mSaveItemMapping[5][0] = (UINT16) HarddiskInfoIndex;
      }

      if ((CallBackFormId == HDD_PASSWORD_CH_4_SLAVE_LABEL)) {
        SetupVariable->C4SlSetHddPasswordFlag = 1;
        SetupVariable->C4SlUnlockHddPasswordFlag = 0;
        mSaveItemMapping[7][0] = (UINT16) HarddiskInfoIndex;
      }
    }
  }
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
ScuLibSearchMatchedPortNum (
  IN     UINT32                              Bus,
  IN     UINT32                              Device,
  IN     UINT32                              Function,
  IN     UINT8                               PrimarySecondary,
  IN     UINT8                               SlaveMaster,
  OUT    UINTN                               *PortNum
  )
/*++

Routine Description:

  According Bus, Device, Function, PrimarySecondary, SlaveMaster to get corresponding
  SATA port number

Arguments:

  Bus -             - PCI bus number
  Device            - PCI device number
  Function          - PCI function number
  PrimarySecondary  - primary or scondary
  SlaveMaster       - slave or master
  PortNum           - An address to save SATA port number.

Returns:

  EFI_SUCCESS           - get corresponding port number successfully
  EFI_INVALID_PARAMETER - input parameter is invalid
  EFI_NOT_FOUND         - can't get corresponding port number

--*/
{
  UINTN                 Index;
  EFI_STATUS            Status;
  PORT_NUMBER_MAP       *PortMappingTable;
  UINTN                 NoPorts;
  EFI_CHIPSET_LIB_SERVICES_PROTOCOL *ChipsetLibServices;

  Status = gBS->LocateProtocol (
                        &gEfiChipsetLibServicesProtocolGuid,
                        NULL,
                        &ChipsetLibServices
                        );
  ASSERT_EFI_ERROR (Status);                        

  if (PortNum == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = ChipsetLibServices->GetPortNumberMapTable (&PortMappingTable, &NoPorts);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  for (Index = 0; Index < NoPorts; Index++) {
    if ((PortMappingTable[Index].Bus == Bus) &&
        (PortMappingTable[Index].Device == Device) &&
        (PortMappingTable[Index].Function == Function) &&
        (PortMappingTable[Index].PrimarySecondary == PrimarySecondary) &&
        (PortMappingTable[Index].SlaveMaster == SlaveMaster)) {
      *PortNum = PortMappingTable[Index].PortNum;
      return EFI_SUCCESS;

    }
  }
  return EFI_NOT_FOUND;
}
