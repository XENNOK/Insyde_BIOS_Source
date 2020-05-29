/** @file
  This file include all BDS platform recovery flash functions.

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

#include "RecoveryFlash.h"
#include <Library/VariableSupportLib.h>
#include "SecureFlash.h"
#include <Library/LockBoxLib.h>


EFI_INPUT_KEY mKeyList[] = {
                SCAN_UP,   CHAR_NULL,
                SCAN_DOWN, CHAR_NULL,
                SCAN_ESC,  CHAR_NULL,
                SCAN_NULL, CHAR_CARRIAGE_RETURN,
                SCAN_NULL, CHAR_NULL
                };


UINT8 *mAuthVarBackupDataBuffer = NULL;
EFI_RESET_SYSTEM                mOriginalResetSystemPtr;

BOOLEAN
RecoveryCapsuleIsExecutable (
  IN EFI_PEI_HOB_POINTERS      *RecoveryHob
  );

EFI_STATUS
ExecuteRecoveryCapsule (
  IN EFI_PEI_HOB_POINTERS      *RecoveryHob
  );

VOID
EFIAPI
ResetSystemDoNothing (
  IN EFI_RESET_TYPE             ResetType,
  IN EFI_STATUS                 ResetStatus,
  IN UINTN                      DataSize,
  IN VOID                       *ResetData OPTIONAL
  )
{
  return;
}

VOID
HookResetSystem (
  IN BOOLEAN                    ToHook
  )
{

  if (ToHook) {
    mOriginalResetSystemPtr = gRT->ResetSystem;
    gRT->ResetSystem = ResetSystemDoNothing;
  } else {
    gRT->ResetSystem = mOriginalResetSystemPtr;
  }
}

/**
  Restore authenticated variables from the buffer mAuthVarBackupDataBuffer.

**/
VOID
RestoreAuthVariable (
  VOID
  )
{
  EFI_FIRMWARE_VOLUME_HEADER    *OnboardNvStoreageFvHeader;
  VARIABLE_HEADER               *VariableHeader;
  UINTN                         SkipHeaderSize;
  UINT32                        FlashSize;
  UINT32                        FlashAddress;


  if (mAuthVarBackupDataBuffer == NULL) {
    return ;
  }

  OnboardNvStoreageFvHeader = (EFI_FIRMWARE_VOLUME_HEADER *)(UINTN)PcdGet32 (PcdFlashNvStorageVariableBase);
  SkipHeaderSize = (UINTN)OnboardNvStoreageFvHeader->HeaderLength + GetVariableStoreHeaderSize ();

  //
  // The current variable region should be empty after done Crisis Recovery Flash.
  // If the variable region with any data, not to restore AuthVariable data to prevent variable region from breaking.
  // But it shall not happen.
  //
  VariableHeader = NULL;
  VariableHeader = (VARIABLE_HEADER *)((UINTN)PcdGet32 (PcdFlashNvStorageVariableBase) + SkipHeaderSize);
  if (IsValidVariableHeader (VariableHeader)) {
    gBS->FreePool (mAuthVarBackupDataBuffer);
    return ;
  }

  //
  // Copy new BIOS firmware volume and variable store header information to mAuthVarBackupDataBuffer.
  //
  CopyMem (mAuthVarBackupDataBuffer, OnboardNvStoreageFvHeader, SkipHeaderSize);
  FlashSize = PcdGet32 (PcdFlashNvStorageVariableSize);
  FlashAddress = (UINT32)(UINTN)OnboardNvStoreageFvHeader;
  FlashWrite (mAuthVarBackupDataBuffer, FlashSize, FlashAddress, FLASH_SMI_PORT);

}

/**
  Backup authenticated variables to the buffer mAuthVarBackupDataBuffer.

**/
VOID
BackupAuthVariable (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_FIRMWARE_VOLUME_HEADER    *OnboardNvStoreageFvHeader;
  VARIABLE_HEADER               *VariableHeader;
  VARIABLE_HEADER               *NextVariable;
  UINTN                         VariableSize;
  UINTN                         LastVariableOffset;
  UINTN                         SkipHeaderSize;


  OnboardNvStoreageFvHeader = (EFI_FIRMWARE_VOLUME_HEADER *)(UINTN)PcdGet32 (PcdFlashNvStorageVariableBase);
  SkipHeaderSize = (UINTN)OnboardNvStoreageFvHeader->HeaderLength + GetVariableStoreHeaderSize ();

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  PcdGet32 (PcdFlashNvStorageVariableSize),
                  (VOID **) &mAuthVarBackupDataBuffer
                  );
  gBS->SetMem (mAuthVarBackupDataBuffer, PcdGet32 (PcdFlashNvStorageVariableSize), 0xFF);

  LastVariableOffset = 0;
  VariableSize = 0;
  NextVariable = NULL;
  VariableHeader = (VARIABLE_HEADER *)((UINTN)PcdGet32 (PcdFlashNvStorageVariableBase) + SkipHeaderSize);
  while (IsValidVariableHeader (VariableHeader)) {
    NextVariable = GetNextVariablePtr (VariableHeader);
    if (VariableHeader->State == VAR_ADDED || VariableHeader->State == (VAR_ADDED & VAR_IN_DELETED_TRANSITION)) {
      if (((VariableHeader->Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) == EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) ||
          ((VariableHeader->Attributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) == EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)){
        VariableSize = (UINTN)NextVariable - (UINTN)VariableHeader;
        if (LastVariableOffset + VariableSize < PcdGet32 (PcdFlashNvStorageVariableSize)) {
          CopyMem (
            &mAuthVarBackupDataBuffer[LastVariableOffset] + SkipHeaderSize,
            VariableHeader,
            VariableSize
            );
        }
        LastVariableOffset += VariableSize;
      }
    }
    VariableHeader = NextVariable;
  }

}

VOID
CreateSetupModeVariable (
  VOID
  )
{
  EFI_STATUS                            Status;
  UINT8                                 SetupMode;
  UINTN                                 Size;


  Size = sizeof (UINT8);
  Status = gRT->GetVariable (
                  EFI_SETUP_MODE_NAME,
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &Size,
                  &SetupMode
                  );
  if (Status == EFI_NOT_FOUND) {
    SetupMode = 0;
    Status = gRT->SetVariable (
                    EFI_SETUP_MODE_NAME,
                    &gEfiGlobalVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    sizeof (UINT8),
                    &SetupMode
                    );
  }
}

/**
  Prints a formatted unicode string to the default console, at
  the supplied cursor position

  @param  Column         The column of  cursor position to print the string at
  @param  Row            The row of  cursor position to print the string at
  @param  Fmt            Format string

  @return Length of string printed to the console

**/
UINTN
PrintAt (
  IN UINTN     Column,
  IN UINTN     Row,
  IN CHAR16    *Fmt,
  ...
  )
{
  CHAR16      *Buffer;
  UINTN       StrLen;
  VA_LIST     Marker;


  Buffer = AllocateZeroPool (0x10000);
  ASSERT (Buffer);
  if (Column != (UINTN) - 1) {
    gST->ConOut->SetCursorPosition (gST->ConOut, Column, Row);
  }

  VA_START (Marker, Fmt);
  StrLen = UnicodeVSPrint (Buffer, 0x10000, Fmt, Marker);
  VA_END (Marker);

  if (gST->ConOut != NULL) {
    //
    // To be extra safe make sure ConOut has been initialized
    //
    gST->ConOut->OutputString (gST->ConOut, Buffer);
  }
  gBS->FreePool (Buffer);

  return StrLen;
}

/**
  Display the Recovery flash user interface to user to select

  @param  FlashMode     Input flash mode (DEFAULT_FLASH_DEVICE_TYPE, SPI_FLASH_DEVICE_TYPE, OTHER_FLASH_DEVICE_TYPE)

  @retval EFI_SUCCESS   Success
  @retval Other         Error

**/
EFI_STATUS
RecoveryPopUp (
  IN  UINTN     FlashMode
  )
{
  CHAR16                                *TitleString;
  CHAR16                                *FlashInfoStringArray[5];
  UINT32                                Index;
  EFI_INPUT_KEY                         Key;
  H2O_DIALOG_PROTOCOL                   *H2ODialog;
  EFI_STATUS                            Status;
  UINTN                                 Size;
  UINT8                                 Data;

  Data = 0;
  Size = sizeof (UINT8);
  Status = gRT->GetVariable (
                  FIRMWARE_FAILURE_FLAG_NAME,
                  &gEfiGenericVariableGuid,
                  NULL,
                  &Size,
                  &Data
                  );
  if (Data == FIRMWARE_FAILURE_FLAG_DATA) {
    //
    // If it is recovery from firmware failure, update firmware directly without user interface
    //
    RecoveryFlash (FlashMode);
    return EFI_SUCCESS;
  }

  Status = gBS->LocateProtocol (
                  &gH2ODialogProtocolGuid,
                  NULL,
                  (VOID **) &H2ODialog
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Intialize local variable and string for use
  //
  Index = 0;
  TitleString = BdsLibGetStringById (STRING_TOKEN (STR_RECOVERY_FLASH_TITLE));
  FlashInfoStringArray[0] = BdsLibGetStringById (STRING_TOKEN (STR_RECOVERY_FLASH_YES));
  FlashInfoStringArray[1] = BdsLibGetStringById (STRING_TOKEN (STR_RECOVERY_FLASH_RESETSYSTEM));
  FlashInfoStringArray[2] = BdsLibGetStringById (STRING_TOKEN (STR_RECOVERY_FLASH_NO));
  FlashInfoStringArray[3] = 0x00;

  gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_WHITE, EFI_BLACK));
  while (TRUE) {
    //
    // Print recovery flash message
    //
    H2ODialog->OneOfOptionDialog (
                         3,
                         FALSE,
                         NULL,
                         &Key,
                         60,
                         TitleString,
                         &Index,
                         (CHAR16 **) (FlashInfoStringArray),
                         0
                         );

    if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      //
      // Adjust the item number, there are 3 choices.
      //
      if (Index == 0) {
        RecoveryFlash (FlashMode);
      } else if (Index == 1) {
        ResetCommand ();
      } else {
        break;
      }
    }
  }
  gST->ConOut->ClearScreen (gST->ConOut);
  //
  // Free allocated strings
  //
  gBS->FreePool (TitleString);
  gBS->FreePool (FlashInfoStringArray[0]);
  gBS->FreePool (FlashInfoStringArray[1]);
  gBS->FreePool (FlashInfoStringArray[2]);
  gBS->FreePool (FlashInfoStringArray[3]);

  return EFI_SUCCESS;
}


/**
  Send reset request to reset system

  @retval EFI_SUCCESS

**/
EFI_STATUS
ResetCommand (
  VOID
  )
{
  FlashComplete (IHISI_REBOOT, FLASH_SMI_PORT);

  return EFI_SUCCESS;
}


/**
  Update the precentage of recovery flash progress in dialog

  @param  PercentageValue    The finished percentage of flash process

**/
VOID
Drawpercentage (
  IN   UINTN        PercentageValue
  )
{
  UINTN                 Columns;
  UINTN                 ColumnPosition;
  UINTN                 Rows;
  UINTN                 RowPosition;


  gST->ConOut->QueryMode (gST->ConOut, gST->ConOut->Mode->Mode, &Columns, &Rows);
  RowPosition = Columns / 2;
  ColumnPosition = Rows / 2;
  PrintAt (RowPosition, ColumnPosition, L"%d%%", PercentageValue);
}

/**
  Drawing dialog for showing recovery flash progress

**/
VOID
DrawDialogBlock (
  VOID
  )
{
  CHAR16                CleanLine[80];
  CHAR16                *StatusString;
  CHAR16                SelectIndexLin[80];
  UINTN                 Color;
  UINTN                 Columns;
  UINTN                 Index;
  UINTN                 Item;
  UINTN                 Rows;
  UINTN                 StrLenth;

  Color    =  0;
  Item     =  1;
  StrLenth = 20;

  //
  // Set the background
  //
  gST->ConOut->QueryMode (gST->ConOut, gST->ConOut->Mode->Mode, &Columns, &Rows);
  Color = EFI_TEXT_ATTR (EFI_LIGHTGRAY, EFI_BLUE);
  gST->ConOut->SetAttribute (gST->ConOut, Color);
  StrLenth = StrLenth / 2;
  for (Index = 0; Index < (StrLenth + 2) * 2 + 1; Index++) {
    CleanLine[Index] = 0x20;
    SelectIndexLin[Index] = 0x20;
  }
  CleanLine[(StrLenth + 2) * 2 + 1] = 0x0000;
  SelectIndexLin[1 + 1] = 0x0000;
  Item = Item / 2;
  for (Index = Rows / 2 - (Item + 3); Index <= Rows / 2 + (Item + 1); Index++) {
    PrintAt (Columns / 2 - (StrLenth + 2), Index, L"%s", CleanLine);
  }

  //
  // Next three step will draw a dialog
  // 1.draw three row line
  // 2.draw two side
  // 3.draw four corner
  //

  //
  // This is draw three row line
  //
  for (Index = Columns / 2 - (StrLenth + 1); Index < Columns / 2 + (StrLenth + 2) ; Index++) {
    PrintAt (Index, Rows / 2 - (Item + 3), L"%c", BOXDRAW_HORIZONTAL);
    PrintAt (Index, Rows / 2 - (Item + 1), L"%c", BOXDRAW_HORIZONTAL);
    PrintAt (Index, Rows / 2 + (Item + 1), L"%c", BOXDRAW_HORIZONTAL);
  }

  //
  // Draw two side
  //
  for (Index = Rows / 2 - (Item + 2); Index < Rows / 2 + (Item + 1); Index++) {
    PrintAt (Columns / 2 - (StrLenth + 2), Index, L"%c", BOXDRAW_VERTICAL);
    PrintAt (Columns / 2 + (StrLenth + 2), Index, L"%c", BOXDRAW_VERTICAL);
  }

  //
  // This is draw the dialog four corner
  //
  PrintAt (Columns / 2 + (StrLenth + 2) ,Rows / 2 + (Item + 1), L"%c", BOXDRAW_UP_LEFT);
  PrintAt (Columns / 2 + (StrLenth + 2) ,Rows / 2 - (Item + 3), L"%c", BOXDRAW_DOWN_LEFT);
  PrintAt (Columns / 2 - (StrLenth + 2), Rows / 2 + (Item + 1), L"%c", BOXDRAW_UP_RIGHT);
  PrintAt (Columns / 2 - (StrLenth + 2), Rows / 2 - (Item + 3), L"%c", BOXDRAW_DOWN_RIGHT);

  //
  // Print the title and flash status percentage
  //
  StatusString = BdsLibGetStringById (STRING_TOKEN (STR_RECOVERY_FLASH_STATUS));
  PrintAt (Columns / 2 - (StrLen (StatusString) / 2), Rows / 2 - (Item + 2), L"%s", StatusString);
  gBS->FreePool (StatusString);
}

/**
  The entry point to doing recovery flash

  @param[in]  FlashMode     Input flash mode (DEFAULT_FLASH_DEVICE_TYPE, SPI_FLASH_DEVICE_TYPE, OTHER_FLASH_DEVICE_TYPE)

**/
VOID
RecoveryFlash (
  IN  UINTN     FlashMode
  )
{
  UINT8                     *BufferTmp;
  UINT8                     FlashDevice;
  UINT8                     *FlashTmp;
  UINT8                     CommTmp;
  UINT8                     *MapTmp;
  UINT8                     Index;
  CHAR16                    *RebootString;
  UINT32                    FlashAddress;
  UINT32                    FlashSize;
  UINTN                     Columns;
  UINTN                     CompareTemp;
  UINTN                     FlashPrecentage;
  UINTN                     IndexCounter;
  UINTN                     Item;
  UINTN                     PEIBaseTemp[20];
  UINTN                     Rows;
  UINTN                     WriteSize;
  VOID                      *HobList;
  EFI_PEI_HOB_POINTERS      RecoveryHob;
  EFI_STATUS                Status;
  UINTN                     TotalFlashSectors;
  UINTN                     FirmwareSize;
  BOOLEAN                   RecoveryFlag;


  POST_CODE (BDS_RECOVERY_START_FLASH);

  //
  // Initial local variable
  //
  Item        = 0;
  WriteSize   = 0;
  CommTmp     = 0xFF;
  Index       = 0;
  CompareTemp = 0;
  FlashTmp    = NULL;
  MapTmp      = NULL;
  FlashDevice = (UINT8) FlashMode;

  BackupAuthVariable ();

  ZeroMem (PEIBaseTemp, 20);
  gST->ConOut->ClearScreen (gST->ConOut);

  //
  // Get the PEI phase .FD file memory base addess Hob ()
  //
  HobList = GetHobList ();
  RecoveryHob.Raw = HobList;
  RecoveryHob.Raw = GetNextHob (EFI_HOB_TYPE_MEMORY_ALLOCATION, RecoveryHob.Raw);
  while (RecoveryHob.Header->HobType == EFI_HOB_TYPE_MEMORY_ALLOCATION &&
         !CompareGuid (&RecoveryHob.MemoryAllocationModule->MemoryAllocationHeader.Name,
         &gEfiRecoveryFileAddressGuid)) {
    RecoveryHob.Raw = GET_NEXT_HOB (RecoveryHob);
    RecoveryHob.Raw = GetNextHob (EFI_HOB_TYPE_MEMORY_ALLOCATION, RecoveryHob.Raw);
  }

  HookResetSystem (TRUE);
  RecoveryFlag = FALSE;
  if (RecoveryCapsuleIsExecutable (&RecoveryHob)) {
    //
    // Set this flag to hook IHISI 16h(FbtsComplete), force it to return here,
    // so, we can restore the authenticated variables.
    //
    RecoveryFlag = TRUE;
    Status = SaveLockBox (&gSecureFlashInfoGuid, &RecoveryFlag, sizeof (RecoveryFlag));
    ExecuteRecoveryCapsule (&RecoveryHob);
    RestoreAuthVariable ();
    HookResetSystem (FALSE);
    gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
  }
  BufferTmp =(UINT8 *) (UINTN)RecoveryHob.MemoryAllocationModule->MemoryAllocationHeader.MemoryBaseAddress;
  FirmwareSize = (UINTN) RecoveryHob.MemoryAllocationModule->MemoryAllocationHeader.MemoryLength;

  //
  // Detect flash device
  //
  Status = gBS->AllocatePool (EfiBootServicesData, 0x100, (VOID **) &FlashTmp);
  Status = gBS->AllocatePool (EfiBootServicesData, 0x100, (VOID **) &MapTmp);
  gBS->SetMem (FlashTmp, 0x100, 0);
  gBS->SetMem (MapTmp, 0x100, 0);
  CommTmp = FlashPartInfo (FlashTmp, MapTmp, FlashDevice, FLASH_SMI_PORT);
  FlashAddress = 0xFFFFFFFF - (UINT32)FirmwareSize + 1;
  WriteSize = *(UINT16 *)(&(MapTmp[0]));

  if (FLASH_BLOCK_SIZE == WriteSize * SMI_FLASH_UNIT_BYTES) {
    FlashSize = FLASH_BLOCK_SIZE;
  } else {
    FlashSize = FLASH_SECTOR_SIZE;
  }

  //
  // Each block size is 0x1000 or 0x10000 and Call IHISI to flash ROM part
  //
  DrawDialogBlock ();
  TotalFlashSectors = FirmwareSize / FlashSize;
  for (IndexCounter = 0; IndexCounter < TotalFlashSectors; IndexCounter++) {
    CommTmp = FlashWrite (BufferTmp, FlashSize, FlashAddress, FLASH_SMI_PORT);
    FlashAddress += FlashSize;
    BufferTmp += FlashSize;
    FlashPrecentage = (IndexCounter + 1) * 100 / TotalFlashSectors;
    Drawpercentage (FlashPrecentage);
  }

  RestoreAuthVariable ();
  HookResetSystem (FALSE);

  gST->ConOut->QueryMode (gST->ConOut, gST->ConOut->Mode->Mode, &Columns, &Rows);
  RebootString = BdsLibGetStringById (STRING_TOKEN (STR_RECOVERY_FLASH_REBOOT));
  PrintAt (Columns / 2 - 4, Rows / 2 - Item + 0, L"%s", RebootString);
  gBS->FreePool (RebootString);
  for (IndexCounter = 0; IndexCounter < 10000; IndexCounter++) {
    gBS->Stall (300);
  }
  ResetCommand ();
}

