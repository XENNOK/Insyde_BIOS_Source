//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "ChipsetLibServicesDxe.h"

EFI_DRIVER_ENTRY_POINT (ChipsetLibServicesEntryPoint)

EFI_STATUS
ChipsetLibServicesEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
)
/*++

  Routine Description:
    The entry point is to install chipset lib interface.
  Arguments:
    EFI_HANDLE       - ImageHandle.
    EFI_SYSTEM_TABLE - SystemTable: A pointer to the EFI system table
  Returns:
    EFI_STATUS

--*/
{
  EFI_STATUS                             Status;
  EFI_CHIPSET_LIB_SERVICES_PROTOCOL      *ChipsetLibServices;

  //
  // Initialize the Driver
  //
  EfiInitializeDriverLib (ImageHandle, SystemTable);
//[-start-111128-IB08130011-add]//
  INITIALIZE_SCRIPT (ImageHandle, SystemTable);
//[-end-111128-IB08130011-add]//

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof (EFI_CHIPSET_LIB_SERVICES_PROTOCOL),
                  &ChipsetLibServices
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  LinkChipsetLibProtocol (ChipsetLibServices);

  //
  // Setup Chipset Lib Interface in BS
  //
  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gEfiChipsetLibServicesProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  ChipsetLibServices
                  );
  return Status;
}

VOID
LinkChipsetLibProtocol (
  IN OUT   EFI_CHIPSET_LIB_SERVICES_PROTOCOL    *ProtocolServices
)
/*++

  Routine Description:
    Link Chipset Lib Protocol Functions
  Arguments:
    EFI_CHIPSET_LIB_SERVICES_PROTOCOL - Protocol Services Interface
  Returns:
    VOID

--*/
{
  ProtocolServices->SataComreset = SATAComReset;
  ProtocolServices->SetAhciEnableBit = SetAhciEnableBit;
  ProtocolServices->EnableFvbWrites = EnableFvbWrites;
  ProtocolServices->InitFlashMode = InitFlashMode;
  ProtocolServices->SelectFlashInterface = SelectFlashInterface;
  ProtocolServices->IsSpiFlashDevice = IsSpiFlashDevice;
  ProtocolServices->IsLpcFlashDevice = IsLpcFlashDevice;
  ProtocolServices->GetFlashTable = GetFlashTable;
  ProtocolServices->GetFlashNumber = GetFlashNumber;
  ProtocolServices->GetSpiSupportNumber = GetSpiSupportNumber;
  ProtocolServices->GetSpiFdSupportTable = GetSpiFdSupportTable;
  ProtocolServices->SpiMemoryToBiosRegionAddress = SpiMemoryToBiosRegionAddress;
  ProtocolServices->NeedToBuildIdeData = NeedToBuildIdeData;
  ProtocolServices->IsRaidOpRom = IsRAIDOpRom;
  ProtocolServices->RaidAhciCdromSupport = RaidAhciCdromSupport;
#ifndef SEAMLESS_AHCI_SUPPORT
  ProtocolServices->UpdateBBSDeviceType = UpdateBBSDeviceType;
#endif
  ProtocolServices->NeedToOverWriteVar = NeedToOverWriteVar;
  ProtocolServices->DifferentSizeStrCmp = DifferentSizeStrCmp;
  ProtocolServices->ResetSystem = ResetSystem;
  ProtocolServices->GetPortNumberMapTable = GetPortNumberMapTable;
  ProtocolServices->IoDecodeControl = IoDecodeControl;
  ProtocolServices->MmioDecodeControl = MmioDecodeControl;
  ProtocolServices->IsaPositiveDecodeModeChecking = IsaPositiveDecodeModeChecking;
  ProtocolServices->LegacyRegionAccessCtrl = LegacyRegionAccessCtrl;
  ProtocolServices->ProgramChipsetSSID = ProgramChipsetSSID;
}
