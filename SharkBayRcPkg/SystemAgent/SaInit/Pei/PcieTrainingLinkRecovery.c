/*++ @file
  This driver trains the PEG interface.

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
  Copyright (c) 2012 - 2014 Intel Corporation. All rights reserved
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

*/

#include "PcieTraining.h"
///===============================================
///  MdePkg/Include/
///===============================================
#include <Library/IoLib.h>

///===============================================
///  $(CHIPSET_REF_CODE_PKG)/Chipset/IntelPch/LynxPoint/Include/
///===============================================
#include <PchRegs.h>
///===============================================
///  $(CHIPSET_REF_CODE_PKG)/Chipset/IntelMch/SystemAgent/
///===============================================
#include <Ppi/PchReset.h>

#ifdef PEG_FLAG

EFI_STATUS
EnsureLinkIsHealthy (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SA_PLATFORM_POLICY_PPI      *SaPlatformPolicyPpi,
  IN SA_DATA_HOB            *SaDataHob,
  IN       EFI_PEI_STALL_PPI           *StallPpi,
  IN       PEG_PORT                    *PegPort,
  IN       UINT8                       OriginalLinkSpeed,
  IN       UINT8                       OriginalLinkWidth
  )
{
  EFI_STATUS Status;

  Status = EFI_SUCCESS;

  ///
  /// Check Link Status and Recover the Link if needed
  ///
  Status = RecoverLinkWidth (PeiServices, StallPpi, PegPort, OriginalLinkWidth);
  if (EFI_ERROR (Status)) {
    Status = RecoverLinkFailure (PeiServices, SaPlatformPolicyPpi, SaDataHob, StallPpi, PegPort, OriginalLinkSpeed, OriginalLinkWidth);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }
  Status = RecoverLinkSpeed (PeiServices, StallPpi, PegPort, OriginalLinkSpeed);
  if (EFI_ERROR (Status)) {
    Status = RecoverLinkFailure (PeiServices, SaPlatformPolicyPpi, SaDataHob, StallPpi, PegPort, OriginalLinkSpeed, OriginalLinkWidth);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }
  Status = WaitForL0 (PeiServices, StallPpi, PegPort, FALSE);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "DOWNGRADE, Link is not in L0\n"));
    Status = RetrainLink (PeiServices, StallPpi, PegPort);
    if (EFI_ERROR (Status)) {
      Status = RecoverLinkFailure (PeiServices, SaPlatformPolicyPpi, SaDataHob, StallPpi, PegPort, OriginalLinkSpeed, OriginalLinkWidth);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
  }

  return Status;
}

EFI_STATUS
WaitForL0 (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_STALL_PPI           *StallPpi,
  IN       PEG_PORT                    *PegPort,
  IN       BOOLEAN                     UseVcu
  )
{
  UINT32        MchBar;
  UINT32        PegStatus;
  UINT32        i;
  EFI_STATUS    Status;
  UINT32        VcuAddress;
  UINT8         VcuReadOp;
  UINT8         VcuWriteOp;
  CPU_FAMILY    CpuFamilyId;
  CPU_STEPPING  CpuSteppingId;

  Status         = EFI_TIMEOUT;
  MchBar         = McD0PciCfg64 (R_SA_MCHBAR) & ~BIT0;
  CpuFamilyId    = GetCpuFamily();
  CpuSteppingId  = GetCpuStepping();
  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
    VcuAddress = R_SA_VCU_REUT_PH1_PIS_ADDRESS_REV1;
    VcuReadOp  = V_SA_VCU_OPCODE_READ_CSR_REV1;
    VcuWriteOp = V_SA_VCU_OPCODE_WRITE_CSR_REV1;
  } else {
    VcuAddress = R_SA_VCU_REUT_PH1_PIS_ADDRESS_REV2;
    VcuReadOp  = V_SA_VCU_OPCODE_READ_CSR_REV2;
    VcuWriteOp = V_SA_VCU_OPCODE_WRITE_CSR_REV2;
  }

  for (i = 0; i < 100; i++) {
    if (UseVcu) {
      PegStatus = SendVcuApiSequence (MchBar, VcuAddress, VcuReadOp, 0);
      PegStatus = (PegStatus >> (PegPort->Function * B_SA_PEG_REUT_PH1_PIS_ST_STEP)) & B_SA_PEG_REUT_PH1_PIS_ST_MASK;
      if (PegStatus == 0x10) {
        Status = EFI_SUCCESS;
        break;
      }
    } else {
      PegStatus = MmPci16 (0, PegPort->Bus, PegPort->Device, PegPort->Function, R_SA_PEG_LSTS2_OFFSET);
      PegStatus = (PegStatus >> 1) & 0x1;
      if (PegStatus == 0x1) {
        Status = EFI_SUCCESS;
        break;
      }
    }
    StallPpi->Stall (PeiServices, StallPpi, STALL_ONE_MILLI_SECOND);
  }

  return Status;
}

EFI_STATUS
SetPchGpio (
  IN UINT8    GpioNumber,
  IN UINT8    Level
  )
/**
  This function sets a GPIO to a particular level.

  @param[in] GpioNumber          - PCH GPIO number
  @param[in] Level               - 0 = Low, 1 = High

  @retval EFI_SUCCESS            - Did toggle GPIO
  @retval EFI_UNSUPPORTED        - Didn't toggle GPIO
  @retval EFI_INVALID_PARAMETER  - Didn't toggle GPIO
**/
{
  UINT32      Data32;
  UINT16      LpcDeviceId;
  UINT16      GpioBase;
  UINT16      UseSelOffset;
  UINT16      IoSelOffset;
  UINT16      LvlOffset;
  UINT8       GpioBit;
  EFI_STATUS  Status;

  Level       &= 0x1;
  Status       = EFI_SUCCESS;
  GpioBase     = 0;
  UseSelOffset = 0;
  IoSelOffset  = 0;
  LvlOffset    = 0;
  GpioBit      = 0;

  LpcDeviceId = McDevFunPciCfg16 (
                  DEFAULT_PCI_BUS_NUMBER_PCH,
                  PCI_DEVICE_NUMBER_PCH_LPC,
                  PCI_FUNCTION_NUMBER_PCH_LPC,
                  R_PCH_LPC_DEVICE_ID
                  );
  if (!IS_PCH_LPT_LPC_DEVICE_ID (LpcDeviceId)) {
    Status = EFI_UNSUPPORTED;
    return Status;
  }

  GpioBase = McDevFunPciCfg16 (
               DEFAULT_PCI_BUS_NUMBER_PCH,
               PCI_DEVICE_NUMBER_PCH_LPC,
               PCI_FUNCTION_NUMBER_PCH_LPC,
               R_PCH_LPC_GPIO_BASE
               ) & B_PCH_LPC_GPIO_BASE_BAR;
  if (GpioBase == 0) {
    Status = EFI_UNSUPPORTED;
    return Status;
  }

  if (GpioNumber < 0x20) {
    UseSelOffset = R_PCH_GPIO_USE_SEL;
    IoSelOffset  = R_PCH_GPIO_IO_SEL;
    LvlOffset    = R_PCH_GPIO_LVL;
    GpioBit      = GpioNumber;
  } else if (GpioNumber < 0x40) {
    UseSelOffset = R_PCH_GPIO_USE_SEL2;
    IoSelOffset  = R_PCH_GPIO_IO_SEL2;
    LvlOffset    = R_PCH_GPIO_LVL2;
    GpioBit      = GpioNumber - 0x20;
  } else if (GpioNumber < 0x60) {
    UseSelOffset = R_PCH_GPIO_USE_SEL3;
    IoSelOffset  = R_PCH_GPIO_IO_SEL3;
    LvlOffset    = R_PCH_GPIO_LVL3;
    GpioBit      = GpioNumber - 0x40;
  } else {
    Status = EFI_INVALID_PARAMETER;
    return Status;
  }

  if (!EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "Setting GPIO%d to %x\n", GpioNumber, Level));
    IoOr32    ((UINTN) (GpioBase + UseSelOffset), (UINT32)  (1 << GpioBit));
    IoAnd32   ((UINTN) (GpioBase + IoSelOffset),  (UINT32) ~(1 << GpioBit));
    Data32   = IoRead32  ((UINTN) (GpioBase + LvlOffset));
    Data32  &= (UINT32) ~(1     << GpioBit);
    Data32  |= (UINT32)  (Level << GpioBit);
    IoWrite32 ((UINTN) (GpioBase + LvlOffset), Data32);
  }

  return Status;
}

EFI_STATUS
TogglePegSlotReset (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_STALL_PPI           *StallPpi,
  IN       SA_PLATFORM_POLICY_PPI      *SaPlatformPolicyPpi
  )
/**
  This function asserts and deasserts a GPIO that controls PERST#.
  The specific GPIO and its active level is provided by a policy.
  The GPIO minimum assertion time, T_PERST (100 usec), is defined in the PCIe CEM Specification.

  @param[in] PeiServices         - Pointer to the PEI services table
  @param[in] StallPpi            - Pointer to PEI_STALL_PPI
  @param[in] SaPlatformPolicyPpi - Pointer to SA_PLATFORM_POLICY_PPI

  @retval EFI_SUCCESS            - Did toggle GPIO
  @retval EFI_UNSUPPORTED        - Didn't toggle GPIO
  @retval EFI_INVALID_PARAMETER  - Didn't toggle GPIO
**/
{
  EFI_STATUS  Status;
  UINT8       i;

  DEBUG ((EFI_D_INFO, "Toggling PEG slot reset.\n"));
  for (i = 0; i < SA_PEG_MAX_FUN; i++) {
    MmPci16Or (0, SA_PEG_BUS_NUM, SA_PEG_DEV_NUM, i, R_SA_PEG_LCTL_OFFSET, BIT4);
  }
  Status = AssertPegSlotReset (SaPlatformPolicyPpi);
  if (!EFI_ERROR (Status)) {
    StallPpi->Stall (PeiServices, StallPpi, 100 * STALL_ONE_MICRO_SECOND);
    for (i = 0; i < SA_PEG_MAX_FUN; i++) {
      MmPci16And (0, SA_PEG_BUS_NUM, SA_PEG_DEV_NUM, i, R_SA_PEG_LCTL_OFFSET, (UINT16) ~(BIT4));
    }
    Status = DeassertPegSlotReset (SaPlatformPolicyPpi);
  } else {
    for (i = 0; i < SA_PEG_MAX_FUN; i++) {
      MmPci16And (0, SA_PEG_BUS_NUM, SA_PEG_DEV_NUM, i, R_SA_PEG_LCTL_OFFSET, (UINT16) ~(BIT4));
    }
  }

  return Status;
}

EFI_STATUS
AssertPegSlotReset (
  IN SA_PLATFORM_POLICY_PPI *SaPlatformPolicyPpi
  )
/**
  This function asserts a GPIO that controls PERST#.
  The specific GPIO and its active level is provided by a policy.
  The GPIO minimum assertion time, T_PERST (100 usec), is defined in the PCIe CEM Specification.

  @param[in] SaPlatformPolicyPpi - Pointer to SA_PLATFORM_POLICY_PPI

  @retval EFI_SUCCESS            - Did assert GPIO
  @retval EFI_UNSUPPORTED        - Didn't assert GPIO
  @retval EFI_INVALID_PARAMETER  - Didn't assert GPIO
**/
{
  EFI_STATUS  Status;
  UINT8       GpioNumber;
  UINT8       AssertLevel;

  Status = EFI_SUCCESS;

  DEBUG ((EFI_D_INFO, "Asserting PEG slot reset.\n"));

  if (!((SaPlatformPolicyPpi->Revision >= SA_PLATFORM_POLICY_PPI_REVISION_3) &&
        (SaPlatformPolicyPpi->PcieConfig->PegGpioData->GpioSupport == TRUE))) {
    Status = EFI_UNSUPPORTED;
  }

  if (!EFI_ERROR (Status)) {
    GpioNumber  = (UINT8) (SaPlatformPolicyPpi->PcieConfig->PegGpioData->SaPegReset->Value);
    AssertLevel = (UINT8) (SaPlatformPolicyPpi->PcieConfig->PegGpioData->SaPegReset->Active & 0x1);
    Status      = SetPchGpio (GpioNumber, AssertLevel);
  }

  return Status;
}

EFI_STATUS
DeassertPegSlotReset (
  IN SA_PLATFORM_POLICY_PPI *SaPlatformPolicyPpi
  )
/**
  This function deasserts a GPIO that controls PERST#.
  The specific GPIO and its active level is provided by a policy.
  The GPIO minimum assertion time, T_PERST (100 usec), is defined in the PCIe CEM Specification.

  @param[in] SaPlatformPolicyPpi - Pointer to SA_PLATFORM_POLICY_PPI

  @retval EFI_SUCCESS            - Did deassert GPIO
  @retval EFI_UNSUPPORTED        - Didn't deassert GPIO
  @retval EFI_INVALID_PARAMETER  - Didn't deassert GPIO
**/
{
  EFI_STATUS  Status;
  UINT8       GpioNumber;
  UINT8       DeassertLevel;

  Status = EFI_SUCCESS;

  DEBUG ((EFI_D_INFO, "Deasserting PEG slot reset.\n"));

  if (!((SaPlatformPolicyPpi->Revision >= SA_PLATFORM_POLICY_PPI_REVISION_3) &&
        (SaPlatformPolicyPpi->PcieConfig->PegGpioData->GpioSupport == TRUE))) {
    Status = EFI_UNSUPPORTED;
  }

  if (!EFI_ERROR (Status)) {
    GpioNumber    = (UINT8)  (SaPlatformPolicyPpi->PcieConfig->PegGpioData->SaPegReset->Value);
    DeassertLevel = (UINT8) ((SaPlatformPolicyPpi->PcieConfig->PegGpioData->SaPegReset->Active & 0x1) ^ 0x1);
    SetPchGpio (GpioNumber, DeassertLevel);
  }

  return Status;
}

EFI_STATUS
RecoverLinkFailure (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SA_PLATFORM_POLICY_PPI      *SaPlatformPolicyPpi,
  IN SA_DATA_HOB            *SaDataHob,
  IN       EFI_PEI_STALL_PPI           *StallPpi,
  IN PEG_PORT                          *PegPort,
  IN UINT8                             OriginalLinkSpeed,
  IN UINT8                             OriginalLinkWidth
  )
{
  EFI_STATUS Status;
  UINT8      CurrentLinkWidth;
  UINT8      CurrentLinkSpeed;
  UINT8      PegBus;
  UINT8      PegDev;
  UINT8      PegFunc;

  ///
  /// A platform reset should be done after presets are saved in NVRAM
  ///
  if (SaDataHob != NULL) {
    if ((SaPlatformPolicyPpi->Revision >= SA_PLATFORM_POLICY_PPI_REVISION_2) &&
        (SaPlatformPolicyPpi->PcieConfig->PegGen3ForcePresetSearch == 0)) {
      SaDataHob->PegPlatformResetRequired = TRUE;
    } else if (SaPlatformPolicyPpi->Revision < SA_PLATFORM_POLICY_PPI_REVISION_2) {
      SaDataHob->PegPlatformResetRequired = TRUE;
    }
  }

  ///
  /// Bypass phase2 and assert slot reset
  ///
  McD1PciCfg32Or (R_SA_PEG_EQCFG_OFFSET, BIT15);
  Status = TogglePegSlotReset (PeiServices, StallPpi, SaPlatformPolicyPpi);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "Error 0x%x.\n", Status));
  } else {
    DEBUG ((EFI_D_INFO, "Success.\n"));
  }
  PegBus  = PegPort->Bus;
  PegDev  = PegPort->Device;
  PegFunc = PegPort->Function;
  ///
  /// Wait for Equalization Done
  ///
  while (((MmPci32 (0, PegBus, PegDev, PegFunc, R_SA_PEG_LSTS2_OFFSET) >> 1) & 0x1) != 0x1);
  ///
  /// Wait for flow control credits exchange
  ///
  WaitForVc0Negotiation (PeiServices, StallPpi, PegBus, PegDev, PegFunc);

  CurrentLinkWidth = GetNegotiatedWidth (PegPort);
  if (CurrentLinkWidth < OriginalLinkWidth) {
    DEBUG ((EFI_D_ERROR, "Link Width DOWNGRADED!\n"));
    Status = EFI_TIMEOUT;
  }
  CurrentLinkSpeed = GetLinkSpeed (PegPort);
  if (CurrentLinkSpeed < OriginalLinkSpeed) {
    DEBUG ((EFI_D_ERROR, "Link Speed DOWNGRADED!\n"));
    Status = EFI_TIMEOUT;
  }

  return Status;
}

BOOLEAN
LinkIsDowngraded (
  IN PEG_PORT *PegPort,
  IN UINT8    OriginalLinkSpeed,
  IN UINT8    OriginalLinkWidth
  )
{
  BOOLEAN   IsDowngraded;

  IsDowngraded = FALSE;

  if (OriginalLinkSpeed != GetLinkSpeed (PegPort)) {
    DEBUG ((EFI_D_INFO, "Link speed downgrade detected\n"));
    IsDowngraded = TRUE;
  }
  if (OriginalLinkWidth != GetNegotiatedWidth (PegPort)) {
    DEBUG ((EFI_D_INFO, "Link width downgrade detected\n"));
    IsDowngraded = TRUE;
  }
  return IsDowngraded;
}

EFI_STATUS
SecondaryBusReset (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_STALL_PPI           *StallPpi,
  IN       PEG_PORT                    *PegPort
  )
{
  EFI_STATUS Status;

  Status = EFI_SUCCESS;
  DEBUG ((EFI_D_INFO, "SECONDARY BUS RESET!\n"));
  MmPci16Or (0, PegPort->Bus, PegPort->Device, PegPort->Function, R_SA_PEG_BCTRL_OFFSET, B_SA_PEG_BCTRL_SRESET_MASK);
  MmPci16And (0, PegPort->Bus, PegPort->Device, PegPort->Function, R_SA_PEG_BCTRL_OFFSET,~(B_SA_PEG_BCTRL_SRESET_MASK));
  Status = WaitForL0 (PeiServices, StallPpi, PegPort, FALSE);
  DEBUG ((EFI_D_INFO, "Reset Complete\n"));

  return Status;
}

EFI_STATUS
ResetPhyLayer (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_STALL_PPI           *StallPpi,
  IN       PEG_PORT                    *PegPort
  )
{
  EFI_STATUS        Status;
  UINT32            MchBar;
  UINT32            Data32;
  UINT32            VcuAddress;
  UINT8             VcuReadOp;
  UINT8             VcuWriteOp;
  BOOLEAN           UseVcu;
  CPU_FAMILY        CpuFamilyId;
  CPU_STEPPING      CpuSteppingId;

  UseVcu        = TRUE;
  Status        = EFI_SUCCESS;
  MchBar        = McD0PciCfg64 (R_SA_MCHBAR) &~BIT0;
  CpuFamilyId   = GetCpuFamily();
  CpuSteppingId = GetCpuStepping();

  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
    VcuAddress = R_SA_VCU_REUT_PH_CTR_ADDRESS_REV1;
    VcuReadOp  = V_SA_VCU_OPCODE_READ_CSR_REV1;
    VcuWriteOp = V_SA_VCU_OPCODE_WRITE_CSR_REV1;
  } else {
    VcuAddress = R_SA_VCU_REUT_PH_CTR_ADDRESS_REV2;
    VcuReadOp  = V_SA_VCU_OPCODE_READ_CSR_REV2;
    VcuWriteOp = V_SA_VCU_OPCODE_WRITE_CSR_REV2;
  }

  DEBUG ((EFI_D_INFO, "PHY LAYER RESET!\n"));
  if (UseVcu) {
    Data32  = SendVcuApiSequence (MchBar, VcuAddress, VcuReadOp, 0);
    Data32 &= (UINT32) ~(BIT1);
    SendVcuApiSequence (MchBar, VcuAddress, VcuWriteOp, Data32);
  } else {
    McD1PciCfg32And (R_SA_PEG_REUT_PH_CTR_OFFSET, ~(B_SA_PEG_REUT_PH_CTR_RESETMOD_MASK));
  }

  if (UseVcu) {
    Data32  = SendVcuApiSequence (MchBar, VcuAddress, VcuReadOp, 0);
    Data32 |= (UINT32) BIT0;
    SendVcuApiSequence (MchBar, VcuAddress, VcuWriteOp, Data32);
  } else {
    McD1PciCfg32Or (R_SA_PEG_REUT_PH_CTR_OFFSET, B_SA_PEG_REUT_PH_CTR_PHYRESET_MASK);
  }

  if (UseVcu) {
    Data32  = SendVcuApiSequence (MchBar, VcuAddress, VcuReadOp, 0);
    Data32 |= (UINT32) BIT13;
    SendVcuApiSequence (MchBar, VcuAddress, VcuWriteOp, Data32);
  } else {
    McD1PciCfg32Or (R_SA_PEG_REUT_PH_CTR_OFFSET, B_SA_PEG_REUT_PH_CTR_AUTOCOMP_MASK);
  }

  if (UseVcu) {
    Data32  = SendVcuApiSequence (MchBar, VcuAddress, VcuReadOp, 0);
    Data32 &= (UINT32) ~(BIT0);
    SendVcuApiSequence (MchBar, VcuAddress, VcuWriteOp, Data32);
  } else {
    McD1PciCfg32And (R_SA_PEG_REUT_PH_CTR_OFFSET, ~(B_SA_PEG_REUT_PH_CTR_PHYRESET_MASK));
  }

  Status = WaitForL0 (PeiServices, StallPpi, PegPort, TRUE);
  DEBUG ((EFI_D_INFO, "Reset Complete\n"));

  return Status;
}

EFI_STATUS
RetrainLink (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_STALL_PPI           *StallPpi,
  IN       PEG_PORT                    *PegPort
  )
{
  EFI_STATUS Status;

  ///
  /// Initiate Link Retrain
  ///
  MmPci16Or (0, PegPort->Bus, PegPort->Device, PegPort->Function, R_SA_PEG_LCTL_OFFSET, 0x20);

  Status = WaitForL0 (PeiServices, StallPpi, PegPort, FALSE);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "Link retrain FAILED!!!\n"));
  }

  return Status;
}

UINT8
GetNegotiatedWidth (
  IN PEG_PORT *PegPort
  )
{
  UINT16 Lsts;

  Lsts = MmPci16 (0, PegPort->Bus, PegPort->Device, PegPort->Function, R_SA_PEG_LSTS_OFFSET);

  return (UINT8)((Lsts >> 4) & 0x3F);
}

EFI_STATUS
RecoverLinkWidth (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_STALL_PPI           *StallPpi,
  IN       PEG_PORT                    *PegPort,
  IN       UINT8                       OriginalLinkWidth
  )
{
  EFI_STATUS Status;
  UINT8      CurrentLinkWidth;

  Status = EFI_SUCCESS;

  CurrentLinkWidth = GetNegotiatedWidth (PegPort);
  if (CurrentLinkWidth < OriginalLinkWidth) {
    DEBUG ((EFI_D_INFO, "DOWNGRADE from x%d to x%d detected\n", OriginalLinkWidth, CurrentLinkWidth));
    MmPci32AndThenOr (
      0,
      PegPort->Bus,
      PegPort->Device,
      PegPort->Function,
      R_SA_PEG_LTSSMC_OFFSET,
      B_SA_PEG_LTSSMC_WIDTH_MASK,
      OriginalLinkWidth
    );
    MmPci16Or (0, PegPort->Bus, PegPort->Device, PegPort->Function, R_SA_PEG_LCTL_OFFSET, 0x10);
    StallPpi->Stall (PeiServices, StallPpi, STALL_ONE_MICRO_SECOND);
    MmPci16And (0, PegPort->Bus, PegPort->Device, PegPort->Function, R_SA_PEG_LCTL_OFFSET, 0xFFEF);
    Status = WaitForL0 (PeiServices, StallPpi, PegPort, FALSE);
    MmPci32Or (
      0,
      PegPort->Bus,
      PegPort->Device,
      PegPort->Function,
      R_SA_PEG_LTSSMC_OFFSET,
      0x1F
    );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    CurrentLinkWidth = GetNegotiatedWidth (PegPort);
    if (CurrentLinkWidth == OriginalLinkWidth) {
      DEBUG ((EFI_D_INFO, "Width Recovery Successful\n"));
      Status = EFI_SUCCESS;
    } else {
      DEBUG ((EFI_D_INFO, "Width Recovery FAILED!\n"));
      Status = EFI_DEVICE_ERROR;
    }
  }

  return Status;
}

UINT8
GetLinkSpeed (
  IN PEG_PORT *PegPort
  )
{
  UINT16 Lsts;

  Lsts = MmPci16 (0, PegPort->Bus, PegPort->Device, PegPort->Function, R_SA_PEG_LSTS_OFFSET);

  return (UINT8)(Lsts & 0xF);
}

EFI_STATUS
RecoverLinkSpeed (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_STALL_PPI           *StallPpi,
  IN       PEG_PORT                    *PegPort,
  IN       UINT8                       OriginalLinkSpeed
  )
{
  EFI_STATUS Status;
  UINT8      CurrentLinkSpeed;

  Status = EFI_SUCCESS;

  CurrentLinkSpeed = GetLinkSpeed (PegPort);
  if (CurrentLinkSpeed < OriginalLinkSpeed) {
    DEBUG ((EFI_D_INFO, "DOWNGRADE from Gen %d to Gen %d detected\n", OriginalLinkSpeed, CurrentLinkSpeed));

    Status = RetrainLink (PeiServices, StallPpi, PegPort);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_INFO, "Link Speed Recovery FAILED!\n"));
      return Status;
    }

    CurrentLinkSpeed = GetLinkSpeed (PegPort);
    if (CurrentLinkSpeed < OriginalLinkSpeed) {
      DEBUG ((EFI_D_INFO, "Link Speed Recovery FAILED!\n"));
      Status = EFI_DEVICE_ERROR;
    } else {
      DEBUG ((EFI_D_INFO, "Link Speed Recovery Successful\n"));
    }
  }

  return Status;
}

VOID
PcieTrainingWarmReset (
  IN CONST EFI_PEI_SERVICES            **PeiServices
  )
{
  EFI_STATUS      Status;
  PCH_RESET_PPI   *PchResetPpi;

  Status = (*PeiServices)->LocatePpi (
                              PeiServices,
                              &gPchResetPpiGuid,
                              0,
                              NULL,
                              (VOID **)&PchResetPpi
                              );
  ASSERT_EFI_ERROR (Status);
  PchResetPpi->Reset (PchResetPpi, WarmReset);

  return;
}

#endif // PEG_FLAG
