/** @file
    Provide support functions for variable lock service.

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
#include "VariableLock.h"

///
/// The list to store the variables which cannot be set after the EFI_END_OF_DXE_EVENT_GROUP_GUID
/// or EVT_GROUP_READY_TO_BOOT event.
///
LIST_ENTRY             mLockedVariableList    = INITIALIZE_LIST_HEAD_VARIABLE (mLockedVariableList);
BOOLEAN                mEnableLocking         = TRUE;

/**
  Acquires lock only at boot time. Simply returns at runtime.

  This is a temperary function that will be removed when
  EfiAcquireLock() in UefiLib can handle the call in UEFI
  Runtimer driver in RT phase.
  It calls EfiAcquireLock() at boot time, and simply returns
  at runtime.

  @param[in]  Lock         A pointer to the lock to acquire.

**/
STATIC
VOID
AcquireLockOnlyAtBootTime (
  IN EFI_LOCK                             *Lock
  )
{
  if (!AtRuntime ()) {
    EfiAcquireLock (Lock);
  }
}


/**
  Releases lock only at boot time. Simply returns at runtime.

  This is a temperary function which will be removed when
  EfiReleaseLock() in UefiLib can handle the call in UEFI
  Runtimer driver in RT phase.
  It calls EfiReleaseLock() at boot time and simply returns
  at runtime.

  @param[in]  Lock         A pointer to the lock to release.

**/
STATIC
VOID
ReleaseLockOnlyAtBootTime (
  IN EFI_LOCK                             *Lock
  )
{
  if (!AtRuntime ()) {
    EfiReleaseLock (Lock);
  }
}

/**
  Internal function to check this variable is whether in variable lock list

  @param[in] VariableName  Name of Variable to be found.
  @param[in] VendorGuid    Variable vendor GUID.

  @retval TRUE             This variable is in variable lock list.
  @retval FALSE            This variable isn't in variable lock list.
**/
STATIC
BOOLEAN
IsVariableInLockList (
  IN CHAR16                  *VariableName,
  IN EFI_GUID                *VendorGuid
  )
{
  LIST_ENTRY              *Link;
  VARIABLE_ENTRY          *Entry;

  for ( Link = GetFirstNode (&mLockedVariableList)
      ; !IsNull (&mLockedVariableList, Link)
      ; Link = GetNextNode (&mLockedVariableList, Link)
      ) {
    Entry = BASE_CR (Link, VARIABLE_ENTRY, Link);
    if (CompareGuid (&Entry->Guid, VendorGuid) && (StrCmp (Entry->Name, VariableName) == 0)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  Mark a variable that will become read-only after leaving the DXE phase of execution.

  @param[in] This          The VARIABLE_LOCK_PROTOCOL instance.
  @param[in] VariableName  A pointer to the variable name that will be made read-only subsequently.
  @param[in] VendorGuid    A pointer to the vendor GUID that will be made read-only subsequently.

  @retval EFI_SUCCESS           The variable specified by the VariableName and the VendorGuid was marked
                                as pending to be read-only.
  @retval EFI_INVALID_PARAMETER VariableName or VendorGuid is NULL.
                                Or VariableName is an empty string.
  @retval EFI_ACCESS_DENIED     EFI_END_OF_DXE_EVENT_GROUP_GUID or EFI_EVENT_GROUP_READY_TO_BOOT has
                                already been signaled.
  @retval EFI_OUT_OF_RESOURCES  There is not enough resource to hold the lock request.
**/
EFI_STATUS
EFIAPI
VariableLockRequestToLock (
  IN CONST EDKII_VARIABLE_LOCK_PROTOCOL *This,
  IN       CHAR16                       *VariableName,
  IN       EFI_GUID                     *VendorGuid
  )
{
  VARIABLE_ENTRY                  *Entry;

  if (VariableName == NULL || VariableName[0] == 0 || VendorGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (mVariableModuleGlobal->EndOfDxe) {
    return EFI_ACCESS_DENIED;
  }

  if (IsVariableInLockList (VariableName, VendorGuid)) {
    return EFI_SUCCESS;
  }

  Entry = VariableAllocateZeroBuffer (sizeof (*Entry) + StrSize (VariableName), TRUE);
  if (Entry == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  AcquireLockOnlyAtBootTime(&mVariableModuleGlobal->VariableBase.VariableServicesLock);

  Entry->Name = (CHAR16 *) (Entry + 1);
  StrCpy   (Entry->Name, VariableName);
  CopyGuid (&Entry->Guid, VendorGuid);
  InsertTailList (&mLockedVariableList, &Entry->Link);

  ReleaseLockOnlyAtBootTime (&mVariableModuleGlobal->VariableBase.VariableServicesLock);

  return EFI_SUCCESS;
}


/**
  Convert the standard Lib double linked list to a virtual mapping.

  This service uses gRT->ConvertPointer() to walk a double linked list and convert all the link
  pointers to their virtual mappings. This function is only guaranteed to work during the
  EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE event and calling it at other times has undefined results.

  @param[in]  ListHead           Head of linked list to convert.
**/
STATIC
VOID
ConvertList (
  IN OUT LIST_ENTRY       *ListHead
  )
{
  LIST_ENTRY  *Link;
  LIST_ENTRY  *NextLink;

  if (ListHead == NULL) {
    return;
  }
  //
  // Convert all the ForwardLink & BackLink pointers in the list
  //
  Link = ListHead;
  do {
    NextLink = Link->ForwardLink;
    gRT->ConvertPointer (0, (VOID **) &Link->ForwardLink);
    gRT->ConvertPointer (0, (VOID **) &Link->BackLink);
    Link = NextLink;
  } while (Link != ListHead);

}

/**
  Convert all of variables in variable list to virtual address.
**/
VOID
ConvertVariablLockList (
  VOID
  )
{

  LIST_ENTRY     *Link;
  VARIABLE_ENTRY *Entry;
  //
  // in the list of locked variables, convert the name pointers first
  //
  for ( Link = GetFirstNode (&mLockedVariableList)
      ; !IsNull (&mLockedVariableList, Link)
      ; Link = GetNextNode (&mLockedVariableList, Link)
      ) {
    Entry = BASE_CR (Link, VARIABLE_ENTRY, Link);
    gRT->ConvertPointer  (0x0, (VOID **) &Entry->Name);
  }
  //
  // second, convert the list itself.
  //
  ConvertList (&mLockedVariableList);
}

/**
  Internal function to check this variable is whether locked.

  @param[in] VariableName  Name of Variable to be found.
  @param[in] VendorGuid    Variable vendor GUID.

  @retval TRUE             This variable is locked.
  @retval FALSE            This variable isn't locked.
**/
BOOLEAN
IsVariableLocked (
  IN CHAR16                  *VariableName,
  IN EFI_GUID                *VendorGuid
  )
{
  if (mSmst == NULL) {
    if (mVariableModuleGlobal->EndOfDxe && mEnableLocking && IsVariableInLockList (VariableName, VendorGuid)) {
      return TRUE;
    }
  } else {
    if (mSmmVariableGlobal->ProtectedModeVariableModuleGlobal->EndOfDxe && mEnableLocking && IsVariableInLockList (VariableName, VendorGuid)) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Create variable lock list in SMM RAM.

  @param[in] ListHead      Head of linked list from protected mode.

  @retval EFI_SUCCESS           Create variable lock list in SMM RAM successfully.
  @retval EFI_USUPPORTED        System isn't in SMM mode.
  @retval EFI_OUT_OF_RESOURCES  There is not enough resource to hold the variable entry.
**/
STATIC
EFI_STATUS
CreateVariableLockListInSmm (
  LIST_ENTRY         *ListHead
  )
{
  LIST_ENTRY     *Link;
  VARIABLE_ENTRY *Entry;
  UINTN          EntrySize;
  VARIABLE_ENTRY *WorkingEntry;

  if (mSmst == NULL) {
    return EFI_UNSUPPORTED;
  }

  //
  // in the list of locked variables, convert the name pointers first
  //
  for ( Link = GetFirstNode (ListHead)
      ; !IsNull (ListHead, Link)
      ; Link = GetNextNode (ListHead, Link)
      ) {
    Entry     = BASE_CR (Link, VARIABLE_ENTRY, Link);
    EntrySize = sizeof (VARIABLE_ENTRY) + StrSize (Entry->Name);
    WorkingEntry = VariableAllocateZeroBuffer (EntrySize, TRUE);
    ASSERT (WorkingEntry != NULL);
    if (WorkingEntry == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    CopyMem (WorkingEntry, Entry, EntrySize);
    WorkingEntry->Name = (CHAR16 *) (WorkingEntry + 1);
    InsertTailList (&mLockedVariableList, &WorkingEntry->Link);
  }

  return EFI_SUCCESS;
}


/**
  This function uses to send SMI to create vriable lock list in SMM mode.
**/
VOID
CreateVariableLockListThroughSmi (
  VOID
  )
{
  UINT32            Signature;
  LIST_ENTRY        *ListHead;

  if (mVariableModuleGlobal->SmmCodeReady && mSmst == NULL) {
    Signature = SMM_VARIABLE_LOCK_SIGNATURE;
    CopyMem (mSmmPhyVarBuf, &Signature, sizeof (Signature));
    ListHead = &mLockedVariableList;
    CopyMem (mSmmPhyVarBuf + 1, &ListHead, sizeof (LIST_ENTRY *));
    SmmSecureBootCall ((UINT8 *) mSmmPhyVarBuf, sizeof (SMM_VAR_BUFFER) + sizeof (LIST_ENTRY *), SMM_VARIABLE_LOCK_FUN_NUM, SW_SMI_PORT);
  }
}

/**
  This fucnitons uses to create variable lock list.

  @return EFI_SUCCESS    Create variable lock list in SMM RAM successfully.
  @return Other          Any error occured while creating variable lock list in SMM RAM.
**/
EFI_STATUS
SmmCreateVariableLockList (
  VOID
  )
{
  UINT32         BufferSize;
  UINT8          *VariableBuffer;
  UINT32         Signature;
  LIST_ENTRY     *ListHead;


  GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RBX ,&BufferSize);
  VariableBuffer = NULL;
  GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI ,(UINT32 *) &VariableBuffer);
  Signature = SMM_VARIABLE_LOCK_SIGNATURE;

  if (BufferSize != sizeof (SMM_VAR_BUFFER) + sizeof (LIST_ENTRY *) || CompareMem (VariableBuffer, &Signature, sizeof (UINT32))!= 0) {
    return EFI_UNSUPPORTED;
  }
  ListHead = *((LIST_ENTRY **) (VariableBuffer + sizeof (SMM_VAR_BUFFER)));
  return CreateVariableLockListInSmm (ListHead);
}

