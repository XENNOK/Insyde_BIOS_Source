/** @file
  Password relaitve code, including HDD password and System password

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

#include "Password.h"

#include <Protocol/SimpleTextInEx.h>
#include <Protocol/IdeControllerInit.h>
#include <Protocol/DiskInfo.h>

#include <Library/DxeOemSvcKernelLib.h>
#include <Library/OemGraphicsLib.h>
#include <Library/DxeChipsetSvcLib.h>

#include <PortNumberMap.h>

BOOLEAN                     mSetHddPassword       = FALSE;
BOOLEAN                     mHDDPswdBootIn        = FALSE;
BOOLEAN                     mUnLockHddPassword    = FALSE;
BOOLEAN                     mSetAllHddPassword    = FALSE;
BOOLEAN                     mDisableHddPassword = FALSE;
UINT16                      mSetAllCurrentPassword[HDD_PASSWORD_MAX_NUMBER + 1];
BOOLEAN                     mDisableAllHddFlag  = FALSE;
UINT16                      mSaveItemMapping[0xf][2];
PASSWORD_CONFIGURATION      mPasswordConfig;

STATIC
BOOLEAN
CheckHarddiskStatus (
  IN  HDD_PASSWORD_INFO_DATA     *PswHddInfoTablePtr,
  IN  UINTN                      NumOfHarddisk
  );

STATIC
EFI_STATUS
CheckSetupPassword (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL *PswdConsoleService,
  IN  SETUP_UTILITY_CONFIGURATION       *ScuInfo,
  IN  KERNEL_CONFIGURATION              *KernelConfig
  );



STATIC
EFI_STATUS
SetupUtilityLibGetSecurityData (
  IN  HDD_PASSWORD_INFO_DATA            *HddPasswordDataInfo,
  IN  UINT16                            CallBackFormId,
  OUT BOOLEAN                           *HaveSecurityData,
  OUT UINTN                             *HarddiskInfoIndex
  );

BOOLEAN
HaveSysPasswordCheck (
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL    *SysPasswordService  OPTIONAL
  );

EFI_STATUS
CleanSysPasswordCheck (
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL    *SysPasswordService  OPTIONAL
  );

CURRENT_USER_TYPE
GetSysPasswordCurrentUser (
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL    *SysPasswordService  OPTIONAL
  );

EFI_STATUS
SetSysPasswordCurrentUser (
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL    *SysPasswordService, OPTIONAL
  CURRENT_USER_TYPE       CurrentUserType
  );

/**
  This function converts Unicode string to ASCII string.

  @param  PasswordPtr            Password string address.
  @param  PasswordLength         Password string length.
  @param  EncodePasswordPtr      Encode password address.
  @param  EncodePasswordLength   Encode string length.

  @retval Start of the ASCII ouput string.

**/
STATIC
EFI_STATUS
Unicode2Ascii (
  IN     VOID   *PasswordPtr,
  IN     UINTN  PasswordLength,
  IN OUT VOID   **EncodePasswordPtr,
  IN OUT UINTN  *EncodePasswordLength
  )
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

/**
  If you want to Encode password string just edit at here.

  @param  PasswordPtr            Password string address.
  @param  PasswordLength         Password string length.
  @param  EncodePasswordPtr      Encode password address.
  @param  EncodePasswordLength   Encode string length.

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
Encode (
  IN     VOID   *PasswordPtr,
  IN     UINTN  PasswordLength,
  IN OUT VOID   **EncodePasswordPtr,
  IN OUT UINTN  *EncodePasswordLength
  )
{
  *EncodePasswordLength = PasswordLength;
  *EncodePasswordPtr    = PasswordPtr;

  return EFI_SUCCESS;
}

/**
  To update current MAster HDD password state to all labels of SATA devices.

  @param  MasterStatus           MASTER_PASSWORD_GRAYOUT: Grayout "Set Master Hdd Password"
                                 other: Doesn't grayout "Set Master Hdd Password"
  @param  CallBackFormId         Callback form ID number

  @retval EFI_SUCCESS            always return successfully.

**/
EFI_STATUS
UpdateMasterHddPasswordStatus (
  IN  UINTN                             MasterStatus, 
  IN  UINT16                            CallBackFormId
  )
{

  if (MasterStatus != MASTER_PASSWORD_GRAYOUT) {
    if ((CallBackFormId == HDD_PASSWORD_CH_1_MASTER_CBM_LABEL)) {
      mPasswordConfig.C1MsCBMMasterHddPasswordFlag = 1;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_2_MASTER_CBM_LABEL)) {
      mPasswordConfig.C2MsCBMMasterHddPasswordFlag = 1;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_3_MASTER_CBM_LABEL)){
      mPasswordConfig.C3MsCBMMasterHddPasswordFlag = 1;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_4_MASTER_CBM_LABEL)) {
      mPasswordConfig.C4MsCBMMasterHddPasswordFlag = 1;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_1_SLAVE_CBM_LABEL)) {
      mPasswordConfig.C1SlCBMMasterHddPasswordFlag = 1;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_2_SLAVE_CBM_LABEL)) {
      mPasswordConfig.C2SlCBMMasterHddPasswordFlag = 1;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_3_SLAVE_CBM_LABEL)) {
      mPasswordConfig.C3SlCBMMasterHddPasswordFlag = 1;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_4_SLAVE_CBM_LABEL)) {
      mPasswordConfig.C4SlCBMMasterHddPasswordFlag = 1;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_1_MASTER_LABEL)) {
      mPasswordConfig.C1MsMasterHddPasswordFlag = 1;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_2_MASTER_LABEL)) {
      mPasswordConfig.C2MsMasterHddPasswordFlag = 1;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_3_MASTER_LABEL)) {
      mPasswordConfig.C3MsMasterHddPasswordFlag = 1;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_4_MASTER_LABEL)) {
      mPasswordConfig.C4MsMasterHddPasswordFlag = 1;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_1_SLAVE_LABEL)) {
      mPasswordConfig.C1SlMasterHddPasswordFlag = 1;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_2_SLAVE_LABEL)) {
      mPasswordConfig.C2SlMasterHddPasswordFlag = 1;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_3_SLAVE_LABEL)) {
      mPasswordConfig.C3SlMasterHddPasswordFlag = 1;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_4_SLAVE_LABEL)) {
      mPasswordConfig.C4SlMasterHddPasswordFlag = 1;
    }
  } else {
    if ((CallBackFormId == HDD_PASSWORD_CH_1_MASTER_CBM_LABEL)) {
      mPasswordConfig.C1MsCBMMasterHddPasswordFlag = 0;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_2_MASTER_CBM_LABEL)) {
      mPasswordConfig.C2MsCBMMasterHddPasswordFlag = 0;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_3_MASTER_CBM_LABEL)){
      mPasswordConfig.C3MsCBMMasterHddPasswordFlag = 0;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_4_MASTER_CBM_LABEL)) {
      mPasswordConfig.C4MsCBMMasterHddPasswordFlag = 0;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_1_SLAVE_CBM_LABEL)) {
      mPasswordConfig.C1SlCBMMasterHddPasswordFlag = 0;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_2_SLAVE_CBM_LABEL)) {
      mPasswordConfig.C2SlCBMMasterHddPasswordFlag = 0;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_3_SLAVE_CBM_LABEL)) {
      mPasswordConfig.C3SlCBMMasterHddPasswordFlag = 0;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_4_SLAVE_CBM_LABEL)) {
      mPasswordConfig.C4SlCBMMasterHddPasswordFlag = 0;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_1_MASTER_LABEL)) {
      mPasswordConfig.C1MsMasterHddPasswordFlag = 0;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_2_MASTER_LABEL)) {
      mPasswordConfig.C2MsMasterHddPasswordFlag = 0;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_3_MASTER_LABEL)) {
      mPasswordConfig.C3MsMasterHddPasswordFlag = 0;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_4_MASTER_LABEL)) {
      mPasswordConfig.C4MsMasterHddPasswordFlag = 0;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_1_SLAVE_LABEL)) {
      mPasswordConfig.C1SlMasterHddPasswordFlag = 0;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_2_SLAVE_LABEL)) {
      mPasswordConfig.C2SlMasterHddPasswordFlag = 0;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_3_SLAVE_LABEL)) {
      mPasswordConfig.C3SlMasterHddPasswordFlag = 0;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_4_SLAVE_LABEL)) {
      mPasswordConfig.C4SlMasterHddPasswordFlag = 0;
    }
  }
  
  return EFI_SUCCESS;
}


/**
  Search match Label number of harddisk information

  @param  PswHddInfoTablePtr     The Simple password descriptor Information of harddisk.
  @param  CurrentMachLabel       Return match IDE-Harddisk Label
  @param  SecurityModeToken      Return string token of device status.

  @retval EFI_SUCCESS            Found match harddisk info and Label.
  @retval EFI_NOT_FOUND          Could not found match harddisk info.

**/
EFI_STATUS
SearchMatchLabelIndex (
  IN  HDD_SECURITY_INFO_DATA                *PswHddInfoTablePtr,
  OUT UINT16                                *CurrentMachLabel,
  OUT STRING_REF                            *SecurityModeToken
  )
{
  BOOLEAN                    CheckFoundFlag;
  UINT8                      Channel;
  UINT8                      Device;
  UINT16                     HddCount;
  UINT16                     BbsTotalCount;
  UINTN                      Index;
  UINTN                      PortNum;
  BBS_TABLE                  *LocalBbsTable;
  EFI_LEGACY_BIOS_PROTOCOL   *LegacyBios;
  EFI_STATUS                 Status;
  HDD_INFO                   *LocalHddInfo;
  SETUP_UTILITY_BROWSER_DATA *SuBrowser;
  IDE_CONFIG                 *IdeConfig;
  EFI_DISK_INFO_PROTOCOL     *DiskInfo;
  IDENTIFY_TAB               *IdentifyData;
  UINT32                     Size;
  BOOLEAN                    LegacyBiosSupport;

  LegacyBiosSupport          = FALSE;
  HddCount      = 0;
  BbsTotalCount = 0;
  LocalBbsTable = NULL;
  LegacyBios    = NULL;
  LocalHddInfo  = NULL;
  DiskInfo      = NULL;
  IdentifyData  = NULL;
  Size          = 0;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (
                  &gEfiLegacyBiosProtocolGuid,
                  NULL,
                  (VOID **) &LegacyBios
                  );
  if (!EFI_ERROR (Status)) {
    LegacyBiosSupport = TRUE;

    Status = LegacyBios->GetBbsInfo(LegacyBios, &HddCount, &LocalHddInfo, &BbsTotalCount, &LocalBbsTable);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  CheckFoundFlag = FALSE;
  IdeConfig = SuBrowser->IdeConfig;
  if (!LegacyBiosSupport) {
    Size   = sizeof(IDENTIFY_TAB);
    Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    Size,
                    (VOID **) &IdentifyData
                    );
  }

  for (Index = 0; Index <= 7; Index++) {
    //
    // Only need check exist device
    //
    if (IdeConfig[Index].IdeDevice != 1) {
      continue;
    }
    if (LegacyBiosSupport) {
      Channel = IdeConfig[Index].Channel;
      Device  = IdeConfig[Index].Device;
      IdentifyData = (IDENTIFY_TAB *) &LocalHddInfo[Channel].IdentifyDrive[Device];
    }

    if (PswHddInfoTablePtr->ControllerMode == ATA_IDE_MODE) {
      PortNum = PswHddInfoTablePtr->MappedPort;
    } else {
      PortNum = PswHddInfoTablePtr->PortNumber;
    }

    if (!LegacyBiosSupport) {
      Status = gBS->HandleProtocol (
                      IdeConfig[Index].DiskInfoHandle,
                      &gEfiDiskInfoProtocolGuid,
                      (VOID **) &DiskInfo
                      );
      if (EFI_ERROR (Status)) {
        continue;
      }


      Status = DiskInfo->Identify (
                           DiskInfo,
                           IdentifyData,
                           &Size
                           );
    }
    //
    // Check port number is match and is not ODD
    //
    if (PortNum == Index && (IdentifyData->config & ATAPI_DEVICE_BIT) == 0) {
      CheckFoundFlag = TRUE;
      break;
    }
  }

  if (!LegacyBiosSupport) {
    gBS->FreePool (IdentifyData);
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
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  HDD_PASSWORD_INFO_DATA            *HddPasswordDataInfo,
  IN  UINTN                             NumOfHddPswd
  )
{
  UINTN                                 Index;
  BOOLEAN                               HddPaswdLockFlag;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  EFI_STATUS                            Status;


  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (HddPasswordDataInfo == NULL) {
    ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->SetAllHddPasswordFlag = 0;
    return EFI_SUCCESS;
  }

  if (NumOfHddPswd == 0) {
    ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->SetAllHddPasswordFlag = 0;
    return EFI_SUCCESS;
  }

  HddPaswdLockFlag = FALSE;
  for (Index = 0; Index < NumOfHddPswd; Index++) {
    if (HddPasswordDataInfo[Index].Flag == SECURITY_LOCK) {
      HddPaswdLockFlag = TRUE;
    }
  }
  if (HddPaswdLockFlag == TRUE) {
    ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->SetAllHddPasswordFlag = 0;
  } else {
    ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->SetAllHddPasswordFlag = 1;
    ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->UseMasterPassword = 1;
    ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->SetAllMasterHddPasswordFlag = 1;
    for (Index = 0; Index < NumOfHddPswd; Index++) {
      if (HddPasswordDataInfo[Index].Flag == CHANGE_PASSWORD || 
          HddPasswordDataInfo[Index].Flag == ENABLE_PASSWORD) {
        ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->SetAllHddPasswordFlag = 0;
        break;
      }
      if (HddPasswordDataInfo[Index].MasterFlag == MASTER_PASSWORD_GRAYOUT) {
        //
        //  If ALL HDD Master Password is MASTER_PASSWORD_GRAYOUT,
        //  let Set All Master HDD Password is grayout.
        //

        ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->SetAllMasterHddPasswordFlag = 0;
        break;
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Update Harddisk password form information

  @param  HiiHandle              Specific HII handle.
  @param  HddPasswordDataInfo    Security Info table of Harddisk.
  @param  CurrentHddIndex        The harddisk index number.

  @retval EFI_SUCCESS            Update Harddisk password successful
  @retval Other                  Locate gEfiSetupUtilityBrowserProtocolGuid failed

**/
EFI_STATUS
UpdateHDDPswForm (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  HDD_PASSWORD_INFO_DATA            *HddPasswordDataInfo,
  IN  UINT16                            CurrentHddIndex
  )
{
  STRING_REF                            Token;
  CHAR16                                *NewString;
  UINT16                                LabelIndex;
  STRING_REF                            SecurityModeToken;
  UINT16                                LabelIndex1;
  UINT16                                LabelIndex2;
  UINTN                                 Index;
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

  Status = SearchMatchLabelIndex (
             HddPasswordDataInfo->HddDataInfo,
             &LabelIndex,
             &SecurityModeToken
             );

  //
  // Update Information
  //
  if (HddPasswordDataInfo->Flag == SECURITY_LOCK) {
    //
    // Harddisk Security Enable [Unlock]
    //
    Token = STRING_TOKEN (STR_HDD_PSW_LOCK);
  } else if (HddPasswordDataInfo->Flag == CHANGE_PASSWORD) {
    //
    // Password Change
    //
    Token = STRING_TOKEN (STR_HDD_PSW_CHANGE);
  } else if ((HddPasswordDataInfo->Flag != DISABLE_PASSWORD &&
              HddPasswordDataInfo->Flag != NO_ACCESS_PASSWORD) &&
              HddPasswordDataInfo->MasterFlag == CHANGE_PASSWORD) {
    //
    // Harddisk Security Disable
    //
    Token = STRING_TOKEN (STR_HDD_PSW_CHANGE);
  } else if (HddPasswordDataInfo->Flag == DISABLE_PASSWORD ||
             HddPasswordDataInfo->MasterFlag == DISABLE_PASSWORD ) {
    //
    // Harddisk Security Disable
    //
    Token = STRING_TOKEN (STR_HDD_PSW_DISABLE);
  } else if (HddPasswordDataInfo->Flag == ENABLE_PASSWORD) {
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
    NewString = HiiGetString (
                  SuBrowser->SUCInfo->MapTable[SecurityHiiHandle].HiiHandle,
                  Token,
                  NULL
                  );
    HiiSetString (
      SuBrowser->SUCInfo->MapTable[AdvanceHiiHandle].HiiHandle,
      SecurityModeToken,
      NewString,
      NULL
      );
    gBS->FreePool (NewString);
    SuBrowser->SUCInfo->DoRefresh = TRUE;
  }

  if (LabelIndex < HDD_PASSWORD_CH_1_MASTER_CBM_LABEL) {
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
    HddPasswordDataInfo,
    LabelIndex1,
    &HaveSecurityData,
    &HarddiskInfoIndex
    );
  if (HaveSecurityData == TRUE) {
    UpdateHdPswLabel (LabelIndex1, HaveSecurityData, HarddiskInfoIndex);
  }
  SetupUtilityLibGetSecurityData (
    HddPasswordDataInfo,
    LabelIndex2,
    &HaveSecurityData,
    &HarddiskInfoIndex
    );
  if (HaveSecurityData == TRUE) {
    UpdateHdPswLabel (LabelIndex2, HaveSecurityData, HarddiskInfoIndex);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
ValidateHddPassword (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL *PswdConsoleService,
  IN  HDD_PASSWORD_INFO_DATA            *HddPasswordDataInfo,
  IN  UINT16                            CurrentHddIndex,
  IN  BOOLEAN                           MasterOrUser,
  IN  CHAR16                            *Password,
  IN  UINTN                             NumOfHddPswd
  )
{
  EFI_STATUS                         Status;
  KERNEL_CONFIGURATION               *MyIfrNVData;
  UINT16                             CurrentHddIndexTmp;
  SETUP_UTILITY_BROWSER_DATA         *SuBrowser;
  VOID                               *EncodePasswordPtr;
  VOID                               *EncodePasswordPtrTmp;
  UINTN                              EncodePasswordLength;
  UINTN                              EncodePasswordLengthTmp;
  SEC_HDD_PASSWORD_STRUCT            *HddPasswordTable;
  UINTN                              DataSize;
  UINT16                             CallBackFormId;

  if (PswdConsoleService == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CurrentHddIndexTmp = CurrentHddIndex;
  CurrentHddIndex = mSaveItemMapping[CurrentHddIndex][0];
  MyIfrNVData = (KERNEL_CONFIGURATION *) SuBrowser->SCBuffer;
  HddPasswordTable = NULL;
  DataSize = 0;

  if (HddPasswordDataInfo[CurrentHddIndex].Flag == ENABLE_PASSWORD) {
    mSetHddPassword = TRUE;
    mHDDPswdBootIn  = TRUE;
  }
  if (HddPasswordDataInfo[CurrentHddIndex].Flag == DISABLE_PASSWORD) {
    //
    //  return EFI_SUCCESS to display two password items for new password and confirm new password
    //
    return EFI_SUCCESS;
  }
  if (HddPasswordDataInfo[CurrentHddIndex].Flag == NO_ACCESS_PASSWORD) {
    //
    //  return EFI_SUCCESS to display two password items for new password and confirm new password
    //
    return EFI_SUCCESS;
  }

  if (Password[0] == 0x00 && HddPasswordDataInfo[CurrentHddIndex].Flag == SECURITY_LOCK) {
    //
    // If First time enter here, HDD is LOCK and not yet ask user input User password
    //
    
    mSetHddPassword = TRUE;
    mHDDPswdBootIn  = TRUE;
    //
    //  return EFI_CRC_ERROR to display one password item for unlock HDD first by corrent password
    //
    return EFI_CRC_ERROR;
  }

  if (Password[0] == 0) {
    return EFI_NOT_READY;
  }
  if (mSetHddPassword == TRUE && HddPasswordDataInfo[CurrentHddIndex].Flag != SECURITY_LOCK) {
    //
    //  To check previous password for change password or disable password
    //

    if (HddPasswordDataInfo[CurrentHddIndex].Flag != ENABLE_PASSWORD) {
      //
      //  User has changed new password, but the new password is not set to HDD.
      //  The new password is stored in buffer until "Save and Exit" to set to HDD by SetSecurityStatus()
      //
      if (StrCmp (mSetAllCurrentPassword, Password) == 0) {
        //
        //  user input password is match current set password
        //  return EFI_SUCCESS to display another two password items for new password and confirm new password
        //
        return EFI_SUCCESS;
      } else if (MasterOrUser == USER_PSW &&
                 (StrCmp (HddPasswordDataInfo[CurrentHddIndex].InputString, Password) == 0)) {
        //
        //  input password is match current set User password
        //  return EFI_SUCCESS to display another two password items for new password and confirm new password
        //
        return EFI_SUCCESS;
      } else if (MasterOrUser == MASTER_PSW &&
                 (StrCmp (HddPasswordDataInfo[CurrentHddIndex].MasterInputString, Password) == 0)) {
        //
        //  input password is match current set Master password
        //  return EFI_SUCCESS to display another two password items for new password and confirm new password
        //
        return EFI_SUCCESS;
      } else if (HddPasswordDataInfo[CurrentHddIndex].InputString[0] != 0 && 
                 HddPasswordDataInfo[CurrentHddIndex].MasterInputString[0] != 0) {
        //
        //  input password is  NOT  match current set password
        //  return EFI_NOT_READY to clean dialog
        //
        return EFI_NOT_READY;
      }
    }

    if (HddPasswordDataInfo[CurrentHddIndex].DisableAllType == MasterOrUser && 
        StrCmp (HddPasswordDataInfo[CurrentHddIndex].DisableInputString, Password) == 0) {
      //
      //  Check previos HDD password stored in variable.
      //

      mUnLockHddPassword = TRUE;

      UpdateHDDPswForm (
        HiiHandle,
        &HddPasswordDataInfo[CurrentHddIndex],
        CurrentHddIndex
      );

      return EFI_SUCCESS;
    } else {
      //
      //  Directly checking  password which is stored in HDD by using Unlock command.
      //
      Status = Unicode2Ascii (
                 Password,
                 StrLen (Password) * sizeof (UINT16),
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
        return EFI_NOT_READY;
      }

      Status = PswdConsoleService->UnlockHddPassword (
                                     PswdConsoleService,
                                     HddPasswordDataInfo[CurrentHddIndex].HddDataInfo,
                                     MasterOrUser,
                                     EncodePasswordPtr,
                                     EncodePasswordLength
                                     );
      if (EFI_ERROR (Status)) {
        //
        //  user input password is  NOT  match the password in HDD
        //  return EFI_NOT_READY to clean dialog
        //
        return EFI_NOT_READY;
      }

      mUnLockHddPassword = TRUE;

      UpdateHDDPswForm (
        HiiHandle,
        &HddPasswordDataInfo[CurrentHddIndex],
        CurrentHddIndex
      );

      return EFI_SUCCESS;
    }
  }

  //
  //  Unlock HDD in SCU
  //
  Status = Unicode2Ascii (
             Password,
             StrLen (Password) * sizeof (UINT16),
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
                                 HddPasswordDataInfo[CurrentHddIndex].HddDataInfo,
                                 MasterOrUser,
                                 EncodePasswordPtr,
                                 EncodePasswordLength
                                 );
  if (EFI_ERROR (Status)) {
    //
    //  Unlock HDD in SCU fail
    //  return EFI_NOT_READY to clean dialog
    //
    return EFI_NOT_READY;
  }

  ZeroMem (
    HddPasswordDataInfo[CurrentHddIndex].DisableInputString,
    (HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (UINT16)
    );
  StrCpy (
    HddPasswordDataInfo[CurrentHddIndex].DisableInputString,
    Password
    );
  HddPasswordDataInfo[CurrentHddIndex].DisableStringLength = (HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (UINT16);
  HddPasswordDataInfo[CurrentHddIndex].DisableAllType = MasterOrUser;

  if (MasterOrUser != MASTER_PSW) {
    //
	//  If input HDD User Password,
	//  let Set HDD Master Password grayout
	//
	
    HddPasswordDataInfo[CurrentHddIndex].MasterFlag = MASTER_PASSWORD_GRAYOUT;
  }
  CallBackFormId = mSaveItemMapping[CurrentHddIndexTmp][1];
  Status = UpdateMasterHddPasswordStatus (
             HddPasswordDataInfo[CurrentHddIndex].MasterFlag,
             CallBackFormId
             );

  if ((MasterOrUser == MASTER_PSW) && (HddPasswordDataInfo[CurrentHddIndex].Flag != SECURITY_LOCK)) {
    HddPasswordDataInfo[CurrentHddIndex].MasterFlag = ENABLE_PASSWORD;
    return EFI_SUCCESS;
  }
  mUnLockHddPassword = TRUE;
  HddPasswordDataInfo[CurrentHddIndex].Flag = ENABLE_PASSWORD;
  if (!CheckHarddiskStatus (HddPasswordDataInfo, NumOfHddPswd)) {
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
    HiiHandle,
    &HddPasswordDataInfo[CurrentHddIndex],
    CurrentHddIndex
    );
  UpdateHddPasswordLabel (
    HiiHandle,
    HddPasswordDataInfo,
    HddPasswordDataInfo->HddDataInfo->TotalNumOfHddInfo
    );

  return EFI_UNSUPPORTED;

}

EFI_STATUS
SetHddsPassword (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  HDD_PASSWORD_INFO_DATA            *HddPasswordDataInfo,
  IN  UINT16                            CurrentHddIndex,
  IN  BOOLEAN                           MasterOrUser,
  IN  CHAR16                            *Password
  )
{
  EFI_STATUS                         Status;
  UINT16                             CurrentHddIndexTmp;
  UINT16                             CallBackFormId;

  CurrentHddIndexTmp = CurrentHddIndex;
  CurrentHddIndex = mSaveItemMapping[CurrentHddIndex][0];
  if (Password[0] != 0x00) {
    //
    // Password String is not NULL => Change New Password
    //

    if (MasterOrUser == USER_PSW) {
      //
      // Set flag relative to User password
      //

      HddPasswordDataInfo[CurrentHddIndex].Flag = CHANGE_PASSWORD;
      if (HddPasswordDataInfo[CurrentHddIndex].DisableAllType != MASTER_PSW) {
        HddPasswordDataInfo[CurrentHddIndex].MasterFlag = MASTER_PASSWORD_GRAYOUT;
      }
      mSetHddPassword = TRUE;
      mDisableAllHddFlag = FALSE;
      CallBackFormId = mSaveItemMapping[CurrentHddIndexTmp][1];
      Status = UpdateMasterHddPasswordStatus (
                 HddPasswordDataInfo[CurrentHddIndex].MasterFlag,
                 CallBackFormId
                 );
    } else {
      //
      // Set flag relative to Master password
      //

      if (HddPasswordDataInfo[CurrentHddIndex].Flag != DISABLE_PASSWORD &&
          HddPasswordDataInfo[CurrentHddIndex].Flag != NO_ACCESS_PASSWORD) {
        HddPasswordDataInfo[CurrentHddIndex].Flag = CHANGE_PASSWORD;
      }
      HddPasswordDataInfo[CurrentHddIndex].MasterFlag = CHANGE_PASSWORD;
      mDisableAllHddFlag = FALSE;
      CallBackFormId = mSaveItemMapping[CurrentHddIndexTmp][1];
      Status = UpdateMasterHddPasswordStatus (
                 HddPasswordDataInfo[CurrentHddIndex].MasterFlag,
                 CallBackFormId
                 );
    }
    HddPasswordDataInfo[CurrentHddIndex].StringLength = (HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (UINT16);
    ZeroMem (
      mSetAllCurrentPassword,
      ((HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (UINT16))
      );
    if (MasterOrUser == USER_PSW) {
      //
      // Store password string to User password buffer
      //

      ZeroMem (
        HddPasswordDataInfo[CurrentHddIndex].InputString,
        HddPasswordDataInfo[CurrentHddIndex].StringLength
        );

      StrCpy (
        HddPasswordDataInfo[CurrentHddIndex].InputString,
        Password
        );
    } else {
      //
      // Store password string to Master password buffer
      //

      ZeroMem (
        HddPasswordDataInfo[CurrentHddIndex].MasterInputString,
        HddPasswordDataInfo[CurrentHddIndex].StringLength
        );

      StrCpy (
        HddPasswordDataInfo[CurrentHddIndex].MasterInputString,
        Password
        );
    }
  } else {
    //
    // Password String is NULL => Disable password
    //

    HddPasswordDataInfo[CurrentHddIndex].Flag = DISABLE_PASSWORD;
    HddPasswordDataInfo[CurrentHddIndex].MasterFlag = DISABLE_PASSWORD;
    HddPasswordDataInfo[CurrentHddIndex].DisableAllType = NO_ACCESS_PSW;
    CallBackFormId = mSaveItemMapping[CurrentHddIndexTmp][1];
    Status = UpdateMasterHddPasswordStatus (
               HddPasswordDataInfo[CurrentHddIndex].MasterFlag,
               CallBackFormId
               );

  }

  UpdateHDDPswForm (
     HiiHandle,
     &HddPasswordDataInfo[CurrentHddIndex],
     CurrentHddIndex
     );

  UpdateHddPasswordLabel (
    HiiHandle,
    HddPasswordDataInfo,
    HddPasswordDataInfo->HddDataInfo->TotalNumOfHddInfo
    );

  return EFI_SUCCESS;
}

/**
  The current Harddisk security call back.

  @param  FormCallback           The form call back protocol.
  @param  HiiHandle              Hii protocol.
  @param  Type                   Return string token of device status.
  @param  Value
  @param  ActionRequest
  @param  PState                 Password access is success or not, if access success then return TRUE.
  @param  CurrentHddIndex        The harddisk index number.
  @param  PswdConsoleService     HDD Password protocol service.
  @param  HddPasswordDataInfo    Pointer to HDD password buffer
  @param  MasterOrUser           HDD password or master HDD password
  @param  NumOfHddPswd           Number of Harddisk

  @retval EFI_SUCCESS            Password callback success.
  @retval EFI_ALREADY_STARTED    Already password in SetupUtility.
  @retval EFI_NOT_READY          Password confirm error.

**/
EFI_STATUS
HddPasswordCallback (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *FormCallback,
  IN  EFI_HII_HANDLE                      HiiHandle,
  IN  UINT8                               Type,
  IN  EFI_IFR_TYPE_VALUE                  *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest,
  OUT BOOLEAN                             *PState,
  IN  UINT16                              CurrentHddIndex,
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL   *PswdConsoleService,
  IN  HDD_PASSWORD_INFO_DATA              *HddPasswordDataInfo,
  IN  BOOLEAN                             MasterOrUser,
  IN  UINTN                               NumOfHddPswd
  )
{
  STATIC UINTN             PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
  EFI_STATUS               Status;
  CHAR16                   *Password;

  *PState = FALSE;
  if (Type != EFI_IFR_TYPE_STRING) {
    return  EFI_INVALID_PARAMETER;
  }

  if (Value->string == 0) {
    //
    // If Value->String == 0, only need reset the state machine to BROWSER_STATE_VALIDATE_PASSWORD
    //
    PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
    return EFI_SUCCESS;
  }

  Password = NULL;
  Password = HiiGetString (HiiHandle, Value->string, NULL);
  //
  // When try to set a new password, user will be chanlleged with old password.
  // The Callback is responsible for validating old password input by user,
  // If Callback return EFI_SUCCESS, it indicates validation pass.
  //
  switch (PasswordState) {

  case BROWSER_STATE_VALIDATE_PASSWORD:
    Status = ValidateHddPassword (
               HiiHandle,
               PswdConsoleService,
               HddPasswordDataInfo,
               CurrentHddIndex,
               MasterOrUser,
               Password,
               NumOfHddPswd
               );
    if (Status == EFI_SUCCESS) {
      PasswordState = BROWSER_STATE_SET_PASSWORD;
    }
    break;

  case BROWSER_STATE_SET_PASSWORD:
    Status = SetHddsPassword (
               HiiHandle,
               HddPasswordDataInfo,
               CurrentHddIndex,
               MasterOrUser,
               Password
               );
    *PState = TRUE;
    PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
    break;

  default:
    Status = EFI_NOT_AVAILABLE_YET;
    break;
  }

  if (Password != NULL) {
    gBS->FreePool (Password);
  }

  return Status;
}

/**
  Set disable password information to SecurityDataInfo and update string of Security mode.

  @param  FormCallback           The form call back protocol.
  @param  HiiHandle              Specific HII handle.
  @param  HddPasswordDataInfo    Security Info table of Harddisk.
  @param  NumOfHddPswd           Number of Harddisk

  @retval EFI_SUCCESS            Always return EFI_SUCCESS.

**/
EFI_STATUS
DisableAllHarddiskPswd (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL        *FormCallback,
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  HDD_PASSWORD_INFO_DATA            *HddPasswordDataInfo,
  IN  UINTN                             NumOfHddPswd
  )
{
  UINTN Index;

  for (Index = 0; Index < NumOfHddPswd; Index++) {
    HddPasswordDataInfo[Index].Flag = DISABLE_PASSWORD;
    UpdateHDDPswForm (
       HiiHandle,
       &HddPasswordDataInfo[Index],
       (UINT16)Index
       );
  }

  UpdateHddPasswordLabel (
    HiiHandle,
    HddPasswordDataInfo,
    NumOfHddPswd
    );

  return EFI_SUCCESS;
}

EFI_STATUS
ValidateAllHddsPassword (
  IN  BOOLEAN                           MasterOrUser,
  IN  HDD_PASSWORD_INFO_DATA            *HddPasswordDataInfo,
  IN  UINTN                             NumOfHarddisk
  )
{
  EFI_STATUS                Status;
  SEC_HDD_PASSWORD_STRUCT   *HddPasswordTable;
  UINTN                     DataSize;
  UINTN                     Index;

  HddPasswordTable = NULL;
  DataSize = 0;

  Status = gRT->GetVariable (
                  SAVE_HDD_PASSWORD_VARIABLE_NAME,
                  &gSaveHddPasswordGuid,
                  NULL,
                  &DataSize,
                  (VOID *) HddPasswordTable
                  );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Status = gBS->AllocatePool (
                    EfiRuntimeServicesData,
                    DataSize,
                    (VOID **) &HddPasswordTable
                    );

    Status = gRT->GetVariable (
                    SAVE_HDD_PASSWORD_VARIABLE_NAME,
                    &gSaveHddPasswordGuid,
                    NULL,
                    &DataSize,
                    (VOID *) HddPasswordTable
                    );
  } else if (Status == EFI_NOT_FOUND || HddPasswordTable == NULL) {
    return EFI_SUCCESS;
  }

  for (Index = 0; Index < NumOfHarddisk; Index++) {
    ZeroMem (
      HddPasswordDataInfo[Index].DisableAllInputString,
      (HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (UINT16)
      );

    StrCpy (
      HddPasswordDataInfo[Index].DisableAllInputString,
      HddPasswordTable[Index].PasswordStr
      );

    HddPasswordDataInfo[Index].DisableStringLength = (HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (UINT16);
    HddPasswordDataInfo[Index].DisableAllType = HddPasswordTable[Index].PasswordType;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
SetAllHddsPassword (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  HDD_PASSWORD_INFO_DATA            *HddPasswordDataInfo,
  IN  BOOLEAN                           MasterOrUser,
  IN  UINTN                             NumOfHddPswd,
  IN  CHAR16                            *Password
  )
{
  KERNEL_CONFIGURATION               *MyIfrNVData;
  UINTN                              Index;
  UINTN                              SetSecurityFlag;
  SETUP_UTILITY_BROWSER_DATA         *SuBrowser;
  EFI_STATUS                         Status;
  UINTN                              MappingIndex;  
  UINT16                             CallBackFormId;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  MyIfrNVData = (KERNEL_CONFIGURATION *) SuBrowser->SCBuffer;
  SetSecurityFlag = DISABLE_PASSWORD;
  MyIfrNVData->SetAllMasterHddPasswordFlag = FALSE;
  if ((Password[0] == 0)) {
    SetSecurityFlag = DISABLE_PASSWORD;
    mDisableAllHddFlag = TRUE;
    MyIfrNVData->SetAllMasterHddPasswordFlag = FALSE;
  } else {
    mSetHddPassword = TRUE;
    mSetAllHddPassword = TRUE;
    SetSecurityFlag = CHANGE_PASSWORD;
    mDisableAllHddFlag = FALSE;
  }

  if (MasterOrUser == USER_PSW) {
    for (Index = 0; Index < NumOfHddPswd; Index++) {
      HddPasswordDataInfo[Index].Flag = SetSecurityFlag;
      if ((Password[0] == 0)) {
        HddPasswordDataInfo[Index].MasterFlag = SetSecurityFlag;
      } else {
        MyIfrNVData->SetAllMasterHddPasswordFlag = FALSE;
        HddPasswordDataInfo[Index].MasterFlag = MASTER_PASSWORD_GRAYOUT;
      }
      HddPasswordDataInfo[Index].StringLength = (HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (UINT16);

      ZeroMem (
        HddPasswordDataInfo[Index].InputString,
        HddPasswordDataInfo[Index].StringLength
        );
 
      StrCpy (
        HddPasswordDataInfo[Index].InputString,
        Password
        );

      CallBackFormId = 0;
      for (MappingIndex = 0; MappingIndex < 0xf; MappingIndex++) {
        if (Index == mSaveItemMapping[MappingIndex][0] &&
            mSaveItemMapping[MappingIndex][1] != 0) {
          CallBackFormId = mSaveItemMapping[MappingIndex][1];
          break;
        }
      }

      Status = UpdateMasterHddPasswordStatus (
                 HddPasswordDataInfo[Index].MasterFlag,
                 CallBackFormId
                 );
 
      UpdateHDDPswForm (
        HiiHandle,
        &HddPasswordDataInfo[Index],
        (UINT16)Index
        );
    }
  } else {
    for (Index = 0; Index < NumOfHddPswd; Index++) {
       HddPasswordDataInfo[Index].MasterFlag = SetSecurityFlag;
       if ((Password[0] == 0)) {
         HddPasswordDataInfo[Index].Flag = SetSecurityFlag;
       }
       HddPasswordDataInfo[Index].StringLength = (HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (UINT16);

       ZeroMem (
	     HddPasswordDataInfo[Index].MasterInputString,
         HddPasswordDataInfo[Index].StringLength
         );

       StrCpy (
	     HddPasswordDataInfo[Index].MasterInputString,
         Password
         );

      CallBackFormId = 0;
      for (MappingIndex = 0; MappingIndex < 0xf; MappingIndex++) {
        if (Index == mSaveItemMapping[MappingIndex][0] &&
            mSaveItemMapping[MappingIndex][1] != 0) {
          CallBackFormId = mSaveItemMapping[MappingIndex][1];
          break;
        }
      }

      Status = UpdateMasterHddPasswordStatus (
                 HddPasswordDataInfo[Index].MasterFlag,
                 CallBackFormId
                 );

       UpdateHDDPswForm (
         HiiHandle,
         &HddPasswordDataInfo[Index],
         (UINT16)Index
         );
    }
  }

  UpdateHddPasswordLabel (
    HiiHandle,
    HddPasswordDataInfo,
    NumOfHddPswd
    );

  return EFI_SUCCESS;
}

/**
  Set password to all harddisk

  @param  FormCallback           The form call back protocol.
  @param  MasterOrUser           HDD password or master HDD password.
  @param  HiiHandle              Return string token of device status.
  @param  Type                   Return string token of device status.
  @param  Value
  @param  ActionRequest
  @param  PState                 Password access is success or not, if access success then return TRUE.
  @param  HddPasswordDataInfo    Pointer to the HDD password buffer
  @param  NumOfHddPswd           Number of Harddisk

  @retval EFI_SUCCESS            Password callback success.
  @retval EFI_ALREADY_STARTED    Already password in SetupUtility.
  @retval EFI_NOT_READY          Password confirm error.

**/
EFI_STATUS
SetAllHarddiskPswd (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL    *FormCallback,
  IN  BOOLEAN                                 MasterOrUser,
  IN  EFI_HII_HANDLE                          HiiHandle,
  IN  UINT8                                   Type,
  IN  EFI_IFR_TYPE_VALUE                      *Value,
  OUT EFI_BROWSER_ACTION_REQUEST              *ActionRequest,
  OUT BOOLEAN                                 *PState,
  IN  HDD_PASSWORD_INFO_DATA                  *HddPasswordDataInfo,
  IN  UINTN                                   NumOfHddPswd
  )
{

  STATIC UINTN             PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
  EFI_STATUS               Status;
  CHAR16                   *Password;

  *PState = FALSE;
  if (Type != EFI_IFR_TYPE_STRING) {
    return  EFI_INVALID_PARAMETER;
  }

  if (Value->string == 0) {
    //
    // If Value->String == 0, only need reset the state machine to BROWSER_STATE_VALIDATE_PASSWORD
    //
    PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
    return EFI_SUCCESS;
  }


  Password = HiiGetString (HiiHandle, Value->string, NULL);
  //
  // When try to set a new password, user will be chanlleged with old password.
  // The Callback is responsible for validating old password input by user,
  // If Callback return EFI_SUCCESS, it indicates validation pass.
  //
  switch (PasswordState) {

  case BROWSER_STATE_VALIDATE_PASSWORD:
    Status = ValidateAllHddsPassword (
               MasterOrUser,
               HddPasswordDataInfo,
               NumOfHddPswd
               );
    if (Status == EFI_SUCCESS) {
      PasswordState = BROWSER_STATE_SET_PASSWORD;
    }
    break;

  case BROWSER_STATE_SET_PASSWORD:
    Status = SetAllHddsPassword (
               HiiHandle,
               HddPasswordDataInfo,
               MasterOrUser,
               NumOfHddPswd,
               Password

               );
    *PState = TRUE;
    PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
    break;

  default:
    Status = EFI_NOT_AVAILABLE_YET;
    break;
  }

  if (Password != NULL) {
    gBS->FreePool (Password);
  }

  return Status;
}

/**
  Check system pasword status.

  @param  ScuInfo                Point to SETUP_UTILITY_CONFIGURATION instance.
  @param  KernelConfig           The kernel configuration data.

  @retval EFI_SUCCESS            Check system password success.
  @retval EFI_NOT_FOUND          Not found system password.

**/
EFI_STATUS
PasswordCheck (
  IN  SETUP_UTILITY_CONFIGURATION       *ScuInfo,
  IN  KERNEL_CONFIGURATION              *KernelConfig
  )
{
  EFI_STATUS                            Status;
  EFI_HII_HANDLE                        HiiHandle;
  CHAR16                                *NewString;
  UINTN                                 NumOfSupervisorPswdInfo;
  UINTN                                 NumOfUserPswdInfo;
  SYS_PASSWORD_INFO_DATA                *SysPasswordInfoData;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  EFI_BOOT_MODE                         BootMode;


  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  HiiHandle               = ScuInfo->MapTable[SecurityHiiHandle].HiiHandle;
  NumOfSupervisorPswdInfo = 0;
  NumOfUserPswdInfo       = 0;

  if (ScuInfo->SysPasswordService == NULL) {
    return EFI_UNSUPPORTED;
  }
  //
  // Get Hob list
  //
  BootMode = GetBootModeHob ();
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }
  if (BootMode == BOOT_ON_S4_RESUME) {
    return EFI_SUCCESS;
  }

  SysPasswordInfoData = NULL;
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
                  sizeof (SYS_PASSWORD_INFO_DATA),
                  (VOID **) &SysPasswordInfoData
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  SysPasswordInfoData->Flag           = NO_ACCESS_PASSWORD;
  SysPasswordInfoData->StringLength   = 0x00;
  SysPasswordInfoData->NumOfEntry     = NumOfSupervisorPswdInfo;

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  ((SysPasswordMaxNumber (ScuInfo->SysPasswordService) + 1) * sizeof (CHAR16)),
                  (VOID **)&(SysPasswordInfoData->InputString)
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

   ZeroMem (
     SysPasswordInfoData->InputString,
     ((SysPasswordMaxNumber (ScuInfo->SysPasswordService) + 1) * sizeof (CHAR16))
     );

  ScuInfo->SupervisorPassword = SysPasswordInfoData;

  //
  // Get User password
  //
  if (PcdGetBool (PcdSysPasswordSupportUserPswd)) {
    Status = ScuInfo->SysPasswordService->GetStatus (
                                            ScuInfo->SysPasswordService,
                                            SystemUser
                                            );
    SysPasswordInfoData = NULL;

    if (!EFI_ERROR (Status)) {
      NumOfUserPswdInfo = 1;
    }


    Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    sizeof (SYS_PASSWORD_INFO_DATA),
                    (VOID **)&SysPasswordInfoData
                    );

    if (EFI_ERROR (Status)) {
      gBS->FreePool (ScuInfo->SupervisorPassword);
      return Status;
    }

    SysPasswordInfoData->Flag           = NO_ACCESS_PASSWORD;
    SysPasswordInfoData->StringLength   = 0x00;
    SysPasswordInfoData->NumOfEntry     = NumOfUserPswdInfo;

    Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    ((SysPasswordMaxNumber (ScuInfo->SysPasswordService) + 1) * sizeof (CHAR16)),
                    (VOID **)&(SysPasswordInfoData->InputString)
                    );

    if (EFI_ERROR (Status)) {
      return Status;
    }

     ZeroMem (
       SysPasswordInfoData->InputString,
       ((SysPasswordMaxNumber (ScuInfo->SysPasswordService) + 1) * sizeof (CHAR16))
       );

    ScuInfo->UserPassword = SysPasswordInfoData;

    if ((NumOfUserPswdInfo != 0) || (NumOfSupervisorPswdInfo != 0)) {
      //
      // OemServices
      //
      Status = OemSvcPasswordEntryCallBack ();
    }
  } else {
    if (NumOfSupervisorPswdInfo != 0) {
      //
      // OemServices
      //
      Status = OemSvcPasswordEntryCallBack ();
    }
  }

  if (ScuInfo->SupervisorPassword->NumOfEntry == 0) {
    //
    // No set Supervisor password
    //
    NewString = HiiGetString (ScuInfo->MapTable[SecurityHiiHandle].HiiHandle, STRING_TOKEN (STR_NOT_INSTALLED_TEXT), NULL);
    UpdateSupervisorTags (
      HiiHandle,
      FALSE,
      NewString,
      KernelConfig
      );
    ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->SupervisorFlag = 0;
    ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->UserFlag = 0;
    SuBrowser->SUCInfo->SupervisorPwdFlag = FALSE;
    SuBrowser->SUCInfo->UserPwdFlag       = FALSE;
    gBS->FreePool (NewString);
    Status = ScuInfo->SysPasswordService->UnlockPassword (ScuInfo->SysPasswordService, NULL, 0);
    return EFI_SUCCESS;
  }

  if (HaveSysPasswordCheck (ScuInfo->SysPasswordService)) {
    DisableQuietBoot ();
    gST->ConOut->EnableCursor (gST->ConOut, TRUE);
    Status = CheckSetupPassword (
               ScuInfo->MapTable[SecurityHiiHandle].HiiHandle,
               ScuInfo->SysPasswordService,
               ScuInfo,
               KernelConfig
               );
    CleanSysPasswordCheck (ScuInfo->SysPasswordService);
  } else {
    if (GetSysPasswordCurrentUser (ScuInfo->SysPasswordService) == SystemSupervisorType) {
      KernelConfig->SetUserPass = FALSE;
    } else {
      KernelConfig->SetUserPass = TRUE;
    }
  }

  NewString = HiiGetString (ScuInfo->MapTable[SecurityHiiHandle].HiiHandle, STRING_TOKEN (STR_INSTALLED_TEXT), NULL);
  UpdateSupervisorTags (
    HiiHandle,
    TRUE,
    NewString,
    KernelConfig
    );
  ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->SupervisorFlag = 1;
  SuBrowser->SUCInfo->SupervisorPwdFlag = TRUE;

  if (PcdGetBool (PcdSysPasswordSupportUserPswd)) {
    if (ScuInfo->UserPassword->NumOfEntry == 0) {
      ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->UserFlag = 0;
      SuBrowser->SUCInfo->UserPwdFlag = FALSE;
    } else {
      ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->UserFlag = 1;
      SuBrowser->SUCInfo->UserPwdFlag = TRUE;
    }

    if (ScuInfo->UserPassword->NumOfEntry != 0) {
      UpdateUserTags (
        HiiHandle,
        TRUE,
        NewString,
        KernelConfig
        );
    }
  }
  gBS->FreePool (NewString);

  return EFI_SUCCESS;
}

/**
  Update supervisor tag.

  @param  HiiHandle              Return string token of device status.
  @param  Installed              TRUE installed supervisor password.
  @param  NewString              String buffer.
  @param  KernelConfig           The kernel configuration data.

  @retval EFI_SUCCESS            Always return.

**/
EFI_STATUS
UpdateSupervisorTags (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  BOOLEAN                           Installed,
  IN  CHAR16                            *NewString,
  IN  KERNEL_CONFIGURATION              *KernelConfig
  )
{
  STRING_REF                            TokenToUpdate;

  if (Installed) {
    KernelConfig->SupervisorFlag = 1;
  } else {
    KernelConfig->SupervisorFlag = 0;
    if (PcdGetBool (PcdSysPasswordSupportUserPswd)) {
      UpdateUserTags (HiiHandle, FALSE, NewString, KernelConfig);
    }
  }

  TokenToUpdate = STRING_TOKEN (STR_SUPERVISOR_PASSWORD_STRING2);
  HiiSetString (HiiHandle, TokenToUpdate, NewString, NULL);

  return EFI_SUCCESS;
}

/**
  Update user password tag.

  @param  HiiHandle              Return string token of device status.
  @param  Installed              TRUE installed supervisor password.
  @param  NewString              String buffer.
  @param  KernelConfig           The kernel configuration data.

  @retval EFI_SUCCESS            Always return.

**/
EFI_STATUS
UpdateUserTags (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  BOOLEAN                           Installed,
  IN  CHAR16                            *NewString,
  IN  KERNEL_CONFIGURATION              *KernelConfig
  )
{
  STRING_REF                            TokenToUpdate;

  if (Installed) {
    KernelConfig->UserFlag = TRUE;
  } else {
    KernelConfig->UserFlag = FALSE;
  }

  TokenToUpdate = STRING_TOKEN (STR_USER_PASSWORD_STRING2);
  HiiSetString (HiiHandle, TokenToUpdate, NewString, NULL);

  return EFI_SUCCESS;
}

/**
  Check system pasword status.

  @param  HiiHandle              Return string token of device status.
  @param  PswdConsoleService     Password console protocol service.
  @param  ScuInfo
  @param  SetupVariable          The Setup NV data.

  @retval EFI_SUCCESS            Check system password success.
  @retval EFI_NOT_FOUND          Not found system password.

**/
STATIC
EFI_STATUS
CheckSetupPassword (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL *PswdConsoleService,
  IN  SETUP_UTILITY_CONFIGURATION       *ScuInfo,
  IN  KERNEL_CONFIGURATION              *KernelConfig
  )
{
  EFI_STATUS                            Status;
  EFI_INPUT_KEY                         Key;
  CHAR16                                *PasswordString;
  CHAR16                                *StringPtr;
  CHAR16                                *ErrorStatusMsg;
  CHAR16                                *ThreeIncorrectMsg;
  CHAR16                                *RebootMsg;
  UINTN                                 TryPasswordTimes;
  BOOLEAN                               DrawFlag;
  BOOLEAN                               ReadChar;
  UINT16                                Password;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  CHAR16                                *SysInvalidPassword;
  CURRENT_USER_TYPE                     CurrentUserType;

  if (PswdConsoleService == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (ScuInfo->SupervisorPassword->NumOfEntry == 0) {
    return EFI_SUCCESS;
  }

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }


  Password = 0;
  DrawFlag = FALSE;
  ReadChar = TRUE;
  TryPasswordTimes = (UINTN)PcdGet8 (PcdMaxCheckPasswordCount);
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
				  ((SysPasswordMaxNumber (PswdConsoleService) + 1) * sizeof (CHAR16)),
                  (VOID **)&StringPtr
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // OemServices
  //
  Status = OemSvcGetMaxCheckPasswordCount (
             &TryPasswordTimes
             );

  PasswordString = HiiGetString (HiiHandle, STRING_TOKEN (STR_CHECK_PASSWORD_STRING), NULL);

  SysInvalidPassword = HiiGetString (HiiHandle, STRING_TOKEN (STR_SYS_DIALOG_INVALID_PASSWORD), NULL);
  //
  // Clear screen before print Harddisk passwork prompt message
  //
  gST->ConOut->ClearScreen (gST->ConOut);
  do {
    ZeroMem (StringPtr, (SysPasswordMaxNumber (PswdConsoleService) + 1) * sizeof (CHAR16));
    SuBrowser->H2ODialog->PasswordDialog (
                              0,
                              FALSE,
                              (UINT32) (SysPasswordMaxNumber (PswdConsoleService) + 1),
                              StringPtr,
                              &Key,
                              PasswordString
                              );

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
                                     (SysPasswordMaxNumber (PswdConsoleService) + 1) * sizeof(CHAR16),
                                     SystemSupervisor
                                     );

      if (Status == EFI_SUCCESS) {
        KernelConfig->SetUserPass = FALSE;
        CurrentUserType = SystemSupervisorType;
        gBS->FreePool (PasswordString);
        gBS->FreePool (SysInvalidPassword);
        Status = PswdConsoleService->UnlockPassword (
                                       PswdConsoleService,
                                       StringPtr,
                                       (SysPasswordMaxNumber (PswdConsoleService) + 1) * sizeof(CHAR16)
                                       );

        SetSysPasswordCurrentUser (
          PswdConsoleService,
          CurrentUserType
          );
        gBS->FreePool (StringPtr);
        return EFI_SUCCESS;
      }
    }
    if (PcdGetBool (PcdSysPasswordSupportUserPswd)) {
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
                                         (SysPasswordMaxNumber (PswdConsoleService) + 1) * sizeof(CHAR16),
                                         SystemUser
                                         );

          if (Status == EFI_SUCCESS) {
            KernelConfig->SetUserPass = TRUE;
            CurrentUserType = SystemUserType;
            gBS->FreePool (PasswordString);
            gBS->FreePool (SysInvalidPassword);
            SetSysPasswordCurrentUser (
              PswdConsoleService,
              CurrentUserType
              );
            gBS->FreePool (StringPtr);
            return EFI_SUCCESS;
          }
        }
      }
    }

    TryPasswordTimes--;
    ReadChar = TRUE;
    if (TryPasswordTimes > 0) {
      SuBrowser->H2ODialog->ConfirmDialog (
                                2,
                                FALSE,
                                40,
                                NULL,
                                &Key,
                                SysInvalidPassword
                                );
    }
  } while (TryPasswordTimes > 0);

  //
  // OemServices
  //
  Status = OemSvcCheckPasswordFailCallBack ();

  if (Status == EFI_UNSUPPORTED) {
    ErrorStatusMsg    = HiiGetString (HiiHandle, STRING_TOKEN (STR_SYS_DIALOG_ERROR_STATUS), NULL);
    ThreeIncorrectMsg = HiiGetString (HiiHandle, STRING_TOKEN (STR_SYS_DIALOG_THREE_ERRORS), NULL);
    RebootMsg         = HiiGetString (HiiHandle, STRING_TOKEN (STR_SYS_DIALOG_RESTART_MSG), NULL);
    SuBrowser->H2ODialog->CreateMsgPopUp (
                              40,
                              3,
                              ErrorStatusMsg,
                              ThreeIncorrectMsg,
                              RebootMsg
                              );

    CpuDeadLoop ();
  }

  gBS->FreePool (PasswordString);
  gBS->FreePool (StringPtr);

  return EFI_SECURITY_VIOLATION;
}

/**
  Check Harddisk security ststus.

  @param  PswHddInfoTablePtr     The Harddisk information array.
  @param  NumOfHarddisk          Number of simple password protocol of harddisk.

  @retval TRUE                   Harddisk security status is locked
  @retval EFI_NOT_FOUND          It could not found any harddisk security status is locked.

**/
BOOLEAN
CheckHarddiskStatus (
  IN  HDD_PASSWORD_INFO_DATA     *PswHddInfoTablePtr,
  IN  UINTN                      NumOfHarddisk
  )
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

/**
  Check hard disk password status.

  @param  HiiHandle              Specific HII handle for Security menu.
  @param  BootOrSCU              TRUE: In to SCU
                                 FALSE: want to boot
  @param  KernelConfig           Point to KERNEL_CONFIGURATION instance.
  @param  SavePasswordPoint      Point to security harddisk password instance

  @retval EFI_SUCCESS            Check hard disk password success.
  @retval EFI_NOT_FOUND          Not found system password.

**/
EFI_STATUS
HddPasswordCheck (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  BOOLEAN                           BootOrSCU,
  IN  KERNEL_CONFIGURATION              *KernelConfig,
  OUT SEC_HDD_PASSWORD_STRUCT           **SavePasswordPoint
  )
{
  EFI_STATUS                            Status;
  UINTN                                 Index;
  HDD_PASSWORD_INFO_DATA                *HddPasswordInfoArray;
  EFI_HDD_PASSWORD_SERVICE_PROTOCOL     *HddPswdServiceProtocol;
  HDD_SECURITY_INFO_DATA                *HarddiskStr;
  UINTN                                 NumOfHDDInfo;
  EFI_BOOT_MODE                         BootMode;
  UINT8                                 IndexTmp;
  CHAR16                                HardDiskPassword[HDD_PASSWORD_MAX_NUMBER + 1];
  UINTN                                 PswdLength;
  SEC_HDD_PASSWORD_STRUCT               *HddPasswordTable;
  BOOLEAN                               UnlockPasswordType;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;

  HddPswdServiceProtocol  = NULL;
  HddPasswordTable        = NULL;
  HarddiskStr             = NULL;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }


  SUCInfo = SuBrowser->SUCInfo;
  HddPasswordInfoArray = SUCInfo->HarddiskPassword;


  BootMode = GetBootModeHob ();
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  Status = gBS->LocateProtocol (
                  &gEfiHddPasswordServiceProtocolGuid,
                  NULL,
                  (VOID **) &HddPswdServiceProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  HddPswdServiceProtocol->GetHddSecurityInfo (
                            HddPswdServiceProtocol,
                            (VOID *)&HarddiskStr
                            );
  NumOfHDDInfo = HarddiskStr->TotalNumOfHddInfo;

  if (BootOrSCU != INTO_SCU) {
    HddPasswordTable = CommonGetVariableData (
                         SAVE_HDD_PASSWORD_VARIABLE_NAME,
                         &gSaveHddPasswordGuid
                         );

    if (HddPasswordTable == NULL) {
      return EFI_SUCCESS;
    }
  } else {
    for (Index = 0; Index < NumOfHDDInfo; Index++) {
      if ((HarddiskStr[Index].HddSecurityStatus & HDD_SECURITY_FROZEN) == HDD_SECURITY_FROZEN) {
        HddPswdServiceProtocol->SetFeaturesCmd (
                                  HddPswdServiceProtocol,
                                  &HarddiskStr[Index],
                                  0x90,
                                  0x06
                                  );
      if (HarddiskStr[Index].ControllerMode == ATA_IDE_MODE) {
        Status = DxeCsSvcSataComReset (HarddiskStr[Index].MappedPort);
      } else {
        Status = DxeCsSvcSataComReset (HarddiskStr[Index].PortNumber);
      }
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

  //
  //Auto Unlcok Hdd in S4 resume
  //
  if (BootMode == BOOT_ON_S4_RESUME) {
    if (HddPasswordTable == NULL) {
      return EFI_NOT_FOUND;
    }
    for (Index = 0; Index < NumOfHDDInfo; Index++) {
      if ((HarddiskStr[Index].HddSecurityStatus & HDD_SECURITY_LOCK) != HDD_SECURITY_LOCK) {
        continue;
      }
      if (HarddiskStr[Index].ControllerMode == ATA_IDE_MODE) {
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
            CopyMem (HardDiskPassword, HddPasswordTable[IndexTmp].PasswordStr, ((HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (UINT16)));
            UnlockPasswordType = USER_PSW;
          } else {
            CopyMem (HardDiskPassword, HddPasswordTable[IndexTmp].PasswordStr, ((HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (UINT16)));
            UnlockPasswordType = MASTER_PSW;
          }

          PswdLength = StrLen (HardDiskPassword);


          HddPswdServiceProtocol->UnlockHddPassword (
                                    HddPswdServiceProtocol,
                                    &HarddiskStr[IndexTmp],
                                    UnlockPasswordType,
                                    (UINT8 *)HardDiskPassword,
                                    (PswdLength * sizeof (UINT16))
                                    );

          //
          //To clean the locked flags if unlock Hdd success from the EBDA offset 0x3d9
          //
          if (HarddiskStr[Index].ControllerMode == ATA_IDE_MODE) {
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

  if (!CheckHarddiskStatus (HddPasswordInfoArray, NumOfHDDInfo)) {
    KernelConfig->SetAllMasterHddPasswordFlag = FALSE;
    return EFI_SUCCESS;
  }

  return EFI_SUCCESS;
}

/**
  Set user set password status. This founction could set password status
  when press 'F10' and set 'yes'.

  @retval EFI_SUCCESS            Set system password and harddisk password success.

**/
EFI_STATUS
SetSecurityStatus (
  VOID
  )
{
  UINTN                               Index;
  SETUP_UTILITY_CONFIGURATION         *SUCInfo;
  EFI_STATUS                          Status;
  SETUP_UTILITY_BROWSER_DATA          *SuBrowser;
  VOID                                *EncodePasswordPtr;
  VOID                                *EncodePasswordPtrTmp;
  UINTN                               EncodePasswordLength;
  UINTN                               EncodePasswordLengthTmp;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo = SuBrowser->SUCInfo;

  if (SUCInfo->SysPasswordService == NULL) {
    return EFI_NOT_READY;
  }

  if (PcdGetBool (PcdSysPasswordSupportUserPswd)) {
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
  }

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
    if ((SUCInfo->HarddiskPassword[Index].Flag != CHANGE_PASSWORD) &&
        (SUCInfo->HarddiskPassword[Index].Flag != DISABLE_PASSWORD) &&
        (SUCInfo->HarddiskPassword[Index].MasterFlag != CHANGE_PASSWORD) &&
        (SUCInfo->HarddiskPassword[Index].MasterFlag != DISABLE_PASSWORD)) {
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

      SUCInfo->PswdConsoleService->DisableService (
                                     SUCInfo->PswdConsoleService,
                                     SUCInfo->HarddiskPassword[Index].HddDataInfo,
                                     SUCInfo->HarddiskPassword[Index].DisableAllType,
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

/**
  According to Channel and device to get secruity data.

  @param  HddPasswordDataInfo    Pointer security information data instance
  @param  CallBackFormId         Callback form ID number
  @param  HaveSecurityData       TRUE: Have security data
                                 FALSE: Doesn't have security data.
  @param  HarddiskInfoIndex      If have Security data, this value is the index in SECURITY_INFO_DATA array.

  @retval EFI_SUCCESS            Check have security data successful
  @retval Other                  Cannot get SETUP_UTILITY_BROWSER_DATA instance

**/
STATIC
EFI_STATUS
SetupUtilityLibGetSecurityData (
  IN  HDD_PASSWORD_INFO_DATA *HddPasswordDataInfo,
  IN  UINT16                 CallBackFormId,
  OUT BOOLEAN                *HaveSecurityData,
  OUT UINTN                  *HarddiskInfoIndex
  )
{
  EFI_STATUS                         Status;
  UINTN                              Index;
  UINTN                              NumOfHarddisk;
  HDD_PASSWORD_INFO_DATA             *HddDataInfo;
  SETUP_UTILITY_CONFIGURATION        *SCUInfo;
  SETUP_UTILITY_BROWSER_DATA         *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SCUInfo       = SuBrowser->SUCInfo;
  NumOfHarddisk = SCUInfo->NumOfHarddiskPswd;
  HddDataInfo  = SCUInfo->HarddiskPassword;

  *HaveSecurityData = FALSE;

  if ((NumOfHarddisk == 0) || (HddDataInfo == NULL)) {
    return EFI_UNSUPPORTED;
  }

  for (Index = 0; Index < NumOfHarddisk; Index++) {
    //
    // Search match Harddisk info
    //

    if (HddDataInfo[Index].LabelIndex != CallBackFormId) {
      continue;
    }

    if (&HddDataInfo[Index] != HddPasswordDataInfo) {
      continue;
    }

    *HaveSecurityData = TRUE;
    *HarddiskInfoIndex = Index;
    break;
  }

  return EFI_SUCCESS;
}

/**
  To update current hard disk password sate to all labels of SATA devices.

  @param  CallBackFormId         Callback form ID number
  @param  HaveSecurityData       TRUE: Have security data
                                 FALSE: Doesn't have security data.
  @param  HarddiskInfoIndex      If have Security data, this value is the index in SECURITY_INFO_DATA array.

  @retval EFI_SUCCESS            Update the labels of all SATA devices successfully.
  @retval Other                  Update labels of all SATA devices failed.

**/
EFI_STATUS
UpdateHdPswLabel (
  IN UINT16              CallBackFormId,
  IN BOOLEAN             HaveSecurityData,
  IN UINTN               HarddiskInfoIndex
  )
 {
  EFI_STATUS                      Status;
  SETUP_UTILITY_BROWSER_DATA      *SuBrowser;
  KERNEL_CONFIGURATION            *KernelConfig;
  HDD_PASSWORD_INFO_DATA          *HddDataInfo;
  SETUP_UTILITY_CONFIGURATION     *SCUInfo;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  KernelConfig  = (KERNEL_CONFIGURATION *) SuBrowser->SCBuffer;
  SCUInfo       = SuBrowser->SUCInfo;
  HddDataInfo   = SCUInfo->HarddiskPassword;

  KernelConfig->UseMasterPassword = 1;

  if (HaveSecurityData == FALSE) {
    if ((CallBackFormId == HDD_PASSWORD_CH_1_MASTER_CBM_LABEL)) {
      KernelConfig->C1MsCBMSetHddPasswordFlag = 0;
      KernelConfig->C1MsCBMUnlockHddPasswordFlag = 0;
      mSaveItemMapping[0][0] = (UINT16) HarddiskInfoIndex;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_2_MASTER_CBM_LABEL)) {
      KernelConfig->C2MsCBMSetHddPasswordFlag = 0;
      KernelConfig->C2MsCBMUnlockHddPasswordFlag = 0;
      mSaveItemMapping[2][0] = (UINT16) HarddiskInfoIndex;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_3_MASTER_CBM_LABEL)){
      KernelConfig->C3MsCBMSetHddPasswordFlag = 0;
      KernelConfig->C3MsCBMUnlockHddPasswordFlag = 0;
      mSaveItemMapping[4][0] = (UINT16) HarddiskInfoIndex;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_4_MASTER_CBM_LABEL)) {
      KernelConfig->C4MsCBMSetHddPasswordFlag = 0;
      KernelConfig->C4MsCBMUnlockHddPasswordFlag = 0;
      mSaveItemMapping[6][0] = (UINT16) HarddiskInfoIndex;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_1_SLAVE_CBM_LABEL)) {
      KernelConfig->C1SlCBMSetHddPasswordFlag = 0;
      KernelConfig->C1SlCBMUnlockHddPasswordFlag = 0;
      mSaveItemMapping[1][0] = (UINT16) HarddiskInfoIndex;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_2_SLAVE_CBM_LABEL)) {
      KernelConfig->C2SlCBMSetHddPasswordFlag = 0;
      KernelConfig->C2SlCBMUnlockHddPasswordFlag = 0;
      mSaveItemMapping[3][0] = (UINT16) HarddiskInfoIndex;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_3_SLAVE_CBM_LABEL)) {
      KernelConfig->C3SlCBMSetHddPasswordFlag = 0;
      KernelConfig->C3SlCBMUnlockHddPasswordFlag = 0;
      mSaveItemMapping[5][0] = (UINT16) HarddiskInfoIndex;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_4_SLAVE_CBM_LABEL)) {
      KernelConfig->C4SlCBMSetHddPasswordFlag = 0;
      KernelConfig->C4SlCBMUnlockHddPasswordFlag = 0;
      mSaveItemMapping[7][0] = (UINT16) HarddiskInfoIndex;
    }
    if ((CallBackFormId == HDD_PASSWORD_CH_1_MASTER_LABEL)) {
      KernelConfig->C1MsSetHddPasswordFlag = 0;
      KernelConfig->C1MsUnlockHddPasswordFlag = 0;
      mSaveItemMapping[0][0] = (UINT16) HarddiskInfoIndex;
    }

    if ((CallBackFormId == HDD_PASSWORD_CH_2_MASTER_LABEL)) {
      KernelConfig->C2MsSetHddPasswordFlag = 0;
      KernelConfig->C2MsUnlockHddPasswordFlag = 0;
      mSaveItemMapping[2][0] = (UINT16) HarddiskInfoIndex;
    }

    if ((CallBackFormId == HDD_PASSWORD_CH_3_MASTER_LABEL)) {
      KernelConfig->C3MsSetHddPasswordFlag = 0;
      KernelConfig->C3MsUnlockHddPasswordFlag = 0;
      mSaveItemMapping[4][0] = (UINT16) HarddiskInfoIndex;
    }

    if ((CallBackFormId == HDD_PASSWORD_CH_4_MASTER_LABEL)) {
      KernelConfig->C4MsSetHddPasswordFlag = 0;
      KernelConfig->C4MsUnlockHddPasswordFlag = 0;
      mSaveItemMapping[6][0] = (UINT16) HarddiskInfoIndex;
    }

    if ((CallBackFormId == HDD_PASSWORD_CH_1_SLAVE_LABEL)) {
      KernelConfig->C1SlSetHddPasswordFlag = 0;
      KernelConfig->C1SlUnlockHddPasswordFlag = 0;
      mSaveItemMapping[1][0] = (UINT16) HarddiskInfoIndex;
    }

    if ((CallBackFormId == HDD_PASSWORD_CH_2_SLAVE_LABEL)) {
      KernelConfig->C2SlSetHddPasswordFlag = 0;
      KernelConfig->C2SlUnlockHddPasswordFlag = 0;
      mSaveItemMapping[3][0] = (UINT16) HarddiskInfoIndex;
    }

    if ((CallBackFormId == HDD_PASSWORD_CH_3_SLAVE_LABEL)) {
      KernelConfig->C3SlSetHddPasswordFlag = 0;
      KernelConfig->C3SlUnlockHddPasswordFlag = 0;
      mSaveItemMapping[5][0] = (UINT16) HarddiskInfoIndex;
    }

    if ((CallBackFormId == HDD_PASSWORD_CH_4_SLAVE_LABEL)) {
      KernelConfig->C4SlSetHddPasswordFlag = 0;
      KernelConfig->C4SlUnlockHddPasswordFlag = 0;
      mSaveItemMapping[7][0] = (UINT16) HarddiskInfoIndex;
    }
  } else {
    //
    // Get string token
    //
    if (HddDataInfo[HarddiskInfoIndex].Flag == SECURITY_LOCK) {
      if ((CallBackFormId == HDD_PASSWORD_CH_1_MASTER_CBM_LABEL)) {
        KernelConfig->C1MsCBMUnlockHddPasswordFlag = 1;
        KernelConfig->C1MsCBMSetHddPasswordFlag = 0;
        mSaveItemMapping[0][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[0][1], CallBackFormId);
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_2_MASTER_CBM_LABEL)) {
        KernelConfig->C2MsCBMUnlockHddPasswordFlag = 1;
        KernelConfig->C2MsCBMSetHddPasswordFlag = 0;
        mSaveItemMapping[2][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[2][1], CallBackFormId);
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_3_MASTER_CBM_LABEL)) {
        KernelConfig->C3MsCBMUnlockHddPasswordFlag = 1;
        KernelConfig->C3MsCBMSetHddPasswordFlag = 0;
        mSaveItemMapping[4][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[4][1], CallBackFormId);
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_4_MASTER_CBM_LABEL)) {
        KernelConfig->C4MsCBMUnlockHddPasswordFlag = 1;
        KernelConfig->C4MsCBMSetHddPasswordFlag = 0;
        mSaveItemMapping[6][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[6][1], CallBackFormId);
        }
      if ((CallBackFormId == HDD_PASSWORD_CH_1_SLAVE_CBM_LABEL)){
        KernelConfig->C1SlCBMUnlockHddPasswordFlag = 1;
        KernelConfig->C1SlCBMSetHddPasswordFlag = 0;
        mSaveItemMapping[1][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[1][1], CallBackFormId);
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_2_SLAVE_CBM_LABEL)) {
        KernelConfig->C2SlCBMUnlockHddPasswordFlag = 1;
        KernelConfig->C2SlCBMSetHddPasswordFlag = 0;
        mSaveItemMapping[3][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[3][1], CallBackFormId);
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_3_SLAVE_CBM_LABEL)) {
        KernelConfig->C3SlCBMUnlockHddPasswordFlag = 1;
        KernelConfig->C3SlCBMSetHddPasswordFlag = 0;
        mSaveItemMapping[5][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[5][1], CallBackFormId);
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_4_SLAVE_CBM_LABEL)) {
        KernelConfig->C4SlCBMUnlockHddPasswordFlag = 1;
        KernelConfig->C4SlCBMSetHddPasswordFlag = 0;
        mSaveItemMapping[7][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[7][1], CallBackFormId);
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_1_MASTER_LABEL)) {
        KernelConfig->C1MsUnlockHddPasswordFlag = 1;
        KernelConfig->C1MsSetHddPasswordFlag = 0;
        mSaveItemMapping[0][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[0][1], CallBackFormId);
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_2_MASTER_LABEL)) {
        KernelConfig->C2MsUnlockHddPasswordFlag = 1;
        KernelConfig->C2MsSetHddPasswordFlag = 0;
        mSaveItemMapping[2][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[2][1], CallBackFormId);
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_3_MASTER_LABEL)) {
        KernelConfig->C3MsUnlockHddPasswordFlag = 1;
        KernelConfig->C3MsSetHddPasswordFlag = 0;
        mSaveItemMapping[4][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[4][1], CallBackFormId);
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_4_MASTER_LABEL)) {
        KernelConfig->C4MsUnlockHddPasswordFlag = 1;
        KernelConfig->C4MsSetHddPasswordFlag = 0;
        mSaveItemMapping[6][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[6][1], CallBackFormId);
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_1_SLAVE_LABEL)) {
        KernelConfig->C1SlUnlockHddPasswordFlag = 1;
        KernelConfig->C1SlSetHddPasswordFlag = 0;
        mSaveItemMapping[1][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[1][1], CallBackFormId);
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_2_SLAVE_LABEL)) {
        KernelConfig->C2SlUnlockHddPasswordFlag = 1;
        KernelConfig->C2SlSetHddPasswordFlag = 0;
        mSaveItemMapping[3][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[3][1], CallBackFormId);
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_3_SLAVE_LABEL)) {
        KernelConfig->C3SlUnlockHddPasswordFlag = 1;
        KernelConfig->C3SlSetHddPasswordFlag = 0;
        mSaveItemMapping[5][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[5][1], CallBackFormId);
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_4_SLAVE_LABEL)) {
        KernelConfig->C4SlUnlockHddPasswordFlag = 1;
        KernelConfig->C4SlSetHddPasswordFlag = 0;
        mSaveItemMapping[7][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[7][1], CallBackFormId);
      }
      Status = UpdateMasterHddPasswordStatus (
                 HddDataInfo[HarddiskInfoIndex].MasterFlag,
                 CallBackFormId
                 );
    } else {
      if ((CallBackFormId == HDD_PASSWORD_CH_1_MASTER_CBM_LABEL)) {
        KernelConfig->C1MsCBMSetHddPasswordFlag = 1;
        KernelConfig->C1MsCBMUnlockHddPasswordFlag = 0;
        mSaveItemMapping[0][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[0][1], CallBackFormId);
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_2_MASTER_CBM_LABEL)) {
        KernelConfig->C2MsCBMSetHddPasswordFlag = 1;
        KernelConfig->C2MsCBMUnlockHddPasswordFlag = 0;
        mSaveItemMapping[2][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[2][1], CallBackFormId);
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_3_MASTER_CBM_LABEL)){
        KernelConfig->C3MsCBMSetHddPasswordFlag = 1;
        KernelConfig->C3MsCBMUnlockHddPasswordFlag = 0;
        mSaveItemMapping[4][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[4][1], CallBackFormId);
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_4_MASTER_CBM_LABEL)) {
        KernelConfig->C4MsCBMSetHddPasswordFlag = 1;
        KernelConfig->C4MsCBMUnlockHddPasswordFlag = 0;
        mSaveItemMapping[6][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[6][1], CallBackFormId);        
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_1_SLAVE_CBM_LABEL)) {
        KernelConfig->C1SlCBMSetHddPasswordFlag = 1;
        KernelConfig->C1SlCBMUnlockHddPasswordFlag = 0;
        mSaveItemMapping[1][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[1][1], CallBackFormId);
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_2_SLAVE_CBM_LABEL)) {
        KernelConfig->C2SlCBMSetHddPasswordFlag = 1;
        KernelConfig->C2SlCBMUnlockHddPasswordFlag = 0;
        mSaveItemMapping[3][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[3][1], CallBackFormId);
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_3_SLAVE_CBM_LABEL)) {
        KernelConfig->C3SlCBMSetHddPasswordFlag = 1;
        KernelConfig->C3SlCBMUnlockHddPasswordFlag = 0;
        mSaveItemMapping[5][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[5][1], CallBackFormId);
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_4_SLAVE_CBM_LABEL)) {
        KernelConfig->C4SlCBMSetHddPasswordFlag = 1;
        KernelConfig->C4SlCBMUnlockHddPasswordFlag = 0;
        mSaveItemMapping[7][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[7][1], CallBackFormId);
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_1_MASTER_LABEL)) {
        KernelConfig->C1MsSetHddPasswordFlag = 1;
        KernelConfig->C1MsUnlockHddPasswordFlag = 0;
        mSaveItemMapping[0][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[0][1], CallBackFormId);
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_2_MASTER_LABEL)) {
        KernelConfig->C2MsSetHddPasswordFlag = 1;
        KernelConfig->C2MsUnlockHddPasswordFlag = 0;
        mSaveItemMapping[2][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[2][1], CallBackFormId);
      }

      if ((CallBackFormId == HDD_PASSWORD_CH_3_MASTER_LABEL)) {
        KernelConfig->C3MsSetHddPasswordFlag = 1;
        KernelConfig->C3MsUnlockHddPasswordFlag = 0;
        mSaveItemMapping[4][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[4][1], CallBackFormId);
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_4_MASTER_LABEL)) {
        KernelConfig->C4MsSetHddPasswordFlag = 1;
        KernelConfig->C4MsUnlockHddPasswordFlag = 0;
        mSaveItemMapping[6][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[6][1], CallBackFormId);
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_1_SLAVE_LABEL)) {
        KernelConfig->C1SlSetHddPasswordFlag = 1;
        KernelConfig->C1SlUnlockHddPasswordFlag = 0;
        mSaveItemMapping[1][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[1][1], CallBackFormId);
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_2_SLAVE_LABEL)) {
        KernelConfig->C2SlSetHddPasswordFlag = 1;
        KernelConfig->C2SlUnlockHddPasswordFlag = 0;
        mSaveItemMapping[3][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[3][1], CallBackFormId);
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_3_SLAVE_LABEL)) {
        KernelConfig->C3SlSetHddPasswordFlag = 1;
        KernelConfig->C3SlUnlockHddPasswordFlag = 0;
        mSaveItemMapping[5][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[5][1], CallBackFormId);
      }
      if ((CallBackFormId == HDD_PASSWORD_CH_4_SLAVE_LABEL)) {
        KernelConfig->C4SlSetHddPasswordFlag = 1;
        KernelConfig->C4SlUnlockHddPasswordFlag = 0;
        mSaveItemMapping[7][0] = (UINT16) HarddiskInfoIndex;
        INIT_ITEM_MAPPING (mSaveItemMapping[7][1], CallBackFormId);
      }

      Status = UpdateMasterHddPasswordStatus (
                 HddDataInfo[HarddiskInfoIndex].MasterFlag,
                 CallBackFormId 
                 );

    }
  }
  return EFI_SUCCESS;
}



/**
  Check the system password checking flag to decide to ask password or not.

  @param  SysPasswordService     system password service protocol

  @retval TRUE                   the system password checking flag is enable.
  @retval FALSE                  the system password checking flag is disable.

**/
BOOLEAN
HaveSysPasswordCheck (
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL    *SysPasswordService  OPTIONAL
  )
{
  EFI_STATUS                              Status;
  SYS_PASSWORD_SETUP_INFO_DATA            SetupInfoBuffer;


  if (SysPasswordService == NULL) {
    Status = gBS->LocateProtocol (
                    &gEfiSysPasswordServiceProtocolGuid,
                    NULL,
                    (VOID **) &SysPasswordService
                    );
    if (EFI_ERROR (Status)) {
      return FALSE;
    }
  }


  Status = SysPasswordService->GetSysPswdSetupInfoData(
                                 SysPasswordService,
                                 &SetupInfoBuffer
                                 );

  return SetupInfoBuffer.HaveSysPasswordCheck;
}

/**
  Set system password checking to disable.

  @param  SysPasswordService     system password service protocol

  @retval EFI_SUCCESS            set successful

**/
EFI_STATUS
CleanSysPasswordCheck (
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL       *SysPasswordService OPTIONAL
  )
{
  EFI_STATUS                              Status;
  SYS_PASSWORD_SETUP_INFO_DATA            SetupInfoBuffer;
  UINT32                                  SetupBits;

  SetupBits = 0;

  if (SysPasswordService == NULL) {
    Status = gBS->LocateProtocol (
                    &gEfiSysPasswordServiceProtocolGuid,
                    NULL,
                    (VOID **) &SysPasswordService
                    );
    if (EFI_ERROR (Status)) {
      return FALSE;
    }
  }

  Status = SysPasswordService->GetSysPswdSetupInfoData(
                                 SysPasswordService,
                                 &SetupInfoBuffer
                                 );

  SetupInfoBuffer.HaveSysPasswordCheck = FALSE;

  SetupBits = SYS_PASSWORD_CHECK_BIT;

  Status = SysPasswordService->SetSysPswdSetupInfoData(
                                 SysPasswordService,
                                 &SetupInfoBuffer,
                                 SetupBits
                                 );

  return Status;
}

/**
  Get previous checking password is supervior password or user password.

  @param  SysPasswordService     system password service protocol

  @retval SystemSupervisorType   current user is supervisor.
  @retval SystemUserType         current user is user.

**/
CURRENT_USER_TYPE
GetSysPasswordCurrentUser (
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL       *SysPasswordService OPTIONAL
  )
{
  EFI_STATUS                              Status;
  SYS_PASSWORD_SETUP_INFO_DATA            SetupInfoBuffer;


  if (SysPasswordService == NULL) {
    Status = gBS->LocateProtocol (
                    &gEfiSysPasswordServiceProtocolGuid,
                    NULL,
                    (VOID **) &SysPasswordService
                    );
    if (EFI_ERROR (Status)) {
      return FALSE;
    }
  }

  Status = SysPasswordService->GetSysPswdSetupInfoData(
                                 SysPasswordService,
                                 &SetupInfoBuffer
                                 );

  return SetupInfoBuffer.CurrentUser;
}

/**
  Set current password input is supervior password or user password.

  @param  SysPasswordService     system password service protocol
  @param  CurrentUserType        systemSupervisorType or systemUserType

  @retval EFI_SUCCESS            set current user successful.

**/
EFI_STATUS
SetSysPasswordCurrentUser (
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL       *SysPasswordService, OPTIONAL
  CURRENT_USER_TYPE       CurrentUserType
  )
{
  EFI_STATUS                              Status;
  SYS_PASSWORD_SETUP_INFO_DATA            SetupInfoBuffer;
  UINT32                                  SetupBits;

  SetupBits = 0;

  if (SysPasswordService == NULL) {
    Status = gBS->LocateProtocol (
                    &gEfiSysPasswordServiceProtocolGuid,
                    NULL,
                    (VOID **) &SysPasswordService
                    );
    if (EFI_ERROR (Status)) {
      return FALSE;
    }
  }

  Status = SysPasswordService->GetSysPswdSetupInfoData(
                                 SysPasswordService,
                                 &SetupInfoBuffer
                                 );

  SetupInfoBuffer.CurrentUser = CurrentUserType;

  SetupBits = CURRENT_USER_BIT;

  Status = SysPasswordService->SetSysPswdSetupInfoData(
                                 SysPasswordService,
                                 &SetupInfoBuffer,
                                 SetupBits
                                 );

  return Status;
}

/**
  Get the length of system password support.

  @param  SysPasswordService     system password service protocol

  @retval                        return the length of system password support.

**/
UINTN
SysPasswordMaxNumber (
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL    *SysPasswordService  OPTIONAL
  )
{
  EFI_STATUS                              Status;
  SYS_PASSWORD_SETUP_INFO_DATA            SetupInfoBuffer;


  if (SysPasswordService == NULL) {
    Status = gBS->LocateProtocol (
                    &gEfiSysPasswordServiceProtocolGuid,
                    NULL,
                    (VOID **) &SysPasswordService
                    );
    if (EFI_ERROR (Status)) {
      return FALSE;
    }
  }


  Status = SysPasswordService->GetSysPswdSetupInfoData(
                                 SysPasswordService,
                                 &SetupInfoBuffer
                                 );

  if (EFI_ERROR(Status)) {
    return 0;
  }

  return SetupInfoBuffer.MaxSysPasswordLength;
}

/**
 Initialize HDD Password information

 @retval EFI_SUCCESS            Initialize setup utility private data success
 @retval EFI_OUT_OF_RESOURCES   Allocate pool fail
 @return other                  Get setup utility private data fail or locate HDD Password service protocol fail or
                                get HDD security info fail
**/
EFI_STATUS
InitHddPasswordInfoForSCU (
  VOID
  )
{
  EFI_HDD_PASSWORD_SERVICE_PROTOCOL     *HddPasswordService;
  HDD_PASSWORD_INFO_DATA                *HddPasswordInfoArray;
  UINTN                                 Index;
  EFI_STATUS                            Status;
  UINTN                                 NumOfHdd;
  HDD_SECURITY_INFO_DATA                *HddInfoArray;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  SEC_HDD_PASSWORD_STRUCT               *HddPasswordTable;
  UINTN                                 DataSize;

  SuBrowser = NULL;
  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (Status != EFI_SUCCESS || SuBrowser == NULL) {
    return Status;
  }

  HddPasswordService          = NULL;
  HddPasswordInfoArray        = NULL;
  HddInfoArray                = NULL;
  SUCInfo                     = SuBrowser->SUCInfo;
  SUCInfo->HarddiskPassword   = NULL;
  SUCInfo->NumOfHarddiskPswd  = 0;
  SuBrowser->IdeConfig        = NULL;
  NumOfHdd                    = 0;
  HddPasswordTable            = NULL;
  DataSize                    = 0;

  ZeroMem (&mPasswordConfig, sizeof (PASSWORD_CONFIGURATION));

  Status = gBS->LocateProtocol (
                  &gEfiHddPasswordServiceProtocolGuid,
                  NULL,
                  (VOID **)&HddPasswordService
                  );
  if (Status != EFI_SUCCESS || HddPasswordService == NULL) {
    return Status;
  }

  Status = HddPasswordService->GetHddSecurityInfo (
                                 HddPasswordService,
                                 (VOID **)&HddInfoArray
                                 );
  if (Status != EFI_SUCCESS || HddInfoArray == NULL) {
    return Status;
  }

  NumOfHdd = HddInfoArray->TotalNumOfHddInfo;

  HddPasswordInfoArray = (HDD_PASSWORD_INFO_DATA *)AllocateZeroPool (((NumOfHdd) * sizeof(HDD_PASSWORD_INFO_DATA)));

  if (HddPasswordInfoArray == NULL) {
    return Status;
  }

  SUCInfo->NumOfHarddiskPswd = NumOfHdd;
  SUCInfo->HarddiskPassword  = HddPasswordInfoArray;

  if (NumOfHdd != 0) {
    mPasswordConfig.HddPasswordSupport = 1;

    Status = CommonGetVariableDataAndSize (
               SAVE_HDD_PASSWORD_VARIABLE_NAME,
               &gSaveHddPasswordGuid,
               &DataSize,
               (VOID **) &HddPasswordTable
               );

    for (Index = 0; Index < NumOfHdd; Index++) {
      if ((HddInfoArray[Index].HddSecurityStatus & HDD_SECURITY_LOCK) == HDD_SECURITY_LOCK) {
        HddPasswordInfoArray[Index].Flag = SECURITY_LOCK;
      } else if ((HddInfoArray[Index].HddSecurityStatus & HDD_SECURITY_ENABLE) == HDD_SECURITY_ENABLE) {
        HddPasswordInfoArray[Index].Flag = ENABLE_PASSWORD;

        if (HddPasswordTable != NULL){
          if (HddPasswordTable[Index].PasswordType != MASTER_PSW) {
            HddPasswordInfoArray[Index].MasterFlag = MASTER_PASSWORD_GRAYOUT;
          }

          ZeroMem (
            HddPasswordInfoArray[Index].DisableInputString,
            (HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (UINT16)
            );

          StrCpy (
            HddPasswordInfoArray[Index].DisableInputString,
            HddPasswordTable[Index].PasswordStr
            );

          HddPasswordInfoArray[Index].DisableAllType = HddPasswordTable[Index].PasswordType;
          HddPasswordInfoArray[Index].DisableStringLength = ((HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (UINT16));
        }        
        
      } else {
        HddPasswordInfoArray[Index].Flag           = NO_ACCESS_PASSWORD;
        HddPasswordInfoArray[Index].DisableAllType = NO_ACCESS_PSW;
      }
      HddPasswordInfoArray[Index].HddDataInfo = &HddInfoArray[Index];
      HddPasswordInfoArray[Index].NumOfEntry  = 0x01;
    }

    if (HddPasswordTable != NULL) {
      FreePool (HddPasswordTable);
    }    
    
  }

  return EFI_SUCCESS;
}


