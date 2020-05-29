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

#include <IccPolicy.h>

EFI_STATUS
EFIAPI
ICCPolicyEntry (
  IN      EFI_HANDLE              ImageHandle,
  IN      EFI_SYSTEM_TABLE        *SystemTable
  )
{
  EFI_STATUS                              Status;
  EFI_HANDLE                              Handle;
  DXE_PLATFORM_ICC_POLICY_PROTOCOL        *ICCPlatformPolicy;
  EFI_SETUP_UTILITY_PROTOCOL              *SetupUtility;
  CHIPSET_CONFIGURATION                    *SetupVariable;
  UINT32                                  SRC0;
  UINT32                                  SRC1;
  UINT32                                  SRC2;
  UINT32                                  SRC3;
  UINT32                                  SRC4;
  UINT32                                  SRC5;
  UINT32                                  SRC6;
  UINT32                                  SRC7;
  BOOLEAN                                 HotPlug;
  BOOLEAN                                 FunctionDisable;
  BOOLEAN                                 Presence;

  Handle            = NULL;
  ICCPlatformPolicy = NULL;
  SetupUtility      = NULL;
  SetupVariable     = NULL;
  SRC0              = 0;
  SRC1              = 0;
  SRC2              = 0;
  SRC3              = 0;
  SRC4              = 0;
  SRC5              = 0;
  SRC6              = 0;
  SRC7              = 0;
  HotPlug           = FALSE;
  FunctionDisable   = FALSE;
  Presence          = FALSE;

  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\nICC Policy Entry\n"));

  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility);
  ASSERT_EFI_ERROR (Status);
  SetupVariable = (CHIPSET_CONFIGURATION *)(SetupUtility->SetupNvData);

  //
  // Allocate and set ICC Policy structure to known value
  //
  ICCPlatformPolicy = NULL;
  ICCPlatformPolicy = AllocateZeroPool (sizeof (DXE_PLATFORM_ICC_POLICY_PROTOCOL));
  ASSERT (ICCPlatformPolicy != NULL);
//[-start-130104-IB10870064-add]//
  if (ICCPlatformPolicy == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

  ICCPlatformPolicy->FunctionEnabling = NULL;
  ICCPlatformPolicy->FunctionEnabling = AllocateZeroPool (sizeof (ICC_FUNCTION_ENABLING));
  ASSERT (ICCPlatformPolicy->FunctionEnabling != NULL);
//[-start-130104-IB10870064-add]//
  if (ICCPlatformPolicy->FunctionEnabling == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

  ICCPlatformPolicy->ClockEnabling = NULL;
  ICCPlatformPolicy->ClockEnabling = AllocateZeroPool (sizeof (ICC_CLOCK_ENABLING));
  ASSERT (ICCPlatformPolicy->ClockEnabling != NULL);
//[-start-130104-IB10870064-add]//
  if (ICCPlatformPolicy->ClockEnabling == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

  ICCPlatformPolicy->ClockEnablesMask = NULL;
  ICCPlatformPolicy->ClockEnablesMask = AllocateZeroPool (sizeof (ICC_CLOCK_ENABLES_MASK));
  ASSERT (ICCPlatformPolicy->ClockEnablesMask != NULL);
//[-start-130104-IB10870064-add]//
  if (ICCPlatformPolicy->ClockEnablesMask == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

//[-start-130104-IB10870064-remove]//
//  if (
//    ( ICCPlatformPolicy                   == NULL ) ||
//    ( ICCPlatformPolicy->FunctionEnabling == NULL ) ||
//    ( ICCPlatformPolicy->ClockEnabling    == NULL ) ||
//    ( ICCPlatformPolicy->ClockEnablesMask == NULL )
//    ) {
//    return EFI_OUT_OF_RESOURCES;
//  }
//[-end-130104-IB10870064-remove]//
  if (EFI_ERROR (Status)) {
    return Status;
  }

//[-start-120315-IB06460374-add]//
  //
  // ICC default profile setting: OEM could change ICC default profile setting via this policy. This policy is also workable when user try to load default in SCU.
  //
  ICCPlatformPolicy->IccDefaultProfile = PcdGet8 (PcdIccPlatformPolicyIccDefaultProfile);
//[-end-120315-IB06460374-add]//

//[-start-121005-IB05330380-modify]//
  //
  // No schematic for SharkBay Flathead Creek EV FABI.
  // Sync. to Intel demo 75, Set SRC0,1,3,4 always enable.
  // SRC0 : PCIEx1 Slot 5 : PCIE Port 6
  // SRC1 : MiniPCIE/WLAN : PCIE Port 1
  // SRC2 : GBE LAN       : PCIE Port 2
  // SRC3 : Unknown
  // SRC4 : Unknown
  // SRC5 : PCIEx4        : PCIE Port 4
  // SRC6 : PCIEx1 Slot 4 : PCIE Port 7
  // SRC7 : PCIEx1 Slot 6 : PCIE Port 5
  //

  //
  // ICCPlatformPolicy->ClockEnabling->Fields.SRC0
  //
  HotPlug         = SetupVariable->PcieRootPortHotPlug6 ? TRUE : FALSE;
  FunctionDisable = ( PchMmRcrb32 ( R_PCH_RCRB_FUNC_DIS ) & B_PCH_RCRB_FUNC_DIS_PCI_EX_PORT7 ) ? TRUE : FALSE;
  Presence        = ( !FunctionDisable && ( MmioRead16 ( MmPciAddress ( 0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_7, R_PCH_PCIE_SLSTS )) & B_PCH_PCIE_SLSTS_PDS ) ) ? TRUE : FALSE;
  SRC0            = ( HotPlug || Presence ) ? ICC_ENABLE : ICC_DISABLE;

  //
  // ICCPlatformPolicy->ClockEnabling->Fields.SRC1
  //
  HotPlug         = SetupVariable->PcieRootPortHotPlug1 ? TRUE : FALSE;
  FunctionDisable = ( PchMmRcrb32 ( R_PCH_RCRB_FUNC_DIS ) & B_PCH_RCRB_FUNC_DIS_PCI_EX_PORT2 ) ? TRUE : FALSE;
  Presence        = ( !FunctionDisable && ( MmioRead16 ( MmPciAddress ( 0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_2, R_PCH_PCIE_SLSTS )) & B_PCH_PCIE_SLSTS_PDS ) ) ? TRUE : FALSE;
  SRC1            = ( HotPlug || Presence ) ? ICC_ENABLE : ICC_DISABLE;

  //
  // ICCPlatformPolicy->ClockEnabling->Fields.SRC2
  //
  SRC2 = SetupVariable->Lan ? ICC_ENABLE : ICC_DISABLE;

  //
  // ICCPlatformPolicy->ClockEnabling->Fields.SRC5
  //
  HotPlug         = SetupVariable->PcieRootPortHotPlug4 ? TRUE : FALSE;
  FunctionDisable = ( PchMmRcrb32 ( R_PCH_RCRB_FUNC_DIS ) & B_PCH_RCRB_FUNC_DIS_PCI_EX_PORT5 ) ? TRUE : FALSE;
  Presence        = ( !FunctionDisable && ( MmioRead16 ( MmPciAddress ( 0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_5, R_PCH_PCIE_SLSTS )) & B_PCH_PCIE_SLSTS_PDS ) ) ? TRUE : FALSE;
  SRC5            = ( HotPlug || Presence ) ? ICC_ENABLE : ICC_DISABLE;

  //
  // ICCPlatformPolicy->ClockEnabling->Fields.SRC6
  //
  HotPlug         = SetupVariable->PcieRootPortHotPlug7 ? TRUE : FALSE;
  FunctionDisable = ( PchMmRcrb32 ( R_PCH_RCRB_FUNC_DIS ) & B_PCH_RCRB_FUNC_DIS_PCI_EX_PORT8 ) ? TRUE : FALSE;
  Presence        = ( !FunctionDisable && ( MmioRead16 ( MmPciAddress ( 0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8, R_PCH_PCIE_SLSTS )) & B_PCH_PCIE_SLSTS_PDS ) ) ? TRUE : FALSE;
  SRC6            = ( HotPlug || Presence ) ? ICC_ENABLE : ICC_DISABLE;

  //
  // ICCPlatformPolicy->ClockEnabling->Fields.SRC7
  //
  HotPlug         = SetupVariable->PcieRootPortHotPlug5 ? TRUE : FALSE;
  FunctionDisable = ( PchMmRcrb32 ( R_PCH_RCRB_FUNC_DIS ) & B_PCH_RCRB_FUNC_DIS_PCI_EX_PORT6 ) ? TRUE : FALSE;
  Presence        = ( !FunctionDisable && ( MmioRead16 ( MmPciAddress ( 0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_6, R_PCH_PCIE_SLSTS )) & B_PCH_PCIE_SLSTS_PDS ) ) ? TRUE : FALSE;
  SRC7            = ( HotPlug || Presence ) ? ICC_ENABLE : ICC_DISABLE;

  //
  // Configure ICC Function Enabling
  //
  ICCPlatformPolicy->FunctionEnabling->FCIM              = PcdGet32 (PcdIccPlatformPolicyFunctionEnablingFcim);

  //
  // Configure Clock Enables
  //
  ICCPlatformPolicy->ClockEnabling->Fields.Flex0         = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_ENABLE;
  ICCPlatformPolicy->ClockEnabling->Fields.Flex1         = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_ENABLE;
  ICCPlatformPolicy->ClockEnabling->Fields.Flex2         = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_ENABLE;
  ICCPlatformPolicy->ClockEnabling->Fields.Flex3         = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_ENABLE;
  ICCPlatformPolicy->ClockEnabling->Fields.Reserved1     = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_DISABLE;
  ICCPlatformPolicy->ClockEnabling->Fields.PCI_Clock0    = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_ENABLE;
  ICCPlatformPolicy->ClockEnabling->Fields.PCI_Clock1    = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_ENABLE;
  ICCPlatformPolicy->ClockEnabling->Fields.PCI_Clock2    = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_ENABLE;
  ICCPlatformPolicy->ClockEnabling->Fields.PCI_Clock3    = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_ENABLE;
  ICCPlatformPolicy->ClockEnabling->Fields.PCI_Clock4    = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_ENABLE;
  ICCPlatformPolicy->ClockEnabling->Fields.Reserved2     = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_DISABLE;

  ICCPlatformPolicy->ClockEnabling->Fields.SRC0          = ( !SetupVariable->IccUnusedPci ) ? ICC_ENABLE : ICC_ENABLE;
  ICCPlatformPolicy->ClockEnabling->Fields.SRC1          = ( !SetupVariable->IccUnusedPci ) ? ICC_ENABLE : ICC_ENABLE;
  ICCPlatformPolicy->ClockEnabling->Fields.SRC2          = ( !SetupVariable->IccUnusedPci ) ? ICC_ENABLE : SRC2;
  ICCPlatformPolicy->ClockEnabling->Fields.SRC3          = ( !SetupVariable->IccUnusedPci ) ? ICC_ENABLE : ICC_ENABLE;
  ICCPlatformPolicy->ClockEnabling->Fields.SRC4          = ( !SetupVariable->IccUnusedPci ) ? ICC_ENABLE : ICC_ENABLE;
  ICCPlatformPolicy->ClockEnabling->Fields.SRC5          = ( !SetupVariable->IccUnusedPci ) ? ICC_ENABLE : SRC5;
  ICCPlatformPolicy->ClockEnabling->Fields.SRC6          = ( !SetupVariable->IccUnusedPci ) ? ICC_ENABLE : SRC6;
  ICCPlatformPolicy->ClockEnabling->Fields.SRC7          = ( !SetupVariable->IccUnusedPci ) ? ICC_ENABLE : SRC7;
  ICCPlatformPolicy->ClockEnabling->Fields.CSI_SRC8      = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_ENABLE;
  ICCPlatformPolicy->ClockEnabling->Fields.CSI_DP        = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_ENABLE;
  ICCPlatformPolicy->ClockEnabling->Fields.PEG_A         = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_ENABLE;
  ICCPlatformPolicy->ClockEnabling->Fields.PEG_B         = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_ENABLE;
  ICCPlatformPolicy->ClockEnabling->Fields.DMI           = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_ENABLE;
  ICCPlatformPolicy->ClockEnabling->Fields.Reserved3     = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_DISABLE;

  //
  // Configure Clock Enables Mask
  //
  ICCPlatformPolicy->ClockEnablesMask->Fields.Flex0      = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_DISABLE;
  ICCPlatformPolicy->ClockEnablesMask->Fields.Flex1      = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_DISABLE;
  ICCPlatformPolicy->ClockEnablesMask->Fields.Flex2      = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_DISABLE;
  ICCPlatformPolicy->ClockEnablesMask->Fields.Flex3      = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_DISABLE;
  ICCPlatformPolicy->ClockEnablesMask->Fields.Reserved1  = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_DISABLE;
  ICCPlatformPolicy->ClockEnablesMask->Fields.PCI_Clock0 = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_DISABLE;
  ICCPlatformPolicy->ClockEnablesMask->Fields.PCI_Clock1 = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_DISABLE;
  ICCPlatformPolicy->ClockEnablesMask->Fields.PCI_Clock2 = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_DISABLE;
  ICCPlatformPolicy->ClockEnablesMask->Fields.PCI_Clock3 = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_DISABLE;
  ICCPlatformPolicy->ClockEnablesMask->Fields.PCI_Clock4 = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_DISABLE;
  ICCPlatformPolicy->ClockEnablesMask->Fields.Reserved2  = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_DISABLE;
  ICCPlatformPolicy->ClockEnablesMask->Fields.SRC0       = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_ENABLE;
  ICCPlatformPolicy->ClockEnablesMask->Fields.SRC1       = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_ENABLE;
  ICCPlatformPolicy->ClockEnablesMask->Fields.SRC2       = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_ENABLE;
  ICCPlatformPolicy->ClockEnablesMask->Fields.SRC3       = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_ENABLE;
  ICCPlatformPolicy->ClockEnablesMask->Fields.SRC4       = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_ENABLE;
  ICCPlatformPolicy->ClockEnablesMask->Fields.SRC5       = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_ENABLE;
  ICCPlatformPolicy->ClockEnablesMask->Fields.SRC6       = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_ENABLE;
  ICCPlatformPolicy->ClockEnablesMask->Fields.SRC7       = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_ENABLE;
  ICCPlatformPolicy->ClockEnablesMask->Fields.CSI_SRC8   = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_DISABLE;
  ICCPlatformPolicy->ClockEnablesMask->Fields.CSI_DP     = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_DISABLE;
  ICCPlatformPolicy->ClockEnablesMask->Fields.PEG_A      = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_DISABLE;
  ICCPlatformPolicy->ClockEnablesMask->Fields.PEG_B      = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_DISABLE;
  ICCPlatformPolicy->ClockEnablesMask->Fields.DMI        = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_DISABLE;
  ICCPlatformPolicy->ClockEnablesMask->Fields.Reserved3  = (!SetupVariable->IccUnusedPci) ? ICC_ENABLE : ICC_DISABLE;

  //
  // ICC Set Backup Profile function
  //
  ICCPlatformPolicy->SetBackupProfile                    = ICCSetBackupProfile;

  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "Install ICC Policy Protocol\n"));
//[-start-121220-IB10820206-modify]//
//[-start-130524-IB05160451-modify]//
  Status = OemSvcUpdateDxePlatformIccPolicy (ICCPlatformPolicy);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "DexOemSvcChipsetLib OemSvcUpdateDxePlatformIccPolicy, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
  if (Status != EFI_SUCCESS) {
    //
    // Install ICC Policy Protocol.
    //
    Handle = NULL;
    Status = gBS->InstallProtocolInterface (
               &Handle,
               &gDxePlatformICCPolicyGuid,
               EFI_NATIVE_INTERFACE,
               ICCPlatformPolicy
               );
    ASSERT_EFI_ERROR (Status);
  }
//[-end-121220-IB10820206-modify]//

  //
  // Dump policy
  //
  DumpICCPolicy (ICCPlatformPolicy);

  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "ICC Policy Exit\n"));

  return Status;
}

EFI_STATUS
EFIAPI
ICCSetBackupProfile (
     OUT  UINT8        *ICCBackupProfile
  )
{
  EFI_STATUS                        Status;
  EFI_SETUP_UTILITY_PROTOCOL        *SetupUtility;
  CHIPSET_CONFIGURATION              *SetupVariable;

  SetupUtility      = NULL;
  SetupVariable     = NULL;

  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility);
  ASSERT_EFI_ERROR (Status);
  SetupVariable = (CHIPSET_CONFIGURATION *)(SetupUtility->SetupNvData);

  *ICCBackupProfile = SetupVariable->IccBackupProfile;
  if (ReadExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, SetupNVFlag) == NV_NOT_FOUND) {
    //
    // If NV Data is unavailable, set ICC Backup Profile to defaule data.
    //
    *ICCBackupProfile = SetupVariable->IccProfile;
  }

  return Status;
}

VOID
DumpICCPolicy (
  IN      DXE_PLATFORM_ICC_POLICY_PROTOCOL        *ICCPlatformPolicy
  )
{
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\n"));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "ICCPlatformPolicy ( Address : 0x%x )\n", ICCPlatformPolicy));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-FunctionEnabling ( Address : 0x%x )\n", ICCPlatformPolicy->FunctionEnabling));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-FCIM          : %x\n", ICCPlatformPolicy->FunctionEnabling->FCIM));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-Reserved      : %x\n", ICCPlatformPolicy->FunctionEnabling->Reserved));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-ClockEnabling ( Address : 0x%x )\n", ICCPlatformPolicy->ClockEnabling));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-Dword         : %x\n", ICCPlatformPolicy->ClockEnabling->Dword));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-Fields ( Address : 0x%x )\n", &ICCPlatformPolicy->ClockEnabling->Fields));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |     |-Flex0      : %x\n", ICCPlatformPolicy->ClockEnabling->Fields.Flex0));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |     |-Flex1      : %x\n", ICCPlatformPolicy->ClockEnabling->Fields.Flex1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |     |-Flex2      : %x\n", ICCPlatformPolicy->ClockEnabling->Fields.Flex2));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |     |-Flex3      : %x\n", ICCPlatformPolicy->ClockEnabling->Fields.Flex3));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |     |-Reserved1  : %x\n", ICCPlatformPolicy->ClockEnabling->Fields.Reserved1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |     |-PCI_Clock0 : %x\n", ICCPlatformPolicy->ClockEnabling->Fields.PCI_Clock0));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |     |-PCI_Clock1 : %x\n", ICCPlatformPolicy->ClockEnabling->Fields.PCI_Clock1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |     |-PCI_Clock2 : %x\n", ICCPlatformPolicy->ClockEnabling->Fields.PCI_Clock2));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |     |-PCI_Clock3 : %x\n", ICCPlatformPolicy->ClockEnabling->Fields.PCI_Clock3));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |     |-PCI_Clock4 : %x\n", ICCPlatformPolicy->ClockEnabling->Fields.PCI_Clock4));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |     |-Reserved2  : %x\n", ICCPlatformPolicy->ClockEnabling->Fields.Reserved2));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |     |-SRC0       : %x\n", ICCPlatformPolicy->ClockEnabling->Fields.SRC0));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |     |-SRC1       : %x\n", ICCPlatformPolicy->ClockEnabling->Fields.SRC1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |     |-SRC2       : %x\n", ICCPlatformPolicy->ClockEnabling->Fields.SRC2));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |     |-SRC3       : %x\n", ICCPlatformPolicy->ClockEnabling->Fields.SRC3));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |     |-SRC4       : %x\n", ICCPlatformPolicy->ClockEnabling->Fields.SRC4));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |     |-SRC5       : %x\n", ICCPlatformPolicy->ClockEnabling->Fields.SRC5));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |     |-SRC6       : %x\n", ICCPlatformPolicy->ClockEnabling->Fields.SRC6));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |     |-SRC7       : %x\n", ICCPlatformPolicy->ClockEnabling->Fields.SRC7));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |     |-CSI_SRC8   : %x\n", ICCPlatformPolicy->ClockEnabling->Fields.CSI_SRC8));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |     |-CSI_DP     : %x\n", ICCPlatformPolicy->ClockEnabling->Fields.CSI_DP));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |     |-PEG_A      : %x\n", ICCPlatformPolicy->ClockEnabling->Fields.PEG_A));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |     |-PEG_B      : %x\n", ICCPlatformPolicy->ClockEnabling->Fields.PEG_B));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |     |-DMI        : %x\n", ICCPlatformPolicy->ClockEnabling->Fields.DMI));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |     |-Reserved3  : %x\n", ICCPlatformPolicy->ClockEnabling->Fields.Reserved3));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-ClockEnablesMask ( Address : 0x%x )\n", ICCPlatformPolicy->ClockEnablesMask));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-Dword         : %x\n", ICCPlatformPolicy->ClockEnablesMask->Dword));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-Fields ( Address : 0x%x )\n", &ICCPlatformPolicy->ClockEnablesMask->Fields));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |    |-Flex0       : %x\n", ICCPlatformPolicy->ClockEnablesMask->Fields.Flex0));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |    |-Flex1       : %x\n", ICCPlatformPolicy->ClockEnablesMask->Fields.Flex1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |    |-Flex2       : %x\n", ICCPlatformPolicy->ClockEnablesMask->Fields.Flex2));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |    |-Flex3       : %x\n", ICCPlatformPolicy->ClockEnablesMask->Fields.Flex3));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |    |-Reserved1   : %x\n", ICCPlatformPolicy->ClockEnablesMask->Fields.Reserved1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |    |-PCI_Clock0  : %x\n", ICCPlatformPolicy->ClockEnablesMask->Fields.PCI_Clock0));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |    |-PCI_Clock1  : %x\n", ICCPlatformPolicy->ClockEnablesMask->Fields.PCI_Clock1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |    |-PCI_Clock2  : %x\n", ICCPlatformPolicy->ClockEnablesMask->Fields.PCI_Clock2));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |    |-PCI_Clock3  : %x\n", ICCPlatformPolicy->ClockEnablesMask->Fields.PCI_Clock3));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |    |-PCI_Clock4  : %x\n", ICCPlatformPolicy->ClockEnablesMask->Fields.PCI_Clock4));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |    |-Reserved2   : %x\n", ICCPlatformPolicy->ClockEnablesMask->Fields.Reserved2));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |    |-SRC0        : %x\n", ICCPlatformPolicy->ClockEnablesMask->Fields.SRC0));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |    |-SRC1        : %x\n", ICCPlatformPolicy->ClockEnablesMask->Fields.SRC1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |    |-SRC2        : %x\n", ICCPlatformPolicy->ClockEnablesMask->Fields.SRC2));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |    |-SRC3        : %x\n", ICCPlatformPolicy->ClockEnablesMask->Fields.SRC3));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |    |-SRC4        : %x\n", ICCPlatformPolicy->ClockEnablesMask->Fields.SRC4));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |    |-SRC5        : %x\n", ICCPlatformPolicy->ClockEnablesMask->Fields.SRC5));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |    |-SRC6        : %x\n", ICCPlatformPolicy->ClockEnablesMask->Fields.SRC6));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |    |-SRC7        : %x\n", ICCPlatformPolicy->ClockEnablesMask->Fields.SRC7));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |    |-CSI_SRC8    : %x\n", ICCPlatformPolicy->ClockEnablesMask->Fields.CSI_SRC8));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |    |-CSI_DP      : %x\n", ICCPlatformPolicy->ClockEnablesMask->Fields.CSI_DP));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |    |-PEG_A       : %x\n", ICCPlatformPolicy->ClockEnablesMask->Fields.PEG_A));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |    |-PEG_B       : %x\n", ICCPlatformPolicy->ClockEnablesMask->Fields.PEG_B));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |    |-DMI         : %x\n", ICCPlatformPolicy->ClockEnablesMask->Fields.DMI));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |    |-Reserved3   : %x\n", ICCPlatformPolicy->ClockEnablesMask->Fields.Reserved3));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-SetBackupProfile ( Address : 0x%x )\n", ICCPlatformPolicy->SetBackupProfile));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\n" ) );

  return;
}
