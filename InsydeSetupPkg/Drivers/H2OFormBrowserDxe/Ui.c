/** @file
Utility functions for User Interface functions.
;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

Copyright (c) 2004 - 2012, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "InternalH2OFormBrowser.h"

LIST_ENTRY                   mMenuOption;
LIST_ENTRY                   mMenuList;
MENU_REFRESH_ENTRY           *mMenuRefreshHead;                // Menu list used for refresh timer opcode.

/**
  Create a menu with specified FormSet GUID and Form ID, and add it as a child
  of the given parent menu.

  @param  [in,out] Parent             The parent of menu to be added.
  @param  [in] HiiHandle              HII handle related to this FormSet.
  @param  [in] FormSetGuid            The FormSet Guid of menu to be added.
  @param  [in] FormId                 The Form ID of menu to be added.

  @return A pointer to the newly added menu or NULL if memory is insufficient.

**/
UI_MENU_LIST *
UiAddMenuList (
  IN OUT UI_MENU_LIST     *Parent,
  IN EFI_HII_HANDLE       HiiHandle,
  IN EFI_GUID             *FormSetGuid,
  IN EFI_FORM_ID          FormId
  )
{
  UI_MENU_LIST  *MenuList;

  MenuList = AllocateZeroPool (sizeof (UI_MENU_LIST));
  if (MenuList == NULL) {
    return NULL;
  }

  MenuList->Signature = UI_MENU_LIST_SIGNATURE;
  InitializeListHead (&MenuList->ChildListHead);

  MenuList->HiiHandle = HiiHandle;
  CopyMem (&MenuList->FormSetGuid, FormSetGuid, sizeof (EFI_GUID));
  MenuList->FormId = FormId;
  MenuList->Parent = Parent;

  if (Parent == NULL) {
    //
    // If parent is not specified, it is the root Form of a FormSet
    //
    InsertTailList (&mMenuList, &MenuList->Link);
  } else {
    InsertTailList (&Parent->ChildListHead, &MenuList->Link);
  }

  return MenuList;
}


/**
  Search the menu with the specified Form ID, FormSet GUID and handle in the entire menu list.

  @param  [in] Parent                 The parent of menu to search.
  @param  [in] Handle                 HII handle related to this FormSet.
  @param  [in] FormSetGuid            The FormSet GUID of the menu to search.
  @param  [in] FormId                 The Form ID of menu to search.

  @return A pointer to menu found or NULL if not found.

**/
UI_MENU_LIST *
UiFindChildMenuList (
  IN UI_MENU_LIST         *Parent,
  IN EFI_HII_HANDLE       Handle,
  IN EFI_GUID             *FormSetGuid,
  IN EFI_FORM_ID          FormId
  )
{
  LIST_ENTRY      *Link;
  UI_MENU_LIST    *Child;
  UI_MENU_LIST    *MenuList;

  ASSERT (Parent != NULL);

  if (Parent->FormId == FormId && CompareGuid (FormSetGuid, &Parent->FormSetGuid) && Parent->HiiHandle == Handle) {
    return Parent;
  }

  Link = GetFirstNode (&Parent->ChildListHead);
  while (!IsNull (&Parent->ChildListHead, Link)) {
    Child = UI_MENU_LIST_FROM_LINK (Link);

    MenuList = UiFindChildMenuList (Child, Handle, FormSetGuid, FormId);
    if (MenuList != NULL) {
      return MenuList;
    }

    Link = GetNextNode (&Parent->ChildListHead, Link);
  }

  return NULL;
}


/**
  Search Menu with given Handle, FormSetGuid and FormId in all cached menu list.

  @param [in] FormSetGuid            The FormSet GUID of the menu to search.
  @param [in] Handle                 HII handle related to this FormSet.
  @param [in] FormId                 The Form ID of menu to search.

  @return A pointer to menu found or NULL if not found.

**/
UI_MENU_LIST *
UiFindMenuList (
  IN EFI_HII_HANDLE       Handle,
  IN EFI_GUID             *FormSetGuid,
  IN EFI_FORM_ID          FormId
  )
{
  LIST_ENTRY      *Link;
  UI_MENU_LIST    *MenuList;
  UI_MENU_LIST    *Child;

  Link = GetFirstNode (&mMenuList);
  while (!IsNull (&mMenuList, Link)) {
    MenuList = UI_MENU_LIST_FROM_LINK (Link);

    Child = UiFindChildMenuList(MenuList, Handle, FormSetGuid, FormId);
    if (Child != NULL) {

      //
      // If this Form is already in the menu history list,
      // just free the menu list .
      //
      UiFreeMenuList(&Child->ChildListHead);
      return Child;
    }

    Link = GetNextNode (&mMenuList, Link);
  }

  return NULL;
}

/**
  Free Menu list linked list.

  @param  [in] MenuListHead    One Menu list point in the menu list.

**/
VOID
UiFreeMenuList (
  IN LIST_ENTRY   *MenuListHead
  )
{
  UI_MENU_LIST    *MenuList;

  while (!IsListEmpty (MenuListHead)) {
    MenuList = UI_MENU_LIST_FROM_LINK (MenuListHead->ForwardLink);
    RemoveEntryList (&MenuList->Link);

    UiFreeMenuList(&MenuList->ChildListHead);
    FreePool (MenuList);
  }

}

/**
  Find HII Handle in the HII database associated with given Device Path.

  If DevicePath is NULL, then ASSERT.

  @param [in]  DevicePath        Device Path associated with the HII package list
                                 handle.

  @retval Handle                 HII package list Handle associated with the Device
                                        Path.
  @retval NULL                   HII Package list handle is not found.

**/
STATIC
EFI_HII_HANDLE
DevicePathToHiiHandle (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  )
{
  EFI_STATUS                  Status;
  EFI_DEVICE_PATH_PROTOCOL    *TmpDevicePath;
  UINTN                       BufferSize;
  UINTN                       HandleCount;
  UINTN                       Index;
  EFI_HANDLE                  Handle;
  EFI_HANDLE                  DriverHandle;
  EFI_HII_HANDLE              *HiiHandles;
  EFI_HII_HANDLE              HiiHandle;

  ASSERT (DevicePath != NULL);

  TmpDevicePath = DevicePath;
  //
  // Locate Device Path Protocol handle buffer
  //
  Status = gBS->LocateDevicePath (
                  &gEfiDevicePathProtocolGuid,
                  &TmpDevicePath,
                  &DriverHandle
                  );
  if (EFI_ERROR (Status) || !IsDevicePathEnd (TmpDevicePath)) {
    return NULL;
  }

  //
  // Retrieve all HII Handles from HII database
  //
  BufferSize = 0x1000;
  HiiHandles = AllocatePool (BufferSize);
  ASSERT (HiiHandles != NULL);
  if (HiiHandles == NULL) {
    return NULL;
  }
  Status = gHiiDatabase->ListPackageLists (
                           gHiiDatabase,
                           EFI_HII_PACKAGE_TYPE_ALL,
                           NULL,
                           &BufferSize,
                           HiiHandles
                           );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    FreePool (HiiHandles);
    HiiHandles = AllocatePool (BufferSize);
    ASSERT (HiiHandles != NULL);
    if (HiiHandles == NULL) {
      return NULL;
    }

    Status = gHiiDatabase->ListPackageLists (
                             gHiiDatabase,
                             EFI_HII_PACKAGE_TYPE_ALL,
                             NULL,
                             &BufferSize,
                             HiiHandles
                             );
  }

  if (EFI_ERROR (Status)) {
    FreePool (HiiHandles);
    return NULL;
  }

  //
  // Search for HII Handle by Driver Handle
  //
  HiiHandle = NULL;
  HandleCount = BufferSize / sizeof (EFI_HII_HANDLE);
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gHiiDatabase->GetPackageListHandle (
                             gHiiDatabase,
                             HiiHandles[Index],
                             &Handle
                             );
    if (!EFI_ERROR (Status) && (Handle == DriverHandle)) {
      HiiHandle = HiiHandles[Index];
      break;
    }
  }

  FreePool (HiiHandles);
  return HiiHandle;
}

/**
  Find HII Handle in the HII database associated with given Form Set guid.

  If FormSetGuid is NULL, then ASSERT.

  @param [in] ComparingGuid      FormSet Guid associated with the HII package list
                                 handle.

  @return Handle                 HII package list Handle associated with the Device
                                        Path.
  @return NULL                   Hii Package list handle is not found.

**/
STATIC
EFI_HII_HANDLE
FormSetGuidToHiiHandle (
  EFI_GUID     *ComparingGuid
  )
{
  EFI_HII_HANDLE               *HiiHandles;
  UINTN                        Index;
  EFI_HII_PACKAGE_LIST_HEADER  *HiiPackageList;
  UINTN                        BufferSize;
  UINT32                       Offset;
  UINT32                       Offset2;
  UINT32                       PackageListLength;
  EFI_HII_PACKAGE_HEADER       PackageHeader;
  UINT8                        *Package;
  UINT8                        *OpCodeData;
  EFI_STATUS                   Status;
  EFI_HII_HANDLE               HiiHandle;

  ASSERT (ComparingGuid != NULL);
  if (ComparingGuid == NULL) {
    return NULL;
  }

  HiiHandle  = NULL;
  //
  // Get all the HII handles
  //
  HiiHandles = HiiGetHiiHandles (NULL);
  ASSERT (HiiHandles != NULL);
  if (HiiHandles == NULL) {
    return NULL;
  }
  //
  // Search for FormSet of each class type
  //
  for (Index = 0; HiiHandles[Index] != NULL; Index++) {
    BufferSize = 0;
    HiiPackageList = NULL;
    Status = gHiiDatabase->ExportPackageLists (gHiiDatabase, HiiHandles[Index], &BufferSize, HiiPackageList);
    if (Status == EFI_BUFFER_TOO_SMALL) {
      HiiPackageList = AllocatePool (BufferSize);
      ASSERT (HiiPackageList != NULL);

      Status = gHiiDatabase->ExportPackageLists (gHiiDatabase, HiiHandles[Index], &BufferSize, HiiPackageList);
    }
    if (EFI_ERROR (Status) || HiiPackageList == NULL) {
      return NULL;
    }

    //
    // Get Form package from this HII package List
    //
    Offset = sizeof (EFI_HII_PACKAGE_LIST_HEADER);
    Offset2 = 0;
    CopyMem (&PackageListLength, &HiiPackageList->PackageLength, sizeof (UINT32));

    while (Offset < PackageListLength) {
      Package = ((UINT8 *) HiiPackageList) + Offset;
      CopyMem (&PackageHeader, Package, sizeof (EFI_HII_PACKAGE_HEADER));

      if (PackageHeader.Type == EFI_HII_PACKAGE_FORMS) {
        //
        // Search FormSet in this Form Package
        //
        Offset2 = sizeof (EFI_HII_PACKAGE_HEADER);
        while (Offset2 < PackageHeader.Length) {
          OpCodeData = Package + Offset2;

          if (((EFI_IFR_OP_HEADER *) OpCodeData)->OpCode == EFI_IFR_FORM_SET_OP) {
            //
            // Try to compare against FormSet GUID
            //
            if (CompareGuid (ComparingGuid, (EFI_GUID *)(OpCodeData + sizeof (EFI_IFR_OP_HEADER)))) {
              HiiHandle = HiiHandles[Index];
              break;
            }
          }

          Offset2 += ((EFI_IFR_OP_HEADER *) OpCodeData)->Length;
        }
      }
      if (HiiHandle != NULL) {
        break;
      }
      Offset += PackageHeader.Length;
    }

    FreePool (HiiPackageList);
  	if (HiiHandle != NULL) {
  		break;
  	}
  }

  FreePool (HiiHandles);

  return HiiHandle;
}

/**
  Convert the device path from string to binary format.

  @param   [in] StringPtr     The device path string info.

  @return  Device path binary info.

**/
EFI_DEVICE_PATH_PROTOCOL *
ConvertDevicePathFromText (
  IN CHAR16  *StringPtr
  )
{
  UINTN                           BufferSize;
  EFI_DEVICE_PATH_PROTOCOL        *DevicePath;
  CHAR16                          TemStr[2];
  UINT8                           *DevicePathBuffer;
  UINTN                           Index;
  UINT8                           DigitUint8;

  ASSERT (StringPtr != NULL);
  if (StringPtr == NULL) {
    return NULL;
  }

  BufferSize = StrLen (StringPtr) / 2;
  DevicePath = AllocatePool (BufferSize);
  ASSERT (DevicePath != NULL);
  if (DevicePath == NULL) {
    return NULL;
  }

  //
  // Convert from Device Path String to DevicePath Buffer in the reverse order.
  //
  DevicePathBuffer = (UINT8 *) DevicePath;
  for (Index = 0; StringPtr[Index] != L'\0'; Index ++) {
    TemStr[0] = StringPtr[Index];
    DigitUint8 = (UINT8) StrHexToUint64 (TemStr);
    if (DigitUint8 == 0 && TemStr[0] != L'0') {
      //
      // Invalid Hex Char as the tail.
      //
      break;
    }
    if ((Index & 1) == 0) {
      DevicePathBuffer [Index/2] = DigitUint8;
    } else {
      DevicePathBuffer [Index/2] = (UINT8) ((DevicePathBuffer [Index/2] << 4) + DigitUint8);
    }
  }

  return DevicePath;
}

/**
  Process the goto op code, update the info in the selection structure.

  @param [in] Statement    The statement belong to goto op code.
  @param [in] Selection    The selection info.
  @param [in] Repaint      Whether need to repaint the menu.
  @param [in] NewLine      Whether need to create new line.

  @retval EFI_SUCCESS      The menu process successfully.
  @return Other value if the process failed.
**/
EFI_STATUS
ProcessGotoOpCode (
  IN OUT   FORM_BROWSER_STATEMENT      *Statement,
  IN OUT   UI_MENU_SELECTION           *Selection,
  OUT      BOOLEAN                     *Repaint,
  OUT      BOOLEAN                     *NewLine
  )
{
  CHAR16                          *StringPtr;
  EFI_DEVICE_PATH_PROTOCOL        *DevicePath;
  FORM_BROWSER_FORM               *RefForm;
  EFI_STATUS                      Status;

  Status = EFI_SUCCESS;
  StringPtr = NULL;

  //
  // Prepare the device path check, get the device path info first.
  //
  if (Statement->HiiValue.Value.ref.DevicePath != 0) {
    StringPtr = GetString (Statement->HiiValue.Value.ref.DevicePath, Selection->FormSet->HiiHandle);
  }

  //
  // Check whether the device path string is a valid string.
  //
  if (Statement->HiiValue.Value.ref.DevicePath != 0 && StringPtr != NULL) {
    if (Selection->Form->ModalForm) {
      return Status;
    }
    //
    // Goto another HII Package list
    //
    Selection->Action = UI_ACTION_REFRESH_FORMSET;
    DevicePath = ConvertDevicePathFromText (StringPtr);

    Selection->Handle = DevicePathToHiiHandle (DevicePath);
    FreePool (DevicePath);
    FreePool (StringPtr);

    if (Selection->Handle == NULL) {
      //
      // If target HII Handle not found, exit
      //
      Selection->Action = UI_ACTION_EXIT;
      Selection->Statement = NULL;
      return Status;
    }

    CopyMem (&Selection->FormSetGuid, &Statement->HiiValue.Value.ref.FormSetGuid, sizeof (EFI_GUID));
    Selection->FormId = Statement->HiiValue.Value.ref.FormId;
    Selection->QuestionId = Statement->HiiValue.Value.ref.QuestionId;
  } else if (!CompareGuid (&Statement->HiiValue.Value.ref.FormSetGuid, &mZeroGuid)) {
    if (Selection->Form->ModalForm) {
      return Status;
    }
    //
    // Goto another FormSet, check for uncommitted data
    //
    Selection->Action = UI_ACTION_REFRESH_FORMSET;

    Selection->Handle = FormSetGuidToHiiHandle (&Statement->HiiValue.Value.ref.FormSetGuid);
    if (Selection->Handle == NULL) {
      //
      // If target HII Handle not found, exit
      //
      Selection->Action = UI_ACTION_EXIT;
      Selection->Statement = NULL;
      return Status;
    }

    CopyGuid (&Selection->FormSetGuid, &Statement->HiiValue.Value.ref.FormSetGuid);
    Selection->FormId = Statement->HiiValue.Value.ref.FormId;
    Selection->QuestionId = Statement->HiiValue.Value.ref.QuestionId;
  } else if (Statement->HiiValue.Value.ref.FormId != 0) {
    //
    // Check whether target Form is suppressed.
    //
    RefForm = IdToForm (Selection->FormSet, Statement->HiiValue.Value.ref.FormId);

    if ((RefForm != NULL) && (RefForm->SuppressExpression != NULL)) {
      if (EvaluateExpressionList(RefForm->SuppressExpression, TRUE, Selection->FormSet, RefForm) != ExpressFalse) {
        //
        // Form is suppressed.
        //
        if (Repaint != NULL) {
          *Repaint = TRUE;
        }
        return Status;
      }
    }

    //
    // Goto another Form inside this FormSet,
    //
    Selection->Action = UI_ACTION_REFRESH_FORM;

    Selection->FormId = Statement->HiiValue.Value.ref.FormId;
    Selection->QuestionId = Statement->HiiValue.Value.ref.QuestionId;
  } else if (Statement->HiiValue.Value.ref.QuestionId != 0) {
    //
    // Goto another Question
    //
    Selection->QuestionId = Statement->HiiValue.Value.ref.QuestionId;

    if ((Statement->QuestionFlags & EFI_IFR_FLAG_CALLBACK) != 0) {
      Selection->Action = UI_ACTION_REFRESH_FORM;
    } else {
      if (Repaint != NULL) {
        *Repaint = TRUE;
      }
      if (NewLine != NULL) {
        *NewLine = TRUE;
      }
    }
  } else {
    if ((Statement->QuestionFlags & EFI_IFR_FLAG_CALLBACK) != 0) {
      Selection->Action = UI_ACTION_REFRESH_FORM;
    }
  }

  return Status;
}

