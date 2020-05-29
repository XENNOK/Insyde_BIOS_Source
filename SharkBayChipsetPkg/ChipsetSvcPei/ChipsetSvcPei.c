/** @file
 PEI Chipset Services driver. 
 
 It produces an instance of the PEI Chipset Services PPI to provide the chipset related functions 
 which will be used by Kernel or Project code. These PPI functions should be used by calling the 
 corresponding functions in PeiChipsetSvcLib to have the protocol size checking

***************************************************************************
* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

//
// Libraries
//
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Ppi/H2OPeiChipsetServices.h>
#include <Ppi/EcpAtaController.h>

//
// Structure for H2O Chipsete Services PPI Descriptor
//
typedef struct {
  EFI_PEI_PPI_DESCRIPTOR    PpiDescriptor;
  H2O_CHIPSET_SERVICES_PPI  H2OChipsetSvcPpi;
} H2O_CHIPSET_SERVICES_PPI_DESCRIPTOR;

EFI_STATUS
EFIAPI
EnableFdWrites (
  IN  BOOLEAN                       EnableWrites
  );

EFI_STATUS
LegacyRegionAccessCtrl (
  IN  UINT32                        Start,
  IN  UINT32                        Length,
  IN  UINT32                        Mode  
  );

EFI_STATUS
GetSleepState (
  IN OUT  UINT8                    *SleepState
  );

EFI_STATUS
HdaInitHook (
  IN UINT32                         HdaBar
  );

EFI_STATUS
PlatformStage1Init (
  VOID
  );
  
EFI_STATUS
PlatformStage2Init (
  VOID
  );
  
EFI_STATUS
Stage1MemoryDiscoverCallback (
  IN  EFI_BOOT_MODE                 *BootMode
  );

EFI_STATUS
Stage2MemoryDiscoverCallback (
  IN EFI_BOOT_MODE                  *BootMode
  );

EFI_STATUS
ModifyMemoryRange (
  IN OUT  PEI_MEMORY_RANGE_OPTION_ROM           *OptionRomMask,
  IN OUT  PEI_MEMORY_RANGE_SMRAM                *SmramMask,
  IN OUT  PEI_MEMORY_RANGE_GRAPHICS_MEMORY      *GraphicsMemoryMask,
  IN OUT  PEI_MEMORY_RANGE_PCI_MEMORY           *PciMemoryMask
  );

EFI_STATUS
SetSetupVariableDefault (
  IN VOID                           *SystemConfiguration
  );

EFI_STATUS
SetPlatformHardwareSwitch (
  OUT PLATFORM_HARDWARE_SWITCH          **PlatformHardwareSwitchDptr
  );

/**
 This is the declaration of PEI Chipset Services PEIM entry point. 
 
 @param[in]         FileHandle          Handle of the file being invoked.
 @param[in]         PeiServices         Describes the list of possible PEI Services.
                    
 @retval            EFI_SUCCESS         The operation completed successfully.
 @retval            Others              An unexpected error occurred.
*/
EFI_STATUS
EFIAPI
ChipsetSvcPeiEntryPoint (
  IN EFI_PEI_FILE_HANDLE             FileHandle,
  IN CONST EFI_PEI_SERVICES        **PeiServices
  )
{
  EFI_STATUS                            Status;
  UINT32                                Size;
  H2O_CHIPSET_SERVICES_PPI_DESCRIPTOR  *ChipsetSvcPpiDescriptor;
  EFI_PEI_PPI_DESCRIPTOR               *PpiDescriptor;
  VOID                                 *Ppi;

  PpiDescriptor = NULL;
  Ppi = NULL;

  //
  // Register so it will be automatically shadowed to memory
  //
  Status = PeiServicesRegisterForShadow (FileHandle);

  //
  // Create an instance of the H2O Chipset Services ppi,
  //
  ChipsetSvcPpiDescriptor = AllocateZeroPool (sizeof (H2O_CHIPSET_SERVICES_PPI_DESCRIPTOR));
  if (ChipsetSvcPpiDescriptor == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Size = sizeof (H2O_CHIPSET_SERVICES_PPI);
  if (Size < sizeof (UINT32)) {         // must at least contain Size field.
    return EFI_INVALID_PARAMETER;
  }
  
  //
  // Build ChipsetSvcPpi Private Data
  //
  ChipsetSvcPpiDescriptor->PpiDescriptor.Flags   = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  ChipsetSvcPpiDescriptor->PpiDescriptor.Guid    = &gH2OChipsetServicesPpiGuid;
  ChipsetSvcPpiDescriptor->PpiDescriptor.Ppi     = &ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi;
  ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.Size = Size;
//[-start-121114-IB08460014-modify]//
  //
  // Populate all functions based on Size.
  //
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PPI, EnableFdWrites) + sizeof (VOID*))) {
    ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.EnableFdWrites = EnableFdWrites;
  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PPI, LegacyRegionAccessCtrl) + sizeof (VOID*))) {
    ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.LegacyRegionAccessCtrl = LegacyRegionAccessCtrl;
//  }
//  if (Size >= (OFFSET_OF(H2O_CHIPSET_SERVICES_PPI, OnStartOfTcgPei) + sizeof(VOID*))) {
//    ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.OnStartOfTcgPei = OnStartOfTcgPei;
//  }
//  if (Size >= (OFFSET_OF(H2O_CHIPSET_SERVICES_PPI, ResetSystem) + sizeof(VOID*))) {
//    ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.ResetSystem = ResetSystem;
  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PPI, GetSleepState) + sizeof (VOID*))) {
    ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.GetSleepState = GetSleepState;
  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PPI, HdaInitHook) + sizeof (VOID*))) {
    ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.HdaInitHook = HdaInitHook;
  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PPI, PlatformStage1Init) + sizeof (VOID*))) {
    ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.PlatformStage1Init = PlatformStage1Init;
  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PPI, Stage1MemoryDiscoverCallback) + sizeof (VOID*))) {
    ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.Stage1MemoryDiscoverCallback = Stage1MemoryDiscoverCallback;
  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PPI, PlatformStage2Init) + sizeof (VOID*))) {
    ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.PlatformStage2Init = PlatformStage2Init;
  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PPI, Stage2MemoryDiscoverCallback) + sizeof (VOID*))) {
    ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.Stage2MemoryDiscoverCallback = Stage2MemoryDiscoverCallback;
  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PPI, ModifyMemoryRange) + sizeof (VOID*))) {
    ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.ModifyMemoryRange = ModifyMemoryRange;
  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PPI, SetSetupVariableDefault) + sizeof (VOID*))) {
    ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.SetSetupVariableDefault = SetSetupVariableDefault;
  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PPI, SetPlatformHardwareSwitch) + sizeof (VOID*))) {
    ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.SetPlatformHardwareSwitch = SetPlatformHardwareSwitch;
  }
//[-end-121114-IB08460014-modify]//
  if (Status == EFI_ALREADY_STARTED) {
    //
    // Shadow completed and running from memory
    //
    Status = PeiServicesLocatePpi (
               &gH2OChipsetServicesPpiGuid,
               0,
               &PpiDescriptor,
               (VOID **)&Ppi
               );
    if (!EFI_ERROR (Status)) {
      Status = PeiServicesReInstallPpi (
                 PpiDescriptor, 
                 &ChipsetSvcPpiDescriptor->PpiDescriptor
                 );
    }
  } else {
    //
    // Install the Ppi.
    //
    Status = PeiServicesInstallPpi (&ChipsetSvcPpiDescriptor->PpiDescriptor);
  }
  
  return Status;
}

