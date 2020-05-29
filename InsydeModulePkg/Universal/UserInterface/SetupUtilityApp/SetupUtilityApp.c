/** @file
 Setup Utility application to display Setup Utility.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#include "SetupUtilityApp.h"

EFI_HII_DATABASE_PROTOCOL     *mHiiDatabase;
EFI_GUID                      mSetupVarGuid = SYSTEM_CONFIGURATION_GUID;

/**
  Check if input HII package support specific form set class GUID or not

  @param[in] FormsetClassGuid    The form set class GUID
  @param[in] Package             Pointer to HII package

  @retval TRUE                   HII package support specific form set class GUID
  @retval FALSE                  HII package does not support specific form set class GUID
**/
BOOLEAN
SupportFormsetClassGuidByPackage (
  IN EFI_GUID                  *FormsetClassGuid,
  IN UINT8                     *Package
  )
{
  EFI_HII_PACKAGE_HEADER       *PackageHeader;
  UINT32                       Offset;
  UINT8                        *OpCodeData;
  UINT8                        ClassGuidNum;
  EFI_GUID                     *ClassGuid;


  if (FormsetClassGuid == NULL || Package == NULL) {
    return FALSE;
  }

  PackageHeader = (EFI_HII_PACKAGE_HEADER *) Package;
  if (PackageHeader->Type != EFI_HII_PACKAGE_FORMS) {
    return FALSE;
  }

  Offset = sizeof (EFI_HII_PACKAGE_HEADER);
  while (Offset < PackageHeader->Length) {
    OpCodeData = Package + Offset;

    if (((EFI_IFR_OP_HEADER *) OpCodeData)->OpCode == EFI_IFR_FORM_SET_OP &&
        ((EFI_IFR_OP_HEADER *) OpCodeData)->Length > OFFSET_OF (EFI_IFR_FORM_SET, Flags)) {
      ClassGuidNum = (UINT8) (((EFI_IFR_FORM_SET *) OpCodeData)->Flags & 0x3);
      ClassGuid    = (EFI_GUID *) (VOID *) (OpCodeData + sizeof (EFI_IFR_FORM_SET));

      while (ClassGuidNum > 0) {
        if (CompareGuid (ClassGuid, FormsetClassGuid)) {
          return TRUE;
        }
        ClassGuidNum--;
        ClassGuid++;
      }
    }

    Offset += ((EFI_IFR_OP_HEADER *) OpCodeData)->Length;
  }

  return FALSE;
}


/**
  Check if input HII handle support specific form set class GUID or not.

  @param[in] FormsetClassGuid    The form set class GUID.
  @param[in] Handle              The HII handle.

  @retval TRUE                   HII handle support specific form set class GUID.
  @retval FALSE                  HII handle does not support specific form set class GUID.
**/
BOOLEAN
SupportFormsetClassGuid (
  IN EFI_GUID                  *FormsetClassGuid,
  IN EFI_HII_HANDLE            HiiHandle
  )
{
  EFI_STATUS                   Status;
  UINTN                        HiiPackageListSize;
  EFI_HII_PACKAGE_LIST_HEADER  *HiiPackageList;
  UINT8                        *Package;
  EFI_HII_PACKAGE_HEADER       *PackageHeader;
  UINT32                       Offset;
  UINT32                       PackageListLength;


  if (HiiHandle == NULL || FormsetClassGuid == NULL) {
    return FALSE;
  }

  //
  // Get HII package list
  //
  HiiPackageList     = NULL;
  HiiPackageListSize = 0;
  Status = mHiiDatabase->ExportPackageLists (mHiiDatabase, HiiHandle, &HiiPackageListSize, HiiPackageList);
  if (Status != EFI_BUFFER_TOO_SMALL) {
    return FALSE;
  }
  HiiPackageList = AllocateZeroPool (HiiPackageListSize);
  if (HiiPackageList == NULL) {
    return FALSE;
  }
  Status = mHiiDatabase->ExportPackageLists (mHiiDatabase, HiiHandle, &HiiPackageListSize, HiiPackageList);
  if (EFI_ERROR (Status)) {
    gBS->FreePool (HiiPackageList);
    return FALSE;
  }

  //
  // In HII package list, find the Form package which support specific form set class GUID.
  //
  Offset            = sizeof (EFI_HII_PACKAGE_LIST_HEADER);
  PackageListLength = ReadUnaligned32 (&HiiPackageList->PackageLength);
  while (Offset < PackageListLength) {
    Package       = (UINT8 *) HiiPackageList + Offset;
    PackageHeader = (EFI_HII_PACKAGE_HEADER *) Package;

    if (PackageHeader->Type == EFI_HII_PACKAGE_FORMS &&
        SupportFormsetClassGuidByPackage (FormsetClassGuid, Package)) {
      gBS->FreePool (HiiPackageList);
      return TRUE;
    }

    Offset += PackageHeader->Length;
  }

  gBS->FreePool (HiiPackageList);

  return FALSE;
}


/**
  Get all HII handles which supports specific form set class GUID.

  @param[in]  FormsetClassGuid     The specific form set class GUID
  @param[out] HiiHandleList        Pointer to HII handle list
  @param[out] HiiHandleListCount   Pointer to the number of HII handle list

  @retval EFI_SUCCESS              Success to get all HII handles which supports the specific form set class GUID
  @retval EFI_NOT_FOUND            Not found the HII handle which supports the specific form set GUID
  @retval EFI_OUT_OF_RESOURCES     Allocate memory fail
**/
EFI_STATUS
GetAllHiiHandlesByClassGuid (
  IN  EFI_GUID                   *FormSetClassGuid,
  OUT EFI_HII_HANDLE             **HiiHandleList,
  OUT UINTN                      *HiiHandleListCount
  )
{
  EFI_STATUS                    Status;
  EFI_HII_HANDLE                *HiiHandles;
  EFI_HII_HANDLE                *ListBuffer;
  EFI_HII_HANDLE                *Buffer;
  UINTN                         ListCount;
  UINTN                         Index;
  UINTN                         Num;


  HiiHandles = HiiGetHiiHandles (NULL);
  if (HiiHandles == NULL) {
    return EFI_NOT_FOUND;
  }

  ListCount  = 10;
  ListBuffer = AllocatePool (ListCount * sizeof(EFI_HII_HANDLE));
  if (ListBuffer == NULL) {
    gBS->FreePool (HiiHandles);
    return EFI_OUT_OF_RESOURCES;
  }

  Status = EFI_NOT_FOUND;
  Num = 0;
  for (Index = 0; HiiHandles[Index] != NULL; Index++) {
    if (SupportFormsetClassGuid (FormSetClassGuid, HiiHandles[Index])) {
      if (Num == ListCount) {
        ListCount += 10;
        Buffer = ReallocatePool (
                   Num * sizeof(EFI_HII_HANDLE),
                   ListCount * sizeof(EFI_HII_HANDLE),
                   ListBuffer
                   );
        if (Buffer == NULL) {
          gBS->FreePool (HiiHandles);
          gBS->FreePool (ListBuffer);
          return EFI_OUT_OF_RESOURCES;
        }

        ListBuffer = Buffer;
      }

      ListBuffer[Num] = HiiHandles[Index];
      Num++;
    }
  }

  if (Num != 0) {
    Buffer = AllocateCopyPool (Num * sizeof(EFI_HII_HANDLE), ListBuffer);
    if (Buffer == NULL) {
      gBS->FreePool (HiiHandles);
      gBS->FreePool (ListBuffer);
      return EFI_OUT_OF_RESOURCES;
    }

    *HiiHandleList      = Buffer;
    *HiiHandleListCount = Num;
    Status = EFI_SUCCESS;
  }

  gBS->FreePool (HiiHandles);
  gBS->FreePool (ListBuffer);

  return Status;
}


/**
  Get opcodes which is for Load Default hot key of Setup Utility

  @param[out] OpCodes              Pointer to opcodes which is for Load Default hot key
  @param[out] OpCodesSize          Pointer to the size of opcodes

  @retval EFI_SUCCESS              Success to get opcodes
  @retval EFI_OUT_OF_RESOURCES     Allocate memory fail
**/
EFI_STATUS
GetLoadDefaultOpCodes (
  OUT UINT8                    **OpCodes,
  OUT UINTN                    *OpCodesSize
  )
{
  UINT8                        *Buffer;
  UINT8                        *BufferPtr;
  UINTN                        BufferSize;
  EFI_STRING_ID                StringId;
  EFI_IFR_FORM                 *OpCodeForm;
  EFI_IFR_ACTION               *OpCodeAction;
  EFI_IFR_END                  *OpCodeEnd;


  //
  //  Genernate following opcodes for Load Default hot key of Setup Utility
  //
  //  form
  //    formid = 0xFFFF,
  //    title = STRING_TOKEN();
  //
  //    text
  //      help   = STRING_TOKEN(),
  //      text   = STRING_TOKEN(),
  //      flags  = INTERACTIVE,
  //      key    = KEY_SCAN_F9;
  //  endform;
  //
  BufferSize = sizeof(EFI_IFR_FORM) + sizeof(EFI_IFR_ACTION) + sizeof(EFI_IFR_END) + sizeof(EFI_IFR_END);
  Buffer     = AllocateZeroPool (BufferSize);
  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  BufferPtr = Buffer;

  StringId = 0x1;

  OpCodeForm                = (EFI_IFR_FORM *) BufferPtr;
  OpCodeForm->Header.OpCode = EFI_IFR_FORM_OP;
  OpCodeForm->Header.Length = sizeof(EFI_IFR_FORM);
  OpCodeForm->Header.Scope  = 1;
  OpCodeForm->FormId        = 0xFFFF;
  OpCodeForm->FormTitle     = StringId;
  BufferPtr += sizeof(EFI_IFR_FORM);

  OpCodeAction                         = (EFI_IFR_ACTION *) BufferPtr;
  OpCodeAction->Header.OpCode          = EFI_IFR_ACTION_OP;
  OpCodeAction->Header.Length          = sizeof(EFI_IFR_ACTION);
  OpCodeAction->Header.Scope           = 1;
  OpCodeAction->Question.Header.Prompt = StringId;
  OpCodeAction->Question.Header.Help   = StringId;
  OpCodeAction->Question.QuestionId    = KEY_SCAN_F9;
  OpCodeAction->Question.Flags         = EFI_IFR_FLAG_CALLBACK;
  BufferPtr += sizeof(EFI_IFR_ACTION);

  OpCodeEnd                = (EFI_IFR_END *) BufferPtr;
  OpCodeEnd->Header.OpCode = EFI_IFR_END_OP;
  OpCodeEnd->Header.Length = sizeof(EFI_IFR_END);
  BufferPtr += sizeof(EFI_IFR_END);

  OpCodeEnd                = (EFI_IFR_END *) BufferPtr;
  OpCodeEnd->Header.OpCode = EFI_IFR_END_OP;
  OpCodeEnd->Header.Length = sizeof(EFI_IFR_END);

  *OpCodes     = Buffer;
  *OpCodesSize = BufferSize;

  return EFI_SUCCESS;
}


/**
  For each HII handle, append load default opcodes into the form package which support specific form set class GUID.

  @param[in] FormsetClassGuid      The specific form set class GUID
  @param[in] HiiHandleList         Pointer to HII handle list
  @param[in] HiiHandleListCount    The number of HII handle list

  @retval EFI_SUCCESS              Success to append
  @retval EFI_INVALID_PARAMETER    Input parameter is invalid
  @retval EFI_NOT_FOUND            Can not find HII package data from HII database
  @retval EFI_OUT_OF_RESOURCES     Allocate memory fail
  @retval Other                    Fail to get load default opcodes
**/
EFI_STATUS
AppendLoadDefaultOpCodes (
  IN EFI_GUID                   *FormSetClassGuid,
  IN EFI_HII_HANDLE             *HiiHandleList,
  IN UINTN                      HiiHandleListCount
  )
{
  EFI_STATUS                   Status;
  UINT8                        *AddedOpCodes;
  UINTN                        AddedOpCodesSize;
  UINTN                        Index;
  UINTN                        HiiPackageListSize;
  EFI_HII_PACKAGE_LIST_HEADER  *HiiPackageList;
  EFI_HII_PACKAGE_LIST_HEADER  *NewHiiPackageList;
  UINT8                        *NewHiiPackageListPtr;
  UINT32                       Offset;
  UINT32                       PackageListLength;
  BOOLEAN                      Appended;
  UINT8                        *Package;
  EFI_HII_PACKAGE_HEADER       *PackageHeader;
  EFI_HII_PACKAGE_HEADER       *TempPackageHeader;


  if (FormSetClassGuid == NULL || HiiHandleList == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetLoadDefaultOpCodes (&AddedOpCodes, &AddedOpCodesSize);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  for (Index = 0; Index < HiiHandleListCount; Index++) {
    HiiPackageList     = NULL;
    HiiPackageListSize = 0;
    Status = mHiiDatabase->ExportPackageLists (mHiiDatabase, HiiHandleList[Index], &HiiPackageListSize, HiiPackageList);
    if (Status != EFI_BUFFER_TOO_SMALL) {
      return EFI_NOT_FOUND;
    }
    HiiPackageList = AllocateZeroPool (HiiPackageListSize);
    if (HiiPackageList == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    Status = mHiiDatabase->ExportPackageLists (mHiiDatabase, HiiHandleList[Index], &HiiPackageListSize, HiiPackageList);
    if (EFI_ERROR (Status)) {
      gBS->FreePool (HiiPackageList);
      return Status;
    }

    NewHiiPackageList = AllocateZeroPool (HiiPackageListSize + AddedOpCodesSize);
    if (NewHiiPackageList == NULL) {
      gBS->FreePool (HiiPackageList);
      return EFI_OUT_OF_RESOURCES;
    }
    NewHiiPackageListPtr = (UINT8 *) NewHiiPackageList;

    Appended          = FALSE;
    Offset            = sizeof (EFI_HII_PACKAGE_LIST_HEADER);
    PackageListLength = ReadUnaligned32 (&HiiPackageList->PackageLength);

    CopyMem (NewHiiPackageListPtr, HiiPackageList, Offset);
    NewHiiPackageListPtr += Offset;

    while (Offset < PackageListLength) {
      Package       = (UINT8 *) HiiPackageList + Offset;
      PackageHeader = (EFI_HII_PACKAGE_HEADER *) Package;

      if (PackageHeader->Type == EFI_HII_PACKAGE_FORMS &&
          !Appended &&
          SupportFormsetClassGuidByPackage (FormSetClassGuid, Package)) {
        //
        // Append load default opcodes before endformset opcode
        //
        TempPackageHeader = (EFI_HII_PACKAGE_HEADER *) NewHiiPackageListPtr;

        CopyMem (NewHiiPackageListPtr, Package, PackageHeader->Length - sizeof(EFI_IFR_END));
        NewHiiPackageListPtr += PackageHeader->Length - sizeof(EFI_IFR_END);

        CopyMem (NewHiiPackageListPtr, AddedOpCodes, AddedOpCodesSize);
        NewHiiPackageListPtr += AddedOpCodesSize;

        CopyMem (NewHiiPackageListPtr, Package + PackageHeader->Length - sizeof(EFI_IFR_END), sizeof(EFI_IFR_END));
        NewHiiPackageListPtr += sizeof(EFI_IFR_END);

        NewHiiPackageList->PackageLength += (UINT32) AddedOpCodesSize;
        TempPackageHeader->Length        += (UINT32) AddedOpCodesSize;
        Appended = TRUE;
      } else {
        CopyMem (NewHiiPackageListPtr, Package, PackageHeader->Length);
        NewHiiPackageListPtr += PackageHeader->Length;
      }

      Offset += PackageHeader->Length;
    }

    if (Appended) {
      mHiiDatabase->UpdatePackageList (mHiiDatabase, HiiHandleList[Index], NewHiiPackageList);
    }

    gBS->FreePool (HiiPackageList);
    gBS->FreePool (NewHiiPackageList);
  }

  gBS->FreePool (AddedOpCodes);

  return EFI_SUCCESS;
}

/**
 Call SCU by compitable method.

 @retval EFI_SUCCESS              Success to call Setup Utility
 @retval EFI_UNSUPPORTED          StartEntry function pointer is NULL
 @retval Other                    Fail to locate setup utility protocol
**/
EFI_STATUS
CallCompatibleScu (
  VOID
  )
{
  EFI_STATUS                   Status;
  EFI_SETUP_UTILITY_PROTOCOL   *SetupUtility;

  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **) &SetupUtility);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (SetupUtility->StartEntry == NULL) {
    return EFI_UNSUPPORTED;
  }

  return SetupUtility->StartEntry (SetupUtility);
}

EFI_STATUS
FilterByClassAndSubclass (
  IN EFI_GUID                          *FormSetClassGuid,
  IN OUT EFI_HII_HANDLE                *HiiHandle,
  IN OUT UINTN                         *HiiHandleCount
  )
{
  EFI_STATUS                           Status;
  KERNEL_CONFIGURATION                 *SetupVar;
  UINTN                                BufferSize;
  EFI_HII_HANDLE                       *List;
  UINTN                                ListCount;
  UINT32                               Index;
  EFI_HII_PACKAGE_LIST_HEADER          *Buffer;
  UINT8                                *TempPtr;
  EFI_IFR_FORM_SET                     *FormSetPtr;
  UINT16                               Class;
  UINT16                               SubClass;

  Status = CommonGetVariableDataAndSize (L"Setup", &mSetupVarGuid, &BufferSize, (VOID **) &SetupVar);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  ListCount = 0;
  List      = AllocatePool (*HiiHandleCount * sizeof(EFI_HII_HANDLE));
  if (List == NULL) {
    return EFI_UNSUPPORTED;
  }

  for (Index = 0; Index < *HiiHandleCount; Index++) {
    //
    // Am not initializing Buffer since the first thing checked is the size
    // this way I can get the real buffersize in the smallest code size
    //
    Buffer     = NULL;
    BufferSize = 0;
    Status = mHiiDatabase->ExportPackageLists (mHiiDatabase, HiiHandle[Index], &BufferSize, Buffer);
    if (Status == EFI_NOT_FOUND) {
      break;
    }

    Buffer = AllocateZeroPool (BufferSize);
    ASSERT (Buffer != NULL);
    if (Buffer == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      break;
    }

    Status = mHiiDatabase->ExportPackageLists (mHiiDatabase, HiiHandle[Index], &BufferSize, Buffer);
    if (EFI_ERROR (Status)) {
      break;
    }

    TempPtr    = (UINT8 *) (Buffer + 1);
    TempPtr    = (UINT8 *) ((EFI_HII_FORM_PACKAGE_HDR *) TempPtr + 1);
    FormSetPtr = (EFI_IFR_FORM_SET *) TempPtr;

    //
    // check Platform form set guid
    //
    if (CompareGuid ((EFI_GUID *) (TempPtr + sizeof (EFI_IFR_FORM_SET)), FormSetClassGuid)) {
      TempPtr  = (UINT8 *) (TempPtr + FormSetPtr->Header.Length);
      Class    = ((EFI_IFR_GUID_CLASS *) TempPtr)->Class;
      TempPtr += sizeof (EFI_IFR_GUID_CLASS);
      SubClass = ((EFI_IFR_GUID_SUBCLASS *) TempPtr)->SubClass;

      if ((Class == EFI_NON_DEVICE_CLASS) ||
          (SubClass == EFI_USER_ACCESS_THREE && SetupVar->SetUserPass == 1 && SetupVar->UserAccessLevel == 3) ||
          (SubClass == EFI_USER_ACCESS_TWO   && SetupVar->SetUserPass == 1 && SetupVar->UserAccessLevel == 2)) {
        gBS->FreePool(Buffer);
        continue;
      }
    }

    List[ListCount] = HiiHandle[Index];
    ListCount++;

    gBS->FreePool(Buffer);
  }

  CopyMem (HiiHandle, List, ListCount * sizeof(EFI_HII_HANDLE));
  *HiiHandleCount = ListCount;

  gBS->FreePool(SetupVar);
  gBS->FreePool(List);

  return Status;
}

/**
 Display Setup Utility.

 It will collect all HII handles which supports Setup Utility form set class GUID.
 Then call oem service function to adjust setup menu. Finally call send form function to display.

 @retval EFI_SUCCESS              Success to display Setup Utility
 @retval Other                    Fail to locate form browser protocol, collect HiII handles or send form
**/
EFI_STATUS
DisplaySetupUtility (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_FORM_BROWSER2_PROTOCOL    *FormBrowser2;
  EFI_GUID                      FormSetClassGuid = SETUP_UTILITY_FORMSET_CLASS_GUID;
  EFI_HII_HANDLE                *HiiHandle;
  UINTN                         HiiHandleCount;
  EFI_BROWSER_ACTION_REQUEST    ActionRequest;


  Status = gBS->LocateProtocol (&gEfiFormBrowser2ProtocolGuid, NULL, (VOID **) &FormBrowser2);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = GetAllHiiHandlesByClassGuid (&FormSetClassGuid, &HiiHandle, &HiiHandleCount);
  if (Status != EFI_SUCCESS) {
    return CallCompatibleScu ();
  }

  FilterByClassAndSubclass (&FormSetClassGuid, HiiHandle, &HiiHandleCount);
  OemSvcAdjustNavigationMenu (HiiHandle, &HiiHandleCount, &FormSetClassGuid);
  if (HiiHandleCount == 0) {
    gBS->FreePool (HiiHandle);
    return EFI_NOT_FOUND;
  }

  AppendLoadDefaultOpCodes (&FormSetClassGuid, HiiHandle, HiiHandleCount);

  ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  Status = FormBrowser2->SendForm (
                           FormBrowser2,
                           HiiHandle,
                           HiiHandleCount,
                           &FormSetClassGuid,
                           1,
                           NULL,
                           &ActionRequest
                           );
  if (ActionRequest == EFI_BROWSER_ACTION_REQUEST_RESET) {
    gBS->RaiseTPL (TPL_NOTIFY);
    gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
  }

  gST->ConOut->SetAttribute (gST->ConOut, EFI_BLACK | EFI_BACKGROUND_BLACK);
  gST->ConOut->ClearScreen (gST->ConOut);

  gBS->FreePool (HiiHandle);

  return Status;
}

/**
 Display Setup Utility to allow user to configure the platform.

 It will install gEfiSetupUtilityApplicationProtocolGuid protocol to trigger Setup related VFR drivers
 to install HII data. Then it will collect all HII handles and display in Setup Utility.
 When exit Setup Utility, it will re-install gEfiSetupUtilityApplicationProtocolGuid protocol to trigger
 Setup related VFR drivers to uninstall HII data.

 @param[in] ImageHandle    The firmware allocated handle for the EFI image.
 @param[in] SystemTable    A pointer to the EFI System Table.

 @retval EFI_SUCCESS            Display Setup Utility successfully.
 @retval EFI_ACCESS_DENIED      Another Setup Utility is running on system now.
 @retval EFI_OUT_OF_RESOURCES   Fail to allocate pool.
 @retval other                  Fail to locate Hii database protocol or install protocol instance.
**/
EFI_STATUS
EFIAPI
SetupUtilityAppMain (
  IN EFI_HANDLE                  ImageHandle,
  IN EFI_SYSTEM_TABLE            *SystemTable
  )
{
  EFI_STATUS                               Status;
  EFI_HANDLE                               SetupUtilityHandle;
  EFI_SETUP_UTILITY_APPLICATION_PROTOCOL   *SetupUtilityApp;

  //
  // Check if there is only one setup utility application protocol in system
  //
  Status = gBS->LocateProtocol (&gEfiSetupUtilityApplicationProtocolGuid, NULL, (VOID **) &SetupUtilityApp);
  if (!EFI_ERROR (Status)) {
    return EFI_ACCESS_DENIED;
  }

  Status = gBS->LocateProtocol (&gEfiHiiDatabaseProtocolGuid, NULL, (VOID **) &mHiiDatabase);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SetupUtilityApp = AllocatePool (sizeof(EFI_SETUP_UTILITY_APPLICATION_PROTOCOL));
  if (SetupUtilityApp == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Install setup utility application protocol to trigger VFR driver to install HII package
  //
  SetupUtilityHandle = 0;
  SetupUtilityApp->VfrDriverState = InitializeSetupUtility;
  Status = gBS->InstallProtocolInterface (
                  &SetupUtilityHandle,
                  &gEfiSetupUtilityApplicationProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  SetupUtilityApp
                  );
  if (!EFI_ERROR (Status)) {
    DisplaySetupUtility ();

    //
    // Re-install setup utility application protocol to trigger VFR driver to uninstall HII package
    //
    SetupUtilityApp->VfrDriverState = ShutdownSetupUtility;
    gBS->ReinstallProtocolInterface (
           SetupUtilityHandle,
           &gEfiSetupUtilityApplicationProtocolGuid,
           SetupUtilityApp,
           SetupUtilityApp
           );

    gBS->UninstallProtocolInterface (
           SetupUtilityHandle,
           &gEfiSetupUtilityApplicationProtocolGuid,
           SetupUtilityApp
           );
  }

  gBS->FreePool (SetupUtilityApp);

  return Status;
}

