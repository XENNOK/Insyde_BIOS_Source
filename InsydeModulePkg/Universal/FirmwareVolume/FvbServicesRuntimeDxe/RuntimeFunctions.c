/** @file
  Provide support functions for FVB services in protected mode (including
  EFI boot time and EFI runtime.)

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

#include "RuntimeFunctions.h"
#include "CommonFunctions.h"

SMM_FVB_BUFFER         *mSmmFvbBuffer;
SMM_FVB_BUFFER         *mSmmPhyFvbBuffer;
BOOLEAN                mGoneVirtual;
BOOLEAN                mFvbAccessThroughSmi;
extern EFI_GUID        gEfiAlternateFvBlockGuid;

FV_MEMMAP_DEVICE_PATH mFvMemmapDevicePathTemplate = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_MEMMAP_DP,
      {
        (UINT8)(sizeof (MEMMAP_DEVICE_PATH)),
        (UINT8)(sizeof (MEMMAP_DEVICE_PATH) >> 8)
      }
    },
    EfiMemoryMappedIO,
    (EFI_PHYSICAL_ADDRESS) 0,
    (EFI_PHYSICAL_ADDRESS) 0,
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      END_DEVICE_PATH_LENGTH,
      0
    }
  }
};

FV_PIWG_DEVICE_PATH mFvPIWGDevicePathTemplate = {
  {
    {
      MEDIA_DEVICE_PATH,
      MEDIA_PIWG_FW_VOL_DP,
      {
        (UINT8)(sizeof (MEDIA_FW_VOL_DEVICE_PATH)),
        (UINT8)(sizeof (MEDIA_FW_VOL_DEVICE_PATH) >> 8)
      }
    },
    { 0 }
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      END_DEVICE_PATH_LENGTH,
      0
    }
  }
};

EFI_FW_VOL_BLOCK_DEVICE mFvbDeviceTemplate = {
  FVB_DEVICE_SIGNATURE,
  NULL,
  0,
  {
    FvbProtocolGetAttributes,
    FvbProtocolSetAttributes,
    FvbProtocolGetPhysicalAddress,
    FvbProtocolGetBlockSize,
    FvbProtocolRead,
    FvbProtocolWrite,
    FvbProtocolEraseBlocks,
    NULL
  }
};

/**
  Creates and returns a notification event and registers that event with all the protocol
  instances specified by ProtocolGuid.

  This function causes the notification function to be executed for every protocol of type
  ProtocolGuid instance that exists in the system when this function is invoked. In addition,
  every time a protocol of type ProtocolGuid instance is installed or reinstalled, the notification
  function is also executed. This function returns the notification event that was created.
  If ProtocolGuid is NULL, then ASSERT().
  If NotifyTpl is not a legal TPL value, then ASSERT().
  If NotifyFunction is NULL, then ASSERT().
  If Registration is NULL, then ASSERT().


  @param[in]  ProtocolGuid    Supplies GUID of the protocol upon whose installation the event is fired.
  @param[in]  NotifyTpl       Supplies the task priority level of the event notifications.
  @param[in]  NotifyFunction  Supplies the function to notify when the event is signaled.
  @param[in]  NotifyContext   The context parameter to pass to NotifyFunction.
  @param[out] Registration    A pointer to a memory location to receive the registration value.
                              This value is passed to LocateHandle() to obtain new handles that
                              have been added that support the ProtocolGuid-specified protocol.

  @return EFI_EVENT           The notification event that was created.
**/
STATIC
EFI_EVENT
EFIAPI
InternalCreateProtocolNotifyEvent(
  IN  EFI_GUID          *ProtocolGuid,
  IN  EFI_TPL           NotifyTpl,
  IN  EFI_EVENT_NOTIFY  NotifyFunction,
  IN  VOID              *NotifyContext,  OPTIONAL
  OUT VOID              **Registration
  )
{
  EFI_STATUS  Status;
  EFI_EVENT   Event;

  ASSERT (ProtocolGuid != NULL);
  ASSERT (NotifyFunction != NULL);
  ASSERT (Registration != NULL);

  //
  // Create the event
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  NotifyTpl,
                  NotifyFunction,
                  NotifyContext,
                  &Event
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Register for protocol notifications on this event
  //

  Status = gBS->RegisterProtocolNotify (
                  ProtocolGuid,
                  Event,
                  Registration
                  );
  ASSERT_EFI_ERROR (Status);

  return Event;
}

/**
  Internal function to allocate buffer for accessing FVB through SMI.

  @return EFI_SUCCESS           Allocate resources successful.
  @return EFI_OUT_OF_RESOURCES  Allocate pool failed.
**/
STATIC
EFI_STATUS
InitializeResourceForFvbAcessTroughSmi (
  VOID
  )
{
  //
  // Allocate SmmFvbBuffer for access FVB through SMI
  //
  mSmmFvbBuffer = FvbAllocateZeroBuffer (
                    sizeof (SMM_FVB_BUFFER) + (UINTN) PcdGet32 (PcdFlashNvStorageFtwSpareSize),
                    TRUE
                    );
  ASSERT (mSmmFvbBuffer != NULL);
  if (mSmmFvbBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  mSmmPhyFvbBuffer = mSmmFvbBuffer;

  return EFI_SUCCESS;
}



/**
  Callback function to provide FVB access interface for FVB access through SMI.

  After SMM related services for accessing FVB through, system will
  SMM code will signal this callback function to initialize related code
  and provide FVB access interface in protected mode.

  @param[in] Event    The Event that is being processed.
  @param[in] Context  The Event Context.
**/
STATIC
VOID
EFIAPI
EnableFvbAccessThroughSmiEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS       Status;

  mFvbAccessThroughSmi = TRUE;

  Status = InitializeResourceForFvbAcessTroughSmi ();
  ASSERT_EFI_ERROR (Status);

  Status = InitializeFVbServices ();
  ASSERT_EFI_ERROR (Status);

  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }
}

/**
  Fixup internal data so that EFI and SAL can be call in virtual mode.
  Call the passed in Child Notify event and convert the mFvbModuleGlobal
  date items to there virtual address.
  mFvbModuleGlobal->FvInstance[FVB_PHYSICAL]  - Physical copy of instance data
  mFvbModuleGlobal->FvInstance[FVB_VIRTUAL]   - Virtual pointer to common
  instance data.

  @param[in] Event    The Event that is being processed.
  @param[in] Context  The Event Context.
**/
STATIC
VOID
EFIAPI
FvbVirtualddressChangeEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_FW_VOL_INSTANCE *FwhInstance;
  UINTN               Index;


  gRT->ConvertPointer (0x0, (VOID **) &mFvbModuleGlobal->FvInstance[FVB_VIRTUAL]);

  //
  // Convert the base address of all the instances
  //
  Index       = 0;
  FwhInstance = mFvbModuleGlobal->FvInstance[FVB_PHYSICAL];
  while (Index < mFvbModuleGlobal->NumFv) {
    gRT->ConvertPointer (0x0, (VOID **) &FwhInstance->FvBase[FVB_VIRTUAL]);
    FwhInstance = (EFI_FW_VOL_INSTANCE *)
      (
        (UINTN) ((UINT8 *) FwhInstance) + FwhInstance->VolumeHeader.HeaderLength +
          (sizeof (EFI_FW_VOL_INSTANCE) - sizeof (EFI_FIRMWARE_VOLUME_HEADER))
      );
    Index++;
  }

  gRT->ConvertPointer (0x0, (VOID **) &mFvbModuleGlobal->FvbScratchSpace[FVB_VIRTUAL]);
  gRT->ConvertPointer (0x0, (VOID **) &mFvbModuleGlobal);

  if (IsFvbAccessThroughSmi ()) {
    gRT->ConvertPointer (0x0, (VOID **) &mSmmFvbBuffer);
  }

  mGoneVirtual = TRUE;
}


/**
 Check the integrity of firmware volume header.

 @param[in] FwVolHeader  A pointer to a firmware volume header.

 @retval EFI_SUCCESS     The firmware volume is consistent.
 @retval EFI_NOT_FOUND   The firmware volume has corrupted. So it is not an FV.
**/
STATIC
EFI_STATUS
ValidateFvHeader (
  IN EFI_FIRMWARE_VOLUME_HEADER            *FwVolHeader
  )
{
  //
  // Verify the header revision, header signature, length
  // Length of FvBlock cannot be 2**64-1
  // HeaderLength cannot be an odd number
  //
  if ((FwVolHeader->Revision != EFI_FVH_REVISION) ||
      (FwVolHeader->Signature != EFI_FVH_SIGNATURE) ||
      (FwVolHeader->FvLength == ((UINTN) -1)) ||
      ((FwVolHeader->HeaderLength & 0x01) != 0)) {
    return EFI_NOT_FOUND;
  }

  //
  // Verify the header checksum
  //
  if (CalculateCheckSum16 ((UINT16 *) FwVolHeader, FwVolHeader->HeaderLength) != 0) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

/**
  This function uses to get NV Storage base address

  @return EFI_PHYSICAL_ADDRESS  the NV storage start address.
**/
STATIC
EFI_PHYSICAL_ADDRESS
GetNvStoreBaseAddress (
  VOID
  )
{
  if (PcdGet64 (PcdFlashNvStorageVariableBase64) != 0) {
    return (EFI_PHYSICAL_ADDRESS) PcdGet64 (PcdFlashNvStorageVariableBase64);
  }
  return (EFI_PHYSICAL_ADDRESS) (UINTN) PcdGet32 (PcdFlashNvStorageVariableBase);
}


/**
  Get next valid firmware volume HOB.

  @param[in] Hob  Input EFI_PEI_HOB_POINTERS instance.

  @return VOID*   A pointer to next valid Fv HOB or NULL if not found.
**/
STATIC
VOID *
GetNextValidFvHob (
  IN      EFI_PEI_HOB_POINTERS        Hob
  )
{
  EFI_PEI_HOB_POINTERS                WorkingHob;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR     Descriptor;
  EFI_STATUS                          Status;

  WorkingHob     = Hob;
  WorkingHob.Raw = GET_NEXT_HOB (WorkingHob);
  while (WorkingHob.Raw != NULL) {
    WorkingHob.Raw = GetNextHob (EFI_HOB_TYPE_FV, WorkingHob.Raw);
    if (WorkingHob.Raw == NULL) {
      break;
    }
    Status = gDS->GetMemorySpaceDescriptor (
                    WorkingHob.FirmwareVolume->BaseAddress,
                    &Descriptor
                    );
    if (EFI_ERROR (Status)) {
      WorkingHob.Raw = NULL;
      break;
    }

    if (Descriptor.GcdMemoryType == EfiGcdMemoryTypeMemoryMappedIo) {
      break;
    }
    WorkingHob.Raw = GET_NEXT_HOB (WorkingHob);
  }
  return WorkingHob.Raw;
}

/**
  This functions uses to calculate total size memory size by bytes for FV instances buffer
  usage.

  @param[out] FvInstanceBufferSize  Pointer to total buffer size.

  @retval EFI_SUCCESS               Calculate total FV instances buffer size successful.
  @retval EFI_INVALID_PARAMETER     FvInstanceBufferSize is NULL.
**/
STATIC
EFI_STATUS
ClaculateFvInstancesBufferSize (
  OUT  UINTN          *FvInstanceBufferSize
  )
{
  EFI_PEI_HOB_POINTERS                FvHob;
  EFI_STATUS                          Status;
  EFI_FIRMWARE_VOLUME_HEADER          *FwVolHeader;
  UINTN                               BufferSize;
  EFI_PHYSICAL_ADDRESS                BaseAddress;

  if (FvInstanceBufferSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  BufferSize = 0;
  FvHob.Raw = GetHobList ();
  while ((FvHob.Raw = GetNextValidFvHob (FvHob)) != NULL) {
    //
    // Only need create FWH instance for NV Storage.
    //
    if (GetNvStoreBaseAddress () != FvHob.FirmwareVolume->BaseAddress) {
      continue;
    }
    BaseAddress = FvHob.FirmwareVolume->BaseAddress;
    FwVolHeader = (EFI_FIRMWARE_VOLUME_HEADER *) (UINTN) BaseAddress;
    Status      = ValidateFvHeader (FwVolHeader);
    if (EFI_ERROR (Status)) {
      Status = GetFvbInfo (BaseAddress, &FwVolHeader);
      if (EFI_ERROR (Status)) {
        continue;
      }
    }
    BufferSize += (sizeof (EFI_FW_VOL_INSTANCE) + FwVolHeader->HeaderLength - sizeof (EFI_FIRMWARE_VOLUME_HEADER));
  }

  *FvInstanceBufferSize = BufferSize;
  return EFI_SUCCESS;
}

/**
  This function restores correct firmware volume header contents to firmware volume header.

  @param[in] BaseAddress  Firmware volume start address.

  @retval EFI_SUCCESS     Restore firmware volume header contents successful.
  @retval EFI_NOT_FOUND   Cannot get correct firmware volume header contents.
  @return Other           Write firmware volume header contents failed.
**/
STATIC
EFI_STATUS
RestoreCorrectFvHeader (
  IN  EFI_PHYSICAL_ADDRESS    BaseAddress
  )
{
  EFI_STATUS                     Status;
  EFI_FIRMWARE_VOLUME_HEADER     *FwVolHeader;
  UINTN                          DataSize;

  RecalculateFvHeaderChecksum ();
  Status = GetFvbInfo (BaseAddress, &FwVolHeader);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  //  Write healthy FV header back.
  //
  Status = CommonFlashErase ((UINTN) BaseAddress, (UINTN) FwVolHeader->BlockMap->Length);
  ASSERT_EFI_ERROR(Status);
  DataSize = (UINTN) FwVolHeader->HeaderLength;
  Status = CommonFlashWrite ( (UINTN) BaseAddress, &DataSize, (VOID *) FwVolHeader);
  ASSERT_EFI_ERROR(Status);

  return Status;
}

/**
  Get total number of blocks from input firmware volume header.

  @param[in]  FwVolHeader        Pointer to EFI_FIRMWARE_VOLUME_HEADER instance.
  @param[out] NumOfBlocks        Output total number of blocks

  @retval EFI_SUCCESS            Get total number of blocks successful.
  @retval EFI_INVALID_PARAMETER  FwVolHeader or NumOfBlocks is NULL.
**/
STATIC
EFI_STATUS
GetNumOfBlocksFromFv (
  IN   EFI_FIRMWARE_VOLUME_HEADER   *FwVolHeader,
  OUT  UINTN                        *NumOfBlocks
  )
{
  UINTN                     WorkingBlocksNum;
  EFI_FV_BLOCK_MAP_ENTRY    *BlockMapEntry;

  if (FwVolHeader == NULL || NumOfBlocks == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  WorkingBlocksNum = 0;
  for (BlockMapEntry = FwVolHeader->BlockMap; BlockMapEntry->NumBlocks != 0; BlockMapEntry++) {
    WorkingBlocksNum = WorkingBlocksNum + BlockMapEntry->NumBlocks;
  }
  *NumOfBlocks = WorkingBlocksNum;

  return EFI_SUCCESS;
}

/**
  According to input base address to initialize FWH instance.

  @param[in] FwhInstance        Pointer to EFI_FW_VOL_INSTANCE instance.
  @param[in] BaseAddress        Firmware volume start address.

  @retval EFI_SUCCESS            Initialize FWH instance successful.
  @retval EFI_INVALID_PARAMETER  FwhInstance is NULL or BaseAddress is 0.
**/
STATIC
EFI_STATUS
InitializeFwhInstance (
  IN EFI_FW_VOL_INSTANCE     *FwhInstance,
  IN EFI_PHYSICAL_ADDRESS    BaseAddress
  )
{

  EFI_FIRMWARE_VOLUME_HEADER          *FwVolHeader;

  if (FwhInstance == NULL || BaseAddress == 0) {
    return EFI_INVALID_PARAMETER;
  }

  FwhInstance->FvBase[FVB_PHYSICAL] = (UINTN) BaseAddress;
  FwhInstance->FvBase[FVB_VIRTUAL]  = (UINTN) BaseAddress;
  FwVolHeader = (EFI_FIRMWARE_VOLUME_HEADER *) (UINTN) BaseAddress;
  CopyMem (&FwhInstance->VolumeHeader, FwVolHeader, FwVolHeader->HeaderLength);
  return GetNumOfBlocksFromFv (FwVolHeader, &FwhInstance->NumOfBlocks);
}

/**
  According to input base address to initialize EFI_FW_VOL_BLOCK_DEVICE instance.

  @param[in] FvbDevice           Pointer to EFI_FW_VOL_BLOCK_DEVICE instance.
  @param[in] BaseAddress         Firmware volume start address.

  @retval EFI_SUCCESS            Initialize EFI_FW_VOL_BLOCK_DEVICE instance successful.
  @retval EFI_INVALID_PARAMETER  FvbDevice is NULL or BaseAddress is 0.
**/
STATIC
EFI_STATUS
InitializeFvDevice (
  IN EFI_FW_VOL_BLOCK_DEVICE  *FvbDevice,
  IN EFI_PHYSICAL_ADDRESS     BaseAddress
  )
{
  EFI_FIRMWARE_VOLUME_HEADER          *FwVolHeader;

  if (FvbDevice == NULL || BaseAddress == 0) {
    return EFI_INVALID_PARAMETER;
  }

  CopyMem (FvbDevice, &mFvbDeviceTemplate, sizeof (EFI_FW_VOL_BLOCK_DEVICE));
  FvbDevice->Instance = mFvbModuleGlobal->NumFv;
  mFvbModuleGlobal->NumFv++;

  FwVolHeader = (EFI_FIRMWARE_VOLUME_HEADER *) (UINTN) BaseAddress;
  if (FwVolHeader->ExtHeaderOffset == 0) {
      //
      // FV does not contains extension header, then produce MEMMAP_DEVICE_PATH
      //
    FvbDevice->DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) AllocateCopyPool (sizeof (FV_MEMMAP_DEVICE_PATH), &mFvMemmapDevicePathTemplate);
    ((FV_MEMMAP_DEVICE_PATH *) FvbDevice->DevicePath)->MemMapDevPath.StartingAddress = BaseAddress;
    ((FV_MEMMAP_DEVICE_PATH *) FvbDevice->DevicePath)->MemMapDevPath.EndingAddress   = BaseAddress + FwVolHeader->FvLength - 1;
  } else {
    FvbDevice->DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) AllocateCopyPool (sizeof (FV_PIWG_DEVICE_PATH), &mFvPIWGDevicePathTemplate);
    CopyGuid (
      &((FV_PIWG_DEVICE_PATH *) FvbDevice->DevicePath)->FvDevPath.FvName,
      (GUID *)(UINTN)(BaseAddress + FwVolHeader->ExtHeaderOffset)
      );
  }

  return EFI_SUCCESS;
}

/**
  This function initializes all of related functions which used in protected mode.

  @retval  TRUE  The system has finished executing the EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE event.
  @retval  FALSE The system has not finished executing the EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE event.
**/
STATIC
EFI_STATUS
InitializeRuntimeFunctions (
  VOID
  )
{
  EFI_EVENT         Event;
  EFI_STATUS        Status;
  //
  // Register SetVirtualAddressMap () notify function
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  FvbVirtualddressChangeEvent,
                  NULL,
                  &gEfiEventVirtualAddressChangeGuid,
                  &Event
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  This function allows the caller to determine if UEFI SetVirtualAddressMap() has been called.

  This function returns TRUE after all the EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE functions have
  executed as a result of the OS calling SetVirtualAddressMap(). Prior to this time FALSE
  is returned. This function is used by runtime code to decide it is legal to access services
  that go away after SetVirtualAddressMap().

  @retval  TRUE  The system has finished executing the EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE event.
  @retval  FALSE The system has not finished executing the EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE event.
**/
BOOLEAN
GoneVirtual (
  VOID
  )
{
  return mGoneVirtual;
}

/**
  According to input base address to get next FWH instance.

  @param[in] FwhInstance          Pointer to EFI_FW_VOL_INSTANCE instance.

  @retunr EFI_FW_VOL_INSTANCE *   Start address of next FWH instance.
  @retval NULL                    Cannot find next FWH intance.
**/
EFI_FW_VOL_INSTANCE *
GetNextFwhInstance (
  IN EFI_FW_VOL_INSTANCE     *FwhInstance
  )
{

  EFI_FW_VOL_INSTANCE                 *NextFwhInstance;

  if (FwhInstance == NULL) {
    return NULL;
  }
  NextFwhInstance = (EFI_FW_VOL_INSTANCE *) ((UINTN) ((UINT8 *) FwhInstance) + FwhInstance->VolumeHeader.HeaderLength +
                                             (sizeof (EFI_FW_VOL_INSTANCE) - sizeof (EFI_FIRMWARE_VOLUME_HEADER)));
  return NextFwhInstance;
}

/**
  Routine to enable Access FVB through SMI feature.

  The mainly action of this routine is follows:
  1. Install gFvbAccessThroughSmiGuid to meet dependency to load image to SMM RAM.
  2. Create event to wait the completion of SMM image initialization.
  Note: If enabling access FVB through SMI, all of FVB access in this driver MUST
  through SMI. User should take attention to disable all of FVB access in protected
  mode.

  @retval EFI_SUCCESS      Enable FVB acccess through SMI feature successful.
  @retval EFI_UNSUPPORTED  System is in SMM mode or at runtime.
**/
EFI_STATUS
EnableFvbAccessThroughSmi (
  VOID
  )
{
  EFI_HANDLE       Handle;
  EFI_STATUS       Status;
  VOID             *Registration;
  EFI_EVENT        Event;

  if (mSmst != NULL || GoneVirtual ()) {
    return EFI_UNSUPPORTED;
  }

  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gFvbAccessThroughSmiGuid,
                  NULL,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  Event = InternalCreateProtocolNotifyEvent (
            &gFvbAccessThroughSmiGuid,
            TPL_NOTIFY,
            EnableFvbAccessThroughSmiEvent,
            NULL,
            &Registration
            );
  ASSERT (Event != NULL);

  return Status;
}

/**
  Add the EFI_MEMORY_RUNTIME memory attribute to input memory region.

  @param[in] BaseAddress    Input memory base address.
  @param[in] Length         Input memory size.

  @retval    EFI_SUCCESS    Add EFI_MEMORY_RUNTIME memory attribute successfully.
  @retval    other          Any other occurred while adding EFI_MEMORY_RUNTIME memory attribute.
**/
STATIC
EFI_STATUS
SetRuntimeMemoryAttribute (
  IN EFI_PHYSICAL_ADDRESS                BaseAddress,
  IN UINT64                              Length
  )
{
  EFI_STATUS                            Status;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR       GcdDescriptor;

  //
  // Mark the Flash part memory space as EFI_MEMORY_RUNTIME
  //
  BaseAddress = BaseAddress & (~EFI_PAGE_MASK);
  Length = (Length + EFI_PAGE_SIZE - 1) & (~EFI_PAGE_MASK);

  Status      = gDS->GetMemorySpaceDescriptor (BaseAddress, &GcdDescriptor);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gDS->SetMemorySpaceAttributes (
                  BaseAddress,
                  Length,
                  GcdDescriptor.Attributes | EFI_MEMORY_RUNTIME
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Initialize all of FVB services and provide FVB services relative interfaces.

  Any error occurred in this function will be asserted.

  @retval EFI_SUCCESS  Initialize FVbServices successful.
**/
EFI_STATUS
InitializeFVbServices (
  VOID
  )
{
  EFI_STATUS                          Status;
  EFI_FW_VOL_INSTANCE                 *FwhInstance;
  UINTN                               BufferSize;
  EFI_HANDLE                          FwbHandle;
  EFI_FW_VOL_BLOCK_DEVICE             *FvbDevice;
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  *OldFwbInterface;
  EFI_PHYSICAL_ADDRESS                BaseAddress;
  UINT64                              Length;
  EFI_PEI_HOB_POINTERS                FvHob;

  if (mFvbModuleGlobal != NULL) {
    return EFI_SUCCESS;
  }
  //
  // Allocate runtime services data for global variable, which contains
  // the private data of all firmware volume block instances
  //
  mFvbModuleGlobal = FvbAllocateZeroBuffer (sizeof (ESAL_FWB_GLOBAL), TRUE);
  ASSERT (mFvbModuleGlobal != NULL);
  if (mFvbModuleGlobal == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Calculate the total size for all firmware volume block instances
  //
  Status = ClaculateFvInstancesBufferSize (&BufferSize);
  ASSERT_EFI_ERROR(Status);
  //
  // Only need to allocate once. There is only one copy of physical memory for
  // the private data of each FV instance. But in virtual mode or in physical
  // mode, the address of the the physical memory may be different.
  //
  FwhInstance = FvbAllocateZeroBuffer (BufferSize, TRUE);
  ASSERT (FwhInstance != NULL);
  if (FwhInstance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Make a virtual copy of the FvInstance pointer.
  //
  mFvbModuleGlobal->FvInstance[FVB_PHYSICAL] = FwhInstance;
  mFvbModuleGlobal->FvInstance[FVB_VIRTUAL]  = FwhInstance;

  mFvbModuleGlobal->NumFv = 0;
  FvHob.Raw = GetHobList ();
  FwbHandle = NULL;
  while ((FvHob.Raw = GetNextValidFvHob (FvHob)) != NULL) {
    //
    // Only need create FWH instance for NV Storage.
    //
    if (GetNvStoreBaseAddress () != FvHob.FirmwareVolume->BaseAddress) {
      continue;
    }
    //
    // Restore correct firmware volume header if the contents is incorrect.
    //
    BaseAddress = FvHob.FirmwareVolume->BaseAddress;
    Length      = FvHob.FirmwareVolume->Length;
    Status      = ValidateFvHeader ((EFI_FIRMWARE_VOLUME_HEADER *) (UINTN) BaseAddress);
    if (EFI_ERROR (Status)) {
      Status = RestoreCorrectFvHeader (BaseAddress);
      if (EFI_ERROR (Status)) {
        continue;
      }
    }
    SetRuntimeMemoryAttribute (BaseAddress, Length);
    Status = InitializeFwhInstance (FwhInstance, BaseAddress);
    ASSERT_EFI_ERROR(Status);

    //
    // Add a FVB Protocol Instance and initialize this instance.
    //
    FvbDevice = FvbAllocateZeroBuffer (sizeof (EFI_FW_VOL_BLOCK_DEVICE), TRUE);
    ASSERT (FvbDevice != NULL);
    if (FvbDevice == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    Status = InitializeFvDevice (FvbDevice, BaseAddress);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Find a handle with a matching device path that has supports FW Block protocol
    //
    Status = gBS->LocateDevicePath (&gEfiFirmwareVolumeBlockProtocolGuid, &FvbDevice->DevicePath, &FwbHandle);
    if (EFI_ERROR (Status)) {
      //
      // LocateDevicePath fails so install a new interface and device path
      //
      FwbHandle = NULL;
      Status = gBS->InstallMultipleProtocolInterfaces (
                      &FwbHandle,
                      &gEfiFirmwareVolumeBlockProtocolGuid,
                      &FvbDevice->FwVolBlockInstance,
                      &gEfiDevicePathProtocolGuid,
                      FvbDevice->DevicePath,
                      NULL
                      );
      ASSERT_EFI_ERROR (Status);
    } else if (IsDevicePathEnd (FvbDevice->DevicePath)) {
      //
      // Device allready exists, so reinstall the FVB protocol
      //
      Status = gBS->HandleProtocol (
                      FwbHandle,
                      &gEfiFirmwareVolumeBlockProtocolGuid,
                      (VOID**)&OldFwbInterface
                      );
      ASSERT_EFI_ERROR (Status);

      Status = gBS->ReinstallProtocolInterface (
                      FwbHandle,
                      &gEfiFirmwareVolumeBlockProtocolGuid,
                      OldFwbInterface,
                      &FvbDevice->FwVolBlockInstance
                      );
      ASSERT_EFI_ERROR (Status);

    } else {
      //
      // There was a FVB protocol on an End Device Path node
      //
      ASSERT (FALSE);
    }
    FwhInstance = GetNextFwhInstance (FwhInstance);
  }
  Status = InitializeRuntimeFunctions ();
  ASSERT_EFI_ERROR (Status);
  //
  // Install Alternate FVB on the same handle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &FwbHandle,
                  &gEfiAlternateFvBlockGuid,
                  NULL,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

