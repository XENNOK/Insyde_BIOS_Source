/** @file
  This file contains functions that initializes PCI Express Root Ports of PCH.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 1999 - 2014 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

**/
#include <PchInit.h>

#ifdef TRAD_FLAG
UINT32 PchHPcieHsioAddr[] = {
  0xE9002E40,
  0xE9002C40,
  0xE9002A40,
  0xE9002840,
  0xE9002640,
  0xE9002440,
  0xE9002240,
  0xE9002040,
  0xEA002040,
  0xEA002240
};
#endif // TRAD_FLAG

#ifdef ULT_FLAG
UINT32 PchLpPcieHsioAddr[] = {
  0xE9002440,
  0xE9002640,
  0xE9000840,
  0xE9000A40,
  0xE9000C40,
  0xE9000E40,
  0xE9001040,
  0xE9001240,
  0xEA002040,
  0xEA002240,
  0xEA002440,
  0xEA002640
};
#endif // ULT_FLAG

/**
  Set an Init Root Port Downstream devices S3 dispatch item, this function may assert if any error happend

  @param[in] RootPortBus          Pci Bus Number of the root port
  @param[in] RootPortDevice       Pci Device Number of the root port
  @param[in] RootPortFunc         Pci Function Number of the root port
  @param[in] TempBusNumberMin     Minimal temp bus number that can be assigned to the root port (as secondary
                                  bus number) and its down stream switches
  @param[in] TempBusNumberMax     Maximal temp bus number that can be assigned to the root port (as subordinate
                                  bus number) and its down stream switches

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
SetInitRootPortDownstreamS3Item (
  IN UINT8                          RootPortBus,
  IN UINT8                          RootPortDevice,
  IN UINT8                          RootPortFunc,
  IN UINT8                          TempBusNumberMin,
  IN UINT8                          TempBusNumberMax
  )
{
  EFI_STATUS                                                  Status;
#ifdef EFI_S3_RESUME
  STATIC EFI_PCH_S3_SUPPORT_PROTOCOL                          *PchS3Support;
  STATIC EFI_PCH_S3_PARAMETER_INIT_PCIE_ROOT_PORT_DOWNSTREAM  S3ParameterRootPortDownstream;
  STATIC EFI_PCH_S3_DISPATCH_ITEM                             S3DispatchItem = {
    PchS3ItemTypeInitPcieRootPortDownstream,
    &S3ParameterRootPortDownstream
  };
  EFI_PHYSICAL_ADDRESS                                        S3DispatchEntryPoint;
  //[-start-121112-IB10920000-add]//
    EFI_S3_BOOT_SCRIPT_DISPATCH_PROTOCOL   *S3ScriptDispatch;
    Status = gBS->LocateProtocol (
                    &gEfiS3BootScriptDispatchProtocolGuid,
                    NULL,
                     (VOID **)&S3ScriptDispatch
                    );
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  //[-end-121113-IB10920000-add]//

  if (!PchS3Support) {
    ///
    /// Get the PCH S3 Support Protocol
    ///
    Status = gBS->LocateProtocol (
                    &gEfiPchS3SupportProtocolGuid,
                    NULL,
                    (VOID **) &PchS3Support
                    );
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  S3ParameterRootPortDownstream.RootPortBus       = RootPortBus;
  S3ParameterRootPortDownstream.RootPortDevice    = RootPortDevice;
  S3ParameterRootPortDownstream.RootPortFunc      = RootPortFunc;
  S3ParameterRootPortDownstream.TempBusNumberMin  = TempBusNumberMin;
  S3ParameterRootPortDownstream.TempBusNumberMax  = TempBusNumberMax;
  Status = PchS3Support->SetDispatchItem (
                          PchS3Support,
                          &S3DispatchItem,
                          &S3DispatchEntryPoint
                          );
  ASSERT_EFI_ERROR (Status);
  ///
  /// Save the script dispatch item in the Boot Script
  ///
  //SCRIPT_DISPATCH (EFI_ACPI_S3_RESUME_SCRIPT_TABLE, (VOID*)(UINTN)S3DispatchEntryPoint);
  //S3BootScriptSaveDispatch((VOID*)(UINTN)S3DispatchEntryPoint);
//[-start-121113-IB10920000-add]//
  Status = S3ScriptDispatch->Dispatch (
                                     S3ScriptDispatch,
                                     //EFI_BOOT_SCRIPT_DISPATCH_OPCODE,
                                     (VOID *)(UINTN)S3DispatchEntryPoint
                                     );
//[-end-121113-IB10920000-add]//
#else
  Status = EFI_SUCCESS;
#endif
  return Status;
}

/**
  Perform Initialization of the Downstream Root Ports.

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol
  @param[in] RootComplexBar       RCBA of the PCH
  @param[in] PmBase               The PM I/O Base address of the PCH
  @param[in, out] FuncDisableReg  The function disable register. IN / OUT parameter.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   The PCIe Root Port Number of D28:F0 is not found
                                  or invalid
**/
EFI_STATUS
PchInitRootPorts (
  IN DXE_PCH_PLATFORM_POLICY_PROTOCOL           *PchPlatformPolicy,
  IN UINT32                                     RootComplexBar,
  IN UINT16                                     PmBase,
  IN OUT UINT32                                 *FuncDisableReg
  )
{
  EFI_STATUS              Status;
  UINT32                  Data32And;
  UINT32                  Data32Or;
  PCH_PCI_EXPRESS_CONFIG  *PciExpressConfig;
  UINT32                  RpEnableMask;
  UINT8                   PortIndex;
  UINTN                   RPBase;
  UINT32                  LoopTime;
  UINTN                   PciD31F0RegBase;
  UINTN                   PciD31F2RegBase;
  UINTN                   PciD28F0RegBase;
  UINTN                   PciD28F4RegBase;
  UINT32                  RpFnAnd;
  UINT32                  RpFnOr;
  UINT32                  StrpFuseCfg1;
  UINT32                  StrpFuseCfg2;
  UINT8                   RpLaneOwner;
  UINT8                   GbePort;
  UINT8                   NandPort;
  UINT16                  LpcDeviceId;
  UINT32                  BitMask;
  UINT32                  BitValue;
  UINT8                   FuncNum;
  UINT8                   RpPortFuncIndex;
  UINT8                   Func0PortNum;
//[-start-120321-IB05330330-add]//
  UINT16                  LpcRevisionId;
//[-end-120321-IB05330330-add]//
  ///
  /// Whether a root port is hidden by another one with width > x1
  ///
  UINT32                  RpHiddenMask;
  ///
  /// Subtractive Decode ports if enabled
  ///
  UINT32                  SubDecodePort;
  PCH_SERIES              PchSeries;
  UINT8                   Mask;
  BOOLEAN                 LanEnabled;

  DEBUG ((EFI_D_INFO, "PchInitRootPorts() Start\n"));

  PchSeries         = GetPchSeries();
  Status            = EFI_SUCCESS;
  RpEnableMask      = 0;
  RpHiddenMask      = 0;
  PciExpressConfig  = PchPlatformPolicy->PciExpressConfig;
  Data32And         = 0xFFFFFFFF;
  Data32Or          = 0;
  PciD31F0RegBase   = MmPciAddress (
                        0,
                        PchPlatformPolicy->BusNumber,
                        PCI_DEVICE_NUMBER_PCH_LPC,
                        PCI_FUNCTION_NUMBER_PCH_LPC,
                        0
                        );
  PciD31F2RegBase   = MmPciAddress (
                        0,
                        PchPlatformPolicy->BusNumber,
                        PCI_DEVICE_NUMBER_PCH_SATA,
                        PCI_FUNCTION_NUMBER_PCH_SATA,
                        0
                        );
  PciD28F0RegBase   = MmPciAddress (
                        0,
                        PchPlatformPolicy->BusNumber,
                        PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS,
                        GetPchPcieRpfn( RootComplexBar, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1),
                        0
                        );
  PciD28F4RegBase   = MmPciAddress (
                        0,
                        PchPlatformPolicy->BusNumber,
                        PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS,
                        GetPchPcieRpfn( RootComplexBar, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_5),
                        0
                        );
  LpcDeviceId       = MmioRead16 (PciD31F0RegBase + R_PCH_LPC_DEVICE_ID);
  FuncNum           = 0;
  RpPortFuncIndex   = 0;
  Func0PortNum      = 0xFF;
  RpLaneOwner       = 0;

  ///
  /// Configure root port function number mapping and configuration space hiding
  /// Program at end of function
  ///
  RpFnAnd = 0xFFFFFFFF;
  RpFnOr  = 0;
  for (PortIndex = 0; PortIndex < GetPchMaxPciePortNum (); PortIndex++) {
    //
    // if RootPortFunctionSwapping is enabled, Function number is equal to port index.
    // else, use the function number mapping from platform policy.
    //
    if ((PchPlatformPolicy->Revision >= DXE_PCH_PLATFORM_POLICY_PROTOCOL_REVISION_3) &&
        (PciExpressConfig->RootPortFunctionSwapping == 1)) {
      FuncNum = PortIndex;
    } else {
      FuncNum = PciExpressConfig->RootPort[PortIndex].FunctionNumber;
    }
    RpFnAnd &= (UINT32) (~((B_PCH_RCRB_RPFN_RP1CH | B_PCH_RCRB_RPFN_RP1FN) << (PortIndex * S_PCH_RCRB_PRFN_RP_FIELD)));
    RpFnOr |= (FuncNum) << (PortIndex * S_PCH_RCRB_PRFN_RP_FIELD);

    if (FuncNum < GetPchMaxPciePortNum ()) {
      ///
      /// If FunctionNumber of the PCIE Root Port is duplicated, then disable the corresponding "Enable" field.
      ///
      if (RpPortFuncIndex & (UINT8) (1 << FuncNum)) {
        DEBUG ((EFI_D_ERROR, " Hide Root Port %x since its FunctionNumber is duplicated.\n", PortIndex + 1));
        ASSERT (FALSE);
        PciExpressConfig->RootPort[PortIndex].Hide    = PCH_DEVICE_ENABLE;
        PciExpressConfig->RootPort[PortIndex].Enable  = PCH_DEVICE_DISABLE;
        RpHiddenMask |= (1 << PortIndex);
      }
      ///
      /// Set RpPortFuncIndex while the FunctionNumber is used.
      ///
      RpPortFuncIndex |= (UINT8) (1 << FuncNum);
    } else {
      ///
      /// If FunctionNumber of the PCIE Root Port is outside 7, the Root Port Config Hide bit will be set.
      /// If so, then disable the corresponding "Enable" field.
      ///
      DEBUG ((EFI_D_ERROR, " Root Port %x will be hidden since its FunctionNumber is out of 7.\n", PortIndex + 1));
      ASSERT (FALSE);
      PciExpressConfig->RootPort[PortIndex].Enable = PCH_DEVICE_DISABLE;
      RpHiddenMask |= (1 << PortIndex);
    }

    RpFnOr |= ((PciExpressConfig->RootPort[PortIndex].Hide) ? B_PCH_RCRB_RPFN_RP1CH : 0) << (PortIndex * S_PCH_RCRB_PRFN_RP_FIELD);
    ///
    /// Func0PortNum indicates which PCIe Root Port is D28:F0
    ///
    if (FuncNum == 0) {
      Func0PortNum = PortIndex;
    }
  }

  if (Func0PortNum >= GetPchMaxPciePortNum ()) {
    DEBUG ((EFI_D_ERROR, "The PCIe Root Port Number of D28:F0 is not found or invalid!\n"));
    return EFI_INVALID_PARAMETER;
  }
  ///
  /// Hide PCIE root port 1-4 according to the PCIE port configuration 1
  ///
  StrpFuseCfg1 = MmioRead32 (PciD28F0RegBase + R_PCH_PCIE_STRPFUSECFG);
  switch (StrpFuseCfg1 & (UINT32) B_PCH_PCIE_STRPFUSECFG_RPC) {
  case V_PCH_PCIE_STRPFUSECFG_RPC_2_1_1:
    ///
    /// Port Configuration = 01b: 1x2, 2x1 Port 1 (x2), Port 2 (disabled), Ports 3, 4 (x1)
    ///
    RpHiddenMask |= BIT1;
    break;

  case V_PCH_PCIE_STRPFUSECFG_RPC_2_2:
    ///
    /// Port Configuration = 10b: 2x2 Port 1 (x2), Port 3 (x2), Ports 2, 4 (disabled)
    ///
    RpHiddenMask |= (BIT1 | BIT3);
    break;

  case V_PCH_PCIE_STRPFUSECFG_RPC_4:
    ///
    /// Port Configuration = 11b: 1x4 Port 1 (x4), Ports 2-4 (disabled)
    ///
    RpHiddenMask |= (BIT1 | BIT2 | BIT3);
    break;

  default:
    break;
  }

  if (PchSeries == PchH) {
    ///
    /// Hide PCIE root port 5-8 according to the PCIE port configuration
    ///
    StrpFuseCfg2 = MmioRead32 (PciD28F4RegBase + R_PCH_PCIE_STRPFUSECFG);
    switch (StrpFuseCfg2 & (UINT32) B_PCH_PCIE_STRPFUSECFG_RPC) {
      case V_PCH_PCIE_STRPFUSECFG_RPC_2_1_1:
        ///
        /// Port Configuration = 01b: 1x2, 2x1 Port 5 (x2), Port 6 (disabled), Ports 7, 8 (x1)
        ///
        RpHiddenMask |= BIT5;
        break;

      case V_PCH_PCIE_STRPFUSECFG_RPC_2_2:
        ///
        /// Port Configuration = 10b: 2x2 Port 5 (x2), Port 7 (x2), Ports 6, 8 (disabled)
        ///
        RpHiddenMask |= (BIT5 | BIT7);
        break;

      case V_PCH_PCIE_STRPFUSECFG_RPC_4:
        ///
        /// Port Configuration = 11b: 1x4 Port 5 (x4), Ports 6-8 (disabled)
        ///
        RpHiddenMask |= (BIT5 | BIT6 | BIT7);
        break;

      default:
        break;
    }
  }
  ///
  /// If GBE Over PCIe Enabled, then System BIOS must disable the PCI Express* Root Port
  ///
  LanEnabled = !(MmioRead32 (RootComplexBar + R_PCH_RCRB_BUC) & B_PCH_RCRB_BUC_LAN_DIS);
    
  if ((StrpFuseCfg1 & B_PCH_PCIE_STRPFUSECFG_GBE_PCIE_PEN) && LanEnabled) {
    GbePort = (UINT8) ((StrpFuseCfg1 & B_PCH_PCIE_STRPFUSECFG_GBE_PCIEPORTSEL) >> N_PCH_PCIE_STRPFUSECFG_GBE_PCIEPORTSEL);
  } else {
    GbePort = 0xFF;
  }
  ///
  /// If NAND Over PCIe Enabled, then System BIOS must disable the PCI Express* Root Port
  ///
  if ((MmioRead32 (PciD31F2RegBase + 0x300) & BIT0) != 0) {
    NandPort = (UINT8) (((MmioRead32 (PciD31F2RegBase + 0x300)) & 0x1FE) >> 1);
  } else {
    NandPort = 0x00;
  }
  if (PchSeries == PchH) {
    ///
    /// PCH BIOS Spec Rev 0.5.0, Section 8.14 Additional PCI Express* Programming Steps
    /// Step 3
    /// Function disable unused PCIE port
    /// Disable PCIe Port 1 if either of the conditions are met
    /// i.   B0:D28:F0 + 410h[4] = 0b and B0:D28:F0 + 410h[0] = 0b
    /// ii.  GbeOverPCIe is configured to use Pcie Port 1 and SATA port 4 is mapped to this lane instead of PCIe Port 1
    /// iii. NandOverPCIe is configured to use PCIe Port 1
    /// NOTE:
    /// For condition ii, if Gbe is configured to Pcie Port 1, and Pcie Port 1 ownes the shared lane instead of SATA port 4,
    /// then it supports Gbe + 8 PCIES configuration, and BIOS won't hide the Root Port 1.
    ///
    RpLaneOwner = MmioRead8 (PciD28F0RegBase + 0x410);
    if ((((RpLaneOwner & (BIT4)) == 0x0) && ((RpLaneOwner & BIT0) == 0x0)) ||
        ((GbePort == 0x0) && ((RpLaneOwner & (BIT4)) == 0)) ||
        (NandPort == BIT0)) {
      RpHiddenMask |= BIT0;
    }
  }
  if (PchSeries == PchLp) {
    ///
    /// Function disabled unused PCIE port
    /// Disable PCIe Port 1 if either of the conditions are met
    /// i.  B0:D28:F0 + 410h [1:0] = 00b or 10b
    /// ii. NandOverPCIe is configured to use PCIe Port 1
    ///
    if (((MmioRead32 (PciD28F0RegBase + 0x410) & (BIT1 | BIT0)) == 0) ||
        ((MmioRead32 (PciD28F0RegBase + 0x410) & (BIT1 | BIT0)) == BIT1) ||
        (NandPort == BIT0)) {
      RpHiddenMask |= BIT0;
    }
  }
  if (PchSeries == PchH) {
    ///
    /// Disable PCIe Port 2 if either of the conditions are met
    /// i.   B0:D28:F0 + 410h [5] = 0b and B0:D28:F0 + 410h [2] = 0b
    /// ii.  GbeOverPCIe is configured to use Pcie Port 2 and SATA port 5 is mapped to this lane instead of PCIe Port 2
    /// iii. NandOverPCIe is configured to use PCIe Port 2
    /// NOTE:
    /// For condition ii, if Gbe is configured to Pcie Port 2, and Pcie Port 2 ownes the shared lane instead of SATA port 5,
    /// then it supports Gbe + 8 PCIES configuration, and BIOS won't hide the Root Port 2.
    ///
    if ((((RpLaneOwner & (BIT5)) == 0x0) && ((RpLaneOwner & BIT2) == 0x0)) ||
        ((GbePort == 0x1) && ((RpLaneOwner & (BIT5)) == 0)) ||
        (NandPort == BIT1)) {
      RpHiddenMask |= BIT1;
    }
  }
  if (PchSeries == PchLp) {
    ///
    /// Disable PCIe Port 2 if either of the conditions are met
    /// i.  B0:D28:F0 + 410h [3:2] = 00b or 10b
    /// ii. NandOverPCIe is configured to use PCIe Port 2
    ///
    if (((MmioRead32 (PciD28F0RegBase + 0x410) & (BIT3 | BIT2)) == 0) ||
        ((MmioRead32 (PciD28F0RegBase + 0x410) & (BIT3 | BIT2)) == BIT3) ||
        (NandPort == BIT1)) {
      RpHiddenMask |= BIT1;
    }
  }
  ///
  /// Disable PCIe Port 3 if GbeOverPCIe is configured to use Port 3
  ///
  if ((PchSeries == PchH) && (GbePort == 0x2)) {
    RpHiddenMask |= BIT2;
  }
  if ((PchSeries == PchLp) && (GbePort == 0x0)) {
    RpHiddenMask |= BIT2;
  }
  ///
  /// Disable PCIe Port 4 if GbeOverPCIe is configured to use Port 4
  ///
  if ((PchSeries == PchH) && (GbePort == 0x3)) {
    RpHiddenMask |= BIT3;
  }
  if ((PchSeries == PchLp) && (GbePort == 0x1)) {
    RpHiddenMask |= BIT3;
  }
  ///
  /// Disable PCIe Port 5 if GbeOverPCIe is configured to use Port 5
  /// or NandOverPCIe is configure to use Port 5
  ///
  if ((PchSeries == PchH) && (GbePort == 0x4 || NandPort == BIT4)) {
    RpHiddenMask |= BIT4;
  }
  ///
  /// Disable PCIe Port 5 if GbeOverPCIe is configured to use Port 5
  /// NandOverPCIe is configure to use Port 5
  ///
  if ((PchSeries == PchLp) && (GbePort == 0x2 || GbePort == 0x3 || GbePort == 0x4 || GbePort == 0x5 || NandPort == BIT4)) {
    RpHiddenMask |= BIT4;
  }
  ///
  /// Disable PCIe Port 6 if GbeOverPCIe is configured to use Port 6
  /// or NandOverPCIe is configure to use Port 6
  ///
  if ((PchSeries == PchH) && (GbePort == 0x5 || NandPort == BIT5)) {
    RpHiddenMask |= BIT5;
  }
  ///
  /// Disable PCIe Port 6 if SATA P1-P4 is configured
  /// to use Port 6 Lane 0 - Lane 3
  /// or NandOverPCIe is configure to use Port 6
  ///
  if ((PchSeries == PchLp) && ((MmioRead32 (PciD28F0RegBase + 0x410) & (BIT7 | BIT6 | BIT5 | BIT4)) == 0x0) || NandPort == BIT5) {
    RpHiddenMask |= BIT5;
  }
  if (PchSeries == PchH) {
    ///
    /// Disable PCIe Port 7 if GbeOverPCIe is configured to use Port 7
    ///
    if (GbePort == 0x6) {
      RpHiddenMask |= BIT6;
    }
    ///
    /// Disable PCIe Port 8 if GbeOverPCIe is configured to use Port 8
    ///
    if (GbePort == 0x7) {
      RpHiddenMask |= BIT7;
    }
  }
  if (((MmioRead32 ((UINTN) (RootComplexBar + 0x1030))) & ((UINT32) (BIT22))) &&
      (PciExpressConfig->EnableSubDecode)) {
    ///
    /// Assert if Subtractive Decode Port is disabled by configuration
    ///
    ASSERT_EFI_ERROR ((RpHiddenMask & (B_PCH_RCRB_FUNC_DIS_PCI_EX_PORT1 <<
                      (PciExpressConfig->PchPcieSbdePort))) == 0x1);
    SubDecodePort = PciExpressConfig->PchPcieSbdePort;
  } else {
    SubDecodePort = 0xFF;
  }
  ///
  /// The port of function number 0 might be disabled.
  /// Will swap the function number 0 to enabled port on the end of this function.
  /// Gather the enabled root ports here.
  ///
  for (PortIndex = 0; PortIndex < GetPchMaxPciePortNum (); PortIndex++) {
    if ((PciExpressConfig->RootPort[PortIndex].Enable) &&
        (((*FuncDisableReg) & (B_PCH_RCRB_FUNC_DIS_PCI_EX_PORT1 << PortIndex)) == 0)) {
      RpEnableMask |= 1 << PortIndex;
    }
  }
  ///
  /// Disable the port which is going to be hidden.
  ///
  if (RpEnableMask != 0) {
    RpEnableMask &= ~(RpHiddenMask);
  }
  //
  // If RootPortFunctionSwapping is disabled, force to enable the root port of function 0
  //
  if (!((PchPlatformPolicy->Revision >= DXE_PCH_PLATFORM_POLICY_PROTOCOL_REVISION_3) &&
        (PciExpressConfig->RootPortFunctionSwapping == 1))) {
    RpEnableMask |= 1 << Func0PortNum;
  }
  for (PortIndex = 0; PortIndex < GetPchMaxPciePortNum (); PortIndex++) {
    FuncNum = GetPchPcieRpfn (RootComplexBar, PortIndex);
    RPBase = MmPciAddress (
              0,
              PchPlatformPolicy->BusNumber,
              PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS,
              FuncNum,
              0
              );

    ///
    /// PCH BIOS Spec Rev 0.5.0 Section 8.2
    /// Else if the port is hot-plug enable, do not disable the port. If BIOS wants to disable the port,
    /// BIOS should not enable the hot plug capability or must disable the hot plug capability of the port.
    /// Set B0:D28:Fn + 338h [26] = 0b at early POST. Done in PchInitPeim.c PchMiscInit().
    ///
    /// Enabled Slot implemented for the enabled PCIE Root Ports. This is due to new PCIe disabling methodtology
    /// to check if any is populated on the slots.
    ///
    if ((RpHiddenMask & (1 << PortIndex)) == 0) {
      MmioOr16 (RPBase + R_PCH_PCIE_XCAP, B_PCH_PCIE_XCAP_SI);
    }

    if ((RpHiddenMask & (1 << PortIndex)) != 0) {
      *FuncDisableReg |= B_PCH_RCRB_FUNC_DIS_PCI_EX_PORT1 << PortIndex;
    } else if (((RpEnableMask & (1 << PortIndex)) != 0) &&
              ((MmioRead16 (RPBase + R_PCH_PCIE_SLSTS) & BIT6) == 0) &&
              (PciExpressConfig->RootPort[PortIndex].HotPlug == 0) &&
              (PortIndex != SubDecodePort)) {
      ///
      /// PCH BIOS Spec Rev 0.5.0 Section 8.2
      /// Else if the port is not hot plug enable and no PCIe card is detected,
      /// Set B0:D28:Fn + 338h [26] = 1b
      /// Poll B0:D28:Fn + 328h [31:24] until 01h or else 50ms timeout
      /// Set B0:D28:Fn + 408h [27] = 1b
      /// Function disable the port at RCBA+ 3418
      ///
      Data32And = 0xFFFFFFFF;
      Data32Or  = (UINT32) BIT26;
      MmioOr32 ((RPBase + 0x338), Data32Or);
      SCRIPT_MEM_READ_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (RPBase + 0x338),
        &Data32Or,  // Data to be ORed
        &Data32And  // Data to be ANDed
        );

//[-start-120321-IB05330330-modify]//
      //  Refer doc:497869 (For platforms using Lynx Point ES0.)
      //  Step 1: Skip polling of B0:D28:Fn + 328h [31:24] until 01h or 50ms timeout
      //  during S3, S4 and S5 resume
      LpcRevisionId = MmioRead16 (PciD31F0RegBase + R_PCH_LPC_RID);
//[-start-120419-IB05330336-modify]//
      if ((LpcRevisionId != V_PCH_LPT_LPC_RID_0) && (LpcRevisionId != V_PCH_LPT_LPC_RID_1)) {
//[-end-120419-IB05330336-modify]//
      BitMask   = (UINT32) (BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24);
      BitValue  = 1 << 24;
      for (LoopTime = 0; LoopTime < 500; LoopTime++) {
        if ((MmioRead32 (RPBase + 0x328) & BitMask) == BitValue) {
          break;
        } else {
          PchPmTimerStall (100);
        }
      }
      SCRIPT_MEM_POLL (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        RPBase + 0x328,
        &BitMask,
        &BitValue,
        50,
        1000
        );
      }
//[-end-120321-IB05330330-modify]//
      Data32And = 0xFFFFFFFF;
      Data32Or  = (UINT32) BIT27;
      MmioOr32 ((RPBase + 0x408), Data32Or);
      SCRIPT_MEM_READ_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (RPBase + 0x408),
        &Data32Or,  // Data to be ORed
        &Data32And  // Data to be ANDed
        );
      *FuncDisableReg |= B_PCH_RCRB_FUNC_DIS_PCI_EX_PORT1 << PortIndex;
    } else if ((RpEnableMask & (1 << PortIndex)) == 0) {
      ///
      /// Else if the port is not hot plug enable, and BIOS wants to disable the port
      /// If a PCIe card is detected, set B0:D28:Fn + 50h[4] = 1b
      /// followed by function disable the port at RCBA + 3418h
      ///
      if ((MmioRead16 (RPBase + R_PCH_PCIE_SLSTS) & BIT6) != 0) {
        MmioOr16 ((RPBase + R_PCH_PCIE_LCTL), (UINT16) BIT4);
        SCRIPT_MEM_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint16,
          (UINTN) (RPBase + R_PCH_PCIE_LCTL),
          1,
          (VOID *) (UINTN) (RPBase + R_PCH_PCIE_LCTL)
          );
      }
      *FuncDisableReg |= B_PCH_RCRB_FUNC_DIS_PCI_EX_PORT1 << PortIndex;
    } else {
      ///
      /// Configure the rootports
      ///
      Status = PchInitSingleRootPort (
                (UINT8) PortIndex,
                FuncNum,
                PchPlatformPolicy,
                PmBase,
                RootComplexBar
                );
      if (!EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, " Root Port %x device enabled. RpEnableMask: 0x%x\n", PortIndex + 1, RpEnableMask));
      }

      if ((PciExpressConfig->RootPort[PortIndex].TransmitterHalfSwing) &&
          (((MmioRead32 (RPBase + 0x328) & (0x00780000)) >> 19) == 0x7)) {
        MmioOr8 (RPBase + R_PCH_PCIE_LCTL, B_PCH_PCIE_LCTL_LD);
        SCRIPT_MEM_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint8,
          (UINTN) (RPBase + R_PCH_PCIE_LCTL),
          1,
          (VOID *) (UINTN) (RPBase + R_PCH_PCIE_LCTL)
          );
        MmioOr16 (RPBase + R_PCH_PCIE_PECR1, BIT13);
        SCRIPT_MEM_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint16,
          (UINTN) (RPBase + R_PCH_PCIE_PECR1),
          1,
          (VOID *) (UINTN) (RPBase + R_PCH_PCIE_PECR1)
          );
        MmioAnd8 (RPBase + R_PCH_PCIE_LCTL, (UINT8) ~(B_PCH_PCIE_LCTL_LD));
        SCRIPT_MEM_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint8,
          (UINTN) (RPBase + R_PCH_PCIE_LCTL),
          1,
          (VOID *) (UINTN) (RPBase + R_PCH_PCIE_LCTL)
          );
      }
    }

    if (MmioRead32 (RPBase) == 0xFFFFFFFF) {
      continue;
    }
    
    if ((PchSeries == PchH) && (PortIndex == 0 || PortIndex == 4)) {
      ///
      /// PCH BIOS Spec Rev 0.5.0, Section 8.14 Additional PCI Express* Programming Steps
      /// Step 19
      /// Set B0:F28:F0&F4 + F7h[3:2] = 00b
      ///
      MmioAnd8 (RPBase + 0xF7, (UINT8) ~(BIT3 | BIT2));
      SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint8,
        (UINTN) (RPBase + 0xF7),
        1,
        (VOID *) (UINTN) (RPBase + 0xF7)
        );
    }
    if ((PchSeries == PchLp) && (PortIndex == 0 || PortIndex == 4 || PortIndex == 5)) {
      ///
      /// Set B0:F28:F0,F4&F5 + F7h[3:2] = 00b
      ///
      MmioAnd8 (RPBase + 0xF7, (UINT8) ~(BIT3 | BIT2));
      SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint8,
        (UINTN) (RPBase + 0xF7),
        1,
        (VOID *) (UINTN) (RPBase + 0xF7)
        );
    }

    if ((RpHiddenMask & (1 << PortIndex)) == 0) {
      ///
      /// Disable the forwarding of EOI messages.
      /// Set B0:D28:F0/F1/F2/F3/F4/F5/F6/F7 + D4h [1] = 1b
      ///
      MmioOr8 (RPBase + 0xD4, (UINT8) (BIT1));
      SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint8,
        (UINTN) (RPBase + 0xD4),
        1,
        (VOID *) (UINTN) (RPBase + 0xD4)
        );
    }
  }
  //
  // If RootPortFunctionSwapping is disabled, force to enable the root port of function 0
  //
  if (!((PchPlatformPolicy->Revision >= DXE_PCH_PLATFORM_POLICY_PROTOCOL_REVISION_3) &&
        (PciExpressConfig->RootPortFunctionSwapping == 1))) {
    Mask = (0xFF >>(8-GetPchMaxPciePortNum ()));
    if (((*FuncDisableReg >> N_PCH_RCRB_FUNC_DIS_PCI_EX_PORT1) & Mask) != Mask) {
      *FuncDisableReg &= ~(B_PCH_RCRB_FUNC_DIS_PCI_EX_PORT1 << Func0PortNum);
    }
  }

  ///
  /// Configure root port clock gating
  ///
  RpEnableMask = (UINT8)~(*FuncDisableReg >> 16);
  if (PciExpressConfig->RootPortClockGating) {
    PcieEnableClockGating (
      PchPlatformPolicy->BusNumber,
      PchPlatformPolicy,
      RpEnableMask,
      RpHiddenMask,
      RootComplexBar,
      NandPort
      );
  }

  ///
  /// Enable PCIE Relaxed Order. It always allows downstream completions to pass posted write.
  /// Set B0:D28:Fx offset 320h [24,23] to 1, 1b.
  /// Set RCBA 2314h[31,7] to 1, 1b.
  /// Set RCBA 1114h[15,14] to 1, 1b.
  ///
  for (FuncNum = 0; FuncNum < GetPchMaxPciePortNum (); FuncNum++) {
    RPBase = MmPciAddress (
              0,
              PchPlatformPolicy->BusNumber,
              PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS,
              FuncNum,
              0
              );
    if (MmioRead16 (RPBase + R_PCH_PCIE_VENDOR_ID) == 0xFFFF) {
      continue;
    }
    MmioOr32 (RPBase + R_PCH_PCIE_PECR2, (BIT24 | BIT23));
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RPBase + R_PCH_PCIE_PECR2),
      1,
      (VOID *) (UINTN) (RPBase + R_PCH_PCIE_PECR2)
      );
  }
  MmioOr32 (RootComplexBar + R_PCH_RCRB_CIR2314, (BIT31 | BIT7));
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RootComplexBar + R_PCH_RCRB_CIR2314),
    1,
    (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_CIR2314)
    );
  MmioOr16 (RootComplexBar + 0x1114, (BIT15 | BIT14));
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    (UINTN) (RootComplexBar + 0x1114),
    1,
    (VOID *) (UINTN) (RootComplexBar + 0x1114)
    );

  ///
  /// PCH BIOS Spec Rev 0.5.0, Section 8.14 Additional PCI Express* Programming Steps
  /// Step 4
  /// Reconfigured the Function number using RPFN register at RCBA + 404h if function 0 (F0) is disabled
  /// (If Port of function 0 is disable, swap the function number with other enabled port)
  ///
  if ((PchPlatformPolicy->Revision >= DXE_PCH_PLATFORM_POLICY_PROTOCOL_REVISION_3) &&
      (PciExpressConfig->RootPortFunctionSwapping == 1)) {
    Func0PortNum = 0xFF;
    for (PortIndex = 0; PortIndex < GetPchMaxPciePortNum (); PortIndex++) {
      FuncNum = (UINT8)((RpFnOr >> (PortIndex * S_PCH_RCRB_PRFN_RP_FIELD)) & B_PCH_RCRB_RPFN_RP1FN);
      if (FuncNum == 0) {
        Func0PortNum = PortIndex;
        break;
      }
    }
    if ((Func0PortNum < GetPchMaxPciePortNum ()) && ((RpEnableMask & (BIT0 << Func0PortNum)) == 0)) {
      for (PortIndex = 0; PortIndex < GetPchMaxPciePortNum (); PortIndex++) {
        if ((RpEnableMask & (BIT0 << PortIndex)) != 0) {
          FuncNum = (UINT8)((RpFnOr >> (PortIndex * S_PCH_RCRB_PRFN_RP_FIELD)) & B_PCH_RCRB_RPFN_RP1FN);
          RpFnOr &= ((UINT32)~(B_PCH_RCRB_RPFN_RP1FN << (Func0PortNum * S_PCH_RCRB_PRFN_RP_FIELD))) &
                    ((UINT32)~(B_PCH_RCRB_RPFN_RP1FN << (PortIndex * S_PCH_RCRB_PRFN_RP_FIELD)));
          RpFnOr |= ((UINT32)(((UINT32)FuncNum) << (Func0PortNum * S_PCH_RCRB_PRFN_RP_FIELD)));
          break;
        }
      }
    }
  }

  MmioAndThenOr32 (RootComplexBar + R_PCH_RCRB_RPFN, RpFnAnd, RpFnOr);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RootComplexBar + R_PCH_RCRB_RPFN),
    &RpFnOr,  // Data to be ORed
    &RpFnAnd  // Data to be ANDed
    );

  DEBUG ((EFI_D_INFO, "PchInitRootPorts() End\n"));

  return EFI_SUCCESS;
}

/**
  Perform Root Port Initialization.

  @param[in] RootPort             The root port to be initialized (zero based)
  @param[in] RootPortFunction     The PCI function number of the root port
  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol
  @param[in] PmBase               The PM I/O Base address of the PCH
  @param[in] RootComplexBar       RCBA of the PCH

  @retval EFI_SUCCESS             Device found. The root port must be enabled.
  @retval EFI_NOT_FOUND           No device is found on the root port. It may be disabled.
  @exception EFI_UNSUPPORTED      Unsupported operation.
**/
EFI_STATUS
PchInitSingleRootPort (
  IN  UINT8                                     RootPort,
  IN  UINT8                                     RootPortFunction,
  IN  DXE_PCH_PLATFORM_POLICY_PROTOCOL          *PchPlatformPolicy,
  IN  UINT16                                    PmBase,
  IN  UINT32                                    RootComplexBar
  )
{
  EFI_STATUS                        Status;
  UINTN                             RPBase;
  UINTN                             LpcBase;
  UINTN                             PciD28F0RegBase;
  UINTN                             PciD28F4RegBase;
  UINTN                             PciD28F5RegBase;
  UINT32                            CapOffset;
  UINT8                             BusNumber;
  UINT32                            Data32;
  UINT16                            Data16;
  UINT32                            Data32Or;
  UINT32                            Data32And;
  UINT16                            Data16Or;
  UINT16                            Data16And;
  UINT32                            PcieNccSSc;
  UINT8                             DeviceLaneOwner;
  UINT32                            PchPcieHsioAddrPerPort[4];
  UINT8                             NumOfLanePerPort;
  UINT8                             LaneIndex;
  PCH_PCI_EXPRESS_ROOT_PORT_CONFIG  *RootPortConfig;
  BOOLEAN                           DeviceFound;
  PCH_SERIES                        PchSeries;
  UINT32                            DeviceClassDword;

  PchSeries        = GetPchSeries();
  DeviceFound      = FALSE;
  RootPortConfig   = &PchPlatformPolicy->PciExpressConfig->RootPort[RootPort];
  BusNumber        = PchPlatformPolicy->BusNumber;
  RPBase           = MmPciAddress (0, BusNumber, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, RootPortFunction, 0);
  LpcBase          = MmPciAddress (0, BusNumber, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC, 0);
  PciD28F0RegBase  = MmPciAddress (0, BusNumber, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, GetPchPcieRpfn( RootComplexBar, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1), 0);
  PciD28F4RegBase  = MmPciAddress (0, BusNumber, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, GetPchPcieRpfn( RootComplexBar, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_5), 0);
  PciD28F5RegBase  = 0;
  DeviceClassDword = 0;
  if (PchSeries == PchLp) {
    PciD28F5RegBase = MmPciAddress (0, BusNumber, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, GetPchPcieRpfn( RootComplexBar, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_6), 0);
  }
  CapOffset = PcieFindCapId (
                BusNumber,
                PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS,
                RootPortFunction,
                0x10
                );

  if (CapOffset == 0) {
    return EFI_UNSUPPORTED;
  }

  PcieNccSSc       = 0;
  NumOfLanePerPort = 0;
  switch (RootPort) {
    case PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1:
      PcieNccSSc = MmioRead32 (PciD28F0RegBase + 0x32C) & BIT28;
      break;

    case PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_2:
      PcieNccSSc = MmioRead32 (PciD28F0RegBase + 0x32C) & BIT29;
      break;

    case PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_3:
      PcieNccSSc = MmioRead32 (PciD28F0RegBase + 0x32C) & BIT30;
      break;

    case PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_4:
      PcieNccSSc = MmioRead32 (PciD28F0RegBase + 0x32C) & BIT31;
      break;

    case PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_5:
      PcieNccSSc = MmioRead32 (PciD28F4RegBase + 0x32C) & BIT28;
      break;

    case PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_6:
      if (PchSeries == PchH) {
        PcieNccSSc = MmioRead32 (PciD28F4RegBase + 0x32C) & BIT29;
      } else if (PchSeries == PchLp) {
        PcieNccSSc = MmioRead32 (PciD28F5RegBase + 0x32C) & BIT29;
      }
      break;

    case PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_7:
      if (PchSeries == PchH) {
        PcieNccSSc = MmioRead32 (PciD28F4RegBase + 0x32C) & BIT30;
      }
      break;

    case PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8:
      if (PchSeries == PchH) {
        PcieNccSSc = MmioRead32 (PciD28F4RegBase + 0x32C) & BIT31;
      }
      break;

    default:
      PcieNccSSc = 0;
  }

  ///
  /// PCH BIOS Spec Rev 0.5.0, Section 8.14 Additional PCI Express* Programming Steps
  /// Step 5
  /// If corresponding Root Port 4 to 1 in B0:D28:F0 + 32Ch [31:28], Root Port 8 to 5
  /// in B0:D28:F4 + 32Ch [31:28], is set, for EACH PORT (x):
  ///
  if (PcieNccSSc) {
    ///
    /// Step 5.1, 5.2
    /// Set B0:D28:Fx + D4h[4] = 1b
    /// Set B0:D28:Fx + D4h[3:2] = 10b
    ///
    MmioAndThenOr8 ((RPBase + 0xD4), (UINT8)~BIT2, BIT4 | BIT3);
    ///
    /// Step 5.3
    /// Set B0:D28:Fx + D8h[20:18] = 111b
    ///
    MmioOr32 ((RPBase + 0xD8), BIT20 | BIT19 | BIT18);
    ///
    /// Step 5.4
    /// Set B0:D28:Fx + 4Ch[17:15] = 100b, see also step 9.
    ///
    MmioAndThenOr32 ((RPBase + 0x4C), (UINT32)~(BIT16 | BIT15), BIT17);
#ifdef ULT_FLAG
    if (PchSeries == PchLp) {
      ///
      /// Step 5.5
      /// Read the IOBP register below, increase the values by 2 and write back.
      ///   E9002440 [20:16], [12:8]
      ///   E9002640 [20:16], [12:8]
      ///   E9000840 [20:16], [12:8]
      ///   E9000A40 [20:16], [12:8]
      ///   E9000C40 [20:16], [12:8]
      ///   E9000E40 [20:16], [12:8]
      ///   E9001040 [20:16], [12:8]
      ///   E9001240 [20:16], [12:8]
      ///   EA002040 [20:16], [12:8]
      ///   EA002240 [20:16], [12:8]
      ///   EA002440 [20:16], [12:8]
      ///   EA002640 [20:16], [12:8]
      ///
      DeviceLaneOwner = MmioRead8 (PciD28F0RegBase + 0x410);
      if (RootPort == PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_5) {
        NumOfLanePerPort = 4;
          PchPcieHsioAddrPerPort[0] = PchLpPcieHsioAddr[4];
          PchPcieHsioAddrPerPort[1] = PchLpPcieHsioAddr[5];
          PchPcieHsioAddrPerPort[2] = PchLpPcieHsioAddr[6];
          PchPcieHsioAddrPerPort[3] = PchLpPcieHsioAddr[7];
      } else if (RootPort == PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_6) {
        NumOfLanePerPort = 0;
        if ((DeviceLaneOwner & BIT4) == BIT4) {
          PchPcieHsioAddrPerPort[NumOfLanePerPort++] = PchLpPcieHsioAddr[8];
        }
        if ((DeviceLaneOwner & BIT5) == BIT5) {
          PchPcieHsioAddrPerPort[NumOfLanePerPort++] = PchLpPcieHsioAddr[9];
        }
        if ((DeviceLaneOwner & BIT6) == BIT6) {
          PchPcieHsioAddrPerPort[NumOfLanePerPort++] = PchLpPcieHsioAddr[10];
        }
        if ((DeviceLaneOwner & BIT7) == BIT7) {
          PchPcieHsioAddrPerPort[NumOfLanePerPort++] = PchLpPcieHsioAddr[11];
        }
      } else {
        NumOfLanePerPort = 1;
        PchPcieHsioAddrPerPort[0] = PchLpPcieHsioAddr[RootPort];
      }
    }
#endif // ULT_FLAG
#ifdef TRAD_FLAG
    if (PchSeries == PchH) {
      ///
      /// Step 5.5
      /// Read the IOBP register below, increase the values by 2 and write back.
      /// Dedicated lane Setting
      ///   E9002040 [20:16], [12:8]
      ///   E9002240 [20:16], [12:8]
      ///   E9002440 [20:16], [12:8]
      ///   E9002640 [20:16], [12:8]
      ///   E9002840 [20:16], [12:8]
      ///   E9002A40 [20:16], [12:8]
      /// Shared lane Setting
      ///   E9002C40 [20:16], [12:8]
      ///   E9002E40 [20:16], [12:8]
      ///   EA002040 [20:16], [12:8]
      ///   EA002240 [20:16], [12:8]
      ///
      DeviceLaneOwner = MmioRead8 (PciD28F0RegBase + 0x410);
      NumOfLanePerPort = 1;
      if (RootPort == PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1) {
        if ((DeviceLaneOwner & (BIT1 | BIT0)) == BIT0) {
          PchPcieHsioAddrPerPort[0] = PchHPcieHsioAddr[0];
        } else {
          PchPcieHsioAddrPerPort[0] = PchHPcieHsioAddr[8];
        }
      } else if (RootPort == PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_2) {
        if ((DeviceLaneOwner & (BIT3 | BIT2)) == BIT2) {
          PchPcieHsioAddrPerPort[0] = PchHPcieHsioAddr[1];
        } else {
          PchPcieHsioAddrPerPort[0] = PchHPcieHsioAddr[9];
        }
      } else {
        PchPcieHsioAddrPerPort[0] = PchHPcieHsioAddr[RootPort];
      }
    }
#endif // TRAD_FLAG
    for (LaneIndex = 0; LaneIndex < NumOfLanePerPort; LaneIndex++) {
      Status = ReadIobp (
                RootComplexBar,
                PchPcieHsioAddrPerPort[LaneIndex],
                &Data32
                );
      ASSERT_EFI_ERROR (Status);
      Data32 += 0x00020200;
      Status = ProgramIobp (
                RootComplexBar,
                PchPcieHsioAddrPerPort[LaneIndex],
                0x0,
                Data32
                );
      ASSERT_EFI_ERROR (Status);
      Status = SetProgramIobpS3Item (
                RootComplexBar,
                PchPcieHsioAddrPerPort[LaneIndex],
                0x0,
                Data32
                );
      ASSERT_EFI_ERROR (Status);
    }
    ///
    /// Step 5.6
    /// Set B0:D28:Fx + 338h[26] = 0b
    ///
    MmioAnd32 ((RPBase + 0x338), (UINT32)~BIT26);
  }

  Data32 = MmioRead32 (RPBase + R_PCH_PCIE_DCAP2);
  ///
  /// PCH BIOS Spec Rev 0.5.5, Section 8.14.1 Power Optimizer Configuration
  /// Step 1
  /// Enable support Latency Tolerance Reporting (LTR)
  /// Step 1.1
  /// Program B0:D28:F0~F7 + 400h to 883C883Ch for ports which has a PCIe
  /// device attached to it.
  /// Done in PcieSetPm()
  /// Step 1.2
  /// Program B0:D28:F0~F7 + 404h [1:0] = 11b for ports which has a PCIe device
  /// device attached to it.
  /// Done in PcieSetPm()
  /// Step 1.3
  /// Program B0:D28:F0-F7 + 64h [11] = 1b
  ///
  if (PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[RootPort].LtrEnable == PCH_DEVICE_ENABLE) {
    Data32 |= BIT11;
  } else {
    Data32 &= (UINT32) ~(BIT11);
  }
  ///
  /// Step 2
  /// Support Optimized Buffer Flush/Fill (OBFF)
  /// Step 2.1
  /// Program B0:D28:F0-F7 + 64h [19:18] = 2h
  ///
  if (PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[RootPort].ObffEnable == PCH_DEVICE_ENABLE) {
    Data32 |= BIT19;
  } else {
    Data32 &= (UINT32) ~(BIT19 | BIT18);
  }
  MmioWrite32 (RPBase + R_PCH_PCIE_DCAP2, Data32);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RPBase + R_PCH_PCIE_DCAP2),
    1,
    &Data32
    );

  Data16 = MmioRead16 (RPBase + R_PCH_PCIE_DCTL2);
  ///
  /// Step 1.4
  /// Program B0:D28:F0-F7 + 68h [10] = 1b
  ///
  if (PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[RootPort].LtrEnable == PCH_DEVICE_ENABLE) {
    Data16 |= BIT10;
  } else {
    Data16 &= (UINT16) ~(BIT10);
  }
  ///
  /// Step 2.2
  /// Program B0:D28:F0-F7 + 68h [14:13] = 3h
  ///
  if (PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[RootPort].ObffEnable == PCH_DEVICE_ENABLE) {
    Data16 |= BIT14 | BIT13;
  } else {
    Data16 &= (UINT16) ~(BIT14 | BIT13);
  }

  MmioWrite16 (RPBase + R_PCH_PCIE_DCTL2, Data16);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    (UINTN) (RPBase + R_PCH_PCIE_DCTL2),
    1,
    &Data16
    );
  ///
  /// PCH BIOS Spec Rev 0.5.0, Section 8.14 Additional PCI Express* Programming Steps
  /// Step 6
  /// Set B0:D28:F0~F7 + 318h [31:16] = 1414h
  ///
  Data32Or  = 0x14140000;
  Data32And = (UINT32) (~(0xFFFF0000));
  MmioAndThenOr32 (RPBase + 0x318, Data32And, Data32Or);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RPBase + 0x318),
    &Data32Or,  // Data to be ORed
    &Data32And  // Data to be ANDed
    );

  ///
  /// Step 7
  /// If B0:D28:F0 + F5h[0] = 1b or step 5 is TRUE, set B0:D28:F0~F7 + 4Ch[17:15] = 100b
  /// Else set B0:D28:F0~F7 + 4Ch[17:15] = 010b
  ///
  if ((MmioRead8 (PciD28F0RegBase + 0xF5) & BIT0) || PcieNccSSc) {
    Data32Or = BIT17;
  } else {
    Data32Or = BIT16;
  }

  Data32And = (UINT32) (~B_PCH_PCIE_LCAP_EL1);
  MmioAndThenOr32 (RPBase + R_PCH_PCIE_LCAP, Data32And, Data32Or);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RPBase + R_PCH_PCIE_LCAP),
    &Data32Or,  // Data to be ORed
    &Data32And  // Data to be ANDed
    );
  ///
  /// Step 8
  /// Set B0:D28:F0~F7 + 314h[31:24] = 74h
  /// Step 9
  /// Set B0:D28:F0~F7 + 314h[23:16] = 3Ah
  /// Step 10
  /// Set B0:D28:F0~F7 + 314h[15:08] = 36h
  /// Step 11
  /// Set B0:D28:F0~F7 + 314h[07:00] = 1Bh
  ///
  Data32Or  = 0x743A361B;
  Data32And = (UINT32) (0x0);
  MmioAndThenOr32 (RPBase + 0x314, Data32And, Data32Or);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RPBase + 0x314),
    &Data32Or,  // Data to be ORed
    &Data32And  // Data to be ANDed
    );
  ///
  /// Step 12
  /// Set B0:D28:F0~F7 + D8h[17:15] = 3h
  ///
  Data32And = (UINT32) (~B_PCH_PCIE_MPC_CCEL);
  Data32Or  = BIT16 | BIT15;
  MmioAndThenOr32 (RPBase + R_PCH_PCIE_MPC, Data32And, Data32Or);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RPBase + R_PCH_PCIE_MPC),
    &Data32Or,  // Data to be ORed
    &Data32And  // Data to be ANDed
    );
  ///
  /// Step 13
  /// Set B0:D28:F0~F7 + 33Ch[24:0] = 854C74h
  ///
  Data32And = 0xFF000000;
  Data32Or  = 0x854C74;
  MmioAndThenOr32 (RPBase + 0x33C, Data32And, Data32Or);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RPBase + 0x33C),
    &Data32Or,  // Data to be ORed
    &Data32And  // Data to be ANDed
    );

  ///
  /// Step 16
  /// Set B0:D28:F0~F7 + D8h[25] = 1b
  ///
  Data32And = (UINT32) ~(B_PCH_PCIE_MPC_IRRCE);
  Data32Or  = B_PCH_PCIE_MPC_IRRCE;
  MmioAndThenOr32 (RPBase + R_PCH_PCIE_MPC, Data32And, Data32Or);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RPBase + R_PCH_PCIE_MPC),
    &Data32Or,  // Data to be ORed
    &Data32And  // Data to be ANDed
    );
  ///
  /// Step 17
  /// For system that support MCTP over PCIE set
  /// Set B0:D28:F0~F7 + D8h[27] = 1b
  /// Set B0:D28:F0~F7 + D8h[3] = 1b
  ///
  Data32And = (UINT32) ~(B_PCH_PCIE_MPC_MCTPSE | B_PCH_PCIE_MPC_MMBNCE);
  Data32Or  = B_PCH_PCIE_MPC_MCTPSE | B_PCH_PCIE_MPC_MMBNCE;
  MmioAndThenOr32 (RPBase + R_PCH_PCIE_MPC, Data32And, Data32Or);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RPBase + R_PCH_PCIE_MPC),
    &Data32Or,  // Data to be ORed
    &Data32And  // Data to be ANDed
    );
  ///
  /// Step 18
  /// Set B0:D28:F0~F7 + F5h[7:4] = 0000b
  ///
  MmioAnd8 (RPBase + 0xF5, (UINT8) ~(BIT4 | BIT5 | BIT6 | BIT7));
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint8,
    (UINTN) (RPBase + 0xF5),
    1,
    (VOID *) (UINTN) (RPBase + 0xF5)
    );
  ///
  /// Step 20
  /// If there is no IOAPIC behind the root port, set EOI Forwarding Disable bit (B0:D28:F0-F7:D4h[1]) to 1b.
  /// Done in PchPciExpressHelpersLibrary.c PcieSetEoiFwdDisable()
  ///
  /// Step 21
  /// For systems that support Advanced Error Reporting set
  /// B0:D28:F0~F7:100h[19:0] = 10001h
  /// Else
  /// B0:D28:F0~F7:100h[19:0] = 0h
  ///
  if (RootPortConfig->AdvancedErrorReporting) {
    Data32 = (UINT32)(BIT16 | BIT0);
  } else {
    Data32 = 0;
  }
  ///
  /// For LPT-LP, setup the next capability offset to 0x200
  /// B0:D28:F0~F7:100h[29] = 1b
  ///
  if (PchSeries == PchLp) {
    Data32 |= BIT29;
  }
  MmioWrite32 (RPBase + R_PCH_PCIE_AECH, Data32);

  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RPBase + R_PCH_PCIE_AECH),
    1,
    (VOID *) (UINTN) (RPBase + R_PCH_PCIE_AECH)
    );
  ///
  /// Step 22
  /// System bios should initiate link retrain for all slots that has card populated after register restoration.
  /// Done in PchPciExpressHelpersLibrary.c PchPcieInitRootPortDownstreamDevices ()
  /// Step 23
  /// System BIOS should read and write back to capability register B0:D28:F0 offsets 34h, 40h,
  /// 80h and 90h after it has been configure or prior to boot
  /// Done in PchInit.c PciERWORegInit ()
  ///
  /// Configure Extended Synch
  ///
  if (RootPortConfig->ExtSync) {
    Data16And = (UINT16) (-1);
    Data16Or  = B_PCH_PCIE_LCTL_ES;
  } else {
    Data16And = (UINT16) (~B_PCH_PCIE_LCTL_ES);
    Data16Or  = 0;
  }

  MmioAndThenOr16 (RPBase + R_PCH_PCIE_LCTL, Data16And, Data16Or);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    (UINTN) (RPBase + R_PCH_PCIE_LCTL),
    &Data16Or,  // Data to be ORed
    &Data16And  // Data to be ANDed
    );

  if (PchSeries == PchLp) {
    ///
    /// Step 23
    /// Program B0:D28:F0~F5:320h[21:20] to 01b and [8:6] to 011b
    ///
    Data32And = (UINT32) (~(BIT21 | BIT20 | BIT8 | BIT7 | BIT6));
    Data32Or  = BIT20 | BIT7 | BIT6;
    MmioAndThenOr32 (RPBase + R_PCH_PCIE_PECR2, Data32And, Data32Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RPBase + R_PCH_PCIE_PECR2),
      &Data32Or,  // Data to be ORed
      &Data32And  // Data to be ANDed
      );
  }

  ///
  /// Configure Completion Timeout
  ///
  Data16And = (UINT16)~(B_PCH_PCIE_DCTL2_CTD | B_PCH_PCIE_DCTL2_CTV);
  Data16Or  = 0;
  if (RootPortConfig->CompletionTimeout == PchPcieCompletionTO_Disabled) {
    Data16Or = B_PCH_PCIE_DCTL2_CTD;
  } else {
    switch (RootPortConfig->CompletionTimeout) {
    case PchPcieCompletionTO_Default:
      Data16Or = V_PCH_PCIE_DCTL2_CTV_DEFAULT;
      break;

    case PchPcieCompletionTO_16_55ms:
      Data16Or = V_PCH_PCIE_DCTL2_CTV_40MS_50MS;
      break;

    case PchPcieCompletionTO_65_210ms:
      Data16Or = V_PCH_PCIE_DCTL2_CTV_160MS_170MS;
      break;

    case PchPcieCompletionTO_260_900ms:
      Data16Or = V_PCH_PCIE_DCTL2_CTV_400MS_500MS;
      break;

    case PchPcieCompletionTO_1_3P5s:
      Data16Or = V_PCH_PCIE_DCTL2_CTV_1P6S_1P7S;
      break;

    default:
      Data16Or = 0;
      break;
    }
  }

  MmioAndThenOr16 (RPBase + R_PCH_PCIE_DCTL2, Data16And, Data16Or);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    (UINTN) (RPBase + R_PCH_PCIE_DCTL2),
    &Data16Or,  // Data to be ORed
    &Data16And  // Data to be ANDed
    );

  ///
  /// Set the Slot Implmemented Bit.  Note that this must be set before
  /// presence is valid.
  /// PCH BIOS Spec Rev 0.5.0 section 8.2.2, The System BIOS must
  /// initialize the "Slot Implemented" bit of the PCI Express* Capabilities Register,
  /// XCAP D28:F0~7:Reg 42h[8] of each available and enabled downstream root port.
  /// Setting this bit will indicate that the PCI Express* link associated with this
  /// port is connected to a slot (as compared to being connected to an integrated
  /// device component).
  ///
  if (RootPortConfig->SlotImplemented) {
    ///
    /// Slot Implemented enabled earlier. Here will only save this register for enabled ports
    ///
    Data16Or  = BIT8;
    Data16And = 0xFFFF;
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint16,
      (UINTN) (RPBase + CapOffset + 2),
      &Data16Or,  // Data to be ORed
      &Data16And  // Data to be ANDed
      );
    ///
    /// For Root Port Slots Numbering on the CRBs.
    ///
    Data32Or  = 0;
    Data32And = (UINT32) (~(B_PCH_PCIE_SLCAP_SLV | B_PCH_PCIE_SLCAP_SLS | B_PCH_PCIE_SLCAP_PSN));
    ///
    /// PCH BIOS Spec Rev 0.5.0 section 8.8.2.1
    /// Note: If Hot Plug is supported, then write a 1 to the Hot Plug Capable (bit6) and Hot Plug
    /// Surprise (bit5) in the Slot Capabilities register, D28:F0~7:Reg 54h. Otherwise,
    /// write 0 to the bits PCIe Hot Plug SCI Enable
    ///
    Data32And &= (UINT32) (~(B_PCH_PCIE_SLCAP_HPC | B_PCH_PCIE_SLCAP_HPS));
    if (RootPortConfig->HotPlug) {
      Data32Or |= B_PCH_PCIE_SLCAP_HPC | B_PCH_PCIE_SLCAP_HPS;
    }
    ///
    /// Get the width from LCAP
    /// Slot Type  X1  X4/X8  X16
    /// Default     10W   25W  75W
    /// The slot power consumption and allocation is platform specific. Please refer to the
    /// "PCI Express* Card Electromechanical (CEM) Spec" for details.
    /// bugbug what's the default setting for X2
    ///
    if ((((MmioRead32 (RPBase + R_PCH_PCIE_LCAP)) & B_PCH_PCIE_LCAP_MLW) >> 4) == 0x01) {
      Data32Or |= (UINT32) (100 << 7);
      Data32Or |= (UINT32) (1 << 15);
    } else if ((((MmioRead32 (RPBase + R_PCH_PCIE_LCAP)) & B_PCH_PCIE_LCAP_MLW) >> 4) >= 0x04) {
      Data32Or |= (UINT32) (250 << 7);
      Data32Or |= (UINT32) (1 << 15);
    }

    Data32Or |= (UINT32) (RootPortConfig->PhysicalSlotNumber << 19);
    MmioAndThenOr32 (RPBase + R_PCH_PCIE_SLCAP, Data32And, Data32Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RPBase + R_PCH_PCIE_SLCAP),
      &Data32Or,  // Data to be ORed
      &Data32And  // Data to be ANDed
      );
  }
  ///
  /// Initialize downstream devices
  ///
  Status = PchPcieInitRootPortDownstreamDevices (
            BusNumber,
            (UINT8) PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS,
            RootPortFunction,
            PchPlatformPolicy->PciExpressConfig->TempRootPortBusNumMin,
            PchPlatformPolicy->PciExpressConfig->TempRootPortBusNumMax,
            &DeviceClassDword
            );
  if (Status == EFI_SUCCESS) {
    DeviceFound = TRUE;
  } else {
    ///
    /// Disable the forwarding of EOI messages.
    /// Set B0:D28:F0/F1/F2/F3/F4/F5/F6/F7 + D4h [1] = 1b
    ///
    MmioOr8 (RPBase + 0xD4, (UINT8) (BIT1));
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint8,
      (UINTN) (RPBase + 0xD4),
      1,
      (VOID *) (UINTN) (RPBase + 0xD4)
      );
  }
  ///
  /// Not checking the error status here - downstream device not present does not
  /// mean an error of this root port. Our return status of EFI_SUCCESS means this
  /// port is enabled and outer function depends on this return status to do
  /// subsequent initializations.
  ///
  Status = SetInitRootPortDownstreamS3Item (
            BusNumber,
            (UINT8) PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS,
            RootPortFunction,
            PchPlatformPolicy->PciExpressConfig->TempRootPortBusNumMin,
            PchPlatformPolicy->PciExpressConfig->TempRootPortBusNumMax
            );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Additional configurations
  ///

  ///
  /// Enable Subtractive Decode of RootPort
  /// Step 1
  /// Ensure flash descriptor PCH Strap 9 Bit 14, which read RCBA + 1030h[22] = 1b
  /// Step 2
  /// Set B0:D28:Fn + ECh[1:0] = 11b,
  /// If downstream is PCI-to-PCI bridge, then also set B0:D28:Fn + ECh[2] = 1b
  ///
  if ((RootPort == PchPlatformPolicy->PciExpressConfig->PchPcieSbdePort)      &&
      (MmioRead32 ((UINTN) (RootComplexBar + 0x1030)) & BIT22) &&
      (PchPlatformPolicy->PciExpressConfig->EnableSubDecode))
  {
    Data32Or = (B_PCH_PCIE_PECR3_SDCDID | B_PCH_PCIE_PECR3_SDE);
    if ((((DeviceClassDword >> 24) & 0xFF) == PCI_CLASS_BRIDGE) &&            // BCC
        (((DeviceClassDword >> 16) & 0xFF) == PCI_CLASS_BRIDGE_CARDBUS))      // SCC
    {
      Data32Or |= BIT2;
    }

    MmioOr32 (RPBase + R_PCH_PCIE_PECR3, Data32Or);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RPBase + R_PCH_PCIE_PECR3),
      1,
      (VOID *) (UINTN) (RPBase + R_PCH_PCIE_PECR3)
      );
  }

  ///
  /// Configure Error Reporting policy in the Device Control Register
  ///
  Data16And = (UINT16) (~(B_PCH_PCIE_DCTL_URE | B_PCH_PCIE_DCTL_FEE | B_PCH_PCIE_DCTL_NFE | B_PCH_PCIE_DCTL_CEE));
  Data16Or  = 0;

  if (RootPortConfig->UnsupportedRequestReport) {
    Data16Or |= B_PCH_PCIE_DCTL_URE;
  }

  if (RootPortConfig->FatalErrorReport) {
    Data16Or |= B_PCH_PCIE_DCTL_FEE;
  }

  if (RootPortConfig->NoFatalErrorReport) {
    Data16Or |= B_PCH_PCIE_DCTL_NFE;
  }

  if (RootPortConfig->CorrectableErrorReport) {
    Data16Or |= B_PCH_PCIE_DCTL_CEE;
  }

  MmioAndThenOr16 (RPBase + R_PCH_PCIE_DCTL, Data16And, Data16Or);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    (UINTN) (RPBase + R_PCH_PCIE_DCTL),
    &Data16Or,  // Data to be ORed
    &Data16And  // Data to be ANDed
    );

  ///
  /// Configure Interrupt / Error reporting in R_PCH_PCIE_RCTL
  ///
  Data16And = (UINT16) (~(B_PCH_PCIE_RCTL_PIE | B_PCH_PCIE_RCTL_SFE | B_PCH_PCIE_RCTL_SNE | B_PCH_PCIE_RCTL_SCE));
  Data16Or  = 0;

  if (RootPortConfig->PmeInterrupt) {
    Data16Or |= B_PCH_PCIE_RCTL_PIE;
  }

  if (RootPortConfig->SystemErrorOnFatalError) {
    Data16Or |= B_PCH_PCIE_RCTL_SFE;
  }

  if (RootPortConfig->SystemErrorOnNonFatalError) {
    Data16Or |= B_PCH_PCIE_RCTL_SNE;
  }

  if (RootPortConfig->SystemErrorOnCorrectableError) {
    Data16Or |= B_PCH_PCIE_RCTL_SCE;
  }

  MmioAndThenOr16 (RPBase + R_PCH_PCIE_RCTL, Data16And, Data16Or);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    (UINTN) (RPBase + R_PCH_PCIE_RCTL),
    &Data16Or,  // Data to be ORed
    &Data16And  // Data to be ANDed
    );

  ///
  /// Root PCI-E Powermanagement SCI Enable
  ///
  if (RootPortConfig->PmSci) {
    ///
    /// PCH BIOS Spec Rev 0.5.0 section 8.7.3 BIOS Enabling of Intel PCH PCI Express* PME SCI Generation
    /// Step 1
    /// Make sure that PME Interrupt Enable bit, D28:F0-7:Reg 5Ch[3] is cleared
    ///
    Data16And = (UINT16) (~B_PCH_PCIE_RCTL_PIE);
    Data16Or  = 0;
    MmioAnd16 (RPBase + R_PCH_PCIE_RCTL, Data16And);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint16,
      (UINTN) (RPBase + R_PCH_PCIE_RCTL),
      &Data16Or,  // Data to be ORed
      &Data16And  // Data to be ANDed
      );

    ///
    /// Step 2
    /// Program Misc Port Config (MPC) register at PCI config space offset
    /// D8h as follows:
    /// Set Power Management SCI Enable bit, D28:F0~7:Reg D8h[31]
    /// Clear Power Management SMI Enable bit, D28:F0~7:Reg D8h[0]
    ///
    Data32And = (UINT32) (~B_PCH_PCIE_MPC_PMME);
    Data32Or  = B_PCH_PCIE_MPC_PMCE;
    MmioAndThenOr32 (RPBase + R_PCH_PCIE_MPC, Data32And, Data32Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RPBase + R_PCH_PCIE_MPC),
      &Data32Or,  // Data to be ORed
      &Data32And  // Data to be ANDed
      );

    ///
    /// Step 3
    /// Make sure GPE0 Register (PMBase+20h[9]), PCI_EXP_STS is 0, clear it if not zero
    ///
    if (PchSeries == PchLp) {
      Data32Or = IoRead32 (PmBase + R_PCH_ACPI_GPE0_STS_127_96);
      if ((Data32Or & B_PCH_ACPI_GPE0_STS_127_96_PCI_EXP) != 0) {
        Data32Or = B_PCH_ACPI_GPE0_STS_127_96_PCI_EXP;
        IoWrite32 (PmBase + R_PCH_ACPI_GPE0_STS_127_96, Data32Or);
        SCRIPT_IO_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint32,
          (UINTN) (PmBase + R_PCH_ACPI_GPE0_STS_127_96),
          1,
          &Data32Or
          );
      }
    } else if (PchSeries == PchH) {
      Data32Or = IoRead32 (PmBase + R_PCH_ACPI_GPE0a_STS);
      if ((Data32Or & B_PCH_ACPI_GPE0a_STS_PCI_EXP) != 0) {
        Data32Or = B_PCH_ACPI_GPE0a_STS_PCI_EXP;
        IoWrite32 (PmBase + R_PCH_ACPI_GPE0a_STS, Data32Or);
        SCRIPT_IO_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint32,
          (UINTN) (PmBase + R_PCH_ACPI_GPE0a_STS),
          1,
          &Data32Or
          );
      }
    }
    ///
    /// Step 4
    /// Set BIOS_PCI_EXP_EN bit, D31:F0:Reg A0[10],
    /// to globally enable the setting of the PCI_EXP_STS bit by a PCI Express* PME event.
    ///
    Data16Or = MmioRead16 (LpcBase + R_PCH_LPC_GEN_PMCON_1);
    if ((Data16Or & B_PCH_LPC_GEN_PMCON_BIOS_PCI_EXP_EN) == 0) {
      Data16And = 0xFFFF;
      Data16Or  = B_PCH_LPC_GEN_PMCON_BIOS_PCI_EXP_EN;
      MmioOr16 (LpcBase + R_PCH_LPC_GEN_PMCON_1, Data16Or);
      SCRIPT_MEM_READ_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint16,
        (UINTN) (LpcBase + R_PCH_LPC_GEN_PMCON_1),
        &Data16Or,  // Data to be ORed
        &Data16And  // Data to be ANDed
        );
    }
  }

  if (RootPortConfig->HotPlug) {
    ///
    /// PCH BIOS Spec Rev 0.5.0 section 8.8.2.1
    /// Step 1
    /// Clear following status bits, by writing 1b to them, in the Slot
    /// Status register at offset 1Ah of PCI Express Capability structure:
    /// Attention Button Pressed (bit0)
    /// Presence Detect Changed (bit3)
    ///
    Data16And = 0xFFFF;
    Data16Or  = (BIT3 | BIT0);
    MmioOr16 (RPBase + CapOffset + 0x1A, Data16Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint16,
      (UINTN) (RPBase + CapOffset + 0x1A),
      &Data16Or,  // Data to be ORed
      &Data16And  // Data to be ANDed
      );
    ///
    /// Step 2
    /// Program the following bits in Slot Control register at offset 18h
    /// of PCI Express* Capability structure:
    /// Attention Button Pressed Enable (bit0) = 1b
    /// Presence Detect Changed Enable (bit3) = 1b
    /// Hot Plug Interrupt Enable (bit5) = 0b
    ///
    Data16And = (UINT16) (~BIT5);
    Data16Or  = (BIT3 | BIT0);
    MmioAndThenOr16 (RPBase + CapOffset + 0x18, Data16And, Data16Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint16,
      (UINTN) (RPBase + CapOffset + 0x18),
      &Data16Or,  // Data to be ORed
      &Data16And  // Data to be ANDed
      );
    ///
    /// Step 3
    /// Program Misc Port Config (MPC) register at PCI config space offset
    /// D8h as follows:
    /// Hot Plug SCI Enable (HPCE, bit30) = 1b
    /// Hot Plug SMI Enable (HPME, bit1) = 0b
    ///
    Data32And = (UINT32) (~B_PCH_PCIE_MPC_HPME);
    Data32Or  = B_PCH_PCIE_MPC_HPCE;
    MmioAndThenOr32 (RPBase + R_PCH_PCIE_MPC, Data32And, Data32Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RPBase + R_PCH_PCIE_MPC),
      &Data32Or,  // Data to be ORed
      &Data32And  // Data to be ANDed
      );
    ///
    /// Step 4
    /// Clear GPE0 Register (PMBase+20h), bit1, HOT_PLUG_STS by writing 1
    ///
    if (PchSeries == PchLp) {
      IoWrite32 (PmBase + R_PCH_ACPI_GPE0_STS_127_96, (UINT32) B_PCH_ACPI_GPE0_STS_127_96_HOT_PLUG);
    } else if (PchSeries == PchH) {
      IoWrite32 (PmBase + R_PCH_ACPI_GPE0a_STS, (UINT32) B_PCH_ACPI_GPE0a_STS_HOT_PLUG);
    }

    ///
    /// PCH BIOS Spec Rev 0.5.0 section 8.9
    /// BIOS should mask the reporting of Completion timeout (CT) errors byerrors by setting
    /// the uncorrectable Error Mask register D28:F0~7:Reg 108h[14].
    ///
    Data32And = 0xFFFFFFFF;
    Data32Or  = B_PCH_PCIE_UEM_CT;
    MmioOr32 (RPBase + R_PCH_PCIE_UEM, Data32Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RPBase + R_PCH_PCIE_UEM),
      &Data32Or,  // Data to be ORed
      &Data32And  // Data to be ANDed
      );
  }

  if (DeviceFound == TRUE || (RootPortConfig->HotPlug == PCH_DEVICE_ENABLE)) {
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
}

/**
  This is the function to enable the clock gating for PCI Express ports.

  @param[in] BusNumber            The Bus Number of the PCH device
  @param[in] PchPlatformPolicy    PCH Platform Policy protocol
  @param[in] RpEnableMask         Bit Mask indicating the enabled root ports
  @param[in] RpHiddenMask         Bit Mask indicating the root ports used for other > x1 root ports
  @param[in] RootComplexBar       Root complex base address

  @retval EFI_SUCCESS             Successfully completed.
**/
EFI_STATUS
PcieEnableClockGating (
  IN  UINT8                                     BusNumber,
  IN  DXE_PCH_PLATFORM_POLICY_PROTOCOL          *PchPlatformPolicy,
  IN  UINT32                                    RpEnableMask,
  IN  UINT32                                    RpHiddenMask,
  IN  UINT32                                    RootComplexBar,
  IN  UINT32                                    NandPort
  )
{
  UINTN       RPBase;
  UINT32      PortIndex;
  UINT8       Data8Or;
  UINT8       Data8And;
  UINT32      Data32Or;
  UINT32      Data32And;
  UINT16      GpioBase;
  BOOLEAN     ClkreqPerPortSupported;
  PCH_SERIES  PchSeries;
//[-start-130916-IB08620311-add]//
  UINT16      LpcDeviceId;
//[-end-130916-IB08620311-add]//

  PchSeries              = GetPchSeries();
  GpioBase               = 0;
//[-start-130916-IB08620311-add]//
  LpcDeviceId            = MmioRead16 (
                             MmPciAddress (
                             0,
                             BusNumber,
                             PCI_DEVICE_NUMBER_PCH_LPC,
                             PCI_FUNCTION_NUMBER_PCH_LPC,
                             0
                             ) + R_PCH_LPC_DEVICE_ID);
//[-end-130916-IB08620311-add]//

  ///
  /// PCH BIOS Spec Rev 0.5.0, section 19.10 Enabling Clock Gating
  /// 2.1
  /// For each enabled PCI Express* root port, program D28:F0~F7:Reg E1h[1:0] to 3h to enable dynamic clock gating.
  /// System BIOS also require to set D28:F0~F7:Reg E8h[0] = 1b
  /// 2.2
  /// Additionally, if port 0 is in x2 mode, these bits should not be set for port 1.
  /// Likewise, if port 0 is in x4 mode, these bits should not be set for ports 1, 2, or 3
  ///
  for (PortIndex = 0; PortIndex < GetPchMaxPciePortNum (); PortIndex++) {
    ClkreqPerPortSupported = FALSE;
    if (PchSeries == PchLp) {
      GpioBase = MmioRead16 (
                   MmPciAddress (0,
                   DEFAULT_PCI_BUS_NUMBER_PCH,
                   PCI_DEVICE_NUMBER_PCH_LPC,
                   PCI_FUNCTION_NUMBER_PCH_LPC,
                   R_PCH_LPC_GPIO_BASE)
                   ) & B_PCH_LPC_GPIO_BASE_BAR;
      Data32Or = (IoRead32 ((UINTN) (GpioBase + R_PCH_GP_X_CONFIG0(18 + PortIndex))) & B_PCH_GPIO_OWN0_GPIO_USE_SEL);
      if (Data32Or == 0) {
        ClkreqPerPortSupported = TRUE;
      }
    }
    if (((RpEnableMask & (1 << PortIndex)) != 0) && ((RpHiddenMask & (1 << PortIndex)) == 0)) {
      RPBase = MmPciAddress (
                0,
                BusNumber,
                PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS,
                GetPchPcieRpfn (RootComplexBar, (UINT8) PortIndex),
                0
                );

      Data8Or   = B_PCH_PCIE_RPDCGEN_RPDLCGEN | B_PCH_PCIE_RPDCGEN_RPDBCGEN;
      Data8And  = 0xFF;
      MmioOr8 (
        RPBase + R_PCH_PCIE_RPDCGEN,
        Data8Or
        );
      SCRIPT_MEM_READ_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint8,
        (UINTN) (RPBase + R_PCH_PCIE_RPDCGEN),
        &Data8Or,   // Data to be ORed
        &Data8And   // Data to be ANDed
        );

      if (PchSeries == PchLp) {
        ///
        /// 2.3
        /// Program D28:F0~F5:E2h[6] to 1b
        ///
        Data8Or   = B_PCH_PCIE_RPPGEN_PTOTOP;
        Data8And  = (UINT8)~B_PCH_PCIE_RPPGEN_PTOTOP;
        MmioOr8 (
          RPBase + R_PCH_PCIE_RPPGEN,
          Data8Or
          );
        SCRIPT_MEM_READ_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint8,
          (UINTN) (RPBase + R_PCH_PCIE_RPPGEN),
          &Data8Or,   // Data to be ORed
          &Data8And   // Data to be ANDed
          );

        ///
        /// 2.4
        /// Program D28:F0~F5:E8h[3:2] to 10b before setting D28:F0~F5:E8h[1:0]
        ///
        Data8Or = BIT3;
        Data8And = (UINT8)~(V_PCH_PCIE_PECR1_FIELD_3);
        MmioAndThenOr8 (
          RPBase + R_PCH_PCIE_PECR1,
          Data8And,
          Data8Or
          );
      }

      Data32And = 0xFFFFFFFF;
      Data32Or  = (UINT32) BIT0;
      MmioOr32 (RPBase + R_PCH_PCIE_PECR1, Data32Or);
      SCRIPT_MEM_READ_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (RPBase + R_PCH_PCIE_PECR1),
        &Data32Or,  // Data to be ORed
        &Data32And  // Data to be ANDed
        );
      ///
      /// Step 2.6
      /// Set B0:D28:F0~F7 + 324h[5] = 1b
      ///
      MmioOr32 (RPBase + R_PCH_PCIE_PEETM, (UINT32) (BIT5));

//[-start-130916-IB08620311-add]//
      ///
      /// Denlow BIOS Enabling Weekly Dashboard ww19
      /// Denlow Server Platform Deep Package C State Communication
      ///
      /// For Denlow Server Platform, there is additional steps in Intel C220 Series Chipset Family Platform
      /// Controller Hub (PCH) initialization required to have expected CPU deep Package C State Residency.
      ///
      /// DMI/PCIexpress Programming Steps.
      /// All PCH root ports enabled should set bit3 in offset 324h to allow L1 entry of the DMI port.
      /// This is done at DXE when Root Port Initialization is performed.
      /// - Set B0:D28:F0~F7 + 324h[3] = 1b
      ///
      if (IS_PCH_LPT_LPC_DEVICE_ID_WS     (LpcDeviceId) ||
          IS_PCH_LPT_LPC_DEVICE_ID_SERVER (LpcDeviceId) ) {
        MmioOr32 (RPBase + R_PCH_PCIE_PEETM, (UINT32) (BIT5 | BIT3));
      }
//[-end-130916-IB08620311-add]//

      SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (RPBase + R_PCH_PCIE_PEETM),
        1,
        (VOID *) (UINTN) (RPBase + R_PCH_PCIE_PEETM)
        );
    }

    if (PchSeries == PchLp) {
      ///
      /// Program D28:F0~F5:E2h[5:4] to 11b prior to function disable the port
      /// Program D28:F0~F5:420h[31] to 1b prior to function disable the port
      ///
      if ((RpEnableMask & (1 << PortIndex)) == 0) {
        RPBase = MmPciAddress (
                  0,
                  BusNumber,
                  PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS,
                  GetPchPcieRpfn (RootComplexBar, (UINT8) PortIndex),
                  0
                  );
        Data8Or   = (B_PCH_PCIE_RPPGEN_LMSDOCGE | B_PCH_PCIE_RPPGEN_SEOCGE);
        Data8And  = (UINT8)~(B_PCH_PCIE_RPPGEN_LMSDOCGE | B_PCH_PCIE_RPPGEN_SEOCGE);
        MmioOr8 (
          RPBase + R_PCH_PCIE_RPPGEN,
          Data8Or
          );
        SCRIPT_MEM_READ_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint8,
          (UINTN) (RPBase + R_PCH_PCIE_RPPGEN),
          &Data8Or,   // Data to be ORed
          &Data8And   // Data to be ANDed
          );
        Data32Or   = (B_PCH_PCIE_PCIEPMECTL_FDPGE);
        Data32And  = (UINT32)~(B_PCH_PCIE_PCIEPMECTL_FDPGE);
        MmioOr32 (
          RPBase + R_PCH_PCIE_PCIEPMECTL,
          Data32Or
          );
        SCRIPT_MEM_READ_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint32,
          (UINTN) (RPBase + R_PCH_PCIE_PCIEPMECTL),
          &Data32Or,   // Data to be ORed
          &Data32And   // Data to be ANDed
           );
      }
      ///
      /// Program D28:F0~F5:420h[30:29] to 111b
      ///
      if (ClkreqPerPortSupported) {
        RPBase = MmPciAddress (
                  0,
                  BusNumber,
                  PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS,
                  GetPchPcieRpfn (RootComplexBar, (UINT8) PortIndex),
                  0
                  );
        Data32Or   = (B_PCH_PCIE_PCIEPMECTL_DLSULPGE | B_PCH_PCIE_PCIEPMECTL_DLSULDLSD);
        Data32And  = (UINT32)~(B_PCH_PCIE_PCIEPMECTL_DLSULPGE | B_PCH_PCIE_PCIEPMECTL_DLSULDLSD);
        MmioOr32 (
          RPBase + R_PCH_PCIE_PCIEPMECTL,
          Data32Or
          );
        SCRIPT_MEM_READ_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint32,
          (UINTN) (RPBase + R_PCH_PCIE_PCIEPMECTL),
          &Data32Or,   // Data to be ORed
          &Data32And   // Data to be ANDed
           );
      }
    }
  }

  if (PchSeries == PchLp) {
    ///
    /// PCH BIOS Spec Rev 0.5.0, section 19.10
    /// Step 2.4
    /// Program D28:F0,F4&F5:Reg E1h[5:2] to 1111b
    /// Program D28:F0,F4&F5:Reg E1h[7] to 1b
    ///
    for (PortIndex = 0; PortIndex < GetPchMaxPciePortNum (); PortIndex ++) {
      if ((PortIndex == 0 || PortIndex == 4 || PortIndex == 5) && !(NandPort & (0x1 << PortIndex))) {
        RPBase = MmPciAddress (
                  0,
                  BusNumber,
                  PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS,
                  GetPchPcieRpfn (RootComplexBar, (UINT8) PortIndex),
                  0
                  );
        Data8Or =
          (
            B_PCH_PCIE_RPDCGEN_LCLKREQEN |
            B_PCH_PCIE_RPDCGEN_BBCLKREQEN |
            B_PCH_PCIE_RPDCGEN_SRDLCGEN |
            B_PCH_PCIE_RPDCGEN_SRDBCGEN
          );

        MmioOr8 (RPBase + R_PCH_PCIE_RPDCGEN, Data8Or);
        SCRIPT_MEM_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint8,
          (UINTN) (RPBase + R_PCH_PCIE_RPDCGEN),
          1,
          (VOID *) (UINTN) (RPBase + R_PCH_PCIE_RPDCGEN)
          );

        ///
        /// PCH BIOS Spec Rev 0.5.0, section 19.10
        /// Step 2.5
        /// If PCIe root ports 0-3 are all disabled, set B0:D28:F0 + E2h [0] = 1b and E1h [7] = 1b.
        /// If PCIe root port 4 is disabled, set B0:D28:F4 + E2h [0] = 1b and E1h [7] = 1b.
        /// If PCIe root port 5 is disabled, set B0:D28:F5 + E2h [0] = 1b and E1h [7] = 1b.
        ///
        if (((!(RpEnableMask & 0xF)) && (PortIndex == 0)) || ((!(RpEnableMask & (0x1 << PortIndex))) && (PortIndex >= 4)))  {
          MmioOr8 ((RPBase + 0xE2), BIT0);
          SCRIPT_MEM_WRITE (
            EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
            EfiBootScriptWidthUint8,
           (UINTN) (RPBase + 0xE2),
            1,
            (VOID *) (UINTN) (RPBase + 0xE2)
            );

          MmioOr8 (RPBase + R_PCH_PCIE_RPDCGEN, B_PCH_PCIE_RPDCGEN_RPSCGEN);
          SCRIPT_MEM_WRITE (
            EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
            EfiBootScriptWidthUint8,
            (UINTN) (RPBase + R_PCH_PCIE_RPDCGEN),
            1,
            (VOID *) (UINTN) (RPBase + R_PCH_PCIE_RPDCGEN)
            );
        }
      }
    }
    ///
    /// Additional steps
    /// If all PCIe root ports are disabled, set B0:D28:F0 + E1h[6] to 1b
    ///
    if ((RpEnableMask & 0x3F) == 0) {
      RPBase = MmPciAddress (
          0,
          BusNumber,
          PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS,
          GetPchPcieRpfn(RootComplexBar, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1),
          0
          );
      MmioOr8 (
          RPBase + R_PCH_PCIE_RPDCGEN,
          B_PCH_PCIE_RPDCGEN_POCGE
          );
      SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint8,
        (UINTN) (RPBase + R_PCH_PCIE_RPDCGEN),
        1,
        (VOID *) (UINTN) (RPBase + R_PCH_PCIE_RPDCGEN)
        );
    }
  }
  if (PchSeries == PchH) {
    ///
    /// PCH BIOS Spec Rev 0.5.5, section 19.10
    /// Step 2.3
    /// Program D28:F0&F4:Reg E1h[5:2] to 1111b
    ///
    for (PortIndex = 0; PortIndex < GetPchMaxPciePortNum (); PortIndex += 4) {
      if (((RpHiddenMask & (1 << PortIndex)) == 0) && !(NandPort & (0x1 << PortIndex))) {
        RPBase = MmPciAddress (
                  0,
                  BusNumber,
                  PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS,
                  GetPchPcieRpfn (RootComplexBar, (UINT8) PortIndex),
                  0
                  );
        Data8Or =
          (
            B_PCH_PCIE_RPDCGEN_LCLKREQEN |
            B_PCH_PCIE_RPDCGEN_BBCLKREQEN |
            B_PCH_PCIE_RPDCGEN_SRDLCGEN |
            B_PCH_PCIE_RPDCGEN_SRDBCGEN
          );
        MmioOr8 (RPBase + R_PCH_PCIE_RPDCGEN, Data8Or);
        SCRIPT_MEM_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint8,
          (UINTN) (RPBase + R_PCH_PCIE_RPDCGEN),
          1,
          (VOID *) (UINTN) (RPBase + R_PCH_PCIE_RPDCGEN)
          );
        ///
        /// PCH BIOS Spec Rev 0.5.5, section 19.10
        /// Step 2.4
        /// If PCIe root ports 0-3 are all disabled, set B0:D28:F0 + E2h [0] = 1b and E1h [7] = 1b.
        /// If PCIe root ports 4-7 are all disabled, set B0:D28:F4 + E2h [0] = 1b and E1h [7] = 1b.
        ///
        if (!(RpEnableMask & (0xF << PortIndex))) {
          MmioOr8 ((RPBase + 0xE2), BIT0);
          SCRIPT_MEM_WRITE (
            EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
            EfiBootScriptWidthUint8,
            (UINTN) (RPBase + 0xE2),
            1,
            (VOID *) (UINTN) (RPBase + 0xE2)
            );

          MmioOr8 (RPBase + R_PCH_PCIE_RPDCGEN, B_PCH_PCIE_RPDCGEN_RPSCGEN);
          SCRIPT_MEM_WRITE (
            EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
            EfiBootScriptWidthUint8,
            (UINTN) (RPBase + R_PCH_PCIE_RPDCGEN),
            1,
            (VOID *) (UINTN) (RPBase + R_PCH_PCIE_RPDCGEN)
            );
        }
      }
    }
  }

  return EFI_SUCCESS;
}
