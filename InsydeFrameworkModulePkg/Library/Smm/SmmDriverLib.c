//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
--*/
/*++

Copyright (c)  1999 - 2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  SmmDriverLib.c

Abstract:

  Light weight lib to support EFI 2.0 SMM based drivers.

--*/

#include "Tiano.h"
#include "EfiSmmDriverLib.h"
#include "SmmDriverLib.h"
#include EFI_PROTOCOL_DEFINITION (LoadedImage)

EFI_DEBUG_MASK_PROTOCOL *mDebugMaskInterface = NULL;
UINTN                    mSmmImageDebugMask;
EFI_DEVICE_PATH_PROTOCOL  *
EfiAppendDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *Src1,
  IN EFI_DEVICE_PATH_PROTOCOL  *Src2
  );


EFI_STATUS
InsydeEfiInitializeSmmDriverLib (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable,
  OUT BOOLEAN           *InSmm
  )
/*++

Routine Description:

  Intialize runtime Driver Lib if it has not yet been initialized.

Arguments:

  ImageHandle     - The firmware allocated handle for the EFI image.

  SystemTable     - A pointer to the EFI System Table.

  InSmm           - Indicate whether it is in SMM mode or not.

Returns:

  EFI_STATUS always returns EFI_SUCCESS

--*/
{
  EFI_STATUS                Status;
  EFI_HANDLE                Handle;
  EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;
  EFI_DEVICE_PATH_PROTOCOL  *CompleteFilePath;
  EFI_DEVICE_PATH_PROTOCOL  *ImageDevicePath;

  gST = SystemTable;
  ASSERT (gST != NULL);
  gBS = SystemTable->BootServices;
  ASSERT (gBS != NULL);
  gRT = SystemTable->RuntimeServices;
  ASSERT (gRT != NULL);

  //
  // It is OK if the SmmStatusCode Protocol is not found, don't check the status.
  //
  Status  = gBS->LocateProtocol (&gEfiSmmStatusCodeProtocolGuid, NULL, &mSmmDebug);

  //
  // Get driver debug mask protocol interface
  //
#ifdef EFI_DEBUG
  //
  // It is OK if the DebugMask Protocol is not found, don't check the status.
  //
  Status =  gBS->HandleProtocol (
                    ImageHandle,
                    &gEfiDebugMaskProtocolGuid,
                    (VOID *) &mDebugMaskInterface
                    );
#endif

  Status  = gBS->LocateProtocol (&gEfiSmmBaseProtocolGuid, NULL, &gSMM);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  gSMM->InSmm (gSMM, InSmm);

  if (!(*InSmm)) {
    //
    // Not in SMM, initialization code is running under DXE environment
    //
    //
    // Load this driver's image to memory
    //
    if (ImageHandle != NULL) {
      Status = gBS->HandleProtocol (
                      ImageHandle,
                      &gEfiLoadedImageProtocolGuid,
                      (VOID *) &LoadedImage
                      );

      if (EFI_ERROR (Status)) {
        return Status;
      }

      Status = gBS->HandleProtocol (
                      LoadedImage->DeviceHandle,
                      &gEfiDevicePathProtocolGuid,
                      (VOID *) &ImageDevicePath
                      );

      if (EFI_ERROR (Status)) {
        return Status;
      }

      CompleteFilePath = EfiAppendDevicePath (
                           ImageDevicePath,
                           LoadedImage->FilePath
                           );
      //
      // Load the image in memory to SMRAM; it will automatically generate the
      // SMI.
      //
      Status = gSMM->Register (gSMM, CompleteFilePath, NULL, 0, &Handle, FALSE);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
  } else {
    DEBUG_CODE (
      if (mDebugMaskInterface != NULL) {
        Status = mDebugMaskInterface->GetDebugMask (mDebugMaskInterface, &mSmmImageDebugMask);
        if (EFI_ERROR (Status)) {
          mSmmImageDebugMask = gErrorLevel;
        }
      }
    );
    
  //
  //Trigger EFI_DEBUG once for initialize mSmmBaseInfo(debug.c) when call InsydeEfiInitializeSmmDriverLib() in SMM phase.
  //
    DEBUG ((EFI_D_INFO, "\n Initialize mSmmBaseInfo when call InsydeEfiInitializeSmmDriverLib() in SMM phase."));
  }

  return EFI_SUCCESS;
}

UINTN
EfiDevicePathSize (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  DevicePath  - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
{
  EFI_DEVICE_PATH_PROTOCOL  *Start;

  if (NULL == DevicePath) {
    return 0;
  }
  //
  // Search for the end of the device path structure
  //
  Start = DevicePath;
  while (!EfiIsDevicePathEnd (DevicePath)) {
    DevicePath = EfiNextDevicePathNode (DevicePath);
  }
  //
  // Compute the size and add back in the size of the end device path structure
  //
  return ((UINTN) DevicePath - (UINTN) Start) + sizeof (EFI_DEVICE_PATH_PROTOCOL);
}

EFI_DEVICE_PATH_PROTOCOL *
EfiAppendDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *Src1,
  IN EFI_DEVICE_PATH_PROTOCOL  *Src2
  )
/*++

Routine Description:
  Function is used to append a Src1 and Src2 together.

Arguments:
  Src1  - A pointer to a device path data structure.

  Src2  - A pointer to a device path data structure.

Returns:

  A pointer to the new device path is returned.
  NULL is returned if space for the new device path could not be allocated from pool.
  It is up to the caller to free the memory used by Src1 and Src2 if they are no longer needed.

--*/
{
  EFI_STATUS                Status;
  UINTN                     Size;
  UINTN                     Size1;
  UINTN                     Size2;
  EFI_DEVICE_PATH_PROTOCOL  *NewDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *SecondDevicePath;

  //
  // Allocate space for the combined device path. It only has one end node of
  // length EFI_DEVICE_PATH_PROTOCOL
  //
  Size1 = EfiDevicePathSize (Src1);
  Size2 = EfiDevicePathSize (Src2);
  Size  = Size1 + Size2;

  if (Size1 != 0 && Size2 != 0) {
    Size -= sizeof (EFI_DEVICE_PATH_PROTOCOL);
  }

  Status = gBS->AllocatePool (EfiBootServicesData, Size, (VOID **) &NewDevicePath);

  if (EFI_ERROR (Status)) {
    return NULL;
  }

  gBS->CopyMem (NewDevicePath, Src1, Size1);

  //
  // Over write Src1 EndNode and do the copy
  //
  if (Size1 != 0) {
    SecondDevicePath = (EFI_DEVICE_PATH_PROTOCOL *) ((CHAR8 *) NewDevicePath + (Size1 - sizeof (EFI_DEVICE_PATH_PROTOCOL)));
  } else {
    SecondDevicePath = NewDevicePath;

  }

  gBS->CopyMem (SecondDevicePath, Src2, Size2);

  return NewDevicePath;
}

#if (EFI_SPECIFICATION_VERSION >= 0x00020000)

STATIC
VOID
EFIAPI
EventNotifySignalAllNullEvent (
  IN EFI_EVENT                Event,
  IN VOID                     *Context
  )
{
  //
  // This null event is a size efficent way to enusre that
  // EFI_EVENT_NOTIFY_SIGNAL_ALL is error checked correctly.
  // EFI_EVENT_NOTIFY_SIGNAL_ALL is now mapped into
  // CreateEventEx() and this function is used to make the
  // old error checking in CreateEvent() for Tiano extensions
  // function.
  //
  return;
}

#endif

EFI_STATUS
EFIAPI
SmmEfiCreateEventReadyToBoot (
  IN EFI_TPL                      NotifyTpl,
  IN EFI_EVENT_NOTIFY             NotifyFunction,
  IN VOID                         *NotifyContext,
  OUT EFI_EVENT                   *ReadyToBootEvent
  )
/*++

Routine Description:
  Create a Read to Boot Event.

  Tiano extended the CreateEvent Type enum to add a ready to boot event type.
  This was bad as Tiano did not own the enum. In UEFI 2.0 CreateEventEx was
  added and now it's possible to not voilate the UEFI specification and use
  the ready to boot event class defined in UEFI 2.0. This library supports
  the R8.5/EFI 1.10 form and R8.6/UEFI 2.0 form and allows common code to
  work both ways.

Arguments:
  ReadyToBootEvent  Returns the EFI event returned from gBS->CreateEvent(Ex)

Return:
  EFI_SUCCESS   - Event was created.
  Other         - Event was not created.

--*/
{
  EFI_STATUS        Status;
  UINT32            EventType;
  EFI_EVENT_NOTIFY  WorkerNotifyFunction;

#if (EFI_SPECIFICATION_VERSION < 0x00020000)

  if (NotifyFunction == NULL) {
    EventType = EFI_EVENT_SIGNAL_READY_TO_BOOT | EFI_EVENT_NOTIFY_SIGNAL_ALL;
  } else {
    EventType = EFI_EVENT_SIGNAL_READY_TO_BOOT;
  }
  WorkerNotifyFunction = NotifyFunction;

  //
  // prior to UEFI 2.0 use Tiano extension to EFI
  //
  Status = gBS->CreateEvent (
                  EventType,
                  NotifyTpl,
                  WorkerNotifyFunction,
                  NotifyContext,
                  ReadyToBootEvent
                  );
#else

  EventType = EFI_EVENT_NOTIFY_SIGNAL;
  if (NotifyFunction == NULL) {
    //
    // CreatEventEx will check NotifyFunction is NULL or not
    //
    WorkerNotifyFunction = EventNotifySignalAllNullEvent;
  } else {
    WorkerNotifyFunction = NotifyFunction;
  }

  //
  // For UEFI 2.0 and the future use an Event Group
  //
  Status = gBS->CreateEventEx (
                  EventType,
                  NotifyTpl,
                  WorkerNotifyFunction,
                  NotifyContext,
                  &gEfiEventReadyToBootGuid,
                  ReadyToBootEvent
                  );
#endif
  return Status;
}
