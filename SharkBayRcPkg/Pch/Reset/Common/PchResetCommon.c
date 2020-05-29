/** @file
  PCH RESET Common Library implements the Pch Reset Interface.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 2011 - 2013 Intel Corporation. All rights reserved
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
#include <PchReset.h>

/**
  Initialize an Pch Reset protocol instance.
  The function will assert in debug if PCH RCBA has not been initialized

  @param[in] PchResetInstance     Pointer to PchResetInstance to initialize

  @retval EFI_SUCCESS             The protocol instance was properly initialized
  @exception EFI_UNSUPPORTED      The PCH is not supported by this module
**/
EFI_STATUS
PchResetProtocolConstructor (
  PCH_RESET_INSTANCE          *PchResetInstance
  )
{
  ///
  /// Check if the current PCH is known and supported by this code
  ///
  if (!IsPchSupported ()) {
    DEBUG ((EFI_D_ERROR, "PCH Reset Protocol not supported due to no proper PCH LPC found!\n"));
    return EFI_UNSUPPORTED;
  }
  ///
  /// Initialize the Reset protocol instance
  ///
  PchResetInstance->Signature               = PCH_RESET_SIGNATURE;
  PchResetInstance->Handle                  = NULL;
  PchResetInstance->PchResetProtocol.Reset  = PchReset;

  ///
  /// Sanity check to ensure PCH RCBA initialization has occurred previously.
  ///
  PchResetInstance->PchRootComplexBar = PCH_RCRB_BASE;
  ASSERT (PchResetInstance->PchRootComplexBar != 0);

  return EFI_SUCCESS;
}

/**
  Execute Pch Reset from the host controller.

  @param[in] This                 Pointer to the PCH_RESET_PROTOCOL instance.
  @param[in] PchResetType         Pch Reset Types which includes ColdReset, WarmReset, ShutdownReset,
                                  PowerCycleReset, GlobalReset, GlobalResetWithEc

  @retval EFI_SUCCESS             Successfully completed.
  @retval EFI_INVALID_PARAMETER   If ResetType is invalid.
**/
EFI_STATUS
EFIAPI
PchReset (
  IN     PCH_RESET_PROTOCOL       *This,
  IN     PCH_RESET_TYPE           PchResetType
  )
{
  PCH_RESET_INSTANCE  *PchResetInstance;
  UINTN               PchRootComplexBar;
  UINT16              PmBase;
  UINT16              GpioBase;
  UINT8               OutputData;
  UINT32              Data32;
  UINT16              Data16;
  EFI_STATUS          Status;
  PCH_SERIES          PchSeries;

  PchSeries         = GetPchSeries();
  PchResetInstance  = PCH_RESET_INSTANCE_FROM_THIS (This);
  PchRootComplexBar = PchResetInstance->PchRootComplexBar;
  PmBase = PciRead16 (
            PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH,
            PCI_DEVICE_NUMBER_PCH_LPC,
            PCI_FUNCTION_NUMBER_PCH_LPC,
            R_PCH_LPC_ACPI_BASE)
            ) & B_PCH_LPC_ACPI_BASE_BAR;

  GpioBase = PciRead16 (
              PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH,
              PCI_DEVICE_NUMBER_PCH_LPC,
              PCI_FUNCTION_NUMBER_PCH_LPC,
              R_PCH_LPC_GPIO_BASE)
              ) & B_PCH_LPC_GPIO_BASE_BAR;

  switch (PchResetType) {
  case WarmReset:
    IoWrite8 ((UINTN) R_PCH_RST_CNT, (UINT8) V_PCH_RST_CNT_SOFTSTARTSTATE);
    OutputData = V_PCH_RST_CNT_SOFTRESET;
    break;

  case ColdReset:
    IoWrite8 ((UINTN) R_PCH_RST_CNT, (UINT8) V_PCH_RST_CNT_HARDSTARTSTATE);
    OutputData = V_PCH_RST_CNT_HARDRESET;
    break;

  case ShutdownReset:
    ///
    /// Firstly, ACPI decode must be enabled
    ///
    PciOr8 (
      PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH,
      PCI_DEVICE_NUMBER_PCH_LPC,
      PCI_FUNCTION_NUMBER_PCH_LPC,
      R_PCH_LPC_ACPI_CNT),
      (UINT8) (B_PCH_LPC_ACPI_CNT_ACPI_EN)
      );

    ///
    /// Then, GPE0_EN should be disabled to avoid any GPI waking up the system from S5
    ///
    if (PchSeries == PchLp) {
      IoWrite32 ((UINTN) (PmBase + R_PCH_ACPI_GPE0_EN_127_96), 0);
    } else if (PchSeries == PchH) {
      IoWrite16 ((UINTN) (PmBase + R_PCH_ACPI_GPE0a_EN), 0);
      IoWrite16 ((UINTN) (PmBase + R_PCH_ACPI_GPE0b_EN), 0);
    }

    ///
    /// Secondly, PwrSts register must be cleared
    ///
    /// Write a "1" to bit[8] of power button status register at
    /// (PM_BASE + PM1_STS_OFFSET) to clear this bit
    ///
    Data16 = B_PCH_SMI_STS_PM1_STS_REG;
    IoWrite16 ((UINTN) (PmBase + R_PCH_SMI_STS), Data16);

    ///
    /// Finally, transform system into S5 sleep state
    ///
    Data32  = IoRead32 ((UINTN) (PmBase + R_PCH_ACPI_PM1_CNT));

    Data32  = (UINT32) ((Data32 &~(B_PCH_ACPI_PM1_CNT_SLP_TYP + B_PCH_ACPI_PM1_CNT_SLP_EN)) | V_PCH_ACPI_PM1_CNT_S5);

    IoWrite32 ((UINTN) (PmBase + R_PCH_ACPI_PM1_CNT), Data32);

    Data32 = Data32 | B_PCH_ACPI_PM1_CNT_SLP_EN;

    IoWrite32 ((UINTN) (PmBase + R_PCH_ACPI_PM1_CNT), Data32);

    return EFI_SUCCESS;

  case PowerCycleReset:
  case GlobalReset:
  case GlobalResetWithEc:
    ///
    /// PCH BIOS Spec Rev 0.5.0, Section 4.6 GPIO Reset Requirement
    /// System BIOS is recommended to clear "GPIO Reset Select" registers [GP_RST_SEL1 (GPIOBASE + offset 60h),
    /// GP_RST_SEL2 (GPIOBASE + offset 64h) and GP_RST_SEL3 (GPIOBASE + offset 68h)] before issuing a hard or
    /// global reset unless specially requested by the platform designer.
    ///
    IoWrite32 ((UINTN) (GpioBase + R_PCH_GP_RST_SEL), 0);
    IoWrite32 ((UINTN) (GpioBase + R_PCH_GP_RST_SEL2), 0);
    IoWrite32 ((UINTN) (GpioBase + R_PCH_GP_RST_SEL3), 0);

    if ((PchResetType == GlobalReset) || (PchResetType == GlobalResetWithEc)) {
      PciOr32 (
        PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH,
        PCI_DEVICE_NUMBER_PCH_LPC,
        PCI_FUNCTION_NUMBER_PCH_LPC,
        R_PCH_LPC_PMIR),
        (UINT32) (B_PCH_LPC_PMIR_CF9GR)
        );
    }

    if ((PchResetType == GlobalResetWithEc) &&
        ((MmioRead32 (PchRootComplexBar + R_PCH_RCRB_DEEP_S4_POL) &
         (B_PCH_RCRB_DEEP_S4_POL_DPS4_EN_AC | B_PCH_RCRB_DEEP_S4_POL_DPS4_EN_DC)) == 0) &&
        ((MmioRead32 (PchRootComplexBar + R_PCH_RCRB_DEEP_S5_POL) &
         (B_PCH_RCRB_DEEP_S5_POL_DPS5_EN_AC | B_PCH_RCRB_DEEP_S5_POL_DPS5_EN_DC)) == 0)) {
      ///
      /// PCH BIOS Spec Rev 0.5.0, Section 4.5 Global Reset
      /// For systems with an embedded controller, System BIOS should also take these additional steps when it performs the Global Reset:
      /// CF9h Global Reset:
      ///
      if (PchSeries == PchH) {
        /// 1. Set GPIOBASE + offset 00h[30] = 1b (for non-Deep Sx enabled platforms)
        /// 2. Set GPIOBASE + offset 04h[30] = 0b (for non-Deep Sx enabled platforms)
        /// 3. Set GPIOBASE + offset 0Ch[30] = 0b (for non-Deep Sx enabled platforms)
        /// 4. Set GPIOBASE + offset 60h[30] = 1b (for non-Deep Sx enabled platforms)
        /// NOTE: For Deep Sx enabled platforms steps 1,2 and 3 should be skipped and pin should be left in native mode
        /// 5. Set CF9GR bit, D31:F0:ACh[20], issue a Global Reset through a 0xCF9 write of either 06h or 0Eh commands.
        /// Global Reset MEI Message
        /// 1. BIOS makes sure GPIO30 is left in native mode (default mode) before sending a Global Reset MEI message.
        ///
        IoOr32 ((UINTN) (GpioBase + R_PCH_GPIO_USE_SEL), (UINT32) (BIT30));
        IoAnd32 ((UINTN) (GpioBase + R_PCH_GPIO_IO_SEL), (UINT32) (~BIT30));
        IoAnd32 ((UINTN) (GpioBase + R_PCH_GPIO_LVL), (UINT32) (~BIT30));
      }

      if (PchSeries == PchLp) {
        /// 1. Set GPIOBASE + offset 1F0h[0]  = 1b (for non-Deep Sx enabled platforms)
        /// 2. Set GPIOBASE + offset 1F0h[2]  = 0b (for non-Deep Sx enabled platforms)
        /// 3. Set GPIOBASE + offset 1F0h[31] = 0b (for non-Deep Sx enabled platforms)
        /// 4. Set GPIOBASE + offset 60h[30]  = 1h (for non-Deep Sx enabled platforms)
        /// NOTE: For Deep Sx enabled platforms steps 1,2 and 3 should be skipped and pin should be left in native mode
        /// 5. Set CF9GR bit, D31:F0:ACh[20], issue a Global Reset through a 0xCF9 write of either 06h or 0Eh commands.
        /// Global Reset MEI Message
        /// 1. BIOS makes sure GPIO30 is left in native mode (default mode) before sending a Global Reset MEI message.
        ///
        IoOr32 ((UINTN) (GpioBase + R_PCH_GP_30_CONFIG0), (UINT32) (B_PCH_GPIO_OWN0_GPIO_USE_SEL));
        IoAnd32 ((UINTN) (GpioBase + R_PCH_GP_30_CONFIG0), (UINT32) (~B_PCH_GPIO_OWN0_GPIO_IO_SEL));
        IoAnd32 ((UINTN) (GpioBase + R_PCH_GP_30_CONFIG0), (UINT32) (~B_PCH_GPIO_OWN0_GPO_LVL));
      }

//[-start-130416-IB10920021-modify]//
      if ((MmioRead16 (PchRootComplexBar + R_PCH_SPI_HSFS) & B_PCH_SPI_HSFS_FDV) == B_PCH_SPI_HSFS_FDV) {
        MmioAnd32 ((PchRootComplexBar + R_PCH_SPI_FDOC), (UINT32) (~(B_PCH_SPI_FDOC_FDSS_MASK | B_PCH_SPI_FDOC_FDSI_MASK)));
        MmioOr32 ((PchRootComplexBar + R_PCH_SPI_FDOC), (UINT32) (V_PCH_SPI_FDOC_FDSS_PCHS | R_PCH_SPI_STRP10));
        if (!(MmioRead32 (PchRootComplexBar + R_PCH_SPI_FDOD) & B_PCH_SPI_STRP10_DEEP_SX_EN)) {
          IoOr32 ((UINTN) (GpioBase + R_PCH_GP_RST_SEL), (UINT32) (BIT30));
        }
      }
//[-end-130416-IB10920021-modify]//
    }

    OutputData = V_PCH_RST_CNT_FULLRESET;
    break;

  default:
    return EFI_INVALID_PARAMETER;
  }

  Status = PchResetCallback (PchResetType);

  if ((Status == EFI_SUCCESS) || (Status == EFI_NOT_FOUND)) {
    IoWrite8 ((UINTN) R_PCH_RST_CNT, OutputData);
    ///
    /// Waiting for system reset
    ///
    CpuDeadLoop ();
  }

  return Status;
}
