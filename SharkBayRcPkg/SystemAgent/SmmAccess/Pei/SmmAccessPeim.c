/** @file
  This is the driver that publishes the SMM Access Ppi
  instance.

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

**/

#include "SmmAccessPeim.h"

/**
    This is the standard PEIM entry point that
    installs an SMM Access PPI

    @param[in] FfsHeader       - FfsHeader.
    @param[in] PeiServices     - General purpose services available to every PEIM.

    @retval EFI_SUCCESS           - Protocol successfully started and installed.
    @exception EFI_UNSUPPORTED    - Protocol can't be started.
    @retval EFI_NOT_FOUND         - Protocol can't be found.
    @retval EFI_OUT_OF_RESOURCES  - Protocol does not have enough resources to initialize the driver.
**/
EFI_STATUS
EFIAPI
SmmAccessPeimEntryPoint (
  IN EFI_PEI_FILE_HANDLE        FfsHeader,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                      Status;
  UINT32                          PciVidDid;
  UINTN                           Index;
  EFI_PEI_PPI_DESCRIPTOR          *PpiList;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK  *DescriptorBlock;
  SMM_ACCESS_PRIVATE_DATA         *SmmAccessPrivate;
  VOID                            *HobList;
  EFI_BOOT_MODE                   BootMode;

  Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
  if (EFI_ERROR (Status)) {
    ///
    /// If not in S3 boot path. do nothing
    ///
    return EFI_SUCCESS;
  }

  if (BootMode != BOOT_ON_S3_RESUME) {
    return EFI_SUCCESS;
  }
  ///
  /// Initialize private data
  ///
  SmmAccessPrivate  = AllocateZeroPool (sizeof (*SmmAccessPrivate));
  PpiList           = AllocateZeroPool (sizeof (*PpiList));

  ///
  /// Check if the chipset is supported
  /// by this driver.  Read the PCI Configuration Header for this device.
  ///
  PciVidDid                   = PciRead32 (PCI_LIB_ADDRESS (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, 0));

  SmmAccessPrivate->Signature = SMM_ACCESS_PRIVATE_DATA_SIGNATURE;
  SmmAccessPrivate->Handle    = NULL;

  ///
  /// Get Hob list
  ///
  HobList = GetFirstGuidHob (&gEfiSmmPeiSmramMemoryReserveGuid);
  if (HobList == NULL) {
    DEBUG ((EFI_D_ERROR, "SmramMemoryReserve HOB not found\n"));
    return EFI_NOT_FOUND;
  }

  DescriptorBlock = (EFI_SMRAM_HOB_DESCRIPTOR_BLOCK *) ((UINT8 *) HobList + sizeof (EFI_HOB_GUID_TYPE));

  ///
  /// Alloc space for SmmAccessPrivate->SmramDesc
  ///
  SmmAccessPrivate->SmramDesc = AllocateZeroPool ((DescriptorBlock->NumberOfSmmReservedRegions) * sizeof (EFI_SMRAM_DESCRIPTOR));
  if (SmmAccessPrivate->SmramDesc == NULL) {
    DEBUG ((EFI_D_ERROR, "Alloc SmmAccessPrivate->SmramDesc fail.\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  DEBUG ((EFI_D_INFO, "Alloc SmmAccessPrivate->SmramDesc success.\n"));

  ///
  /// use the hob to publish SMRAM capabilities
  ///
  for (Index = 0; Index < DescriptorBlock->NumberOfSmmReservedRegions; Index++) {
    SmmAccessPrivate->SmramDesc[Index].PhysicalStart  = DescriptorBlock->Descriptor[Index].PhysicalStart;
    SmmAccessPrivate->SmramDesc[Index].CpuStart       = DescriptorBlock->Descriptor[Index].CpuStart;
    SmmAccessPrivate->SmramDesc[Index].PhysicalSize   = DescriptorBlock->Descriptor[Index].PhysicalSize;
    SmmAccessPrivate->SmramDesc[Index].RegionState    = DescriptorBlock->Descriptor[Index].RegionState;
  }

  SmmAccessPrivate->NumberRegions             = Index;
  SmmAccessPrivate->SmmAccess.Open            = Open;
  SmmAccessPrivate->SmmAccess.Close           = Close;
  SmmAccessPrivate->SmmAccess.Lock            = Lock;
  SmmAccessPrivate->SmmAccess.GetCapabilities = GetCapabilities;
  SmmAccessPrivate->SmmAccess.LockState       = FALSE;
  SmmAccessPrivate->SmmAccess.OpenState       = FALSE;

  ///
  /// Install PPI
  ///
  PpiList->Flags  = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  PpiList->Guid   = &gPeiSmmAccessPpiGuid;
  PpiList->Ppi    = &SmmAccessPrivate->SmmAccess;

  Status          = PeiServicesInstallPpi (PpiList);
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

/**
  This routine accepts a request to "open" a region of SMRAM.  The
  region could be legacy ABSEG, HSEG, or TSEG near top of physical memory.
  The use of "open" means that the memory is visible from all PEIM
  and SMM agents.

  @param[in] This             -  Pointer to the SMM Access Interface.
  @param[in] DescriptorIndex  -  Region of SMRAM to Open.
  @param[in] PeiServices      -  General purpose services available to every PEIM.

  @retval EFI_SUCCESS            -  The region was successfully opened.
  @retval EFI_DEVICE_ERROR       -  The region could not be opened because locked by
                            chipset.
  @retval EFI_INVALID_PARAMETER  -  The descriptor index was out of bounds.
**/
EFI_STATUS
EFIAPI
Open (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN PEI_SMM_ACCESS_PPI         *This,
  IN UINTN                      DescriptorIndex
  )
{
  SMM_ACCESS_PRIVATE_DATA *SmmAccess;
  SMRAM                   Smram;

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
  Smram = PciRead32 (PCI_LIB_ADDRESS (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_SMRAMC));

  ///
  ///  Is SMRAM locked?
  ///
  if (Smram & B_SA_SMRAMC_D_LCK_MASK) {
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
  Smram |= B_SA_SMRAMC_D_OPEN_MASK;
  Smram &= ~(B_SA_SMRAMC_D_CLS_MASK);

  ///
  /// Write the SMRAMC register
  ///
  PciWrite32 (PCI_LIB_ADDRESS (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_SMRAMC), Smram);

  ///
  /// END CHIPSET SPECIFIC CODE
  ///
  SmmAccess->SmramDesc[DescriptorIndex].RegionState &= ~(EFI_SMRAM_CLOSED | EFI_ALLOCATED);
  SmmAccess->SmramDesc[DescriptorIndex].RegionState |= EFI_SMRAM_OPEN;
  SmmAccess->SmmAccess.OpenState = TRUE;
  return EFI_SUCCESS;
}

/**
  This routine accepts a request to "close" a region of SMRAM.  This is valid for
  compatible SMRAM region.

  @param[in] PeiServices      -  General purpose services available to every PEIM.
  @param[in] This             -  Pointer to the SMM Access Interface.
  @param[in] DescriptorIndex  -  Region of SMRAM to Close.

  @retval EFI_SUCCESS            -  The region was successfully closed.
  @retval EFI_DEVICE_ERROR       -  The region could not be closed because locked by
                            chipset.
  @retval EFI_INVALID_PARAMETER  -  The descriptor index was out of bounds.
**/
EFI_STATUS
EFIAPI
Close (
  IN EFI_PEI_SERVICES        **PeiServices,
  IN PEI_SMM_ACCESS_PPI      *This,
  IN UINTN                   DescriptorIndex
  )
{
  SMM_ACCESS_PRIVATE_DATA *SmmAccess;
  BOOLEAN                 OpenState;
  UINT8                   Index;
  SMRAM                   Smram;

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
  Smram = PciRead32 (PCI_LIB_ADDRESS (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_SMRAMC));

  ///
  ///  Is SMRAM locked?
  ///
  if ((Smram & B_SA_SMRAMC_D_LCK_MASK) != 0) {
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
  Smram &= ~(B_SA_SMRAMC_D_OPEN_MASK);

  ///
  /// Write the SAD_SMRAM register
  ///
  PciWrite32 (PCI_LIB_ADDRESS (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_SMRAMC), Smram);

  ///
  /// END CHIPSET SPECIFIC CODE
  ///
  SmmAccess->SmramDesc[DescriptorIndex].RegionState &= ~EFI_SMRAM_OPEN;
  SmmAccess->SmramDesc[DescriptorIndex].RegionState |= (EFI_SMRAM_CLOSED | EFI_ALLOCATED);

  ///
  /// Find out if any regions are still open
  ///
  OpenState = FALSE;
  for (Index = 0; Index < SmmAccess->NumberRegions; Index++) {
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
  to PEIM.

  @param[in] PeiServices      - General purpose services available to every PEIM.
  @param[in] This             -  Pointer to the SMM Access Interface.
  @param[in] DescriptorIndex  -  Region of SMRAM to Lock.

  @retval EFI_SUCCESS            -  The region was successfully locked.
  @retval EFI_DEVICE_ERROR       -  The region could not be locked because at least
                            one range is still open.
  @retval EFI_INVALID_PARAMETER  -  The descriptor index was out of bounds.
**/
EFI_STATUS
EFIAPI
Lock (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN PEI_SMM_ACCESS_PPI        *This,
  IN UINTN                     DescriptorIndex
  )
{
  SMM_ACCESS_PRIVATE_DATA *SmmAccess;
  SMRAM                   Smram;

  SmmAccess = SMM_ACCESS_PRIVATE_DATA_FROM_THIS (This);
  if (DescriptorIndex >= SmmAccess->NumberRegions) {
    DEBUG ((EFI_D_WARN, "SMRAM region out of range\n"));

    return EFI_INVALID_PARAMETER;
  } else if (SmmAccess->SmmAccess.OpenState) {
    DEBUG ((EFI_D_WARN, "Cannot lock SMRAM when SMRAM regions are still open\n"));

    return EFI_DEVICE_ERROR;
  }

  SmmAccess->SmramDesc[DescriptorIndex].RegionState |= EFI_SMRAM_LOCKED;
  SmmAccess->SmmAccess.LockState = TRUE;

  ///
  /// Read the SAD_SMRAM register
  ///
  Smram = PciRead32 (PCI_LIB_ADDRESS (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_SMRAMC));

  ///
  /// Lock the chipset
  ///
  Smram |= B_SA_SMRAMC_D_LCK_MASK;

  ///
  /// Write the SAD_SMRAM register
  ///
  PciWrite32 (PCI_LIB_ADDRESS (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_SMRAMC), Smram);

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

  @param[in] PeiServices   - General purpose services available to every PEIM.
  @param[in] This          -  Pointer to the SMRAM Access Interface.
  @param[in] SmramMapSize  -  Pointer to the variable containing size of the
                   buffer to contain the description information.
  @param[in] SmramMap      -  Buffer containing the data describing the Smram
                   region descriptors.

  @retval EFI_BUFFER_TOO_SMALL  -  The user did not provide a sufficient buffer.
  @retval EFI_SUCCESS           -  The user provided a sufficiently-sized buffer.
**/
EFI_STATUS
EFIAPI
GetCapabilities (
  IN EFI_PEI_SERVICES                **PeiServices,
  IN PEI_SMM_ACCESS_PPI              *This,
  IN OUT UINTN                       *SmramMapSize,
  IN OUT EFI_SMRAM_DESCRIPTOR        *SmramMap
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
