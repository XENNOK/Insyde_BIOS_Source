/** @file
  Initial and callback functions for Boot page of SetupUtilityLib

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

#include "Boot.h"

EFI_CALLBACK_INFO                         *mBootCallBackInfo;
BOOT_CONFIGURATION                        mBootConfig;

/**
  Initialize boot menu for setuputility use

  @param HiiHandle               Hii hanlde for the call back routine

  @retval EFI_SUCCESS            Function has completed successfully.
  @return Other                  Error occurred during execution.

**/
STATIC
EFI_STATUS
InitBootMenu (
  IN EFI_HII_HANDLE       HiiHandle
  )
{
  EFI_STATUS                                Status;
  UINT16                                    *BootOrder;
  UINT16                                    BootDeviceNum;
  UINTN                                     BootOrderSize;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  UINT16                                    Timeout;
  UINTN                                     Size;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Sync Timeout variable to timeout value of Setup variable.
  //
  Timeout = 0;
  Size = sizeof(UINT16);
  Status = CommonGetVariable (
                  L"Timeout",
                  &gEfiGlobalVariableGuid,
                  &Size,
                  &Timeout
                  );
  if (!EFI_ERROR(Status) && Timeout != ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->Timeout) {
    ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->Timeout = Timeout;
  }

  GetLegacyBootDevType (
    &(SuBrowser->SUCInfo->LegacyBootDevTypeCount),
    &(SuBrowser->SUCInfo->LegacyBootDevType)
    );

  BootOrder     = NULL;
  BootOrderSize = 0;
  CommonGetVariableDataAndSize (
    L"BootOrder",
    &gEfiGlobalVariableGuid,
    &BootOrderSize,
    (VOID **) &BootOrder
    );
  if (BootOrderSize != 0) {
    BootDeviceNum = (UINT16) (BootOrderSize / sizeof(UINT16));
  } else {
    BootDeviceNum = 0;
    BootOrder     = NULL;

  }

  SuBrowser->SUCInfo->BootOrder        = BootOrder;
  SuBrowser->SUCInfo->AdvBootDeviceNum = BootDeviceNum;
  if (FeaturePcdGet (PcdH2OFormBrowserSupported)) {
    SuBrowser->SUCInfo->BootPriority = CommonGetVariableData (
                                         L"BootOrder",
                                         &gEfiGlobalVariableGuid
                                         );
    Status = UpdateBootOrderToOrderedListOpcode (
               HiiHandle,
               (KERNEL_CONFIGURATION *) SuBrowser->SCBuffer
               );
  } else {
    Status = UpdateBootOrderToActionOpcode (
               HiiHandle,
               (KERNEL_CONFIGURATION *) SuBrowser->SCBuffer
               );
  }


  return EFI_SUCCESS;
}

/**
  This function is called by the forms browser in response to a user action on a question which has the
  EFI_IFR_FLAG_CALLBACK bit set in the EFI_IFR_QUESTION_HEADER. The user action is specified by Action.
  Depending on the action, the browser may also pass the question value using Type and Value. Upon return,
  the callback function may specify the desired browser action. Callback functions should return
  EFI_UNSUPPORTEDfor all values of Action that they do not support.

  @param This                    Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Action                  Specifies the type of action taken by the browser.
  @param QuestionId              A unique value which is sent to the original exporting driver so that it can identify the
                                 type of data to expect. The format of the data tends to vary based on the opcode that
                                 generated the callback.
  @param Type                    The type of value for the question.
  @param Value                   A pointer to the data being sent to the original exporting driver. The type is specified
                                 by Type. Type EFI_IFR_TYPE_VALUE is defined in EFI_IFR_ONE_OF_OPTION.
  @param ActionRequest           On return, points to the action requested by the callback function. Type
                                 EFI_BROWSER_ACTION_REQUEST is specified in SendForm() in the Form Browser Protocol.

  @retval EFI_SUCCESS            The callback successfully handled the action.
  @retval EFI_OUT_OF_RESOURCES   Not enough storage is available to hold the variable and its data.
  @retval EFI_DEVICE_ERROR       The variable could not be saved.
  @return EFI_UNSUPPORTED        The specified Action is not supported by the callback.

**/
EFI_STATUS
EFIAPI
BootCallbackRoutine (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_STATUS                                Status;
  UINT16                                    Index;
  UINT16                                    BbsDevTableTypeCount;
  UINT16                                    *TempBootType;
  KERNEL_CONFIGURATION                      *MyIfrNVData;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  UINT32                                    LegacyBootDeviceNum;
  UINT32                                    EfiBootDeviceNum;
  UINT32                                    AdvBootDeviceNum;
  EFI_CALLBACK_INFO                         *CallbackInfo;
  EFI_HII_HANDLE                            HiiHandle;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL        *Interface;
  UINT32                                    BootOrderKey;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  UINTN                                     BufferSize;
  EFI_GUID                                  VarStoreGuid = SYSTEM_CONFIGURATION_GUID;

  if (!FeaturePcdGet (PcdH2OFormBrowserSupported) && Action == EFI_BROWSER_ACTION_CHANGING) {
    //
    // For compatible to old form browser which only use EFI_BROWSER_ACTION_CHANGING action,
    // change action to EFI_BROWSER_ACTION_CHANGED to make it workable.
    //
    Action = EFI_BROWSER_ACTION_CHANGED;
  }

  if (Action != EFI_BROWSER_ACTION_CHANGED) {
    return BootCallbackRoutineByAction (This, Action, QuestionId, Type, Value, ActionRequest);
  }

  *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  CallbackInfo   = EFI_CALLBACK_INFO_FROM_THIS (This);

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }


  BufferSize = GetVarStoreSize (CallbackInfo->HiiHandle, &CallbackInfo->FormsetGuid, &VarStoreGuid, "SystemConfig");
  if (!FeaturePcdGet (PcdH2OFormBrowserSupported)) {
    if (QuestionId == GET_SETUP_CONFIG || QuestionId == SET_SETUP_CONFIG) {
      Status = SetupVariableConfig (
                 &VarStoreGuid,
                 L"SystemConfig",
                 BufferSize,
                 (UINT8 *) SuBrowser->SCBuffer,
                 (BOOLEAN) (QuestionId == GET_SETUP_CONFIG)
                 );
      BufferSize = sizeof (BOOT_CONFIGURATION);
      Status = SetupVariableConfig (
                 &VarStoreGuid,
                 L"BootConfig",
                 BufferSize,
                 (UINT8 *) &mBootConfig,
                 (BOOLEAN) (QuestionId == GET_SETUP_CONFIG)
                 );

      return Status;
    }
    
    Status = gBS->LocateProtocol (
                   &gEfiSetupUtilityBrowserProtocolGuid,
                   NULL,
                   (VOID **) &Interface
                   );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }
  
  HiiHandle               = CallbackInfo->HiiHandle;
  SUCInfo                 = SuBrowser->SUCInfo;
  BbsDevTableTypeCount    = SUCInfo->LegacyBootDevTypeCount;
  TempBootType            = SUCInfo->LegacyBootDevType;
  AdvBootDeviceNum        = SUCInfo->AdvBootDeviceNum;
  LegacyBootDeviceNum     = SUCInfo->LegacyBootDeviceNum;
  EfiBootDeviceNum        = SUCInfo->EfiBootDeviceNum;

  Status = SetupVariableConfig (
             &VarStoreGuid,
             L"SystemConfig",
             BufferSize,
             (UINT8 *) SuBrowser->SCBuffer,
             TRUE
             );
  
  BufferSize = sizeof (BOOT_CONFIGURATION);
  SetupVariableConfig (
    &VarStoreGuid,
    L"BootConfig",
    BufferSize,
    (UINT8 *) &mBootConfig,
    TRUE
    );
  MyIfrNVData = (KERNEL_CONFIGURATION *) SuBrowser->SCBuffer;

  Status    = EFI_SUCCESS;

  switch (QuestionId) {
  case KEY_UP_SHIFT:
    if (Type != EFI_IFR_TYPE_NUM_SIZE_32) {
      break;
    }
    BootOrderKey = Value->u32;
    Index = 0;
    Status = EFI_INVALID_PARAMETER;
    if ((BootOrderKey > KEY_BOOT_TYPE_ORDER_BASE) &&
        (BootOrderKey < ((UINTN) KEY_BOOT_TYPE_ORDER_BASE + BbsDevTableTypeCount)) &&
        (BootOrderKey - KEY_BOOT_TYPE_ORDER_BASE) > 0) {
      Status = Shiftitem (
                 This,
                 SUCInfo->BootTypeTokenRecord,
                 (BootOrderKey - KEY_BOOT_TYPE_ORDER_BASE),
                 TRUE,
                 MyIfrNVData
                 );
      Index = MAX_BOOT_ORDER_NUMBER + 1;
    } else if ((BootOrderKey > KEY_BOOT_DEVICE_BASE) &&
               (BootOrderKey < KEY_BOOT_DEVICE_BASE + MAX_BOOT_DEVICES_NUMBER) &&
               (BootOrderKey - KEY_BOOT_DEVICE_BASE) > 0) {
      Status = Shiftitem (
                 This,
                 SUCInfo->AdvanceTokenRecord,
                 (BootOrderKey - KEY_BOOT_DEVICE_BASE),
                 TRUE,
                 MyIfrNVData
                 );
      Index = MAX_BOOT_ORDER_NUMBER + 1;
    } else if ((BootOrderKey > KEY_EFI_BOOT_DEVICE_BASE) &&
               (BootOrderKey < KEY_EFI_BOOT_DEVICE_BASE + MAX_BOOT_DEVICES_NUMBER) &&
               (BootOrderKey - KEY_EFI_BOOT_DEVICE_BASE) > 0) {
      Status = Shiftitem (
                 This,
                 SUCInfo->EfiTokenRecord,
                 (BootOrderKey - KEY_EFI_BOOT_DEVICE_BASE),
                 TRUE,
                 MyIfrNVData
                 );
      Index = MAX_BOOT_ORDER_NUMBER + 1;
    } else if ((BootOrderKey > KEY_ADV_LEGACY_BOOT_BASE) &&
               (BootOrderKey < KEY_ADV_LEGACY_BOOT_BASE + MAX_BOOT_DEVICES_NUMBER) &&
               (BootOrderKey - KEY_ADV_LEGACY_BOOT_BASE) > 0) {
      Status = Shiftitem (
                 This,
                 SUCInfo->LegacyAdvanceTokenRecord,
                 (BootOrderKey - KEY_ADV_LEGACY_BOOT_BASE),
                 TRUE,
                 MyIfrNVData
                 );
      Index = MAX_BOOT_ORDER_NUMBER + 1;
    }

    for (; Index < BbsDevTableTypeCount; Index++) {
     if ((BootOrderKey > mBbsTypeTable[TempBootType[Index]].KeyBootDeviceBase) &&
         (BootOrderKey < ((UINTN) ((mBbsTypeTable[TempBootType[Index]].KeyBootDeviceBase + MAX_BOOT_TYPE_DEVICES)))) &&
         (BootOrderKey - mBbsTypeTable[TempBootType[Index]].KeyBootDeviceBase) > 0) {
         Status = Shiftitem (
                    This,
                    SUCInfo->LegacyNormalTokenRecord[TempBootType[Index]],
                    (BootOrderKey - mBbsTypeTable[TempBootType[Index]].KeyBootDeviceBase),
                    TRUE,
                    MyIfrNVData
                    );
         break;
       }
    }

    break;

  case KEY_DOWN_SHIFT:
    if (Type != EFI_IFR_TYPE_NUM_SIZE_32) {
      break;
    }
    BootOrderKey = Value->u32;
    Index = 0;
    Status = EFI_INVALID_PARAMETER;
    if ((BootOrderKey >= KEY_BOOT_TYPE_ORDER_BASE) &&
        (BootOrderKey <= (KEY_BOOT_TYPE_ORDER_BASE + MAX_BOOT_ORDER_NUMBER)) &&
        (BootOrderKey - KEY_BOOT_TYPE_ORDER_BASE + 1) < BbsDevTableTypeCount) {
      Status = Shiftitem (
                 This,
                 SUCInfo->BootTypeTokenRecord,
                (BootOrderKey - KEY_BOOT_TYPE_ORDER_BASE),
                 FALSE,
                 MyIfrNVData
                 );
      Index = MAX_BOOT_ORDER_NUMBER + 2;
    }

    if ((BootOrderKey >= KEY_BOOT_DEVICE_BASE) &&
         //
         // for the shift deivice down, the maximum number of deiveces that we can shift
         // is (BOOT_DEVICE_BASE + MAX_BOOT_DEVICES_NUMBER - 2)
         //
        (BootOrderKey < KEY_BOOT_DEVICE_BASE + MAX_BOOT_DEVICES_NUMBER - 1) &&
        (BootOrderKey - KEY_BOOT_DEVICE_BASE + 1) < AdvBootDeviceNum) {
      Status = Shiftitem (
                 This,
                 SUCInfo->AdvanceTokenRecord,
                 (BootOrderKey - KEY_BOOT_DEVICE_BASE),
                 FALSE,
                 MyIfrNVData
                 );
      Index = MAX_BOOT_ORDER_NUMBER + 2;
    }

    if ((BootOrderKey >= KEY_EFI_BOOT_DEVICE_BASE) &&
         //
         // for the shift deivice down, the maximum number of deiveces that we can shift
         // is (BOOT_DEVICE_BASE + MAX_BOOT_DEVICES_NUMBER - 2)
         //
        (BootOrderKey < KEY_EFI_BOOT_DEVICE_BASE + MAX_BOOT_DEVICES_NUMBER - 1) &&
        (BootOrderKey - KEY_EFI_BOOT_DEVICE_BASE + 1) < EfiBootDeviceNum) {
      Status = Shiftitem (
                 This,
                 SUCInfo->EfiTokenRecord,
                 (BootOrderKey - KEY_EFI_BOOT_DEVICE_BASE),
                 FALSE,
                 MyIfrNVData
                 );
      Index = MAX_BOOT_ORDER_NUMBER + 2;
    }

    if ((BootOrderKey >= KEY_ADV_LEGACY_BOOT_BASE) &&
         //
         // for the shift deivice down, the maximum number of deiveces that we can shift
         // is (BOOT_DEVICE_BASE + MAX_BOOT_DEVICES_NUMBER - 2)
         //
        (BootOrderKey < KEY_ADV_LEGACY_BOOT_BASE + MAX_BOOT_DEVICES_NUMBER - 1) &&
        (BootOrderKey - KEY_ADV_LEGACY_BOOT_BASE + 1) < LegacyBootDeviceNum) {

      Status = Shiftitem (
                 This,
                 SUCInfo->LegacyAdvanceTokenRecord,
                 (BootOrderKey - KEY_ADV_LEGACY_BOOT_BASE),
                 FALSE,
                 MyIfrNVData
                 );
      Index = MAX_BOOT_ORDER_NUMBER + 2;
    }

    for (; Index < BbsDevTableTypeCount; Index++) {
      if ( (BootOrderKey >= mBbsTypeTable[TempBootType[Index]].KeyBootDeviceBase) &&
            //
            // for the shift deivice down, the maximum number of deiveces that we can shift
            // is (BOOT_DEVICE_BASE + MAX_BOOT_DEVICES_NUMBER - 2)
            //
            (BootOrderKey < ((UINTN) (mBbsTypeTable[TempBootType[Index]].KeyBootDeviceBase + MAX_BOOT_TYPE_DEVICES - 1))) &&
            ((BootOrderKey - mBbsTypeTable[TempBootType[Index]].KeyBootDeviceBase + 1) < ((UINT32)(mBbsTypeTable[TempBootType[Index]].DeviceTypeCount)))) {

         Status = Shiftitem (
                    This,
                    SUCInfo->LegacyNormalTokenRecord[TempBootType[Index]],
                    (BootOrderKey - mBbsTypeTable[TempBootType[Index]].KeyBootDeviceBase),
                    FALSE,
                    MyIfrNVData
                    );
         break;
       }
    }

    break;

  case KEY_BOOT_MENU_TYPE:
  case KEY_NORMAL_BOOT_PRIORITY:
  case KEY_LEGACY_NORMAL_BOOT_MENU:
    if (QuestionId == KEY_NORMAL_BOOT_PRIORITY) {
      AdjustBootOrder (
        !((BOOLEAN) MyIfrNVData->BootNormalPriority),
        SUCInfo->AdvBootDeviceNum,
        SUCInfo->BootOrder
        );
    }
    if (!FeaturePcdGet (PcdH2OFormBrowserSupported)) {
      Status = SetupUtilityLibUpdateDeviceString (
                 HiiHandle,
                 MyIfrNVData
                 );      
    }
    break;

  case KEY_NEW_POSITION_POLICY:
    //
    // If change New Added postion policy to auto, we need adjust boot oreder to suit the
    // auto position policy
    //
    if (MyIfrNVData->NewPositionPolicy == IN_AUTO) {
      ChangeToAutoBootOrder (
        SUCInfo->AdvBootDeviceNum,
        SUCInfo->BootOrder
        );
      if (!FeaturePcdGet (PcdH2OFormBrowserSupported)) {
        Status = SetupUtilityLibUpdateDeviceString (
                   HiiHandle,
                   MyIfrNVData
                   );
      }
    }

    break;

  case KEY_EFI_BOOT_DEVICE_BASE:
  case KEY_ADV_LEGACY_BOOT_BASE:
  case KEY_BOOT_TYPE_ORDER_BASE:
  case KEY_FDD_BOOT_DEVICE_BASE:
  case KEY_HDD_BOOT_DEVICE_BASE:
  case KEY_CD_BOOT_DEVICE_BASE:
  case KEY_OTHER_BOOT_DEVICE_BASE:
  case KEY_PCMCIA_BOOT_DEVICE_BASE:
  case KEY_USB_BOOT_DEVICE_BASE:
  case KEY_EMBED_NETWORK_BASE:
  case KEY_BEV_BOOT_DEVICE_BASE:
    if (FeaturePcdGet (PcdH2OFormBrowserSupported)) {
      SyncOrderListValueToBootOrder (QuestionId, (UINT16 *) Value);
      break;
    }


  default:
    Status = SuBrowser->HotKeyCallback (
                          This,
                          Action,
                          QuestionId,
                          Type,
                          Value,
                          ActionRequest
                          );
    break;
  }

  BufferSize = GetVarStoreSize (CallbackInfo->HiiHandle, &CallbackInfo->FormsetGuid, &VarStoreGuid, "SystemConfig");
  SetupVariableConfig (
    &VarStoreGuid,
    L"SystemConfig",
    BufferSize,
    (UINT8 *) SuBrowser->SCBuffer,
    FALSE
    );
  BufferSize = sizeof (BOOT_CONFIGURATION);
  SetupVariableConfig (
    &VarStoreGuid,
    L"BootConfig",
    BufferSize,
    (UINT8 *) &mBootConfig,
    FALSE
    );

  return Status;
}

EFI_STATUS
BootCallbackRoutineByAction (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_STATUS                                Status;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  EFI_CALLBACK_INFO                         *CallbackInfo;
  UINTN                                     BufferSize;
  EFI_GUID                                  VarStoreGuid = SYSTEM_CONFIGURATION_GUID;

  if ((This == NULL) ||
      ((Value == NULL) &&
       (Action != EFI_BROWSER_ACTION_FORM_OPEN) &&
       (Action != EFI_BROWSER_ACTION_FORM_CLOSE))||
      (ActionRequest == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  CallbackInfo   = EFI_CALLBACK_INFO_FROM_THIS (This);
  Status         = EFI_UNSUPPORTED;
  
  switch (Action) {

  case EFI_BROWSER_ACTION_FORM_OPEN:
    if (QuestionId == 0) {
      BufferSize = GetVarStoreSize (CallbackInfo->HiiHandle, &CallbackInfo->FormsetGuid, &VarStoreGuid, "SystemConfig");
      Status = SetupVariableConfig (
                 &VarStoreGuid,
                 L"SystemConfig",
                 BufferSize,
                 (UINT8 *) SuBrowser->SCBuffer,
                 FALSE
                 );
      BufferSize = sizeof (BOOT_CONFIGURATION);
      Status = SetupVariableConfig (
                 &VarStoreGuid,
                 L"BootConfig",
                 BufferSize,
                 (UINT8 *) &mBootConfig,
                 FALSE
                 );
    }
    break;

  case EFI_BROWSER_ACTION_FORM_CLOSE:
    if (QuestionId == 0) {
      BufferSize = GetVarStoreSize (CallbackInfo->HiiHandle, &CallbackInfo->FormsetGuid, &VarStoreGuid, "SystemConfig");
      Status = SetupVariableConfig (
                 &VarStoreGuid,
                 L"SystemConfig",
                 BufferSize,
                 (UINT8 *) SuBrowser->SCBuffer,
                 TRUE
                 );
      BufferSize = sizeof (BOOT_CONFIGURATION);
      Status = SetupVariableConfig (
                 &VarStoreGuid,
                 L"BootConfig",
                 BufferSize,
                 (UINT8 *) &mBootConfig,
                 TRUE
                 );
    }
    break;

  case EFI_BROWSER_ACTION_CHANGING:
    Status = EFI_SUCCESS;
    break;

  case EFI_BROWSER_ACTION_DEFAULT_MANUFACTURING:
    if (QuestionId == KEY_SCAN_F9) {
      Status = SuBrowser->HotKeyCallback (
                            This,
                            Action,
                            QuestionId,
                            Type,
                            Value,
                            ActionRequest
                            );
      BufferSize = GetVarStoreSize (CallbackInfo->HiiHandle, &CallbackInfo->FormsetGuid, &VarStoreGuid, "SystemConfig");
      SetupVariableConfig (
        &VarStoreGuid,
        L"SystemConfig",
        BufferSize,
        (UINT8 *) SuBrowser->SCBuffer,
        FALSE
        );
      BufferSize = sizeof (BOOT_CONFIGURATION);
      SetupVariableConfig (
        &VarStoreGuid,
        L"BootConfig",
        BufferSize,
        (UINT8 *) &mBootConfig,
        FALSE
        );
    }
    //
    // avoid GetQuestionDefault execute ExtractConfig
    //
    return EFI_SUCCESS;

  default:
    break;
  }

  return Status;
}

/**
  Install Boot Callback routine.

  @param DriverHandle            Specific driver handle for the call back routine
  @param HiiHandle               Hii hanlde for the call back routine

  @retval EFI_SUCCESS            Function has completed successfully.
  @return Other                  Error occurred during execution.

**/
EFI_STATUS
EFIAPI
InstallBootCallbackRoutine (
  IN EFI_HANDLE                     DriverHandle,
  IN EFI_HII_HANDLE                 HiiHandle
  )
{
  EFI_STATUS                                Status;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  EFI_GUID                                  FormsetGuid = FORMSET_ID_GUID_BOOT;
  EFI_GUID                                  OldFormsetGuid = SYSTEM_CONFIGURATION_GUID;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mBootCallBackInfo = AllocatePool (sizeof(EFI_CALLBACK_INFO));
  if (mBootCallBackInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  mBootCallBackInfo->Signature                    = EFI_CALLBACK_INFO_SIGNATURE;
  mBootCallBackInfo->DriverCallback.ExtractConfig = SuBrowser->ExtractConfig;
  mBootCallBackInfo->DriverCallback.RouteConfig   = SuBrowser->RouteConfig;
  mBootCallBackInfo->DriverCallback.Callback      = BootCallbackRoutine;
  mBootCallBackInfo->HiiHandle                    = HiiHandle;
  if (FeaturePcdGet (PcdH2OFormBrowserSupported)) {
    CopyGuid (&mBootCallBackInfo->FormsetGuid, &FormsetGuid);
  } else {
    CopyGuid (&mBootCallBackInfo->FormsetGuid, &OldFormsetGuid);
  }

  //
  // Install protocol interface
  //
  Status = gBS->InstallProtocolInterface (
                  &DriverHandle,
                  &gEfiHiiConfigAccessProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mBootCallBackInfo->DriverCallback
                  );
  ASSERT_EFI_ERROR (Status);

  Status = InitBootMenu (HiiHandle);

  return Status;
}

/**
  Uninstall Boot Callback routine.

  @param DriverHandle            Specific driver handle for the call back routine

  @retval EFI_SUCCESS            Function has completed successfully.
  @return Other                  Error occurred during execution.

**/
EFI_STATUS
EFIAPI
UninstallBootCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle
  )
{
  EFI_STATUS                                Status;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  SETUP_UTILITY_CONFIGURATION                *SUCInfo;

  if (mBootCallBackInfo == NULL) {
    return EFI_SUCCESS;
  }
  Status = gBS->UninstallProtocolInterface (
                  DriverHandle,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &mBootCallBackInfo->DriverCallback
                  );
  ASSERT_EFI_ERROR (Status);
  gBS->FreePool (mBootCallBackInfo);
  mBootCallBackInfo = NULL;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (!EFI_ERROR (Status)) {
    SUCInfo = SuBrowser->SUCInfo;

    if (SUCInfo->LegacyBootDevType != NULL) {
      gBS->FreePool (SUCInfo->LegacyBootDevType);
      SUCInfo->LegacyBootDevType = NULL;
    }
    if (SUCInfo->BootOrder != NULL) {
      gBS->FreePool (SUCInfo->BootOrder);
      SUCInfo->BootOrder = NULL;
    }
    if (SUCInfo->BootPriority != NULL) {
      gBS->FreePool (SUCInfo->BootPriority);
      SUCInfo->BootPriority = NULL;
    }
  }

  return Status;
}
