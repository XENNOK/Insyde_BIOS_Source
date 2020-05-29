/** @file
  PCH S3 Save and Restore SMM Driver Entry

@copyright
  Copyright (c) 2012 Intel Corporation. All rights reserved
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
#include "PchLateInitSmm.h"

STATIC PCH_SAVE_RESTORE_REG PchSaveRestoreReg_Common[] = {
  {Acpi, R_PCH_ACPI_PM1_EN, 0, BIT14, 2},
  {Rcrb, 0x3300, 0, 0xFFFFFFFF, 4},
  {Rcrb, 0x3304, 0, 0xFFFFFFFF, 4},
  {Rcrb, 0x3308, 0, 0xFFFFFFFF, 4},
  {Rcrb, 0x330C, 0, 0xFFFFFFFF, 4}
};

#ifdef TRAD_FLAG
STATIC PCH_SAVE_RESTORE_REG PchSaveRestoreReg_PchH[] = {
  {Acpi, R_PCH_ACPI_GPE0a_EN, 0, 0xFFFF0246, 4},
  {Acpi, R_PCH_ACPI_GPE0b_EN, 0, 0xFF000040, 4},
  {Tco , R_PCH_TCO2_CNT , 0, BIT5 | BIT4, 1}
};
#endif // TRAD_FLAG

#ifdef ULT_FLAG
STATIC PCH_SAVE_RESTORE_REG PchSaveRestoreReg_PchLp[] = {
  {Rcrb, 0x3320, 0, 0xFFFFFFFF, 4}
};
#endif // ULT_FLAG

STATIC PCH_SAVE_RESTORE_PCI PchSaveRestorePciReg_Common[] = {
  {PciCfg, PCI_DEVICE_NUMBER_PCH_AZALIA, 0, 0, R_PCH_HDA_PCS + 1, 0, BIT0, 1, NULL},
  {PciMmr, PCI_DEVICE_NUMBER_PCH_AZALIA, 0, PCI_BASE_ADDRESSREG_OFFSET, R_HDA_WAKEEN, 0, BIT3 | BIT2 | BIT1 | BIT0, 1, NULL},
  {PciCfg, PCI_DEVICE_NUMBER_PCH_USB, 0, 0, R_PCH_EHCI_PWR_CNTL_STS + 1, 0, BIT0, 1, NULL},
  {PciMmr, PCI_DEVICE_NUMBER_PCH_USB, 0, PCI_BASE_ADDRESSREG_OFFSET, R_PCH_EHCI_CONFIGFLAG, 0, BIT0, 1, NULL},
  {PciMmr, PCI_DEVICE_NUMBER_PCH_USB, 0, PCI_BASE_ADDRESSREG_OFFSET, R_PCH_EHCI_PORTSC0 + 2, 0, BIT6 | BIT5 | BIT4, 1, NULL},
  {PciMmr, PCI_DEVICE_NUMBER_PCH_USB, 0, PCI_BASE_ADDRESSREG_OFFSET, R_PCH_EHCI_PORTSC0 + 6, 0, BIT6 | BIT5 | BIT4, 1, NULL},
  {PciMmr, PCI_DEVICE_NUMBER_PCH_USB, 0, PCI_BASE_ADDRESSREG_OFFSET, R_PCH_EHCI_PORTSC0 + 10, 0, BIT6 | BIT5 | BIT4, 1, NULL},
  {PciMmr, PCI_DEVICE_NUMBER_PCH_USB, 0, PCI_BASE_ADDRESSREG_OFFSET, R_PCH_EHCI_PORTSC0 + 14, 0, BIT6 | BIT5 | BIT4, 1, NULL},
  {PciMmr, PCI_DEVICE_NUMBER_PCH_USB, 0, PCI_BASE_ADDRESSREG_OFFSET, R_PCH_EHCI_PORTSC0 + 18, 0, BIT6 | BIT5 | BIT4, 1, NULL},
  {PciMmr, PCI_DEVICE_NUMBER_PCH_USB, 0, PCI_BASE_ADDRESSREG_OFFSET, R_PCH_EHCI_PORTSC0 + 22, 0, BIT6 | BIT5 | BIT4, 1, NULL},
  {PciMmr, PCI_DEVICE_NUMBER_PCH_USB, 0, PCI_BASE_ADDRESSREG_OFFSET, R_PCH_EHCI_PORTSC0 + 26, 0, BIT6 | BIT5 | BIT4, 1, NULL},
  {PciMmr, PCI_DEVICE_NUMBER_PCH_USB, 0, PCI_BASE_ADDRESSREG_OFFSET, R_PCH_EHCI_PORTSC0 + 30, 0, BIT6 | BIT5 | BIT4, 1, NULL},
  {PciMmr, PCI_DEVICE_NUMBER_PCH_USB, 0, PCI_BASE_ADDRESSREG_OFFSET, R_PCH_EHCI_PORTSC0 + 34, 0, BIT6 | BIT5 | BIT4, 1, NULL},
  {PciMmr, PCI_DEVICE_NUMBER_PCH_USB, 0, PCI_BASE_ADDRESSREG_OFFSET, R_PCH_EHCI_PORTSC0 + 38, 0, BIT6 | BIT5 | BIT4, 1, NULL},
  {PciCfg, PCI_DEVICE_NUMBER_PCH_XHCI, 0, 0, R_PCH_XHCI_PWR_CNTL_STS + 1, 0, BIT0 | BIT7, 1, NULL},
  {PciCfg, PCI_DEVICE_NUMBER_PCH_XHCI, 0, 0, R_PCH_XHCI_USB2PR, 0, 0x7FFF, 2, NULL},
  {PciCfg, PCI_DEVICE_NUMBER_PCH_XHCI, 0, 0, R_PCH_XHCI_USB3PR, 0, 0x3F, 1, NULL},
  {PciCfg, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, 0, 0, R_PCH_PCIE_PMCS + 1, 0, BIT0, 1, NULL},
  {PciCfg, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, 1, 0, R_PCH_PCIE_PMCS + 1, 0, BIT0, 1, NULL},
  {PciCfg, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, 2, 0, R_PCH_PCIE_PMCS + 1, 0, BIT0, 1, NULL},
  {PciCfg, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, 3, 0, R_PCH_PCIE_PMCS + 1, 0, BIT0, 1, NULL},
  {PciCfg, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, 4, 0, R_PCH_PCIE_PMCS + 1, 0, BIT0, 1, NULL},
  {PciCfg, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, 5, 0, R_PCH_PCIE_PMCS + 1, 0, BIT0, 1, NULL}
};

#ifdef TRAD_FLAG
STATIC PCH_SAVE_RESTORE_PCI PchSaveRestorePciReg_PchH[] = {
  {PciCfg, PCI_DEVICE_NUMBER_PCH_USB_EXT, 0, 0, R_PCH_EHCI_PWR_CNTL_STS + 1, 0, BIT0, 1, NULL},
  {PciMmr, PCI_DEVICE_NUMBER_PCH_USB_EXT, 0, PCI_BASE_ADDRESSREG_OFFSET, R_PCH_EHCI_CONFIGFLAG, 0, BIT0, 1, NULL},
  {PciMmr, PCI_DEVICE_NUMBER_PCH_USB_EXT, 0, PCI_BASE_ADDRESSREG_OFFSET, R_PCH_EHCI_PORTSC0 + 2, 0, BIT6 | BIT5 | BIT4, 1, NULL},
  {PciMmr, PCI_DEVICE_NUMBER_PCH_USB_EXT, 0, PCI_BASE_ADDRESSREG_OFFSET, R_PCH_EHCI_PORTSC0 + 6, 0, BIT6 | BIT5 | BIT4, 1, NULL},
  {PciMmr, PCI_DEVICE_NUMBER_PCH_USB_EXT, 0, PCI_BASE_ADDRESSREG_OFFSET, R_PCH_EHCI_PORTSC0 + 10, 0, BIT6 | BIT5 | BIT4, 1, NULL},
  {PciMmr, PCI_DEVICE_NUMBER_PCH_USB_EXT, 0, PCI_BASE_ADDRESSREG_OFFSET, R_PCH_EHCI_PORTSC0 + 14, 0, BIT6 | BIT5 | BIT4, 1, NULL},
  {PciMmr, PCI_DEVICE_NUMBER_PCH_USB_EXT, 0, PCI_BASE_ADDRESSREG_OFFSET, R_PCH_EHCI_PORTSC0 + 18, 0, BIT6 | BIT5 | BIT4, 1, NULL},
  {PciMmr, PCI_DEVICE_NUMBER_PCH_USB_EXT, 0, PCI_BASE_ADDRESSREG_OFFSET, R_PCH_EHCI_PORTSC0 + 22, 0, BIT6 | BIT5 | BIT4, 1, NULL},
  {PciMmr, PCI_DEVICE_NUMBER_PCH_USB_EXT, 0, PCI_BASE_ADDRESSREG_OFFSET, R_PCH_EHCI_PORTSC0 + 26, 0, BIT6 | BIT5 | BIT4, 1, NULL},
  {PciMmr, PCI_DEVICE_NUMBER_PCH_USB_EXT, 0, PCI_BASE_ADDRESSREG_OFFSET, R_PCH_EHCI_PORTSC0 + 30, 0, BIT6 | BIT5 | BIT4, 1, NULL},
  {PciMmr, PCI_DEVICE_NUMBER_PCH_USB_EXT, 0, PCI_BASE_ADDRESSREG_OFFSET, R_PCH_EHCI_PORTSC0 + 34, 0, BIT6 | BIT5 | BIT4, 1, NULL},
  {PciMmr, PCI_DEVICE_NUMBER_PCH_USB_EXT, 0, PCI_BASE_ADDRESSREG_OFFSET, R_PCH_EHCI_PORTSC0 + 38, 0, BIT6 | BIT5 | BIT4, 1, NULL},
  {PciCfg, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, 6, 0, R_PCH_PCIE_PMCS + 1, 0, BIT0, 1, NULL},
  {PciCfg, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, 7, 0, R_PCH_PCIE_PMCS + 1, 0, BIT0, 1, NULL}
};
#endif // TRAD_FLAG

#ifdef ULT_FLAG
STATIC PCH_SAVE_RESTORE_PCI PchSaveRestorePciReg_PchLp[] = {
  {PciMmr, PCI_DEVICE_NUMBER_PCH_SATA, 2, R_PCH_SATA_AHCI_BAR, R_PCH_SATA_AHCI_P0DEVSLP, 0, 0x1FFFFFFF, 4, RestorePxDevSlp},
  {PciMmr, PCI_DEVICE_NUMBER_PCH_SATA, 2, R_PCH_SATA_AHCI_BAR, R_PCH_SATA_AHCI_P1DEVSLP, 0, 0x1FFFFFFF, 4, RestorePxDevSlp},
  {PciMmr, PCI_DEVICE_NUMBER_PCH_SATA, 2, R_PCH_SATA_AHCI_BAR, R_PCH_SATA_AHCI_P3DEVSLP, 0, 0x1FFFFFFF, 4, RestorePxDevSlp}
};
#endif // ULT_FLAG
DEVICE_POWER_STATE DevicePowerState[] = {
  {PCI_DEVICE_NUMBER_PCH_AZALIA, 0, DeviceD0},
  {PCI_DEVICE_NUMBER_PCH_USB_EXT, 0, DeviceD0},
  {PCI_DEVICE_NUMBER_PCH_USB, 0, DeviceD0},
  {PCI_DEVICE_NUMBER_PCH_XHCI, 0, DeviceD0},
  {PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, 0, DeviceD0},
  {PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, 1, DeviceD0},
  {PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, 2, DeviceD0},
  {PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, 3, DeviceD0},
  {PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, 4, DeviceD0},
  {PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, 5, DeviceD0},
  {PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, 6, DeviceD0},
  {PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS, 7, DeviceD0}
};

PCH_SAVE_RESTORE_PCI_WRAP PchSaveRestorePciRegWrap[] = {
#ifdef TRAD_FLAG
  {PchSaveRestorePciReg_Common, ARRAY_SIZE(PchSaveRestorePciReg_Common), PchH},
  {PchSaveRestorePciReg_PchH, ARRAY_SIZE (PchSaveRestorePciReg_PchH), PchH}
#endif // TRAD_FLAG
#if (defined ULT_FLAG) && (defined TRAD_FLAG)
  ,
#endif // ULT_FLAG && TRAD_FLAG
#ifdef ULT_FLAG
  {PchSaveRestorePciReg_Common, ARRAY_SIZE(PchSaveRestorePciReg_Common), PchLp},
  {PchSaveRestorePciReg_PchLp, ARRAY_SIZE(PchSaveRestorePciReg_PchLp), PchLp}
#endif // ULT_FLAG
};

PCH_SAVE_RESTORE_REG_WRAP PchSaveRestoreRegWrap[] = {
#ifdef TRAD_FLAG
  {PchSaveRestoreReg_Common, ARRAY_SIZE(PchSaveRestoreReg_Common), PchH},
  {PchSaveRestoreReg_PchH, ARRAY_SIZE(PchSaveRestoreReg_PchH), PchH}
#endif // TRAD_FLAG
#if (defined ULT_FLAG) && (defined TRAD_FLAG)
  ,
#endif // ULT_FLAG && TRAD_FLAG
#ifdef ULT_FLAG
  {PchSaveRestoreReg_Common, ARRAY_SIZE(PchSaveRestoreReg_Common), PchLp},
  {PchSaveRestoreReg_PchLp, ARRAY_SIZE(PchSaveRestoreReg_PchLp), PchLp}  
#endif // ULT_FLAG
};

UINT32      PciMemBase = 0;
//[-start-130502-IB10130048-add]//
BOOLEAN     IsRestoredFlag = FALSE;
//[-end-130502-IB10130048-add]//
/**

  Find the Offset to a given Capabilities ID
  CAPID list:
    0x01 = PCI Power Management Interface
    0x04 = Slot Identification
    0x05 = MSI Capability
    0x10 = PCI Express Capability

  @param[in] Bus       -   Pci Bus Number
  @param[in] Device    -   Pci Device Number
  @param[in] Func   -   Pci Function Number
  @param[in] CapId     -   CAPID to search for

  @retval 0       - CAPID not found
  @retval Other   - CAPID found, Offset of desired CAPID

**/
UINT8
PcieFindCapId (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Func,
  IN UINT8   CapId
  )
{
  UINT8 CapHeader;

  ///
  /// Always start at Offset 0x34
  ///
  CapHeader = MmioRead8 (MmPciAddress (0, Bus, Device, Func, PCI_CAPBILITY_POINTER_OFFSET));
  if (CapHeader == 0xFF) {
    return 0;
  }

  while (CapHeader != 0) {
    ///
    /// Bottom 2 bits of the pointers are reserved per PCI Local Bus Spec 2.2
    ///
    CapHeader &= ~(BIT1 | BIT0);
    ///
    /// Search for desired CapID
    ///
    if (MmioRead8 (MmPciAddress (0, Bus, Device, Func, CapHeader)) == CapId) {
      return CapHeader;
    }

    CapHeader = MmioRead8 (MmPciAddress (0, Bus, Device, Func, CapHeader + 1));
  }

  return 0;
}

/**

  Save Device Power State and restore later

  @param[in] Device    -   Pci Device Number
  @param[in] Function  -   Pci Function Number

  @retval None

**/
VOID
SaveDevPowState (
  IN UINT8   Device,
  IN UINT8   Function
  )
{
  UINTN    index;

  for (index = 0; index < ARRAY_SIZE(DevicePowerState); index++) {
    if ((DevicePowerState[index].Device == Device) && (DevicePowerState[index].Function) == Function) {
      DevicePowerState[index].PowerState = DeviceD3;
      break;
    }
  }
}

/**

  Restore Device Power State back to D3

  @retval None

**/
VOID
RestoreDevPowState (
  VOID
  )
{
  UINTN    index;
  UINTN    PciBaseAddress;
  UINT8    CapOffset;

  for (index = 0; index < ARRAY_SIZE(DevicePowerState); index++) {
    if (DevicePowerState[index].PowerState == DeviceD3) {
      PciBaseAddress = MmPciAddress(
                         0,
                         0,
                         DevicePowerState[index].Device,
                         DevicePowerState[index].Function,
                         0
                       );
      CapOffset = PcieFindCapId (
                    0,
                    DevicePowerState[index].Device,
                    DevicePowerState[index].Function,
                    EFI_PCI_CAPABILITY_ID_PCIPM
                  );
      if (CapOffset != 0) {
        MmioOr8 (PciBaseAddress + CapOffset + 0x4, DeviceD3);
      }
    }
  }
}

/**
  Restore PxDevSlp

  @param[in] *PchSaveRestorePci  - Pointer to Pch Save Restore Pci to be restored.

  @retval None
**/
VOID
RestorePxDevSlp(
  IN PCH_SAVE_RESTORE_PCI   *PchSaveRestorePci
  )
{
  UINTN  PciBaseAddress;
  UINT32 PciBar;
  UINTN  Address;
  UINT8  PciCmd;
  UINT32 Data32;
  UINT32 PortIndex;

  ASSERT ((PchSaveRestorePci->AccessType == PciMmr) && 
         (PchSaveRestorePci->Device == PCI_DEVICE_NUMBER_PCH_SATA) && 
         ((PchSaveRestorePci->Offset == R_PCH_SATA_AHCI_P0DEVSLP) || 
          (PchSaveRestorePci->Offset == R_PCH_SATA_AHCI_P1DEVSLP) || 
          (PchSaveRestorePci->Offset == R_PCH_SATA_AHCI_P3DEVSLP)) &&
         (PchSaveRestorePci->Width == 4));

  PciBaseAddress = MmPciAddress (
                   0,
                   0,
                   PchSaveRestorePci->Device,
                   PchSaveRestorePci->Function,
                   0
                 );
  PortIndex = (PchSaveRestorePci->Offset - R_PCH_SATA_AHCI_P0DEVSLP)/0x80;
  PciCmd = MmioRead8 (PciBaseAddress + PCI_COMMAND_OFFSET);
  MmioWrite8 (PciBaseAddress + PCI_COMMAND_OFFSET, BIT1);
  PciBar = MmioRead32 (PciBaseAddress + PchSaveRestorePci->BarOffset);
  MmioWrite32 (PciBaseAddress + PchSaveRestorePci->BarOffset, PciMemBase);
  Address = PciMemBase + PchSaveRestorePci->Offset;
  ///
  /// Restore DM and DITO
  ///
  MmioAndThenOr32 (Address, (UINT32)~PchSaveRestorePci->Mask, (PchSaveRestorePci->Data & (B_PCH_SATA_AHCI_PxDEVSLP_DM_MASK | B_PCH_SATA_AHCI_PxDEVSLP_DITO_MASK)));

  ///
  /// Makesure PxCMD.ST and PxDEVSLP.ADSE are cleared to '0' before updating PxDEVSLP.DETO and PxDEVSLP.MDAT value.
  ///
  Data32 = MmioRead32 (PciMemBase + (R_PCH_SATA_AHCI_P0CMD + (0x80 * PortIndex)));
  MmioAnd32 (PciMemBase + (R_PCH_SATA_AHCI_P0CMD + (0x80 * PortIndex)), (UINT32)~B_PCH_SATA_AHCI_PxCMD_ST);
  MmioAnd32 (Address, (UINT32)~B_PCH_SATA_AHCI_PxDEVSLP_ADSE);
  MmioOr32 (Address, (PchSaveRestorePci->Data & (UINT32)~B_PCH_SATA_AHCI_PxDEVSLP_ADSE));
  MmioOr32 (PciMemBase + (R_PCH_SATA_AHCI_P0CMD + (0x80 * PortIndex)), (Data32 & B_PCH_SATA_AHCI_PxCMD_ST));
  MmioOr32 (Address, (PchSaveRestorePci->Data & B_PCH_SATA_AHCI_PxDEVSLP_ADSE));

  ///
  /// Restore original PCI command and bar
  ///
  MmioWrite8 (PciBaseAddress + PCI_COMMAND_OFFSET, PciCmd);
  MmioWrite32 (PciBaseAddress + PchSaveRestorePci->BarOffset, PciBar);
}

/**
  A SMI callback to do PCH SMI register restoration

  @param[in] DispatchHandle  - The handle of this callback, obtained when registering
  @param[in] DispatchContext - Pointer to the EFI_SMM_IO_TRAP_DISPATCH_CALLBACK_CONTEXT

  @retval None
**/
VOID
PchIoTrapSmiCallback (
  IN  EFI_HANDLE                                  DispatchHandle,
  IN  VOID                                        *CallbackContext
  )
{
  UINT16                  AcpiBase;
  UINT16                  TcoBase;
  UINTN                   index;
  UINTN                   i;
  UINTN                   PciBaseAddress;
  UINTN                   Address;
  UINT8                   PowerState;
  UINT8                   PciCmd;
  UINT32                  PciBar;
  UINT32                  Mask;
  PCH_SERIES              PchSeries;

  AcpiBase   = PchLpcPciCfg32(R_PCH_LPC_ACPI_BASE) & B_PCH_LPC_ACPI_BASE_BAR;
  TcoBase    = AcpiBase + PCH_TCO_BASE;
  PowerState = 0x0;
  PchSeries  = GetPchSeries();
	
//[-start-130502-IB10130048-modify]//
	if (IsRestoredFlag) {
  ///
  /// Restoring IO and MMIO registers
  ///
	  for (i = 0; i < ARRAY_SIZE (PchSaveRestoreRegWrap); i++) {
	    if (PchSeries != PchSaveRestoreRegWrap[i].PchSeries) {
	      continue;
	    }
	    for (index = 0; index < PchSaveRestoreRegWrap[i].size; index++) {
	      Mask = PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Mask;
	      switch (PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].AccessType) {
	        case Tco:
	          Address = TcoBase + PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Address;
	          switch (PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Width) { 
	            case 4:
	              IoAndThenOr32 (Address,  (UINT32) ~Mask, PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Data);
	              break;
	            case 2:
	              IoAndThenOr16 (Address, (UINT16) ~Mask, (UINT16) (PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Data));
	              break;
	            case 1:
	              IoAndThenOr8 (Address, (UINT8) ~Mask, (UINT8) (PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Data));
	              break;
	          }
	          break;          
	        case Acpi:
	          Address = AcpiBase + PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Address;
	          switch (PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Width) {
	            case 4:
	              IoAndThenOr32 (Address, (UINT32) ~Mask, PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Data);
	              break;
	            case 2:
	              IoAndThenOr16 (Address, (UINT16) ~Mask, (UINT16) (PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Data));
	              break;
	            case 1:
	              IoAndThenOr8 (Address, (UINT8) ~Mask, (UINT8) (PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Data));
	              break;
	          }
	          break;
	        case Rcrb:
	          Address = PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Address;
	          switch (PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Width) {
	            case 4:
	              PchMmRcrb32AndThenOr (Address, (UINT32) ~Mask, PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Data);
	              break;
	            case 2:
	              PchMmRcrb16AndThenOr (Address, (UINT16) ~Mask, (UINT16) (PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Data));
	              break;
	            case 1:
	              PchMmRcrb8AndThenOr (Address, (UINT8) ~Mask, (UINT8) (PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Data));
	              break;
	          }
	          break;
	      }
	    }
	  }

	  ///
	  /// Restoring PCI config space and PCI bar registers
	  ///
	  for (i = 0; i < ARRAY_SIZE (PchSaveRestorePciRegWrap); i++) {
	    if (PchSeries != PchSaveRestorePciRegWrap[i].PchSeries) {
	      continue;
	    }
	    for (index = 0; index < PchSaveRestorePciRegWrap[i].size; index++) {
	      PciBaseAddress = MmPciAddress(
	                         0,
	                         0,
	                         PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Device,
	                         PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Function,
	                         0
	                       );
	      if (MmioRead32(PciBaseAddress) == 0xFFFFFFFF) {
	        continue;
	      }
	      if (PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].RestoreFunction == NULL) {
	        Mask = PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Mask;
	        switch (PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].AccessType) {
	          case PciCfg:
	            Address = PciBaseAddress + PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Offset;
	            switch (PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Width) {
	              case 4:
	                MmioAndThenOr32 (Address, (UINT32) ~Mask, PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Data);
	                break;
	              case 2:
	                MmioAndThenOr16 (Address, (UINT16) ~Mask, (UINT16) (PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Data));
	                break;
	              case 1:
	                MmioAndThenOr8 (Address, (UINT8) ~Mask, (UINT8) (PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Data));
	                break;
	            }
	            break;
	          case PciMmr:
	            PciCmd = MmioRead8 (PciBaseAddress + PCI_COMMAND_OFFSET);
	            MmioWrite8 (PciBaseAddress + PCI_COMMAND_OFFSET, BIT1);
	            PciBar = MmioRead32 (PciBaseAddress + PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].BarOffset);
	            MmioWrite32 (PciBaseAddress + PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].BarOffset, PciMemBase);
	            Address = PciMemBase + PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Offset;
	            switch (PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Width) {
	              case 4:
	                MmioAndThenOr32 (Address, (UINT32) ~Mask, PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Data);
	                break;
	              case 2:
	                MmioAndThenOr16 (Address, (UINT16) ~Mask, (UINT16) (PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Data));
	                break;
	              case 1:
	                MmioAndThenOr8 (Address, (UINT8) ~Mask, (UINT8) (PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Data));
	                break;
	            }
	            ///
	            /// Restore original PCI command and bar
	            ///
	            MmioWrite8 (PciBaseAddress + PCI_COMMAND_OFFSET, PciCmd);
	            MmioWrite32 (PciBaseAddress + PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].BarOffset, PciBar);
	            break;
	        }
	      } else {
	        PchSaveRestorePciRegWrap[i].PchSaveRestorePci [index].RestoreFunction (&(PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index]));
	      }
	    }
	  }
  }
//[-end-130502-IB10130048-modify]//
}

/**
  This function save PCH register before enter S3

  @param[in] Handle    Handle of the callback
  @param[in] Context   The dispatch context

  @retval EFI_SUCCESS  PCH register saved
**/
EFI_STATUS
PchS3EntryCallBack (
  IN  EFI_HANDLE                        Handle,
  IN  VOID                              *Context
  )
{
  UINT32                  AcpiBase;
  UINT32                  TcoBase;
  UINTN                   PciBaseAddress;
  UINTN                   index;
  UINTN                   i;
  UINT8                   PowerState;
  UINT8                   CapOffset;
  UINT32                  PciBar;
  UINT32                  Mask;
  UINT32                  Address;
  PCH_SERIES              PchSeries;

  AcpiBase   = PchLpcPciCfg32(R_PCH_LPC_ACPI_BASE) & B_PCH_LPC_ACPI_BASE_BAR;
  TcoBase    = AcpiBase + PCH_TCO_BASE;
  PowerState = 0x0;
  PchSeries  = GetPchSeries();

  ///
  /// Saving IO and MMIO registers
  ///
  for (i = 0; i < ARRAY_SIZE (PchSaveRestoreRegWrap); i++) {
    if (PchSeries != PchSaveRestoreRegWrap[i].PchSeries) {
      continue;
    }
    for (index = 0; index < PchSaveRestoreRegWrap[i].size; index++) {
      Mask = PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Mask;
      switch (PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].AccessType) {
        case Tco:
          Address = TcoBase + PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Address;
          switch (PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Width) {
            case 4:
              PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Data = (IoRead32 (Address)) & Mask;
              break;
            case 2:
              PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Data = (IoRead16 (Address)) & Mask;
              break;
            case 1:
              PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Data = (IoRead8 (Address)) & Mask;
              break;
          }
          break;
        case Acpi:
          Address = AcpiBase + PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Address;
          switch (PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Width) {
            case 4:
              PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Data = (IoRead32 (Address)) & Mask;
              break;
            case 2:
              PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Data = (IoRead16 (Address)) & Mask;
              break;
            case 1:
              PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Data = (IoRead8 (Address)) & Mask;
              break;
          }
          break;
        case Rcrb:
          Address = PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Address;
          switch (PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Width) {
            case 4:
              PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Data = (PchMmRcrb32 (Address)) & Mask;
              break;
            case 2:
              PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Data = (PchMmRcrb16 (Address)) & Mask;
              break;
            case 1:
              PchSaveRestoreRegWrap[i].PchSaveRestoreReg[index].Data = (PchMmRcrb8 (Address)) & Mask;
              break;
          }
          break;
      }
    }
  }

  ///
  /// Saving PCI config space and PCI bar registers
  ///
  for (i = 0; i < ARRAY_SIZE (PchSaveRestorePciRegWrap); i++) {
    if (PchSeries != PchSaveRestorePciRegWrap[i].PchSeries) {
      continue;
    }
    for (index = 0; index < PchSaveRestorePciRegWrap[i].size; index++) {
      PciBaseAddress = MmPciAddress(
                         0,
                         0,
                         PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Device,
                         PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Function,
                         0
                       );
      if (MmioRead32(PciBaseAddress) == 0xFFFFFFFF) {
        continue;
      }
      Mask = PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Mask;
      switch (PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].AccessType) {
        case PciCfg:
          Address = (UINT32) PciBaseAddress + PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Offset;
          switch (PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Width) {
            case 4:
              PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Data = (MmioRead32 (Address)) & Mask;
              break;
            case 2:
              PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Data = (MmioRead16 (Address)) & Mask;
               break;
            case 1:
              PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Data = (MmioRead8 (Address)) & Mask;
              break;
          }
          break;
        case PciMmr:
          PciBar = MmioRead32 (PciBaseAddress + PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].BarOffset);
          CapOffset = PcieFindCapId (
                        0,
                        PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Device,
                        PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Function,
                        EFI_PCI_CAPABILITY_ID_PCIPM
                      );
          if (CapOffset != 0) {
            PowerState = MmioRead8(PciBaseAddress + CapOffset + 0x4) & (BIT1 | BIT0);
            if (PowerState == DeviceD3) {
              ///
              /// Bring up device to D0
              ///
              MmioAnd8 (PciBaseAddress + CapOffset + 0x4, (UINT8)~(BIT1 | BIT0));
              MmioWrite32 (PciBaseAddress + PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].BarOffset, PciBar);
              MmioWrite8 (PciBaseAddress + PCI_COMMAND_OFFSET, BIT1);
              SaveDevPowState(PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Device, PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Function);
            }
          }
          PciBar = PciBar & (UINT32)~(0xF);
          if (PciBar == 0x0) {
            continue;
          }
          Address = PciBar + PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Offset;
          switch (PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Width) {
            case 4:
              PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Data = (MmioRead32 (Address)) & Mask;
              break;
            case 2:
              PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Data = (MmioRead16 (Address)) & Mask;
              break;
            case 1:
              PchSaveRestorePciRegWrap[i].PchSaveRestorePci[index].Data = (MmioRead8 (Address)) & Mask;
              break;
          }
          break;
      }
    }
  }

  ///
  /// Restore devices to D3
  ///
  RestoreDevPowState();
//[-start-130502-IB10130048-add]//
  IsRestoredFlag = TRUE;
//[-end-130502-IB10130048-add]//
  return EFI_SUCCESS;
}

/**
  Initializes the PCH SMM handler for PCH save and restore

  @param[in] ImageHandle - Handle for the image of this driver
  @param[in] SystemTable - Pointer to the EFI System Table

  @retval EFI_SUCCESS    - PCH SMM handler was installed
**/
EFI_STATUS
EFIAPI
PchLateInitSmmEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                                Status;
  EFI_PHYSICAL_ADDRESS                      MemBase;
  EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL         *PchIoTrap;
  EFI_SMM_IO_TRAP_DISPATCH_REGISTER_CONTEXT PchIoTrapContext;
  EFI_HANDLE                                PchIoTrapHandle;
  EFI_SMM_SX_DISPATCH_CONTEXT               SxDispatchContext;
  EFI_SMM_SX_DISPATCH_PROTOCOL              *SxDispatchProtocol;
  EFI_HANDLE                                SxDispatchHandle;
  PCH_LATE_INIT_SMM_VARIABLE                SaveRestoreData;

  DEBUG ((EFI_D_INFO, "PchLateInitSmmEntryPoint()\n"));

  ///
  /// Locate the PCH Trap dispatch protocol
  ///
  Status = gBS->LocateProtocol (&gEfiSmmIoTrapDispatchProtocolGuid, NULL, (VOID **)&PchIoTrap);
  ASSERT_EFI_ERROR (Status);

  PchIoTrapContext.Type     = ReadWriteTrap;
  PchIoTrapContext.Length   = 4;
  PchIoTrapContext.Address  = 0;
  PchIoTrapContext.Context  = NULL;
  PchIoTrapContext.MergeDisable = FALSE;
  Status = PchIoTrap->Register (
                        PchIoTrap,
                        PchIoTrapSmiCallback,
                        &PchIoTrapContext,
                        &PchIoTrapHandle
                        );
  ASSERT_EFI_ERROR (Status);

  SaveRestoreData.IoTrapAddress = PchIoTrapContext.Address;

  DEBUG ((EFI_D_INFO, "PchIotrapSmiAddress = 0x%x\n", PchIoTrapContext.Address));

  ///
  /// Locate the Sx Dispatch Protocol
  ///
  Status = gBS->LocateProtocol (
                  &gEfiSmmSxDispatchProtocolGuid,
                  NULL,
                  (VOID **)&SxDispatchProtocol
                  );
  ASSERT_EFI_ERROR (Status);
  ///
  /// Register the callback for S3 entry
  ///
  SxDispatchContext.Type  = SxS3;
  SxDispatchContext.Phase = SxEntry;
  Status = SxDispatchProtocol->Register (
                                SxDispatchProtocol,
                                PchS3EntryCallBack,
                                &SxDispatchContext,
                                &SxDispatchHandle
                                );
  ASSERT_EFI_ERROR (Status);

  MemBase = 0x0ffffffff;
  Status = gDS->AllocateMemorySpace (
                  EfiGcdAllocateMaxAddressSearchBottomUp,
                  EfiGcdMemoryTypeMemoryMappedIo,
                  16,  // 2^16: 64K Alignment
                  0x10000,     // 64K Length
                  &MemBase,
                  ImageHandle,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  PciMemBase = (UINT32) MemBase;

  SaveRestoreData.PciMemBase = PciMemBase;

  Status = gRT->SetVariable (
                  PCH_INIT_PEI_VARIABLE_NAME,
                  &gPchInitPeiVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  sizeof (PCH_LATE_INIT_SMM_VARIABLE),
                  &SaveRestoreData
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
