/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file
  SmmAccessDriver.c

@brief
  This is the driver that publishes the SMM Access Protocol
  instance for System Agent.

**/
#include "SmmAccessDriver.h"

static SMM_ACCESS_PRIVATE_DATA  mSmmAccess;

//EFI_DRIVER_ENTRY_POINT (SmmAccessDriverEntryPoint)

/**

    This is the standard EFI driver point that
    installs an SMM Access Protocol

    @param[in] ImageHandle     - Handle for the image of this driver
    @param[in] SystemTable     - Pointer to the EFI System Table

    @retval EFI_SUCCESS           - Protocol was installed successfully
    @exception EFI_UNSUPPORTED    - Protocol was not installed
    @retval EFI_NOT_FOUND         - Protocol can't be found.
    @retval EFI_OUT_OF_RESOURCES  - Protocol does not have enough resources to initialize the driver.

**/
EFI_STATUS
EFIAPI
SmmAccessDriverEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                      Status;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridgeIo;
  UINTN                           Index;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK  *DescriptorBlock;
  EFI_PEI_HOB_POINTERS            *Hob;

  ///
  /// --cr-- INITIALIZE_SCRIPT (ImageHandle, SystemTable);
  ///
  /// Initialize Global variables
  ///
  ZeroMem (&mSmmAccess, sizeof (mSmmAccess));

  Status = gBS->LocateProtocol (
                  &gEfiPciRootBridgeIoProtocolGuid,
                  NULL,
                  (VOID **) &PciRootBridgeIo
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Could not locate PCI Root Bridge IO Protocol\n"));
    return EFI_NOT_FOUND;
  }

  mSmmAccess.Signature        = SMM_ACCESS_PRIVATE_DATA_SIGNATURE;
  mSmmAccess.Handle           = NULL;
  mSmmAccess.PciRootBridgeIo  = PciRootBridgeIo;

  ///
  /// Get Hob list
  ///
  Hob = GetFirstGuidHob (&gEfiSmmPeiSmramMemoryReserveGuid);
  if (Hob == NULL) {
    DEBUG ((EFI_D_ERROR, "SmramMemoryReserve HOB not found\n"));
    return EFI_NOT_FOUND;
  }

  DescriptorBlock = (VOID *) ((UINT8 *) Hob + sizeof (EFI_HOB_GUID_TYPE));

  ///
  /// Alloc space for mSmmAccess.SmramDesc
  ///
  mSmmAccess.SmramDesc = AllocateZeroPool ((DescriptorBlock->NumberOfSmmReservedRegions) * sizeof (EFI_SMRAM_DESCRIPTOR));
  if (mSmmAccess.SmramDesc == NULL) {
    DEBUG ((EFI_D_ERROR, "Alloc mSmmAccess.SmramDesc fail.\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  DEBUG ((EFI_D_INFO, "Alloc mSmmAccess.SmramDesc success.\n"));

  ///
  /// Use the HOB to publish SMRAM capabilities
  ///
  for (Index = 0; Index < DescriptorBlock->NumberOfSmmReservedRegions; Index++) {
    mSmmAccess.SmramDesc[Index].PhysicalStart = DescriptorBlock->Descriptor[Index].PhysicalStart;
    mSmmAccess.SmramDesc[Index].CpuStart      = DescriptorBlock->Descriptor[Index].CpuStart;
    mSmmAccess.SmramDesc[Index].PhysicalSize  = DescriptorBlock->Descriptor[Index].PhysicalSize;
    mSmmAccess.SmramDesc[Index].RegionState   = DescriptorBlock->Descriptor[Index].RegionState;
  }

  mSmmAccess.NumberRegions              = Index;
  mSmmAccess.SmmAccess.Open             = Open;
  mSmmAccess.SmmAccess.Close            = Close;
  mSmmAccess.SmmAccess.Lock             = Lock;
  mSmmAccess.SmmAccess.GetCapabilities  = GetCapabilities;
  mSmmAccess.SmmAccess.LockState        = FALSE;
  mSmmAccess.SmmAccess.OpenState        = FALSE;

  ///
  /// Install our protocol interfaces on the device's handle
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mSmmAccess.Handle,
                  &gEfiSmmAccessProtocolGuid,
                  &mSmmAccess.SmmAccess,
                  NULL
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "InstallMultipleProtocolInterfaces returned %r\n", Status));
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

/**

    This routine accepts a request to "open" a region of SMRAM.  The
    region could be legacy ABSEG, HSEG, or TSEG near top of physical memory.
    The use of "open" means that the memory is visible from all boot-service
    and SMM agents.

    @param[in] This                  - Pointer to the SMM Access Interface.
    @param[in] DescriptorIndex       - Region of SMRAM to Open.

    @retval EFI_SUCCESS           - The region was successfully opened.
    @retval EFI_DEVICE_ERROR      - The region could not be opened because locked by
                            chipset.
    @retval EFI_INVALID_PARAMETER - The descriptor index was out of bounds.

**/
EFI_STATUS
EFIAPI
Open (
  IN EFI_SMM_ACCESS_PROTOCOL  *This,
  UINTN                       DescriptorIndex
  )
{
  EFI_STATUS              Status;
  SMM_ACCESS_PRIVATE_DATA *SmmAccess;
  UINT64                  Address;
  UINT8                   SmramControl;

  SmmAccess = SMM_ACCESS_PRIVATE_DATA_FROM_THIS (This);

  if (DescriptorIndex >= SmmAccess->NumberRegions) {
    DEBUG ((EFI_D_WARN, "SMRAM region out of range\n"));
    return EFI_INVALID_PARAMETER;
  } else if (SmmAccess->SmramDesc[DescriptorIndex].RegionState & EFI_SMRAM_LOCKED) {
    ///
    /// Cannot open a "locked" region
    ///
    DEBUG ((EFI_D_WARN, "Cannot open a locked SMRAM region\n"));

    return EFI_DEVICE_ERROR;
  }
  ///
  /// BEGIN CHIPSET SPECIFIC CODE
  ///
  ///
  /// SMRAM register is PCI 0:0:0:88, SMRAMC (8 bit)
  ///
  Address = EFI_PCI_ADDRESS (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_SMRAMC);

  Status = SmmAccess->PciRootBridgeIo->Pci.Read (
                                            SmmAccess->PciRootBridgeIo,
                                            EfiPciWidthUint8,
                                            Address,
                                            1,
                                            &SmramControl
                                            );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "SmmAccess->PciRootBridgeIo->Pci.Read returned %r\n", Status));
    return Status;
  }
  ///
  ///  Is SMRAM locked?
  ///
  if ((SmramControl & B_SA_SMRAMC_D_LCK_MASK) != 0) {
    ///
    /// Cannot Open a locked region
    ///
    SmmAccess->SmramDesc[DescriptorIndex].RegionState |= EFI_SMRAM_LOCKED;
    DEBUG ((EFI_D_WARN, "Cannot open a locked SMRAM region\n"));
    return EFI_DEVICE_ERROR;
  }
  ///
  /// Open SMRAM region
  ///
  SmramControl |= B_SA_SMRAMC_D_OPEN_MASK;
  SmramControl &= ~(B_SA_SMRAMC_D_CLS_MASK);

  Status = SmmAccess->PciRootBridgeIo->Pci.Write (
                                            SmmAccess->PciRootBridgeIo,
                                            EfiPciWidthUint8,
                                            Address,
                                            1,
                                            &SmramControl
                                            );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "SmmAccess->PciRootBridgeIo->Pci.Write returned %r\n", Status));
    return Status;
  }
  ///
  /// END CHIPSET SPECIFIC CODE
  ///
  SmmAccess->SmramDesc[DescriptorIndex].RegionState &= ~(EFI_SMRAM_CLOSED | EFI_ALLOCATED);
  SmmAccess->SmramDesc[DescriptorIndex].RegionState |= EFI_SMRAM_OPEN;
  SmmAccess->SmmAccess.OpenState = TRUE;
  return EFI_SUCCESS;
}

/**

    This routine accepts a request to "close" a region of SMRAM.  The
    region could be legacy AB or TSEG near top of physical memory.
    The use of "close" means that the memory is only visible from SMM agents,
    not from BS or RT code.

    @param[in] This                  - Pointer to the SMM Access Interface.
    @param[in] DescriptorIndex       - Region of SMRAM to Close.

    @retval EFI_SUCCESS           - The region was successfully closed.
    @retval EFI_DEVICE_ERROR      - The region could not be closed because locked by
                              chipset.
    @retval EFI_INVALID_PARAMETER - The descriptor index was out of bounds.

**/
EFI_STATUS
EFIAPI
Close (
  IN EFI_SMM_ACCESS_PROTOCOL *This,
  UINTN                      DescriptorIndex
  )
{
  EFI_STATUS              Status;
  SMM_ACCESS_PRIVATE_DATA *SmmAccess;
  UINT64                  Address;
  BOOLEAN                 OpenState;
  UINT8                   Index;
  UINT8                   SmramControl;

  SmmAccess = SMM_ACCESS_PRIVATE_DATA_FROM_THIS (This);

  if (DescriptorIndex >= SmmAccess->NumberRegions) {
    DEBUG ((EFI_D_WARN, "SMRAM region out of range\n"));
    return EFI_INVALID_PARAMETER;
  } else if (SmmAccess->SmramDesc[DescriptorIndex].RegionState & EFI_SMRAM_LOCKED) {
    ///
    /// Cannot close a "locked" region
    ///
    DEBUG ((EFI_D_WARN, "Cannot close a locked SMRAM region\n"));
    return EFI_DEVICE_ERROR;
  }

  if (SmmAccess->SmramDesc[DescriptorIndex].RegionState & EFI_SMRAM_CLOSED) {
    return EFI_DEVICE_ERROR;
  }
  ///
  /// SMRAM register is PCI 0:0:0:88, SMRAMC (8 bit)
  ///
  Address = EFI_PCI_ADDRESS (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_SMRAMC);

  Status = SmmAccess->PciRootBridgeIo->Pci.Read (
                                            SmmAccess->PciRootBridgeIo,
                                            EfiPciWidthUint8,
                                            Address,
                                            1,
                                            &SmramControl
                                            );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "SmmAccess->PciRootBridgeIo->Pci.Read returned %r\n", Status));
    return Status;
  }
  ///
  ///  Is SMRAM locked?
  ///
  if ((SmramControl & B_SA_SMRAMC_D_LCK_MASK) != 0) {
    ///
    /// Cannot Close a locked region
    ///
    SmmAccess->SmramDesc[DescriptorIndex].RegionState |= EFI_SMRAM_LOCKED;
    DEBUG ((EFI_D_WARN, "Cannot close a locked SMRAM region\n"));
    return EFI_DEVICE_ERROR;
  }
  ///
  /// Close SMRAM region
  ///
  SmramControl &= ~(B_SA_SMRAMC_D_OPEN_MASK);

  Status = SmmAccess->PciRootBridgeIo->Pci.Write (
                                            SmmAccess->PciRootBridgeIo,
                                            EfiPciWidthUint8,
                                            Address,
                                            1,
                                            &SmramControl
                                            );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "SmmAccess->PciRootBridgeIo->Pci.Write returned %r\n", Status));
    return Status;
  }
  ///
  /// END CHIPSET SPECIFIC CODE
  ///
  SmmAccess->SmramDesc[DescriptorIndex].RegionState &= ~EFI_SMRAM_OPEN;
  SmmAccess->SmramDesc[DescriptorIndex].RegionState |= (EFI_SMRAM_CLOSED | EFI_ALLOCATED);

  ///
  /// Find out if any regions are still open
  ///
  OpenState = FALSE;
  for (Index = 0; Index < mSmmAccess.NumberRegions; Index++) {
    if ((SmmAccess->SmramDesc[Index].RegionState & EFI_SMRAM_OPEN) == EFI_SMRAM_OPEN) {
      OpenState = TRUE;
    }
  }

  SmmAccess->SmmAccess.OpenState = OpenState;
  return EFI_SUCCESS;
}

/**

    This routine accepts a request to "lock" SMRAM.  The
    region could be legacy AB or TSEG near top of physical memory.
    The use of "lock" means that the memory can no longer be opened
    to BS state..

    @param[in] This                  - Pointer to the SMM Access Interface.
    @param[in] DescriptorIndex       - Region of SMRAM to Lock.

    @retval EFI_SUCCESS           - The region was successfully locked.
    @retval EFI_DEVICE_ERROR      - The region could not be locked because at least
                            one range is still open.
    @retval EFI_INVALID_PARAMETER - The descriptor index was out of bounds.

**/
EFI_STATUS
EFIAPI
Lock (
  IN EFI_SMM_ACCESS_PROTOCOL *This,
  UINTN                      DescriptorIndex
  )
{
  EFI_STATUS              Status;
  SMM_ACCESS_PRIVATE_DATA *SmmAccess;
  UINT64                  Address;
  UINT8                   SmramControl;

  SmmAccess = SMM_ACCESS_PRIVATE_DATA_FROM_THIS (This);

  if (DescriptorIndex >= SmmAccess->NumberRegions) {
    DEBUG ((EFI_D_WARN, "SMRAM region out of range\n"));
    return EFI_INVALID_PARAMETER;
  } else if (SmmAccess->SmmAccess.OpenState) {
    DEBUG ((EFI_D_WARN, "Cannot lock SMRAM when SMRAM regions are still open\n"));
    return EFI_DEVICE_ERROR;
  }

//[-start-130618-IB05400417-add]//
  if (PcdGetBool (PcdHeadlessSystem)) {
    return EFI_SUCCESS;
  }
//[-end-130618-IB05400417-add]//

  SmmAccess->SmramDesc[DescriptorIndex].RegionState |= EFI_SMRAM_LOCKED;
  SmmAccess->SmmAccess.LockState = TRUE;

  ///
  /// SMRAM register is PCI 0:0:0:88, SMRAMC (8 bit)
  ///
  Address = EFI_PCI_ADDRESS (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_SMRAMC);

  Status = SmmAccess->PciRootBridgeIo->Pci.Read (
                                            SmmAccess->PciRootBridgeIo,
                                            EfiPciWidthUint8,
                                            Address,
                                            1,
                                            &SmramControl
                                            );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "SmmAccess->PciRootBridgeIo->Pci.Read returned %r\n", Status));
    return Status;
  }
  ///
  /// Lock the SMRAM
  ///
  SmramControl |= B_SA_SMRAMC_D_LCK_MASK;

  Status = SmmAccess->PciRootBridgeIo->Pci.Write (
                                            SmmAccess->PciRootBridgeIo,
                                            EfiPciWidthUint8,
                                            Address,
                                            1,
                                            &SmramControl
                                            );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "SmmAccess->PciRootBridgeIo->Pci.Write returned %r\n", Status));
    return Status;
  }
  ///
  /// END CHIPSET SPECIFIC CODE
  ///
  return EFI_SUCCESS;
}

/**

    This routine services a user request to discover the SMRAM
    capabilities of this platform.  This will report the possible
    ranges that are possible for SMRAM access, based upon the
    memory controller capabilities.

    @param[in] This                  - Pointer to the SMRAM Access Interface.
    @param[in] SmramMapSize          - Pointer to the variable containing size of the
                              buffer to contain the description information.
    @param[in] SmramMap              - Buffer containing the data describing the Smram
                              region descriptors.

    @retval EFI_BUFFER_TOO_SMALL  - The user did not provide a sufficient buffer.
    @retval EFI_SUCCESS           - The user provided a sufficiently-sized buffer.

**/
EFI_STATUS
EFIAPI
GetCapabilities (
  IN EFI_SMM_ACCESS_PROTOCOL     *This,
  IN OUT UINTN                   *SmramMapSize,
  IN OUT EFI_SMRAM_DESCRIPTOR    *SmramMap
  )
{
  EFI_STATUS              Status;
  SMM_ACCESS_PRIVATE_DATA *SmmAccess;
  UINTN                   NecessaryBufferSize;

  SmmAccess           = SMM_ACCESS_PRIVATE_DATA_FROM_THIS (This);

  NecessaryBufferSize = SmmAccess->NumberRegions * sizeof (EFI_SMRAM_DESCRIPTOR);

  if (*SmramMapSize < NecessaryBufferSize) {
    DEBUG ((EFI_D_WARN, "SMRAM Map Buffer too small\n"));
    Status = EFI_BUFFER_TOO_SMALL;
  } else {
    CopyMem (SmramMap, SmmAccess->SmramDesc, NecessaryBufferSize);
    Status = EFI_SUCCESS;
  }

  *SmramMapSize = NecessaryBufferSize;

  return Status;
}
