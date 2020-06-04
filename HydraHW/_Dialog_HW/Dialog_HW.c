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

#include "Dialog_HW.h"


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
DialogEntryPoint (
  IN EFI_HANDLE                        ImageHandle,
  IN EFI_SYSTEM_TABLE                 *SystemTable
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

  EFI_GUID                  TargetGuid ;
  CHAR16                    *VariName = {L"HydraSetUpVari"};
  TargetGuid.Data1 = 0xd300c7b8;
  TargetGuid.Data2 = 0x7bf8;
  TargetGuid.Data3 = 0x4f7f;
  TargetGuid.Data4[0] = 0x92;
  TargetGuid.Data4[1] = 0x03;
  TargetGuid.Data4[2] = 0x9e;
  TargetGuid.Data4[3] = 0xff;
  TargetGuid.Data4[4] = 0x18;
  TargetGuid.Data4[5] = 0x09;
  TargetGuid.Data4[6] = 0x17;
  TargetGuid.Data4[7] = 0x24;
  //
  // If variable has been exist, it means not first time boot.
  //
  DataSize = sizeof (UINT8);
  Status = CommonGetVariable (VariName, &TargetGuid, &DataSize, &PlatformSku);
  if (EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }

  H2ODialog = NULL;
  Status = gBS->LocateProtocol (&gH2ODialogProtocolGuid, NULL, (VOID **)&H2ODialog);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  ItemNum = sizeof (mSkuInfo)/sizeof (mSkuInfo[0]);
  if (ItemNum == 0) {
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
  // DataSize = sizeof(UINT8);
  // Status = CommonSetVariable (
  //            D01_PLATFORM_SKU_REC,
  //            &HydraSetUpVari,
  //            EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
  //            DataSize,
  //            &mSkuInfo[Index].Value
  //            );

  for (Index = 0; Index < ItemNum; Index++) {
    gBS->FreePool(DialogItemInfoStringPtr[Index]);
  }

  // if (!EFI_ERROR(Status)) {
  //   gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
  // }

  return Status;
}