/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <SaPolicy.h>
//[-start-130812-IB06720232-add]//
#include <Library/BaseOemSvcChipsetLib.h>
//[-end-130812-IB06720232-add]//

//[-start-120404-IB05300309-add]//
PCIE_ASPM_DEV_INFO mPcieAspmDevsOverride[] = {
  ///
  /// Tekoa w/o iAMT
  ///
  {0x8086, 0x108b, 0xff, 2, 2},
  ///
  /// Tekoa A2
  ///
  {0x8086, 0x108c, 0x00, 0, 0},
  ///
  /// Tekoa others
  ///
  {0x8086, 0x108c, 0xff, 2, 2},
  ///
  /// Vidalia
  ///
  {0x8086, 0x109a, 0xff, 2, 2},
  ///
  /// 3945ABG
  ///
  {0x8086, 0x4222, 0xff, 2, 3},
  ///
  /// 3945ABG
  ///
  {0x8086, 0x4227, 0xff, 2, 3},
  ///
  /// 3945ABG
  ///
  {0x8086, 0x4228, 0xff, 2, 3},
  
  {0x10DE, 0x06E4, 0xff, 2, 2},
  ///
  /// End of table
  ///
  {SA_PCIE_DEV_END_OF_TABLE, 0, 0, 0, 0}
};

PCIE_LTR_DEV_INFO mPcieLtrDevsOverride[] = {
  ///
  /// Place holder for PCIe devices with correct LTR requirements
  ///
  ///
  /// End of table
  ///
  {SA_PCIE_DEV_END_OF_TABLE, 0, 0, 0, 0}
};
//[-end-120404-IB05300309-add]//

EFI_STATUS
EFIAPI
SAPolicyEntry (
  IN      EFI_HANDLE              ImageHandle,
  IN      EFI_SYSTEM_TABLE        *SystemTable
 )
{
  EFI_STATUS                             Status;
  EFI_HANDLE                             Handle;
  DXE_PLATFORM_SA_POLICY_PROTOCOL        *SaPlatformPolicy;
  EFI_SETUP_UTILITY_PROTOCOL             *SetupUtility;
  CHIPSET_CONFIGURATION                   *SetupVariable;
  EFI_PHYSICAL_ADDRESS                   RmrrUsbBaseAddress;
  EFI_PHYSICAL_ADDRESS                   RmrrUsbBaseAddressArray[2];
  UINT32                                 VTDBaseAddress[SA_VTD_ENGINE_NUMBER];
  EFI_HOB_GUID_TYPE                      *GuidHob;
  UINT8                                  *DimmSmbusAddrArray;
  UINT8                                  SpdAddressTable[SA_MC_MAX_SOCKETS];
  UINT8                                  Index;
//[-start-120629-IB06460411-add]//
  UINT8                                  BoardID;
  SA_PLATFORM_BOARD_ID                   PlatformBoardID;
//[-end-120629-IB06460411-add]//
//[-start-121004-IB10370017-add]//
  PLATFORM_INFO_PROTOCOL                 *PlatformInfoProtocol = NULL;
//[-end-121004-IB10370017-add]//
//[-start-121004-IB10370017-remove]//
//[-start-120726-IB05330361-add]//
//#ifdef ULT_SUPPORT
//  UINT8                                  BoardID2;
//#endif
//[-end-120726-IB05330361-add]//
//[-end-121004-IB10370017-remove]//
//[-start-120925-IB06150247-add]//
  PLATFORM_GOP_POLICY_PROTOCOL           *PlatformGopPolicyPtr;
//[-end-120925-IB06150247-add]//
//[-start-121012-IB03780464-add]//
  VOID                                   *Registration;
//[-end-121012-IB03780464-add]//

  Handle             = NULL;
  SaPlatformPolicy   = NULL;
  SetupUtility       = NULL;
  SetupVariable      = NULL;
  RmrrUsbBaseAddress = 0;
  GuidHob            = NULL;
  DimmSmbusAddrArray = NULL;
  Index              = 0;
//[-start-120629-IB06460411-add]//
  BoardID            = 0;
  PlatformBoardID    = SAPlatformBoardIDGraysReef;
//[-end-120629-IB06460411-add]//

  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\nSA Policy Entry\n"));

  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility);
  ASSERT_EFI_ERROR (Status);
  SetupVariable = (CHIPSET_CONFIGURATION *)(SetupUtility->SetupNvData);

  //
  // Allocate and set SA Policy structure to known value
  //
  SaPlatformPolicy = NULL;
  SaPlatformPolicy = AllocateZeroPool (sizeof (DXE_PLATFORM_SA_POLICY_PROTOCOL));
  ASSERT (SaPlatformPolicy != NULL);
//[-start-130104-IB10870064-add]//
  if (SaPlatformPolicy == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

  SaPlatformPolicy->Vtd = NULL;
  SaPlatformPolicy->Vtd = AllocateZeroPool (sizeof (SA_VTD_CONFIGURATION));
  ASSERT (SaPlatformPolicy->Vtd != NULL);
//[-start-130104-IB10870064-add]//
  if (SaPlatformPolicy->Vtd == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

  SaPlatformPolicy->Vtd->RmrrUsbBaseAddress = NULL;
  SaPlatformPolicy->Vtd->RmrrUsbBaseAddress = AllocateZeroPool (sizeof (EFI_PHYSICAL_ADDRESS) * 2);
  ASSERT (SaPlatformPolicy->Vtd->RmrrUsbBaseAddress != NULL);
//[-start-130104-IB10870064-add]//
  if (SaPlatformPolicy->Vtd->RmrrUsbBaseAddress == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

  RmrrUsbBaseAddress = (EFI_PHYSICAL_ADDRESS)(MEM_EQU_4GB - 1);
  Status = (gBS->AllocatePages) (
               AllocateMaxAddress,
               EfiReservedMemoryType,
               EFI_SIZE_TO_PAGES (RMRR_USB_MEM_SIZE),
               &RmrrUsbBaseAddress
               );
  ASSERT_EFI_ERROR (Status);
  SetMem ((VOID *)RmrrUsbBaseAddress, RMRR_USB_MEM_SIZE, 0);

  SaPlatformPolicy->PcieConfig = NULL;
  SaPlatformPolicy->PcieConfig = AllocateZeroPool (sizeof (SA_PCIE_CONFIGURATION));
  ASSERT (SaPlatformPolicy->PcieConfig != NULL);
//[-start-130104-IB10870064-add]//
  if (SaPlatformPolicy->PcieConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

  SaPlatformPolicy->MemoryConfig  = NULL;
  SaPlatformPolicy->MemoryConfig  = AllocateZeroPool (sizeof (SA_MEMORY_CONFIGURATION));
  ASSERT (SaPlatformPolicy->MemoryConfig != NULL);
//[-start-130104-IB10870064-add]//
  if (SaPlatformPolicy->MemoryConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

  SaPlatformPolicy->MemoryConfig->SpdAddressTable = NULL;
  SaPlatformPolicy->MemoryConfig->SpdAddressTable = AllocateZeroPool (sizeof (UINT8) * SA_MC_MAX_SOCKETS);
  ASSERT (SaPlatformPolicy->MemoryConfig->SpdAddressTable != NULL);
//[-start-130104-IB10870064-add]//
  if (SaPlatformPolicy->MemoryConfig->SpdAddressTable == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

  SaPlatformPolicy->IgdConfig = NULL;
  SaPlatformPolicy->IgdConfig = AllocateZeroPool (sizeof (SA_IGD_CONFIGURATION));
  ASSERT (SaPlatformPolicy->IgdConfig != NULL);
//[-start-130104-IB10870064-add]//
  if (SaPlatformPolicy->IgdConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

  SaPlatformPolicy->MiscConfig = NULL;
  SaPlatformPolicy->MiscConfig = AllocateZeroPool (sizeof (SA_MISC_CONFIGURATION));
  ASSERT (SaPlatformPolicy->MiscConfig != NULL);
//[-start-130104-IB10870064-add]//
  if (SaPlatformPolicy->MiscConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

  SaPlatformPolicy->MiscConfig->DefaultSvidSid = NULL;
  SaPlatformPolicy->MiscConfig->DefaultSvidSid = AllocateZeroPool (sizeof (SA_DEFAULT_SVID_SID));
  ASSERT (SaPlatformPolicy->MiscConfig->DefaultSvidSid != NULL);
//[-start-130104-IB10870064-add]//
  if (SaPlatformPolicy->MiscConfig->DefaultSvidSid == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//
//[-start-140219-IB10920078-modify]//
//[-start-120717-IB10040004-remove]//
////[-start-120627-IB05330350-add]//
  if (FeaturePcdGet (PcdIntelSwitchableGraphicsSupported)) { 
  //[-start-120705-IB05330353-remove]//
  //  Status = (gBS->AllocatePool) (EfiBootServicesData, sizeof (SA_SG_VBIOS_CONFIGURATION), &(mDxePlatformSaPolicy.VbiosConfig));
  //[-end-120705-IB05330353-remove]//
    SaPlatformPolicy->VbiosConfig = NULL;
    SaPlatformPolicy->VbiosConfig = AllocateZeroPool (sizeof (SA_SG_VBIOS_CONFIGURATION));
//[-start-140625-IB05080432-modify]//
    if (SaPlatformPolicy->VbiosConfig == NULL) {
      ASSERT (SaPlatformPolicy->VbiosConfig != NULL);
      return EFI_OUT_OF_RESOURCES;
    }
//[-end-140625-IB05080432-modify]//
  }
////[-end-120627-IB05330350-add]//
//[-end-120717-IB10040004-remove]//
//[-end-140219-IB10920078-modify]//

//[-start-130104-IB10870064-remove]//
//  if (
//     (SaPlatformPolicy                                == NULL) ||
//     (SaPlatformPolicy->Vtd                           == NULL) ||
//     (SaPlatformPolicy->Vtd->RmrrUsbBaseAddress       == NULL) ||
//     (SaPlatformPolicy->PcieConfig                    == NULL) ||
//     (SaPlatformPolicy->MemoryConfig                  == NULL) ||
//     (SaPlatformPolicy->MemoryConfig->SpdAddressTable == NULL) ||
//     (SaPlatformPolicy->IgdConfig                     == NULL) ||
//     (SaPlatformPolicy->MiscConfig                    == NULL) ||
//     (SaPlatformPolicy->MiscConfig->DefaultSvidSid    == NULL)
//    ) {
//    return EFI_OUT_OF_RESOURCES;
//  }
//[-end-130104-IB10870064-remove]//
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // SaPlatformPolicy->Vtd->RmrrUsbBaseAddress
  //
  RmrrUsbBaseAddressArray[0] = RmrrUsbBaseAddress;
  RmrrUsbBaseAddressArray[1] = RmrrUsbBaseAddress + RMRR_USB_MEM_SIZE - 1;

  //
  // SaPlatformPolicy->Vtd->BaseAddress
  //
  VTDBaseAddress[0] = 0xFED90000;
  VTDBaseAddress[1] = 0xFED91000;

  //
  // SaPlatformPolicy->MemoryConfig->SpdAddressTable
  //
  GuidHob = NULL;
  GuidHob = GetFirstGuidHob (&gDimmSmbusAddrHobGuid);
  ASSERT (GuidHob != NULL);
  if (GuidHob != NULL) {
    DimmSmbusAddrArray = (UINT8 *)(UINTN)(GET_GUID_HOB_DATA (GuidHob));
    for (Index = 0; Index < SA_MC_MAX_SOCKETS; Index = Index + 1) {
      SpdAddressTable[Index] = DimmSmbusAddrArray[Index];
    }
  } else {
    SpdAddressTable[0] = DIMM_SMB_SPD_P0C0D0;
    SpdAddressTable[1] = DIMM_SMB_SPD_P0C0D1;
    SpdAddressTable[2] = DIMM_SMB_SPD_P0C1D0;
    SpdAddressTable[3] = DIMM_SMB_SPD_P0C1D1;
  }

  //
//[-start-120629-IB06460411-add]//
  //
  // SAPlatformPolicy->PlatformBoardID
  //
//[-start-121004-IB10370017-modify]//
//[-start-120726-IB05330361-modify]//
//  if (FeaturePcdGet ( PcdUltFlag ) ) {
////[-start-121114-IB052800010-modify]//
//    GetUltBoardId ( &BoardID,&BoardID2);
////[-end-121114-IB052800010-modify]//
//  } else {
////[-start-121114-IB052800010-modify]//
//    GetBoardId ( &BoardID );
////[-end-121114-IB052800010-modify]//
//  }
  Status = gBS->LocateProtocol (&gEfiPlatformInfoProtocolGuid, NULL, (VOID **)&PlatformInfoProtocol);
  ASSERT_EFI_ERROR (Status);    
  BoardID = PlatformInfoProtocol->PlatInfo.BoardId;
//[-end-120726-IB05330361-modify]//
//[-end-121004-IB10370017-modify]//
//[-start-130422-IB05400398-remove]//
//  switch (BoardID & B_EC_GET_CRB_BOARD_ID_BOARD_ID) {
//  case V_EC_GET_CRB_BOARD_ID_BOARD_ID_GRAYS_REEF:
//    PlatformBoardID = SAPlatformBoardIDGraysReef;
//    break;
//
//  case V_EC_GET_CRB_BOARD_ID_BOARD_ID_BASKING_RIDGE:
//    PlatformBoardID = SAPlatformBoardIDBaskingRidge;
//    break;
//
//  default:
//    break;
//  }
//[-end-130422-IB05400398-remove]//
//[-end-120629-IB06460411-add]//
  // SA DXE Policy Init
  //
//[-start-121212-IB06460476-modify]//
//[-start-121113-IB06150257-modify]//
  SaPlatformPolicy->Revision                                           = DXE_SA_PLATFORM_POLICY_PROTOCOL_REVISION_8;
//[-end-121113-IB06150257-modify]//
//[-end-121212-IB06460476-modify]//

  //
  // Initialzie the VTD Configuration
  //
  SaPlatformPolicy->Vtd->VtdEnable                                     = SetupVariable->VTD;
  CopyMem (SaPlatformPolicy->Vtd->RmrrUsbBaseAddress, RmrrUsbBaseAddressArray, sizeof (EFI_PHYSICAL_ADDRESS) * 2);
  CopyMem (SaPlatformPolicy->Vtd->BaseAddress, VTDBaseAddress, sizeof (UINT32) * SA_VTD_ENGINE_NUMBER);

//[-start-120904-IB06150243-add]//
  //
  // Follow SA BIOS Spec Rev0.6.0 Section 3.2.6 to disable SA DMI ASPM for A1 stepping PCH.
  // This workaround should remove in production phase.
  //
  if (MmPci8 (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC, R_PCH_LPC_RID ) <= V_PCH_LPT_LPC_RID_1) {
    SetupVariable->SaDmiAspm                                           = 0;
  }
//[-end-120904-IB06150243-add]//

  //
  // Initialize the PCIE Configuration
  //
  SaPlatformPolicy->PcieConfig->DmiAspm                                = SetupVariable->SaDmiAspm;
//[-start-121122-IB05300360-modify]//
//  SaPlatformPolicy->PcieConfig->PegAspm[0]                             = PcdGet8 (PcdPcieConfigPegAspm0);
//  SaPlatformPolicy->PcieConfig->PegAspm[1]                             = PcdGet8 (PcdPcieConfigPegAspm0);
//  SaPlatformPolicy->PcieConfig->PegAspm[2]                             = PcdGet8 (PcdPcieConfigPegAspm0);
//  SaPlatformPolicy->PcieConfig->PegAspmL0s[0]                          = PcdGet8 (PcdPcieConfigPegAspmL0s0);
//  SaPlatformPolicy->PcieConfig->PegAspmL0s[1]                          = PcdGet8 (PcdPcieConfigPegAspmL0s1);
//  SaPlatformPolicy->PcieConfig->PegAspmL0s[2]                          = PcdGet8 (PcdPcieConfigPegAspmL0s2);
//  SaPlatformPolicy->PcieConfig->PegDeEmphasis[0]                       = PcdGet8 (PcdPcieConfigPegDeEmphasis0);
//  SaPlatformPolicy->PcieConfig->PegDeEmphasis[1]                       = PcdGet8 (PcdPcieConfigPegDeEmphasis1);
//  SaPlatformPolicy->PcieConfig->PegDeEmphasis[2]                       = PcdGet8 (PcdPcieConfigPegDeEmphasis2);
  for (Index = 0; Index < SA_PEG_MAX_FUN; Index++) {
    SaPlatformPolicy->PcieConfig->PegAspm[Index]                       = SetupVariable->PegAspm;
    SaPlatformPolicy->PcieConfig->PegAspmL0s[Index]                    = SetupVariable->PegAspmL0s;
    SaPlatformPolicy->PcieConfig->PegDeEmphasis[Index]                 = PcdGet8 (PcdPcieConfigPegDeEmphasis0);
  }
//[-end-121122-IB05300360-modify]//
  SaPlatformPolicy->PcieConfig->DmiExtSync                             = SetupVariable->SaDmiExtSync;
  SaPlatformPolicy->PcieConfig->DmiDeEmphasis                          = PcdGet8 (PcdPcieConfigPegDmiDeEmphasis);
//[-start-120404-IB05300309-modify]//
  SaPlatformPolicy->PcieConfig->DmiIot                                 = PcdGetBool ( PcdPcieConfigPegDmiIot );
  SaPlatformPolicy->PcieConfig->C7Allowed 							               = 0;
  for (Index = 0; Index < SA_PEG_MAX_FUN; Index++) {
    SaPlatformPolicy->PcieConfig->PegPwrOpt[Index].LtrEnable            = 1;
    SaPlatformPolicy->PcieConfig->PegPwrOpt[Index].LtrMaxSnoopLatency   = V_SA_LTR_MAX_SNOOP_LATENCY_VALUE;
    SaPlatformPolicy->PcieConfig->PegPwrOpt[Index].LtrMaxNoSnoopLatency = V_SA_LTR_MAX_NON_SNOOP_LATENCY_VALUE;
    SaPlatformPolicy->PcieConfig->PegPwrOpt[Index].ObffEnable           = 1;
  }
  SaPlatformPolicy->PcieConfig->PcieAspmDevsOverride                   = mPcieAspmDevsOverride;
  SaPlatformPolicy->PcieConfig->PcieLtrDevsOverride                    = mPcieLtrDevsOverride;
//[-end-120404-IB05300309-modify]//

  //
  // Platform Memory Slot Information
  //
  CopyMem (SaPlatformPolicy->MemoryConfig->SpdAddressTable, SpdAddressTable, sizeof (UINT8) * SA_MC_MAX_SOCKETS);
  SaPlatformPolicy->MemoryConfig->ChannelASlotMap                      = PcdGet8 (PcdMemoryConfigChannelASlotMap);
  SaPlatformPolicy->MemoryConfig->ChannelBSlotMap                      = PcdGet8 (PcdMemoryConfigChannelBSlotMap);
//[-start-121003-IB10820132-modify]//
//  if (FeaturePcdGet ( PcdMrcRmt ) ) {
  SaPlatformPolicy->MemoryConfig->RmtBdatEnable                        = PcdGetBool (PcdMemConfigRmtBdatEnable);
//  } else {
//    SaPlatformPolicy->MemoryConfig->RmtBdatEnable                        = 0;
//  }
//[-end-121003-IB10820132-modify]//
  SaPlatformPolicy->MemoryConfig->MrcTimeMeasure                       = PcdGet8 (PcdMemConfigMrcTimeMeasure);
  SaPlatformPolicy->MemoryConfig->MrcFastBoot                          = FALSE;

  //
  // Initialzie Graphics Configuration
  //
  SaPlatformPolicy->IgdConfig->RenderStandby                           = SetupVariable->EnableRc6;
  SaPlatformPolicy->IgdConfig->DeepRenderStandby                       = SetupVariable->DeepRenderStandby;
//[-start-120924-IB06150247-add]//
  Status = gBS->LocateProtocol (
                  &gPlatformGopPolicyProtocolGuid,
                  NULL,
                  (VOID **)&PlatformGopPolicyPtr
                  );
  if (!EFI_ERROR (Status)) {
    PlatformGopPolicyPtr->GetVbtData (&SaPlatformPolicy->IgdConfig->VbtAddress, &SaPlatformPolicy->IgdConfig->Size);
  } else {
    SaPlatformPolicy->IgdConfig->VbtAddress                            = 0x00000000;
    SaPlatformPolicy->IgdConfig->Size                                  = 0;
  }
//[-end-120924-IB06150247-add]//
//[-start-121009-IB03780464-add]//
  SaPlatformPolicy->IgdConfig->CdClk                                   = 1;
  SaPlatformPolicy->IgdConfig->CdClkVar                                = 0;
//[-end-121009-IB03780464-add]//
//[-start-121113-IB06150257-add]//
  SaPlatformPolicy->IgdConfig->PlatformConfig                          = 1;
//[-end-121113-IB06150257-add]//
//[-start-121211-IB06460476-add]//
  SaPlatformPolicy->IgdConfig->IuerStatusVal                           = PcdGet8 (PcdIuerStatusVal);
//[-end-121211-IB06460476-add]//

//[-start-130401-IB05160428-add]//
  SaPlatformPolicy->IgdConfig->FdiLaneReversal                         = 0; // FdiLaneReversal = 1 for Basking Ridge and Walnut Canyon CRB
//[-end-130401-IB05160428-add]//

  //
  // SA Internal Devices and Misc Configuration
  //
  SaPlatformPolicy->MiscConfig->ChapDeviceEnable                       = SetupVariable->ChapDeviceEnable;
  SaPlatformPolicy->MiscConfig->Device4Enable                          = SetupVariable->Device4Enable;
  SaPlatformPolicy->MiscConfig->DefaultSvidSid->SubSystemVendorId      = PcdGet16 (PcdSaDefaultSvidSidSubSystemVendorId);
  SaPlatformPolicy->MiscConfig->DefaultSvidSid->SubSystemId            = PcdGet16 (PcdSaDefaultSvidSidSubSystemId);
  SaPlatformPolicy->MiscConfig->CridEnable                             = SetupVariable->CRIDEnable;
//[-start-121130-IB06150264-modify]//
  SaPlatformPolicy->MiscConfig->AudioEnable                            = SetupVariable->SaAudioEnable;
//[-end-121130-IB06150264-modify]//
  SaPlatformPolicy->MiscConfig->FviReport                              = PcdGet8 (PcdDxeCpuConfigFviReport);
  SaPlatformPolicy->MiscConfig->FviSmbiosType                          = PcdGet8 (PcdMeMiscConfigFviSmbiosType);    // Default SMBIOS Type 221
//[-start-120717-IB10040004-remove]//
//[-start-121211-IB06460476-add]//
  SaPlatformPolicy->MiscConfig->SaHdaVerbTableNum                      = PcdGet8 (PcdSaHdaVerbTableNum);
//[-end-121211-IB06460476-add]//
//[-start-140521-IB05400527-add]//
  SaPlatformPolicy->MiscConfig->EnableAbove4GBMmio                     = SetupVariable->PciMem64DecodeSupport;
//[-end-140521-IB05400527-add]//
//[-start-140219-IB10920078-modify]//
//[-start-120627-IB05330350-add]//
  if (FeaturePcdGet (PcdIntelSwitchableGraphicsSupported)) { 
    ///
    /// Initialize the Switchable Graphics DXE Policies
    ///

    ///
    /// 1 = Load secondary display device VBIOS 
    /// 0 = Do not load
    ///
    SaPlatformPolicy->VbiosConfig->LoadVbios = PcdGetBool (PcdSgVbiosConfigSgDgpuLoadVbios);
    ///
    /// 1 = Execute the secondary display device VBIOS (only if LoadVbios == 1)
    /// 0 = Do no execute
    ///
    SaPlatformPolicy->VbiosConfig->ExecuteVbios = PcdGetBool (PcdSgVbiosConfigSgDgpuExecuteVbioss);
    ///
    /// 1 = secondary display device VBIOS Source is PCI Card
    /// 0 = secondary display device VBIOS Source is FW Volume
    ///
    SaPlatformPolicy->VbiosConfig->VbiosSource = PcdGetBool (PcdSgVbiosConfigSgDgpuVbiosSource);
  }
//[-end-120627-IB05330350-add]//
//[-end-120717-IB10040004-remove]//
//[-end-140219-IB10920078-modify]//
//[-start-120629-IB06460411-add]//
  SaPlatformPolicy->PlatformBoardID                                    = PlatformBoardID;
//[-end-120629-IB06460411-add]//

  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "Install SA Policy Protocol\n"));
//[-start-121220-IB10820206-modify]//
//[-start-130524-IB05160451-modify]//
  Status = OemSvcUpdateDxePlatformSaPolicy (SaPlatformPolicy);
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "DexOemSvcChipsetLib OemSvcUpdateDxePlatformSaPolicy, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
  if (Status != EFI_SUCCESS) {
    //
    // Install SA Policy Protocol.
    //
    Handle = NULL;
    Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &gDxePlatformSaPolicyGuid,
                    EFI_NATIVE_INTERFACE,
                    SaPlatformPolicy
                    );
    ASSERT_EFI_ERROR (Status);
  }
//[-end-121220-IB10820206-modify]//
  //
  // Dump policy
  //
  DumpSAPolicy (SaPlatformPolicy);

//[-start-121012-IB03780464-add]//
  //
  // Create a event to update SYSTEM_AGENT_GLOBAL_NVS_AREA.
  //
  EfiCreateProtocolNotifyEvent (
    &gSaGlobalNvsAreaProtocolGuid,
    TPL_CALLBACK,
    UpdateSaGlobalNvsCallback,
    NULL,
    &Registration
  );
//[-end-121012-IB03780464-add]//

  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "SA Policy Exit\n"));

  return Status;
}

//[-start-121120-IB08050186-add]//
EFI_STATUS
UpdateLidStateToNvs (
  SYSTEM_AGENT_GLOBAL_NVS_AREA_PROTOCOL        *SaGlobalNvsArea
  )
/*++

Routine Description:

  Get Lid state and update to global NVS area.
  
Arguments:

  GlobalNvsAreaProtocol  - Global NVS area protocol

Returns:

  EFI_SUCCESS    - Get Lid state successfully.
  Others         - Get Lid state failed.

--*/
{
//[-start-130809-IB06720232-modify]//
  EFI_STATUS    EcGetLidState;
  BOOLEAN       LidIsOpen;
  
  EcGetLidState = EFI_SUCCESS;
  LidIsOpen     = TRUE;
 
  OemSvcEcGetLidState (&EcGetLidState, &LidIsOpen);
  ASSERT (!EFI_ERROR (EcGetLidState));

  SaGlobalNvsArea->Area->LidState = 1;
  if (!EFI_ERROR (EcGetLidState)) {
    if (!LidIsOpen) {
      //
      // If get lid state form EC successfully and lid is closed.
      //
      SaGlobalNvsArea->Area->LidState = 0;
    }
  } else {
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, "OemSvcEcGetLidState ERROR in SAPolicy! Status is %r.\n", EcGetLidState));
  }

  return EcGetLidState;
}
//[-end-130809-IB06720232-modify]//
//[-end-121120-IB08050186-add]//


VOID
DumpSAPolicy (
  IN      DXE_PLATFORM_SA_POLICY_PROTOCOL        *SaPlatformPolicy
 )
{
  UINTN        Count;

  Count = 0;

  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\n"));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "SaPlatformPolicy (Address : 0x%x)\n", SaPlatformPolicy));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-Revision                        : %x\n", SaPlatformPolicy->Revision));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-Vtd (Address : 0x%x)\n", SaPlatformPolicy->Vtd));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-VtdEnable                    : %x\n", SaPlatformPolicy->Vtd->VtdEnable));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RmrrUsbBaseAddress (Address : 0x%x)\n", SaPlatformPolicy->Vtd->RmrrUsbBaseAddress));
  for (Count = 0; Count < SA_VTD_ENGINE_NUMBER; Count++) {
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-BaseAddress[%2x]              : %x\n", Count, SaPlatformPolicy->Vtd->BaseAddress[Count]));
  }
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-PcieConfig (Address : 0x%x)\n", SaPlatformPolicy->PcieConfig));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-DmiAspm                      : %x\n", SaPlatformPolicy->PcieConfig->DmiAspm));
  for (Count = 0; Count < SA_PEG_MAX_FUN; Count++) {
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PegAspm[%2x]                  : %x\n", Count, SaPlatformPolicy->PcieConfig->PegAspm[Count]));
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PegAspmL0s[%2x]               : %x\n", Count, SaPlatformPolicy->PcieConfig->PegAspmL0s[Count]));
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PegDeEmphasis[%2x]            : %x\n", Count, SaPlatformPolicy->PcieConfig->PegDeEmphasis[Count]));
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PegPwrOpt[%2x]\n", Count));
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-LtrEnable                  : %x\n", SaPlatformPolicy->PcieConfig->PegPwrOpt[Count].LtrEnable));
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-LtrMaxSnoopLatency         : %x\n", SaPlatformPolicy->PcieConfig->PegPwrOpt[Count].LtrMaxSnoopLatency ));
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-LtrMaxNoSnoopLatency       : %x\n", SaPlatformPolicy->PcieConfig->PegPwrOpt[Count].LtrMaxNoSnoopLatency ));
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-ObffEnable                 : %x\n", SaPlatformPolicy->PcieConfig->PegPwrOpt[Count].ObffEnable ));    
  }
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-DmiExtSync                   : %x\n", SaPlatformPolicy->PcieConfig->DmiExtSync));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-DmiDeEmphasis                : %x\n", SaPlatformPolicy->PcieConfig->DmiDeEmphasis));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-DmiIot                       : %x\n", SaPlatformPolicy->PcieConfig->DmiIot));
//[-start-120404-IB05300309-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PcieAspmDevsOverride         : %x\n", SaPlatformPolicy->PcieConfig->PcieAspmDevsOverride));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PcieLtrDevsOverride          : %x\n", SaPlatformPolicy->PcieConfig->PcieLtrDevsOverride));
//[-end-120404-IB05300309-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-MemoryConfig (Address : 0x%x)\n", SaPlatformPolicy->MemoryConfig));
  for (Count = 0; Count < SA_MC_MAX_SOCKETS; Count++) {
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-SpdAddressTable[%2x]          : %x\n", Count, SaPlatformPolicy->MemoryConfig->SpdAddressTable[Count]));
  }
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ChannelASlotMap              : %x\n", SaPlatformPolicy->MemoryConfig->ChannelASlotMap));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ChannelBSlotMap              : %x\n", SaPlatformPolicy->MemoryConfig->ChannelBSlotMap));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RmtBdatEnable                : %x\n", SaPlatformPolicy->MemoryConfig->RmtBdatEnable));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-MrcTimeMeasure               : %x\n", SaPlatformPolicy->MemoryConfig->MrcTimeMeasure));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-MrcFastBoot                  : %x\n", SaPlatformPolicy->MemoryConfig->MrcFastBoot));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-IgdConfig (Address : 0x%x)\n", SaPlatformPolicy->IgdConfig));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RenderStandby                : %x\n", SaPlatformPolicy->IgdConfig->RenderStandby));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-DeepRenderStandby            : %x\n", SaPlatformPolicy->IgdConfig->DeepRenderStandby));
//[-start-120925-IB06150247-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-VbtAddress                   : %x\n", SaPlatformPolicy->IgdConfig->VbtAddress));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-Size                         : %x\n", SaPlatformPolicy->IgdConfig->Size));
//[-end-120925-IB06150247-add]//
//[-start-121009-IB03780464-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-CdClk                        : %x\n", SaPlatformPolicy->IgdConfig->CdClk));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-CdClkVar                     : %x\n", SaPlatformPolicy->IgdConfig->CdClkVar));
//[-end-121009-IB03780464-add]//
//[-start-121211-IB06460476-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PlatformConfig               : %x\n", SaPlatformPolicy->IgdConfig->PlatformConfig));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-IuerStatusVal                : %x\n", SaPlatformPolicy->IgdConfig->IuerStatusVal));
//[-end-121211-IB06460476-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-MiscConfig (Address : 0x%x)\n", SaPlatformPolicy->MiscConfig));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ChapDeviceEnable             : %x\n", SaPlatformPolicy->MiscConfig->ChapDeviceEnable));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-Device4Enable                : %x\n", SaPlatformPolicy->MiscConfig->Device4Enable));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-DefaultSvidSid (Address : 0x%x)\n", SaPlatformPolicy->MiscConfig->DefaultSvidSid));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-SubSystemVendorId         : %x\n", SaPlatformPolicy->MiscConfig->DefaultSvidSid->SubSystemVendorId));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-SubSystemId               : %x\n", SaPlatformPolicy->MiscConfig->DefaultSvidSid->SubSystemId));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-CridEnable                   : %x\n", SaPlatformPolicy->MiscConfig->CridEnable));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-AudioEnable                  : %x\n", SaPlatformPolicy->MiscConfig->AudioEnable));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-FviReport                    : %x\n", SaPlatformPolicy->MiscConfig->FviReport));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-FviSmbiosType                : %x\n", SaPlatformPolicy->MiscConfig->FviSmbiosType));
//[-start-120717-IB10040004-remove]//
//[-start-121211-IB06460476-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-SaHdaVerbTableNum                : %x\n", SaPlatformPolicy->MiscConfig->SaHdaVerbTableNum));
//[-end-121211-IB06460476-add]//
//[-start-140521-IB05400527-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-EnableAbove4GBMmio               : %x\n", SaPlatformPolicy->MiscConfig->EnableAbove4GBMmio));
//[-end-140521-IB05400527-add]//
////[-start-120627-IB05330350-add]//
//#ifdef SWITCHABLE_GRAPHICS_SUPPORT
//  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-VbiosConfig (Address : 0x%x)\n", SaPlatformPolicy->VbiosConfig));
//  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-LoadVbios                    : %x\n", SaPlatformPolicy->VbiosConfig->LoadVbios));
//  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ExecuteVbios                 : %x\n", SaPlatformPolicy->VbiosConfig->ExecuteVbios));
//  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-VbiosSource                  : %x\n", SaPlatformPolicy->VbiosConfig->VbiosSource));  
//#endif
////[-end-120627-IB05330350-add]//
//[-end-120717-IB10040004-remove]//
//[-start-120629-IB06460411-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-PlatformBoardID             : %x\n", SaPlatformPolicy->PlatformBoardID));
//[-end-120629-IB06460411-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\n"));

  return;
}

//[-start-121012-IB03780464-add]//
/**
 This callback function is to update SYSTEM_AGENT_GLOBAL_NVS_AREA when SA driver finish SA GNVS initialization.

 @param[in]         Event               Event whose notification function is being invoked.
 @param[in]         Context             Pointer to the notification function's context.

 @retval            None.
*/
VOID
EFIAPI
UpdateSaGlobalNvsCallback (
  IN  EFI_EVENT                Event,
  IN  VOID                     *Context
  )
{
  EFI_STATUS                                   Status;
  EFI_SETUP_UTILITY_PROTOCOL                   *SetupUtility;
  CHIPSET_CONFIGURATION                         *Setup;
  SYSTEM_AGENT_GLOBAL_NVS_AREA_PROTOCOL        *SaGlobalNvsArea;
  MEM_INFO_PROTOCOL                            *MemoryInfo;
  UINT8                                        UnitDVMT;
  UINT8                                        MaxDVMT;

  Status = gBS->LocateProtocol (&gSaGlobalNvsAreaProtocolGuid, NULL, (VOID **)&SaGlobalNvsArea);
  if (EFI_ERROR (Status)) {
    return;
  }

  Status = gBS->CloseEvent (Event);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility);
  ASSERT_EFI_ERROR (Status);
  Setup = (CHIPSET_CONFIGURATION *) (SetupUtility->SetupNvData);

  Status = gBS->LocateProtocol (&gMemInfoProtocolGuid, NULL, (VOID **)&MemoryInfo);
  ASSERT_EFI_ERROR ( Status );

  //
  // Follow DVMT 5.0 White Paper to limit the Maximum DVMT
  //
  UnitDVMT = 128; // 128 MB as the unit
  MaxDVMT  = 128 / UnitDVMT;
  if (MemoryInfo->MemInfoData.memSize >= 1024) MaxDVMT = (UINT8) ( 512 / UnitDVMT);
  if (MemoryInfo->MemInfoData.memSize >= 1536) MaxDVMT = (UINT8) ( 768 / UnitDVMT);
  if (MemoryInfo->MemInfoData.memSize >= 2048) MaxDVMT = (UINT8) (1024 / UnitDVMT);

  //
  // Update SA GNVS here
  //
  SaGlobalNvsArea->Area->IgdDvmtMemSize = ((Setup->DvmtTotalGfxMemSize == 0xFF) || (Setup->DvmtTotalGfxMemSize > MaxDVMT)) ? MaxDVMT : Setup->DvmtTotalGfxMemSize; // Real Size : IgdDvmtMemSize << 17
  SaGlobalNvsArea->Area->IgdState = 1; // PRIMARY_DISPLAY

  SaGlobalNvsArea->Area->DeviceId1             = 0x00000100;           // Device ID 1
  SaGlobalNvsArea->Area->DeviceId2             = 0x80000410;           // Device ID 2
  SaGlobalNvsArea->Area->DeviceId3             = 0x80000300;           // Device ID 3
  SaGlobalNvsArea->Area->DeviceId4             = 0x00000301;           // Device ID 4
  SaGlobalNvsArea->Area->DeviceId5             = 0x05;                 // Device ID 5
  SaGlobalNvsArea->Area->NumberOfValidDeviceId = 4;                    // Number of Valid Device IDs
  SaGlobalNvsArea->Area->CurrentDeviceList     = 0x0F;                 // Default setting
  SaGlobalNvsArea->Area->PreviousDeviceList    = 0x0F;
  SaGlobalNvsArea->Area->XPcieCfgBaseAddress   = (UINT32) (MmPciAddress (0, 0, 0, 0, 0x0));

//[-start-121120-IB08050186-add]//
  Status = UpdateLidStateToNvs (SaGlobalNvsArea);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, "UpdateLidStateToNvs Error.\n"));
  }
//[-end-121120-IB08050186-add]//

//[-start-140521-IB05400527-add]//
  UpdateGlobalNvsCallback (NULL, NULL);
//[-end-140521-IB05400527-add]//
  
  return;
}
//[-end-121012-IB03780464-add]//


//[-start-140521-IB05400527-add]//
/**
 This callback function is to update EFI_GLOBAL_NVS_AREA when GNVS initialization.

 @param[in]         Event               Event whose notification function is being invoked.
 @param[in]         Context             Pointer to the notification function's context.

 @retval            None.
*/
VOID
EFIAPI
UpdateGlobalNvsCallback (
  IN  EFI_EVENT                Event,
  IN  VOID                     *Context
  )
{
  EFI_STATUS                      Status;
  EFI_GLOBAL_NVS_AREA             *GlobalNvsArea;
  EFI_GLOBAL_NVS_AREA_PROTOCOL    *GlobalNvsAreaProtocol;
  UINT64                          Touud;


  
  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID**)&GlobalNvsAreaProtocol);
  if (EFI_ERROR (Status)) {
    //
    // GlobalNvsAreaProtocol is not ready, use protocol notify. 
    //
    EfiNamedEventListen (
      &gEfiGlobalNvsAreaProtocolGuid,
      TPL_CALLBACK,
      UpdateGlobalNvsCallback,
      NULL,
      NULL
    );

    return;
  }

  GlobalNvsArea = GlobalNvsAreaProtocol->Area;

  //
  // Always provide available above 4GB MMIO resource
  // BIOS will only use it when policy EnableAbove4GBMmio enabled
  //
  Touud = McD0PciCfg64 (R_SA_TOUUD) & B_SA_TOUUD_TOUUD_MASK;
  if (Touud > MEM_EQU_4GB) {
    GlobalNvsArea->MmioMem64Base = Touud;
  } else {
    GlobalNvsArea->MmioMem64Base = MEM_EQU_4GB;
  }
  GlobalNvsArea->MmioMem64Length = PcdGet64 (PcdMmioMem64Length);
  GlobalNvsArea->MmioMem64Limit = GlobalNvsArea->MmioMem64Base + GlobalNvsArea->MmioMem64Length - 1;

  //
  // Pass 64 bit MMIO support to ASL
  //
  if (FeaturePcdGet (Pcd64BitAmlSupported)) {
    GlobalNvsArea->Mmio64Support = 1;
  }

  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }
  
}
//[-end-140521-IB05400527-add]//
