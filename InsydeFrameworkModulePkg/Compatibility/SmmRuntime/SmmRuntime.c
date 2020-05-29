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
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  SmmRuntime.c

Abstract:

  SMM Runtime Infrastructutre for the IA32 Runtime drivers.

--*/

#include "SmmRuntime.h"

EFI_SMM_RT_GLOBAL               *SmmRtGlobal;
EFI_SMM_SYSTEM_TABLE            *mSmst;

EFI_SMM_RUNTIME_PROTOCOL        *mSmmRT;

EFI_GUID  gEfiSmmRuntimeProtocolGuid = EFI_SMM_RUNTIME_PROTOCOL_GUID;
EFI_STATUS
CheckCallbacks (
  IN  EFI_GUID    *Protocol
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Protocol  - GC_TODO: add argument description

Returns:

  EFI_SUCCESS - GC_TODO: Add description for return value

--*/
{
  UINTN i;

  //
  // Check if any notifications are pending.
  //
  for (i = 0; i < MAX_CALLBACK; i++) {
    if (SmmRtGlobal->Callback[i].Valid) {
      if (EfiCompareGuid (Protocol, &SmmRtGlobal->Callback[i].ProtocolGuid)) {
        SmmRtGlobal->Callback[i].CallbackFunction (
                                  &SmmRtGlobal->Callback[i],
                                  SmmRtGlobal->Callback[i].Context
                                  );
      }
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
SmmSignalCallback (
  IN  EFI_EVENT   Event
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Event - GC_TODO: add argument description

Returns:

  EFI_SUCCESS - GC_TODO: Add description for return value

--*/
{
  EFI_SMM_CALLBACK_SERVICES *Callback;

  Callback = (EFI_SMM_CALLBACK_SERVICES *) Event;

  if (Callback->CallbackFunction != NULL) {
    Callback->CallbackFunction (Event, Callback->Context);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
SmmEnableProtocolNotifyCallback (
  EFI_EVENT_NOTIFY            CallbackFunction,
  VOID                        *Context,
  EFI_GUID                    *ProtocolGuid,
  EFI_EVENT                   *Event
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  CallbackFunction  - GC_TODO: add argument description
  Context           - GC_TODO: add argument description
  ProtocolGuid      - GC_TODO: add argument description
  Event             - GC_TODO: add argument description

Returns:

  EFI_SUCCESS - GC_TODO: Add description for return value
  EFI_OUT_OF_RESOURCES - GC_TODO: Add description for return value

--*/
{
  UINTN i;

  for (i = 0; i < MAX_CALLBACK; i++) {
    if (!SmmRtGlobal->Callback[i].Valid) {
      SmmRtGlobal->Callback[i].Context          = Context;
      SmmRtGlobal->Callback[i].CallbackFunction = CallbackFunction;
      EfiCopyMem (&SmmRtGlobal->Callback[i].ProtocolGuid, ProtocolGuid, sizeof (EFI_GUID));
      SmmRtGlobal->Callback[i].Valid  = TRUE;
      *Event                          = (EFI_EVENT) & SmmRtGlobal->Callback[i];
      return EFI_SUCCESS;
    }
  }

  return EFI_OUT_OF_RESOURCES;
}

EFI_STATUS
SmmDisableProtocolNotifyCallback (
  IN  EFI_EVENT   Event
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Event - GC_TODO: add argument description

Returns:

  EFI_SUCCESS - GC_TODO: Add description for return value

--*/
{
  EFI_SMM_CALLBACK_SERVICES *Callback;

  Callback        = (EFI_SMM_CALLBACK_SERVICES *) Event;

  Callback->Valid = FALSE;

  return EFI_SUCCESS;
}

EFI_STATUS
CheckServiceIndex (
  IN UINTN     Index,
  IN EFI_GUID  *Protocol,
  OUT VOID     **Interface
  )
/*++

  Routine Description:
    This Function gets the protocol interface related with the Index and updates the
    internal data index when protocol match is found.

  Arguments:
    Index                 - Relative Index where protocol match should be done
    Protocol              - Protocol that should be matched with at the input Index.
    Interface             - Pointer to the Interface that matches with the protocol at a given Index.

  Returns:
    EFI_SUCCESS           - Protocol Match done successfully.
    EFI_NOT_FOUND         - Protocol Match couldn't be done.

--*/
{
  if (SmmRtGlobal->Services[Index].Valid) {
    if (EfiCompareGuid (Protocol, &SmmRtGlobal->Services[Index].ProtocolGuid)) {
      *Interface                = SmmRtGlobal->Services[Index].Protocol;
      SmmRtGlobal->CurrentIndex = Index;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
LocateSmmProtocol (
  EFI_GUID  *Protocol,
  VOID      *Registration, OPTIONAL
  VOID      **Interface
  )
/*++

  Routine Description:
    This Function locates the protocol interrface that satisfy the protocol GUID defination

  Arguments:
    Protocol              - Protocol GUID.
    Registration          - Optional per EFI specifications. Not required in SMM scope.
    Interface             - Pointer to the Interface that contains the protocol.

  Returns:
    EFI_SUCCESS           - Protocol Interface located within SMM scope.

--*/
// GC_TODO:    EFI_NOT_FOUND - add return value to function comment
{
  UINTN i;

  for (i = SmmRtGlobal->CurrentIndex; i < MAX_SMM_PROTOCOL; i++) {
    if (CheckServiceIndex (i, Protocol, Interface) == EFI_SUCCESS) {
      return EFI_SUCCESS;
    }
  }

  for (i = 0; i < SmmRtGlobal->CurrentIndex; i++) {
    if (CheckServiceIndex (i, Protocol, Interface) == EFI_SUCCESS) {
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
LocateSmmProtocolHandles (
  IN  EFI_GUID    *Protocol,
  OUT EFI_HANDLE  **Handles,
  OUT UINTN       *HandlesCount
  )
/*++

  Routine Description:
    This Function get the handles for a specific protocol.

  Arguments:
    Protocol              - Protocol GUID.
    Handles               - Array of handles that satisfy the protocol defination.
    HandlesCount          - Number of handles found

  Returns:
    EFI_SUCCESS           - Protocol handles located within SMM scope.

--*/
{
  UINTN       Index;
  UINTN       NumHandles;
  UINTN       Size;
  EFI_HANDLE  *Buffer;
  EFI_STATUS  Status;
  
  NumHandles  = 0;
  Buffer      = NULL;

  for (Index = 0; Index < MAX_SMM_PROTOCOL; Index++) {
    if (SmmRtGlobal->Services[Index].Valid) {
      if (EfiCompareGuid (Protocol, &SmmRtGlobal->Services[Index].ProtocolGuid)) {
        SmmRtGlobal->HandleBuffer[NumHandles] = SmmRtGlobal->Services[Index].Handle;
        NumHandles++;
      }
    }
  }

  if (!NumHandles) {
    return EFI_NOT_FOUND;
  }

  Size = NumHandles * sizeof (EFI_HANDLE);
  if (Size > 0) {
    Status =  mSmst->SmmAllocatePool (
                       EfiRuntimeServicesData,
                       Size,
                       (void **)&Buffer
                       );
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    }

    EfiCommonLibCopyMem (Buffer, SmmRtGlobal->HandleBuffer, Size);
    *Handles      = Buffer;
    *HandlesCount = NumHandles;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
SmmHandleProtocol (
  IN EFI_HANDLE               Handle,
  IN EFI_GUID                 *Protocol,
  OUT VOID                    **Interface
  )
/*++

  Routine Description:
    This Function gets the interface pointer based upon the handle

  Arguments:
    Handle                - Protocol handle that is Installed within SMM space.
    Protocol              - Protocol GUID.
    Interface             - Interface pointer that points to a protocol.

  Returns:
    EFI_SUCCESS           - Protocol Interface found within SMM scope.
    EFI_NOT_FOUND         - Protocol not found.

--*/
{
  UINTN                   Index;

  for (Index = 0; Index < MAX_SMM_PROTOCOL; Index++) {
    if ((SmmRtGlobal->Services[Index].Valid) && 
        (SmmRtGlobal->Services[Index].Handle == Handle) && 
        (EfiCompareGuid (Protocol, &SmmRtGlobal->Services[Index].ProtocolGuid))) {
      *Interface = SmmRtGlobal->Services[Index].Protocol;
      return EFI_SUCCESS;  
    }
  }
  return EFI_NOT_FOUND;
}

EFI_STATUS
InstallSmmProtocolInterface (
  IN OUT EFI_HANDLE           *Handle,
  IN EFI_GUID                 *Protocol,
  IN EFI_INTERFACE_TYPE       InterfaceType,
  IN VOID                     *Interface
  )
/*++

  Routine Description:
    This Function Installs the protocol interface within SMM scope.

  Arguments:
    Handle                - Protocol handle that is to be Installed within SMM space. On NULL we gat a new Protocol
                            Handle
    Protocol              - Protocol GUID that is to be Installed.
    InterfaceType         - Type of Interface per EFI specification.
    Interface             - Interface that needs to be Installed.

  Returns:
    EFI_SUCCESS           - Protocol Installed within SMM scope.
    EFI_OUT_OF_RESOURCES  - No more resources available to install another interface.

--*/
{
  UINTN i;

  for (i = 0; i < MAX_SMM_PROTOCOL; i++) {
    if (!SmmRtGlobal->Services[i].Valid) {
      SmmRtGlobal->Services[i].InterfaceType  = InterfaceType;
      SmmRtGlobal->Services[i].Protocol       = Interface;
      EfiCopyMem (&SmmRtGlobal->Services[i].ProtocolGuid, Protocol, sizeof (EFI_GUID));
      SmmRtGlobal->Services[i].Valid = TRUE;
      if (*Handle == NULL) {
        SmmRtGlobal->Services[i].Handle = &SmmRtGlobal->Services[i];
        *Handle                         = SmmRtGlobal->Services[i].Handle;
      } else {
        SmmRtGlobal->Services[i].Handle = *Handle;
      }

      CheckCallbacks (Protocol);
      return EFI_SUCCESS;
    }
  }

  return EFI_OUT_OF_RESOURCES;
}

EFI_STATUS
ReinstallSmmProtocolInterface (
  IN EFI_HANDLE               SmmProtocolHandle,
  IN EFI_GUID                 *Protocol,
  IN VOID                     *OldInterface,
  IN VOID                     *NewInterface
  )
/*++

  Routine Description:
    This Function Re-Installs the protocol interface within SMM scope.

  Arguments:
    SmmProtocolHandle     - Protocol handle that is to be Un-Installed.
    Protocol              - Protocol GUID that is to be Un-Installed.
    OldInterface          - Current Interface that needs to be Re-Installed.
    NewInterface          - New Interface that needs to be Re-Installed over OldInterface.

  Returns:
    EFI_SUCCESS           - Protocol Re-Installed within SMM scope.
    EFI_NOT_FOUND         - Old Interface Not Found.

--*/
{
  UINTN i;

  for (i = 0; i < MAX_SMM_PROTOCOL; i++) {
    if (SmmRtGlobal->Services[i].Valid) {
      if (SmmProtocolHandle == SmmRtGlobal->Services[i].Handle) {
        if (OldInterface == SmmRtGlobal->Services[i].Protocol) {
          SmmRtGlobal->Services[i].Protocol = NewInterface;
          CheckCallbacks (NewInterface);
          return EFI_SUCCESS;
        }
      }
    }
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
UninstallSmmProtocolInterface (
  IN EFI_HANDLE               SmmProtocolHandle,
  IN EFI_GUID                 *Protocol,
  IN VOID                     *Interface
  )
/*++

  Routine Description:
    This Function Un-Installs the protocol interface from within SMM scope.

  Arguments:
    SmmProtocolHandle     - Protocol handle that is to be Un-Installed.
    Protocol              - Protocol GUID that is to be Un-Installed.
    Interface             - Interface Pointer that needs to be Un-Installed.
  Returns:
    EFI_SUCCESS           - Protocol Uninstalled from within SMM scope.
    EFI_NOT_FOUND         - Protocol Not Found.

--*/
{
  UINTN i;

  for (i = 0; i < MAX_SMM_PROTOCOL; i++) {
    if (SmmRtGlobal->Services[i].Valid) {
      if (SmmProtocolHandle == SmmRtGlobal->Services[i].Handle) {
        if (EfiCompareGuid (Protocol, &SmmRtGlobal->Services[i].ProtocolGuid)) {
          if (Interface == SmmRtGlobal->Services[i].Protocol) {
            SmmRtGlobal->Services[i].Valid = FALSE;
            return EFI_SUCCESS;
          }
        }
      }
    }
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
InstallVendorConfigurationTable (
  IN EFI_GUID                 *Guid,
  IN VOID                     *Table
  )
/*++

  Routine Description:
    This Function installs the configuration table within SMM scope.

  Arguments:
    Guid                  - Vandor GUID
    Table                 - Pointer to the Table. This table should be located within SMM scope.

  Returns:
    EFI_SUCCESS           - Vendor table Found
    EFI_OUT_OF_RESOURCES  - Vendor Table cannot be registered because no more SMM resources are there.

--*/
{
  UINTN i;

  //
  // Check if the vendor GUID already exists.
  //
  for (i = 0; i < MAX_SMM_PROTOCOL; i++) {
    if (SmmRtGlobal->ConfigTable[i].VendorTable != NULL) {
      if (EfiCompareGuid (&SmmRtGlobal->ConfigTable[i].VendorGuid, Guid)) {
        SmmRtGlobal->ConfigTable[i].VendorTable = Table;
        return EFI_SUCCESS;
      }
    }
  }
  //
  // If the Vendor GUID don't exist, Find an empty slot.
  //
  for (i = 0; i < MAX_SMM_PROTOCOL; i++) {
    if (SmmRtGlobal->ConfigTable[i].VendorTable == NULL) {
      EfiCopyMem (&SmmRtGlobal->ConfigTable[i].VendorGuid, Guid, sizeof (EFI_GUID));
      SmmRtGlobal->ConfigTable[i].VendorTable = Table;
      return EFI_SUCCESS;
    }
  }

  return EFI_OUT_OF_RESOURCES;
}

EFI_STATUS
GetVendorConfigurationTable (
  IN EFI_GUID                 *Guid,
  OUT VOID                    **Table
  )
/*++

  Routine Description:
    This Function gets the configuration table that has been registered within SMM scope.

  Arguments:
    Guid                  - Vandor GUID
    Table                 - Pointer to the Table. This table should be located within SMM scope.

  Returns:
    EFI_SUCCESS           - Vendor table Found
    EFI_NOT_FOUND         - Vendor Table not found.

--*/
{
  UINTN i;

  //
  // Check if the vendor GUID already exists.
  //
  for (i = 0; i < MAX_SMM_PROTOCOL; i++) {
    if (SmmRtGlobal->ConfigTable[i].VendorTable != NULL) {
      if (EfiCompareGuid (&SmmRtGlobal->ConfigTable[i].VendorGuid, Guid)) {
        *Table = SmmRtGlobal->ConfigTable[i].VendorTable;
        return EFI_SUCCESS;
      }
    }
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
EfiRegisterRuntimeCallback (
  IN EFI_SMM_RUNTIME_CALLBACK               SmmRuntimeCallback,
  IN VOID                                   *Context,
  OUT EFI_HANDLE                            *SmmRuntimeCallHandle
  )
/*++

  Routine Description:
    This Function Registers the Callback Function from executing.

  Arguments:
    SmmRuntimeCallback    - Callback Function Name.
    Context               - Context, that is saved and restored when callback function is called.
    ChildRuntimeBuffer    - Returns the pointer to the messaging buffer that is to be used for SMM communication.
    SmmRuntimeCallHandle  - EFI Handle for the callback function. This handle is valid within SMM scope only.

  Returns:
    EFI_SUCCESS           - Callback Function Registered Successfully.
    EFI_OUT_OF_RESOURCES  - All SMM resources have been used up. So more callback functions cannot
                            be registered.
--*/
{
  UINTN i;

  for (i = 0; i < MAX_SM_RT_CALLBACK; i++) {
    if (SmmRtGlobal->RtCallback[i].CallbackFunction == NULL) {
      SmmRtGlobal->RtCallback[i].CallbackFunction = SmmRuntimeCallback;
      SmmRtGlobal->RtCallback[i].Context          = Context;
      *SmmRuntimeCallHandle                       = &SmmRtGlobal->RtCallback[i];
      return EFI_SUCCESS;
    }
  }

  return EFI_OUT_OF_RESOURCES;
}

EFI_STATUS
EfiUnRegisterRuntimeCallback (
  IN EFI_SMM_RUNTIME_CALLBACK       SmmRuntimeCallback
  )
/*++

  Routine Description:
    This Function Un-Registers the Callback Function from executing.

  Arguments:
    SmmRuntimeCallback  - Callback Function Name.

  Returns:
    EFI_SUCCESS         - Callback Function UnRegistered.
    EFI_NOT_FOUND       - Callback Function not found.
--*/
{
  UINTN i;

  for (i = 0; i < MAX_SM_RT_CALLBACK; i++) {
    if (SmmRtGlobal->RtCallback[i].CallbackFunction == SmmRuntimeCallback) {
      SmmRtGlobal->RtCallback[i].CallbackFunction = NULL;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
SmmRuntimeManagementCallback (
  IN EFI_HANDLE             SmmImageHandle,
  IN OUT VOID               *CommunicationBuffer,
  IN OUT UINTN              *SourceSize
  )
/*++

  Routine Description:
    This Function executes the Callback Function. This function is called at every SMI occurance.
    At the SMI occurance, it investigates if somebody asked for the Runtime Service. On a valid
    RT Service request, it executes the callback function.

  Arguments:
    SmmImageHandle      - Handle for the smm image of this driver
    Smst                - Pointer to the SMM System Table
    CommunicationBuffer - Pointer to the buffer that contains the communication Message
    Source Size         - Size of the memory image to be used for handler.

  Returns:
    EFI_SUCCESS         - Callback Function Executed
--*/
// GC_TODO:    SourceSize - add argument and description to function comment
{
  SMM_RUNTIME_COMMUNICATION_STRUCTURE *SmmRtStruct;
  EFI_SMM_RT_CALLBACK_SERVICES        *RtServices;

  RtServices  = NULL;

  SmmRtStruct = (SMM_RUNTIME_COMMUNICATION_STRUCTURE *) CommunicationBuffer;
  RtServices  = (EFI_SMM_RT_CALLBACK_SERVICES *) SmmRtStruct->PrivateData.SmmRuntimeCallHandle;

  if (RtServices != NULL) {
    RtServices->CallbackFunction (RtServices->Context, mSmst, (VOID *) &SmmRtStruct->PrivateData);
    SmmRtStruct->PrivateData.SmmRuntimeCallHandle = NULL;
  }

  return EFI_SUCCESS;
}

UINTN
DevicePathSize (
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

EFI_DEVICE_PATH_PROTOCOL *
AppendDevicePath (
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
  Size1 = DevicePathSize (Src1);
  Size2 = DevicePathSize (Src2);
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

VOID
ResetSystem (
  IN EFI_RESET_TYPE   ResetType,
  IN EFI_STATUS       ResetStatus,
  IN UINTN            DataSize,
  IN CHAR16           *ResetData OPTIONAL
  )
/*++

Routine Description:

  Reset the system.

Arguments:
  
    ResetType - warm or cold
    ResetStatus - possible cause of reset
    DataSize - Size of ResetData in bytes
    ResetData - Optional Unicode string
    For details, see efiapi.h

Returns:
  Does not return if the reset takes place.
  EFI_INVALID_PARAMETER   If ResetType is invalid.

--*/
{
  UINT8       InitialData;
  UINT8       OutputData;
   
  switch(ResetType) {
    case EfiResetWarm:
      InitialData  = 0x00;//V_PCH_RST_CNT_SOFTSTARTSTATE;
      OutputData   = 0x04;//V_PCH_RST_CNT_SOFTRESET;
      break;

    case EfiResetCold:
       InitialData  = 0x02;//V_PCH_RST_CNT_HARDSTARTSTATE;
       OutputData   = 0x06;//V_PCH_RST_CNT_HARDRESET;    
      break;
      
    default:
      return;
  }

  IoOutput8 ((UINT16)0xCF9, (UINT8)InitialData);  
  IoOutput8 ((UINT16)0xCF9, (UINT8)OutputData);    
  //
  // Given we should have reset getting here would be bad
  //
  EFI_DEADLOOP ();
}
EFI_STATUS
SmmRuntimeInitialize (
  IN EFI_HANDLE               ImageHandle,
  IN EFI_SYSTEM_TABLE         *SystemTable
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  ImageHandle - GC_TODO: add argument description
  SystemTable - GC_TODO: add argument description

Returns:

  EFI_SUCCESS - GC_TODO: Add description for return value
  EFI_SUCCESS - GC_TODO: Add description for return value

--*/
{
  EFI_SMM_BASE_PROTOCOL     *SmmBase;
  EFI_STATUS                Status;
  EFI_HANDLE                SmmImageHandle;
  EFI_HANDLE                Handle;
  UINTN                     Size;
  BOOLEAN                   InSmm;
  EFI_SMM_RUNTIME_PROTOCOL  *SmmRT;
  EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;
  EFI_DEVICE_PATH_PROTOCOL  *CompleteFilePath;
  EFI_DEVICE_PATH_PROTOCOL  *ImageDevicePath;
  EFI_SMM_RT_GLOBAL         *RtGlobal;

  SmmImageHandle  = NULL;
  RtGlobal        = NULL;

  gST             = SystemTable;
  gBS             = SystemTable->BootServices;

  Status          = gBS->LocateProtocol (&gEfiSmmBaseProtocolGuid, NULL, &SmmBase);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SmmBase->InSmm (SmmBase, &InSmm);

  SmmBase->GetSmstLocation (SmmBase, &mSmst);

  if (!InSmm) {
    Status = gBS->HandleProtocol (
                    ImageHandle,
                    &gEfiLoadedImageProtocolGuid,
                    &LoadedImage
                    );
    ASSERT_EFI_ERROR (Status);
    //
    // If outside the SMM scope, call a function to register this driver in SMM space also.
    // Not all parts of the code runs both in SMM & Non-SMM space. Some parts only RUN in SMM
    // Space and some do not as they may not be required.
    //
    Status = gBS->HandleProtocol (
                    LoadedImage->DeviceHandle,
                    &gEfiDevicePathProtocolGuid,
                    (VOID *) &ImageDevicePath
                    );
    ASSERT_EFI_ERROR (Status);

    CompleteFilePath = AppendDevicePath (
                        ImageDevicePath,
                        LoadedImage->FilePath
                        );

    SmmBase->Register (SmmBase, CompleteFilePath, NULL, 0, &SmmImageHandle, FALSE);

    if (EFI_ERROR (gBS->LocateProtocol (&gEfiSmmRuntimeProtocolGuid, NULL, &SmmRT))) {
      return EFI_SUCCESS;
    }

    SmmRtGlobal = INSTANCE_FROM_EFI_SMM_RT_THIS (SmmRT);

  } else {

    Status = SmmBase->SmmAllocatePool (SmmBase, EfiRuntimeServicesData, sizeof (EFI_SMM_RT_GLOBAL), &SmmRtGlobal);
    if (Status == EFI_SUCCESS) {
      EfiZeroMem (SmmRtGlobal, sizeof (EFI_SMM_RT_GLOBAL));
    }

    Status = gBS->AllocatePool (
                    EfiReservedMemoryType,
                    sizeof (SMM_RUNTIME_COMMUNICATION_STRUCTURE),
                    &SmmRtGlobal->SmmRtServices.ChildRuntimeBuffer
                    );
    ASSERT_EFI_ERROR (Status);
    EfiZeroMem (SmmRtGlobal->SmmRtServices.ChildRuntimeBuffer, sizeof (SMM_RUNTIME_COMMUNICATION_STRUCTURE));

    Status = gBS->AllocatePool (EfiReservedMemoryType, sizeof (EFI_SMM_RT_GLOBAL), &RtGlobal);
    ASSERT_EFI_ERROR (Status);
    EfiZeroMem (RtGlobal, sizeof (EFI_SMM_RT_GLOBAL));

    //
    // Register the Callback function that acts as a parent dispatcher for all the Runtime Functions.
    // Any RT function call should be a child of this callback.
    //
    SmmRtGlobal->SmmRtServices.ChildRuntimeBuffer->MessageLength = sizeof (SMM_RUNTIME_COMMUNICATION_STRUCTURE);
    SmmBase->RegisterCallback (SmmBase, ImageHandle, SmmRuntimeManagementCallback, FALSE, FALSE);
    SmmRtGlobal->CallbackEntryPoint = SmmRuntimeManagementCallback;

    Size = sizeof (SMM_RUNTIME_COMMUNICATION_STRUCTURE);
    SmmBase->Communicate (SmmBase, ImageHandle, (VOID *) SmmRtGlobal->SmmRtServices.ChildRuntimeBuffer, &Size);

    //
    // Install the services that are called within the SMM scope. All Runtime Libraries will use these
    // functions to create the services required for driver bindings within the SMM space.
    //
    SmmRtGlobal->Signature  = SMM_RT_SIGNATURE;
    SmmRtGlobal->SmmRtServices.InstallProtocolInterface   = InstallSmmProtocolInterface;
    SmmRtGlobal->SmmRtServices.LocateProtocol             = LocateSmmProtocol;
    SmmRtGlobal->SmmRtServices.ReinstallProtocolInterface = ReinstallSmmProtocolInterface;
    SmmRtGlobal->SmmRtServices.UninstallProtocolInterface = UninstallSmmProtocolInterface;
    SmmRtGlobal->SmmRtServices.SignalProtocol             = SmmSignalCallback;
    SmmRtGlobal->SmmRtServices.EnableProtocolNotify       = SmmEnableProtocolNotifyCallback;
    SmmRtGlobal->SmmRtServices.DisableProtocolNotify      = SmmDisableProtocolNotifyCallback;
    SmmRtGlobal->SmmRtServices.LocateProtocolHandles      = LocateSmmProtocolHandles;
    SmmRtGlobal->SmmRtServices.HandleProtocol             = SmmHandleProtocol;
    SmmRtGlobal->SmmRtServices.InstallVendorConfigTable   = InstallVendorConfigurationTable;
    SmmRtGlobal->SmmRtServices.GetVendorConfigTable       = GetVendorConfigurationTable;
    SmmRtGlobal->SmmRtServices.RegisterSmmRuntimeChild    = EfiRegisterRuntimeCallback;
    SmmRtGlobal->SmmRtServices.UnRegisterSmmRuntimeChild  = EfiUnRegisterRuntimeCallback;

    SmmRtGlobal->SmmRtServices.SmmRuntime.Hdr.Signature   = 0;
    SmmRtGlobal->SmmRtServices.SmmRuntime.Hdr.Revision    = EFI_RUNTIME_SERVICES_REVISION;
    SmmRtGlobal->SmmRtServices.SmmRuntime.Hdr.HeaderSize  = sizeof (EFI_TABLE_HEADER);
    SmmRtGlobal->SmmRtServices.SmmRuntime.ResetSystem     = ResetSystem;
    //
    // Install the Driver within the SMM scope so that drivers created within the SMM scope can find it
    // and use the functionality. Runtime Library searches the protocol for performing the above functions.
    //
    Handle = NULL;
    Status = InstallSmmProtocolInterface (
              &Handle,
              &gEfiSmmRuntimeProtocolGuid,
              EFI_NATIVE_INTERFACE,
              &(SmmRtGlobal->SmmRtServices)
              );
    ASSERT_EFI_ERROR (Status);
    mSmmRT = &SmmRtGlobal->SmmRtServices;

    EfiInitializeSmmDriverLib (ImageHandle, SystemTable);

    //
    //  Install the Protocol Interface in the Boot Time Space. This is requires to satisfy the
    //  dependency within the drivers that are dependent upon Smm Runtime Driver.
    //
    Handle = NULL;
    EfiCopyMem (RtGlobal, SmmRtGlobal, sizeof (EFI_SMM_RT_GLOBAL));

    Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &gEfiSmmRuntimeProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &(RtGlobal->SmmRtServices)
                    );
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}
