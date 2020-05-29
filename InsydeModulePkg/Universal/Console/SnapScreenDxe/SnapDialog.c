/** @file
  Serial driver for standard UARTS on an ISA bus.

Copyright (c) 2006 - 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#include "SnapDialog.h"
#include "SnapLib.h"
#include "SnapWin.h"

//#include EFI_PROTOCOL_DEFINITION (FileInfo)
//#include EFI_PROTOCOL_DEFINITION (FileSystemInfo)
//#include EFI_PROTOCOL_DEFINITION (SimpleFileSystem)

#include <Guid/FileInfo.h>
#include <Guid/FileSystemInfo.h>
#include <Protocol/SimpleFileSystem.h>


#define DIALOG_NORMAL_ATTR    EFI_TEXT_ATTR (EFI_LIGHTGRAY, EFI_BLUE)
#define DIALOG_ACCENT_ATTR    EFI_TEXT_ATTR (EFI_WHITE, EFI_CYAN)

#define MSG_DIALOG_HEIGHT       3

#define MAX_DISK_NAME_SIZE      256

#define MAX_DIALOG_ITEM         10

#define FS_ITEM_SIGNATURE       SIGNATURE_32('D','I','A','i')
#define FS_ITEM_FROM_LINK(a)    CR(a, FS_ITEM, Link, SIGNATURE_32('D','I','A','i'));

//
// Global variable
//
FS_DIALOG   gFsDialog;

CHAR16  *gDiskTypeStr[] = {
  L" IDE",
  L"SCSI",
  L" USB",
  L"AHCI",
  L"UNKN"
};

CHAR16 *
MakeFsName (
  UINTN         AIndex,
  DISK_TYPE     DiskType,
  CHAR16        *ADiskName,
  CHAR16        *AVolumeName,
  CHAR16        *AVolumeSize
  )
{
  CHAR16        *FsName;
  CHAR16        *FsSeparate = L":";
  CHAR16        *DsSeparate = L"\\";
  CHAR16        FsIndexStr[3] = L" 1";
  CHAR16        DiskName[13] = L"            ";
  CHAR16        VolumeName[12] = L"           ";
  CHAR16        VolumeSize[12] = L"       MB  ";
  CHAR16        *DiskTypeStr;
  UINTN         FsNameSize;
  UINTN         Index;
  UINTN         Len;
  UINTN         FsNameLen;
  

  FsIndexStr[1] = (CHAR16) (FsIndexStr[1] + AIndex);

  DiskTypeStr = gDiskTypeStr[DiskType];
  
  if (ADiskName != NULL) {
    StrnCpy (DiskName ,ADiskName, 12);
  }
  
  if (AVolumeName != NULL) {
    StrnCpy (VolumeName, AVolumeName, 11);
  }

  if (AVolumeSize != NULL) {
    Len = StrLen (AVolumeSize);
    
    if (Len > 9) {
      Len = 9;
    } 
    
    Index = 9 - Len;
    StrnCpy(&VolumeSize[Index], AVolumeSize, 9-Index);
  }

  // 1: AHCI\JetFlash    \MURPHY_128M 120.5 MB 
  // 2: IDE \ST300845AS  \WINDOW2008      1 TB
  FsNameLen = 2+ 4 + 12 + 11 + 11 + 3;
  FsNameSize = (FsNameLen + 1) * sizeof(CHAR16);

  FsName =  AllocatePool(FsNameSize);
  for (Index=0; Index < FsNameLen; Index++)
    FsName[Index] = L' ';

  StrCpy (FsName, FsIndexStr);     // FS1
  StrCat (FsName, FsSeparate);     // FS1:
  StrCat (FsName, DiskTypeStr);    // FS1: USB
  StrCat (FsName, DsSeparate);     // FS1: USB/
  StrCat (FsName, DiskName);       // FS1: USB/JetFlash
  StrCat (FsName, DsSeparate);     // FS1: USB/JetFlash     /
  StrCat (FsName, VolumeName);     // FS1: USB/JetFlash     /MURPHY_128M
  
  Len = StrLen (FsName);
  for (Index = Len; Index < 32; Index++)
    FsName[Index] = L' ';
  FsName[Index] = 0;
  
  StrCat (FsName, VolumeSize);     // FS1: USB/JetFlash     /MURPHY_128M 120.8 MB
  
  return FsName;
}

EFI_STATUS
TFsDialogAddItem (
  FS_DIALOG     *FsDialog,
  EFI_HANDLE    FsHandle
  )
{
  EFI_STATUS    Status;
  EFI_HANDLE    ParentHandle;
  DISK_TYPE     DiskType = DT_UNKNOW;
  CHAR16        *DiskName = NULL;
  CHAR16        *VolumnName = NULL;
  CHAR16        *VolumnSize = NULL;
  FS_ITEM       *FsItem;
  
  Status = GetParentHandle (FsHandle, &ParentHandle);
  if (EFI_ERROR(Status))
    return EFI_UNSUPPORTED;

  if (IsRecordableDevice (ParentHandle) == FALSE)
    return EFI_UNSUPPORTED;
  
  if (Status == EFI_SUCCESS) {
    DiskType = GetDiskInfoType (ParentHandle);
    DiskName = GetDiskInfoName (ParentHandle);
  }
  else {
    ParentHandle = NULL;
  }

  VolumnName = GetVolumnName ( FsHandle);
  VolumnSize = GetVolumnSizeAsString ( FsHandle);

  FsItem = (FS_ITEM *) AllocateZeroPool (sizeof(FS_ITEM));
  
  if (FsItem != NULL) { 
    FsItem->Signature = FS_ITEM_SIGNATURE;
    FsItem->FsHandle = FsHandle;
    FsItem->ParentHandle = ParentHandle;
    FsItem->Title = MakeFsName (FsDialog->ItemCount, DiskType, DiskName, VolumnName, VolumnSize);
    InsertTailList(&FsDialog->ItemHead, &FsItem->Link);
    FsDialog->ItemCount++;
    Status = EFI_SUCCESS;
  }
  else {
    Status = EFI_OUT_OF_RESOURCES;
  }
  
  if (DiskName != NULL)
    gBS->FreePool (DiskName);

  if (VolumnName != NULL)
    gBS->FreePool (VolumnName);

  if (VolumnSize != NULL)
    gBS->FreePool (VolumnSize);

  return Status;
}
  
EFI_STATUS
TFsDialogCreate (
  UINT16                          X,
  UINT16                          Y,
  CHAR16                          *Title,
  DIALOG_PROC                     DialogProc,
  FS_DIALOG                       **AFsDialog
  )
{
  EFI_STATUS      Status;
  EFI_HANDLE      *HandleBuffer;
  UINTN           HandleCount;
  UINTN           Index;
  UINTN           Len;
  UINTN           MaxLen = 0;
  FS_DIALOG       *FsDialog;
  FS_ITEM         *FsItem;
  LIST_ENTRY      *Link;

  Status = gBS->LocateHandleBuffer (
                            ByProtocol, 
                            &gEfiSimpleFileSystemProtocolGuid,
                            NULL,
                            &HandleCount,
                            &HandleBuffer
                          );

  if (EFI_ERROR(Status))
    return Status;

  FsDialog = (FS_DIALOG *) AllocateZeroPool (sizeof(FS_DIALOG));

  InitializeListHead(&FsDialog->ItemHead);
  
  FsDialog->DialogProc = DialogProc;

  // Add Item to FsDialog
  for (Index=0; Index < HandleCount; Index++) {
  
    Status = TFsDialogAddItem (FsDialog, HandleBuffer[Index]);
    
    if (FsDialog->ItemCount >= MAX_DIALOG_ITEM)
      break;
  }

  if (FsDialog->ItemCount == 0) {
    gBS->FreePool (FsDialog);
    return EFI_NOT_FOUND;
  }

  // looking for max string length
  Link = FsDialog->ItemHead.ForwardLink;
  while (Link != &FsDialog->ItemHead) {
  
    FsItem = FS_ITEM_FROM_LINK(Link);
    Len = StrLen (FsItem->Title);
    
    if (Len > MaxLen)
      MaxLen = Len;
      
    Link = Link->ForwardLink;
  }

  // initialize default item as selection
  FsDialog->Index = 0;
  FsDialog->CurrLink = FsDialog->ItemHead.ForwardLink;

  // Create a window object
  Status = TWinCreate (
                        X, 
                        Y, 
                        (UINT16) MaxLen, 
                        (UINT16) FsDialog->ItemCount, 
                        TRUE, 
                        Title,
                        &FsDialog->Win
                       );
                       
  if (EFI_ERROR(Status)) {
    TFsDialogDestroy (FsDialog);
  }
  else {
    *AFsDialog = FsDialog;
  }
  
  gBS->FreePool (HandleBuffer);

  return Status;
}

VOID
TFsDialogPrintItem (
  FS_DIALOG   *Dialog,
  UINTN       Index,
  UINTN       Attribute
  )
{
  LIST_ENTRY        *Link;
  FS_ITEM           *FsItem;
  UINT16            Y;

  if (Index >= Dialog->ItemCount)
    return;

  Y = (UINT16) Index;
  
  Link = Dialog->ItemHead.ForwardLink;
  
  while (Index > 0) {
    Link = Link->ForwardLink;
    Index--;
  }

  FsItem = FS_ITEM_FROM_LINK (Link);

  TWinSetAttribue (Dialog->Win, Attribute);
  TWinPrintAt (Dialog->Win,  0, Y, L"%s", FsItem->Title);

  
}

EFI_STATUS
TFsDialogExec (
  FS_DIALOG   *Dialog,
  EFI_HANDLE  *Result
  )
{
  UINTN             Index;
  TWINDOW           *Win;
  EFI_INPUT_KEY     EfiKey;
  INT16             OldIndex;
  FS_ITEM           *FsItem;

  Win = Dialog->Win;

  TWinShow (Win);

  // Print all item
  for (Index = 0; Index < Dialog->ItemCount; Index++) {
    TFsDialogPrintItem (Dialog, Index, DIALOG_NORMAL_ATTR);
  }

  TFsDialogPrintItem (Dialog, Dialog->Index, DIALOG_ACCENT_ATTR);

  OldIndex = Dialog->Index;
  
  while (TRUE) {
    GetKey(&EfiKey);

    switch (EfiKey.ScanCode) {
      case SCAN_UP :
        Dialog->Index--;
        Dialog->CurrLink = Dialog->CurrLink->BackLink;
        if (Dialog->Index < 0) {
          Dialog->Index = (INT16)(Dialog->ItemCount - 1);
          Dialog->CurrLink = Dialog->ItemHead.BackLink;
        }
        break;

      case SCAN_DOWN :
        Dialog->Index++;
        Dialog->CurrLink = Dialog->CurrLink->ForwardLink;
        if (Dialog->Index >= Dialog->ItemCount) {
          Dialog->Index = 0;
          Dialog->CurrLink = Dialog->ItemHead.ForwardLink;
        }
        break;

      case SCAN_ESC : 
        return EFI_ABORTED;

      default :
        if (EfiKey.UnicodeChar == CHAR_CARRIAGE_RETURN) {
        
          FsItem = FS_ITEM_FROM_LINK(Dialog->CurrLink);
          *Result = FsItem->FsHandle;
          
          return EFI_SUCCESS;
        }
    }

    if (Dialog->Index != OldIndex) {
      TFsDialogPrintItem (Dialog, OldIndex, DIALOG_NORMAL_ATTR);
      TFsDialogPrintItem (Dialog, Dialog->Index, DIALOG_ACCENT_ATTR);
      OldIndex = Dialog->Index;
    }

  }
  
}

EFI_STATUS
TFsDialogDestroy (
  FS_DIALOG   *FsDialog
  )
{
  LIST_ENTRY        *Link;
  FS_ITEM           *FsItem;

  Link = FsDialog->ItemHead.ForwardLink;
  
  while (Link != &FsDialog->ItemHead)  {

    FsItem = FS_ITEM_FROM_LINK(Link);

    Link = Link->ForwardLink;

    RemoveEntryList (&(FsItem->Link));

    if (FsItem->Title != NULL)
      gBS->FreePool (FsItem->Title);

    gBS->FreePool (FsItem);
  }

  TWinDestroy (FsDialog->Win);

  gBS->FreePool (FsDialog);
  
  return EFI_SUCCESS;
}

EFI_STATUS
TMsgDialogCreate (
  CHAR16        *MessageStr,
  MSG_DIALOG    **AMsgDialog
  )
{
  TWINDOW       *Win;
  MSG_DIALOG    *MsgDialog;
  UINT16        Width;
  UINT16        Height;
  UINTN         BufferSize;
  
  Width = (UINT16)(StrLen(MessageStr) + 2);
  Height = MSG_DIALOG_HEIGHT;

  BufferSize = sizeof (MSG_DIALOG) + Width * sizeof(CHAR16);
  MsgDialog = (MSG_DIALOG *) AllocateZeroPool (BufferSize);

  MsgDialog->Message = (CHAR16 *)((UINT8 *)MsgDialog + sizeof (MSG_DIALOG));

  StrCpy (MsgDialog->Message, MessageStr);
  
  TWinCreate(WIN_SCR_CENTER, WIN_SCR_CENTER, Width, Height, TRUE, L"Message", &Win);

  MsgDialog->Win = Win;

  *AMsgDialog = MsgDialog;

  return EFI_SUCCESS;
}

EFI_STATUS
TMsgDialogShow (
  MSG_DIALOG      *MsgDialog
  )
{
  if (MsgDialog->IsShow == TRUE)
    return EFI_SUCCESS;

  MsgDialog->IsShow = TRUE;
  
  TWinShow (MsgDialog->Win);
  TWinPrintAt ( MsgDialog->Win, 1, 1, L"%s", MsgDialog->Message);

  return EFI_SUCCESS;
}

EFI_STATUS
TMsgDialogRun (
  MSG_DIALOG      *MsgDialog
  )
{
  EFI_INPUT_KEY   Key;

  if (MsgDialog->IsShow == FALSE) {
    TMsgDialogShow (MsgDialog);
  }
  
  GetKey(&Key);

  return EFI_SUCCESS;
}

EFI_STATUS
TMsgDialogDestroy (
  MSG_DIALOG      *MsgDialog
  )
{
  if (MsgDialog != NULL)
    gBS->FreePool (MsgDialog);
  
  TWinDestroy (MsgDialog->Win);
  
  return EFI_SUCCESS;
}

EFI_STATUS
PopUpMessage (
  CHAR16    *MessageStr
  )
{
  EFI_STATUS      Status;
  MSG_DIALOG      *MsgDialog;

  Status = TMsgDialogCreate ( MessageStr, &MsgDialog);
  if (EFI_ERROR(Status))
    return Status;

  TMsgDialogRun( MsgDialog);

  Status = TMsgDialogDestroy (MsgDialog);
  
  return Status;
}

