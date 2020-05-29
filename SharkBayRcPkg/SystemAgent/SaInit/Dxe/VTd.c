/** @file
  This code provides a initialization of intel VT-d (Virtualization Technology for Directed I/O).

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

@todo assumption is ANDD table will use device path to be loaded dynamically, need to use pchplatformpolicy if assumption is incorrect.
**/
#include "SaInit.h"
#include "VTd.h"
#include <TisPc.h>
#include <IndustryStandard/Acpi30.h>

#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Protocol/FirmwareVolume.h>
#include <Protocol/AcpiTable.h>

UINT32                           mPchRootComplexBar;
DXE_PLATFORM_SA_POLICY_PROTOCOL  *mDxePlatformSaPolicy;
DXE_PCH_PLATFORM_POLICY_PROTOCOL *mDxePlatformPchPolicy;
PCH_SERIES                       mPchSeries;

/**
For device that specified by Device Num and Function Num,
mDevEnMap is used to check device presence.
  0x80 means use Device ID to detemine presence

 The structure is used to check if device scope is valid when update DMAR table
**/
UINT16  mDevEnMap[][2] = {
  {
    0x0200,
    0x80
  },  ///< D2F0
  {
    0x1D00,
    0x80
  },  ///< D29F0
  {
    0x1A00,
    0x80
  },  ///< D26F0
  {
    0x1400,
    0x80
  }   ///< D20F0
};

BOOLEAN mInterruptRemappingSupport;

UINT16
GetFunDisableBit (
  UINT8 DevNum,
  UINT8 FunNum
  )
/**
    Get the corresponding device Enable/Disable bit according DevNum and FunNum

    @param[in] DevNum  - Device Number
    @param[in] FunNum  - Function Number

    @retval If the device is found, return disable/Enable bit in FD/Deven reigster
    @retval If not found return 0xFF
**/
{
  UINTN Index;

  for (Index = 0; Index < sizeof (mDevEnMap) / 4; Index++) {
    if (mDevEnMap[Index][0] == ((DevNum << 0x08) | FunNum)) {
      return mDevEnMap[Index][1];
    }
  }

  return 0xFF;
}

VOID
UpdateDRHD (
  IN OUT VOID *DrhdEnginePtr
  )
/**
    Update the DRHD structure

    @param[in, out] DrhdEnginePtr       - A pointer to DRHD structure
**/
{
  UINT16                        Length;
  UINT16                        DisableBit;
  UINTN                         DeviceScopeNum;
  BOOLEAN                       NeedRemove;
  EFI_ACPI_DRHD_ENGINE1_STRUCT  *DrhdEngine;

  //
  // Convert DrhdEnginePtr to EFI_ACPI_DRHD_ENGINE1_STRUCT Pointer
  //
  DrhdEngine      = (EFI_ACPI_DRHD_ENGINE1_STRUCT *) DrhdEnginePtr;

  Length          = DrhdEngine->Length;
  DeviceScopeNum  = (DrhdEngine->Length - EFI_ACPI_DRHD_ENGINE_HEADER_LENGTH) / sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);
  DisableBit = GetFunDisableBit (
                DrhdEngine->DeviceScope[0].PciPath[0],
                DrhdEngine->DeviceScope[0].PciPath[1]
                );
  NeedRemove = FALSE;
  if ((DisableBit == 0xFF) ||
      (DrhdEngine->RegisterBaseAddress == 0) ||
      ((DisableBit == 0x80) &&
      (MmPci32 (0, 0, DrhdEngine->DeviceScope[0].PciPath[0], DrhdEngine->DeviceScope[0].PciPath[1], 0x00) == 0xFFFFFFFF))
      ){
        NeedRemove = TRUE;
  }
  if (NeedRemove) {
    Length -= sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);
  }
  ///
  /// If no devicescope is left, we set the structure length as 0x00
  ///
  if ((Length > EFI_ACPI_DRHD_ENGINE_HEADER_LENGTH) || (DrhdEngine->Flags == 0x01)) {
    DrhdEngine->Length = Length;
  } else {
    DrhdEngine->Length = 0;
  }
}

UINT8
GetIoApicID (
  VOID
  )
/**
    Get IOAPIC ID from LPC

    @retval APIC ID
**/
{
  UINT8 VOLATILE  *IoapicIndex;
  UINT32 VOLATILE *IoapicData;
  UINT32          Data32;

  ///
  /// Get IOAPIC base
  ///
  IoapicIndex = (UINT8 *) (UINTN) (R_PCH_IO_APIC_INDEX + ((PchMmRcrb16 (R_PCH_RCRB_OIC) & 0x0ff) << 12));
  IoapicData  = (UINT32 *) (UINTN) (R_PCH_IO_APIC_DATA + ((PchMmRcrb16 (R_PCH_RCRB_OIC) & 0x0ff) << 12));

  ///
  /// Get APIC ID from Identification Register (Index 0)
  ///
  *IoapicIndex  = 0;
  Data32        = (*IoapicData & 0x0F000000) >> 24;

  return (UINT8) Data32;
}

VOID
UpdateDRHD2 (
  IN OUT VOID *DrhdEnginePtr
  )
/**
    Update the second DRHD structure

    @param[in, out] DrhdEnginePtr       - A pointer to DRHD structure
**/
{
  UINT16                        Length;
  UINTN                         DeviceScopeNum;
  UINTN                         ValidDeviceScopeNum;
  UINT16                        Data16;
  UINT16                        HpetReg;
  UINT16                        Index;
  UINT8                         Bus;
  UINT8                         Path[2] = { 0, 0 };
  BOOLEAN                       NeedRemove;
  EFI_ACPI_DRHD_ENGINE2_STRUCT  *DrhdEngine;

  ///
  /// Convert DrhdEnginePtr to EFI_ACPI_DRHD_ENGINE2_STRUCT Pointer
  ///
  DrhdEngine      = (EFI_ACPI_DRHD_ENGINE2_STRUCT *) DrhdEnginePtr;

  Length          = DrhdEngine->Length;
  DeviceScopeNum  = (DrhdEngine->Length - EFI_ACPI_DRHD_ENGINE_HEADER_LENGTH) / sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);
  Data16          = 0;
  Bus             = 0;
  HpetReg         = R_PCH_LPC_HPET0;
  ValidDeviceScopeNum = 0;

  for (Index = 0; Index < DeviceScopeNum; Index++) {
    NeedRemove = FALSE;
    /**
      For HPET and APIC, update device scope if Interrupt remapping is supported. remove device scope
      if interrupt remapping is not supported.
      - Index = 0 - IOAPIC
      - Index = 1 - HPET
      For Serial IO devices, they do not appear in PCI space, use platform policy to determine existence, also remove if PCH not LP
      - Index = 2 - I2C0
      - Index = 3 - I2C1
      - Index = 4 - SPI0
      - Index = 5 - SPI1
      - Index = 6 - UART0
      - Index = 7 - UART1
      - Index = 8 - SDIO
    **/
    if (mInterruptRemappingSupport) {
      if (Index == 0) {
        ///
        /// Update source id for IoApic's device scope entry
        ///
        Data16  = PchLpcPciCfg16 (R_PCH_LPC_IOXAPIC);
        Bus     = (UINT8) (Data16 >> 8);
        if (Bus != 0x00) {
          Path[0] = (UINT8) ((Data16 & 0xff) >> 3);
          Path[1] = (UINT8) (Data16 & 0x7);
        } else {
          //
          // BUGBUG: Here we just hardcode, because in this version, R_PCH_LPC_IOXAPIC is initialized AFTER Vtd run.  We can NOT get proper setting from PCH
          // We can NOT get proper setting from PCH
          /// @todo check if code still needed
          //
          DEBUG ((EFI_D_WARN, "BUGBUG: UpdateApicHpet use hardcode value - To be fixed!\n"));
          Bus     = 0xF0;
          Path[0] = 0x1F;
          Path[1] = 0x0;
        }
        DrhdEngine->DeviceScope[Index].StartBusNumber = Bus;
        //
        // Update APIC ID
        //
        DrhdEngine->DeviceScope[Index].EnumId = GetIoApicID ();
      }
      if (Index == 1) {
        ///
        /// Update source id for HPET's device scope entry
        ///
        Data16  = PchLpcPciCfg16 (HpetReg);
        Bus     = (UINT8) (Data16 >> 8);
        Path[0] = (UINT8) ((Data16 & 0xFF) >> 3);
        Path[1] = (UINT8) (Data16 & 0x7);
        DrhdEngine->DeviceScope[Index].StartBusNumber = Bus;
      }
    } else {
      if ((Index == 0) || (Index == 1)) {
        NeedRemove = TRUE;
      }
    }
    /*
    Pch removed device from PCI space and it is visible by ACPI only, we use platform policy to check
    if device is present. If Pch is 2 chip, remove all serialio devices.
    */
#ifdef SERIAL_IO_FLAG
    if (mPchSeries == PchLp){
      if (Index == 2){
        if (mDxePlatformPchPolicy->DeviceEnabling->SerialIoI2c0 == 0){
          NeedRemove = TRUE;
        }
      }
      if (Index == 3){
        if (mDxePlatformPchPolicy->DeviceEnabling->SerialIoI2c1 == 0){
          NeedRemove = TRUE;
        }
      }
      if (Index == 4){
        if (mDxePlatformPchPolicy->DeviceEnabling->SerialIoSpi0 == 0){
          NeedRemove = TRUE;
        }
      }
      if (Index == 5){
        if (mDxePlatformPchPolicy->DeviceEnabling->SerialIoSpi1 == 0){
          NeedRemove = TRUE;
        }
      }
      if (Index == 6){
        if (mDxePlatformPchPolicy->DeviceEnabling->SerialIoUart0 == 0){
          NeedRemove = TRUE;
        }
      }
      if (Index == 7){
        if (mDxePlatformPchPolicy->DeviceEnabling->SerialIoUart1 == 0){
          NeedRemove = TRUE;
        }
      }
      if (Index == 8){
        if (mDxePlatformPchPolicy->DeviceEnabling->SerialIoSdio == 0){
          NeedRemove = TRUE;
        }
      }
    } else {
#endif
      if ((Index >= 2) && (Index <= 8)){
        NeedRemove = TRUE;
      }
#ifdef SERIAL_IO_FLAG
    }
#endif
    CopyMem (
      &DrhdEngine->DeviceScope[ValidDeviceScopeNum],
      &DrhdEngine->DeviceScope[Index],
      sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE)
      );
    if (NeedRemove) {
      Length -= sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);
    } else {
      ValidDeviceScopeNum++;
    }
  }
  ///
  /// If no devicescope is left, we set the structure length as 0x00
  ///
  if ((Length > EFI_ACPI_DRHD_ENGINE_HEADER_LENGTH) || (DrhdEngine->Flags == 0x01)) {
    DrhdEngine->Length = Length;
  } else {
    DrhdEngine->Length = 0;
  }
}

VOID
UpdateRMRR (
  IN OUT VOID *RmrrPtr
  )
/**
    Update the RMRR structure

    @param[in, out] RmrrPtr             - A pointer to RMRR structure
**/
{
  UINT16                  Length;
  UINT16                  DisableBit;
  UINTN                   DeviceScopeNum;
  UINTN                   ValidDeviceScopeNum;
  UINTN                   Index;
  BOOLEAN                 NeedRemove;
  EFI_ACPI_RMRR_USB_STRUC *Rmrr;

  ///
  /// To make sure all devicescope can be checked,
  /// we convert the RmrrPtr to EFI_ACPI_RMRR_USB_STRUC pointer
  ///
  Rmrr                = (EFI_ACPI_RMRR_USB_STRUC *) RmrrPtr;
  Length              = Rmrr->Length;
  ValidDeviceScopeNum = 0;
  DeviceScopeNum      = (Rmrr->Length - EFI_ACPI_RMRR_HEADER_LENGTH) / sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);
  for (Index = 0; Index < DeviceScopeNum; Index++) {
    ///
    /// here we assume Device will exist on PCH if Device Number is greater than 0x0F
    ///
    DisableBit = GetFunDisableBit (
                  Rmrr->DeviceScope[Index].PciPath[0],
                  Rmrr->DeviceScope[Index].PciPath[1]
                  );
    NeedRemove = FALSE;
    if ((DisableBit == 0xFF) ||
        ((DisableBit == 0x80) &&
        (MmPci32 (0, 0, Rmrr->DeviceScope[Index].PciPath[0], Rmrr->DeviceScope[Index].PciPath[1], 0x00) == 0xFFFFFFFF))
        ){
      NeedRemove = TRUE;
    }
    CopyMem (
      &Rmrr->DeviceScope[ValidDeviceScopeNum],
      &Rmrr->DeviceScope[Index],
      sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE)
      );
    if (NeedRemove) {
      Length -= sizeof (EFI_ACPI_DEV_SCOPE_STRUCTURE);
    } else {
      ValidDeviceScopeNum++;
    }
  }
  ///
  /// If No deviceScope is left, set length as 0x00
  ///
  if (Length > EFI_ACPI_RMRR_HEADER_LENGTH) {
    Rmrr->Length = Length;
  } else {
    Rmrr->Length = 0;
  }
}

VOID
DmarTableUpdate (
  IN OUT   EFI_ACPI_DESCRIPTION_HEADER       *TableHeader,
  IN OUT   EFI_ACPI_TABLE_VERSION            *Version
  )
/**
  Update the DMAR table

  @param[in, out] TableHeader         - The table to be set
  @param[in, out] Version             - Version to publish
**/
{
  EFI_ACPI_DMAR_TABLE *DmarTable;
  EFI_ACPI_DMAR_TABLE TempDmarTable;
  UINTN               Offset;
  UINTN               StructureLen;
  UINT16              IgdMode;
  UINT16              GttMode;
  UINT32              IgdMemSize;
  UINT32              GttMemSize;

  IgdMemSize  = 0;
  GttMemSize  = 0;
  DmarTable = (EFI_ACPI_DMAR_TABLE *) TableHeader;

  if (mInterruptRemappingSupport) {
    DmarTable->Flags |= BIT0; ///< Set INTR_REMAP bit (BIT 0) if interrupt remapping is supported
  }
  ///
  /// Find IGD memsize
  ///
  IgdMode = (McD0PciCfg16 (R_SA_GGC) & B_SA_GGC_GMS_MASK) >> 3;
  if (IgdMode <= V_SA_GGC_GMS_512MB) {
    IgdMemSize = IgdMode * 32 * (1024) * (1024);
  } else {
    IgdMemSize = 0;
  }
  ///
  /// Find GTT mem size
  ///
  GttMode = (McD0PciCfg16 (R_SA_GGC) & B_SA_GGC_GGMS_MASK) >> 8;
  if (GttMode <= V_SA_GGC_GGMS_2MB) {
    GttMemSize = GttMode * (1024) * (1024);
  } else {
    GttMemSize = 0;
  }

  DmarTable->RmrrIgd.RmrBaseAddress   = (McD0PciCfg32 (R_SA_TOLUD) &~(0x01)) - IgdMemSize - GttMemSize;
  DmarTable->RmrrIgd.RmrLimitAddress  = DmarTable->RmrrIgd.RmrBaseAddress + IgdMemSize + GttMemSize - 1;
  DEBUG ((EFI_D_INFO, "RMRR Base  address IGD %016lX\n", DmarTable->RmrrIgd.RmrBaseAddress));
  DEBUG ((EFI_D_INFO, "RMRR Limit address IGD %016lX\n", DmarTable->RmrrIgd.RmrLimitAddress));

  DmarTable->RmrrUsb.RmrBaseAddress   = mDxePlatformSaPolicy->Vtd->RmrrUsbBaseAddress[0];
  DmarTable->RmrrUsb.RmrLimitAddress  = mDxePlatformSaPolicy->Vtd->RmrrUsbBaseAddress[1];

  ///
  /// Convert to 4KB alignment.
  ///
  DmarTable->RmrrUsb.RmrBaseAddress &= ~0xFFF;
  DmarTable->RmrrUsb.RmrLimitAddress &= ~0xFFF;
  DmarTable->RmrrUsb.RmrLimitAddress += 0x1000 - 1;

  DEBUG ((EFI_D_INFO, "RMRR Base  address USB %016lX\n", DmarTable->RmrrUsb.RmrBaseAddress));
  DEBUG ((EFI_D_INFO, "RMRR Limit address USB %016lX\n", DmarTable->RmrrUsb.RmrLimitAddress));

  ///
  /// @todo check if this check is still needed.
  ///
  if (DmarTable->RmrrUsb.RmrBaseAddress == 0) {
    DEBUG ((EFI_D_WARN, "BUGBUG:  RmrrUsb.RmrBaseAddress is 0 - To be fixed\n"));
  }
  ///
  /// Update DRHD structures of DmarTable
  ///
  DmarTable->DrhdEngine1.RegisterBaseAddress = (McMmio32 (R_SA_MCHBAR_VTD1_OFFSET) &~1);
  DmarTable->DrhdEngine2.RegisterBaseAddress = (McMmio32 (R_SA_MCHBAR_VTD2_OFFSET) &~1);

  DEBUG ((EFI_D_INFO, "VTD base address1 %x\n", DmarTable->DrhdEngine1.RegisterBaseAddress));
  DEBUG ((EFI_D_INFO, "VTD base address2 %x\n", DmarTable->DrhdEngine2.RegisterBaseAddress));
  ///
  /// copy DmarTable to TempDmarTable to be processed
  ///
  CopyMem (&TempDmarTable, DmarTable, sizeof (EFI_ACPI_DMAR_TABLE));

  ///
  /// Update DRHD structures of temp DMAR table
  ///
  UpdateDRHD (&TempDmarTable.DrhdEngine1);
  UpdateDRHD2 (&TempDmarTable.DrhdEngine2);

  ///
  /// Update RMRR structures of temp DMAR table
  ///
  UpdateRMRR ((VOID *) &TempDmarTable.RmrrUsb);
  UpdateRMRR ((VOID *) &TempDmarTable.RmrrIgd);

  ///
  /// Remove unused device scope or entire DRHD structures
  ///
  Offset = (UINTN) (&TempDmarTable.DrhdEngine1);
  if (TempDmarTable.DrhdEngine1.Length != 0) {
    Offset += TempDmarTable.DrhdEngine1.Length;
  }
  if (TempDmarTable.DrhdEngine2.Length != 0) {
    StructureLen = TempDmarTable.DrhdEngine2.Length;
    CopyMem ((VOID *) Offset, (VOID *) &TempDmarTable.DrhdEngine2, TempDmarTable.DrhdEngine2.Length);
    Offset += StructureLen;
  }
  ///
  /// Remove unused device scope or entire RMRR structures
  ///
  if (TempDmarTable.RmrrUsb.Length != 0) {
    StructureLen = TempDmarTable.RmrrUsb.Length;
    CopyMem ((VOID *) Offset, (VOID *) &TempDmarTable.RmrrUsb, TempDmarTable.RmrrUsb.Length);
    Offset += StructureLen;
  }
  if (TempDmarTable.RmrrIgd.Length != 0) {
    StructureLen = TempDmarTable.RmrrIgd.Length;
    CopyMem ((VOID *) Offset, (VOID *) &TempDmarTable.RmrrIgd, TempDmarTable.RmrrIgd.Length);
    Offset += StructureLen;
  }
#ifdef SERIAL_IO_FLAG
  ///
  /// Include necessary ANDD structures. If not PchLp, remove all ANDD.
  ///
  if (mPchSeries == PchLp){
    if (mDxePlatformPchPolicy->DeviceEnabling->SerialIoI2c0 != 0) {
      StructureLen = TempDmarTable.AnddI2C0.Length;
      CopyMem ((VOID *) Offset, (VOID *) &TempDmarTable.AnddI2C0, TempDmarTable.AnddI2C0.Length);
      Offset += StructureLen;
    }
    if (mDxePlatformPchPolicy->DeviceEnabling->SerialIoI2c1 != 0) {
      StructureLen = TempDmarTable.AnddI2C1.Length;
      CopyMem ((VOID *) Offset, (VOID *) &TempDmarTable.AnddI2C1, TempDmarTable.AnddI2C1.Length);
      Offset += StructureLen;
    }
    if (mDxePlatformPchPolicy->DeviceEnabling->SerialIoSpi0 != 0) {
      StructureLen = TempDmarTable.AnddSpi0.Length;
      CopyMem ((VOID *) Offset, (VOID *) &TempDmarTable.AnddSpi0, TempDmarTable.AnddSpi0.Length);
      Offset += StructureLen;
    }
    if (mDxePlatformPchPolicy->DeviceEnabling->SerialIoSpi1 != 0) {
      StructureLen = TempDmarTable.AnddSpi1.Length;
      CopyMem ((VOID *) Offset, (VOID *) &TempDmarTable.AnddSpi1, TempDmarTable.AnddSpi1.Length);
      Offset += StructureLen;
    }
    if (mDxePlatformPchPolicy->DeviceEnabling->SerialIoUart0 != 0) {
      StructureLen = TempDmarTable.AnddUa00.Length;
      CopyMem ((VOID *) Offset, (VOID *) &TempDmarTable.AnddUa00, TempDmarTable.AnddUa00.Length);
      Offset += StructureLen;
    }
    if (mDxePlatformPchPolicy->DeviceEnabling->SerialIoUart1 != 0) {
      StructureLen = TempDmarTable.AnddUa01.Length;
      CopyMem ((VOID *) Offset, (VOID *) &TempDmarTable.AnddUa01, TempDmarTable.AnddUa01.Length);
      Offset += StructureLen;
    }
    if (mDxePlatformPchPolicy->DeviceEnabling->SerialIoSdio != 0) {
      StructureLen = TempDmarTable.AnddSdhc.Length;
      CopyMem ((VOID *) Offset, (VOID *) &TempDmarTable.AnddSdhc, TempDmarTable.AnddSdhc.Length);
      Offset += StructureLen;
    }
  }
#endif
  Offset = Offset - (UINTN) &TempDmarTable;
  TempDmarTable.Header.Checksum = (UINT8) (TempDmarTable.Header.Checksum + TempDmarTable.Header.Length - Offset);
  TempDmarTable.Header.Length = (UINT32) Offset;
  ///
  /// Replace DMAR table with rebuilt table TempDmarTable
  ///
  CopyMem ((VOID *) DmarTable, (VOID *) &TempDmarTable, TempDmarTable.Header.Length);
}

EFI_STATUS
WaForVc0RemappingEngine (
  UINT64                      MchBar
  )
/**
  Workaround for VC0 remapping engine

  @param[in] MchBar             - MCHBAR address

  @retval EFI_SUCCESS           - successed.
**/
{
  UINT16              DeviceId;
  UINT32              Vc0RemapEngineBase;
  UINT32              Data32Or;
  UINT32              Data32And;

  DeviceId  = PchLpcPciCfg16(R_PCH_LPC_DEVICE_ID);
  Vc0RemapEngineBase = Mmio32(MchBar, R_SA_MCHBAR_VTD2_OFFSET) & 0xFFFFFFFE;

  ///
  /// Disable VTD SuperPage policy when iGfx is enabled
  ///
  if (McD2PciCfg16(R_SA_IGD_VID) != 0xFFFF) {
    Data32And = (UINT32)~(BIT25);
    Data32Or  = 0;
    Mmio32And(Vc0RemapEngineBase, 0xFF0, Data32And);
    SCRIPT_MEM_READ_WRITE
      (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (Vc0RemapEngineBase + 0xFF0),
        &Data32Or,  ///< Data to be ORed
        &Data32And  ///< Data to be ANDed
      );
  }

  return EFI_SUCCESS;
}

VOID
UpdateDmarExitPmAuth (
  VOID
  )
/**
  ExitPmAuth routine for update DMAR
**/
{
  EFI_STATUS                      Status;
  EFI_HANDLE                      *HandleBuffer;
  UINTN                           NumberOfHandles;
  EFI_FV_FILETYPE                 FileType;
  UINT32                          FvStatus;
  EFI_FV_FILE_ATTRIBUTES          Attributes;
  UINTN                           Size;
  UINTN                           i;
  INTN                            Instance;
  EFI_ACPI_TABLE_VERSION          Version;
  EFI_ACPI_COMMON_HEADER          *CurrentTable;
  UINTN                           AcpiTableHandle;
  EFI_FIRMWARE_VOLUME_PROTOCOL    *FwVol;
  EFI_ACPI_TABLE_PROTOCOL         *AcpiTable;
  EFI_ACPI_DESCRIPTION_HEADER     *VtdAcpiTable;
  STATIC BOOLEAN                  Triggered = FALSE;

  if (Triggered) {
    return;
  }

  Triggered     = TRUE;

  FwVol         = NULL;
  AcpiTable     = NULL;
  VtdAcpiTable  = NULL;

  ///
  /// Locate PCH platform policy protocol and PCH series to support feature enabling/disabling
  ///
  Status = gBS->LocateProtocol (
                  &gDxePchPlatformPolicyProtocolGuid,
                  NULL,
                  (VOID**) &mDxePlatformPchPolicy
                  );
  ASSERT_EFI_ERROR (Status);
  mPchSeries  = GetPchSeries();

  if ((!mDxePlatformSaPolicy->Vtd->VtdEnable) || (McD0PciCfg32 (R_SA_MC_CAPID0_A_OFFSET) & BIT23)) {
      DEBUG ((EFI_D_INFO, "Vtd Disabled, skip DMAR Table install\n"));

      return;
  }

  ///
  /// Locate ACPI support protocol
  ///
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **) &AcpiTable);

  ///
  /// Locate protocol.
  /// There is little chance we can't find an FV protocol
  ///
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolumeProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Looking for FV with ACPI storage file
  ///
  for (i = 0; i < NumberOfHandles; i++) {
    ///
    /// Get the protocol on this handle
    /// This should not fail because of LocateHandleBuffer
    ///
    Status = gBS->HandleProtocol (
                    HandleBuffer[i],
                    &gEfiFirmwareVolumeProtocolGuid,
                    (VOID **) &FwVol
                    );
    ASSERT_EFI_ERROR (Status);

    ///
    /// See if it has the ACPI storage file
    ///
    Size      = 0;
    FvStatus  = 0;
    Status = FwVol->ReadFile (
                      FwVol,
                      &gSaAcpiTableStorageGuid,
                      NULL,
                      &Size,
                      &FileType,
                      &Attributes,
                      &FvStatus
                      );

    ///
    /// If we found it, then we are done
    ///
    if (Status == EFI_SUCCESS) {
      break;
    }
  }
  ///
  /// Our exit status is determined by the success of the previous operations
  /// If the protocol was found, Instance already points to it.
  ///
  ///
  /// Free any allocated buffers
  ///
  FreePool (HandleBuffer);

  ///
  /// Sanity check that we found our data file
  ///
  ASSERT (FwVol);
  if (FwVol == NULL) {
    return ;
  }
  ///
  /// By default, a table belongs in all ACPI table versions published.
  ///
  Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;

  ///
  /// Read tables from the storage file.
  ///
  Instance      = 0;
  CurrentTable  = NULL;

  while (Status == EFI_SUCCESS) {
    Status = FwVol->ReadSection (
                      FwVol,
                      &gSaAcpiTableStorageGuid,
                      EFI_SECTION_RAW,
                      Instance,
                      (VOID **) &CurrentTable,
                      &Size,
                      &FvStatus
                      );

    if (!EFI_ERROR (Status)) {
      ///
      /// Check the Signature ID to modify the table
      ///
      switch (((EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable)->Signature) {

      case EFI_ACPI_VTD_DMAR_TABLE_SIGNATURE:
        VtdAcpiTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
        DmarTableUpdate (VtdAcpiTable, &Version);
        break;

      default:
        break;
      }
      ///
      /// Increment the instance
      ///
      Instance++;
      CurrentTable = NULL;
    }
  }
  ///
  /// Update the VTD table in the ACPI tables.
  ///
  AcpiTableHandle = 0;
  if (VtdAcpiTable != NULL) {
    Status = AcpiTable->InstallAcpiTable (
                          AcpiTable,
                          VtdAcpiTable,
                          VtdAcpiTable->Length,
                          &AcpiTableHandle
                          );
  }
}

EFI_STATUS
VtdInit (
  IN  DXE_PLATFORM_SA_POLICY_PROTOCOL    *DxePlatformSaPolicy
  )
/**
@brief
  Locate the VT-d ACPI tables data file and read ACPI SSDT tables.
  Publish the appropriate SSDT based on current configuration and capabilities.

  @param[in] DxePlatformSaPolicy  -  SA DxePlatformPolicy protocol

  @retval EFI_SUCCESS     - Vtd initialization complete
  @exception EFI_UNSUPPORTED - Vtd is not enabled by policy
**/
{
  EFI_STATUS   Status;
  UINTN        i;
  UINT64       MchBar;
  UINT32       Data32Or;
  UINT32       Data32And;
  UINT32       VtdBase;
  UINT32       VtBarReg [SA_VTD_ENGINE_NUMBER];
  CPU_FAMILY   CpuFamilyId;
  CPU_STEPPING CpuStepping;

  mInterruptRemappingSupport  = FALSE;
  mPchRootComplexBar          = MmPci32 (0, 0, 31, 0, 0xF0) &~BIT0;
  MchBar                      = McD0PciCfg64 (R_SA_MCHBAR) &~BIT0;

  VtBarReg[0]                 = R_SA_MCHBAR_VTD1_OFFSET;
  VtBarReg[1]                 = R_SA_MCHBAR_VTD2_OFFSET;
  mDxePlatformSaPolicy        = DxePlatformSaPolicy;

  ///
  /// Check SA supports VTD and VTD is enabled in setup menu
  ///
  if ((!mDxePlatformSaPolicy->Vtd->VtdEnable) || (McD0PciCfg32 (R_SA_MC_CAPID0_A_OFFSET) & BIT23)) {
    DEBUG ((EFI_D_WARN, "VTd disabled or no capability!\n"));
    return EFI_UNSUPPORTED;
  }
  DEBUG ((EFI_D_INFO, "VTd enabled\n"));

  ///
  /// 14.1 Program Remap Engine Base Address
  /// Configure VTD1 BAR
  ///
  i = 0;

  ///
  /// Skip GFXVTBAR if IGD is disabled
  ///
  if (McD2PciCfg16 (R_SA_IGD_VID) != 0xFFFF) {
    Data32Or = mDxePlatformSaPolicy->Vtd->BaseAddress[i];
    Data32Or |= 0x1;
    Mmio32 (MchBar, R_SA_MCHBAR_VTD1_OFFSET) = Data32Or;
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (MchBar + R_SA_MCHBAR_VTD1_OFFSET),
      1,
      &Data32Or
      );
    i++;
  }

  ///
  /// Configure VTD2 BAR
  ///
  Data32Or = mDxePlatformSaPolicy->Vtd->BaseAddress[i];
  Data32Or |= 0x1;
  Mmio32 (MchBar, R_SA_MCHBAR_VTD2_OFFSET) = Data32Or;
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (MchBar + R_SA_MCHBAR_VTD2_OFFSET),
    1,
    &Data32Or
    );

  ///
  /// Workaround for VC0 remapping engine
  ///
  Status = WaForVc0RemappingEngine (MchBar);
  ASSERT_EFI_ERROR (Status);

  for (i = 0; i < SA_VTD_ENGINE_NUMBER; i++) {
    VtdBase = Mmio32 (MchBar, VtBarReg[i]) & 0xfffffffe;

    ///
    /// skip if the VT bar is 0
    ///
    if (VtdBase == 0) {
      continue;
    }

    CpuFamilyId = GetCpuFamily();
    CpuStepping = GetCpuStepping();

    if ((CpuFamilyId == EnumCpuHsw) || (CpuFamilyId == EnumCpuHswUlt) || (CpuFamilyId == EnumCpuCrw)
        ) {
      Data32And = (UINT32) ~(BIT15+BIT16+BIT17+BIT18+BIT19); ///< mask out 19:15
      Data32Or  = BIT15;
      if (i == 1) {
        Mmio32AndThenOr (VtdBase, 0xF04, Data32And, Data32Or);
        SCRIPT_MEM_READ_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint32,
          (UINTN) (VtdBase + 0xF04),
          &Data32Or,  ///< Data to be ORed
          &Data32And  ///< Data to be ANDed
        );
      }
    }
    ///
    /// 14.2 Set the remap engine policy bits
    ///
    Data32And = 0x0; ///< mask out all bits
    Data32Or  = 0;


    if ((CpuFamilyId == EnumCpuHsw) || (CpuFamilyId == EnumCpuHswUlt) || (CpuFamilyId == EnumCpuCrw)) {
      if (i == 0) {
        Data32Or |= 0x02100000;
      }
      if (i == 1) {
        if (McD2PciCfg16 (R_SA_IGD_VID) != 0xFFFF) {
          Data32Or |= 0x000A5003;
        } else {
          Data32Or |= 0x020A5003;
        }
      }
    }

    ///
    /// Set lock bit
    ///
    Data32Or |= BIT31;


    Mmio32AndThenOr (VtdBase, 0xFF0, Data32And, Data32Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (VtdBase + 0xFF0),
      &Data32Or,  ///< Data to be ORed
      &Data32And  ///< Data to be ANDed
      );

    ///
    /// Check IR status
    ///
    if ((Mmio32 (VtdBase, VTD_ECAP_REG) & IR) && !(mInterruptRemappingSupport)) {
      mInterruptRemappingSupport = TRUE;
    }
  }

  return EFI_SUCCESS;
}
