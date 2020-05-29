/** @file
 DXE Chipset Services driver. 

 It produces an instance of the DXE Chipset Services protocol to provide the chipset related functions 
 which will be used by Kernel or Project code. These protocol functions should be used by calling the 
 corresponding functions in DxeChipsetSvcLib to have the protocol size checking

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
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

//
// UEFI Driver Model Protocols
//
#include <Protocol/DriverSupportedEfiVersion.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/ComponentName.h>

#include <Protocol/H2ODxeChipsetServices.h>

H2O_CHIPSET_SERVICES_PROTOCOL *mDxeChipsetSvc = NULL;

//
// Protocol instances
//
extern EFI_COMPONENT_NAME2_PROTOCOL  gChipsetSvcDxeComponentName2;
extern EFI_COMPONENT_NAME_PROTOCOL   gChipsetSvcDxeComponentName;

//
// Driver Support EFI Version Protocol instance
//
GLOBAL_REMOVE_IF_UNREFERENCED 
EFI_DRIVER_SUPPORTED_EFI_VERSION_PROTOCOL gChipsetSvcDxeDriverSupportedEfiVersion = {
  sizeof (EFI_DRIVER_SUPPORTED_EFI_VERSION_PROTOCOL),
  0x0002001E
};

EFI_STATUS
SataComReset (
  IN  UINTN         PortNumber
  );
  
EFI_STATUS
EnableFdWrites (
  IN  BOOLEAN       EnableWrites
  );

EFI_STATUS
IoDecodeControl (
  IN IO_DECODE_TYPE		    Type,
  IN IO_DECODE_ADDRESS	  Address,
  IN UINT16               Length
  );

EFI_STATUS
LegacyRegionAccessCtrl (
  IN  UINT32              Start,
  IN  UINT32              Length,
  IN  UINT32              Mode  
  );

EFI_STATUS
ProgramChipsetSsid (
  IN UINT8                Bus,
  IN UINT8                Dev,
  IN UINT8                Func,
  IN UINT16               VendorId,
  IN UINT16               DeviceId,
  IN UINT32               SsidSvid
  );

EFI_STATUS
InstallPciRomSwitchTextMode (
  IN      EFI_HANDLE      DeviceHandle,
  IN OUT  BOOLEAN        *ForceSwitchTextMode
  );

EFI_STATUS
SetIsaInfo (
  IN OUT UINT8           *Function,
  IN OUT UINT8           *Device
  );

EFI_STATUS
SetSmbiosMemoryData (
  IN OUT H2O_SMBIOS_MEM_CONFIG_DATA        *ChipsetSmbiosMemData
  );

EFI_STATUS
SkipGetPciRom (
  IN  UINTN               Segment,
  IN  UINTN               Bus,
  IN  UINTN               Device,
  IN  UINTN               Function,
  IN  UINT16              VendorId,
  IN  UINT16              DeviceId,
  OUT BOOLEAN            *SkipGetPciRom
  );

VOID
UpdateAcpiTable (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER      *TableHeader,
  IN OUT EFI_STATUS                       *CommonCodeReturnStatus
  ); 
//[-start-130429-IB06720225-add]//
/**
 Provide IRQ Routing table according to platform specific information.

 @param[OUT]      VirtualBusTablePtr           A pointer to the VirtualBusTable that list all bridges in this platform 
                                               and bridges' virtual secondary bus.
 @param[OUT]      VirtualBusTableEntryNumber   A pointer to the VirtualBusTable entry number.
 @param[OUT]      IrqPoolTablePtr,             A pointer to the IRQ Pool Table.
 @param[OUT]      IrqPoolTableNumber,          A pointer to the IRQ Pool Table entry number.
 @param[OUT]      PirqPriorityTable,           A pointer to the PIRQ priority used Table.
 @param[OUT]      PirqPriorityTableEntryNumber A pointer to the PIRQ priority used Table entry number.
 @param[OUT]      IrqTablePtr                  A pointer to the IRQ routing Table.

 @retval          EFI_SUCCESS                  Get Platform specific IRQ Routing Info successfully..
 @retval          EFI_UNSUPPORTED              If the function is not implemented.
**/
EFI_STATUS
IrqRoutingInformation (
  OUT LEGACY_MODIFY_PIR_TABLE             **VirtualBusTablePtr,
  OUT UINT8                               *VirtualBusTableEntryNumber,
  OUT EFI_LEGACY_IRQ_PRIORITY_TABLE_ENTRY **IrqPoolTablePtr,
  OUT UINT8                               *IrqPoolTableNumber,
  OUT UINT8                               **PirqPriorityTablePtr,
  OUT UINT8                               *PirqPriorityTableEntryNumber,
  OUT IRQ_ROUTING_TABLE                   **IrqRoutingTableInfoPtr
);
//[-end-130429-IB06720225-add]//

//[-start-130715-IB07390108-add]//
//[-start-130724-IB07390110-modify]//
/**
 Get information of recovery FV.

 @param[out]        RecoveryRegion        recovery region table
 @param[out]        NumberOfRegion        number of recovery region
 @param[out]        ChipsetEmuPeiMaskList Specified PEIMs don't to dispatch in recovery mode
                    
 @retval            EFI_SUCCESS           This function always return success.
**/
EFI_STATUS
FvRecoveryInfo (
  OUT  PEI_FV_DEFINITION       **RecoveryRegion,
  OUT  UINTN                    *NumberOfRegion,
  OUT  EFI_GUID                **ChipsetEmuPeiMaskList
  );
//[-end-130724-IB07390110-modify]//
//[-end-130715-IB07390108-add]//
//[-start-130424-12360003-add]//
EFI_STATUS
GetBiosProtectTable (
//[-start-130927-IB12360020-modify]//
  OUT BIOS_PROTECT_REGION           **BiosRegionTable,
  OUT UINT8                         *ProtectRegionNum
//[-end-130927-IB12360020-modify]//
  );
//[-end-130424-12360003-add]// 

/**
 Unloads an image.

 @param[in]         ImageHandle           Handle that identifies the image to be unloaded.
                    
 @retval            EFI_SUCCESS           The image has been unloaded.
 @retval            EFI_INVALID_PARAMETER One of the protocol interfaces was not previously installed on ImageHandle.
**/
EFI_STATUS 
EFIAPI
ChipsetSvcDxeUnload (
  IN EFI_HANDLE  ImageHandle
  )
{
  EFI_STATUS  Status;

  Status = EFI_SUCCESS;
  //
  // Uninstall Driver Supported EFI Version Protocol from the image handle.
  //
  Status = gBS->UninstallMultipleProtocolInterfaces (
                  ImageHandle,
                  &gEfiDriverSupportedEfiVersionProtocolGuid, &gChipsetSvcDxeDriverSupportedEfiVersion,
                  &gH2OChipsetServicesProtocolGuid, mDxeChipsetSvc,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Do any additional cleanup that is required for this driver
  //
  FreePool (mDxeChipsetSvc);
  mDxeChipsetSvc = NULL;
  
  return EFI_SUCCESS;
}

/**
This is the declaration of DXE Chipset Services driver entry point. 

 @param[in]         ImageHandle         The firmware allocated handle for the UEFI image.
 @param[in]         SystemTable         A pointer to the EFI System Table.

 @retval            EFI_SUCCESS         The operation completed successfully.
 @retval            !EFI_SUCCESS        Error to installing Chipset Services Protocol.
*/
EFI_STATUS
EFIAPI
ChipsetSvcDxeEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  UINT32      Size;

  Status = EFI_SUCCESS;

  //
  // Install Driver Component Name Protocol onto ImageHandle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gEfiComponentNameProtocolGuid, &gChipsetSvcDxeComponentName,
                  &gEfiComponentName2ProtocolGuid, &gChipsetSvcDxeComponentName2,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
 
  //
  // Install Driver Supported EFI Version Protocol onto ImageHandle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gEfiDriverSupportedEfiVersionProtocolGuid, &gChipsetSvcDxeDriverSupportedEfiVersion,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
  
  //
  // Create an instance of the H2O Chipset Services protocol.
  // Then install it on the image handle.
  //
  
  Size = sizeof (H2O_CHIPSET_SERVICES_PROTOCOL);
  if (Size < sizeof (UINT32)) {         // must at least contain Size field.
    return EFI_INVALID_PARAMETER;
  }
  
  mDxeChipsetSvc = AllocateZeroPool (Size);
  if (mDxeChipsetSvc == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  mDxeChipsetSvc->Size = Size;
//[-start-121114-IB08460014-modify]//
  //
  // Now, populate functions based on Size.
  //
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PROTOCOL, SataComReset) + sizeof (VOID*))) {
    mDxeChipsetSvc->SataComReset = SataComReset;
  }
//  if (Size >= (OFFSET_OF(H2O_CHIPSET_SERVICES_PROTOCOL, InitFlashMode) + sizeof(VOID*))) {
//    mDxeChipsetSvc->InitFlashMode = InitFlashMode;
//  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PROTOCOL, EnableFdWrites) + sizeof (VOID*))) {
    mDxeChipsetSvc->EnableFdWrites = EnableFdWrites;
  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PROTOCOL, IoDecodeControl) + sizeof (VOID*))) {
    mDxeChipsetSvc->IoDecodeControl = IoDecodeControl;
  }
//  if (Size >= (OFFSET_OF(H2O_CHIPSET_SERVICES_PROTOCOL, MmioDecodeControl) + sizeof(VOID*))) {
//    mDxeChipsetSvc->MmioDecodeControl = MmioDecodeControl;
//  }
//  if (Size >= (OFFSET_OF(H2O_CHIPSET_SERVICES_PROTOCOL, IsaPositiveDecodeModeChecking) + sizeof(VOID*))) {
//    mDxeChipsetSvc->IsaPositiveDecodeModeChecking = IsaPositiveDecodeModeChecking;
//  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PROTOCOL, LegacyRegionAccessCtrl) + sizeof (VOID*))) {
    mDxeChipsetSvc->LegacyRegionAccessCtrl = LegacyRegionAccessCtrl;
  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PROTOCOL, ProgramChipsetSsid) + sizeof (VOID*))) {
    mDxeChipsetSvc->ProgramChipsetSsid = ProgramChipsetSsid;
  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PROTOCOL, InstallPciRomSwitchTextMode) + sizeof (VOID*))) {
    mDxeChipsetSvc->InstallPciRomSwitchTextMode = InstallPciRomSwitchTextMode;
  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PROTOCOL, SetIsaInfo) + sizeof (VOID*))) {
    mDxeChipsetSvc->SetIsaInfo = SetIsaInfo;
  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PROTOCOL, SetSmbiosMemoryData) + sizeof (VOID*))) {
    mDxeChipsetSvc->SetSmbiosMemoryData = SetSmbiosMemoryData;
  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PROTOCOL, SkipGetPciRom) + sizeof (VOID*))) {
    mDxeChipsetSvc->SkipGetPciRom = SkipGetPciRom;
  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PROTOCOL, UpdateAcpiTable) + sizeof (VOID*))) {
    mDxeChipsetSvc->UpdateAcpiTable = UpdateAcpiTable;
  }
//[-start-130419-IB06720225-add]//
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PROTOCOL, IrqRoutingInformation) + sizeof (VOID*))) {
    mDxeChipsetSvc->IrqRoutingInformation = IrqRoutingInformation;
  }
//[-end-130419-IB06720225-add]//
//[-start-130715-IB07390108-add]//
//[-start-130724-IB07390110-modify]//
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PROTOCOL, FvRecoveryInfo) + sizeof (VOID*))) {
    mDxeChipsetSvc->FvRecoveryInfo = FvRecoveryInfo;
  }
//[-end-130724-IB07390110-modify]//
//[-end-130715-IB07390108-add]//
//[-end-121114-IB08460014-modify]//
//[-start-130424-12360003-add]//
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PROTOCOL, GetBiosProtectTable) + sizeof (VOID*))) {
    mDxeChipsetSvc->GetBiosProtectTable = GetBiosProtectTable;
  }
//[-end-130424-12360003-add]//
  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gH2OChipsetServicesProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  mDxeChipsetSvc
                  );
  return Status;
}

