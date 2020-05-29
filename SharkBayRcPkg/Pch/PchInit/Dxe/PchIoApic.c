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
  PchIoApic.c
  
@brief
  Initializes PCH IO APIC Device. 

**/
#include "PchInit.h"

/**
  Configure IoApic Controler

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar address of this PCH device

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
ConfigureIoApic (
  IN  DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN  UINT32                              RootComplexBar
  )
{
  UINT8               RegData8;
  UINT16              RegData16;
  UINT32              RegData32;
  UINTN               PciD31F0RegBase;
  UINT32              IoApicAddress;
  PCH_IO_APIC_CONFIG  *IoApicConfig;
  PCH_LPC_HPET_CONFIG *HpetConfig;
  UINT8               Index;

  DEBUG ((EFI_D_INFO, "ConfigureIoApic() Start\n"));

  ///
  /// Get LPC base address
  ///
  PciD31F0RegBase = MmPciAddress (0, PchPlatformPolicy->BusNumber, 31, 0, 0);
  IoApicConfig    = PchPlatformPolicy->IoApicConfig;

  ///
  /// PCH BIOS Spec Rev 0.5.0, Section 6.6.2.1
  /// 1. Enable the IOAPIC by setting the APIC Enable bit, RCBA + offset 31FFh, Bit[0] if the
  ///    system needs to use the IOxAPIC. The APIC Enable bits needs read back after the bit is written.
  /// Done in PchInitPeim.c PchIoApicInit()
  ///
  /// 2. Build the MP table and/or ACPI APIC table for the OS
  /// This will be done in ACPI code.
  ///
  /// 3. Maximum Redirection Entries (MRE) in APIC Version Register (VER), offset 01h,
  ///    [23:16] has to be written once for Microsoft Windows OS.
  /// The address bits 19:12 of IOAPIC INDEX and DATA are programmable
  /// through OIC register at RCBA + 31FEh[7:0].
  ///
  IoApicAddress = (UINT32) MmioRead8 (RootComplexBar + R_PCH_RCRB_OIC);
  IoApicAddress = IoApicAddress << N_PCH_IO_APIC_ASEL;
  RegData8      = 0x01;
  MmioWrite8 ((UINTN) (R_PCH_IO_APIC_INDEX | IoApicAddress), RegData8);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint8,
    (UINTN) (R_PCH_IO_APIC_INDEX | IoApicAddress),
    1,
    &RegData8
    );
  RegData32 = 0x170020;
  if (GetPchSeries() == PchLp) {
    if (IoApicConfig->IoApicEntry24_39 == PCH_DEVICE_ENABLE) {
      RegData32 = 0x270020;
    }
  }
  MmioWrite32 ((R_PCH_IO_APIC_DATA | IoApicAddress), RegData32);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (R_PCH_IO_APIC_DATA | IoApicAddress),
    1,
    &RegData32
    );

  ///
  /// Program this field to provide a unique bus:device:function number for the internal IOxAPIC
  ///
  if (IoApicConfig->BdfValid) {
    RegData16 = ((UINT16) (IoApicConfig->BusNumber) << 8) & B_PCH_LPC_IOXAPIC_BUS;
    RegData16 |= ((UINT16) (IoApicConfig->DeviceNumber) << 3) & B_PCH_LPC_IOXAPIC_DEVICE;
    RegData16 |= (UINT16) (IoApicConfig->FunctionNumber) & B_PCH_LPC_IOXAPIC_FUNC;
    MmioWrite16 ((UINTN) (PciD31F0RegBase + R_PCH_LPC_IOXAPIC), RegData16);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint16,
      (UINTN) (PciD31F0RegBase + R_PCH_LPC_IOXAPIC),
      1,
      &RegData16
      );
  }
  ///
  /// Program this field accordingly if unique bus:device:function number is required for the
  /// corresponding HPET
  ///
  HpetConfig = PchPlatformPolicy->HpetConfig;
  if (HpetConfig->BdfValid) {
    for (Index = 0; Index < PCH_HPET_BDF_MAX; Index++) {
      RegData16 = ((UINT16) (HpetConfig->Hpet[Index].BusNumber) << 8) & B_PCH_LPC_HPET0_BUS;
      RegData16 |= ((UINT16) (HpetConfig->Hpet[Index].DeviceNumber) << 3) & B_PCH_LPC_HPET0_DEVICE;
      RegData16 |= (UINT16) (HpetConfig->Hpet[Index].FunctionNumber) & B_PCH_LPC_HPET0_FUNC;
      MmioWrite16 ((UINTN) (PciD31F0RegBase + R_PCH_LPC_HPET0 + Index * 2), RegData16);
      SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint16,
        (UINTN) (PciD31F0RegBase + R_PCH_LPC_HPET0 + Index * 2),
        1,
        &RegData16
        );
    }
  }

  DEBUG ((EFI_D_INFO, "ConfigureIoApic() End\n"));

  return EFI_SUCCESS;
}
