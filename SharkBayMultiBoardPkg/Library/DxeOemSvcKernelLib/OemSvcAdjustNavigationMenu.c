/** @file
  Adjust navigation menu which is displayed in Setup Utility.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Protocol/HiiDatabase.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <ChipsetSetupConfig.h>

EFI_HII_DATABASE_PROTOCOL      *mHiiDatabase;
EFI_GUID                       mFsGuidSequence[] = {FORMSET_ID_GUID_MAIN,
                                                    FORMSET_ID_GUID_ADVANCE,
                                                    FORMSET_ID_GUID_SECURITY,
                                                    FORMSET_ID_GUID_POWER,
                                                    FORMSET_ID_GUID_BOOT,
                                                    FORMSET_ID_GUID_EXIT
                                                    };

/**
  Check if input HII handle support specific form set GUID or not.

  @param[in] FormsetGuid         The form set GUID.
  @param[in] Handle              The HII handle.

  @retval TRUE                   HII handle support specific form set GUID.
  @retval FALSE                  HII handle does not support specific form set GUID.
**/
BOOLEAN
SupportFormsetGuid (
  IN EFI_GUID                  *FormsetGuid,
  IN EFI_HII_HANDLE            HiiHandle
  )
{
  EFI_STATUS                   Status;
  UINTN                        BufferSize;
  EFI_HII_PACKAGE_LIST_HEADER  *HiiPackageList;
  UINT8                        *Package;
  UINT8                        *OpCodeData;
  UINT32                       Offset;
  UINT32                       PackageOffset;
  UINT32                       PackageListLength;
  EFI_HII_PACKAGE_HEADER       PackageHeader;

  if (HiiHandle == NULL || FormsetGuid == NULL) {
    return FALSE;
  }

  //
  // Get HII package list
  //
  BufferSize = 0;
  HiiPackageList = NULL;
  Status = mHiiDatabase->ExportPackageLists (mHiiDatabase, HiiHandle, &BufferSize, HiiPackageList);
  if (Status != EFI_BUFFER_TOO_SMALL) {
    return FALSE;
  }
  HiiPackageList = AllocatePool (BufferSize);
  if (HiiPackageList == NULL) {
    return FALSE;
  }
  Status = mHiiDatabase->ExportPackageLists (mHiiDatabase, HiiHandle, &BufferSize, HiiPackageList);
  if (EFI_ERROR (Status)) {
    FreePool (HiiPackageList);
    return FALSE;
  }

  //
  // Get Form package and check form set class GUID in this HII package list
  //
  Offset = sizeof (EFI_HII_PACKAGE_LIST_HEADER);
  PackageListLength = ReadUnaligned32 (&HiiPackageList->PackageLength);
  while (Offset < PackageListLength) {
    Package = (UINT8 *) HiiPackageList + Offset;
    CopyMem (&PackageHeader, Package, sizeof (EFI_HII_PACKAGE_HEADER));

    if (PackageHeader.Type == EFI_HII_PACKAGE_FORMS) {
      PackageOffset = sizeof (EFI_HII_PACKAGE_HEADER);
      while (PackageOffset < PackageHeader.Length) {
        OpCodeData = Package + PackageOffset;

        if (((EFI_IFR_OP_HEADER *) OpCodeData)->OpCode == EFI_IFR_FORM_SET_OP &&
            CompareGuid ((EFI_GUID *) (VOID *) (&((EFI_IFR_FORM_SET *) OpCodeData)->Guid), FormsetGuid)) {
          FreePool (HiiPackageList);
          return TRUE;
        }
        PackageOffset += ((EFI_IFR_OP_HEADER *) OpCodeData)->Length;
      }
    }
    Offset += PackageHeader.Length;
  }

  FreePool (HiiPackageList);

  return FALSE;
}


/**
  Adjust navigation menu which is displayed in Setup Utility

  @param[in, out] HiiHandle            Pointer to HII handle list
  @param[in, out] HiiHandleCount       Pointer to the number of HII handle list
  @param[in, out] FormSetGuid          Pointer to the formset GUID

  @retval EFI_UNSUPPORTED              Returns unsupported by default.
  @retval EFI_MEDIA_CHANGED            The value of IN OUT parameter is changed.
**/
EFI_STATUS
OemSvcAdjustNavigationMenu (
  IN OUT EFI_HII_HANDLE                *HiiHandle,
  IN OUT UINTN                         *HiiHandleCount,
  IN OUT EFI_GUID                      *FormSetGuid
  )
{
  EFI_STATUS          Status;
  EFI_HII_HANDLE      TargetHiiHandle;
  EFI_HII_HANDLE      *List;
  UINTN               ListCount;
  UINTN               ListIndex;
  UINTN               Index;
  UINTN               FsGuidIndex;
  UINTN               FsGuidCount;

  if (HiiHandle == NULL || HiiHandleCount == NULL || *HiiHandleCount == 0) {
    return EFI_UNSUPPORTED;
  }

  Status = gBS->LocateProtocol (&gEfiHiiDatabaseProtocolGuid, NULL, (VOID **) &mHiiDatabase);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  ListCount = *HiiHandleCount;
  List      = AllocateCopyPool (ListCount * sizeof(EFI_HII_HANDLE), HiiHandle);
  if (List == NULL) {
    return EFI_UNSUPPORTED;
  }

  //
  // Sort navigation menu order by formset GUID sequence list.
  //
  ListIndex = 0;
  FsGuidCount = sizeof(mFsGuidSequence) / sizeof(EFI_GUID);
  for (FsGuidIndex = 0; FsGuidIndex < FsGuidCount; FsGuidIndex++) {
    for (Index = ListIndex; Index < ListCount; Index++) {
      if (SupportFormsetGuid (&mFsGuidSequence[FsGuidIndex], List[Index])) {
        TargetHiiHandle = List[Index];

        while (Index > ListIndex) {
          List[Index] = List[Index - 1];
          Index--;
        }
        List[ListIndex] = TargetHiiHandle;
        ListIndex++;
        break;
      }
    }
  }

  CopyMem (HiiHandle, List, ListCount * sizeof(EFI_HII_HANDLE));

  gBS->FreePool (List);

  return EFI_MEDIA_CHANGED;
}

