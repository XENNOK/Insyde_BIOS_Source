/** @file
  PciPlatformDxe driver will produces the PciPlatform Protocol for Dxe driver.

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

//[-start-120712-IB10820083-modify]// 
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/PciPlatform.h>
#include <Protocol/PlatformPhaseNotify.h>
#include <Protocol/PciEnumerationComplete.h>
#include <PchAccess.h>
#include <IrqRoutingInformation.h>

EFI_STATUS
EFIAPI
PlatformNotify (
  IN EFI_PCI_PLATFORM_PROTOCOL                        *This,
  IN EFI_HANDLE                                       HostBridge,
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PHASE    Phase,
  IN EFI_PCI_CHIPSET_EXECUTION_PHASE                  ChipsetPhase
  );

EFI_STATUS
EFIAPI
PlatformPrepController (
  IN EFI_PCI_PLATFORM_PROTOCOL                      *This,
  IN EFI_HANDLE                                     HostBridge,
  IN EFI_HANDLE                                     RootBridge,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS    PciAddress,
  IN EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE   Phase,
  IN EFI_PCI_CHIPSET_EXECUTION_PHASE                ChipsetPhase
  );

EFI_STATUS
EFIAPI
GetPlatformPolicy (
  IN  CONST EFI_PCI_PLATFORM_PROTOCOL               *This,
  OUT       EFI_PCI_PLATFORM_POLICY                 *PciPolicy
  );

EFI_STATUS
EFIAPI
GetPciRom (
  IN CONST EFI_PCI_PLATFORM_PROTOCOL                 *This,
  IN       EFI_HANDLE                                PciHandle,
  OUT      VOID                                      **RomImage,
  OUT      UINTN                                     *RomSize
  );
//[-end-120712-IB10820083-modify]// 

STATIC
VOID
EFIAPI
PciEnumCompletedNotifyFunction (
  IN     EFI_EVENT                      Event,
  IN     VOID                           *Context
  );

EFI_HANDLE                mPciPlatformHandle = NULL;
EFI_PCI_PLATFORM_PROTOCOL mPciPlatform = {
  PlatformNotify,
  PlatformPrepController,
  GetPlatformPolicy,
  GetPciRom
};

EFI_STATUS
EFIAPI
PciPlatformDriverEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS    Status;
  EFI_EVENT     Event;
  VOID          *Registration;
  //
  // Registers the gEfiPciEnumerationCompleteProtocolGuid protocol notification for PCIe port switching
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  PciEnumCompletedNotifyFunction,
                  NULL,
                  &Event
                  );
  if (Status == EFI_SUCCESS) {
    Status = gBS->RegisterProtocolNotify (
                    &gEfiPciEnumerationCompleteProtocolGuid,
                    Event,
                    &Registration
                    );
  }
  Status = gBS->InstallProtocolInterface (
                &mPciPlatformHandle,
                &gEfiPciPlatformProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &mPciPlatform
                );

  return Status;
}

EFI_STATUS
EFIAPI
PlatformNotify (
  IN  EFI_PCI_PLATFORM_PROTOCOL                      *This,
  IN  EFI_HANDLE                                     HostBridge,
  IN  EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PHASE  Phase,
  IN  EFI_PCI_CHIPSET_EXECUTION_PHASE                ExecPhase
  )
{
  EFI_STATUS        Status;

  if ( ( Phase == EfiPciHostBridgeEndResourceAllocation ) && ( ExecPhase == ChipsetExit ) ) {
    Status = gBS->InstallProtocolInterface (
               &mPciPlatformHandle,
               &gEfiPciHostBridgeEndResourceAllocationNotifyGuid,
               EFI_NATIVE_INTERFACE,
               NULL
               );

    return Status;
  }

  return EFI_SUCCESS;
}

/**

  GC_TODO: Add function description

  @param  This                  Pointer to the  EFI_PCI_PLATFORM_PROTOCOL  instance.
  @param  HostBridge            The associated PCI host bridge handle.
  @param  RootBridge            The associated PCI root bridge handle.
  @param  PciAddress            The address of the PCI device on the PCI bus.
  @param  Phase                 The phase of the PCI controller enumeration.
  @param  ChipsetPhase          Defines the execution phase of the PCI chipset driver.

  @retval EFI_UNSUPPORTED
          EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
PlatformPrepController (
  IN  EFI_PCI_PLATFORM_PROTOCOL                      *This,
  IN  EFI_HANDLE                                     HostBridge,
  IN  EFI_HANDLE                                     RootBridge,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS    PciAddress,
  IN  EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE   Phase,
  IN  EFI_PCI_CHIPSET_EXECUTION_PHASE                ChipsetPhase
  )
{
  //
  // If there is any code in this function, Default Status must be EFI_SUCCESS
  //
  EFI_STATUS            Status = EFI_INVALID_PARAMETER;
//  EFI_STATUS            Status = EFI_SUCCESS;

  return Status;
}

EFI_STATUS
EFIAPI
GetPlatformPolicy (
  IN CONST EFI_PCI_PLATFORM_PROTOCOL               *This,
  OUT EFI_PCI_PLATFORM_POLICY                      *PciPolicy
  )
{
  //
  // If there is any policy in this function, Default Status must be EFI_SUCCESS
  //
  EFI_STATUS            Status = EFI_INVALID_PARAMETER;
//  EFI_STATUS            Status = EFI_SUCCESS;

  return Status;
}

/**

  Return a PCI ROM image for the onboard device represented by PciHandle

  @param  This                  Protocol instance pointer.
  @param  PciHandle             PCI device to return the ROM image for.
  @param  RomImage              PCI Rom Image for onboard device
  @param  RomSize               Size of RomImage in bytes


  @retval EFI_SUCCESS           RomImage is valid
          EFI_NOT_FOUND         No RomImageEFI_UNSUPPORTED

**/
EFI_STATUS
EFIAPI
GetPciRom (
  IN  CONST EFI_PCI_PLATFORM_PROTOCOL                   *This,
  IN  EFI_HANDLE                                  PciHandle,
  OUT VOID                                        **RomImage,
  OUT UINTN                                       *RomSize
  )
{
  //
  // If there is any code in this function, Default Status must be EFI_SUCCESS
  //
//[-start-130111-IB11410041-modify]//
//  EFI_STATUS            Status = EFI_INVALID_PARAMETER;
  EFI_STATUS            Status = EFI_SUCCESS;
//[-end-130111-IB11410041-modify]//

  return Status;
}

/**

  The gEfiPciEnumerationCompleteProtocolGuid installed callback routine
  This routine is going to adjust the VirtualSecondaryBus number in 
  PcdVirtualBusTable to match the actual of PCIe root port number when 
  PCIe root port switched. Without this adjustment, the Pirq table will
  based on incorrect virtual bus table to produce wrong IRQ setting to PCIe
  downstring port devices

  @param[in]  Event                 Event                
  @param[in]  Context               Context for the event

**/
STATIC
VOID
EFIAPI
PciEnumCompletedNotifyFunction (
  IN     EFI_EVENT                      Event,
  IN     VOID                           *Context
  )
{
  EFI_STATUS              Status;
  VOID                    *Interface;
  LEGACY_MODIFY_PIR_TABLE *VirtualBusTable;
  UINTN                   VirtualBusTableEntries;
  INT8                    *VirtualBusConvertTable;
  UINT32                  RootPortFunctionNumber;
  UINTN                   Index;
  UINTN                   Function;
  UINT8                   ChangedFunction;
  INT8                    ChangedVirtualBus;
  //
  // Insure the protocol really installed
  //
  Status = gBS->LocateProtocol (
                  &gEfiPciEnumerationCompleteProtocolGuid,
                  NULL,
                  (VOID **)&Interface
                  );
  if (Status != EFI_SUCCESS) {
    return;
  }  
  //
  // Close event for one time shot
  //
  gBS->CloseEvent (Event);
  VirtualBusTable        = PcdGetExPtr (&gChipsetPkgTokenSpaceGuid, PcdVirtualBusTable);
  VirtualBusTableEntries = (UINTN)(LibPcdGetExSize (&gChipsetPkgTokenSpaceGuid, PcdToken(PcdVirtualBusTable)) / sizeof (LEGACY_MODIFY_PIR_TABLE));
  if (VirtualBusTable == NULL || VirtualBusTableEntries == 0) {
    return;
  }	
  //
  // Allocate virtual bus lookup table for PCIe port switching
  //
  VirtualBusConvertTable = AllocateZeroPool (VirtualBusTableEntries);
  if (VirtualBusConvertTable == NULL) {
    return;
  }	
  //
  // Copy virtial bus produced by PCIe(device 0x1c) into lookup table
  //
  for (Index = 0; Index < VirtualBusTableEntries; Index ++) {
    if (VirtualBusTable[Index].BridgeDevice == 0x1c) {
      VirtualBusConvertTable[Index] = VirtualBusTable[Index].VirtualSecondaryBus;
    }
  }
  //
  // Adjust virtual bus value based on actual PCIe device port position
  //
  RootPortFunctionNumber = MmioRead32 (PCH_RCRB_BASE + R_PCH_RCRB_RPFN);
  for (Function = 0; Function < 8; Function ++) {
    ChangedFunction = (UINT8)((RootPortFunctionNumber >> (Function * S_PCH_RCRB_PRFN_RP_FIELD)) & B_PCH_RCRB_RPFN_RP1FN);
    for (Index = 0, ChangedVirtualBus = 0; Index < VirtualBusTableEntries; Index ++) {
      if (VirtualBusTable[Index].BridgeDevice == 0x1c && VirtualBusTable[Index].BridgeFunction == ChangedFunction) {
        ChangedVirtualBus = VirtualBusConvertTable[Index];
        break;
      }
    }
    if (ChangedVirtualBus != 0) {
      for (Index = 0; Index < VirtualBusTableEntries; Index ++) {
        if (VirtualBusTable[Index].BridgeDevice == 0x1c && VirtualBusTable[Index].BridgeFunction == Function) {
          VirtualBusTable[Index].VirtualSecondaryBus = ChangedVirtualBus;
          break;
        }
      }
    }
  }
  FreePool (VirtualBusConvertTable);
}