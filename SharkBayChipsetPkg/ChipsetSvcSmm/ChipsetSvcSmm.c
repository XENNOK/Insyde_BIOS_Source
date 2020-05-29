/** @file
 SMM Chipset Services driver. 
 
 It produces an instance of the SMM Chipset Services protocol to provide the chipset related functions 
 which will be used by Kernel or Project code. These protocol functions should be used by calling the 
 corresponding functions in SmmChipsetSvcLib to have the protocol size checking

***************************************************************************
* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/
#include <PiSmm.h>

//
// Libraries
//
#include <Library/SmmServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/PciCf8Lib.h>
#include <Library/IoLib.h>
//[-start-130809-IB06720232-remove]//
//#include <Library/EcLib.h>
//[-end-130809-IB06720232-remove]//

//
// Consumer Protocols
//
#include <Protocol/DriverSupportedEfiVersion.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/ComponentName.h>
#include <Protocol/SmmFwBlockService.h>

//
// Produced Protocols
//
#include <Protocol/H2OSmmChipsetServices.h>

//
// Register defination
//

//[-start-121119-IB10820169-add]//
//[-start-121212-IB10820191-remove]//
//#define MSR_PLAT_FRMW_PROT_CTRL                                       0x00000110
//#define B_MSR_PLAT_FRMW_PROT_CTRL_LK                                  BIT0
//#define B_MSR_PLAT_FRMW_PROT_CTRL_EN                                  BIT1
//[-end-121212-IB10820191-remove]//
//[-end-121119-IB10820169-add]//
//[-start-121212-IB10820191-modify]//
#include <CpuRegs.h>
//[-end-121212-IB10820191-modify]//
#include <PchRegs.h>
#include <SaRegs.h>
//
// Global variable
//
extern EFI_COMPONENT_NAME2_PROTOCOL  gChipsetSvcSmmComponentName2;
extern EFI_COMPONENT_NAME_PROTOCOL   gChipsetSvcSmmComponentName;

//
// Driver Support EFI Version Protocol instance
//
GLOBAL_REMOVE_IF_UNREFERENCED 
EFI_DRIVER_SUPPORTED_EFI_VERSION_PROTOCOL gSmmChipsetSvcDriverSupportedEfiVersion = {
  sizeof (EFI_DRIVER_SUPPORTED_EFI_VERSION_PROTOCOL),
  0x0002001E
};

H2O_SMM_CHIPSET_SERVICES_PROTOCOL *mSmmChipsetSvc = NULL;

EFI_STATUS
EFIAPI
ResetSystem (
  IN EFI_RESET_TYPE   ResetType
  );

EFI_STATUS
LegacyRegionAccessCtrl (
  IN  UINT32                        Start,
  IN  UINT32                        Length,
  IN  UINT32                        Mode  
  );

EFI_STATUS
EFIAPI
EnableFdWrites (
  IN  BOOLEAN           EnableWrites
  );

//[-start-130124-IB08460028-add]//
EFI_STATUS
SataComReset (
  IN  UINTN         PortNumber
  );
//[-end-130124-IB08460028-add]//

EFI_STATUS
EFIAPI
ChipsetSvcSmmEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  UINT32      Size;
  EFI_HANDLE  Handle;
  
  Status = EFI_SUCCESS;
  Handle = NULL;

  //
  // Create an instance of the H2O SMM Chipset Services protocol.
  // Then install it on the image handle.
  //
  
  Size = sizeof (H2O_SMM_CHIPSET_SERVICES_PROTOCOL);
  if (Size < sizeof (UINT32)) {         // must at least contain Size field.
    return EFI_INVALID_PARAMETER;
  }
  
  mSmmChipsetSvc = AllocateZeroPool (Size);
  if (mSmmChipsetSvc == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  mSmmChipsetSvc->Size = Size;

//[-start-121114-IB08460014-modify]//
  //
  // Now, populate functions based on Size.
  //
//  if (Size >= (OFFSET_OF(H2O_SMM_CHIPSET_SERVICES_PROTOCOL, InitFlashMode) + sizeof(VOID*))) {
//    mSmmChipsetSvc->InitFlashMode = InitFlashMode;
//  }
  if (Size >= (OFFSET_OF(H2O_SMM_CHIPSET_SERVICES_PROTOCOL, EnableFdWrites) + sizeof(VOID*))) {
    mSmmChipsetSvc->EnableFdWrites = EnableFdWrites;
  }
  if (Size >= (OFFSET_OF (H2O_SMM_CHIPSET_SERVICES_PROTOCOL, LegacyRegionAccessCtrl) + sizeof (VOID*))) {
    mSmmChipsetSvc->LegacyRegionAccessCtrl = LegacyRegionAccessCtrl;
  }
  if (Size >= (OFFSET_OF (H2O_SMM_CHIPSET_SERVICES_PROTOCOL, ResetSystem) + sizeof (VOID*))) {
    mSmmChipsetSvc->ResetSystem = ResetSystem;
  }
//[-end-121114-IB08460014-modify]//
//[-start-130124-IB08460028-add]//
  if (Size >= (OFFSET_OF (H2O_SMM_CHIPSET_SERVICES_PROTOCOL, SataComReset) + sizeof (VOID*))) {
    mSmmChipsetSvc->SataComReset = SataComReset;
  }
//[-end-130124-IB08460028-add]//

  Status = gSmst->SmmInstallProtocolInterface (
                    &Handle,
                    &gH2OSmmChipsetServicesProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    mSmmChipsetSvc
                    );

  return Status;
}

