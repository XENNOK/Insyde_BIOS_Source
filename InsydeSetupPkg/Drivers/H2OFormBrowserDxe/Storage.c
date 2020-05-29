/** @file
 Define function of value operation .
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

#include "InternalH2OFormBrowser.h"

/**
  Allocate a FORMSET_STORAGE data structure and insert to FormSet Storage List.

  @param  [in] FormSet                Pointer of the current FormSet

  @return Pointer to a FORMSET_STORAGE data structure.

**/
FORMSET_STORAGE *
CreateStorage (
  IN FORM_BROWSER_FORMSET  *FormSet
  )
{
  FORMSET_STORAGE  *Storage;

  Storage = AllocateZeroPool (sizeof (FORMSET_STORAGE));
  ASSERT (Storage != NULL);
  if (Storage == NULL) {
    DEBUG ((EFI_D_INFO, "CreateStorage() Storage is NULL"));
    return NULL;
  }

  Storage->Signature = FORMSET_STORAGE_SIGNATURE;
  InitializeListHead (&Storage->NameValueListHead);
  InsertTailList (&FormSet->StorageListHead, &Storage->Link);

  return Storage;
}

/**
  Free resources of a storage.

  @param  [in] Storage                Pointer of the storage

**/
VOID
DestroyStorage (
  IN FORMSET_STORAGE   *Storage
  )
{
  LIST_ENTRY         *Link;
  NAME_VALUE_NODE    *NameValueNode;

  if (Storage == NULL) {
    return;
  }

 FBFreePool ((VOID **) &Storage->Name);
 FBFreePool ((VOID **) &Storage->Buffer);
 FBFreePool ((VOID **) &Storage->EditBuffer);

  while (!IsListEmpty (&Storage->NameValueListHead)) {
    Link = GetFirstNode (&Storage->NameValueListHead);
    NameValueNode = NAME_VALUE_NODE_FROM_LINK (Link);
    RemoveEntryList (&NameValueNode->Link);

    FBFreePool ((VOID **) &NameValueNode->Name);
    FBFreePool ((VOID **) &NameValueNode->Value);
    FBFreePool ((VOID **) &NameValueNode->EditValue);
    FBFreePool ((VOID **) &NameValueNode);
  }

  FBFreePool ((VOID **) &Storage->ConfigHdr);
  FBFreePool ((VOID **) &Storage->ConfigRequest);
  FBFreePool ((VOID **) &Storage);
}

