/** @file
  Miscellaneous PCH initialization tasks

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
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
#include "PchInit.h"

/**
  Perform miscellany PCH initialization

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar value of this PCH device
  @param[in, out] FuncDisableReg  The value of Function disable register

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ConfigureMiscItems (
  IN      DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN      UINT32                              RootComplexBar,
  IN OUT  UINT32                              *FuncDisableReg
  )
{
  UINTN   PciD31F0RegBase;
  UINT8   RegData8;
  UINT16  RegData16;
  UINT16  LpcDeviceId;
  UINTN   RPBase;
  UINT16  RpcConfig;

  DEBUG ((EFI_D_INFO, "ConfigureMiscItems() Start\n"));

  PciD31F0RegBase = MmPciAddress (0, PchPlatformPolicy->BusNumber, 31, 0, 0);
  RegData8        = 0;
  LpcDeviceId     = MmioRead16 (PciD31F0RegBase + R_PCH_LPC_DEVICE_ID);

  ///
  /// Get PCIE Root Port Configuration
  ///
  RPBase = MmPciAddress (
            0,
            PchPlatformPolicy->BusNumber,
            PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS,
            GetPchPcieRpfn( RootComplexBar, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1),
            0
            );
  RpcConfig = MmioRead16 (RPBase + R_PCH_PCIE_STRPFUSECFG);
  ///
  /// PCH BIOS Spec Rev 0.5.0, 8.13 IOSF Port Configuration and Grant Count Programming
  /// The following table shows the setting of IOSF fabric register (RCBA 0x103C [15:0]) based
  /// on PCIe port configuration. BIOS should program the register based on the table below.
  /// For Root Port 1 - 4:
  /// (Program RCBA + 103Ch[7:3] according to B0:D28:F0 + FCh[15:14])
  /// B0:D28:F0+FCh[15:14]  RCBA+103Ch[1:0]  RCBA+103Ch[3:2]  RCBA+103Ch[5:4]  RCBA+103Ch[7:6]
  ///    00b                     00b               00b             00b              00b
  ///    01b                     10b               00b             00b              00b
  ///    10b                     10b               00b             10b              00b
  ///    11b                     10b               00b             00b              00b
  ///
  switch (RpcConfig & B_PCH_PCIE_STRPFUSECFG_RPC) {
    case V_PCH_PCIE_STRPFUSECFG_RPC_2_1_1:
      RegData16 = 0x02;
      break;

    case V_PCH_PCIE_STRPFUSECFG_RPC_2_2:
      RegData16 = 0x22;
      break;

    case V_PCH_PCIE_STRPFUSECFG_RPC_4:
      RegData16 = 0x02;
      break;

    default:
      RegData16 = 0x0;
      break;
  }

  if (GetPchSeries() == PchH) {
    ///
    /// Get PCIE Root Port Configuration
    ///
    RPBase = MmPciAddress (
              0,
              PchPlatformPolicy->BusNumber,
              PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS,
              PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_5,
              0
              );
    RpcConfig = MmioRead16 (RPBase + R_PCH_PCIE_STRPFUSECFG);
    ///
    /// For Root Port 5 - 8:
    /// (Program RCBA + 103Ch[7:3] according to B0:D28:F4 + FCh[15:14])
    /// B0:D28:F4+FCh[15:14]  RCBA+103Ch[9:8]  RCBA+103Ch[11:10]  RCBA+103Ch[13:12]  RCBA+103Ch[15:14]
    ///    00b                     00b               00b             00b              00b
    ///    01b                     10b               00b             00b              00b
    ///    10b                     10b               00b             10b              00b
    ///    11b                     10b               00b             00b              00b
    ///
    switch (RpcConfig & B_PCH_PCIE_STRPFUSECFG_RPC) {
      case V_PCH_PCIE_STRPFUSECFG_RPC_2_1_1:
        RegData16 |= 0x02 << 8;
        break;

      case V_PCH_PCIE_STRPFUSECFG_RPC_2_2:
        RegData16 |= 0x22 << 8;
        break;

      case V_PCH_PCIE_STRPFUSECFG_RPC_4:
        RegData16 |= 0x02 << 8;
        break;

      default:
        RegData16 |= 0x0 << 8;
        break;
    }
  }

  MmioWrite16 (RootComplexBar + 0x103C, RegData16);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    (UINTN) (RootComplexBar + 0x103C),
    1,
    (VOID *) (UINTN) (RootComplexBar + 0x103C)
    );

  ///
  /// PCH BIOS Spec Rev 0.5.0, Section 6.2 Serial IRQs
  /// The only System BIOS requirement to use IRQs as a serial IRQ is to enable the function in D31:F0:Reg 64h[7] and
  /// select continuous or quiet mode, D31:F0:Reg 64h[6].
  /// PCH requires that the System BIOS first set the SERIRQ logic to continuous mode operation for at least one frame
  /// before switching it into quiet mode operation. This operation should be performed during the normal boot sequence
  /// as well as a resume from STR (S3).
  ///
  RegData8  = MmioRead8 (PciD31F0RegBase + R_PCH_LPC_SERIRQ_CNT) &
                (UINT8) ~(B_PCH_LPC_SERIRQ_CNT_SIRQEN | B_PCH_LPC_SERIRQ_CNT_SFPW);

  if (PchPlatformPolicy->SerialIrqConfig->SirqEnable == TRUE) {
    switch (PchPlatformPolicy->SerialIrqConfig->StartFramePulse) {
    case PchSfpw8Clk:
      RegData8 |= V_PCH_LPC_SERIRQ_CNT_SFPW_8CLK;
      break;

    case PchSfpw6Clk:
      RegData8 |= V_PCH_LPC_SERIRQ_CNT_SFPW_6CLK;
      break;

    case PchSfpw4Clk:
    default:
      RegData8 |= V_PCH_LPC_SERIRQ_CNT_SFPW_4CLK;
      break;
    }
    ///
    /// Set the SERIRQ logic to continuous mode
    ///
    RegData8 |= (UINT8) (B_PCH_LPC_SERIRQ_CNT_SIRQEN | B_PCH_LPC_SERIRQ_CNT_SIRQMD);
  }

  MmioWrite8 (PciD31F0RegBase + R_PCH_LPC_SERIRQ_CNT, RegData8);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint8,
    (UINTN) (PciD31F0RegBase + R_PCH_LPC_SERIRQ_CNT),
    1,
    (VOID *) (UINTN) (PciD31F0RegBase + R_PCH_LPC_SERIRQ_CNT)
    );

  ///
  /// PCH BIOS Spec Rev 0.5.0, Section 19.8.1 RTC Resets
  /// The PCH will set the RTC_PWR_STS bit (D31:F0:Reg A4h[2]) when the RTCRST# pin goes low.
  /// The System BIOS shouldn't rely on the RTC RAM contents when the RTC_PWR_STS bit is set.
  /// BIOS should clear this bit by writing a 0 to this bit position.
  /// This bit isn't cleared by any reset function.
  ///
  MmioAnd8 ((UINTN) (PciD31F0RegBase + 0xA4), (UINT8) (~(BIT2)));

  ///
  /// PCH BIOS Spec Rev 0.5.0, Section 5.11 Intel PCH Boot Checklist
  /// Step 8.1
  /// Always set RCBA + Offset 3418h[0] = 1b
  ///
  *FuncDisableReg |= B_PCH_RCRB_FUNC_DIS_FUNCTION_0;

  ///
  /// PCH BIOS Spec Rev 0.5.0, Section 19.1 Handling Status Registers
  /// System BIOS must set 1b to clear the following registers during power-on
  /// and resuming from Sx sleep state.
  /// - RCBA + Offset 3310h[0] = 1b
  /// - RCBA + Offset 3310h[4] = 1b, needs to be done as early as possible during PEI
  ///   Done in InstallPchInitPpi ()
  /// - RCBA + Offset 3310h[5] = 1b
  ///
  MmioOr32 (
    (UINTN) (RootComplexBar + R_PCH_RCRB_PRSTS),
    (UINT32) (B_PCH_RCRB_PRSTS_ME_WAKE_STS | B_PCH_RCRB_PRSTS_WOL_OVR_WK_STS)
    );
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (RootComplexBar + R_PCH_RCRB_PRSTS),
    1,
    (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_PRSTS)
    );

  ///
  /// PCH BIOS Spec Rev 0.5.0, Section 4.7
  /// Enabling SLP_S3# and SLP_S4# Stretch
  /// B0:D31:F0 Reg A4h[12:10] = 110b
  /// B0:D31:F0 Reg A4h[5:3] = 001b
  ///
  RegData16 = MmioRead16 (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_3) &
                (UINT16) (~(B_PCH_LPC_GEN_PMCON_SLP_S3_MAW |
                            B_PCH_LPC_GEN_PMCON_SLP_S4_MAW));

  switch (PchPlatformPolicy->MiscPmConfig->PchSlpS3MinAssert) {
  case PchSlpS360us:
    RegData16 |= V_PCH_LPC_GEN_PMCON_SLP_S3_MAW_60US;
    break;

  case PchSlpS31ms:
    RegData16 |= V_PCH_LPC_GEN_PMCON_SLP_S3_MAW_1MS;
    break;

  case PchSlpS350ms:
  default:
    RegData16 |= V_PCH_LPC_GEN_PMCON_SLP_S3_MAW_50MS;
    break;

  case PchSlpS32s:
    RegData16 |= V_PCH_LPC_GEN_PMCON_SLP_S3_MAW_2S;
    break;
  }

  switch (PchPlatformPolicy->MiscPmConfig->PchSlpS4MinAssert) {
  case PchSlpS4PchTime:
    RegData16 &= (UINT16) (~B_PCH_LPC_GEN_PMCON_SLP_S4_ASE);
    break;

  case PchSlpS41s:
    RegData16 |= V_PCH_LPC_GEN_PMCON_SLP_S4_MAW_1S | B_PCH_LPC_GEN_PMCON_SLP_S4_ASE;
    break;

  case PchSlpS42s:
    RegData16 |= V_PCH_LPC_GEN_PMCON_SLP_S4_MAW_2S | B_PCH_LPC_GEN_PMCON_SLP_S4_ASE;
    break;

  case PchSlpS43s:
    RegData16 |= V_PCH_LPC_GEN_PMCON_SLP_S4_MAW_3S | B_PCH_LPC_GEN_PMCON_SLP_S4_ASE;
    break;

  case PchSlpS44s:
  default:
    RegData16 |= V_PCH_LPC_GEN_PMCON_SLP_S4_MAW_4S | B_PCH_LPC_GEN_PMCON_SLP_S4_ASE;
    break;
  }

  if (PchPlatformPolicy->MiscPmConfig->SlpStrchSusUp == PCH_DEVICE_DISABLE) {
    RegData16 |= B_PCH_LPC_GEN_PMCON_DISABLE_SX_STRETCH;
  } else {
    RegData16 &= ~B_PCH_LPC_GEN_PMCON_DISABLE_SX_STRETCH;
  }

  MmioWrite16 (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_3, RegData16);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    (UINTN) (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_3),
    1,
    (VOID *) (UINTN) (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_3)
    );

  ///
  /// PCH BIOS Spec Rev 0.5.0, Section 6.2 Serial IRQs
  /// The only System BIOS requirement to use IRQs as a serial IRQ is to enable the function
  /// in D31:F0:Reg 64h[7] and select continuous or quiet mode, D31:F0:Reg 64h[6].
  ///
  if ((PchPlatformPolicy->SerialIrqConfig->SirqEnable == TRUE) &&
      (PchPlatformPolicy->SerialIrqConfig->SirqMode == PchQuietMode)) {
    MmioAnd8 (PciD31F0RegBase + R_PCH_LPC_SERIRQ_CNT, (UINT8)~B_PCH_LPC_SERIRQ_CNT_SIRQMD);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint8,
      (UINTN) (PciD31F0RegBase + R_PCH_LPC_SERIRQ_CNT),
      1,
      (VOID *) (UINTN) (PciD31F0RegBase + R_PCH_LPC_SERIRQ_CNT)
      );
  }

  ///
  /// For LPT-LP, if Direct Connect Interface (DCI) is enabled, set RCBA + 3F02h[0] = 1,
  /// else, set RCBA + 3F02h[0] = 0.
  /// When enabling DCI (through the enable bit), it's able to access JTAG and Run Control features
  /// in a closed chassis situation, by using the USB3 port on a Shark Bay ULT platform.
  ///
  if ((PchPlatformPolicy->Revision >= DXE_PCH_PLATFORM_POLICY_PROTOCOL_REVISION_6) &&
      (GetPchSeries() == PchLp)) {
    RegData16 = MmioRead16 (RootComplexBar + 0x3F02) & (UINT16)~BIT0;
    if (PchPlatformPolicy->MiscConfig->DciEn) {
      RegData16 |= BIT0;
    }
    MmioWrite16 (RootComplexBar + 0x3F02, RegData16);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint16,
      (UINTN) (RootComplexBar + 0x3F02),
      1,
      &RegData16
      );
  }

  DEBUG ((EFI_D_INFO, "ConfigureMiscItems() End\n"));

  return EFI_SUCCESS;
}

/**
  Configure PCH Display

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar address of this PCH device

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ConfigureDisplay (
  IN  DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN  UINT32                              RootComplexBar
  )
{
  UINT32  Data32And;
  UINT32  Data32Or;

  DEBUG ((EFI_D_INFO, "ConfigureDisplay() Start\n"));

  if (PchPlatformPolicy->DeviceEnabling->Display == PCH_DEVICE_DISABLE) {
    ///
    /// Disable PCH Display Port
    /// Step 1
    /// Set RCBA + 3424h = 0h
    ///
    MmioWrite16 ((UINTN) (RootComplexBar + R_PCH_RCRB_DISPBDF), (UINT16) 0x0);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint16,
      (UINTN) (RootComplexBar + R_PCH_RCRB_DISPBDF),
      1,
      (VOID *) (UINTN) (RootComplexBar + R_PCH_RCRB_DISPBDF)
      );
    ///
    /// Step 2
    /// Set RCBA + 3428h[0] = 0b
    ///
    Data32Or  = 0;
    Data32And = (UINT32) ~(B_PCH_RCRB_FD2_DBDFEN);
    MmioAnd32 ((UINTN) (RootComplexBar + R_PCH_RCRB_FD2), Data32And);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + R_PCH_RCRB_FD2),
      &Data32Or,  // Data to be ORed
      &Data32And  // Data to be ANDed
      );

    Data32Or  = 0;
    Data32And = (UINT32) (~BIT0);
    MmioAnd32 ((UINTN) (RootComplexBar + R_PCH_RCRB_FD2), Data32And);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (RootComplexBar + R_PCH_RCRB_FD2),
      &Data32Or,  // Data to be ORed
      &Data32And  // Data to be ANDed
      );
  }

  DEBUG ((EFI_D_INFO, "ConfigureDisplay() End\n"));

  return EFI_SUCCESS;
}