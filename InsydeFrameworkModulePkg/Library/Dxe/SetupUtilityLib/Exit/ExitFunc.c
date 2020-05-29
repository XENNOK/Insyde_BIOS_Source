//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
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
//;   ExitFunc.c
//;
//; Abstract:
//;
//;   Exit menu relative functions
//;

#include "Exit.h"

EFI_STATUS
DiscardChange (
  IN EFI_FORM_CALLBACK_PROTOCOL             *This
  )
/*++

Routine Description:

  To discard user changed setup utility setting in this boot.

Arguments:

  This      -  Pointer to Form Callback protocol instance.

Returns:

  EFI_SUCCESS  -  Function has completed successfully.
  Other        -  Cannot get SetupUtility browser data or language data.

--*/
{
  EFI_STATUS                                Status;
  UINTN                                     BootOrderSize = 0;
  UINT16                                    *BootOrderList = NULL;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL        *Interface;
  SYSTEM_CONFIGURATION                      *MyIfrNVData;
  EFI_GUID                                  GuidId = SYSTEM_CONFIGURATION_GUID;
  UINTN                                     BufferSize;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo     = SuBrowser->SUCInfo;
  Interface   = &SuBrowser->Interface;
  BufferSize  = Interface->Size;

  MyIfrNVData = SetupUtilityLibGetVariableAndSize (
                  L"Setup",
                  &GuidId,
                  &BootOrderSize
                  );
  UpdateLangItem (This, (UINT8 *) (&(MyIfrNVData->Language)));

  if (BufferSize != 0) {
    EfiCopyMem (
      Interface->MyIfrNVData,
      MyIfrNVData,
      BufferSize
      );
  } else {
    return EFI_NOT_FOUND;
  }

  if (SUCInfo->AdvBootDeviceNum == 0){
    return EFI_SUCCESS;
  }

  BootOrderList = SetupUtilityLibGetVariableAndSize (
                    L"BootOrder",
                    &gEfiGlobalVariableGuid,
                    &BootOrderSize
                    );

  if (BufferSize != 0) {
    EfiCopyMem (
      SUCInfo->BootOrder,
      BootOrderList,
      SUCInfo->AdvBootDeviceNum  * sizeof(UINT16)
      );

    gBS->FreePool(BootOrderList);
    UpdateStringToken ((SYSTEM_CONFIGURATION *) Interface->MyIfrNVData);
  }

  Interface->Changed = TRUE;
  gBS->FreePool(MyIfrNVData);
  return EFI_SUCCESS;
}


EFI_STATUS
LoadCustomOption (
  IN EFI_FORM_CALLBACK_PROTOCOL             *This
  )
/*++

Routine Description:

  To restore setup utility setting to user custom setting

Arguments:

  This      -  Pointer to Form Callback protocol instance.

Returns:

  EFI_SUCCESS  -  Function has completed successfully.
  Other        -  Cannot get SetupUtility browser data.

--*/
{
  EFI_STATUS                                Status;
  SYSTEM_CONFIGURATION                      *MyIfrNVData;
  SYSTEM_CONFIGURATION                      SetupVariable;
  UINTN                                     BufferSize;
  EFI_GUID                                  GuidId = SYSTEM_CONFIGURATION_GUID;
  UINT8                                     *CustomBootOrder;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL        *Interface;
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
  UINTN                                     OptionSize;
  UINT8                                     *TempPtr;
  CHAR16                                    *CustomOptionDeviceName;
  UINTN                                     DeviceNameSize;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo     = SuBrowser->SUCInfo;
  Interface   = &SuBrowser->Interface;
  BufferSize  = Interface->Size;
  MyIfrNVData = SetupUtilityLibGetVariableAndSize (
                  L"Custom",
                  &GuidId,
                  &BufferSize
                  );
  if (BufferSize == 0) {
    ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->Language = 0;
    UpdateLangItem (This, (UINT8 *) (&(((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->Language)));
    return EFI_SUCCESS;
  } else {
    UpdateLangItem (This, (UINT8 *) (&(MyIfrNVData->Language)));
    //
    //Update the HDD Password status is whther Exit
    //
    MyIfrNVData->SetUserPass = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->SetUserPass;
    MyIfrNVData->UserAccessLevel = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->UserAccessLevel;
    MyIfrNVData->SupervisorFlag = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->SupervisorFlag;
    MyIfrNVData->UserFlag = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->UserFlag;
    MyIfrNVData->SetAllHddPasswordFlag = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->SetAllHddPasswordFlag;
    MyIfrNVData->UseMasterPassword             = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->UseMasterPassword;
    MyIfrNVData->C1MsSetHddPasswordFlag        = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C1MsSetHddPasswordFlag;
    MyIfrNVData->C1SlSetHddPasswordFlag        = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C1SlSetHddPasswordFlag;
    MyIfrNVData->C2MsSetHddPasswordFlag        = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C2MsSetHddPasswordFlag;
    MyIfrNVData->C2SlSetHddPasswordFlag        = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C2SlSetHddPasswordFlag;
    MyIfrNVData->C3MsSetHddPasswordFlag        = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C3MsSetHddPasswordFlag;
    MyIfrNVData->C3SlSetHddPasswordFlag        = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C3SlSetHddPasswordFlag;
    MyIfrNVData->C4MsSetHddPasswordFlag        = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C4MsSetHddPasswordFlag;
    MyIfrNVData->C4SlSetHddPasswordFlag        = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C4SlSetHddPasswordFlag;
    MyIfrNVData->C1MsCBMSetHddPasswordFlag     = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C1MsCBMSetHddPasswordFlag;
    MyIfrNVData->C1SlCBMSetHddPasswordFlag     = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C1SlCBMSetHddPasswordFlag;
    MyIfrNVData->C2MsCBMSetHddPasswordFlag     = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C2MsCBMSetHddPasswordFlag;
    MyIfrNVData->C2SlCBMSetHddPasswordFlag     = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C2SlCBMSetHddPasswordFlag;
    MyIfrNVData->C3MsCBMSetHddPasswordFlag     = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C3MsCBMSetHddPasswordFlag;
    MyIfrNVData->C3SlCBMSetHddPasswordFlag     = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C3SlCBMSetHddPasswordFlag;
    MyIfrNVData->C4MsCBMSetHddPasswordFlag     = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C4MsCBMSetHddPasswordFlag;
    MyIfrNVData->C4SlCBMSetHddPasswordFlag     = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C4SlCBMSetHddPasswordFlag;
    MyIfrNVData->C1MsUnlockHddPasswordFlag     = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C1MsUnlockHddPasswordFlag;
    MyIfrNVData->C1SlUnlockHddPasswordFlag     = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C1SlUnlockHddPasswordFlag;
    MyIfrNVData->C2MsUnlockHddPasswordFlag     = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C2MsUnlockHddPasswordFlag;
    MyIfrNVData->C2SlUnlockHddPasswordFlag     = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C2SlUnlockHddPasswordFlag;
    MyIfrNVData->C3MsUnlockHddPasswordFlag     = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C3MsUnlockHddPasswordFlag;
    MyIfrNVData->C3SlUnlockHddPasswordFlag     = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C3SlUnlockHddPasswordFlag;
    MyIfrNVData->C4MsUnlockHddPasswordFlag     = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C4MsUnlockHddPasswordFlag;
    MyIfrNVData->C4SlUnlockHddPasswordFlag     = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C4SlUnlockHddPasswordFlag;
    MyIfrNVData->C1MsCBMUnlockHddPasswordFlag  = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C1MsCBMUnlockHddPasswordFlag;
    MyIfrNVData->C1SlCBMUnlockHddPasswordFlag  = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C1SlCBMUnlockHddPasswordFlag;
    MyIfrNVData->C2MsCBMUnlockHddPasswordFlag  = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C2MsCBMUnlockHddPasswordFlag;
    MyIfrNVData->C2SlCBMUnlockHddPasswordFlag  = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C2SlCBMUnlockHddPasswordFlag;
    MyIfrNVData->C3MsCBMUnlockHddPasswordFlag  = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C3MsCBMUnlockHddPasswordFlag;
    MyIfrNVData->C3SlCBMUnlockHddPasswordFlag  = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C3SlCBMUnlockHddPasswordFlag;
    MyIfrNVData->C4MsCBMUnlockHddPasswordFlag  = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C4MsCBMUnlockHddPasswordFlag;
    MyIfrNVData->C4SlCBMUnlockHddPasswordFlag  = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->C4SlCBMUnlockHddPasswordFlag;
    //
    //Update the TPM device is whther Exit
    //
    MyIfrNVData->TpmDeviceOk = ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->TpmDeviceOk;
  }

  //
  // reserve the flag of Show BootDev Menu
  //
  EfiCopyMem (
    &SetupVariable,
    Interface->MyIfrNVData,
    Interface->Size
    );
  EfiCopyMem (
    Interface->MyIfrNVData,
    MyIfrNVData,
    Interface->Size
    );
  ((SYSTEM_CONFIGURATION *) Interface->MyIfrNVData)->NoAdvBootDev    = SetupVariable.NoAdvBootDev;
  ((SYSTEM_CONFIGURATION *) Interface->MyIfrNVData)->NoBootTypeOrder = SetupVariable.NoBootTypeOrder;
  ((SYSTEM_CONFIGURATION *) Interface->MyIfrNVData)->NoLegacyBootDev = SetupVariable.NoLegacyBootDev;
  ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->NoEfiBootDev    = MyIfrNVData->NoEfiBootDev;
  for (Index = 0; Index < MAX_BOOT_ORDER_NUMBER; Index++) {
    ((SYSTEM_CONFIGURATION *) Interface->MyIfrNVData)->NoBootDevs[Index] = SetupVariable.NoBootDevs[Index];
  }
  //
  // Load custom BootOrder and BootTypeorder
  //
  BootOrder       = SUCInfo->BootOrder;
  CustomBootOrder = SetupUtilityLibGetVariableAndSize (
                      L"CustomBootOrder",
                      &gEfiGenericVariableGuid,
                      &BufferSize
                      );
  if ((BufferSize == 0) || (SUCInfo->AdvBootDeviceNum == 0)) {
    //
    // Can't get Custom BootOrder
    //
    gBS->FreePool(MyIfrNVData);
    return EFI_SUCCESS;
  }

  BootOptionNum   = SUCInfo->AdvBootDeviceNum;
  BootOrderEntry  = EfiLibAllocateZeroPool (BootOptionNum * sizeof(BOOT_ORDER_OPTION_HEAD));
  for (Index = 0; Index < BootOptionNum; Index++) {
    SPrint (OptionName, sizeof (OptionName), L"Boot%04x", BootOrder[Index]);
    OptionPtr = SetupUtilityLibGetVariableAndSize (
                  OptionName,
                  &gEfiGlobalVariableGuid,
                  &OptionSize
                  );
    if (OptionPtr == NULL) {
      continue;
    }
    TempPtr       = (UINT8 *) OptionPtr;
    TempPtr += sizeof (UINT32) + sizeof (UINT16);
    DeviceNameSize = EfiStrSize ((UINT16 *) TempPtr);

    BootOrderEntry[Index].BootOption       = BootOrder[Index];
    BootOrderEntry[Index].DeviceNameLength = (UINT16 ) DeviceNameSize;

    BootOrderEntry[Index].DeviceName  = EfiLibAllocateZeroPool (DeviceNameSize);
    EfiCopyMem (
      BootOrderEntry[Index].DeviceName,
      TempPtr,
      DeviceNameSize
      );
    BootOrderEntry[Index].FindFlag = FALSE;
    gBS->FreePool(OptionPtr);
  }

  FoundOptionNum            = 0;
  TempBootOrder             = EfiLibAllocateZeroPool (BootOptionNum * sizeof(UINT16));
  TempCustomBootOrder       = CustomBootOrder;
  CustomBootOptionNum       = (UINTN) *CustomBootOrder;
  TempCustomBootOrder       = TempCustomBootOrder + sizeof(UINTN);
  CustomOptionDevNameLength = (UINT16 *) TempCustomBootOrder;
  TempCustomBootOrder       = TempCustomBootOrder + sizeof(UINT16);
  CustomOptionDeviceName    = (CHAR16 *) TempCustomBootOrder;
  for (Index = 0; Index < CustomBootOptionNum; Index++) {
    for (OptionIndex = 0; OptionIndex < BootOptionNum; OptionIndex++) {
      if (*CustomOptionDevNameLength ==  BootOrderEntry[OptionIndex].DeviceNameLength) {
        if ((EfiCompareMem(CustomOptionDeviceName, BootOrderEntry[OptionIndex].DeviceName, BootOrderEntry[OptionIndex].DeviceNameLength)) == 0) {
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
    EfiCopyMem (
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

  UpdateStringToken (MyIfrNVData);

  ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->NoAdvBootDev    = MyIfrNVData->NoAdvBootDev;
  ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->NoBootTypeOrder = MyIfrNVData->NoBootTypeOrder;
  ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->NoLegacyBootDev = MyIfrNVData->NoLegacyBootDev;
  ((SYSTEM_CONFIGURATION *)Interface->MyIfrNVData)->NoEfiBootDev    = MyIfrNVData->NoEfiBootDev;

  gBS->FreePool(MyIfrNVData);
  return EFI_SUCCESS;
}

EFI_STATUS
SaveCustomOption (
  IN EFI_FORM_CALLBACK_PROTOCOL             *This
  )
/*++

Routine Description:

  To save current setup utility setting to user custom setting

Arguments:

  This      -  Pointer to Form Callback protocol instance.

Returns:

  EFI_SUCCESS  -  Function has completed successfully.
  Other        -  Cannot get SetupUtility browser data.

--*/
{
  EFI_STATUS                                Status;
  UINT8                                     *CustomBootOrder;
  UINTN                                     CustomBootOrderSize;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL        *Interface;
  SYSTEM_CONFIGURATION                      *MyIfrNVData;
  UINT16                                    *BootOrder;
  UINTN                                     BootOptionNum;
  UINTN                                     Index;
  UINT16                                    *CustomOptionHead;
  UINT8                                     *TempCustomBootOrder;
  UINTN                                     NewSize;
  CHAR16                                    OptionName[20];
  VOID                                      *OptionPtr;
  UINTN                                     OptionSize;
  UINT8                                     *TempPtr;
  UINTN                                     TempSize;
  EFI_GUID                                  GuidId = SYSTEM_CONFIGURATION_GUID;
  UINTN                                     DeviceNameSize;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo   = SuBrowser->SUCInfo;
  Status = gBS->LocateProtocol (
                 &gEfiSetupUtilityBrowserProtocolGuid,
                 NULL,
                 &Interface
                 );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  MyIfrNVData = (SYSTEM_CONFIGURATION *) Interface->MyIfrNVData;

  Status = gRT->SetVariable (
                  L"Custom",
                  &GuidId,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  Interface->Size,
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
  CustomBootOrder  = EfiLibAllocateZeroPool (NewSize);
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
    SPrint (OptionName, sizeof (OptionName), L"Boot%04x", BootOrder[Index]);
    OptionPtr = SetupUtilityLibGetVariableAndSize (
                  OptionName,
                  &gEfiGlobalVariableGuid,
                  &OptionSize
                  );

    TempPtr = (UINT8 *) OptionPtr;
    TempPtr += sizeof (UINT32) + sizeof (UINT16);
    DeviceNameSize = EfiStrSize ((UINT16 *) TempPtr);

    //
    // check memory size of CustomBootOrder is enough to save new DevicePath or not
    //
    TempSize      = CustomBootOrderSize + sizeof(UINT16) + DeviceNameSize;
    while (TempSize > NewSize) {
      NewSize = NewSize * 2;
      TempPtr  = EfiLibAllocateZeroPool (NewSize);
      if (TempPtr == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      EfiCopyMem (
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


    EfiCopyMem (
      TempCustomBootOrder,
      TempPtr,
      DeviceNameSize
      );
    CustomBootOrderSize = TempSize;
    TempCustomBootOrder = TempCustomBootOrder + DeviceNameSize;

    gBS->FreePool (OptionPtr);
  }

  Status = gRT->SetVariable (
                  L"CustomBootOrder",
                  &gEfiGenericVariableGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  CustomBootOrderSize,
                  CustomBootOrder
                  );

  return EFI_SUCCESS;
}
