/** @file
  Provide common initialization and some common interfaces for platform.

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiPei.h>
#include <Ppi/Stall.h>
#include <Ppi/AtaPolicy.h>
#include <Ppi/SmbusPolicy.h>
#include <Ppi/PlatformMemorySize.h>
#include <Ppi/PlatformMemoryRange.h>
#include <Ppi/MasterBootMode.h>
#include <Ppi/MemoryDiscovered.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/CpuIo.h>
#include <Guid/FdTableInfoHob.h>
#include <Guid/GlobalVariable.h>
#include <Guid/DimmSmbusAddrHob.h>
#include <Library/PeiOemSvcKernelLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PcdLib.h>
#include <Library/PeiChipsetSvcLib.h>
#include <Library/BaseOemSvcKernelLib.h>
#include <Library/VariableLib.h>

#define MAX_SOCKETS FixedPcdGet8(PcdMaxSockets)

#define PLATFORM_NUM_SMBUS_RSVD_ADDRESSES 4

#define SMBUS_ADDR_CH_A_1                 FixedPcdGet8 (PcdSmbusAddrChA1)
#define SMBUS_ADDR_CH_A_2                 FixedPcdGet8 (PcdSmbusAddrChA2)
#define SMBUS_ADDR_CH_B_1                 FixedPcdGet8 (PcdSmbusAddrChB1)
#define SMBUS_ADDR_CH_B_2                 FixedPcdGet8 (PcdSmbusAddrChB2)

#define PEI_STALL_RESOLUTION      1

typedef struct {
  UINT32                        Signature;
  EFI_FFS_FILE_HEADER           *FfsHeader;
  EFI_PEI_NOTIFY_DESCRIPTOR     StallNotify;
} STALL_CALLBACK_STATE_INFORMATION;

VOID
Init8254 (
  );

EFI_STATUS
EFIAPI
StallPpiFunc (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN CONST EFI_PEI_STALL_PPI    *This,
  IN UINTN                      Microseconds
  );

EFI_STATUS
EFIAPI
MemoryDiscoveredNotifyPlatformStage1Callback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

EFI_STATUS
EFIAPI
SetPlatformHardwareSwitchNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

EFI_STATUS
EFIAPI
UpdateBootMode (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN       EFI_PEI_CPU_IO_PPI   *CpuIo,
  IN       EFI_BOOT_MODE        *BootMode
  );

EFI_STATUS
EFIAPI
GetPlatformMemorySize (
  IN      EFI_PEI_SERVICES                       **PeiServices,
  IN      struct _PEI_PLATFORM_MEMORY_SIZE_PPI   *This,
  IN OUT  UINT64                                 *MemorySize
  );

EFI_STATUS
EFIAPI
PlatformChooseRanges (
  IN EFI_PEI_SERVICES                     **PeiServices,
  IN PEI_PLATFORM_MEMORY_RANGE_PPI        *This,
  IN OUT PEI_MEMORY_RANGE_OPTION_ROM      *OptionRomMask,
  IN OUT PEI_MEMORY_RANGE_SMRAM           *SmramMask,
  IN OUT PEI_MEMORY_RANGE_GRAPHICS_MEMORY *GraphicsMemoryMask,
  IN OUT PEI_MEMORY_RANGE_PCI_MEMORY     *PciMemoryMask
  );

//
// Globals
//
static UINT8                    mSmbusRsvdAddresses[PLATFORM_NUM_SMBUS_RSVD_ADDRESSES] = {
  SMBUS_ADDR_CH_A_1,
  SMBUS_ADDR_CH_A_2,
  SMBUS_ADDR_CH_B_1,
  SMBUS_ADDR_CH_B_2
};

static PEI_SMBUS_POLICY_PPI     mSmbusPolicyPpi = {
  FixedPcdGet16(PcdSmBusBaseAddress),
  FixedPcdGet32(PcdSmBusBusDevFunc),
  PLATFORM_NUM_SMBUS_RSVD_ADDRESSES,
  mSmbusRsvdAddresses
};

static PEI_ATA_POLICY_PPI       mAtaPolicyPpi = {
  FixedPcdGet16(PcdPeiAtaBusMasterBaseAddress)
};

static EFI_PEI_STALL_PPI            mStallPpi = {
  PEI_STALL_RESOLUTION,
  StallPpiFunc
};

static PEI_PLATFORM_MEMORY_SIZE_PPI   mPlatformMemorySizePpi = {
  GetPlatformMemorySize
};

static PEI_PLATFORM_MEMORY_RANGE_PPI  mPlatformMemoryRangePpi = {
  PlatformChooseRanges
};
static EFI_PEI_PPI_DESCRIPTOR   mPpiList[3] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI,
    &gPeiSmbusPolicyPpiGuid,
    &mSmbusPolicyPpi
  },
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI,
    &gPeiAtaPolicyPpiGuid,
    &mAtaPolicyPpi
  },
  {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiStallPpiGuid,
    &mStallPpi
  }
};
static EFI_PEI_PPI_DESCRIPTOR         mMemoryPpiList[2] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI,
    &gPeiPlatformMemorySizePpiGuid,
    &mPlatformMemorySizePpi
  },
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gPeiPlatformMemoryRangePpiGuid,
    &mPlatformMemoryRangePpi
  }
};

static EFI_PEI_PPI_DESCRIPTOR         mPpiBootMode = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiMasterBootModePpiGuid,
  NULL
};

static EFI_PEI_NOTIFY_DESCRIPTOR      mNotifyList[2] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK),
    &gEfiPeiMemoryDiscoveredPpiGuid,
    MemoryDiscoveredNotifyPlatformStage1Callback
  },

  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiReadOnlyVariable2PpiGuid,
    SetPlatformHardwareSwitchNotifyCallback
  }
};

/**
  Initializes the first stage for platform.

  @param[in]  FfsHeader   - Header for file of Firmware File System
  @param[in]  PeiServices - The PEI core services table.

  @retval EFI_SUCCESS - Routine finishes initializing stage1 successfully.
  @retval Other       - Error occurs while locating PPI.
                      - Error occurs while re-installing Stall PPI.
                      - Error occurs while allocating memory pool.
                      - Error occurs while publishing notification with PEI Core.
**/
EFI_STATUS
EFIAPI
PeiInitPlatformStage1EntryPoint (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                        Status;
  EFI_PEI_STALL_PPI                 *StallPpi;
  EFI_PEI_PPI_DESCRIPTOR            *StallPeiPpiDescriptor;
  UINT8                             *DimmSmbusAddrArray;
  UINT8                             i;
  EFI_BOOT_MODE                     BootMode;
  UINTN                             VariableSize;
  FD_TABLE_HOB_DATA                 FdTableHobData;


  //
  // Register so it will be automatically shadowed to memory
  //
  Status = PeiServicesRegisterForShadow (FileHandle);

  if (Status == EFI_ALREADY_STARTED) {
    //
    // Now that module in memory, update the
    // PPI that describes the Stall to other modules
    //
    Status = (**PeiServices).LocatePpi (
                               PeiServices,
                               &gEfiPeiStallPpiGuid,
                               0,
                               &StallPeiPpiDescriptor,
                               (VOID **)&StallPpi
                               );

    //
    // Reinstall the Interface using the memory-based descriptor
    //
    if (!EFI_ERROR (Status)) {
      //
      // Only reinstall the "Stall" PPI
      //
      Status = (**PeiServices).ReInstallPpi (
                                 PeiServices,
                                 StallPeiPpiDescriptor,
                                 &mPpiList[2]
                                 );
    }

  } else if (!EFI_ERROR (Status)) {

    Init8254 ();

    //
    // Initialize platform PPIs
    //
    Status = (**PeiServices).InstallPpi (PeiServices, &mPpiList[0]);
    ASSERT_EFI_ERROR (Status);

    PeiCsSvcPlatformStage1Init ();

    OemSvcEcInit ();

    //
    // OemServices
    //
    OemSvcInitPlatformStage1 ();
    //
    // Install PPI PlatformMemorySizePpi and PlatformMemoryRangePpi
    //
    Status = (**PeiServices).InstallPpi (PeiServices, &mMemoryPpiList[0]);
    ASSERT_EFI_ERROR (Status);

    //
    // Dtermine boot mode and return boot mode.
    //
    Status = UpdateBootMode (PeiServices, ((**PeiServices).CpuIo), &BootMode);
    ASSERT_EFI_ERROR (Status);

    //
    // Install Boot mode ppi.
    //
    if (!EFI_ERROR (Status)) {
      Status = (**PeiServices).InstallPpi (PeiServices, &mPpiBootMode);
      ASSERT_EFI_ERROR (Status);
    }

    DimmSmbusAddrArray = AllocateZeroPool (MAX_SOCKETS);

    //
    // Default setting
    //
    if (DimmSmbusAddrArray != NULL) {
      for (i = 0; i <  MAX_SOCKETS; i++) {
        DimmSmbusAddrArray [i] = mSmbusRsvdAddresses[i];
      }
    }

    //
    // Build HOB for consumer.
    //
    BuildGuidDataHob (&gDimmSmbusAddrHobGuid, DimmSmbusAddrArray,  MAX_SOCKETS);
    FdTableHobData.CurrentFdIndex = FD_INDEX_INVALID;
    //
    // Try to get CurrentFD variable.
    // This variable represent the connected Flash device which was set on previos boot
    //
    VariableSize = sizeof (UINT8);
    Status = CommonGetVariable (
               L"CurrentFD",
               &gEfiGlobalVariableGuid,
               &VariableSize,
               &FdTableHobData.CurrentFdIndex
               );

    //
    // Build FdTable Hob for consumer.
    //
    BuildGuidDataHob (&gFdTableInfoHobGuid, &FdTableHobData, sizeof (FD_TABLE_HOB_DATA));
    //
    // Notify for memory discovery callback
    //
    Status = (**PeiServices).NotifyPpi (PeiServices, &mNotifyList[0]);
    ASSERT_EFI_ERROR (Status);

    Status = (**PeiServices).NotifyPpi (PeiServices, &mNotifyList[1]);
    ASSERT_EFI_ERROR (Status);
  }

 return Status;

}

