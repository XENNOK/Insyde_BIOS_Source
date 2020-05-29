/**
  This file contains an 'Intel Peripheral Driver' and uniquely        
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchxSmmHelpers.c

@brief
  This driver is responsible for the registration of child drivers
  and the abstraction of the PCH SMI sources.

**/
#include "PchSmmHelpers.h"

//
// Help handle porting bit shifts to IA-64.
//
#define BIT_ZERO  0x00000001

/**
  Publish SMI Dispatch protocols.

  @param[in] None

  @retval None
**/
VOID
PchSmmPublishDispatchProtocols (
  VOID
  )
{
  EFI_STATUS  Status;

  ///
  /// Install protocol interfaces.
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mPrivateData.InstallMultProtHandle,
                  &gEfiSmmGpiDispatchProtocolGuid,
                  &mPrivateData.Protocols[GpiType].Protocols.Gpi,
                  &gEfiSmmSxDispatchProtocolGuid,
                  &mPrivateData.Protocols[SxType].Protocols.Sx,
                  &gEfiSmmSwDispatchProtocolGuid,
                  &mPrivateData.Protocols[SwType].Protocols.Sw,
                  &gEfiSmmIchnDispatchProtocolGuid,
                  &mPrivateData.Protocols[IchnType].Protocols.Ichn,
                  &gEfiSmmIchnDispatchExProtocolGuid,
                  &mPrivateData.Protocols[IchnExType].Protocols.IchnEx,
                  &gEfiSmmPowerButtonDispatchProtocolGuid,
                  &mPrivateData.Protocols[PowerButtonType].Protocols.PowerButton,
                  &gEfiSmmPeriodicTimerDispatchProtocolGuid,
                  &mPrivateData.Protocols[PeriodicTimerType].Protocols.PeriodicTimer,
                  &gEfiSmmUsbDispatchProtocolGuid,
                  &mPrivateData.Protocols[UsbType].Protocols.Usb,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
}

/**
  Initialize bits that aren't necessarily related to an SMI source.

  @param[in] None

  @retval EFI_SUCCESS             SMI source initialization completed.
  @retval Asserts                 Global Smi Bit is not enabled successfully.
**/
EFI_STATUS
PchSmmInitHardware (
  VOID
  )
{
  EFI_STATUS  Status;

  ///
  /// Clear all SMIs
  ///
  PchSmmClearSmi ();

  Status = PchSmmEnableGlobalSmiBit ();
  ASSERT_EFI_ERROR (Status);

  ///
  /// Be *really* sure to clear all SMIs
  ///
  PchSmmClearSmi ();

  return EFI_SUCCESS;
}

/**
  Enables the PCH to generate SMIs. Note that no SMIs will be generated
  if no SMI sources are enabled. Conversely, no enabled SMI source will
  generate SMIs if SMIs are not globally enabled. This is the main
  switchbox for SMI generation.

  @param[in] None

  @retval EFI_SUCCESS             Enable Global Smi Bit completed
**/
EFI_STATUS
PchSmmEnableGlobalSmiBit (
  VOID
  )
{
  UINT32  SmiEn;

  SmiEn = IoRead32 ((UINTN) (mAcpiBaseAddr + R_PCH_SMI_EN));

  ///
  /// Set the "global smi enable" bit
  ///
  SmiEn |= B_PCH_SMI_EN_GBL_SMI;

  IoWrite32 ((UINTN) (mAcpiBaseAddr + R_PCH_SMI_EN), SmiEn);

  return EFI_SUCCESS;
}

/**
  Clears the SMI after all SMI source have been processed.
  Note that this function will not work correctly (as it is
  written) unless all SMI sources have been processed.
  A revision of this function could manually clear all SMI
  status bits to guarantee success.

  @param[in] None

  @retval EFI_SUCCESS             Clears the SMIs completed
  @retval Asserts                 EOS was not set to a 1
**/
EFI_STATUS
PchSmmClearSmi (
  VOID
  )
{
  BOOLEAN     EosSet;
  BOOLEAN     SciEn;
  UINT32      Pm1Cnt;
  UINT16      Pm1Sts;
  UINT32      Gpe0Sts;
  UINT32      Gpe0aStsLow;
  UINT32      Gpe0bStsHigh;
  UINT32      SmiSts;
  UINT32      AltGpiSmiSts;
  UINT16      DevActSts;
  UINT16      Tco1Sts;
  UINT16      Tco2Sts;
  PCH_SERIES  PchSeries;

  PchSeries    = GetPchSeries();
  Gpe0Sts      = 0;
  Gpe0aStsLow  = 0;
  Gpe0bStsHigh = 0;
  AltGpiSmiSts = 0;
  ///
  /// Determine whether an ACPI OS is present (via the SCI_EN bit)
  ///
  Pm1Cnt = IoRead32 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT));
  SciEn  = (BOOLEAN) ((Pm1Cnt & B_PCH_ACPI_PM1_CNT_SCI_EN) == B_PCH_ACPI_PM1_CNT_SCI_EN);
  if (!SciEn) {
    ///
    /// Clear any SMIs that double as SCIs (when SCI_EN==0)
    ///
    Pm1Sts        = IoRead16 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_PM1_STS));
    if (PchSeries == PchLp) {
      Gpe0Sts   = IoRead32 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_GPE0_STS_127_96));
    } else if (PchSeries == PchH) {
      Gpe0aStsLow   = IoRead32 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_GPE0a_STS));
      Gpe0bStsHigh  = IoRead32 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_GPE0b_STS));
    }

    Pm1Sts |=
      (
        B_PCH_ACPI_PM1_STS_WAK |
        B_PCH_ACPI_PM1_STS_PRBTNOR |
        B_PCH_ACPI_PM1_STS_RTC |
        B_PCH_ACPI_PM1_STS_PWRBTN |
        B_PCH_ACPI_PM1_STS_GBL |
        B_PCH_ACPI_PM1_STS_TMROF
      );

    if (PchSeries == PchLp) {
      Gpe0Sts |=
        (
          B_PCH_ACPI_GPE0_STS_127_96_PME_B0 |
          B_PCH_ACPI_GPE0_STS_127_96_PME |
          B_PCH_ACPI_GPE0_STS_127_96_BATLOW |
          B_PCH_ACPI_GPE0_STS_127_96_PCI_EXP |
          B_PCH_ACPI_GPE0_STS_127_96_RI |
          B_PCH_ACPI_GPE0_STS_127_96_SMB_WAK |
          B_PCH_ACPI_GPE0_STS_127_96_TC0SCI |
          B_PCH_ACPI_GPE0_STS_127_96_HOT_PLUG |
          B_PCH_ACPI_GPE0_STS_127_96_BATLOW |
          B_PCH_ACPI_GPE0_STS_127_96_GP27
        );
    } else if (PchSeries == PchH) {
      Gpe0aStsLow |=
        (
          B_PCH_ACPI_GPE0a_STS_PME_B0 |
          B_PCH_ACPI_GPE0a_STS_PME |
          B_PCH_ACPI_GPE0a_STS_BATLOW |
          B_PCH_ACPI_GPE0a_STS_PCI_EXP |
          B_PCH_ACPI_GPE0a_STS_RI |
          B_PCH_ACPI_GPE0a_STS_SMB_WAK |
          B_PCH_ACPI_GPE0a_STS_TC0SCI |
          B_PCH_ACPI_GPE0a_STS_HOT_PLUG |
          B_PCH_ACPI_GPE0a_STS_BATLOW
        );

      Gpe0bStsHigh |= (B_PCH_ACPI_GPE0b_STS_GP27);
    }

    IoWrite16 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_PM1_STS), (UINT16) Pm1Sts);
    if (PchSeries == PchLp) {
      IoWrite32 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_GPE0_STS_127_96), (UINT32) Gpe0Sts);
    } else if (PchSeries == PchH) {
      IoWrite32 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_GPE0a_STS), (UINT32) Gpe0aStsLow);
      IoWrite32 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_GPE0b_STS), (UINT32) Gpe0bStsHigh);
    }
  }
  ///
  /// Clear all SMIs that are unaffected by SCI_EN
  ///
  if (PchSeries == PchLp) {
    AltGpiSmiSts  = IoRead32 ((UINTN) (mGpioBaseAddr + R_PCH_LPTLP_ALT_GP_SMI_STS));
  } else if (PchSeries == PchH) {
    AltGpiSmiSts  = IoRead16 ((UINTN) (mAcpiBaseAddr + R_PCH_LPTH_ALT_GP_SMI_STS));
  }
  SmiSts        = IoRead32 ((UINTN) (mAcpiBaseAddr + R_PCH_SMI_STS));
  DevActSts     = IoRead16 ((UINTN) (mAcpiBaseAddr + R_PCH_DEVACT_STS));
  Tco1Sts       = IoRead16 ((UINTN) (mAcpiBaseAddr + PCH_TCO_BASE + R_PCH_TCO1_STS));
  Tco2Sts       = IoRead16 ((UINTN) (mAcpiBaseAddr + PCH_TCO_BASE + R_PCH_TCO2_STS));

  SmiSts |=
    (
      B_PCH_SMI_STS_SMBUS |
      B_PCH_SMI_STS_PERIODIC |
      B_PCH_SMI_STS_TCO |
      B_PCH_SMI_STS_MCSMI |
      B_PCH_SMI_STS_SWSMI_TMR |
      B_PCH_SMI_STS_APM |
      B_PCH_SMI_STS_ON_SLP_EN |
      B_PCH_SMI_STS_BIOS
    );
  AltGpiSmiSts |= 0xFFFF;
  DevActSts |=
    (
      B_PCH_DEVACT_STS_KBC |
      B_PCH_DEVACT_STS_PIRQDH |
      B_PCH_DEVACT_STS_PIRQCG |
      B_PCH_DEVACT_STS_PIRQBF |
      B_PCH_DEVACT_STS_PIRQAE
    );
  Tco1Sts |=
    (
      B_PCH_TCO1_STS_DMISERR |
      B_PCH_TCO1_STS_DMISMI |
      B_PCH_TCO1_STS_DMISCI |
      B_PCH_TCO1_STS_BIOSWR |
      B_PCH_TCO1_STS_NEWCENTURY |
      B_PCH_TCO1_STS_TIMEOUT |
      B_PCH_TCO1_STS_TCO_INT |
      B_PCH_TCO1_STS_SW_TCO_SMI
    );
  if(PchSeries == PchLp){
    IoWrite32 ((UINTN) (mGpioBaseAddr + R_PCH_LPTLP_ALT_GP_SMI_STS), AltGpiSmiSts);
  } else if (PchSeries == PchH) {
    IoWrite16 ((UINTN) (mAcpiBaseAddr + R_PCH_LPTH_ALT_GP_SMI_STS), (UINT16)AltGpiSmiSts);
  }
  IoWrite16 ((UINTN) (mAcpiBaseAddr + PCH_TCO_BASE + R_PCH_TCO1_STS), Tco1Sts);

  Tco2Sts |= B_PCH_TCO2_STS_SECOND_TO;
  IoWrite16 ((UINTN) (mAcpiBaseAddr + PCH_TCO_BASE + R_PCH_TCO2_STS), Tco2Sts);

  Tco2Sts |= (B_PCH_TCO2_STS_SMLINK_SLV_SMI | B_PCH_TCO2_STS_BOOT | B_PCH_TCO2_STS_INTRD_DET);
  IoWrite16 ((UINTN) (mAcpiBaseAddr + PCH_TCO_BASE + R_PCH_TCO2_STS), Tco2Sts);

  IoWrite32 ((UINTN) (mAcpiBaseAddr + R_PCH_SMI_STS), SmiSts);

  IoWrite16 ((UINTN) (mAcpiBaseAddr + R_PCH_DEVACT_STS), DevActSts);

  ///
  /// Try to clear the EOS bit. ASSERT on an error
  ///
  EosSet = PchSmmSetAndCheckEos ();
  ASSERT (EosSet);

  return EFI_SUCCESS;
}

/**
  Set the SMI EOS bit after all SMI source have been processed.

  @param[in] None

  @retval FALSE                   EOS was not set to a 1; this is an error
  @retval TRUE                    EOS was correctly set to a 1
**/
BOOLEAN
PchSmmSetAndCheckEos (
  VOID
  )
{
  UINT32  SmiEn;

  SmiEn = IoRead32 ((UINTN) (mAcpiBaseAddr + R_PCH_SMI_EN));

  ///
  /// Reset the PCH to generate subsequent SMIs
  ///
  SmiEn |= B_PCH_SMI_EN_EOS;

  IoWrite32 ((UINTN) (mAcpiBaseAddr + R_PCH_SMI_EN), SmiEn);

  ///
  /// Double check that the assert worked
  ///
  SmiEn = IoRead32 ((UINTN) (mAcpiBaseAddr + R_PCH_SMI_EN));

  ///
  /// Return TRUE if EOS is set correctly
  ///
  if ((SmiEn & B_PCH_SMI_EN_EOS) == 0) {
    ///
    /// EOS was not set to a 1; this is an error
    ///
    return FALSE;
  } else {
    ///
    /// EOS was correctly set to a 1
    ///
    return TRUE;
  }
}

/**
  Determine whether an ACPI OS is present (via the SCI_EN bit)

  @param[in] None

  @retval TRUE                    ACPI OS is present
  @retval FALSE                   ACPI OS is not present
**/
BOOLEAN
PchSmmGetSciEn (
  VOID
  )
{
  BOOLEAN SciEn;
  UINT32  Pm1Cnt;

  ///
  /// Determine whether an ACPI OS is present (via the SCI_EN bit)
  ///
  Pm1Cnt  = IoRead32 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT));
  SciEn   = (BOOLEAN) ((Pm1Cnt & B_PCH_ACPI_PM1_CNT_SCI_EN) == B_PCH_ACPI_PM1_CNT_SCI_EN);

  return SciEn;
}

/**
  Read a specifying bit with the register
  These may or may not need to change w/ the PCH version; they're highly IA-32 dependent, though.

  @param[in] BitDesc              The struct that includes register address, size in byte and bit number

  @retval TRUE                    The bit is enabled
  @retval FALSE                   The bit is disabled
**/
BOOLEAN
ReadBitDesc (
  const PCH_SMM_BIT_DESC  *BitDesc
  )
{
  EFI_STATUS  Status;
  UINT64      Register;
  UINT32      PciBus;
  UINT32      PciDev;
  UINT32      PciFun;
  UINT32      PciReg;
  UINTN       RegSize;
  BOOLEAN     BitWasOne;
  UINTN       ShiftCount;
  UINTN       RegisterOffset;
  UINT32      BaseAddr;

  ASSERT (BitDesc != NULL);
  ASSERT (!IS_BIT_DESC_NULL (*BitDesc));

  RegSize     = 0;
  Register    = 0;
  ShiftCount  = 0;
  BitWasOne   = FALSE;

  switch (BitDesc->Reg.Type) {

  case ACPI_ADDR_TYPE:
  case GPIO_ADDR_TYPE:
    if(BitDesc->Reg.Type == ACPI_ADDR_TYPE){
      RegisterOffset  = BitDesc->Reg.Data.acpi;
      BaseAddr        = mAcpiBaseAddr;
    } else {
      RegisterOffset  = BitDesc->Reg.Data.gpio;
      BaseAddr        = mGpioBaseAddr;
    }
    switch (BitDesc->SizeInBytes) {

    case 0:
      ///
      /// Chances are that this field didn't get initialized.
      /// Check your assignments to bit descriptions.
      ///
      ASSERT (FALSE);
      break;

    case 1:
      RegSize = SMM_IO_UINT8;
      break;

    case 2:
      RegSize = SMM_IO_UINT16;
      break;

    case 4:
      RegSize = SMM_IO_UINT32;
      break;

    case 8:
      RegSize = SMM_IO_UINT64;
      break;

    default:
      ///
      /// Unsupported or invalid register size
      ///
      ASSERT (FALSE);
      break;
    }
    ///
    /// Double check that we correctly read in the acpi base address
    ///
    ASSERT ((BaseAddr != 0x0) && ((BaseAddr & 0x1) != 0x1));

    ShiftCount      = BitDesc->Bit;
    ///
    /// As current CPU Smm Io can only support at most
    /// 32-bit read/write,if Operation is 64 bit,
    /// we do a 32 bit operation according to BitDesc->Bit
    ///
    if (RegSize == SMM_IO_UINT64) {
      RegSize = SMM_IO_UINT32;
      ///
      /// If the operation is for high 32 bits
      ///
      if (BitDesc->Bit >= 32) {
        RegisterOffset += 4;
        ShiftCount -= 32;
      }
    }

    Status = mSmst->SmmIo.Io.Read (
                              &mSmst->SmmIo,
                              RegSize,
                              BaseAddr + RegisterOffset,
                              1,
                              &Register
                              );
    ASSERT_EFI_ERROR (Status);

    if ((Register & (LShiftU64 (BIT_ZERO, ShiftCount))) != 0) {
      BitWasOne = TRUE;
    } else {
      BitWasOne = FALSE;
    }
    break;

  case MEMORY_MAPPED_IO_ADDRESS_TYPE:
    ///
    /// Read the register, and it with the bit to read
    ///
    switch (BitDesc->SizeInBytes) {
    case 1:
      Register = (UINT64) MmioRead8 ((UINTN) BitDesc->Reg.Data.Mmio);
      break;

    case 2:
      Register = (UINT64) MmioRead16 ((UINTN) BitDesc->Reg.Data.Mmio);
      break;

    case 4:
      Register = (UINT64) MmioRead32 ((UINTN) BitDesc->Reg.Data.Mmio);
      break;

    case 8:
      Register                      = (UINT64) MmioRead32 ((UINTN) BitDesc->Reg.Data.Mmio);
      *((UINT32 *) (&Register) + 1) = MmioRead32 ((UINTN) BitDesc->Reg.Data.Mmio + 4);
      break;

    default:
      ///
      /// Unsupported or invalid register size
      ///
      ASSERT (FALSE);
      break;
    }

    Register = Register & (LShiftU64 (BIT0, BitDesc->Bit));
    if (Register) {
      BitWasOne = TRUE;
    } else {
      BitWasOne = FALSE;
    }
    break;

  case PCIE_ADDR_TYPE:
    PciBus  = BitDesc->Reg.Data.pcie.Fields.Bus;
    PciDev  = BitDesc->Reg.Data.pcie.Fields.Dev;
    PciFun  = BitDesc->Reg.Data.pcie.Fields.Fnc;
    PciReg  = BitDesc->Reg.Data.pcie.Fields.Reg;
    switch (BitDesc->SizeInBytes) {

    case 0:
      ///
      /// Chances are that this field didn't get initialized.
      /// Check your assignments to bit descriptions.
      ///
      ASSERT (FALSE);
      break;

    case 1:
      Register = (UINT64) MmioRead8 (MmPciAddress (0, PciBus, PciDev, PciFun, PciReg));
      break;

    case 2:
      Register = (UINT64) MmioRead16 (MmPciAddress (0, PciBus, PciDev, PciFun, PciReg));
      break;

    case 4:
      Register = (UINT64) MmioRead32 (MmPciAddress (0, PciBus, PciDev, PciFun, PciReg));
      break;

    default:
      ///
      /// Unsupported or invalid register size
      ///
      ASSERT (FALSE);
      break;
    }

    if ((Register & (LShiftU64 (BIT_ZERO, BitDesc->Bit))) != 0) {
      BitWasOne = TRUE;
    } else {
      BitWasOne = FALSE;
    }
    break;

  default:
    ///
    /// This address type is not yet implemented
    ///
    ASSERT (FALSE);
    break;
  }

  return BitWasOne;
}

/**
  Write a specifying bit with the register

  @param[in] BitDesc              The struct that includes register address, size in byte and bit number
  @param[in] ValueToWrite         The value to be wrote
  @param[in] WriteClear           If the rest bits of the register is write clear

  @retval None
**/
VOID
WriteBitDesc (
  const PCH_SMM_BIT_DESC  *BitDesc,
  const BOOLEAN           ValueToWrite,
  const BOOLEAN           WriteClear
  )
{
  EFI_STATUS  Status;
  UINT64      Register;
  UINT64      AndVal;
  UINT64      OrVal;
  UINT32      RegSize;
  UINT32      PciBus;
  UINT32      PciDev;
  UINT32      PciFun;
  UINT32      PciReg;
  UINTN       RegisterOffset;
  UINT32      BaseAddr;

  ASSERT (BitDesc != NULL);
  ASSERT (!IS_BIT_DESC_NULL (*BitDesc));

  RegSize   = 0;
  Register  = 0;

  if (WriteClear) {
    AndVal = LShiftU64 (BIT_ZERO, BitDesc->Bit);
  } else {
    AndVal = ~(LShiftU64 (BIT_ZERO, BitDesc->Bit));
  }

  OrVal = (LShiftU64 ((UINT32) ValueToWrite, BitDesc->Bit));

  switch (BitDesc->Reg.Type) {

  case ACPI_ADDR_TYPE:
  case GPIO_ADDR_TYPE:
    if(BitDesc->Reg.Type == ACPI_ADDR_TYPE){
      RegisterOffset  = BitDesc->Reg.Data.acpi;
      BaseAddr        = mAcpiBaseAddr;
    } else {
      RegisterOffset  = BitDesc->Reg.Data.gpio;
      BaseAddr        = mGpioBaseAddr;
    }
    switch (BitDesc->SizeInBytes) {

    case 0:
      ///
      /// Chances are that this field didn't get initialized.
      /// Check your assignments to bit descriptions.
      ///
      ASSERT (FALSE);
      break;

    case 1:
      RegSize = SMM_IO_UINT8;
      break;

    case 2:
      RegSize = SMM_IO_UINT16;
      break;

    case 4:
      RegSize = SMM_IO_UINT32;
      break;

    case 8:
      RegSize = SMM_IO_UINT64;
      break;

    default:
      ///
      /// Unsupported or invalid register size
      ///
      ASSERT (FALSE);
      break;
    }
    ///
    /// Double check that we correctly read in the acpi base address
    ///
    ASSERT ((BaseAddr != 0x0) && ((BaseAddr & 0x1) != 0x1));

    ///
    /// As current CPU Smm Io can only support at most
    /// 32-bit read/write,if Operation is 64 bit,
    /// we do a 32 bit operation according to BitDesc->Bit
    ///
    if (RegSize == SMM_IO_UINT64) {
      RegSize = SMM_IO_UINT32;
      ///
      /// If the operation is for high 32 bits
      ///
      if (BitDesc->Bit >= 32) {
        RegisterOffset += 4;

        if (WriteClear) {
          AndVal = LShiftU64 (BIT_ZERO, BitDesc->Bit - 32);
        } else {
          AndVal = ~(LShiftU64 (BIT_ZERO, BitDesc->Bit - 32));
        }

        OrVal = LShiftU64 ((UINT32) ValueToWrite, BitDesc->Bit - 32);
      }
    }

    Status = mSmst->SmmIo.Io.Read (
                              &mSmst->SmmIo,
                              RegSize,
                              BaseAddr + RegisterOffset,
                              1,
                              &Register
                              );
    ASSERT_EFI_ERROR (Status);

    Register &= AndVal;
    Register |= OrVal;

    Status = mSmst->SmmIo.Io.Write (
                              &mSmst->SmmIo,
                              RegSize,
                              BaseAddr + RegisterOffset,
                              1,
                              &Register
                              );
    ASSERT_EFI_ERROR (Status);
    break;

  case MEMORY_MAPPED_IO_ADDRESS_TYPE:
    ///
    /// Read the register, or it with the bit to set, then write it back.
    ///
    switch (BitDesc->SizeInBytes) {
    case 1:
      Register = (UINT64) MmioRead8 ((UINTN) BitDesc->Reg.Data.Mmio);
      break;

    case 2:
      Register = (UINT64) MmioRead16 ((UINTN) BitDesc->Reg.Data.Mmio);
      break;

    case 4:
      Register = (UINT64) MmioRead32 ((UINTN) BitDesc->Reg.Data.Mmio);
      break;

    case 8:
      Register                      = (UINT64) MmioRead32 ((UINTN) BitDesc->Reg.Data.Mmio);
      *((UINT32 *) (&Register) + 1) = MmioRead32 ((UINTN) BitDesc->Reg.Data.Mmio + 4);
      break;

    default:
      ///
      /// Unsupported or invalid register size
      ///
      ASSERT (FALSE);
      break;
    }

    Register &= AndVal;
    Register |= OrVal;
    ///
    /// Read the register, or it with the bit to set, then write it back.
    ///
    switch (BitDesc->SizeInBytes) {
    case 1:
      MmioWrite8 ((UINTN) BitDesc->Reg.Data.Mmio, (UINT8) Register);
      break;

    case 2:
      MmioWrite16 ((UINTN) BitDesc->Reg.Data.Mmio, (UINT16) Register);
      break;

    case 4:
      MmioWrite32 ((UINTN) BitDesc->Reg.Data.Mmio, (UINT32) Register);
      break;

    case 8:
      MmioWrite32 ((UINTN) BitDesc->Reg.Data.Mmio, (UINT32) Register);
      MmioWrite32 ((UINTN) BitDesc->Reg.Data.Mmio + 4, *((UINT32 *) (&Register) + 1));
      break;

    default:
      ///
      /// Unsupported or invalid register size
      ///
      ASSERT (FALSE);
      break;
    }
    break;

  case PCIE_ADDR_TYPE:
    PciBus  = BitDesc->Reg.Data.pcie.Fields.Bus;
    PciDev  = BitDesc->Reg.Data.pcie.Fields.Dev;
    PciFun  = BitDesc->Reg.Data.pcie.Fields.Fnc;
    PciReg  = BitDesc->Reg.Data.pcie.Fields.Reg;
    switch (BitDesc->SizeInBytes) {

    case 0:
      ///
      /// Chances are that this field didn't get initialized -- check your assignments
      /// to bit descriptions.
      ///
      ASSERT (FALSE);
      break;

    case 1:
      MmioAndThenOr8 (MmPciAddress (0, PciBus, PciDev, PciFun, PciReg), (UINT8) AndVal, (UINT8) OrVal);
      break;

    case 2:
      MmioAndThenOr16 (MmPciAddress (0, PciBus, PciDev, PciFun, PciReg), (UINT16) AndVal, (UINT16) OrVal);
      break;

    case 4:
      MmioAndThenOr32 (MmPciAddress (0, PciBus, PciDev, PciFun, PciReg), (UINT32) AndVal, (UINT32) OrVal);
      break;

    default:
      ///
      /// Unsupported or invalid register size
      ///
      ASSERT (FALSE);
      break;
    }
    break;

  default:
    ///
    /// This address type is not yet implemented
    ///
    ASSERT (FALSE);
    break;
  }
}
