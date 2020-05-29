/** @file
  Statements that include other files

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

#include "PciHotPlugDxe.h"

//
// Instantiation of Driver private data.
//
PCIE_HOT_PLUG_DEVICE_PATH DefaultPcieHotPlugDevicePath [1] = {
  {
    ACPI(0x0),
    PCI(0x20, 0x8),
    END
  }
};

EFI_HPC_LOCATION          DefaultHpcLocation [1] = {
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &DefaultPcieHotPlugDevicePath[0],
    (EFI_DEVICE_PATH_PROTOCOL *) &DefaultPcieHotPlugDevicePath[0]
  }
};

UINTN                       NumberOfHotplugBridge = 0;
EFI_HPC_LOCATION            *HpcLocationTbl;
PCIE_HOT_PLUG_DEVICE_PATH   *PcieHotPlugDevicePathTbl;
HOT_PLUG_BRIDGE_INFO        *HotPlugBridgeInfoTable;


/**
  Entry point for the driver.

  This routine reads the PlatformType GPI on FWH and produces a protocol
  to be consumed by the chipset driver to effect those settings.


  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.

**/
EFI_STATUS
EFIAPI
PciHotPlugEntryPoint (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_STATUS            Status;
  PCI_HOT_PLUG_INSTANCE *PciHotPlug;
  
  //
  // OemServices
  //
  NumberOfHotplugBridge = 0;
  HotPlugBridgeInfoTable = NULL;

  Status = OemSvcGetHotplugBridgeInfo (
             &HotPlugBridgeInfoTable
             );

  if (!EFI_ERROR (Status)) {
    return Status;
  }

  ProcessHotplugBridgeInfoTable (HotPlugBridgeInfoTable);

  if (NumberOfHotplugBridge == 0) {
    return EFI_NOT_FOUND;
  }

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof (PCI_HOT_PLUG_INSTANCE),
                  (VOID **)&PciHotPlug
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Initialize driver private data.
  //
  ZeroMem (PciHotPlug, sizeof (PCI_HOT_PLUG_INSTANCE));

  PciHotPlug->Signature = EFI_PCIHOTPLUG_DRIVER_PRIVATE_SIGNATURE;
  PciHotPlug->HotPlugInitProtocol.GetRootHpcList      = GetRootHpcList;
  PciHotPlug->HotPlugInitProtocol.InitializeRootHpc   = InitializeRootHpc;
  PciHotPlug->HotPlugInitProtocol.GetResourcePadding  = GetResourcePadding;

  Status = gBS->InstallProtocolInterface (
                  &PciHotPlug->Handle,
                  &gEfiPciHotPlugInitProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &PciHotPlug->HotPlugInitProtocol
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

VOID
ProcessHotplugBridgeInfoTable (
  IN HOT_PLUG_BRIDGE_INFO  *HotPlugBridgeInfoTable
)
{
  EFI_STATUS  Status;
  UINTN       Index;
  UINTN       Index2;

  Index = 0;
  if (HotPlugBridgeInfoTable == NULL) {
    return;
  }
  //
  // Calculate how many hot plug bridge we have.
  //
  while (HotPlugBridgeInfoTable[Index].NodeUID!= END_OF_TABLE) {    
    if (HotPlugBridgeInfoTable[Index].NodeUID != IGNORE_DEVICE) {
      NumberOfHotplugBridge++;
    }
    Index++;
  }
  if (NumberOfHotplugBridge == 0) {
    //
    // There is no Hot plug bridge
    //
    return;
  }
  //
  // Prepare PcieHotPlugDevicePathTbl
  //
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  (sizeof (PCIE_HOT_PLUG_DEVICE_PATH) * NumberOfHotplugBridge),
                  (VOID **)&PcieHotPlugDevicePathTbl
                  );
  if (EFI_ERROR (Status)) {
    //
    // Fail to allocate memory for table, treat as no hot plug bridge.
    //
    NumberOfHotplugBridge = 0;
    return;
  }
  //
  // Initial Table default
  //
  ZeroMem (PcieHotPlugDevicePathTbl, (sizeof (PCIE_HOT_PLUG_DEVICE_PATH) * NumberOfHotplugBridge));
  for (Index = 0; Index < NumberOfHotplugBridge; Index++) {
    CopyMem ((VOID*)&PcieHotPlugDevicePathTbl[Index].PciRootBridgeNode, (VOID*)&DefaultPcieHotPlugDevicePath[0].PciRootBridgeNode, sizeof (PCIE_HOT_PLUG_DEVICE_PATH));
  }
  //
  // Prepare HotPlugBridgeInfoTable
  //
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  (sizeof (EFI_HPC_LOCATION) * NumberOfHotplugBridge),
                  (VOID **)&HpcLocationTbl
                  );
  if (EFI_ERROR (Status)) {
    //
    // Fail to allocate memory for table, treat as no hot plug bridge.
    //
    NumberOfHotplugBridge = 0;
    return;
  }
  //
  // Initial Table default
  //
  ZeroMem (HpcLocationTbl, (sizeof (EFI_HPC_LOCATION) * NumberOfHotplugBridge));
  for (Index = 0; Index < NumberOfHotplugBridge; Index++) {
    CopyMem ((VOID*)&HpcLocationTbl[Index].HpcDevicePath, (VOID*)&DefaultHpcLocation[0].HpcDevicePath, sizeof (EFI_HPC_LOCATION));
  }
  //
  // Initial table content by HotPlugBridgeInfoTable
  //
  Index = 0;
  Index2 = 0;
  while (HotPlugBridgeInfoTable[Index2].NodeUID != END_OF_TABLE) {
    if (HotPlugBridgeInfoTable[Index2].NodeUID != IGNORE_DEVICE) {
      PcieHotPlugDevicePathTbl[Index].PciRootBridgeNode.UID = HotPlugBridgeInfoTable[Index2].NodeUID;
      PcieHotPlugDevicePathTbl[Index].PciRootPortNode.Device = HotPlugBridgeInfoTable[Index2].DevNum;
      PcieHotPlugDevicePathTbl[Index].PciRootPortNode.Function = HotPlugBridgeInfoTable[Index2].FuncNum;

      HpcLocationTbl[Index].HpcDevicePath = (EFI_DEVICE_PATH_PROTOCOL *)&PcieHotPlugDevicePathTbl[Index];
      HpcLocationTbl[Index].HpbDevicePath = (EFI_DEVICE_PATH_PROTOCOL *)&PcieHotPlugDevicePathTbl[Index];
      Index++;
    }
    Index2++;
  }  

}

/**
  This procedure returns a list of Root Hot Plug controllers that require
  initialization during boot process


  @param[in] This            The pointer to the instance of the EFI_PCI_HOT_PLUG_INIT protocol.
  @param[in] HpcCount        The number of Root HPCs returned.
  @param[in] HpcList         The list of Root HPCs. HpcCount defines the number of elements in this list.

  @retval EFI_SUCCESS       
  
**/
EFI_STATUS
EFIAPI
GetRootHpcList (
  IN EFI_PCI_HOT_PLUG_INIT_PROTOCOL    *This,
  OUT UINTN                            *HpcCount,
  OUT EFI_HPC_LOCATION                 **HpcList
  )
{

  *HpcCount = (UINTN) NumberOfHotplugBridge;
  *HpcList  = HpcLocationTbl;

  return EFI_SUCCESS;
}

/**
  This procedure Initializes one Root Hot Plug Controller
  This process may casue initialization of its subordinate buses

  @param[in] This            The pointer to the instance of the EFI_PCI_HOT_PLUG_INIT protocol.
  @param[in] HpcDevicePath   The Device Path to the HPC that is being initialized.
  @param[in] HpcPciAddress   The address of the Hot Plug Controller function on the PCI bus.
  @param[in] Event           The event that should be signaled when the Hot Plug Controller initialization is complete. Set to NULL if the caller wants to wait until the entire initialization process is complete. The event must be of the type EFI_EVT_SIGNAL.
  @param[in] HpcState        The state of the Hot Plug Controller hardware. The type EFI_Hpc_STATE is defined in section 3.1.

  @retval EFI_SUCCESS       

**/
EFI_STATUS
EFIAPI
InitializeRootHpc (
  IN  EFI_PCI_HOT_PLUG_INIT_PROTOCOL      *This,
  IN  EFI_DEVICE_PATH_PROTOCOL            *HpcDevicePath,
  IN  UINT64                              HpcPciAddress,
  IN  EFI_EVENT                           Event, OPTIONAL
  OUT EFI_HPC_STATE                       *HpcState
  )
{
  if (Event) {
    gBS->SignalEvent (Event);
  }

  *HpcState = EFI_HPC_STATE_INITIALIZED;

  return EFI_SUCCESS;
}

/**
  Returns the resource padding required by the PCI bus that is controlled by the specified Hot Plug Controller.

  @param[in] This               The pointer to the instance of the EFI_PCI_HOT_PLUG_INIT protocol. initialized.
  @param[in] HpcDevicePath      The Device Path to the Hot Plug Controller.
  @param[in] HpcPciAddress      The address of the Hot Plug Controller function on the PCI bus.
  @param[in] HpcState           The state of the Hot Plug Controller hardware. The type EFI_HPC_STATE is defined in section 3.1.
  @param[in] Padding            This is the amount of resource padding required by the PCI bus under the control of the specified Hpc. Since the caller does not know the size of this buffer, this buffer is allocated by the callee and freed by the caller.
  @param[in] Attribute          Describes how padding is accounted for.

  @retval EFI_SUCCESS       
**/
EFI_STATUS
EFIAPI
GetResourcePadding (
  IN  EFI_PCI_HOT_PLUG_INIT_PROTOCOL  *This,
  IN  EFI_DEVICE_PATH_PROTOCOL        *HpcDevicePath,
  IN  UINT64                          HpcPciAddress,
  OUT EFI_HPC_STATE                   *HpcState,
  OUT VOID                            **Padding,
  OUT EFI_HPC_PADDING_ATTRIBUTES      *Attributes
  )
{
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *PaddingResource;
  EFI_STATUS                        Status;
  UINT64                            PciAddress;
  UINTN                             Index;

  Index = 0;
  while (HotPlugBridgeInfoTable[Index].NodeUID != END_OF_TABLE) {    
    if (HotPlugBridgeInfoTable[Index].NodeUID != IGNORE_DEVICE) {
      PciAddress = EFI_PCI_ADDRESS (HotPlugBridgeInfoTable[Index].NodeUID, HotPlugBridgeInfoTable[Index].DevNum, HotPlugBridgeInfoTable[Index].FuncNum, 0);
      if (HpcPciAddress == PciAddress) {
        break;
      }
    }
    Index++;
  }
  if (Index == NumberOfHotplugBridge) {
    return EFI_UNSUPPORTED;
  }

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  4 * sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) + sizeof (EFI_ACPI_END_TAG_DESCRIPTOR),
                  (VOID **)&PaddingResource
                  );
  ASSERT_EFI_ERROR (Status);

  *Padding = (VOID *) PaddingResource;

  //
  // Padding for bus
  //
  ZeroMem (PaddingResource, 4 * sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) + sizeof (EFI_ACPI_END_TAG_DESCRIPTOR));
  *Attributes                   = EfiPaddingPciBus;

  PaddingResource->Desc         = 0x8A;
  PaddingResource->Len          = 0x2B;
  PaddingResource->ResType      = ACPI_ADDRESS_SPACE_TYPE_BUS;
  PaddingResource->GenFlag      = 0x0;
  PaddingResource->SpecificFlag = 0;
  PaddingResource->AddrRangeMin = 0;
  PaddingResource->AddrRangeMax = 0;
  PaddingResource->AddrLen      = HotPlugBridgeInfoTable[Index].ReservedBusCount;
  //
  // Pad 0 Bus
  //
  // Padding for non-prefetchable memory
  //
  PaddingResource++;
  PaddingResource->Desc                 = 0x8A;
  PaddingResource->Len                  = 0x2B;
  PaddingResource->ResType              = ACPI_ADDRESS_SPACE_TYPE_MEM;
  PaddingResource->GenFlag              = 0x0;
  PaddingResource->AddrSpaceGranularity = 32;
  PaddingResource->SpecificFlag         = 0;
  //
  // Pad non-prefetchable
  //
  PaddingResource->AddrRangeMin = 0;
  PaddingResource->AddrLen      = HotPlugBridgeInfoTable[Index].ReservedNonPrefetchableMmio;
  //
  // Force minimum 4KByte alignment for Virtualization technology for Directed I/O
  //
  if (PaddingResource->AddrLen < SIZE_4K) {
    PaddingResource->AddrRangeMax = SIZE_4K - 1;    
  } else if (PaddingResource->AddrLen > SIZE_16M) {
    PaddingResource->AddrRangeMax = HotPlugBridgeInfoTable[Index].AlignemntOfReservedNonPrefetchableMmio - 1;
  } else  {
    PaddingResource->AddrRangeMax = SIZE_16M - 1;
  }
  //
  // Padding for prefetchable memory
  //
  PaddingResource++;
  PaddingResource->Desc                 = 0x8A;
  PaddingResource->Len                  = 0x2B;
  PaddingResource->ResType              = ACPI_ADDRESS_SPACE_TYPE_MEM;
  PaddingResource->GenFlag              = 0x0;
  PaddingResource->AddrSpaceGranularity = 32;
  PaddingResource->SpecificFlag         = 06;
  //
  // Pad prefetchable
  //
  PaddingResource->AddrRangeMin = 0;
  PaddingResource->AddrLen      = HotPlugBridgeInfoTable[Index].ReservedPrefetchableMmio;
  //
  // Force minimum 4KByte alignment for Virtualization technology for Directed I/O
  //
  if (PaddingResource->AddrLen < SIZE_4K) {
    PaddingResource->AddrRangeMax = SIZE_4K - 1;    
  } else if (PaddingResource->AddrLen > SIZE_16M) {
    PaddingResource->AddrRangeMax = HotPlugBridgeInfoTable[Index].AlignemntOfReservedPrefetchableMmio - 1;
  } else  {
    PaddingResource->AddrRangeMax = SIZE_16M - 1;
  }
  //
  // Padding for I/O
  //
  PaddingResource++;
  PaddingResource->Desc         = 0x8A;
  PaddingResource->Len          = 0x2B;
  PaddingResource->ResType      = ACPI_ADDRESS_SPACE_TYPE_IO;
  PaddingResource->GenFlag      = 0x0;
  PaddingResource->SpecificFlag = 0;
  PaddingResource->AddrRangeMin = 0;
  PaddingResource->AddrLen      = HotPlugBridgeInfoTable[Index].ReservedIoRange;
  //
  // Pad 4K of IO
  //
  PaddingResource->AddrRangeMax = HotPlugBridgeInfoTable[Index].ReservedIoRange / 0x1000;
  //
  // Alignment
  //
  // Terminate the entries.
  //
  PaddingResource++;
  ((EFI_ACPI_END_TAG_DESCRIPTOR *) PaddingResource)->Desc     = ACPI_END_TAG_DESCRIPTOR;
  ((EFI_ACPI_END_TAG_DESCRIPTOR *) PaddingResource)->Checksum = 0x0;

  *HpcState = EFI_HPC_STATE_INITIALIZED | EFI_HPC_STATE_ENABLED;

  return EFI_SUCCESS;
}
