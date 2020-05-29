/** @file
  Firmware Version Info Interface Lib implementation.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "RcFviLib.h"
#include <Protocol/DataHub.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Uefi/UefiSpec.h>
#include <Guid/DataHubRecords.h>
#include <Guid/MiscProducerGuid.h>

EFI_DATA_HUB_PROTOCOL *mDataHub = NULL;

/**
  Initialize callback context for Firmware Version Info (FVI) Interface Spec v0.7
  implementation.

  @param[in] String               The pointer to the string for calculating length

  @retval None
**/
UINT32
GetStringLen (
  IN UINT8 *String
  )
{
  UINT8 Length;

  for (Length = 0; *String != 0; String++, Length++) {
    if (Length >= SMBIOS_STRING_MAX_LENGTH) {
      break;
    }
  }

  return (UINT32) (Length + 1);
}

/**
  Initialize callback context for Firmware Version Info (FVI) Interface Spec v0.7
  implementation.

  @param[in] Type                 Value is defined in SMBIOS Type 14 - Group Associaction structure - item type.
  @param[in] Count                Number of elements included by this SMBIOS table
  @param[in] FviContext           Context of FVI elements for data hub log

  @retval None
**/
VOID
InitFviDataHubCbContext (
  IN UINT8                         Type,
  IN UINT8                         Count,
  IN FVI_DATA_HUB_CALLBACK_CONTEXT *FviContext
  )
{
  ///
  /// Locate the Data hub protocol
  ///
  if (mDataHub == NULL) {
    gBS->LocateProtocol (&gEfiDataHubProtocolGuid, NULL, (VOID **)&mDataHub);
  }

  if (FviContext != NULL) {
    FviContext->FviHeader.FviHdr.Header.Type    = Type;
    FviContext->FviHeader.FviHdr.Count          = Count;
    FviContext->FviHeader.FviHdr.Header.Length  = sizeof (FVI_HEADER) + FVI_ELEMENTS_SIZE_NOSTRING * Count;
  } else {
    ASSERT (FALSE);
  }

  return ;
}

/**
  Create the Reference code version info as per Firmware Version Info (FVI) Interface Spec v0.7
  to Data Hub.

  @param[in] FviContext           Pointer to the notification functions context, which is context of FVI
                                  elements for data hub log

  @retval None
**/
VOID
CreateRcFviDatahub (
  IN FVI_DATA_HUB_CALLBACK_CONTEXT *FviContext
  )
{
  VOID  *Registration;

  if (mDataHub == NULL) {
    EfiCreateProtocolNotifyEvent (
      &gEfiDataHubProtocolGuid,
      TPL_CALLBACK,
      DataHubCallback,
      (VOID *) FviContext,
      &Registration
      );
  } else {
    DataHubCallback ((EFI_EVENT) NULL, (VOID *) FviContext);
  }
}

/**
  Publish the Reference code version info as per Firmware Version Info (FVI) Interface Spec v0.7
  using MiscSubClass Data Hub.

  @param[in] Event                Event whose notification function is being invoked.
  @param[in] Context              Pointer to the notification functions context, which is implementation dependent.

  @retval None
**/
VOID
EFIAPI
DataHubCallback (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS                    Status;
  FVI_DATA_HUB_CALLBACK_CONTEXT *FviContext;
  UINT8                         Index;
  UINT8                         StrIndex;
  UINT8                         *Record;
  UINT8                         *LastRecord;
  UINT8                         *String;
  UINT8                         Count;
  UINT32                        Length;
  FVI_ELEMENT_AND_FUNCTION      *NewElement;

  Status = EFI_SUCCESS;
  if (mDataHub == NULL) {
    Status = gBS->LocateProtocol (&gEfiDataHubProtocolGuid, NULL, (VOID **)&mDataHub);
  }

  if ((mDataHub != NULL) && (Context != NULL)) {

    if (Event != NULL) {
      gBS->CloseEvent (Event);
    }

    FviContext = (FVI_DATA_HUB_CALLBACK_CONTEXT *) Context;
    Count = FviContext->FviHeader.FviHdr.Count;

    ///
    /// Allocate a buffer to record data sorted later
    ///
    Length  = sizeof (MISC_SUBCLASS_FVI_HEADER) + (sizeof (FVI_ELEMENTS) * Count);

    Status  = EFI_OUT_OF_RESOURCES;
    Record  = (UINT8 *) AllocateZeroPool (Length);
    if (Record != NULL) {
      LastRecord = Record;

      ///
      /// Copy the headers including Data Hub and SMBIOS FviSmbios OEM type
      ///
      CopyMem (LastRecord, &(FviContext->FviHeader), sizeof (MISC_SUBCLASS_FVI_HEADER));
      LastRecord += sizeof (MISC_SUBCLASS_FVI_HEADER);
      String      = LastRecord + FVI_ELEMENTS_SIZE_NOSTRING * Count;

      NewElement  = FviContext->Elements;

      ///
      /// Copy elements including strings
      ///
      for (Index = 0, StrIndex = 1; Index < Count; Index++) {
        if (NewElement->Function != NULL) {
          NewElement->Function (&(NewElement->Element));
        }

        ///
        /// If string is implemented for ComponentName or VersionString, and then string
        /// index of ComponentName or VersionString can't be zero. The string index of
        /// ComponentName and VersionString will be updated and calculated while analyse
        /// all elements here.String index must be non-zero if implemented.
        ///
        if (NewElement->Element.ComponentName != 0) {
          NewElement->Element.ComponentName = StrIndex;
          Length = GetStringLen (NewElement->Element.NameString);
          CopyMem (String, &(NewElement->Element.NameString), Length);
          String += Length;
          StrIndex++;
        }

        if (NewElement->Element.VersionString != 0) {
          NewElement->Element.VersionString = StrIndex;
          Length = GetStringLen (NewElement->Element.VerString);
          CopyMem (String, &(NewElement->Element.VerString), Length);
          String += Length;
          StrIndex++;
        }

        CopyMem (LastRecord, &(NewElement->Element), FVI_ELEMENTS_SIZE_NOSTRING);
        LastRecord += FVI_ELEMENTS_SIZE_NOSTRING;

        NewElement++;
      }

      Length = (UINT32) (String - Record) + 1;
      Status = mDataHub->LogData (
                          mDataHub,
                          &gEfiMiscSubClassGuid,
                          &gMiscProducerGuid,
                          EFI_DATA_RECORD_CLASS_DATA,
                          (VOID *) Record,
                          Length
                          );
    }

    ASSERT (!EFI_ERROR (Status));
    if (Record != NULL) {
      FreePool (Record);
    }
  }
}
