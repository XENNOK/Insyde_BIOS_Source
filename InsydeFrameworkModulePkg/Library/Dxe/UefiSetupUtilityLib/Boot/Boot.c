//;******************************************************************************
//;* Copyright (c) 1983-2014, Insyde Software Corporation. All Rights Reserved.
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
//;   Boot.c
//;
//; Abstract:
//;
//;   Main funcitons for Boot menu
//;

#include "Boot.h"
#include EFI_PROTOCOL_DEFINITION (OemServices)
EFI_CALLBACK_INFO                         *mBootCallBackInfo;
BOOT_CONFIGURATION                        mBootConfig;
STATIC
EFI_STATUS
InitBootMenu (
  IN EFI_HII_HANDLE       HiiHandle
  )
/*++

Routine Description:

  Initialize boot menu for setuputility use

Arguments:

  HiiHandle   - Hii hanlde for the call back routine

Returns:

  EFI_SUCCESS  -  Function has completed successfully.
  Others       -  Error occurred during execution.

--*/
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
  Status = gRT->GetVariable (
                  L"Timeout",
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &Size,
                  &Timeout
                  );
  if (!EFI_ERROR(Status) && Timeout != SuBrowser->SCBuffer->Timeout) {
    SuBrowser->SCBuffer->Timeout = Timeout;
  }

  GetBbsTypeTableSize (
    &(SuBrowser->SUCInfo->LegacyBootDevTypeCount),
    &(SuBrowser->SUCInfo->LegacyBootDevType)
    );

  BootOrder = SetupUtilityLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  if (BootOrder != NULL) {
    BootDeviceNum = (UINT16) (BootOrderSize / sizeof(UINT16));
  } else {
    BootDeviceNum = 0;
    BootOrder     = NULL;

  }

  SuBrowser->SUCInfo->BootOrder        = BootOrder;
  SuBrowser->SUCInfo->AdvBootDeviceNum = BootDeviceNum;
  Status = UpdateBootMenu (
             HiiHandle,
             SuBrowser->SCBuffer
             );

  return EFI_SUCCESS;
}

EFI_STATUS
BootCallbackRoutine (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
/*++

Routine Description:

  This function is called by the forms browser in response to a user action on a question which has the
  EFI_IFR_FLAG_CALLBACK bit set in the EFI_IFR_QUESTION_HEADER. The user action is specified by Action.
  Depending on the action, the browser may also pass the question value using Type and Value. Upon return,
  the callback function may specify the desired browser action. Callback functions should return
  EFI_UNSUPPORTEDfor all values of Action that they do not support.

Arguments:

  This           - Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  Action         - Specifies the type of action taken by the browser.
  QuestionId     - A unique value which is sent to the original exporting driver so that it can identify the
                   type of data to expect. The format of the data tends to vary based on the opcode that
                   generated the callback.
  Type           - The type of value for the question.
  Value          - A pointer to the data being sent to the original exporting driver. The type is specified
                   by Type. Type EFI_IFR_TYPE_VALUE is defined in EFI_IFR_ONE_OF_OPTION.
  ActionRequest  - On return, points to the action requested by the callback function. Type
                   EFI_BROWSER_ACTION_REQUEST is specified in SendForm() in the Form Browser Protocol.

Returns:

  EFI_SUCCESS           -  The callback successfully handled the action.
  EFI_OUT_OF_RESOURCES  -  Not enough storage is available to hold the variable and its data.
  EFI_DEVICE_ERROR      -  The variable could not be saved.
  EFI_UNSUPPORTED       -  The specified Action is not supported by the callback.

--*/
{
  EFI_STATUS                                Status;
  CHAR16                                    *StringPtr;
  UINT16                                    Index;
  UINT16                                    BbsDevTableTypeCount;
  UINT16                                    *TempBootType;
  SYSTEM_CONFIGURATION                      *MyIfrNVData;
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
  EFI_GUID                                  SetupGuid = SYSTEM_CONFIGURATION_GUID;


  *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  CallbackInfo         = EFI_BOOT_CALLBACK_INFO_FROM_THIS (This);

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (QuestionId == GET_SETUP_CONFIG || QuestionId == SET_SETUP_CONFIG) {
    BufferSize = sizeof (SYSTEM_CONFIGURATION);
    Status = SetupVariableConfig (
               &SetupGuid,
               L"SystemConfig",
               &BufferSize,
               (UINT8 *) SuBrowser->SCBuffer,
               (BOOLEAN) (QuestionId == GET_SETUP_CONFIG)
               );
    BufferSize = sizeof (BOOT_CONFIGURATION);
    Status = SetupVariableConfig (
               &SetupGuid,
               L"BootConfig",
               &BufferSize,
               (UINT8 *) &mBootConfig,
               (BOOLEAN) (QuestionId == GET_SETUP_CONFIG)
               );
    return Status;
  }

  HiiHandle               = CallbackInfo->HiiHandle;
  SUCInfo                 = SuBrowser->SUCInfo;
  BbsDevTableTypeCount    = SUCInfo->LegacyBootDevTypeCount;
  TempBootType            = SUCInfo->LegacyBootDevType;
  AdvBootDeviceNum        = SUCInfo->AdvBootDeviceNum;
  LegacyBootDeviceNum     = SUCInfo->LegacyBootDeviceNum;
  EfiBootDeviceNum        = SUCInfo->EfiBootDeviceNum;


  Status = gBS->LocateProtocol (
                 &gEfiSetupUtilityBrowserProtocolGuid,
                 NULL,
                 &Interface
                 );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  BufferSize = sizeof (SYSTEM_CONFIGURATION);
  Status = SetupVariableConfig (
             NULL,
             NULL,
             &BufferSize,
             (UINT8 *) SuBrowser->SCBuffer,
             TRUE
             );
  MyIfrNVData = SuBrowser->SCBuffer;

  Status    = EFI_SUCCESS;
  StringPtr = NULL;

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

    for (Index; Index < BbsDevTableTypeCount; Index++) {
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

    for (Index; Index < BbsDevTableTypeCount; Index++) {
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
    Status = SetupUtilityLibUpdateDeviceString (
               HiiHandle,
               MyIfrNVData
               );
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
      Status = SetupUtilityLibUpdateDeviceString (
                 HiiHandle,
                 MyIfrNVData
                 );
    }

    break;

  case KEY_DUAL_VGA_SUPPORT:
    MyIfrNVData->UefiDualVgaControllers = DUAL_VGA_CONTROLLER_DISABLE;
    break;

  default:
    Status = SuBrowser->HotKeyCallback (
                          This,
                          SuBrowser->SUCInfo->MapTable[AdvancedHiiHandle].HiiHandle,
                          Action,
                          QuestionId,
                          Type,
                          Value,
                          ActionRequest
                          );
    break;
  }
  //
  // As the LegacyUsbSupport = 0x02 means having USB in POST but no USB in legacy,
  // However, when system under UEFI boot mode, due to no legacy anymore, the value
  // of 0x02 will making existing USB code misjudged and get into unpredictable path,
  // therefore force the LegacyUsbSupport change from 0x02 to 0x01 to purify the value of
  // LegacyUsbSupport under UEFI boot mode for backward compatibility
  //
  if (MyIfrNVData->BootTypeReserved == EFI_BOOT_TYPE && MyIfrNVData->LegacyUsbSupport == 0x02) {
    MyIfrNVData->LegacyUsbSupport = 0x01;
  }
  BufferSize = sizeof (SYSTEM_CONFIGURATION);
  SetupVariableConfig (
    NULL,
    NULL,
    &BufferSize,
    (UINT8 *) SuBrowser->SCBuffer,
    FALSE
    );

  return Status;
}

EFI_STATUS
InstallBootCallbackRoutine (
  IN EFI_HANDLE                     DriverHandle,
  IN EFI_HII_HANDLE                 HiiHandle
  )
/*++

Routine Description:

  Install Boot Callback routine.

Arguments:

  DriverHandle - Specific driver handle for the call back routine
  HiiHandle    - Hii hanlde for the call back routine

Returns:

  EFI_SUCCESS  -  Function has completed successfully.
  Others       -  Error occurred during execution.

--*/
{
  EFI_STATUS                                Status;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mBootCallBackInfo = EfiLibAllocatePool (sizeof(EFI_CALLBACK_INFO));
  if (mBootCallBackInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  mBootCallBackInfo->Signature               = EFI_BOOT_MENU_SIGNATURE;
  mBootCallBackInfo->DriverCallback.ExtractConfig   = SuBrowser->ExtractConfig;
  mBootCallBackInfo->DriverCallback.RouteConfig  = SuBrowser->RouteConfig;
  mBootCallBackInfo->DriverCallback.Callback = BootCallbackRoutine;
  mBootCallBackInfo->HiiHandle = HiiHandle;
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

EFI_STATUS
UninstallBootCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle
  )
{
  EFI_STATUS       Status;

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
  return Status;
}
