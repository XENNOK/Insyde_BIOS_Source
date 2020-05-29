/** @file
  Exit menu relative functions

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

#include "Exit.h"

/**
  To discard user changed setup utility setting in this boot.

  @param This                    Pointer to Form Callback protocol instance.

  @retval EFI_SUCCESS            Function has completed successfully.
  @return Other                  Cannot get SetupUtility browser data or language data.

**/
EFI_STATUS
DiscardChange (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This
  )
{
  EFI_STATUS                                Status;
  UINT16                                    *BootOrderList = NULL;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL        *Interface;
  KERNEL_CONFIGURATION                      *MyIfrNVData;
  EFI_GUID                                  GuidId = SYSTEM_CONFIGURATION_GUID;
  UINTN                                     BufferSize;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo     = SuBrowser->SUCInfo;
  Interface   = &SuBrowser->Interface;

  Status = CommonGetVariableDataAndSize (
             L"Setup",
             &GuidId,
             &BufferSize,
             (VOID **) &MyIfrNVData
             );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }
  UpdateLangItem (This, (UINT8 *) (&(MyIfrNVData->Language)));

  MyIfrNVData->SupervisorFlag = ((KERNEL_CONFIGURATION *) SUCInfo->SCBuffer)->SupervisorFlag;
  MyIfrNVData->UserFlag       = ((KERNEL_CONFIGURATION *) SUCInfo->SCBuffer)->UserFlag;

  if (BufferSize != 0) {
    CopyMem (
      SuBrowser->SCBuffer,
      MyIfrNVData,
      BufferSize
      );
  } else {
    return EFI_NOT_FOUND;
  }

  if (SUCInfo->AdvBootDeviceNum == 0){
    return EFI_SUCCESS;
  }

  BootOrderList = CommonGetVariableData (
                    L"BootOrder",
                    &gEfiGlobalVariableGuid
                    );

  if (BufferSize != 0) {
    CopyMem (
      SUCInfo->BootOrder,
      BootOrderList,
      SUCInfo->AdvBootDeviceNum  * sizeof(UINT16)
      );

    gBS->FreePool(BootOrderList);
    UpdateStringToken ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer);
  }

  Interface->Changed = TRUE;
  gBS->FreePool(MyIfrNVData);
  return EFI_SUCCESS;
}

/**
  To restore setup utility setting to user custom setting

  @param This                    Pointer to Form Callback protocol instance.

  @retval EFI_SUCCESS            Function has completed successfully.
  @return Other                  Cannot get SetupUtility browser data.

**/
EFI_STATUS
LoadCustomOption (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This
  )
{
  EFI_STATUS                                Status;
  UINT8                                     *CustomVar;
  KERNEL_CONFIGURATION                      *MyIfrNVData;
  UINT8                                     *SetupVariable;
  UINTN                                     BufferSize;
  EFI_GUID                                  GuidId = SYSTEM_CONFIGURATION_GUID;
  UINT8                                     *CustomBootOrder;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  UINT8                                     *TempCustomBootOrder;
  CHAR16                                    *BootOrder;
  BOOT_ORDER_OPTION_HEAD                    *BootOrderEntry;
  UINT16                                    *CustomOptionDevNameLength;
  UINTN                                     CustomBootOptionNum;
  UINTN                                     BootOptionNum;
  UINTN                                     Index;
  UINTN                                     OptionIndex;
  UINTN                                     FoundOptionNum;
  UINT16                                    *TempBootOrder;
  CHAR16                                    OptionName[20];
  VOID                                      *OptionPtr;
  UINT8                                     *TempPtr;
  CHAR16                                    *CustomOptionDeviceName;
  UINTN                                     DeviceNameSize;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  CHAR8                                     *LangStr;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo     = SuBrowser->SUCInfo;
  CustomVar   = CommonGetVariableData (
                  L"Custom",
                  &GuidId
                  );
  if (CustomVar == NULL) {
    return EFI_SUCCESS;
  }

  MyIfrNVData = (KERNEL_CONFIGURATION *) CustomVar;
  MyIfrNVData->Language = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->Language;
  LangStr = (CHAR8 *) CommonGetVariableData (L"CustomPlatformLang", &gEfiGenericVariableGuid);
  if (LangStr != NULL) {
    GetLangIndex (LangStr, &MyIfrNVData->Language);
    UpdateLangItem (This, (UINT8 *) (&(MyIfrNVData->Language)));
    gBS->FreePool (LangStr);
  }

  //
  //Update the HDD Password status is whther Exit
  //
  MyIfrNVData->SetUserPass                   = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->SetUserPass;
  MyIfrNVData->UserAccessLevel               = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->UserAccessLevel;
  MyIfrNVData->SupervisorFlag                = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->SupervisorFlag;
  MyIfrNVData->UserFlag                      = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->UserFlag;
  MyIfrNVData->SetAllHddPasswordFlag         = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->SetAllHddPasswordFlag;
  MyIfrNVData->UseMasterPassword             = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->UseMasterPassword;
  MyIfrNVData->C1MsSetHddPasswordFlag        = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C1MsSetHddPasswordFlag;
  MyIfrNVData->C1SlSetHddPasswordFlag        = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C1SlSetHddPasswordFlag;
  MyIfrNVData->C2MsSetHddPasswordFlag        = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C2MsSetHddPasswordFlag;
  MyIfrNVData->C2SlSetHddPasswordFlag        = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C2SlSetHddPasswordFlag;
  MyIfrNVData->C3MsSetHddPasswordFlag        = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C3MsSetHddPasswordFlag;
  MyIfrNVData->C3SlSetHddPasswordFlag        = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C3SlSetHddPasswordFlag;
  MyIfrNVData->C4MsSetHddPasswordFlag        = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C4MsSetHddPasswordFlag;
  MyIfrNVData->C4SlSetHddPasswordFlag        = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C4SlSetHddPasswordFlag;
  MyIfrNVData->C1MsCBMSetHddPasswordFlag     = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C1MsCBMSetHddPasswordFlag;
  MyIfrNVData->C1SlCBMSetHddPasswordFlag     = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C1SlCBMSetHddPasswordFlag;
  MyIfrNVData->C2MsCBMSetHddPasswordFlag     = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C2MsCBMSetHddPasswordFlag;
  MyIfrNVData->C2SlCBMSetHddPasswordFlag     = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C2SlCBMSetHddPasswordFlag;
  MyIfrNVData->C3MsCBMSetHddPasswordFlag     = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C3MsCBMSetHddPasswordFlag;
  MyIfrNVData->C3SlCBMSetHddPasswordFlag     = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C3SlCBMSetHddPasswordFlag;
  MyIfrNVData->C4MsCBMSetHddPasswordFlag     = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C4MsCBMSetHddPasswordFlag;
  MyIfrNVData->C4SlCBMSetHddPasswordFlag     = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C4SlCBMSetHddPasswordFlag;
  MyIfrNVData->C1MsUnlockHddPasswordFlag     = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C1MsUnlockHddPasswordFlag;
  MyIfrNVData->C1SlUnlockHddPasswordFlag     = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C1SlUnlockHddPasswordFlag;
  MyIfrNVData->C2MsUnlockHddPasswordFlag     = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C2MsUnlockHddPasswordFlag;
  MyIfrNVData->C2SlUnlockHddPasswordFlag     = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C2SlUnlockHddPasswordFlag;
  MyIfrNVData->C3MsUnlockHddPasswordFlag     = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C3MsUnlockHddPasswordFlag;
  MyIfrNVData->C3SlUnlockHddPasswordFlag     = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C3SlUnlockHddPasswordFlag;
  MyIfrNVData->C4MsUnlockHddPasswordFlag     = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C4MsUnlockHddPasswordFlag;
  MyIfrNVData->C4SlUnlockHddPasswordFlag     = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C4SlUnlockHddPasswordFlag;
  MyIfrNVData->C1MsCBMUnlockHddPasswordFlag  = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C1MsCBMUnlockHddPasswordFlag;
  MyIfrNVData->C1SlCBMUnlockHddPasswordFlag  = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C1SlCBMUnlockHddPasswordFlag;
  MyIfrNVData->C2MsCBMUnlockHddPasswordFlag  = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C2MsCBMUnlockHddPasswordFlag;
  MyIfrNVData->C2SlCBMUnlockHddPasswordFlag  = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C2SlCBMUnlockHddPasswordFlag;
  MyIfrNVData->C3MsCBMUnlockHddPasswordFlag  = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C3MsCBMUnlockHddPasswordFlag;
  MyIfrNVData->C3SlCBMUnlockHddPasswordFlag  = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C3SlCBMUnlockHddPasswordFlag;
  MyIfrNVData->C4MsCBMUnlockHddPasswordFlag  = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C4MsCBMUnlockHddPasswordFlag;
  MyIfrNVData->C4SlCBMUnlockHddPasswordFlag  = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->C4SlCBMUnlockHddPasswordFlag;
  //
  //Update the TPM device is whther Exit
  //
  MyIfrNVData->TpmDeviceOk                   = ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->TpmDeviceOk;

  BufferSize = GetSetupVariableSize ();
  SetupVariable = AllocatePool (BufferSize);
  if (SetupVariable == NULL) {
    gBS->FreePool(CustomVar);
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // reserve the flag of Show BootDev Menu
  //
  CopyMem (
    SetupVariable,
    SuBrowser->SCBuffer,
    BufferSize
    );
  CopyMem (
    SuBrowser->SCBuffer,
    CustomVar,
    BufferSize
    );
  ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->NoAdvBootDev    = ((KERNEL_CONFIGURATION *) SetupVariable)->NoAdvBootDev;
  ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->NoBootTypeOrder = ((KERNEL_CONFIGURATION *) SetupVariable)->NoBootTypeOrder;
  ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->NoLegacyBootDev = ((KERNEL_CONFIGURATION *) SetupVariable)->NoLegacyBootDev;
  ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->NoEfiBootDev    = MyIfrNVData->NoEfiBootDev;

  gBS->FreePool (SetupVariable);

  //
  // Load custom BootOrder and BootTypeorder
  //
  BootOrder       = SUCInfo->BootOrder;
  CustomBootOrder = CommonGetVariableData (
                      L"CustomBootOrder",
                      &gEfiGenericVariableGuid
                      );
  if ((CustomBootOrder == NULL) || (SUCInfo->AdvBootDeviceNum == 0)) {
    //
    // Can't get Custom BootOrder
    //
    gBS->FreePool(CustomVar);
    return EFI_SUCCESS;
  }

  BootOptionNum   = SUCInfo->AdvBootDeviceNum;
  BootOrderEntry  = AllocateZeroPool (BootOptionNum * sizeof(BOOT_ORDER_OPTION_HEAD));
  if (BootOrderEntry == NULL) {
    gBS->FreePool (CustomVar);
    gBS->FreePool (CustomBootOrder);
    return EFI_OUT_OF_RESOURCES;
  }

  TempBootOrder = AllocateZeroPool (BootOptionNum * sizeof(UINT16));
  if (TempBootOrder == NULL) {
    gBS->FreePool (CustomVar);
    gBS->FreePool (CustomBootOrder);
    gBS->FreePool (BootOrderEntry);
    return EFI_OUT_OF_RESOURCES;
  }


  for (Index = 0; Index < BootOptionNum; Index++) {
    UnicodeSPrint (OptionName, sizeof (OptionName), L"Boot%04x", BootOrder[Index]);
    OptionPtr = CommonGetVariableData (
                  OptionName,
                  &gEfiGlobalVariableGuid
                  );
    if (OptionPtr == NULL) {
      continue;
    }
    TempPtr       = (UINT8 *) OptionPtr;
    TempPtr += sizeof (UINT32) + sizeof (UINT16);
    DeviceNameSize = StrSize ((UINT16 *) TempPtr);

    BootOrderEntry[Index].BootOption       = BootOrder[Index];
    BootOrderEntry[Index].DeviceNameLength = (UINT16 ) DeviceNameSize;

    BootOrderEntry[Index].DeviceName  = AllocateZeroPool (DeviceNameSize);
    CopyMem (
      BootOrderEntry[Index].DeviceName,
      TempPtr,
      DeviceNameSize
      );
    BootOrderEntry[Index].FindFlag = FALSE;
    gBS->FreePool(OptionPtr);
  }

  FoundOptionNum            = 0;
  TempCustomBootOrder       = CustomBootOrder;
  CustomBootOptionNum       = (UINTN) *CustomBootOrder;
  TempCustomBootOrder       = TempCustomBootOrder + sizeof(UINTN);
  CustomOptionDevNameLength = (UINT16 *) TempCustomBootOrder;
  TempCustomBootOrder       = TempCustomBootOrder + sizeof(UINT16);
  CustomOptionDeviceName    = (CHAR16 *) TempCustomBootOrder;
  for (Index = 0; Index < CustomBootOptionNum; Index++) {
    for (OptionIndex = 0; OptionIndex < BootOptionNum; OptionIndex++) {
      if (*CustomOptionDevNameLength ==  BootOrderEntry[OptionIndex].DeviceNameLength) {
        if ((CompareMem(CustomOptionDeviceName, BootOrderEntry[OptionIndex].DeviceName, BootOrderEntry[OptionIndex].DeviceNameLength)) == 0) {
          TempBootOrder[FoundOptionNum++] = BootOrderEntry[OptionIndex].BootOption;
          BootOrderEntry[OptionIndex].FindFlag = TRUE;
        }
      }
    }
    TempCustomBootOrder       = TempCustomBootOrder + *CustomOptionDevNameLength;
    CustomOptionDevNameLength = (UINT16 *) TempCustomBootOrder;
    TempCustomBootOrder       = TempCustomBootOrder + sizeof(UINT16);
    CustomOptionDeviceName    = (CHAR16 *) TempCustomBootOrder;
  }

  if (FoundOptionNum < BootOptionNum) {
    for (Index = 0; Index < BootOptionNum; Index++) {
      if ( BootOrderEntry[Index].FindFlag == FALSE) {
        TempBootOrder[FoundOptionNum++] = BootOrderEntry[Index].BootOption;
        BootOrderEntry[Index].FindFlag = TRUE;
      }
    }
  }

  if (FoundOptionNum <= BootOptionNum) {
    CopyMem (
      SUCInfo->BootOrder,
      TempBootOrder,
      BootOptionNum * sizeof(UINT16)
      );
  }

  for (Index = 0; Index < BootOptionNum; Index++) {
    gBS->FreePool (BootOrderEntry[Index].DeviceName);
  }
  gBS->FreePool(CustomBootOrder);
  gBS->FreePool(BootOrderEntry);
  gBS->FreePool(TempBootOrder);

  UpdateStringToken ((KERNEL_CONFIGURATION *) CustomVar);

  ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->NoAdvBootDev    = MyIfrNVData->NoAdvBootDev;
  ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->NoBootTypeOrder = MyIfrNVData->NoBootTypeOrder;
  ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->NoLegacyBootDev = MyIfrNVData->NoLegacyBootDev;
  ((KERNEL_CONFIGURATION *)SuBrowser->SCBuffer)->NoEfiBootDev    = MyIfrNVData->NoEfiBootDev;

  gBS->FreePool(CustomVar);

  return EFI_SUCCESS;
}

/**
  To save current setup utility setting to user custom setting

  @param This                    Pointer to Form Callback protocol instance.

  @retval EFI_SUCCESS            Function has completed successfully.
  @return Other                  Cannot save SetupUtility browser data.

**/
EFI_STATUS
SaveCustomOption (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This
  )
{
  EFI_STATUS                                Status;
  UINT8                                     *CustomBootOrder;
  UINTN                                     CustomBootOrderSize;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  KERNEL_CONFIGURATION                      *MyIfrNVData;
  UINT16                                    *BootOrder;
  UINTN                                     BootOptionNum;
  UINTN                                     Index;
  UINT16                                    *CustomOptionHead;
  UINT8                                     *TempCustomBootOrder;
  UINTN                                     NewSize;
  CHAR16                                    OptionName[20];
  VOID                                      *OptionPtr;
  UINT8                                     *TempPtr;
  UINTN                                     TempSize;
  EFI_GUID                                  GuidId = SYSTEM_CONFIGURATION_GUID;
  UINTN                                     DeviceNameSize;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  UINT8                                     *VarData;
  UINTN                                     VarSize;
  UINTN                                     BufferSize;
  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo   = SuBrowser->SUCInfo;

  BufferSize = GetSetupVariableSize ();
  if (BufferSize == 0) {
    return EFI_ABORTED;
  }

  MyIfrNVData = (KERNEL_CONFIGURATION *) SuBrowser->SCBuffer;

  VarSize = 0;
  VarData = NULL;
  CommonGetVariableDataAndSize (
    L"PlatformLang",
    &gEfiGlobalVariableGuid,
    &VarSize,
    (VOID **) &VarData
    );
  if (VarData != NULL) {
    Status = CommonSetVariable (
                    L"CustomPlatformLang",
                    &gEfiGenericVariableGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    VarSize,
                    VarData
                    );
    gBS->FreePool (VarData);
  }

  Status = CommonSetVariable (
                  L"Custom",
                  &GuidId,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  BufferSize,
                  (VOID *) MyIfrNVData
                  );

  if (SUCInfo->AdvBootDeviceNum == 0) {
    return EFI_SUCCESS;
  }

  //
  // Locate memory and initial CustomBootOrder
  //
  BootOrder        = SUCInfo->BootOrder;
  BootOptionNum    = SUCInfo->AdvBootDeviceNum;
  NewSize          = CUSTOM_BOOT_ORDER_SIZE;
  CustomBootOrder  = AllocateZeroPool (NewSize);
  if (CustomBootOrder == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  *((UINTN *) CustomBootOrder) = BootOptionNum;
  CustomBootOrderSize      = sizeof(UINTN);
  TempCustomBootOrder      = CustomBootOrder + CustomBootOrderSize;

  //
  // get current BootOrder and Device name
  //
  for (Index = 0; Index < BootOptionNum; Index++) {
    UnicodeSPrint (OptionName, sizeof (OptionName), L"Boot%04x", BootOrder[Index]);
    OptionPtr = CommonGetVariableData (
                  OptionName,
                  &gEfiGlobalVariableGuid
                  );

    TempPtr = (UINT8 *) OptionPtr;
    TempPtr += sizeof (UINT32) + sizeof (UINT16);
    DeviceNameSize = StrSize ((UINT16 *) TempPtr);

    //
    // check memory size of CustomBootOrder is enough to save new DevicePath or not
    //
    TempSize      = CustomBootOrderSize + sizeof(UINT16) + DeviceNameSize;
    while (TempSize > NewSize) {
      NewSize = NewSize * 2;
      TempPtr  = AllocateZeroPool (NewSize);
      if (TempPtr == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      CopyMem (
        TempPtr,
        CustomBootOrder,
        CustomBootOrderSize
        );

      gBS->FreePool (CustomBootOrder);
      CustomBootOrder     = TempPtr;
      TempCustomBootOrder = CustomBootOrder;
      TempCustomBootOrder = TempCustomBootOrder + CustomBootOrderSize;
    }

    CustomOptionHead    = (UINT16 *) TempCustomBootOrder;
    *CustomOptionHead   = (UINT16) DeviceNameSize;

    TempCustomBootOrder = TempCustomBootOrder + sizeof(UINT16);


    CopyMem (
      TempCustomBootOrder,
      TempPtr,
      DeviceNameSize
      );
    CustomBootOrderSize = TempSize;
    TempCustomBootOrder = TempCustomBootOrder + DeviceNameSize;

    gBS->FreePool (OptionPtr);
  }

  Status = CommonSetVariable (
             L"CustomBootOrder",
             &gEfiGenericVariableGuid,
             EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
             CustomBootOrderSize,
             CustomBootOrder
             );

  gBS->FreePool (CustomBootOrder);
  return EFI_SUCCESS;
}


/**
  Make form browser to refresh form set. Update label data to update Hii form package.

  @retval EFI_SUCCESS            Success to trigger form browser refresh form set
  @retval EFI_OUT_OF_RESOURCES   Allocate op code data fail
  @return Other                  Update label fail

**/
EFI_STATUS
BrowserRefreshFormSet (
  VOID
  )
{
  EFI_STATUS                            Status;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  EFI_HII_HANDLE                        HiiHandle;
  VOID                                  *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL                    *StartLabel;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  HiiHandle = SuBrowser->SUCInfo->MapTable[ExitHiiHandle].HiiHandle;

  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  if (StartOpCodeHandle == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = TRIGGER_BROWSER_REFRESH_LABEL;

  Status = HiiUpdateForm (
             HiiHandle,
             NULL,
             ROOT_FORM_ID,
             StartOpCodeHandle,
             NULL
             );

  HiiFreeOpCodeHandle (StartOpCodeHandle);

  return Status;
}

