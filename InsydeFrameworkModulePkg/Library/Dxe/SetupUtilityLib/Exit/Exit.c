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
//;   Exit.c
//;
//; Abstract:
//;
//;   Main funcitons for Eixt menu
//;

#include "Exit.h"
#include EFI_PROTOCOL_DEFINITION (OemServices)

EFI_STATUS
ExitCallbackRoutine (
  IN  EFI_FORM_CALLBACK_PROTOCOL            *This,
  IN  UINT16                                KeyValue,
  IN  EFI_IFR_DATA_ARRAY                    *Data,
  OUT EFI_HII_CALLBACK_PACKET               **Packet
  )
/*++

Routine Description:

  This is the callback function for the Exit Menu.

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
  SYSTEM_CONFIGURATION                      *MyIfrNVData;
  EFI_HII_PROTOCOL                          *Hii;
  EFI_HII_HANDLE                            HiiHandle;
  EFI_INPUT_KEY                             Key;
  EFI_CALLBACK_INFO                         *CallbackInfo;
  EFI_FORM_BROWSER_PROTOCOL                 *Browser;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL        *Interface;
  OEM_SERVICES_PROTOCOL                     *OemServices;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CallbackInfo            = EFI_EXIT_CALLBACK_INFO_FROM_THIS (This);
  Browser                 = SuBrowser->Browser;
  SUCInfo                 = SuBrowser->SUCInfo;
  Status                  = EFI_SUCCESS;
  UsePacket               = FALSE;
  StringPtr               = NULL;
  Hii                     = SuBrowser->Hii;
  HiiHandle               = CallbackInfo->HiiHandle;
  Interface               = &SuBrowser->Interface;
  Interface->MyIfrNVData  = Data->NvRamMap;
  MyIfrNVData             = (SYSTEM_CONFIGURATION *) Interface->MyIfrNVData;

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

  case KEY_SAVE_EXIT:
    Status = SuBrowser->HotKeyCallback (
                          This,
                          SuBrowser->SUCInfo->AdvancedHiiHandle,
                          KEY_SCAN_F10,
                          Data,
                          &UsePacket,
                          Packet
                          );
    break;

  case KEY_EXIT_DISCARD:
    Status = SuBrowser->HotKeyCallback (
                          This,
                          SuBrowser->SUCInfo->AdvancedHiiHandle,
                          KEY_SCAN_ESC,
                          Data,
                          &UsePacket,
                          Packet
                          );
    break;

  case KEY_LOAD_OPTIMAL:
    //
    //Select "Load Optimal Defaults" and Press "Load Optimal hotkey", the functionality is the same,
    //so remove the original code Use "Load Optimal hotkey" to call HotKeyCallBack () directly
    //
    Status = SuBrowser->HotKeyCallback (
                          This,
                          SuBrowser->SUCInfo->AdvancedHiiHandle,
                          KEY_SCAN_F9,
                          Data,
                          &UsePacket,
                          Packet
                          );
    break;

  case KEY_LOAD_CUSTOM:
    //
    // Load custom setup.
    //
    if ((MyIfrNVData->SetUserPass == TRUE) &&
        ((MyIfrNVData->UserAccessLevel == 2) ||
         (MyIfrNVData->UserAccessLevel == 3))) {
       gBS->FreePool (*Packet);
       *Packet = NULL;
    	 return EFI_SUCCESS;
    }
    StringPtr = SetupUtilityLibGetTokenString (
                  HiiHandle,
                    STRING_TOKEN (STR_LOAD_CUSTOM_DEFAULTS_STRING)
                  );
    Browser->CreatePopUp (
               0,
               FALSE,
               0,
               NULL,
               &Key,
               StringPtr
               );
    if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      Status = LoadCustomOption (This);
      if (!EFI_ERROR (Status)) {
        SUCInfo->DoRefresh = TRUE;
        Interface->Firstin = TRUE;
      }
    }

    gBS->FreePool (StringPtr);
    break;

  case KEY_SAVE_CUSTOM:
    //
    // Save custom setup.
    //
    StringPtr = SetupUtilityLibGetTokenString (
                    HiiHandle,
                    STRING_TOKEN (STR_SAVE_CUSTOM_DEFAULTS_STRING)
                  );
    Browser->CreatePopUp (
               0,
               FALSE,
               0,
               NULL,
               &Key,
               StringPtr
               );
    if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      Status = SaveCustomOption (This);
    }

    gBS->FreePool (StringPtr);
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

  if (!UsePacket && Packet != NULL) {

    gBS->FreePool (*Packet);
    *Packet = NULL;
  }

  return Status;
}



EFI_STATUS
InstallExitCallbackRoutine (
  IN EFI_HII_HANDLE                        HiiHandle
  )
/*++

Routine Description:

  Install Exit Callback routine.

Arguments:

  HiiHandle   - Hii hanlde for the call back routine

Returns:

  EFI_SUCCESS  -  Function has completed successfully.
  Others       -  Error occurred during execution.

--*/
{
  EFI_CALLBACK_INFO                         *ExitCallBackInfo;
  EFI_HII_UPDATE_DATA                       *UpdateData;
  EFI_STATUS                                Status;
  EFI_HII_PROTOCOL                          *Hii;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Hii = SuBrowser->Hii;
  ExitCallBackInfo = EfiLibAllocatePool (sizeof (EFI_CALLBACK_INFO));
  if (ExitCallBackInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  ExitCallBackInfo->Signature               = EFI_EXIT_MENU_SIGNATURE;
  ExitCallBackInfo->DriverCallback.NvRead   = SuBrowser->NvRead;
  ExitCallBackInfo->DriverCallback.NvWrite  = SuBrowser->NvWrite;
  ExitCallBackInfo->DriverCallback.Callback = ExitCallbackRoutine;

  //
  // Install protocol interface
  //
  ExitCallBackInfo->CallbackHandle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &ExitCallBackInfo->CallbackHandle,
                  &gEfiFormCallbackProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &ExitCallBackInfo->DriverCallback
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Allocate space for creation of UpdateData Buffer
  //
  UpdateData = EfiLibAllocateZeroPool (0x1000);
  ASSERT (UpdateData);
  //
  // Flag update pending in FormSet
  //
  UpdateData->FormSetUpdate = TRUE;
  //
  // Register CallbackHandle data for FormSet
  //
  UpdateData->FormCallbackHandle = (EFI_PHYSICAL_ADDRESS) ExitCallBackInfo->CallbackHandle;
  UpdateData->FormUpdate = FALSE;
  ExitCallBackInfo->HiiHandle = HiiHandle;
  //
  // Simply registering the callback handle
  //
  Hii->UpdateForm (
         Hii,
         HiiHandle,
         (EFI_FORM_LABEL) 0x0000,
         TRUE,
         UpdateData
         );

  gBS->FreePool (UpdateData);
  return Status;
}

EFI_STATUS
InitExitMenu (
  IN EFI_HII_HANDLE                         HiiHandle
)
/*++

Routine Description:

  Initialize exit menu for setuputility use

Arguments:

  HiiHandle   - Hii hanlde for the call back routine

Returns:

  EFI_SUCCESS  -  Function has completed successfully.
  Others       -  Error occurred during execution.

--*/
{
  EFI_STATUS                                Status = EFI_SUCCESS;

  return Status;
}



