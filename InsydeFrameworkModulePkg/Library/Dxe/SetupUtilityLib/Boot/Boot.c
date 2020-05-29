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
//;   Boot.c
//;
//; Abstract:
//;
//;   Main funcitons for Boot menu
//;

#include "Boot.h"
#include EFI_PROTOCOL_DEFINITION (OemServices)

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

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
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
  IN EFI_FORM_CALLBACK_PROTOCOL       *This,
  IN UINT16                           KeyValue,
  IN EFI_IFR_DATA_ARRAY               *Data,
  OUT EFI_HII_CALLBACK_PACKET         **Packet
  )
/*++

Routine Description:

  This is the callback function for the Boot Menu. Dynamically creates the boot order list.

Arguments:

  This      -  Pointer to Form Callback protocol instance.
  KeyValue  -  A unique value which is sent to the original exporting driver so that it
               can identify the type of data to expect.  The format of the data tends to
               vary based on the op-code that geerated the callback.
  Data      -  A pointer to the data being sent to the original exporting driver.
  Packet    -  The result data returned.

Returns:

  EFI_SUCCESS  -  Must return success upon completion.
  Others       -  Error occurred during execution.

--*/
{
  EFI_STATUS                                Status;
  BOOLEAN                                   UsePacket;
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
  OEM_SERVICES_PROTOCOL                     *OemServices;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CallbackInfo            = EFI_BOOT_CALLBACK_INFO_FROM_THIS (This);
  HiiHandle               = CallbackInfo->HiiHandle;
  SUCInfo                 = SuBrowser->SUCInfo;
  BbsDevTableTypeCount    = SUCInfo->LegacyBootDevTypeCount;
  TempBootType            = SUCInfo->LegacyBootDevType;
  AdvBootDeviceNum        = SUCInfo->AdvBootDeviceNum;
  LegacyBootDeviceNum     = SUCInfo->LegacyBootDeviceNum;
  EfiBootDeviceNum        = SUCInfo->EfiBootDeviceNum;
  Interface               = &SuBrowser->Interface;
  Interface->MyIfrNVData  = Data->NvRamMap;
  MyIfrNVData             = (SYSTEM_CONFIGURATION *) Interface->MyIfrNVData;

  Status    = EFI_SUCCESS;
  UsePacket = FALSE;
  StringPtr = NULL;

  if (Packet != NULL) {
    Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    sizeof (EFI_HII_CALLBACK_PACKET) + 2,
                    Packet
                    );

    EfiZeroMem (*Packet, sizeof (EFI_HII_CALLBACK_PACKET) + 2);
    (*Packet)->DataArray.EntryCount = 1;
    (*Packet)->DataArray.NvRamMap = NULL;
  }
  OemServices = NULL;
  Status = gBS->LocateProtocol (
                  &gOemServicesProtocolGuid,
                  NULL,
                  &OemServices
                  );
  switch (KeyValue) {

  case KEY_UP_SHIFT:
    Index = 0;
    Status = EFI_INVALID_PARAMETER;
    if ((Data->EntryCount > KEY_BOOT_TYPE_ORDER_BASE) &&
        (Data->EntryCount < ((UINTN) KEY_BOOT_TYPE_ORDER_BASE + BbsDevTableTypeCount)) &&
        (Data->EntryCount - KEY_BOOT_TYPE_ORDER_BASE) > 0) {
      Status = Shiftitem (
                 This,
                 SUCInfo->BootTypeTokenRecord,
                 (Data->EntryCount - KEY_BOOT_TYPE_ORDER_BASE),
                 TRUE,
                 MyIfrNVData
                 );
      Index = MAX_BOOT_ORDER_NUMBER + 1;
    } else if ((Data->EntryCount > KEY_BOOT_DEVICE_BASE) &&
               (Data->EntryCount < KEY_BOOT_DEVICE_BASE + MAX_BOOT_DEVICES_NUMBER) &&
               (Data->EntryCount - KEY_BOOT_DEVICE_BASE) > 0) {
      Status = Shiftitem (
                 This,
                 SUCInfo->AdvanceTokenRecord,
                 (Data->EntryCount - KEY_BOOT_DEVICE_BASE),
                 TRUE,
                 MyIfrNVData
                 );
      Index = MAX_BOOT_ORDER_NUMBER + 1;
    } else if ((Data->EntryCount > KEY_EFI_BOOT_DEVICE_BASE) &&
               (Data->EntryCount < KEY_EFI_BOOT_DEVICE_BASE + MAX_BOOT_DEVICES_NUMBER) &&
               (Data->EntryCount - KEY_EFI_BOOT_DEVICE_BASE) > 0) {
      Status = Shiftitem (
                 This,
                 SUCInfo->EfiTokenRecord,
                 (Data->EntryCount - KEY_EFI_BOOT_DEVICE_BASE),
                 TRUE,
                 MyIfrNVData
                 );
      Index = MAX_BOOT_ORDER_NUMBER + 1;
    } else if ((Data->EntryCount > KEY_ADV_LEGACY_BOOT_BASE) &&
               (Data->EntryCount < KEY_ADV_LEGACY_BOOT_BASE + MAX_BOOT_DEVICES_NUMBER) &&
               (Data->EntryCount - KEY_ADV_LEGACY_BOOT_BASE) > 0) {
      Status = Shiftitem (
                 This,
                 SUCInfo->LegacyAdvanceTokenRecord,
                 (Data->EntryCount - KEY_ADV_LEGACY_BOOT_BASE),
                 TRUE,
                 MyIfrNVData
                 );
      Index = MAX_BOOT_ORDER_NUMBER + 1;
    }

    for (Index; Index < BbsDevTableTypeCount; Index++) {
     if ((Data->EntryCount > mBbsTypeTable[TempBootType[Index]].KeyBootDeviceBase) &&
         (Data->EntryCount < ((UINTN) ((mBbsTypeTable[TempBootType[Index]].KeyBootDeviceBase + MAX_BOOT_TYPE_DEVICES)))) &&
         (Data->EntryCount - mBbsTypeTable[TempBootType[Index]].KeyBootDeviceBase) > 0) {
         Status = Shiftitem (
                    This,
                    SUCInfo->LegacyNormalTokenRecord[TempBootType[Index]],
                    (Data->EntryCount - mBbsTypeTable[TempBootType[Index]].KeyBootDeviceBase),
                    TRUE,
                    MyIfrNVData
                    );
         break;
       }
    }
    break;

  case KEY_DOWN_SHIFT:
    Index = 0;
    Status = EFI_INVALID_PARAMETER;
    if ((Data->EntryCount >= KEY_BOOT_TYPE_ORDER_BASE) &&
        (Data->EntryCount <= ((UINTN) KEY_BOOT_TYPE_ORDER_BASE + MAX_BOOT_ORDER_NUMBER)) &&
        (Data->EntryCount - KEY_BOOT_TYPE_ORDER_BASE + 1) < (UINT32)(BbsDevTableTypeCount)) {
      Status = Shiftitem (
                 This,
                 SUCInfo->BootTypeTokenRecord,
                 (Data->EntryCount - KEY_BOOT_TYPE_ORDER_BASE),
                 FALSE,
                 MyIfrNVData
                 );
      Index = MAX_BOOT_ORDER_NUMBER + 2;
    }

    if ((Data->EntryCount >= KEY_BOOT_DEVICE_BASE) &&
         //
         // for the shift deivice down, the maximum number of deiveces that we can shift
         // is (BOOT_DEVICE_BASE + MAX_BOOT_DEVICES_NUMBER - 2)
         //
        (Data->EntryCount < KEY_BOOT_DEVICE_BASE + MAX_BOOT_DEVICES_NUMBER - 1) &&
        (Data->EntryCount - KEY_BOOT_DEVICE_BASE + 1) < AdvBootDeviceNum) {
      Status = Shiftitem (
                 This,
                 SUCInfo->AdvanceTokenRecord,
                 (Data->EntryCount - KEY_BOOT_DEVICE_BASE),
                 FALSE,
                 MyIfrNVData
                 );
      Index = MAX_BOOT_ORDER_NUMBER + 2;
    }

    if ((Data->EntryCount >= KEY_EFI_BOOT_DEVICE_BASE) &&
         //
         // for the shift deivice down, the maximum number of deiveces that we can shift
         // is (BOOT_DEVICE_BASE + MAX_BOOT_DEVICES_NUMBER - 2)
         //
        (Data->EntryCount < KEY_EFI_BOOT_DEVICE_BASE + MAX_BOOT_DEVICES_NUMBER - 1) &&
        (Data->EntryCount - KEY_EFI_BOOT_DEVICE_BASE + 1) < EfiBootDeviceNum) {
      Status = Shiftitem (
                 This,
                 SUCInfo->EfiTokenRecord,
                 (Data->EntryCount - KEY_EFI_BOOT_DEVICE_BASE),
                 FALSE,
                 MyIfrNVData
                 );
      Index = MAX_BOOT_ORDER_NUMBER + 2;
    }

    if ((Data->EntryCount >= KEY_ADV_LEGACY_BOOT_BASE) &&
         //
         // for the shift deivice down, the maximum number of deiveces that we can shift
         // is (BOOT_DEVICE_BASE + MAX_BOOT_DEVICES_NUMBER - 2)
         //
        (Data->EntryCount < KEY_ADV_LEGACY_BOOT_BASE + MAX_BOOT_DEVICES_NUMBER - 1) &&
        (Data->EntryCount - KEY_ADV_LEGACY_BOOT_BASE + 1) < LegacyBootDeviceNum) {

      Status = Shiftitem (
                 This,
                 SUCInfo->LegacyAdvanceTokenRecord,
                 (Data->EntryCount - KEY_ADV_LEGACY_BOOT_BASE),
                 FALSE,
                 MyIfrNVData
                 );
      Index = MAX_BOOT_ORDER_NUMBER + 2;
    }

    for (Index; Index < BbsDevTableTypeCount; Index++) {
    	 if ( (Data->EntryCount >= mBbsTypeTable[TempBootType[Index]].KeyBootDeviceBase) &&
            //
            // for the shift deivice down, the maximum number of deiveces that we can shift
            // is (BOOT_DEVICE_BASE + MAX_BOOT_DEVICES_NUMBER - 2)
            //
    	      (Data->EntryCount < ((UINTN) (mBbsTypeTable[TempBootType[Index]].KeyBootDeviceBase + MAX_BOOT_TYPE_DEVICES - 1))) &&
            ((Data->EntryCount - mBbsTypeTable[TempBootType[Index]].KeyBootDeviceBase + 1) < ((UINT32)(mBbsTypeTable[TempBootType[Index]].DeviceTypeCount)))) {

         Status = Shiftitem (
                    This,
                    SUCInfo->LegacyNormalTokenRecord[TempBootType[Index]],
                    (Data->EntryCount - mBbsTypeTable[TempBootType[Index]].KeyBootDeviceBase),
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
    //
    // Temporary!!
    // if the SetupBroswer is OK, remove these codes
    //
    SuBrowser->SCBuffer->BootMenuType         = MyIfrNVData->BootMenuType;
    SuBrowser->SCBuffer->BootNormalPriority   = MyIfrNVData->BootNormalPriority;
    SuBrowser->SCBuffer->LegacyNormalMenuType = MyIfrNVData->LegacyNormalMenuType;
    if (KeyValue == KEY_NORMAL_BOOT_PRIORITY) {
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

  default:
    Status = SuBrowser->HotKeyCallback (
                          This,
                          SuBrowser->SUCInfo->AdvancedHiiHandle,
                          KeyValue,
                          Data,
                          &UsePacket,
                          Packet
                          );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    if ( OemServices != NULL ) {
      Status = OemServices->Funcs[COMMON_SCU_CALLBACK_FUNCTION] (
                              OemServices,
                              COMMON_SCU_CALLBACK_FUNCTION_ARG_COUNT,
                              This,
                              KeyValue,
                              (EFI_IFR_DATA_ARRAY *) Data,
                              Packet,
                              MyIfrNVData,
                              &(SuBrowser->SUCInfo->AdvancedHiiHandle),
                              &UsePacket,
                              SuBrowser->Browser,
                              NULL
                              );
    }
    break;
  }
  return Status;
}

EFI_STATUS
InstallBootCallbackRoutine (
  IN EFI_HII_HANDLE       HiiHandle
  )
/*++

Routine Description:

  Install Boot Callback routine.

Arguments:

  HiiHandle   - Hii hanlde for the call back routine

Returns:

  EFI_SUCCESS  -  Function has completed successfully.
  Others       -  Error occurred during execution.

--*/
{
  EFI_CALLBACK_INFO                         *BootCallBackInfo;
  EFI_HII_UPDATE_DATA                       UpdateData;
  EFI_STATUS                                Status;
  EFI_HII_PROTOCOL                          *Hii;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Hii              = SuBrowser->Hii;
  BootCallBackInfo = EfiLibAllocatePool (sizeof (EFI_CALLBACK_INFO));
  if (BootCallBackInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  BootCallBackInfo->Signature               = EFI_BOOT_MENU_SIGNATURE;
  BootCallBackInfo->DriverCallback.NvRead   = SuBrowser->NvRead;
  BootCallBackInfo->DriverCallback.NvWrite  = SuBrowser->NvWrite;
  BootCallBackInfo->DriverCallback.Callback = BootCallbackRoutine;

  //
  // Install protocol interface
  //
  BootCallBackInfo->CallbackHandle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &BootCallBackInfo->CallbackHandle,
                  &gEfiFormCallbackProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &BootCallBackInfo->DriverCallback
                  );
  ASSERT_EFI_ERROR (Status);
  EfiZeroMem (&UpdateData, sizeof(EFI_HII_UPDATE_DATA));

  UpdateData.FormSetUpdate      = TRUE;
  UpdateData.FormCallbackHandle = (EFI_PHYSICAL_ADDRESS) BootCallBackInfo->CallbackHandle;
  BootCallBackInfo->HiiHandle = HiiHandle;
  Hii->UpdateForm (
         Hii,
         HiiHandle,
         0,
         TRUE,
         &UpdateData
         );

  Status = InitBootMenu (HiiHandle);

  return Status;
}

