//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "ChipsetLibServicesSmm.h"

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
  EFI_STATUS                                Status;
  BOOLEAN                                   InSmm;
  EFI_SMM_SYSTEM_TABLE                      *SmmSt;
  EFI_SMM_RUNTIME_PROTOCOL                  *SmmRT;
  EFI_SMM_CHIPSET_LIB_SERVICES_PROTOCOL     *SmmChipsetLibServices;
  EFI_SMM_CHIPSET_LIB_SERVICES_PROTOCOL     *OldSmmChipsetLibServices;
//[-start-110905-IB08450015-add]//
  EFI_CHIPSET_LIB_SERVICES_PROTOCOL         *OldChipsetLibServicesInSmm;
//[-end-110905-IB08450015-add]//

  //
  // Initialize the EFI Runtime Library
  //
  InsydeEfiInitializeSmmDriverLib (ImageHandle, SystemTable, &InSmm);

  if (!InSmm) {
//[-start-110914-IB08450017-modify]//
    Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    sizeof (EFI_SMM_CHIPSET_LIB_SERVICES_PROTOCOL),
                    &SmmChipsetLibServices
                    );
  
    if (EFI_ERROR (Status)) {
      return Status;
    }
  
    LinkChipsetLibProtocol (SmmChipsetLibServices);
  
    //
    // Setup Chipset Lib Interface in BS
    //
    Status = gBS->InstallProtocolInterface (
                    &ImageHandle,
                    &gEfiSmmChipsetLibServicesProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    SmmChipsetLibServices
                    );
//[-end-110914-IB08450017-modify]//
  } else {

    Status = gBS->LocateProtocol (&gEfiSmmRuntimeProtocolGuid, NULL, &SmmRT);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = gSMM->GetSmstLocation (gSMM, &SmmSt);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = SmmSt->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      sizeof (EFI_SMM_CHIPSET_LIB_SERVICES_PROTOCOL),
                      &SmmChipsetLibServices
                      );
    if (EFI_ERROR (Status)) {
      return Status;
    }

//[-start-110905-IB08450015-add]//
    //
    // Make sure EFI_SMM_CHIPSET_LIB_SERVICES_PROTOCOL and EFI_CHIPSET_LIB_SERVICES_PROTOCOL
    // are same in content. In the below code, the protocol instance will be
    // installed with both GUID with same content.  
    //
//[-end-110905-IB08450015-add]//
    LinkChipsetLibProtocol (SmmChipsetLibServices);

    //
    // Check if we need to reinstall
    //
    Status = SmmRT->LocateProtocol (
                      &gEfiSmmChipsetLibServicesProtocolGuid,
                      NULL,
                      &OldSmmChipsetLibServices
                      );
    if (EFI_ERROR(Status)) {
      //
      // Install Chipset Lib Interface in SMM
      //
      Status = SmmRT->InstallProtocolInterface (
                        &ImageHandle,
                        &gEfiSmmChipsetLibServicesProtocolGuid,
                        EFI_NATIVE_INTERFACE,
                        SmmChipsetLibServices
                        );
    } else {
      //
      // ReInstall Chipset Lib Interface in SMM
      //
      Status = SmmRT->ReinstallProtocolInterface (
                        &ImageHandle,
                        &gEfiSmmChipsetLibServicesProtocolGuid,
                        OldSmmChipsetLibServices,
                        SmmChipsetLibServices
                        );
    }
    
//[-start-110905-IB08450015-add]//

    //
    // Check if we need to reinstall 
    // 2nd time with ChipsetLibServicesProtocolGuid.
    //
    Status = SmmRT->LocateProtocol (
                      &gEfiChipsetLibServicesProtocolGuid,
                      NULL,
                      &OldChipsetLibServicesInSmm
                      );
    if (EFI_ERROR(Status)) {
      //
      // Install Chipset Lib Interface in SMM
      //
      Status = SmmRT->InstallProtocolInterface (
                        &ImageHandle,
                        &gEfiChipsetLibServicesProtocolGuid,
                        EFI_NATIVE_INTERFACE,
                        SmmChipsetLibServices
                        );
    } else {
      //
      // ReInstall Chipset Lib Interface in SMM
      //
      Status = SmmRT->ReinstallProtocolInterface (
                        &ImageHandle,
                        &gEfiChipsetLibServicesProtocolGuid,
                        OldChipsetLibServicesInSmm,
                        SmmChipsetLibServices
                        );
    }
//[-end-110905-IB08450015-add]//
  }
  return Status;
}

VOID
LinkChipsetLibProtocol (
  IN OUT   EFI_SMM_CHIPSET_LIB_SERVICES_PROTOCOL    *ProtocolServices
)
/*++

  Routine Description:

    Link Chipset Lib Protocol Functions

  Arguments:

    EFI_SMM_CHIPSET_LIB_SERVICES_PROTOCOL - Protocol Services Interface

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
