// ## @file
// #
// #******************************************************************************
// #* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
// #*
// #* You may not reproduce, distribute, publish, display, perform, modify, adapt,
// #* transmit, broadcast, present, recite, release, license or otherwise exploit
// #* any part of this publication in any form, by any means, without the prior
// #* written permission of Insyde Software Corporation.
// #*
// #******************************************************************************

#include "D01PlatformSkuDialog.h"

EFI_EVENT   Event;
EFI_GUID    gEfiD01VariableGuid = {0x0d9c223f, 0x5635, 0x488d, {0x86, 0x71, 0xf5, 0x0b, 0x59, 0x39, 0x51, 0x85}};

D01_SKU_INFO mSkuInfo[] = {
  { 0x01 , L" Sku 1 "},
  { 0x02 , L" Sku 2 "},
  { 0x03 , L" Sku 3 "},
  { 0x04 , L" Sku 4 "}
};
/**
  To show Dialog and store information. 

  @retval EFI_SUCCESS        Function was executed.
**/
EFI_STATUS
EFIAPI
D01SkuDialogEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_INPUT_KEY         Key;
  UINTN                 Index;
  EFI_STATUS            Status;
  UINTN                 ItemNum;
  UINTN                 DataSize;
  H2O_DIALOG_PROTOCOL   *H2ODialog;
  UINT16                PlatformSku;
  CHAR16                **DialogItemInfoStringPtr;
  EFI_INPUT_KEY         KeyList[] = {{SCAN_UP,   CHAR_NULL},
                                     {SCAN_DOWN, CHAR_NULL},
                                     {SCAN_NULL, CHAR_CARRIAGE_RETURN},
                                     {SCAN_NULL, CHAR_NULL}
                                     };

  CHAR16 *Msg;

  gBS->AllocatePool(EfiBootServicesData, 4, &Msg);

  H2ODialog = NULL;
  Status = gBS->LocateProtocol (&gH2ODialogProtocolGuid, NULL, (VOID **)&H2ODialog);
  if (Status != EFI_SUCCESS) {
    
    Print(L"LocateProtocol\n");
    gBS->Stall(10000000);
    return Status;
  }

  //
  // If variable has been exist, it means not first time boot.
  //
  DataSize = sizeof (UINT16);
  Status = CommonGetVariable (D01_PLATFORM_SKU_REC, &gEfiD01VariableGuid, &DataSize, &PlatformSku);
  if (!EFI_ERROR(Status)) {
    Msg[0] = (CHAR16)(PlatformSku + 48);
    Msg[1] = 0;

    H2ODialog->ShowPageInfo(L"Value", Msg);

    return EFI_SUCCESS;
  } else {
    Print(L"%r\n", Status);
    gBS->Stall(5000000);      // 5 second
  }

  ItemNum = sizeof (mSkuInfo)/sizeof (mSkuInfo[0]);
  if (ItemNum == 0) {
    
    Print(L"ItemNum\n");
    gBS->Stall(10000000);
    return EFI_SUCCESS;
  }
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  ItemNum * sizeof(CHAR16 *),
                  (VOID *)&DialogItemInfoStringPtr
                  );
  if (EFI_ERROR(Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < ItemNum; Index++) {
    DialogItemInfoStringPtr[Index] = mSkuInfo[Index].ItemString;
  }

  Index = 0;
  //Select option
  while (TRUE) {
    Key.ScanCode    = SCAN_NULL;
    Key.UnicodeChar = CHAR_NULL;

    gST->ConOut->SetCursorPosition (gST->ConOut, 0, 0);

    Status = H2ODialog->OneOfOptionDialog (
                          (UINT32)ItemNum,
                          TRUE,
                          KeyList,
                          &Key,
                          STRING_MAX_SIZE,
                          L"Select Platform Sku",
                          (UINT32 *)&Index,
                          DialogItemInfoStringPtr,
                          0
                          );
    if (Key.UnicodeChar != CHAR_CARRIAGE_RETURN) {
      switch (Key.ScanCode) {
      case SCAN_UP:
          if (Index == 0) {
            Index = ItemNum - 1;
          } else {
            Index--;
          }
          break;
      case SCAN_DOWN:
          if (Index == (UINTN) (ItemNum - 1)) {
            Index = 0;
          } else {
            Index++;
          }
          break;
      default:
          break;
      }
      continue;
    }

    break;
  }

  //Store value which option is selected in variable 
  DataSize = sizeof(UINT16);
  Status = CommonSetVariable (
             D01_PLATFORM_SKU_REC,
             &gEfiD01VariableGuid,
             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
             DataSize,
             &mSkuInfo[Index].Value
             );

  for (Index = 0; Index < ItemNum; Index++) {
    gBS->FreePool(DialogItemInfoStringPtr[Index]);
  }

  if (!EFI_ERROR(Status)) {
    gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
  }

  return Status;
}

/**
  Register Checkpoint for gH2OBdsCpConnectAllBeforeGuid to show dialog.

  @param[in] ImageHandle      The firmware allocated handle for the EFI image.
  @param[in] SystemTable      A pointer to the EFI System Table.

  @retval EFI_SUCCESS         Success to register checkpoint
  @retval EFI_ABORTED         Failed to register ready to boot event
  @retval EFI_UNSUPPORTED     Feature is not supported by the FW
**/
EFI_STATUS
EFIAPI
D01SkuDialogEntryPoint (
  VOID
  )
{
  EFI_STATUS    Status;
  Event = NULL;
  
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  (EFI_EVENT_NOTIFY)D01SkuDialogEvent,
                  NULL,
                  &Event
                  );
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  Status = gBS->SignalEvent(Event);
  
  return Status; 
}