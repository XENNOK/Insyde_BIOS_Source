/** @file

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

/*++

Module Name:

  BiosProtectDxe.c

Abstract:

  This driver registers event to enable BIOS region protection before boot to OS

--*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DxeChipsetSvcLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/BiosRegionLock.h>
#include <Protocol/ExitPmAuth.h>
#include <SecureFlash.h>


EFI_EVENT                         mReadyToBootEvent;
EFI_EVENT                         mProtocolNotifyEvent;

VOID
EFIAPI
BiosProtectEvent (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  );

/**
 This routine registers event to enable BIOS region protection.

 @param [in]   ImageHandle      Handle for the image of this driver
 @param [in]   SystemTable      Pointer to the EFI System Table

 @retval EFI_SUCCESS            The function completed successfully

**/
EFI_STATUS
EFIAPI
BiosProtectEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                        Status;
  VOID                              *Registration;
  
  //
  // Use ReadyToBootEvent to make sure BiosProtectEvent()
  // performed whether legacy or UEFI boot mode
  //
  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK + 1,
             BiosProtectEvent,
             NULL,
             &mReadyToBootEvent
             );

  ASSERT_EFI_ERROR (Status);

  //
  // This protocol may install in the bds platform code
  // So we have to register a protocol notify event to deal with this situation
  //
  mProtocolNotifyEvent = EfiCreateProtocolNotifyEvent (
                           &gExitPmAuthProtocolGuid,
                           TPL_NOTIFY,
                           BiosProtectEvent,
                           NULL,
                           &Registration
                           );

  ASSERT (mProtocolNotifyEvent != NULL);

  return EFI_SUCCESS;
}

/**
 Call BiosRegionLock protocol to set protected BIOS region and then lock.

 @param [in]   Event            A pointer to the Event that triggered the callback.
 @param [in]   Context          A pointer to private data registered with the callback function.

 @retval EFI_SUCCESS            The function completed successfully
 @retval EFI_UNSUPPORTED        Locate BiosRegionLockProtocol fail

**/
VOID
EFIAPI
BiosProtectEvent (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS                    Status;
  BIOS_REGION_LOCK_PROTOCOL     *BiosRegionLock;
  UINTN                         RegionIndex;
  BIOS_PROTECT_REGION           *BiosRegionTable;
  UINT8                         ProtectRegionNum;
  IMAGE_INFO                    ImageInfo;
  UINTN                         Size;
  VOID                          *ProtocolPointer;

  if (Event != mReadyToBootEvent) {
    //
    // Check whether this is real ExitPmAuth notification, or just a SignalEvent
    //
    Status = gBS->LocateProtocol (&gExitPmAuthProtocolGuid, NULL, (VOID **)&ProtocolPointer);
    if (EFI_ERROR (Status)) {
      return;
    }
  }
  
  RegionIndex = 0;
  ProtectRegionNum = 0;
  BiosRegionLock  = NULL;
  BiosRegionTable = NULL;

  gBS->CloseEvent (mReadyToBootEvent);
  gBS->CloseEvent (mProtocolNotifyEvent);

  Status = gBS->LocateProtocol (&gEfiBiosRegionLockProtocolGuid, NULL, (VOID **)&BiosRegionLock);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return;
  }

  if (PcdGetBool (PcdSecureFlashSupported)) {
    Size = sizeof (IMAGE_INFO);
    Status = gRT->GetVariable (
                    L"SecureFlashInfo",
                    &gSecureFlashInfoGuid,
                    NULL,
                    &Size,
                    &ImageInfo
                    );

    if (Status == EFI_SUCCESS) {
      if ((Event == mProtocolNotifyEvent) && (ImageInfo.FlashMode)) {
        //
        // Update flash mode: Don't protect any region.
        //
        DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Update flash mode: System doesn't protect any region.\n"));
        return;
      }
      DxeCsSvcGetBiosProtectTable (&BiosRegionTable, &ProtectRegionNum);
      if (BiosRegionTable != NULL && ProtectRegionNum != 0) {   
        for (RegionIndex = 0; RegionIndex < ProtectRegionNum; ++RegionIndex) {
          Status = BiosRegionLock->SetRegionByAddress (BiosRegionLock, BiosRegionTable[RegionIndex].Base, BiosRegionTable[RegionIndex].Size);
          if (EFI_ERROR (Status)) {
            DEBUG ((EFI_D_ERROR, "Secure flash support failed in this system!\n"));
            FreePool (BiosRegionTable);
            return;
          }
        }
        FreePool (BiosRegionTable);
      }
    } else {
      //
      // Get Variable fail, system prepare to flash
      //
      DEBUG ((EFI_D_ERROR | EFI_D_INFO, "System doesn't protect any region.\n"));
      return;
    }
  }

  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Secure flash support succeed.\n"));

  BiosRegionLock->Lock (BiosRegionLock);

  return;
}
