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
//;   Main.c
//;
//; Abstract:
//;
//;   Main funcitons for Main menu
//;

#include "Main.h"
#include "OemPnpStrings.h"

STATIC
EFI_STATUS
FindUpdateString (
  IN UINT8                              Type,
  IN UINT8                              Offset,
  IN EFI_HII_HANDLE                     HiiHandle,
  IN STRING_REF                         TokenToUpdate
  )
/*++

Routine Description:

  Update used DMI information from FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE
  , if the used data is in this region.

Arguments:

  Type           - SMBIOS type number
  Offset         - The offset of the string reference
  HiiHandle      - Target EFI_HII_HANDLE instance
  TokenToUpdate  - Updated token number

Returns:

  EFI_SUCESS - Udpate string token successfule
  Other      - Cannot find HII protocol

--*/
{
#if (defined(FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_SIZE)) && \
    (FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_SIZE > 9)
  EFI_STATUS                   Status;
  CHAR16                       *UniString;
  UINTN                        Index;
  EFI_HII_PROTOCOL             *Hii;
  UINT8                        *DmiPtr;
  UINT8                        *Buffer;
  DMI_STRING_STRUCTURE         *CurrentPtr;

  Hii = NULL;
  //
  // There should only be one HII protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiHiiProtocolGuid,
                  NULL,
                  &Hii
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  DmiPtr = (UINT8*) (UINTN) FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_BASE;
  if (!(*(UINT32*) DmiPtr == DMI_UPDATA_STRING_SIGNATURE)){
    return EFI_NOT_FOUND;
  }

  Buffer = (UINT8*) (UINTN) (FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_BASE + 4);
  CurrentPtr = NULL;
  while (TRUE) {
    CurrentPtr = (DMI_STRING_STRUCTURE*) Buffer;
    if ((UINTN) Buffer >=
    (FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_BASE +
    FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_SIZE)
    ) {
      return EFI_NOT_FOUND;
    }
    if (CurrentPtr->Type == 0xFF) {
      return EFI_NOT_FOUND;
    }
    if (CurrentPtr->Type == Type &&
        CurrentPtr->Offset == Offset &&
        CurrentPtr->Valid == 0xFF) {
      break;
    }
    Buffer = Buffer + CurrentPtr->Length;
  }

  UniString = EfiLibAllocateZeroPool ((CurrentPtr->Length-5)*2 + 2);
  //
  // UINT8 transform CHAR16
  //
  for (Index = 0; Index + 5 < CurrentPtr->Length; Index++) {
    UniString[Index] = (CHAR16)CurrentPtr->String[Index];
  }
  UniString[Index] = 0;

  Status = Hii->NewString (Hii, NULL, HiiHandle, &TokenToUpdate, UniString);

  gBS->FreePool (UniString);

  return Status;
#else
  return EFI_NOT_FOUND;
#endif
}

STATIC
EFI_STATUS
UpdateBiosVersionFromBvdt (
  IN EFI_HII_HANDLE     HiiHandle
  )
/*++

Routine Description:

  Using BVDT data to update STR_MISC_BIOS_VERSION string token

Arguments:

  HiiHandle - Target EFI_HII_HANDLE instance

Returns:

  EFI_SUCESS - Udpate string token successfule
  Other      - Cannot find HII protocol

--*/
{
  STRING_REF           TokenToUpdate;
  UINTN                StrSize;
  CHAR16               Str[BVDT_MAX_STR_SIZE];
  EFI_STATUS           Status;
  EFI_HII_PROTOCOL     *Hii;

  Status = gBS->LocateProtocol (
                  &gEfiHiiProtocolGuid,
                  NULL,
                  &Hii
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  TokenToUpdate = STRING_TOKEN(STR_MISC_BIOS_VERSION);
  StrSize = BVDT_MAX_STR_SIZE;
  Status = GetBvdtInfo ((BVDT_TYPE) BvdtBiosVer, &StrSize, Str);
  if (!EFI_ERROR(Status)) {
    Hii->NewString (Hii, NULL, HiiHandle, &TokenToUpdate, Str);
  }
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
UpdateLanguage (
  IN EFI_HII_HANDLE                        HiiHandle
  )
/*++

Routine Description:

  Update the language item with all of support languages for main menu use.

Arguments:

  HiiHandle   - Hii hanlde for the call back routine

Returns:

  EFI_SUCCESS  -  Function has completed successfully.
  Others       -  Error occurred during execution.

--*/
{
  UINT16                                *LanguageString;
  UINT16                                Lang[4];
  UINT16                                QuestionId;
  UINT16                                *StringBuffer;
  UINTN                                 BufferSize;
  UINTN                                 Index;
  UINTN                                 OptionCount;
  EFI_HII_PROTOCOL                      *Hii;
  EFI_HII_UPDATE_DATA                   *UpdateData;
  EFI_STATUS                            Status;
  IFR_OPTION                            *OptionList;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  STRING_REF                            Token;
  SYSTEM_CONFIGURATION                  SetupNvData;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Hii = SuBrowser->Hii;
  LanguageString = NULL;

  SetupUtilityLibGetLangDatabase (&BufferSize, &LanguageString);

  OptionCount = 0;
  OptionList  = EfiLibAllocateZeroPool (BufferSize * sizeof (IFR_OPTION));

  //
  // Try for a 512 byte Buffer
  //
  BufferSize = 0x200;

  //
  // Allocate memory for our Form binary
  //
  StringBuffer = EfiLibAllocateZeroPool (BufferSize);
  UpdateData = EfiLibAllocateZeroPool (0x1000);

  for (Index = 0; LanguageString[Index] != 0; Index += 3) {
    Token = 0;
    EfiCopyMem (Lang, &LanguageString[Index], 6);
    Lang[3] = 0;

    BufferSize = 0x200;
    Status = Hii->GetString(
                    Hii,
                    HiiHandle,
                    1,
                    TRUE,
                    Lang,
                    &BufferSize,
                    StringBuffer
                    );
    Hii->NewString (
           Hii,
           NULL,
           HiiHandle,
           &Token,
           StringBuffer
           );
    OptionList[OptionCount].StringToken = (UINT16) Token;
    OptionList[OptionCount].Value       = (UINT16) OptionCount;
    OptionList[OptionCount].Key         = KEY_LANGUAGE_UPDATE;
    OptionList[OptionCount].Flags       = EFI_IFR_FLAG_INTERACTIVE | EFI_IFR_FLAG_NV_ACCESS;
    OptionCount++;
  }

  OptionList[0].Flags |= EFI_IFR_FLAG_DEFAULT;
  gBS->FreePool (LanguageString);

  QuestionId = (UINT16) ((UINTN) (&SetupNvData.Language) - (UINTN) (&SetupNvData));
  Status = CreateOneOfOpCode (
             QuestionId,
             1,
             (STRING_REF) STRING_TOKEN (STR_LANGUAGE_PROMPT),
             (STRING_REF) STRING_TOKEN (STR_LANGUAGE_HELP),
             OptionList,
             OptionCount,
             &UpdateData->Data
             );

  //
  // Assign the number of options and the oneof and endoneof op-codes to count
  //
  UpdateData->DataCount = (UINT8) (OptionCount + 2);
  Hii->UpdateForm(
         Hii,
         HiiHandle,
         (EFI_FORM_LABEL) SETUP_UTILITY_LANG_MENU,
         TRUE,
         UpdateData
         );

  gBS->FreePool (UpdateData);
  gBS->FreePool (OptionList);
  gBS->FreePool (StringBuffer);
  return Status;
}

STATIC
EFI_STATUS
InitMainMenu (
  IN EFI_HII_HANDLE                         HiiHandle
  )
/*++

Routine Description:

  Initialize main menu for setuputility use

Arguments:

  HiiHandle   - Hii hanlde for the call back routine

Returns:

  EFI_SUCCESS  -  Function has completed successfully.
  Others       -  Error occurred during execution.

--*/
{
  EFI_STATUS                                Status;
  OEM_SERVICES_PROTOCOL                     *OemServices = NULL;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (
                  &gOemServicesProtocolGuid,
                  NULL,
                  &OemServices
                  );
  if (!EFI_ERROR (Status)) {
    Status = OemServices->Funcs[COMMON_UPDATE_SCU_SYSTEM_INFO] (
                            OemServices,
                            COMMON_UPDATE_SCU_SYSTEM_INFO_ARG_COUNT,
                            (SYSTEM_CONFIGURATION *) SuBrowser->SCBuffer,
                            SuBrowser->Hii,
                            &HiiHandle
                            );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  UpdateBiosVersionFromBvdt (HiiHandle);

  Status = FindUpdateString (
             0,
             0x05,
             HiiHandle,
             ((STRING_REF) STRING_TOKEN (STR_MISC_BIOS_VERSION))
             );
  //
  // Update System Information
  //
  UpdateSystemInfo (
    SuBrowser->Hii,
    HiiHandle
    );
  //
  // Update current language
  //
  UpdateLanguage (HiiHandle);

  return Status;
}

EFI_STATUS
UpdateStringToken (
  IN SYSTEM_CONFIGURATION                  *IfrNVData
  )
/*++

Routine Description:

  Update the content of string tokens while changing language.

Arguments:

  IfrNVData -  Point to FSYSTEM_CONFIGURATION instance.

Returns:

  EFI_SUCCESS  -  Update string token successful.
  Other        -  Cannot find SETUP_UTILITY_BROWSER_DATA instance.


--*/
{
  EFI_HII_PROTOCOL                      *Hii;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  EFI_STATUS                            Status;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Hii     = SuBrowser->Hii;
  SUCInfo = SuBrowser->SUCInfo;

  UpdatePasswordState (Hii, SUCInfo->SecurityHiiHandle);
  SetupUtilityLibUpdateDeviceString (SUCInfo->BootHiiHandle, IfrNVData);

  return EFI_SUCCESS;
}

EFI_STATUS
MainCallbackRoutine (
  IN EFI_FORM_CALLBACK_PROTOCOL       *This,
  IN UINT16                           KeyValue,
  IN EFI_IFR_DATA_ARRAY               *Data,
  OUT EFI_HII_CALLBACK_PACKET         **Packet
  )
/*++

Routine Description:

  This is the Main page callback function that is called to provide results data to the driver.
  This dataconsists of a unique key which is used to identify what data is either being passed
  back or being asked for.

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
  EFI_STATUS                            Status;
  BOOLEAN                               UsePacket;
  CHAR16                                *StringPtr;
  SYSTEM_CONFIGURATION                  *MyIfrNVData;
  EFI_HII_PROTOCOL                      *Hii;
  EFI_HII_HANDLE                        HiiHandle;
  EFI_CALLBACK_INFO                     *CallbackInfo;
  OEM_SERVICES_PROTOCOL                 *OemServices;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;

  CallbackInfo = EFI_MAIN_CALLBACK_INFO_FROM_THIS (This);

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }


  MyIfrNVData  = (SYSTEM_CONFIGURATION *) Data->NvRamMap;
  Status    = EFI_SUCCESS;
  UsePacket = FALSE;
  StringPtr = NULL;
  Status = gBS->LocateProtocol (
                  &gOemServicesProtocolGuid,
                  NULL,
                  &Hii
                  );
  HiiHandle = CallbackInfo->HiiHandle;
  if (Packet != NULL) {
    Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    sizeof (EFI_HII_CALLBACK_PACKET) + 2,
                    Packet
                    );

    EfiZeroMem(
      *Packet,
      sizeof (EFI_HII_CALLBACK_PACKET) + 2
      );

    (*Packet)->DataArray.EntryCount = 1;
    (*Packet)->DataArray.NvRamMap   = NULL;
  }
  OemServices = NULL;
  Status = gBS->LocateProtocol (
                  &gOemServicesProtocolGuid,
                  NULL,
                  &OemServices
                  );

  switch (KeyValue) {

  case KEY_LANGUAGE_UPDATE:
    Status = UpdateLangItem (This, (UINT8 *) &MyIfrNVData->Language);
    SuBrowser->Interface.Firstin = TRUE;
    SuBrowser->Interface.JumpToFirstOption = FALSE;
    UpdateStringToken (MyIfrNVData);
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
    if (OemServices != NULL) {
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
InstallMainCallbackRoutine (
  IN EFI_HII_HANDLE                         HiiHandle
  )
/*++

Routine Description:

  Install Main Callback routine.

Arguments:

  HiiHandle   - Hii hanlde for the call back routine

Returns:

  EFI_SUCCESS  -  Function has completed successfully.
  Others       -  Error occurred during execution.

--*/
{
  EFI_CALLBACK_INFO                         *MainCallBackInfo;
  EFI_HII_UPDATE_DATA                       UpdateData;
  EFI_STATUS                                Status;
  EFI_HII_PROTOCOL                          *Hii;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Hii = SuBrowser->Hii;

  MainCallBackInfo = EfiLibAllocatePool (sizeof (EFI_CALLBACK_INFO));
  if (MainCallBackInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  MainCallBackInfo->Signature               = EFI_MAIN_MENU_SIGNATURE;
  MainCallBackInfo->DriverCallback.NvRead   = SuBrowser->NvRead;
  MainCallBackInfo->DriverCallback.NvWrite  = SuBrowser->NvWrite;
  MainCallBackInfo->DriverCallback.Callback = MainCallbackRoutine;

  //
  // Install protocol interface
  //
  MainCallBackInfo->CallbackHandle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &MainCallBackInfo->CallbackHandle,
                  &gEfiFormCallbackProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &MainCallBackInfo->DriverCallback
                  );
  ASSERT_EFI_ERROR (Status);

  EfiZeroMem (&UpdateData, sizeof (EFI_HII_UPDATE_DATA));
  //
  // Flag update pending in FormSet
  //
  UpdateData.FormSetUpdate      = TRUE;
  //
  // Register CallbackHandle data for FormSet
  //
  UpdateData.FormCallbackHandle = (EFI_PHYSICAL_ADDRESS) MainCallBackInfo->CallbackHandle;
  MainCallBackInfo->HiiHandle   = HiiHandle;

  Hii->UpdateForm (
         Hii,
         HiiHandle,
         0,
         TRUE,
         &UpdateData
         );

  Status = InitMainMenu (HiiHandle);

  return Status;
}

