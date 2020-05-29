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
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c)  1999 - 2005 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  MgmtModeRuntimeUtils.c

Abstract:

  Light weight lib to support EFI 2.0 SMM based drivers.

--*/

#include "Tiano.h"
#include "EfiMgmtModeRuntimeLib.h"
#include "EfiRuntimeLib.h"
#include EFI_PROTOCOL_DEFINITION (SmmBase)
#include EFI_PROTOCOL_DEFINITION (SmmControl)
#include EFI_PROTOCOL_DEFINITION (LoadedImage)
#include EFI_PROTOCOL_DEFINITION (SmmRtProtocol)
#include EFI_PROTOCOL_DEFINITION (CpuIo)
#include EFI_PROTOCOL_DEFINITION (FirmwareVolume)

typedef struct {
  EFI_DEVICE_PATH_PROTOCOL  *FilePath;
  EFI_HANDLE                *SmmHandle;
} SMM_PRIVATE_DATA;

//
// Driver Lib Module Globals
//
extern BOOLEAN                              mEfiAtRuntime;
extern EFI_CPU_IO_PROTOCOL                  *gCpuIo;

static EFI_RUNTIME_SERVICES                 *mRT;
static EFI_SMM_BASE_PROTOCOL                *mSmmBase       = NULL;
static EFI_SMM_CONTROL_PROTOCOL             *mSmmRtControl  = NULL;
static EFI_HANDLE                           mSmmImageHandle;
static EFI_EVENT                            mSmmRuntimeNotifyEvent;
static EFI_EVENT                            mCpuIoEvent;
static BOOLEAN                              mRuntimeLibInitialized = FALSE;
static SMM_RUNTIME_COMMUNICATION_STRUCTURE  *mSmmRuntimeComStructure;
static EFI_HANDLE                           mSmmGenericNotifyHandle = NULL;
static VOID                                 *mEfiSmmRuntimeRegisteration;
static VOID                                 *mEfiCpuIoRegisteration;
static SMM_PRIVATE_DATA                     mSmmPrivateData;

EFI_SMM_RUNTIME_PROTOCOL                    *mSmmRT             = NULL;
BOOLEAN                                     mEfiLoadDriverSmm   = FALSE;
BOOLEAN                                     mEfiNoLoadDriverRt  = FALSE;
EFI_DEVICE_PATH_PROTOCOL                    *mFilePath          = NULL;
EFI_GUID
  SmmRtInstallGuid = { 0xf86875f0, 0x1b19, 0x4ce7, 0xa6, 0xd, 0xc5, 0xa8, 0x3e, 0xee, 0x4, 0xd4 };  // Rahul
//
// SMM Infrastructure Functions. Should be used internal to library only.
//
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

  if (DevicePath == NULL) {
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

BOOLEAN
EfiCompareDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath1,
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath2
  )
/*++

Routine Description:

Arguments:

Returns:

  None

--*/
// GC_TODO:    DevicePath1 - add argument and description to function comment
// GC_TODO:    DevicePath2 - add argument and description to function comment
{
  UINTN Size1;
  UINTN Size2;

  Size1 = EfiDevicePathSize (DevicePath1);
  Size2 = EfiDevicePathSize (DevicePath2);

  if (Size1 != Size2) {
    return FALSE;
  }

  if (EfiCompareMem (DevicePath1, DevicePath2, Size1)) {
    return FALSE;
  }

  return TRUE;
}

BOOLEAN
EfiInManagementInterrupt (
  VOID
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  None

Returns:

  GC_TODO: add return values

--*/
{
  BOOLEAN InSmm;

  if (mSmmBase == NULL) {
    InSmm = FALSE;
  } else {
    mSmmBase->InSmm (mSmmBase, &InSmm);
  }

  return InSmm;
}

EFI_SMM_RUNTIME_PROTOCOL *
GetSmmRt (
  IN  VOID
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:


Returns:

  GC_TODO: add return values

--*/
{
  if (mSmmRT == NULL) {
    if (EFI_ERROR (gBS->LocateProtocol (&gEfiSmmRuntimeProtocolGuid, NULL, &mSmmRT))) {
      mSmmRT = NULL;
    } else {
      if (mSmmRT->SmmRuntime.Hdr.Signature == EFI_RUNTIME_SERVICES_SIGNATURE) {
        mRT = &(mSmmRT->SmmRuntime);
      }
    }
  }

  return mSmmRT;
}

VOID
EFIAPI
SmmRuntimeGenericNotify (
  IN VOID                          *Context,
  IN EFI_SMM_SYSTEM_TABLE          *Smst,
  IN VOID                          *ChildRuntimeBuffer
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Context             - GC_TODO: add argument description
  Smst                - GC_TODO: add argument description
  ChildRuntimeBuffer  - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
{
  BOOLEAN *InRuntime;
  InRuntime   = Context;
  *InRuntime  = TRUE;
}
//
// SMM Runtime Driver Notification
//
VOID
EFIAPI
SmmRuntimeNotificationFunction (
  EFI_EVENT       Event,  // GC_TODO: add IN/OUT modifier to Event
  VOID            *Context  // GC_TODO: add IN/OUT modifier to Context
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Event   - GC_TODO: add argument description
  Context - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
{
  EFI_HANDLE  *SmmImageHandle;
  EFI_STATUS  Status;

  SmmImageHandle = (EFI_HANDLE *) Context;

  if (EFI_ERROR (gBS->LocateProtocol (&gEfiSmmBaseProtocolGuid, NULL, &mSmmBase))) {
    mSmmBase = NULL;
  }
  //
  // If in SMM, Locate and save the Smm Control protocol pointer in Boot Time so that it can
  // be converted and used in Runtime for generating an SMI from outside SMI.
  //
  if (EFI_ERROR (gBS->LocateProtocol (&gEfiSmmControlProtocolGuid, NULL, &mSmmRtControl))) {
    mSmmRtControl = NULL;
  }
  //
  // Register the Driver given that we have SMM infrastructure now.
  //
  if (GetSmmRt () != NULL) {
    Status = mSmmBase->Register (mSmmBase, mFilePath, NULL, 0, SmmImageHandle, FALSE);
    ASSERT_EFI_ERROR (Status);
  }
}

EFI_STATUS
RegisterSmmRuntimeDriver (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable,
  OUT EFI_HANDLE            *SmmImageHandle
  )
/*++

Routine Description:

  Registers a Driver with the SMM.

Arguments:

  SmmImageHandle  -   Image handle returned by the SMM driver.

Returns:

  None

--*/
// GC_TODO:    ImageHandle - add argument and description to function comment
// GC_TODO:    SystemTable - add argument and description to function comment
// GC_TODO:    EFI_SUCCESS - add return value to function comment
{
  //
  // Initialize SMM Runtime Infrastructure Services.
  //
  if ((!EfiInManagementInterrupt ()) && (mEfiLoadDriverSmm)) {
    RtEfiLibCreateProtocolNotifyEvent (
      &gEfiSmmRuntimeProtocolGuid,
      EFI_TPL_CALLBACK,
      SmmRuntimeNotificationFunction,
      SmmImageHandle,
      &mEfiSmmRuntimeRegisteration
      );
  }

  return EFI_SUCCESS;
}
//
// Generic Runtime Infrastructure Functions
//
VOID
EFIAPI
SmmRuntimeDriverExitBootServices (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
/*++

Routine Description:

  Set Runtime Global

Arguments:

  (Standard EFI notify event - EFI_EVENT_NOTIFY)

Returns:

  None

--*/
// GC_TODO:    Event - add argument and description to function comment
// GC_TODO:    Context - add argument and description to function comment
{
  Context = &mEfiAtRuntime;

  if (mSmmRtControl != NULL) {
    mSmmRT->ChildRuntimeBuffer->PrivateData.SmmRuntimeCallHandle = mSmmGenericNotifyHandle;
    mSmmRtControl->Trigger (mSmmRtControl, NULL, NULL, FALSE, 0);
  }
}

VOID
EFIAPI
CpuIoCallbackFunction (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
/*++

Routine Description:

  Updates the gCpuIo variable upon the availability of CPU IO Protocol.

Arguments:

  (Standard EFI notify event - EFI_EVENT_NOTIFY)

Returns:

  None

--*/
// GC_TODO:    Event - add argument and description to function comment
// GC_TODO:    Context - add argument and description to function comment
{
  EfiLocateProtocolInterface (&gEfiCpuIoProtocolGuid, NULL, &gCpuIo);
}

EFI_STATUS
EfiInitializeUtilsRuntimeDriverLib (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable,
  IN EFI_EVENT_NOTIFY     GoVirtualChildEvent
  )
/*++

Routine Description:

  Intialize runtime Driver Lib if it has not yet been initialized.

Arguments:

  (Standard EFI Image entry - EFI_IMAGE_ENTRY_POINT)

  GoVirtualChildEvent - Caller can register a virtual notification event.

Returns:

  EFI_STATUS always returns EFI_SUCCESS

--*/
// GC_TODO:    ImageHandle - add argument and description to function comment
// GC_TODO:    SystemTable - add argument and description to function comment
// GC_TODO:    EFI_ALREADY_STARTED - add return value to function comment
{
  EFI_STATUS            Status;
  EFI_BOOT_SERVICES     *BS;
  EFI_RUNTIME_SERVICES  *RT;
  EFI_HANDLE            Handle;
  EFI_HANDLE            *HandleBuffer;
  UINTN                 HandleCount;
  SMM_PRIVATE_DATA      *SmmPrivateData;
  UINTN                 i;

  Handle = NULL;
  if (mRuntimeLibInitialized) {
    return EFI_ALREADY_STARTED;
  }

  mRuntimeLibInitialized  = TRUE;

  gST                     = SystemTable;

  gBS                     = BS = SystemTable->BootServices;
  mRT                     = RT = SystemTable->RuntimeServices;

  if (EFI_ERROR (gBS->LocateProtocol (&gEfiSmmBaseProtocolGuid, NULL, &mSmmBase))) {
    mSmmBase = NULL;
  }

  if (EfiInManagementInterrupt ()) {
    GetSmmRt ();
    SystemTable->BootServices     = NULL;
    SystemTable->RuntimeServices  = mRT;
  }

  EfiInitializeRuntimeDriverLib (ImageHandle, SystemTable, GoVirtualChildEvent);
  RegisterSmmRuntimeDriver (ImageHandle, SystemTable, &mSmmImageHandle);

  SystemTable->BootServices     = BS;
  SystemTable->RuntimeServices  = RT;
  gBS = BS;

  if (EfiInManagementInterrupt ()) {
    if (EFI_ERROR (EfiLocateProtocolInterface (&gEfiCpuIoProtocolGuid, NULL, &gCpuIo))) {

      gBS->LocateProtocol (&gEfiCpuIoProtocolGuid, NULL, &gCpuIo);

      EfiRegisterProtocolCallback (
        CpuIoCallbackFunction,
        NULL,
        &gEfiCpuIoProtocolGuid,
        EFI_TPL_CALLBACK,
        &mEfiCpuIoRegisteration,
        &mCpuIoEvent
        );
    }
    //
    // Only Runtime Mode notification is required. No notification is required for Virtual Mode as
    // SMM is uneffected by it.
    //
    mSmmRT->RegisterSmmRuntimeChild (
              SmmRuntimeGenericNotify,
              &mEfiAtRuntime,
              &mSmmGenericNotifyHandle
              );
    //
    // Locate all PARTIES handle
    //
    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &SmmRtInstallGuid,
                    NULL,
                    &HandleCount,
                    &HandleBuffer
                    );
    for (i = 0; i < HandleCount; i++) {
      gBS->HandleProtocol (HandleBuffer[i], &SmmRtInstallGuid, &SmmPrivateData);
      if (EfiCompareDevicePath (SmmPrivateData->FilePath, mFilePath)) {
        *(SmmPrivateData->SmmHandle) = mSmmGenericNotifyHandle;
      }
    }

  } else {
    //
    // Place-Holder for SMM Notifications...
    //
    mSmmPrivateData.SmmHandle = &mSmmGenericNotifyHandle;
    mSmmPrivateData.FilePath  = mFilePath;
    gBS->InstallProtocolInterface (
          &Handle,
          &SmmRtInstallGuid,
          EFI_NATIVE_INTERFACE,
          &mSmmPrivateData
          );
    //
    // Register our ExitBootServices () notify function
    //
    Status = gBS->CreateEvent (
                    EFI_EVENT_SIGNAL_EXIT_BOOT_SERVICES,
                    EFI_TPL_NOTIFY,
                    SmmRuntimeDriverExitBootServices,
                    NULL,
                    &mSmmRuntimeNotifyEvent
                    );
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}
//
// Boot-Time Callable functions for Runtime infrastructure.
//
EFI_STATUS
EfiAllocateRuntimeMemoryPool (
  IN UINTN                          Size,
  OUT VOID                          **Buffer
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Size    - GC_TODO: add argument description
  Buffer  - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
{
  EFI_STATUS  Status;

  if (EfiInManagementInterrupt ()) {
    Status = mSmmBase->SmmAllocatePool (mSmmBase, EfiRuntimeServicesData, Size, Buffer);
  } else {
    Status = gBS->AllocatePool (EfiRuntimeServicesData, Size, Buffer);
  }

  if (Status == EFI_SUCCESS) {
    EfiZeroMem (*Buffer, Size);
  }

  return Status;
}

EFI_STATUS
EfiFreeRuntimeMemoryPool (
  IN VOID                          *Buffer
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Buffer  - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
{
  if (EfiInManagementInterrupt ()) {
    return mSmmBase->SmmFreePool (mSmmBase, Buffer);
  } else {
    return gBS->FreePool (Buffer);
  }
}

EFI_STATUS
EfiLocateProtocolHandleBuffers (
  IN EFI_GUID                     *Protocol,
  IN OUT UINTN                    *NumberHandles,
  OUT EFI_HANDLE                  **Buffer
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Protocol      - GC_TODO: add argument description
  NumberHandles - GC_TODO: add argument description
  Buffer        - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
{
  if (EfiInManagementInterrupt ()) {
    return GetSmmRt ()->LocateProtocolHandles (Protocol, Buffer, NumberHandles);
  } else {
    return gBS->LocateHandleBuffer (ByProtocol, Protocol, NULL, NumberHandles, Buffer);
  }
}

EFI_STATUS
EfiHandleProtocol (
  IN EFI_HANDLE               Handle,
  IN EFI_GUID                 *Protocol,
  OUT VOID                    **Interface
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Handle    - GC_TODO: add argument description
  Protocol  - GC_TODO: add argument description
  Interface - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
{
  if (EfiInManagementInterrupt ()) {
    return GetSmmRt ()->HandleProtocol (Handle, Protocol, Interface);
  } else {
    return gBS->HandleProtocol (Handle, Protocol, Interface);
  }
}

EFI_STATUS
EfiInstallProtocolInterface (
  IN OUT EFI_HANDLE           *Handle,
  IN EFI_GUID                 *Protocol,
  IN EFI_INTERFACE_TYPE       InterfaceType,
  IN VOID                     *Interface
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Handle        - GC_TODO: add argument description
  Protocol      - GC_TODO: add argument description
  InterfaceType - GC_TODO: add argument description
  Interface     - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
{
  if (EfiInManagementInterrupt ()) {
    return GetSmmRt ()->InstallProtocolInterface (Handle, Protocol, InterfaceType, Interface);
  } else {
    return gBS->InstallProtocolInterface (Handle, Protocol, InterfaceType, Interface);
  }
}

EFI_STATUS
EfiReinstallProtocolInterface (
  IN EFI_HANDLE               SmmProtocolHandle,
  IN EFI_GUID                 *Protocol,
  IN VOID                     *OldInterface,
  IN VOID                     *NewInterface
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  SmmProtocolHandle - GC_TODO: add argument description
  Protocol          - GC_TODO: add argument description
  OldInterface      - GC_TODO: add argument description
  NewInterface      - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
{
  if (EfiInManagementInterrupt ()) {
    return GetSmmRt ()->ReinstallProtocolInterface (SmmProtocolHandle, Protocol, OldInterface, NewInterface);
  } else {
    return gBS->ReinstallProtocolInterface (SmmProtocolHandle, Protocol, OldInterface, NewInterface);
  }
}

EFI_STATUS
EfiLocateProtocolInterface (
  EFI_GUID  *Protocol,
  VOID      *Registration, OPTIONAL
  VOID      **Interface
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Protocol      - GC_TODO: add argument description
  Registration  - GC_TODO: add argument description
  Interface     - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
{
  if (EfiInManagementInterrupt ()) {
    return GetSmmRt ()->LocateProtocol (Protocol, Registration, Interface);
  } else {
    return gBS->LocateProtocol (Protocol, Registration, Interface);
  }
}

EFI_STATUS
UninstallProtocolInterface (
  IN EFI_HANDLE               SmmProtocolHandle,
  IN EFI_GUID                 *Protocol,
  IN VOID                     *Interface
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  SmmProtocolHandle - GC_TODO: add argument description
  Protocol          - GC_TODO: add argument description
  Interface         - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
{
  if (EfiInManagementInterrupt ()) {
    return GetSmmRt ()->UninstallProtocolInterface (SmmProtocolHandle, Protocol, Interface);
  } else {
    return gBS->UninstallProtocolInterface (SmmProtocolHandle, Protocol, Interface);
  }
}

EFI_STATUS
EfiRegisterProtocolCallback (
  IN  EFI_EVENT_NOTIFY            CallbackFunction,
  IN  VOID                        *Context,
  IN  EFI_GUID                    *ProtocolGuid,
  IN  EFI_TPL                     NotifyTpl,
  OUT VOID                        **Registeration,
  OUT EFI_EVENT                   *Event
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  CallbackFunction  - GC_TODO: add argument description
  Context           - GC_TODO: add argument description
  ProtocolGuid      - GC_TODO: add argument description
  NotifyTpl         - GC_TODO: add argument description
  Registeration     - GC_TODO: add argument description
  Event             - GC_TODO: add argument description

Returns:

  EFI_SUCCESS - GC_TODO: Add description for return value

--*/
{
  EFI_STATUS  Status;

  if (EfiInManagementInterrupt ()) {
    return GetSmmRt ()->EnableProtocolNotify (CallbackFunction, Context, ProtocolGuid, Event);
  } else {
    Status = gBS->CreateEvent (EFI_EVENT_NOTIFY_SIGNAL, NotifyTpl, CallbackFunction, Context, Event);
    ASSERT_EFI_ERROR (Status);

    Status = gBS->RegisterProtocolNotify (ProtocolGuid, *Event, Registeration);
    ASSERT_EFI_ERROR (Status);
    return EFI_SUCCESS;
  }
}

EFI_STATUS
EfiSignalProtocolEvent (
  EFI_EVENT                   Event
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Event - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
{
  if (EfiInManagementInterrupt ()) {
    return GetSmmRt ()->SignalProtocol (Event);
  } else {
    return gBS->SignalEvent (Event);
  }
}

EFI_STATUS
EfiInstallVendorConfigurationTable (
  IN EFI_GUID                 *Guid,
  IN VOID                     *Table
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Guid  - GC_TODO: add argument description
  Table - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
{
  if (EfiInManagementInterrupt ()) {
    return GetSmmRt ()->InstallVendorConfigTable (Guid, Table);
  } else {
    return gBS->InstallConfigurationTable (Guid, Table);
  }
}

EFI_STATUS
EfiGetVendorConfigurationTable (
  IN EFI_GUID                 *Guid,
  OUT VOID                    **Table
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Guid  - GC_TODO: add argument description
  Table - GC_TODO: add argument description

Returns:

  EFI_SUCCESS - GC_TODO: Add description for return value
  EFI_NOT_FOUND - GC_TODO: Add description for return value

--*/
{
  UINTN i;

  if (EfiInManagementInterrupt ()) {
    return GetSmmRt ()->GetVendorConfigTable (Guid, Table);
  } else {
    for (i = 0; i < gST->NumberOfTableEntries; i++) {
      if (EfiCompareGuid (&gST->ConfigurationTable[i].VendorGuid, Guid)) {
        *Table = gST->ConfigurationTable[i].VendorTable;
        return EFI_SUCCESS;
      }
    }
  }

  return EFI_NOT_FOUND;
}
