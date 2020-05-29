/** @file
  PCH Platform Lib implementation.
  
;******************************************************************************
;* Copyright (c) 1983-2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
@copyright
  Copyright (c) 2004 - 2013 Intel Corporation. All rights reserved
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
#include "PchPlatformLibrary.h"
#include <Library/DebugLib.h>

/**
  Delay for at least the request number of microseconds.
  This function would be called by runtime driver, please do not use any MMIO marco here.

  @param[in] Microseconds         Number of microseconds to delay.

  @retval NONE
**/
VOID
EFIAPI
PchPmTimerStall (
  IN  UINTN   Microseconds
  )
{
  UINTN   Ticks;
  UINTN   Counts;
  UINTN   CurrentTick;
  UINTN   OriginalTick;
  UINTN   RemainingTick;
  UINT16  AcpiBaseAddr;

  if (Microseconds == 0) {
    return;
  }
  ///
  /// Please use PciRead here, it will link to MmioRead
  /// if the caller is a Runtime driver, please use PchDxeRuntimePciLibPciExpress library, refer
  /// PciExpressRead() on Library\DxeRuntimePciLibPciExpress\DxeRuntimePciLibPciExpress.c for the details.
  /// For the rest please use EdkIIGlueBasePciLibPciExpress library
  ///
  AcpiBaseAddr = PciRead16 (
                  PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH,
                  PCI_DEVICE_NUMBER_PCH_LPC,
                  PCI_FUNCTION_NUMBER_PCH_LPC,
                  R_PCH_LPC_ACPI_BASE)
                  ) & B_PCH_LPC_ACPI_BASE_BAR;

  OriginalTick  = IoRead32 ((UINTN) (AcpiBaseAddr + R_PCH_ACPI_PM1_TMR)) & B_PCH_ACPI_PM1_TMR_VAL;
  CurrentTick   = OriginalTick;

  ///
  /// The timer frequency is 3.579545 MHz, so 1 ms corresponds 3.58 clocks
  ///
  Ticks = Microseconds * 358 / 100 + OriginalTick + 1;

  ///
  /// The loops needed by timer overflow
  ///
  Counts = Ticks / V_PCH_ACPI_PM1_TMR_MAX_VAL;

  ///
  /// Remaining clocks within one loop
  ///
  RemainingTick = Ticks % V_PCH_ACPI_PM1_TMR_MAX_VAL;

  ///
  /// not intend to use TMROF_STS bit of register PM1_STS, because this adds extra
  /// one I/O operation, and maybe generate SMI
  ///
  while ((Counts != 0) || (RemainingTick > CurrentTick)) {
    CurrentTick = IoRead32 ((UINTN) (AcpiBaseAddr + R_PCH_ACPI_PM1_TMR)) & B_PCH_ACPI_PM1_TMR_VAL;
    ///
    /// Check if timer overflow
    ///
    if ((CurrentTick < OriginalTick)) {
      if (Counts != 0) {
        Counts--;
      } else {
        ///
        /// If timer overflow and Counts equ to 0, that means we already stalled more than
        /// RemainingTick, break the loop here
        ///
        break;
      }
    }

    OriginalTick = CurrentTick;
  }
}

/**
  Check whether SPI is in descriptor mode

  @param[in] PchRootComplexBar    The PCH Root Complex Bar

  @retval TRUE                    SPI is in descriptor mode
  @retval FALSE                   SPI is not in descriptor mode
**/
BOOLEAN
EFIAPI
PchIsSpiDescriptorMode (
  IN  UINTN   PchRootComplexBar
  )
{
  if ((MmioRead16 (PchRootComplexBar + R_PCH_SPI_HSFS) & B_PCH_SPI_HSFS_FDV) == B_PCH_SPI_HSFS_FDV) {
    MmioAndThenOr32 (
      PchRootComplexBar + R_PCH_SPI_FDOC,
      (UINT32) (~(B_PCH_SPI_FDOC_FDSS_MASK | B_PCH_SPI_FDOC_FDSI_MASK)),
      (UINT32) (V_PCH_SPI_FDOC_FDSS_FSDM | R_PCH_SPI_FDBAR_FLVALSIG)
      );
    if ((MmioRead32 (PchRootComplexBar + R_PCH_SPI_FDOD)) == V_PCH_SPI_FDBAR_FLVALSIG) {
      return TRUE;
    } else {
      return FALSE;
    }
  } else {
    return FALSE;
  }
}

/**
  Return Pch stepping type

  @param[in] None

  @retval PCH_STEPPING            Pch stepping type
**/
PCH_STEPPING
EFIAPI
PchStepping (
  VOID
  )
{
  UINT8   RevId;
  UINT16  LpcDeviceId;

  RevId = MmioRead8 (
            MmPciAddress (0,
            DEFAULT_PCI_BUS_NUMBER_PCH,
            PCI_DEVICE_NUMBER_PCH_LPC,
            PCI_FUNCTION_NUMBER_PCH_LPC,
            R_PCH_LPC_RID)
            );

  LpcDeviceId = MmioRead16 (
                  MmPciAddress (0,
                  DEFAULT_PCI_BUS_NUMBER_PCH,
                  PCI_DEVICE_NUMBER_PCH_LPC,
                  PCI_FUNCTION_NUMBER_PCH_LPC,
                  R_PCH_LPC_DEVICE_ID)
                  );

  if (IS_PCH_LPTH_LPC_DEVICE_ID (LpcDeviceId)) {
    switch (RevId) {
      case V_PCH_LPT_LPC_RID_2:
        return LptHB0;
        break;

      case V_PCH_LPT_LPC_RID_3:
        return LptHC0;
        break;
        
      case V_PCH_LPT_LPC_RID_4:
        return LptHC1;
        break;
        
      case V_PCH_LPT_LPC_RID_5:
        return LptHC2;
        break;

      default:
        return PchSteppingMax;
        break;
    }
  }

  if (IS_PCH_LPTLP_LPC_DEVICE_ID (LpcDeviceId)) {
    switch (RevId) {
      case V_PCH_LPT_LPC_RID_2:
        return LptLpB0;
        break;

      case V_PCH_LPT_LPC_RID_3:
        return LptLpB1;
        break;
      
      case V_PCH_LPT_LPC_RID_4:
        return LptLpB2;
        break;
        
      default:
        return PchSteppingMax;
        break;
    }
  }

  return PchSteppingMax;
}

/**
  Determine if PCH is supported

  @param[in] None

  @retval TRUE                    PCH is supported
  @retval FALSE                   PCH is not supported
**/
BOOLEAN
IsPchSupported (
  VOID
  )
{
  UINT16  LpcDeviceId;

  LpcDeviceId = MmioRead16 (
                  MmPciAddress (0,
                  DEFAULT_PCI_BUS_NUMBER_PCH,
                  PCI_DEVICE_NUMBER_PCH_LPC,
                  PCI_FUNCTION_NUMBER_PCH_LPC,
                  R_PCH_LPC_DEVICE_ID)
                  );

  ///
  /// Verify that this is a supported chipset
  ///
  if (MmioRead16 (
        MmPciAddress (0,
        DEFAULT_PCI_BUS_NUMBER_PCH,
        PCI_DEVICE_NUMBER_PCH_LPC,
        PCI_FUNCTION_NUMBER_PCH_LPC,
        R_PCH_LPC_VENDOR_ID)
        ) != V_PCH_LPC_VENDOR_ID ||
        !IS_PCH_LPT_LPC_DEVICE_ID (LpcDeviceId)) {
//[-start-120628-IB06460410-remove]//
//    DEBUG ((EFI_D_ERROR, "PCH code doesn't support the LpcDeviceId: 0x%04x!\n", LpcDeviceId));
//[-end-120628-IB06460410-remove]//
    return FALSE;
  }

  return TRUE;
}

/**
  This function can be called to enable/disable Alternate Access Mode

  @param[in] PchRootComplexBar    The PCH Root Complex Bar
  @param[in] AmeCtrl              If TRUE, enable Alternate Access Mode.
                                  If FALSE, disable Alternate Access Mode.

  @retval NONE
**/
VOID
EFIAPI
PchAlternateAccessMode (
  IN  UINTN         PchRootComplexBar,
  IN  BOOLEAN       AmeCtrl
  )
{
  UINT32  Data32Or;
  UINT32  Data32And;

  Data32Or  = 0;
  Data32And = 0xFFFFFFFF;

  if (AmeCtrl == TRUE) {
    ///
    /// Enable Alternate Access Mode
    /// Note: The RTC Index field (including the NMI mask at bit7) is write-only
    /// for normal operation and can only be read in Alt Access Mode.
    ///
    Data32Or  = (UINT32) (B_PCH_RCRB_GCS_AME);
  }

  if (AmeCtrl == FALSE) {
    ///
    /// Disable Alternate Access Mode
    ///
    Data32And = (UINT32) ~(B_PCH_RCRB_GCS_AME);
  }

  ///
  /// Program Alternate Access Mode Enable bit
  ///
  MmioAndThenOr32 (
    PchRootComplexBar + R_PCH_RCRB_GCS,
    Data32And,
    Data32Or
    );

  ///
  /// Reads back for posted write to take effect
  ///
  MmioRead32(PchRootComplexBar + R_PCH_RCRB_GCS);
}

/**
  Check whether Gbe Region is valid in SPI Flash

  @param[in] PchRootComplexBar    The PCH Root Complex Bar

  @retval TRUE                    Gbe Region is valid
  @retval FALSE                   Gbe Region is invalid
**/
BOOLEAN
EFIAPI
PchIsGbeRegionValid (
  IN  UINTN   PchRootComplexBar
  )
{
  ///
  /// If the GbE region is not used,
  /// Region Limit of Flash Region 3 (GbE) Register (SPIBAR + 60h[30:16]) must be programmed to 0000h
  /// Region Base  of Flash Region 3 (GbE) Register (SPIBAR + 60h[14:0] ) must be programmed to 7FFFh
  ///
  if (PchIsSpiDescriptorMode (PchRootComplexBar) == TRUE) {
    if (MmioRead32 (PchRootComplexBar + R_PCH_SPI_FREG3_GBE) == 0x00007FFF) {
      return FALSE;
    } else {
      return TRUE;
    }
  } else {
    return FALSE;
  }
}

/**
   Check if integrated Gbe controller present

  @param[in] None

  @retval TRUE                    Integrated Gbe present
  @retval FALSE                   Integrated Gbe not present
**/
BOOLEAN
EFIAPI
PchIsIntegratedGbePresent (
  IN  VOID
  )
{
  UINT32            Softstrap4;
  UINT32            Softstrap15;
  BOOLEAN           IntegratedGbe;

  ///
  /// Check if Gbe region is present by reading PCH straps 15 (bit 6) and 4 (bits 1:0)
  ///
  MmioAnd32 (
      PCH_RCRB_BASE + R_PCH_SPI_FDOC,
      (UINT32) (~(B_PCH_SPI_FDOC_FDSS_MASK | B_PCH_SPI_FDOC_FDSI_MASK))
    );

  MmioOr32 (
      PCH_RCRB_BASE + R_PCH_SPI_FDOC,
      (UINT32) (V_PCH_SPI_FDOC_FDSS_PCHS | R_PCH_SPI_STRP4)
      );
  
  Softstrap4 = MmioRead32 (PCH_RCRB_BASE + R_PCH_SPI_FDOD);

  MmioAnd32 (
      PCH_RCRB_BASE + R_PCH_SPI_FDOC,
      (UINT32) (~(B_PCH_SPI_FDOC_FDSS_MASK | B_PCH_SPI_FDOC_FDSI_MASK))
    );

  MmioOr32 (
      PCH_RCRB_BASE + R_PCH_SPI_FDOC,
      (UINT32) (V_PCH_SPI_FDOC_FDSS_PCHS | R_PCH_SPI_STRP15)
      );
  
  Softstrap15 = MmioRead32 (PCH_RCRB_BASE + R_PCH_SPI_FDOD);  

  ///
  /// Both values have to be non-zero if integrated phy present
  ///
  IntegratedGbe = !!(Softstrap4 & B_PCH_SPI_STRP4_PHYCON) && !!(Softstrap15 & B_PCH_SPI_STRP15_IWL_EN);

  return IntegratedGbe;
}

/**
  Return Pch Series

  @param[in] None

  @retval PCH_SERIES            Pch Series
**/
PCH_SERIES
EFIAPI
GetPchSeries (
  VOID
  )
{
  UINT16  LpcDeviceId;
  UINT32  PchSeries;

  LpcDeviceId = MmioRead16 (
                  MmPciAddress (0,
                  DEFAULT_PCI_BUS_NUMBER_PCH,
                  PCI_DEVICE_NUMBER_PCH_LPC,
                  PCI_FUNCTION_NUMBER_PCH_LPC,
                  R_PCH_LPC_DEVICE_ID)
                  );

  if (IS_PCH_LPTH_LPC_DEVICE_ID (LpcDeviceId)) {
    PchSeries = PchH;
  } else if (IS_PCH_LPTLP_LPC_DEVICE_ID (LpcDeviceId)) {
    PchSeries = PchLp;
  } else {
    PchSeries = PchUnknownSeries;
//[-start-120628-IB06460410-remove]//
//    DEBUG ((EFI_D_ERROR, "Unsupported PCH SKU, LpcDeviceId: 0x%04x!\n", LpcDeviceId));
//    ASSERT (FALSE);
//[-end-120628-IB06460410-remove]//
  }

  return PchSeries;
}

/**
  Get Pch Maximum Pcie Root Port Number

  @param[in] None

  @retval Pch Maximum Pcie Root Port Number
**/
UINT8
EFIAPI
GetPchMaxPciePortNum (
  VOID
  )
{
  PCH_SERIES  PchSeries;

  PchSeries = GetPchSeries();
  switch (PchSeries) {
    case PchLp:
      return LPTLP_PCIE_MAX_ROOT_PORTS;

    case PchH:
      return LPTH_PCIE_MAX_ROOT_PORTS;

    default:
      return 0;
  }
}

/**
  Get Pch Maximum Sata Port Number

  @param[in] None

  @retval Pch Maximum Sata Port Number
**/
UINT8
EFIAPI
GetPchMaxSataPortNum (
  VOID
  )
{
  PCH_SERIES  PchSeries;

  PchSeries = GetPchSeries();
  switch (PchSeries) {
    case PchLp:
      return LPTLP_AHCI_MAX_PORTS;

    case PchH:
      return LPTH_AHCI_MAX_PORTS;

    default:
      return 0;
  }
}

/**
  Get Pch Maximum Sata Controller Number

  @param[in] None

  @retval Pch Maximum Sata Controller Number
**/
UINT8
EFIAPI
GetPchMaxSataControllerNum (
  VOID
  )
{
  PCH_SERIES  PchSeries;

  PchSeries = GetPchSeries();
  switch (PchSeries) {
    case PchLp:
      return LPTLP_SATA_MAX_CONTROLLERS;

    case PchH:
      return LPTH_SATA_MAX_CONTROLLERS;

    default:
      return 0;
  }
}

/**
  Get Pch Maximum Usb Port Number of EHCI Controller

  @param[in] None

  @retval Pch Maximum Usb Port Number of EHCI Controller
**/
UINT8
EFIAPI
GetPchEhciMaxUsbPortNum (
  VOID
  )
{
  PCH_SERIES  PchSeries;

  PchSeries = GetPchSeries();
  switch (PchSeries) {
    case PchLp:
      return LPTLP_EHCI_MAX_PORTS;

    case PchH:
      return LPTH_EHCI_MAX_PORTS;

    default:
      return 0;
  }
}

/**
  Get Pch Maximum EHCI Controller Number

  @param[in] None

  @retval Pch Maximum EHCI Controller Number
**/
UINT8
EFIAPI
GetPchEhciMaxControllerNum (
  VOID
  )
{
  PCH_SERIES  PchSeries;

  PchSeries = GetPchSeries();
  switch (PchSeries) {
    case PchLp:
      return LPTLP_EHCI_MAX_CONTROLLERS;

    case PchH:
      return LPTH_EHCI_MAX_CONTROLLERS;

    default:
      return 0;
  }
}

/**
  Get Pch Usb Maximum Physical Port Number

  @param[in] None

  @retval Pch Usb Maximum Physical Port Number
**/
UINT8
EFIAPI
GetPchUsbMaxPhysicalPortNum (
  VOID
  )
{
  PCH_SERIES  PchSeries;

  PchSeries = GetPchSeries();
  switch (PchSeries) {
    case PchLp:
      return LPTLP_USB_MAX_PHYSICAL_PORTS;

    case PchH:
      return LPTH_USB_MAX_PHYSICAL_PORTS;

    default:
      return 0;
  }
}

/**
  Get Pch Maximum Usb2 Port Number of XHCI Controller

  @param[in] None

  @retval Pch Maximum Usb2 Port Number of XHCI Controller
**/
UINT8
EFIAPI
GetPchXhciMaxUsb2PortNum (
  VOID
  )
{
  PCH_SERIES  PchSeries;

  PchSeries = GetPchSeries();
  switch (PchSeries) {
    case PchLp:
      return LPTLP_XHCI_MAX_USB2_PORTS;

    case PchH:
      return LPTH_XHCI_MAX_USB2_PORTS;

    default:
      return 0;
  }
}

/**
  Get Pch Maximum Usb3 Port Number of XHCI Controller

  @param[in] None

  @retval Pch Maximum Usb3 Port Number of XHCI Controller
**/
UINT8
EFIAPI
GetPchXhciMaxUsb3PortNum (
  VOID
  )
{
  PCH_SERIES  PchSeries;

  PchSeries = GetPchSeries();
  switch (PchSeries) {
    case PchLp:
      return LPTLP_XHCI_MAX_USB3_PORTS;

    case PchH:
      return LPTH_XHCI_MAX_USB3_PORTS;

    default:
      return 0;
  }
}

/**
  Query PCH to determine the Pm Status

  @param[in] PmStatus - The Pch Pm Status to be probed

  @retval Return TRUE if Status querried is Valid or FALSE if otherwise
**/
BOOLEAN
GetPchPmStatus (
  PCH_PM_STATUS PmStatus
  )
{
  UINT16 PmCon2;
  UINT16 PmCon3;

  PmCon2 = MmioRead16 (
             MmPciAddress (0,
               DEFAULT_PCI_BUS_NUMBER_PCH,
               PCI_DEVICE_NUMBER_PCH_LPC,
               PCI_FUNCTION_NUMBER_PCH_LPC,
               R_PCH_LPC_GEN_PMCON_2
             )
           );
  PmCon3 = MmioRead16 (
             MmPciAddress (0,
               DEFAULT_PCI_BUS_NUMBER_PCH,
               PCI_DEVICE_NUMBER_PCH_LPC,
               PCI_FUNCTION_NUMBER_PCH_LPC,
               R_PCH_LPC_GEN_PMCON_3
             )
           );

  switch(PmStatus){
    case WarmBoot:
      if (PmCon2 & B_PCH_LPC_GEN_PMCON_MEM_SR) {
        return TRUE;
      }
      break;

    case PwrFlr:
      if (PmCon3 & B_PCH_LPC_GEN_PMCON_PWR_FLR) {
        return TRUE;
      }
      break;

    case PwrFlrSys:
      if (PmCon2 & B_PCH_LPC_GEN_PMCON_SYSPWR_FLR) {
        return TRUE;
      }
      break;

    case PwrFlrPch:
      if (PmCon2 & B_PCH_LPC_GEN_PMCON_PWROK_FLR) {
        return TRUE;
      }
      break;

    case ColdBoot:
      ///
      /// Check following conditions for cold boot.
      /// (1)GEN_PMCON_2 (0:31:0 offset 0A2) bit[5] = 0
      /// (2)GEN_PMCON_2 (0:31:0 offset 0A2) bit[1] = 1
      /// (3)GEN_PMCON_3 (0:31:0 offset 0A4) bit[1] = 1
      ///
      if ((PmCon3 & B_PCH_LPC_GEN_PMCON_PWR_FLR) &&
          (PmCon2 & B_PCH_LPC_GEN_PMCON_SYSPWR_FLR) &&
          (!(PmCon2 & B_PCH_LPC_GEN_PMCON_MEM_SR))) {
        return TRUE;
      }
      break;

    default:
      break;
  }

  return FALSE;
}

/**
  Get Pch Pcie Root Port Function Number by Root Port Number

  @param[in] UINT8 Root Port Number (start from 0)

  @retval Pch Pcie Root Port Function Number
**/
UINT8
EFIAPI
GetPchPcieRpfn (
  IN  UINTN   PchRootComplexBar,
  IN  UINT8   RpNumber
  )
{
  return ((MmioRead32(PchRootComplexBar + R_PCH_RCRB_RPFN) >> (RpNumber * S_PCH_RCRB_PRFN_RP_FIELD)) & B_PCH_RCRB_RPFN_RP1FN);
}

/**
  Get Pch Pcie Root Port Number by Root Port Function Number

  @param[in] UINT8 Root Port Function Number

  @retval Pch Pcie Root Port Number
  @retval 0xFF  No Root Port Number found
**/
UINT8
EFIAPI
GetPchPcieRpNumber (
  IN  UINTN   PchRootComplexBar,
  IN  UINT8   Rpfn
  )
{
  UINT8  PortIndex;
  for (PortIndex = 0; PortIndex < GetPchMaxPciePortNum(); PortIndex++) {
    if (((MmioRead32(PchRootComplexBar + R_PCH_RCRB_RPFN) >> (PortIndex * S_PCH_RCRB_PRFN_RP_FIELD)) & B_PCH_RCRB_RPFN_RP1FN) == Rpfn) {
      return PortIndex;
    }
  }
  
  //Assert if function number not found for a root port
  ASSERT (FALSE);
  return 0xff;
}


/**
  Returns  GbE over PCIe port number.

  @return  Root port number (0-based)
  @retval  0xff
**/
UINTN
PchGetGbePortNumber (
  VOID 
  )
{
  UINT32            Softstrap9;
  UINT32            GbePortSel;

  ///
  /// Check if Intel PHY Over PCI Express Enable by reading PCH straps 9 (bit 11)
  ///
  MmioAnd32 (
    PCH_RCRB_BASE + R_PCH_SPI_FDOC,
    (UINT32) (~(B_PCH_SPI_FDOC_FDSS_MASK | B_PCH_SPI_FDOC_FDSI_MASK))
  );

  MmioOr32 (
    PCH_RCRB_BASE + R_PCH_SPI_FDOC,
    (UINT32) (V_PCH_SPI_FDOC_FDSS_PCHS | R_PCH_SPI_STRP9)
    );

  Softstrap9 = MmioRead32 (PCH_RCRB_BASE + R_PCH_SPI_FDOD);
  
  ///
  /// If Intel PHY Over PCI Express Enable bit is set, return GbE port number
  ///
  if (Softstrap9 & B_PCH_SPI_STRP9_GBE_PCIE_EN) {
    GbePortSel = (Softstrap9 & B_PCH_SPI_STRP9_GBE_PCIE_PSC) >> N_PCH_SPI_STRP9_GBE_PCIE_PSC;  
    DEBUG ((EFI_D_INFO, "GbePortSel=%d\n", GbePortSel));
    if (GetPchSeries () == PchLp) {
      switch (GbePortSel) {
      case 0: return 2; // Root Port 3
      case 1: return 3; // Root Port 4
      case 2: // Root Port 5, lane 0
      case 3: // Root Port 5, lane 1
      case 4: // Root Port 5, lane 2
      case 5: // Root Port 5, lane 3
        return 4;
      default:
        ASSERT (FALSE);
      }
    } else {
      return GbePortSel;
    }
  }
  
  return 0xff;
}
