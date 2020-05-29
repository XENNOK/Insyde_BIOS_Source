/** @file
  This driver does SA PCI Express initialization.

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
#include "PciExpressInit.h"

extern SYSTEM_AGENT_GLOBAL_NVS_AREA_PROTOCOL  mSaGlobalNvsAreaProtocol;

EFI_STATUS
PciExpressInit (
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
  )
/**
  PCI Express Dxe Initialization.
  Run before PCI Bus Init, where assignment of Bus, Memory,
    and I/O Resources are assigned.

  @param[in] DxePlatformSaPolicy -     SA DxePlatformPolicy protocol

  @retval EFI_SUCCESS     - Pci Express successfully started and ready to be used
  @exception EFI_UNSUPPORTED - Pci Express can't be initialized
**/
{
  EFI_STATUS  Status;
  UINT64      EgressPortBar;
  UINT64      MchBar;
  UINT64      PciExpressBar;
  UINT64      DmiBar;
  UINT32      PchRootComplexBar;
  CPU_FAMILY  CpuFamilyId;
  CPU_STEPPING CpuSteppingId;

  CpuFamilyId = GetCpuFamily();
  CpuSteppingId = GetCpuStepping();

  ///
  /// The SA Represents Component ID 1 (CID1), while the PCH represents
  /// Component ID 2 (CID2).  This code will completely configure both Components
  /// CID1 Integrated Ports:
  ///    Egress Port = Port 0
  ///    DMI Port    = Port 1
  ///    Peg Port10  = Port 2
  ///    Peg Port11  = Port 3
  ///    Peg Port12  = Port 4
  /// CID2 Integated Ports:
  ///    Egress Port = Port 0
  ///    Root Port 1 = Port 1
  ///    Root Port 2 = Port 2
  ///    Root Port 3 = Port 3
  ///    Root Port 4 = Port 4
  ///    Azalia      = Port 5
  ///
  ///
  /// Get BAR registers
  ///
  PchRootComplexBar = MmPci32 (0, 0, 31, 0, 0xF0) &~BIT0;
  PciExpressBar     = McD0PciCfg64 (R_SA_PCIEXBAR) & (B_SA_PCIEXBAR_PCIEXBAR_MASK | B_SA_PCIEXBAR_ADMSK128_MASK | B_SA_PCIEXBAR_ADMSK64_MASK);
  EgressPortBar     = McD0PciCfg64 (R_SA_PXPEPBAR) &~BIT0;
  MchBar            = McD0PciCfg64 (R_SA_MCHBAR) &~BIT0;
  DmiBar            = McD0PciCfg64 (R_SA_DMIBAR) &~BIT0;

  ///
  /// Configure the Egress Port (0) in CID1
  ///
  Status = Cid1EgressPort0Init (EgressPortBar);

#ifdef PEG_FLAG
  if ((CpuFamilyId == EnumCpuHsw) || (CpuFamilyId == EnumCpuCrw)) {
    ///
    /// Configure the PEG Ports in CID1
    ///
    Status = Cid1PegPortInit (DxePlatformSaPolicy);
  }
#endif // PEG_FLAG

  ///
  /// Configure the SA DMI Port (1) in CID1 and the PCH DMI Port (0) in CID2.
  /// Note that both links must be fully configured before the link
  /// should be checked for negotiation between the 2 CIDs.
  ///
  DEBUG ((EFI_D_INFO, " Going to call Cid1Cid2DmiPortInit\n"));
  Status = Cid1Cid2DmiPortInit (DmiBar, DxePlatformSaPolicy);

  ///
  /// Configure the RootComplex Topology in CID1.
  ///
  DEBUG ((EFI_D_INFO, " Going to call Cid1TopologyInit\n"));
  Status = Cid1TopologyInit (EgressPortBar, DmiBar, PchRootComplexBar);

  ///
  /// Configure the RootComplex Topology in CID2.
  ///
  DEBUG ((EFI_D_INFO, " Going to call Cid2TopologyInit\n"));
  Status = Cid2TopologyInit (PchRootComplexBar, DmiBar);

  if (((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId >= EnumHswC0)) ||
      ((CpuFamilyId == EnumCpuCrw) && (CpuSteppingId >= EnumCrwC0))) {
    if (DxePlatformSaPolicy->Revision >= DXE_SA_PLATFORM_POLICY_PROTOCOL_REVISION_7) {
      mSaGlobalNvsAreaProtocol.Area->C7Allowed  = DxePlatformSaPolicy->PcieConfig->C7Allowed; //Update the Run-time C7 Allowed for ASL usage
    }
  }

  mSaGlobalNvsAreaProtocol.Area->PackageCstateLimit  = (UINT8)(AsmReadMsr64(MSR_PMG_CST_CONFIG) & B_PACKAGE_C_STATE_LIMIT);


  return Status;
}

UINT32
PcieFindCapId (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function,
  IN UINT8   CapId
  )
/**
  Find the Offset to a given Capabilities ID
  CAPID list:
    0x01 = PCI Power Management Interface
    0x04 = Slot Identification
    0x05 = MSI Capability
    0x10 = PCI Express Capability

  @param[in] Bus       -   Pci Bus Number
  @param[in] Device    -   Pci Device Number
  @param[in] Function  -   Pci Function Number
  @param[in] CapId     -   CAPID to search for

  @retval 0       - CAPID not found
  @retval Other   - CAPID found, Offset of desired CAPID
**/
{
  UINT8 CapHeader;

  ///
  /// Always start at Offset 0x34
  ///
  CapHeader = MmPci8 (0, Bus, Device, Function, PCI_CAPP);
  if (CapHeader == 0xFF) {
    return 0;
  }

  while (CapHeader != 0) {
    ///
    /// Bottom 2 bits of the pointers are reserved per PCI Local Bus Spec 2.2
    ///
    CapHeader &= ~(BIT1 + BIT0);
    ///
    /// Search for desired CapID
    ///
    if (MmPci8 (0, Bus, Device, Function, CapHeader) == CapId) {
      return CapHeader;
    }

    CapHeader = MmPci8 (0, Bus, Device, Function, CapHeader + 1);
  }

  return 0;
}

UINT32
PcieFindExtendedCapId (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function,
  IN UINT16  CapId
  )
/**
  Search and return the offset of desired Pci Express Capability ID
  CAPID list:
    0x0001 = Advanced Error Rreporting Capability
    0x0002 = Virtual Channel Capability
    0x0003 = Device Serial Number Capability
    0x0004 = Power Budgeting Capability

  @param[in] Bus       -   Pci Bus Number
  @param[in] Device    -   Pci Device Number
  @param[in] Function  -   Pci Function Number
  @param[in] CapId     -   Extended CAPID to search for

  @retval 0       - CAPID not found
  @retval Other   - CAPID found, Offset of desired CAPID
**/
{
  UINT16  CapHeaderOffset;
  UINT16  CapHeaderId;

  ///
  /// Start to search at Offset 0x100
  /// Get Capability Header
  ///
  CapHeaderId     = 0;
  CapHeaderOffset = 0x100;

  while (CapHeaderOffset != 0 && CapHeaderId != 0xFFFF) {
    ///
    /// Search for desired CapID
    ///
    CapHeaderId = MmPci16 (0, Bus, Device, Function, CapHeaderOffset);
    if (CapHeaderId == CapId) {
      return CapHeaderOffset;
    }

    CapHeaderOffset = (MmPci16 (0, Bus, Device, Function, CapHeaderOffset + 2) >> 4);
  }

  return 0;
}

VOID
PcieSetClkreq (
  IN  UINT8   Bus,
  IN  UINT8   Device
  )
/**
  This function enables the CLKREQ# PM on all the end point functions

  @param[in] Bus       -   Pci Bus Number
  @param[in] Device    -   Pci Device Number
**/
{
  UINT8   Function;
  UINT32  CapOffset;

  ///
  /// Parse through all the functions of the endpoint and find the PCIe Cap ID (offset 10h) and if
  /// exists then enable the CLKREQ# bit (BIT8) on that function
  ///
  for (Function = 0; Function < 8; Function++) {
    ///
    /// Find the PCIe Cap Id (offset 10h)
    ///
    CapOffset = PcieFindCapId (Bus, Device, Function, EFI_PCI_CAPABILITY_ID_PCIEXP);
    if (CapOffset == 0) {
      continue;
    }
    ///
    /// Check if CLKREQ# is supported by the endpoints
    ///
    if ((MmPci32 (0, Bus, Device, Function, (CapOffset + 0x0C)) & BIT18) != BIT18) {
      ///
      /// CLKREQ# is not supported so dont do anything
      ///
      return ;
    }
  }
  ///
  /// Now enable the CLKREQ#
  ///
  for (Function = 0; Function < 8; Function++) {
    ///
    /// Find the PCIe Cap Id (offset 10h)
    ///
    CapOffset = PcieFindCapId (Bus, Device, Function, EFI_PCI_CAPABILITY_ID_PCIEXP);
    if (CapOffset == 0) {
      continue;
    }

    MmPci16Or (0, Bus, Device, Function, (CapOffset + 0x010), BIT8);
  }
}

VOID
AdditionalDMIProgramStepsBeforeASPM (
  IN UINT64                          DmiBar,
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
  )
/**
  Additional DMI Programming Steps as in SA BIOS spec

  @param[in] DmiBar              - DMIBAR Address
  @param[in] DxePlatformSaPolicy - SA DxePlatformPolicy protocol
**/
{
  UINT32             Data32And;
  UINT32             Data32Or;

  ///
  /// Configure the De-emphasis control on DMI
  ///
  Data32And = (UINT32) ~BIT6;
  Data32Or  = (UINT32) ((DxePlatformSaPolicy->PcieConfig->DmiDeEmphasis & 0x1) << 6);
  Mmio32AndThenOr (DmiBar, R_SA_DMIBAR_LCTL2_OFFSET, Data32And, Data32Or);

  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (DmiBar + R_SA_DMIBAR_LCTL2_OFFSET),
    &Data32Or,  ///< Data to be ORed
    &Data32And  ///< Data to be ANDed
    );

  ///
  /// Set 0x22C[31] to 1 before enabling ASPM
  ///
  Data32And = (UINT32) ~(BIT31);
  Data32Or  = (UINT32)   BIT31;
  Mmio32AndThenOr (DmiBar, R_SA_DMIBAR_L0SLAT_OFFSET, Data32And, Data32Or);

  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (DmiBar + R_SA_DMIBAR_L0SLAT_OFFSET),
    &Data32Or,  ///< Data to be ORed
    &Data32And  ///< Data to be ANDed
    );

  ///
  /// Set 0x238 bit29 for DMI before enabling ASPM
  ///
  Data32And = 0xFFFFFFFF;
  Data32Or  = BIT29;
  Mmio32Or (DmiBar, 0x238, Data32Or);

  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (DmiBar + 0x238),
    &Data32Or,  ///< Data to be ORed
    &Data32And  ///< Data to be ANDed
    );

  ///
  /// Set DMI Offset 0xC28 [4:0]
  ///
  Data32And = 0xFFFFFFE0;
  Data32Or  = 0x13;
  Mmio32AndThenOr (DmiBar, R_SA_PEG_AFE_PM_TMR_OFFSET, Data32And, Data32Or);

  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (DmiBar + R_SA_PEG_AFE_PM_TMR_OFFSET),
    &Data32Or,  ///< Data to be ORed
    &Data32And  ///< Data to be ANDed
    );
}

VOID
AdditionalPEGProgramStepsBeforeASPM (
  IN  UINT8                          Bus,
  IN  UINT8                          pegDev,
  IN  UINT8                          pegFn,
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
  )
/**
  Additional PEG Programming Steps as in SA BIOS spec

  @param[in] Bus         - Pci Bus Number
  @param[in] pegDev      - Pci Device Number
  @param[in] pegFn       - Pci Func Number
  @param[in] DxePlatformSaPolicy - SA DxePlatformPolicy protocol
**/
{
  UINT32             Data32Or;
  UINT32             Data32And;
  UINT32             PegBaseAddress;

  PegBaseAddress = (UINT32) MmPciAddress (0x0, Bus, pegDev, pegFn, 0x0);

  ///
  /// Permanently set PEGUESEV[CTS](0x1CC bit14) to 1b during BIOS boot for all the PEG controllers
  ///
  Data32And = 0xFFFFFFFF;
  Data32Or  = BIT14;
  Mmio32Or (PegBaseAddress, 0x1CC, Data32Or);

  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (PegBaseAddress + 0x1CC),
    &Data32Or,  ///< Data to be ORed
    &Data32And  ///< Data to be ANDed
    );

  ///
  /// Configure the De-emphasis control on PEG
  ///
  Data32And = (UINT32)~BIT6;
  Data32Or  = (DxePlatformSaPolicy->PcieConfig->PegDeEmphasis[pegFn] & BIT0) << 6;
  Mmio32AndThenOr (PegBaseAddress, R_SA_PEG_LCTL2_OFFSET, Data32And, Data32Or);

  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (PegBaseAddress + R_SA_PEG_LCTL2_OFFSET),
    &Data32Or,  ///< Data to be ORed
    &Data32And  ///< Data to be ANDed
    );

  ///
  /// Set 0x22C[31] to 1 before enabling ASPM
  ///
  Data32And = (UINT32) ~(BIT31);
  Data32Or  = (UINT32)   BIT31;
  Mmio32AndThenOr (PegBaseAddress, R_SA_PEG_L0SLAT_OFFSET, Data32And, Data32Or);

  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (PegBaseAddress + R_SA_PEG_L0SLAT_OFFSET),
    &Data32Or,  ///< Data to be ORed
    &Data32And  ///< Data to be ANDed
    );

  ///
  /// Program 0x250 bit[22:20] to 010b for all the PEG controllers before enabling ASPM
  ///
  Data32And = (UINT32)~(BIT22 + BIT21 + BIT20);
  Data32Or  = 0x02 << 20;
  Mmio32AndThenOr (PegBaseAddress, 0x250, Data32And, Data32Or);

  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (PegBaseAddress + 0x250),
    &Data32Or,  ///< Data to be ORed
    &Data32And  ///< Data to be ANDed
    );

  ///
  /// Set 0x238 bit29 for PEG controller before enabling ASPM
  ///
  Data32And = 0xFFFFFFFF;
  Data32Or  = BIT29;
  Mmio32Or (PegBaseAddress, 0x238, Data32Or);

  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (PegBaseAddress + 0x238),
    &Data32Or,  ///< Data to be ORed
    &Data32And  ///< Data to be ANDed
    );

  ///
  /// Set PEG Offset 0x1F8 bit16
  ///
  Data32And = 0xFFFFFFFF;
  Data32Or  = BIT16;
  Mmio32Or (PegBaseAddress, 0x1F8, Data32Or);

  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (PegBaseAddress + 0x1F8),
    &Data32Or,  ///< Data to be ORed
    &Data32And  ///< Data to be ANDed
    );

  ///
  /// Set PEG Offset 0xC28 [4:0]
  ///
  Data32And = 0xFFFFFFE0;
  Data32Or  = 0x13;
  Mmio32AndThenOr (PegBaseAddress, R_SA_PEG_AFE_PM_TMR_OFFSET, Data32And, Data32Or);

  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (PegBaseAddress + R_SA_PEG_AFE_PM_TMR_OFFSET),
    &Data32Or,  ///< Data to be ORed
    &Data32And  ///< Data to be ANDed
    );
}

