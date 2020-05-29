/** @file

  PEIM to initialize both IGD, PEG and PCI graphics card.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "GraphicsInit.h"

#define IGD_ENABLE  1
#define IGD_DISABLE 0

/**
  GraphicsInit: Initialize the IGD if no other external graphics is present

  @param[in] PeiServices          - Pointer to the PEI services table
  @param[in] SaPlatformPolicyPpi  - SaPlatformPolicyPpi to access the GtConfig related information
**/
VOID
GraphicsInit (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SA_PLATFORM_POLICY_PPI      *SaPlatformPolicyPpi
  )
{
  DISPLAY_DEVICE  PrimaryDisplay;
  UINT8           GMSData;
  UINT32          PciMmioLength;
  UINT32          PchPcieMmioLength;
  UINT32          PegMmioLength;
  UINT32          IGfxMmioLength;
  UINT32          TotalMmioLength;
  BOOLEAN         IGfxSupported;
  UINT32          LoGTBaseAddress;
  UINT32          HiGTBaseAddress;
  UINT32          RegOffset;
  UINT32          Data32And;
  UINT32          Data32Or;
  UINT32          Data32Mask;
  UINT32          Result;
  EFI_PEI_STALL_PPI   *StallPpi;
  EFI_STATUS      Status;
  UINT64          GttMmAdr;
  UINTN           MchBarBase;
  CPU_FAMILY      CpuFamilyId;

  PciMmioLength     = 0;
  PchPcieMmioLength = 0;
  PegMmioLength     = 0;
  IGfxMmioLength    = 0;
  GttMmAdr          = 0;
  MchBarBase        = McD0PciCfg32 (0x48) &~BIT0;
  CpuFamilyId       = GetCpuFamily();

  ///
  /// Set the VGA Decode bits to a good known starting point where both PEG and
  /// IGD VGA Decode Bits are both disabled.
  ///
  McD0PciCfg16Or (R_SA_GGC, B_SA_GGC_IVD_MASK);
  PrimaryDisplay = IGD;

  ///
  /// Check if IGfx is supported
  ///
  IGfxSupported = (BOOLEAN) (McD2PciCfg16 (R_SA_IGD_VID) != 0xFFFF);

  ///
  /// Check external VGA devices
  ///
  CheckOffboardPcieVga (PeiServices, &PchPcieMmioLength, &PrimaryDisplay);
  CheckAndInitializePegVga (
    PeiServices,
    SaPlatformPolicyPpi->PcieConfig->AlwaysEnablePeg,
    &PrimaryDisplay,
    SaPlatformPolicyPpi->GtConfig->PrimaryDisplay,
    &PegMmioLength
    );

  ///
  /// Temporarily program GttMmAdr
  ///
  GttMmAdr = SaPlatformPolicyPpi->GtConfig->GttMmAdr;
  
  ///
  /// Program GttMmAdr 
  ///
  LoGTBaseAddress = (UINT32) (GttMmAdr & 0xFFFFFFFF);
  HiGTBaseAddress = (UINT32) RShiftU64 ((GttMmAdr & 0xFFFFFFFF00000000), 32);
  McD2PciCfg32 (R_SA_IGD_GTTMMADR)     = LoGTBaseAddress | BIT2;
  McD2PciCfg32 (R_SA_IGD_GTTMMADR + 4) = HiGTBaseAddress;

  ///
  /// Enable Bus Master and Memory access on 0:2:0
  ///
  McD2PciCfg16Or (R_SA_IGD_CMD, (BIT2 | BIT1));

  ///
  /// If primary display device is IGD or no other display detected then enable IGD
  ///
  if (IGfxSupported &&
      (
        (
        ((PrimaryDisplay == IGD) || (SaPlatformPolicyPpi->GtConfig->PrimaryDisplay == IGD)) &&
      (SaPlatformPolicyPpi->GtConfig->InternalGraphics != IGD_DISABLE)
    ) || (SaPlatformPolicyPpi->GtConfig->InternalGraphics == IGD_ENABLE)
    )
      ) {

    DEBUG ((EFI_D_INFO, "IGD enabled.\n"));

    ///
    /// Program GFX Memory by setting D0.F0.R 050h [7:3]
    ///
    McD0PciCfg16And (R_SA_GGC, ~(B_SA_GGC_GMS_MASK));

    GMSData = SaPlatformPolicyPpi->GtConfig->IgdDvmt50PreAlloc;

    McD0PciCfg8Or (R_SA_GGC, (GMSData << N_SA_GGC_GMS_OFFSET));

    ///
    /// Program Graphics GTT Memory D0:F0:R50h[9:8] = 10b => 2MB of GTT
    ///
    McD0PciCfg16AndThenOr (R_SA_GGC, ~(B_SA_GGC_GGMS_MASK), 1 << (N_SA_GGC_GGMS_OFFSET + 1));

    ///
    /// Set register D2.F0.R 062h [2:1] = `01b' to set a 256MByte aperture.
    /// This must be done before Device 2 registers are enumerated.
    ///
    if (SaPlatformPolicyPpi->GtConfig->ApertureSize == APERTURE_SIZE_128MB) {
      McD2PciCfg8And (R_SA_IGD_MSAC_OFFSET, ~(BIT2 + BIT1));
    } else if (SaPlatformPolicyPpi->GtConfig->ApertureSize == APERTURE_SIZE_256MB) {
      McD2PciCfg8AndThenOr (R_SA_IGD_MSAC_OFFSET, ~(BIT2 + BIT1), BIT1);
    } else {
      McD2PciCfg8Or (R_SA_IGD_MSAC_OFFSET, (BIT2 + BIT1));
    }
    ///
    /// Enable IGD VGA Decode.  This is needed so the Class Code will
    /// be correct for the IGD Device when determining which device
    /// should be primary.  If disabled, IGD will show up as a non VGA device.
    ///
    if ((SaPlatformPolicyPpi->GtConfig->PrimaryDisplay != IGD) && (PrimaryDisplay != IGD)) {
      ///
      /// If IGD is forced to be enabled, but is a secondary display, disable IGD VGA Decode
      ///
      McD0PciCfg16Or (R_SA_GGC, B_SA_GGC_IVD_MASK);
      DEBUG ((EFI_D_INFO, "IGD VGA Decode is disabled because it's not a primary display.\n"));
    } else {
      McD0PciCfg16And (R_SA_GGC, ~(B_SA_GGC_IVD_MASK));
    }

    FindPciDeviceMmioLength (0, 2, 0, &IGfxMmioLength);

    if ((SaPlatformPolicyPpi->Revision >= SA_PLATFORM_POLICY_PPI_REVISION_12) && (SaPlatformPolicyPpi->GtConfig->PanelPowerEnable == 1)) {
      ///
      /// Panel Enable VDD bit
      ///
      Mmio32Or (GttMmAdr, 0xc7204, 0x8);
    }
  } else {

    Status = (*PeiServices)->LocatePpi (PeiServices, &gEfiPeiStallPpiGuid, 0, NULL, (VOID **)&StallPpi);
    ASSERT_EFI_ERROR (Status);

    if (CpuFamilyId == EnumCpuHswUlt) {
      ///
      /// For HSW ULT, Disable LCPLL
      /// Set LCPLL_CTL (GTTMMADR Offset 0x130040) PLL_disable (Bit 31) to 1b to disable LCPLL.
      /// Poll for LCPLL_CTL PLL_lock (Bit 30) = 0b to indicate LCPLL lost lock.Timeout and continue after 1 ms.
      /// Set D_COMP COMP_DISABLE (MCHBAR offset 0x5F0C Bit 0)to 1b.
      /// Wait > 100 ns for write to complete.
      /// Poll for D_COMP RCOMP_IN_PROGRESS Bit 9 = 0b.Timeout and continue after 1 ms.
      ///

      RegOffset = 0x130040;
      Data32And = 0xFFFFFFFF;
      Data32Or  = BIT31;
      Mmio32AndThenOr (GttMmAdr, RegOffset, Data32And, Data32Or);

      if (IGfxSupported) {
        Data32Mask = BIT30;
        Result     = 0;
        PollGtReady (PeiServices, StallPpi, GttMmAdr, RegOffset, Data32Mask, Result);
      }

      RegOffset = 0x5F0C;
      Data32And = 0xFFFFFFFF;
      Data32Or  = BIT0;
      Mmio32AndThenOr (MchBarBase, RegOffset, Data32And, Data32Or);

      ///
      /// 1uSec wait
      ///
      StallPpi->Stall (PeiServices, StallPpi, 1);

      if (IGfxSupported) {
        Data32Mask = BIT9;
        Result     = 0;
        PollGtReady (PeiServices, StallPpi, MchBarBase, RegOffset, Data32Mask, Result);
      }
    }

    DEBUG ((EFI_D_INFO, "Disable IGD Device.\n"));
    ///
    /// Disable IGD device
    ///
    /// Register D0:F0 Offset 50h [7:3] = '00000b'.
    /// This prevents UMA memory from being pre-allocated to IGD.
    /// Set D0:F0 Offset 50h [9:8] = '00b'.
    /// GTT Graphics Memory Size to 0
    /// Set VGA Disable (IVD) in D0:F0 Offset 50h [1] = '1b'.
    ///
    McD0PciCfg16AndThenOr (R_SA_GGC, ~(B_SA_GGC_GGMS_MASK | B_SA_GGC_GMS_MASK), B_SA_GGC_IVD_MASK);
    SaPlatformPolicyPpi->GtConfig->GttSize            = 0;
    SaPlatformPolicyPpi->GtConfig->IgdDvmt50PreAlloc  = 0;

    ///
    /// Disable IGD. D0.F0.R 054h [4] = '0b'.
    ///
    McD0PciCfg8And (R_SA_DEVEN, ~(B_SA_DEVEN_D2EN_MASK));
  }

  TotalMmioLength = PciMmioLength + PchPcieMmioLength + PegMmioLength + IGfxMmioLength;
  DEBUG ((EFI_D_INFO, "TotalMmioLength:   0x%08X bytes\n", TotalMmioLength));

  ///
  /// Disable Bus Master and Memory access on 0:2:0 and clear GTTMMADR
  ///
  McD2PciCfg16And (R_SA_IGD_CMD, ~(BIT2 | BIT1));
  McD2PciCfg32And (R_SA_IGD_GTTMMADR, 0x0);
  McD2PciCfg32And (R_SA_IGD_GTTMMADR + 0x4, 0x0);

  ///
  /// Determine MMIO Size for Dynamic Tolud
  ///
//[-start-130709-IB05400426-modify]//
  //
  // If we set "BestTolud" variable, it means we use our method to find out a best
  // TOLUD value. Skip this to use our calculated TOLUD value.
  //
  if ((SaPlatformPolicyPpi->MemConfig->MaxTolud == 0x00) && (!PcdGetBool (PcdBestToludExist))) {
//[-end-130709-IB05400426-modify]//
    ///
    /// if total MMIO need 1GB or over
    ///
    if (TotalMmioLength >= 0x40000000) {
      SaPlatformPolicyPpi->GtConfig->MmioSize = 0x800;
    }
    ///
    /// if total MMIO need 728MB~1GB
    ///
    else if (TotalMmioLength >= 0x30000000) {
      SaPlatformPolicyPpi->GtConfig->MmioSize = 0x700;
    }
    ///
    /// if total MMIO need 512MB~728MB
    ///
    else if (TotalMmioLength >= 0x20000000) {
      SaPlatformPolicyPpi->GtConfig->MmioSize = 0x600;
    }
    ///
    /// if total MMIO need 256MB~512MB
    ///
    else if (TotalMmioLength >= 0x10000000) {
      SaPlatformPolicyPpi->GtConfig->MmioSize = 0x500;
    }
    ///
    /// if total MMIO need less than 256MB
    ///
    else {
      SaPlatformPolicyPpi->GtConfig->MmioSize = 0x400;
    }
  }

//[-start-130709-IB05400426-add]//
  //
  // Save adjusted MMIO size value so that DXE phase can use this value to adjust
  // TOLUD dynamically
  //
  PcdSet16 (PcdAdjustTolud, (UINT16)(0x0fff - SaPlatformPolicyPpi->GtConfig->MmioSize) + 1);
//[-end-130709-IB05400426-add]//

}

/**
  CheckAndInitializePegVga:  Check if PEG card is present and configure accordingly

  @param[in] PeiServices             - Pointer to the PEI services table
  @param[in] AlwaysEnablePeg         - 0 - Peg is not always enabled.
  @param[in, out] PrimaryDisplay     - Primary Display - default is IGD
  @param[in] PrimaryDisplaySelection - Primary display selection from BIOS Setup
  @param[in, out] PegMmioLength      - Total PEG MMIO length on all PEG ports
**/
VOID
CheckAndInitializePegVga (
  IN CONST EFI_PEI_SERVICES                 **PeiServices,
  IN     UINT8                              AlwaysEnablePeg,
  IN OUT DISPLAY_DEVICE                     *PrimaryDisplay,
  IN     UINT8                              PrimaryDisplaySelection,
  IN OUT UINT32                             *PegMmioLength
  )
{
  UINT8     ClassCode;
  BOOLEAN   IgdPresent;
  UINT8     PegBus;
  UINT8     PegDev;
  UINT8     PegFunc;
  UINT16    PegDevenReg;
  UINT8     PegComplete;
  UINT16    PegEnable;
  BOOLEAN   CardDetect;
  UINT32    MmioLength;
  UINT8     Func;
  UINT8     MaxFunction;
  UINT8     HeaderType;
  UINT8     PegVgaFunc;

  PEG_DEVEN PegDevenTable[] = {
    ///
    ///  Bus, Device, Function, DevenMask
    ///
    {
      SA_PEG_BUS_NUM,
      SA_PEG10_DEV_NUM,
      SA_PEG10_FUN_NUM,
      BIT3
    },
    {
      SA_PEG_BUS_NUM,
      SA_PEG11_DEV_NUM,
      SA_PEG11_FUN_NUM,
      BIT2
    },
    {
      SA_PEG_BUS_NUM,
      SA_PEG12_DEV_NUM,
      SA_PEG12_FUN_NUM,
      BIT1
    }
  };

  MmioLength = 0;
  CardDetect = FALSE;
  PegVgaFunc = 0xFF;

  ///
  /// Read the DEVEN register for PEG0/1/2 controllers configuration
  ///
  PegDevenReg = MmPci16 (0, SA_MC_BUS, 0, 0, R_SA_DEVEN) & (BIT3 + BIT2 + BIT1);

  ///
  /// If IGD is disabled
  /// or not present IgdPresent is set to false
  ///
  if (McD2PciCfg16 (PCI_VID) == 0xFFFF) {
    IgdPresent = FALSE;
  } else {
    IgdPresent = TRUE;
  }
  ///
  /// Scan PEG device vs DEVEN register for PEG controllers configuration
  ///
  for (PegComplete = 0; PegComplete < ((sizeof (PegDevenTable)) / (sizeof (PEG_DEVEN))); PegComplete++) {

    PegBus    = PegDevenTable[PegComplete].Bus;
    PegDev    = PegDevenTable[PegComplete].Device;
    PegFunc   = PegDevenTable[PegComplete].Function;
    PegEnable = PegDevenTable[PegComplete].DevenMask;

    if ((PegDevenReg & PegEnable) == 0) {
//[-start-130626-IB05400422-add]//
      //
      // Must clear 0x18, 0x19, 0x1A to release bus number 1
      // Be careful when updating RC.
      //
      MmPci32And (0, PegBus, PegDev, PegFunc, PCI_PBUS, 0xFF0000FF);
//[-end-130626-IB05400422-add]//
      continue;
    }
    ///
    /// Check for a card presence in the PEG slot.
    /// We don't know if it's a graphics card yet.
    ///
    if ((MmPci8 (0, PegBus, PegDev, PegFunc, R_SA_PEG_SLOTSTS_OFFSET) & BIT6) == 0) {
//[-start-130626-IB05400422-add]//
      //
      // Must clear 0x18, 0x19, 0x1A to release bus number 1
      // Be careful when updating RC.
      //
      MmPci32And (0, PegBus, PegDev, PegFunc, PCI_PBUS, 0xFF0000FF);
//[-end-130626-IB05400422-add]//
      continue;
    }
    ///
    /// Set PEG PortBus = 1 to Read Endpoint.
    ///
    MmPci32AndThenOr (0, PegBus, PegDev, PegFunc, PCI_PBUS, 0xFF0000FF, 0x00010100);

    ///
    /// A config write is required in order for the device to re-capture the Bus number,
    /// according to PCI Express Base Specification, 2.2.6.2
    /// Write to a read-only register VendorID to not cause any side effects.
    ///
    MmPci16 (0, 1, 0, 0, PCI_VID) = 0;

    ///
    /// Read Vendor ID to check if endpoint exists
    /// if no device exists, then check next device
    ///
    if (MmPci16 (0, 1, 0, 0, PCI_VID) == 0xFFFF) {
//[-start-130626-IB05400422-add]//
      //
      // Must clear 0x18, 0x19, 0x1A to release bus number 1
      // Be careful when updating RC.
      //
      MmPci32And (0, PegBus, PegDev, PegFunc, PCI_PBUS, 0xFF0000FF);      
//[-end-130626-IB05400422-add]//
      continue;
    }
    ///
    /// Check for a multifunction device
    ///
    HeaderType = MmPci8 (0, 1, 0, 0, PCI_HEADER_TYPE_OFFSET);
    if ((HeaderType & HEADER_TYPE_MULTI_FUNCTION) != 0) {
      MaxFunction = 7;
    } else {
      MaxFunction = 0;
    }
    ///
    /// Calculate total PEG MMIO length on all functions of the endpoint
    ///
    for (Func = 0; Func <= MaxFunction; Func++) {
      if (MmPci16 (0, 1, 0, Func, PCI_VID) == 0xFFFF) {
        continue;
      }

      FindPciDeviceMmioLength (1, 0, Func, &MmioLength);
      *PegMmioLength += MmioLength;
    }
    ///
    /// Perform PEG Endpoint Class Code Check.  If the Endpoint Class Code is
    /// not GFX, then the Port is being used as a standard PCI Express Port.
    ///
    ClassCode = MmPci8 (0, 1, 0, 0, PCI_BCC);
    if (ClassCode == 0x03) {
      ///
      /// Disable PEG if IGD or PCI VGA take precedence.
      ///
      if (AlwaysEnablePeg == 0) {
        ///
        /// If IGD is present and selected as primary, skip the PEG VGA enabling
        ///
        if (IgdPresent && (PrimaryDisplaySelection == IGD)) {
          MmPci32And (0, PegBus, PegDev, PegFunc, PCI_PBUS, 0xFF0000FF);
          continue;
        }
        ///
        /// If PCI video card was detected, skip the PEG VGA enabling
        ///
        if (*PrimaryDisplay == PCI) {
          MmPci32And (0, PegBus, PegDev, PegFunc, PCI_PBUS, 0xFF0000FF);
          continue;
        }
      }
      ///
      /// Enable PEG video and Execute 16-bit address decodes on VGA I/O accesses
      ///
      /// Check if PEG VGA already detected
      ///
      if (*PrimaryDisplay != PEG) {
        MmPci16Or (0, PegBus, PegDev, PegFunc, R_SA_PEG_BCTRL_OFFSET, (BIT3 + BIT4));
        *PrimaryDisplay = PEG;
        PegVgaFunc = PegFunc;
        DEBUG ((EFI_D_INFO, "PCIe card on PEG%x%x (%x:%x:%x) enabled as VGA.\n", PegDev, PegFunc, PegBus, PegDev, PegFunc));
      }
    }

    if (ClassCode == 0x06) {
      CardDetect = EnumerateBridgeDevice (PegBus, PegDev, PegFunc, PegMmioLength);
      if (CardDetect == TRUE) {
        ///
        /// Check if PEG VGA already detected
        ///
        if (*PrimaryDisplay != PEG) {
          MmPci16Or (0, PegBus, PegDev, PegFunc, R_SA_PEG_BCTRL_OFFSET, (BIT3 + BIT4));
          *PrimaryDisplay = PEG;
          PegVgaFunc = PegFunc;
          DEBUG ((EFI_D_INFO, "PCIe card on PEG%x%x (%x:%x:%x) enabled as VGA.\n", PegDev, PegFunc, PegBus, PegDev, PegFunc));
        }
      }
    }
    ///
    /// Restore bus numbers on the PEG bridge.
    ///
    MmPci32And (0, PegBus, PegDev, PegFunc, PCI_PBUS, 0xFF0000FF);
  } // End of the for Loop

  ///
  /// If a PEG device is used for primary graphics, set the ISAEN bit on all other PEG ports.
  ///
  if (PegVgaFunc != 0xFF) {
    for (PegComplete = 0; PegComplete < ((sizeof (PegDevenTable)) / (sizeof (PEG_DEVEN))); PegComplete++) {
      if (PegVgaFunc == PegComplete) {
        continue;
      }
      PegBus    = PegDevenTable[PegComplete].Bus;
      PegDev    = PegDevenTable[PegComplete].Device;
      PegFunc   = PegDevenTable[PegComplete].Function;
      MmPci16Or (0, PegBus, PegDev, PegFunc, R_SA_PEG_BCTRL_OFFSET, BIT2);
      DEBUG ((EFI_D_INFO, "PEG%x%x (%x:%x:%x) ISAEN has been set.\n", PegDev, PegFunc, PegBus, PegDev, PegFunc));
    }
  }
}

/**
  Find the MMIO size that a given PCI device requires

  @param[in] BusNum     - Bus number of the device
  @param[in] DevNum     - device Number of the device
  @param[in] FunNum     - Function number of the device
  @param[out] MmioLength - MMIO Length in bytes
**/
VOID
FindPciDeviceMmioLength (
  IN UINT32  BusNum,
  IN UINT32  DevNum,
  IN UINT32  FunNum,
  OUT UINT32 *MmioLength
  )
{
  UINT32  CurrentMmioLength;
  UINT32  SavedBAR;
  UINT32  i;
  UINT64  BarAlign;
  UINT8   ClassCode;

  *MmioLength = 0;
  BarAlign    = PCI_BAR_OLD_ALIGN;

  ClassCode   = MmPci8 (0, BusNum, DevNum, FunNum, PCI_BCC);
  if (ClassCode == PCI_CLASS_BRIDGE) {
    return ;
  }

  for (i = PCI_BAR0; i <= PCI_BAR5; i += 4) {
    SavedBAR = MmPci32 (0, BusNum, DevNum, FunNum, i);
    ///
    /// Check BAR is read-only or not
    ///
    MmPci32And (0, BusNum, DevNum, FunNum, i, (UINT32) PCI_BAR_NOCHANGE);
    MmPci32Or (0, BusNum, DevNum, FunNum, i, BarAlign);
    if (SavedBAR == MmPci32 (0, BusNum, DevNum, FunNum, i)) {
      ///
      /// Restore BAR as original value
      ///
      MmPci32 (0, BusNum, DevNum, FunNum, i) = SavedBAR;
      continue;
    }
    ///
    /// If BAR is not memory map, skip it
    ///
    if ((SavedBAR & BIT0) != 0) {
      ///
      /// Restore BAR as original value
      ///
      MmPci32 (0, BusNum, DevNum, FunNum, i) = SavedBAR;
      continue;
    }
    ///
    /// Calculate the MMIO length through BAR
    ///
    CurrentMmioLength = ~(MmPci32 (0, BusNum, DevNum, FunNum, i) &~0xF) + 1;
    *MmioLength += CurrentMmioLength;

    ///
    /// Restore BAR as original value
    ///
    MmPci32 (0, BusNum, DevNum, FunNum, i) = SavedBAR;
    ///
    /// Skip next index if BAR is 64bit address
    ///
    if ((SavedBAR & (BIT1 + BIT2)) == 0x4) {
      i += 4;
    }
  }
}

/**
  CheckOffboardPcieVga: Check if off board PCIe graphics Card is present

  @param[in] PeiServices             - Pointer to the PEI services table
  @param[in, out] PchPcieMmioLength  - Total PCIe MMIO length on all PCH root ports
  @param[in, out] PrimaryDisplay     - Primary Display - default is IGD
**/
VOID
CheckOffboardPcieVga (
  IN CONST EFI_PEI_SERVICES   **PeiServices,
  IN OUT UINT32               *PchPcieMmioLength,
  IN OUT DISPLAY_DEVICE       *PrimaryDisplay
  )
{
  UINT8   PortNo;
  UINT32  PcieBusNum;
  UINT8   Bus;
  UINT8   Dev;
  UINT8   Func;
  UINT8   MaxFunction;
  UINT8   SubBusNum;
  UINT8   HeaderType;
  UINT16  Buffer16;
  BOOLEAN CardDetect;
  UINT32  MmioLength;

  MmioLength = 0;

  ///
  /// Initialize Secondary and Subordinate bus number for first Pcie root port
  ///
  PcieBusNum  = 0x00010100;

  SubBusNum   = 0;

  CardDetect  = FALSE;

  for (PortNo = 0; PortNo < GetPchMaxPciePortNum (); PortNo++) {
    ///
    /// Check if root port exists
    ///
    if (MmPci16 (0, 0, 0x1C, PortNo, PCI_VID) == 0xFFFF) {
      continue;
    }

    MmPci32 (0, 0, 0x1c, PortNo, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET)  = PcieBusNum;
    Bus = MmPci8 (0, 0, 0x1c, PortNo, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);

    ///
    /// Assign temporary subordinate bus number so that device this bridge can be seen
    ///
    MmPci8 (0, 0, 0x1c, PortNo, PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET) = 0xFF;

    ///
    /// A config write is required in order for the device to re-capture the Bus number,
    /// according to PCI Express Base Specification, 2.2.6.2
    /// Write to a read-only register VendorID to not cause any side effects.
    ///
    MmPci16 (0, Bus, 0, 0, PCI_VID) = 0;

    SubBusNum                       = EnumerateDownstream (Bus);
    ///
    /// Update the actual subordinate bus number
    ///
    MmPci8 (0, 0, 0x1c, PortNo, PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET) = SubBusNum;
    PcieBusNum = (SubBusNum + 1) << 8;
  }

  for (Bus = 1; Bus <= SubBusNum; Bus++) {
    for (Dev = 0; Dev < 32; Dev++) {
      ///
      /// Read Vendor ID to check if device exists
      /// if no device exists, then check next device
      ///
      if (MmPci16 (0, Bus, Dev, 0, PCI_VID) == 0xFFFF) {
        continue;
      }
      ///
      /// Check for a multifunction device
      ///
      HeaderType = MmPci8 (0, Bus, Dev, 0, PCI_HDR);
      if ((HeaderType & HEADER_TYPE_MULTI_FUNCTION) != 0) {
        MaxFunction = 7;
      } else {
        MaxFunction = 0;
      }

      for (Func = 0; Func <= MaxFunction; Func++) {
        if (MmPci16 (0, Bus, Dev, Func, PCI_VID) == 0xFFFF) {
          continue;
        }

        FindPciDeviceMmioLength (Bus, Dev, Func, &MmioLength);
        *PchPcieMmioLength += MmioLength;

        ///
        /// Video cards can have Base Class 0 with Sub-class 1
        /// or Base Class 3.
        ///
        if (MmPci16 (0, Bus, Dev, Func, PCI_SCC) == 0x0300) {
          if (CardDetect != TRUE) {
            *PrimaryDisplay = PCI;
            DEBUG ((EFI_D_INFO, "PCH PCIe Graphics Card enabled.\n"));
            CardDetect = TRUE;
          }
        }
      }
    }
  }
  ///
  /// Clear bus number on all the bridges that we have opened so far.
  /// We have to do it in the reverse Bus number order.
  ///
  for (Bus = SubBusNum; Bus >= 1; Bus--) {
    for (Dev = 0; Dev < 32; Dev++) {
      ///
      /// Read Vendor ID to check if device exists
      /// if no device exists, then check next device
      ///
      if (MmPci16 (0, Bus, Dev, 0, PCI_VID) == 0xFFFF) {
        continue;
      }

      Buffer16 = MmPci16 (0, Bus, Dev, 0, PCI_SCC);
      ///
      /// Clear Bus Number for PCI/PCI Bridge Device
      ///
      if (Buffer16 == 0x0604) {
        MmPci32 (0, Bus, Dev, 0, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET) = 0;
      }
    }
  }

  for (PortNo = 0; PortNo < GetPchMaxPciePortNum (); PortNo++) {
    ///
    /// Clear bus numbers so that PCIe slots are hidden
    ///
    MmPci32 (0, 0, 0x1c, PortNo, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET) = 0;
  }
}

/**
  This function enumerate all downstream bridge.

  @param[in] BusNum  - Primary bus number of current bridge.

  @retval BusNum: return current bus number if current bus is an enpoint device.
  @retval SubBus: return subordinate bus number if current bus is a bridge.
**/
UINT8
EnumerateDownstream (
  IN UINT8  BusNum
  )
{
  UINT8   DevNum;
  UINT16  Buffer16;
  UINT8   SubBus;
  UINT8   SecBus;

  SubBus  = 0;

  SecBus  = BusNum;

  for (DevNum = 0; DevNum < 32; DevNum++) {
    ///
    /// Read Vendor ID to check if device exists
    /// if no device exists, then check next device
    ///
    if (MmPci16 (0, BusNum, DevNum, 0, PCI_VID) == 0xFFFF) {
      continue;
    }

    Buffer16 = MmPci16 (0, BusNum, DevNum, 0, PCI_SCC);
    ///
    /// Check for PCI/PCI Bridge Device Base Class 6 with subclass 4
    ///
    if (Buffer16 == 0x0604) {
      SecBus++;
      MmPci8 (0, BusNum, DevNum, 0, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET)   = BusNum;
      MmPci8 (0, BusNum, DevNum, 0, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET) = SecBus;
      ///
      /// Assign temporary subordinate bus number so that device behind this bridge can be seen
      ///
      MmPci8 (0, BusNum, DevNum, 0, PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET) = 0xFF;

      ///
      /// A config write is required in order for the device to re-capture the Bus number,
      /// according to PCI Express Base Specification, 2.2.6.2
      /// Write to a read-only register VendorID to not cause any side effects.
      ///
      MmPci16 (0, SecBus, 0, 0, PCI_VID) = 0;

      ///
      /// Enumerate bus behind this bridge by calling this funstion recursively
      ///
      SubBus = EnumerateDownstream (SecBus);
      ///
      /// Update the correct subordinate bus number
      ///
      MmPci8 (0, BusNum, DevNum, 0, PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET) = SubBus;
      SecBus = SubBus;
    }
  }

  if (SubBus == 0) {
    return BusNum;
  } else {
    return SubBus;
  }
}

/**
  This function enumerate the bridge on the device

  @param[in] PegBus  - Particular Bus number
  @param[in] PegDev  - Particular Device number
  @param[in] PegFunc  - Particular Func number
  @param[in, out] PegMmioLength - PEG MMIO length

  @retval CardDetect : TRUE if current bridge device has a Graphics card.
  @retval CardDetect : FALSE if current bridge device has no Graphics card.
**/
BOOLEAN
EnumerateBridgeDevice (
  IN UINT8      PegBus,
  IN UINT8      PegDev,
  IN UINT8      PegFunc,
  IN OUT UINT32 *PegMmioLength
  )
{

  UINT8   Bus;
  UINT8   Dev;
  UINT8   SubBusNum;
  UINT16  Buffer16;
  BOOLEAN CardDetect;

  CardDetect = FALSE;

  ///
  /// Temporarily program the secondary and subordinate bus numbers
  /// of PEG bridge to (1, 0xFF) so that devices behind the bridge can be seen
  ///
  Bus = 1;
  MmPci8 (0, PegBus, PegDev, PegFunc, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET)   = Bus;
  MmPci8 (0, PegBus, PegDev, PegFunc, PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET) = 0xFF;

  ///
  /// A config write is required in order for the device to re-capture the Bus number,
  /// according to PCI Express Base Specification, 2.2.6.2
  /// Write to a read-only register VendorID to not cause any side effects.
  ///
  MmPci16 (0, Bus, 0, 0, PCI_VID) = 0;

  SubBusNum                       = EnumerateDownstream (Bus);

  for (Bus = 1; Bus <= SubBusNum; Bus++) {
    for (Dev = 0; Dev < 32; Dev++) {
      ///
      /// Read Vendor ID to check if device exists
      /// if no device exists, then check next device
      ///
      if (MmPci16 (0, Bus, Dev, 0, PCI_VID) == 0xFFFF) {
        continue;

      }
      ///
      /// Video cards can have Base Class 0 with Sub-class 1
      /// or Base Class 3.
      ///
      if (MmPci16 (0, Bus, Dev, 0, PCI_SCC) == 0x0300) {
        FindPciDeviceMmioLength (Bus, Dev, 0, PegMmioLength);
        CardDetect = TRUE;
        break;
      }
    }

    if (CardDetect == TRUE) {
      break;
    }
  }
  ///
  /// Clear bus number on all the bridges that we have opened so far.
  /// We have to do it in the reverse Bus number order.
  ///
  for (Bus = SubBusNum; Bus >= 1; Bus--) {
    for (Dev = 0; Dev < 32; Dev++) {
      ///
      /// Read Vendor ID to check if device exists
      /// if no device exists, then check next device
      ///
      if (MmPci16 (0, Bus, Dev, 0, PCI_VID) == 0xFFFF) {
        continue;
      }

      Buffer16 = MmPci16 (0, Bus, Dev, 0, PCI_SCC);
      ///
      /// Clear Bus Number for PCI/PCI Bridge Device
      ///
      if (Buffer16 == 0x0604) {
        MmPci32 (0, Bus, Dev, 0, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET) = 0;
      }
    }
  }
  ///
  /// Clear the bus numbers on the PEG bridge
  ///
  MmPci32 (0, PegBus, PegDev, PegFunc, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET) = 0;

  return CardDetect;
}

/**

  "Poll Status" for GT Readiness

  @param[in] PeiServices     - Pointer to the PEI services table
  @param[in] StallPpi        - Pointer to Stall PPI
  @param[in] Base            - Base address of MMIO
  @param[in] Offset          - MMIO Offset
  @param[in] Mask            - Mask
  @param[in] Result          - Value to wait for


**/
VOID
PollGtReady (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_STALL_PPI        *StallPpi,
  UINT64 Base,
  UINT32 Offset,
  UINT32 Mask,
  UINT32 Result
  )
{
  UINT32  GtStatus;
  UINT16  StallCount;

  StallCount = 0;

  ///
  /// Register read
  ///
  GtStatus = Mmio32 (Base, Offset);

  while (((GtStatus & Mask) != Result) && (StallCount < GT_WAIT_TIMEOUT)) {

    GtStatus = Mmio32 (Base, Offset);
    ///
    /// 1mSec wait
    ///
    StallPpi->Stall (PeiServices, StallPpi, 1000);
    StallCount = StallCount + 1;
  }

  ASSERT ((StallCount != GT_WAIT_TIMEOUT));
}
