/** @file
  DXE driver for Initializing SystemAgent Graphics initialization.

;******************************************************************************
;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
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
#include "GraphicsInit.h"
#include <Protocol/LegacyBios.h>
#include <Protocol/GopComponentName2.h>

UINT64                          GTTMMADR;
UINTN                           MCHBAR_BASE;
UINT8                           rP1GraphicsFreq;
EFI_EVENT                       mExitPmAuthEvent;

DXE_PLATFORM_SA_POLICY_PROTOCOL *mDxePlatformSaPolicy;
GOP_COMPONENT_NAME2_PROTOCOL *GopComponentName2Protocol = NULL;

///
/// RC6 Settings
///
BOOT_SCRIPT_REGISTER_SETTING  gSaGtRC6Registers[] = {
  0x0,
  0xA090,
  0xFFFFFFFF,
  0x0,
  ///
  /// RC1e - RC6/6p - RC6pp Wake Rate Limits
  ///
  0x0,
  0xA098,
  0xFFFFFFFF,
  0x3E80000,
  0x0,
  0xA09C,
  0xFFFFFFFF,
  0x00280000,
  0x0,
  0xA0A8,
  0xFFFFFFFF,
  0x1E848,
  0x0,
  0xA0AC,
  0xFFFFFFFF,
  0x19,
  ///
  /// Render/Video/Blitter Idle Max Count
  ///
  0x0,
  0x2054,
  0x0,
  0xA,
  0x0,
  0x12054,
  0x0,
  0xA,
  0x0,
  0x22054,
  0x0,
  0xA,
  0x0,
  0x1a054,
  0x0,
  0xA,
  ///
  /// RC Sleep / RCx Thresholds
  ///
  0x0,
  0xA0B0,
  0xFFFFFFFF,
  0,
  0x0,
  0xA0B4,
  0xFFFFFFFF,
  0x3E8,
  0x0,
  0xA0B8,
  0xFFFFFFFF,
  0xC350,
  ///
  /// RP Settings
  ///
  0x0,
  0xA010,
  0xFFFFFFFF,
  0xF4240,
  0x0,
  0xA014,
  0xFFFFFFFF,
  0x12060000,
  0x0,
  0xA02C,
  0xFFFFFFFF,
  0x0000E808,
  0x0,
  0xA030,
  0xFFFFFFFF,
  0x0003BD08,
  0x0,
  0xA068,
  0xFFFFFFFF,
  0x000101D0,
  0x0,
  0xA06C,
  0xFFFFFFFF,
  0x00055730,
  0x0,
  0xA070,
  0xFFFFFFFF,
  0xA
};

///
/// PM Lock Settings
///
BOOT_SCRIPT_REGISTER_SETTING  gSaGtPmLockBits[] = {
  0x0,
  0xA248,
  0xFFFFFFFF,
  BIT31,
  0x0,
  0xA004,
  0xFFFFFFFF,
  BIT4,
  0x0,
  0xA080,
  0xFFFFFFFF,
  BIT2,
  0x0,
  0xA180,
  0xFFFFFFFF,
  BIT31
};

EFI_STATUS
PmInit (
  IN EFI_HANDLE                      ImageHandle,
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
  )
/**
  Initialize GT PowerManagement of SystemAgent.

  @param[in] ImageHandle         - Handle for the image of this driver
  @param[in] DxePlatformSaPolicy - SA DxePlatformPolicy protocol

  @retval EFI_SUCCESS       - GT Power Management initialization complete
**/
{
  UINT8             i;
  UINT32            RegOffset;
  UINT32            Data32;
  UINT32            Data32And;
  UINT32            Data32Or;
  UINT32            Data32Mask;
  UINT32            Result;
  CPU_STEPPING      CpuSteppingId;
  CPU_FAMILY        CpuFamilyId;

  CpuFamilyId   = GetCpuFamily();
  CpuSteppingId = GetCpuStepping();

      ///
      /// Multi Threaded Force Wake
      ///
      RegOffset                     = 0xA180;
      Data32                        = BIT5;
      Mmio32 (GTTMMADR, RegOffset)  = Data32;
      SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (GTTMMADR + RegOffset),
        1,
        &Data32
        );

      RegOffset                     = 0xA188;
      Data32                        = 0x00010001;
      Mmio32 (GTTMMADR, RegOffset)  = Data32;
      SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (GTTMMADR + RegOffset),
        1,
        &Data32
        );
      ///
      /// Force Wake Acknowledge Bit
      ///
      RegOffset                     = 0x130044;
      Data32Mask                    = BIT0;
      Result                        = 1;
      PollGtReady (GTTMMADR, RegOffset, Data32Mask, Result);
      SCRIPT_MEM_POLL (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (GTTMMADR + RegOffset),
        &Data32Mask,
        &Result,
        50,
        200000
        );

    ///
    /// Enable counters except Power Meter
    ///
    Data32And = 0xFFFFFFFF;
    Data32Or  = 0x16;
    Mmio32AndThenOr (GTTMMADR, 0xA248, Data32And, Data32Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (GTTMMADR + 0xA248),
      &Data32Or,  /// Data to be ORed
      &Data32And  /// Data to be ANDed
      );

      ///
      /// GFXPAUSE Settings
      ///
      Data32 = 0x70020;


      if (((CpuFamilyId == EnumCpuHsw)    && (CpuSteppingId < EnumHswC0)   ) ||
          ((CpuFamilyId == EnumCpuHswUlt) && (CpuSteppingId < EnumHswUltC0)) ||
          ((CpuFamilyId == EnumCpuCrw)    && (CpuSteppingId < EnumCrwC0)   )) {
        Data32 |= 0xFFFF;
      }

      Mmio32 (GTTMMADR, 0xA000) = Data32;

      SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (GTTMMADR + 0xA000),
        1,
        &Data32
        );


    ///
    /// ECO Settings
    /// BIT28 = 1 GFX will be blocked from accessing memory (go=0) during CPD enter
    /// BIT26 = 1 indicates to PCU that we are doing a fifo block due to RC6 and not CPD
    /// BIT[24:22] = 100 RC6 Control
    ///
    RegOffset = 0xA180;
    Data32And = 0xFFFFFFFF;
    Data32Or  = BIT28 + BIT26;
    if (DxePlatformSaPolicy->IgdConfig->RenderStandby) {
      Data32And = (UINT32)~(BIT23 + BIT22);
      Data32Or |= BIT24;
    }

    ///
    /// Force CPD Non-IA. for steppings less than C0 for HSW/CRW
    ///
    if (((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId < EnumHswC0)) ||
        ((CpuFamilyId == EnumCpuCrw) && (CpuSteppingId < EnumCrwC0))) {
        Data32Or |= BIT30;
    }
    ///
    /// Force CPD Block memory bits. for stepping less than C0 for HSW/ULT/CRW
    ///
    if (((CpuFamilyId == EnumCpuHsw)    && (CpuSteppingId < EnumHswC0)   ) ||
        ((CpuFamilyId == EnumCpuHswUlt) && (CpuSteppingId < EnumHswUltC0)) ||
        ((CpuFamilyId == EnumCpuCrw)    && (CpuSteppingId < EnumCrwC0)   )) {
        Data32And &= (UINT32)~(BIT28);
        Data32Or  &= (UINT32)~(BIT28);
    }

    Mmio32AndThenOr (GTTMMADR, RegOffset, Data32And, Data32Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (GTTMMADR + RegOffset),
      &Data32Or,  /// Data to be ORed
      &Data32And  /// Data to be ANDed
      );

    ///
    /// Clock Gating Settings
    ///
    Data32 = 0x3FD;
    Mmio32 (GTTMMADR, 0x9424) = Data32;
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (GTTMMADR + 0x9424),
      1,
      &Data32
      );

    ///
    /// Enable Unit Level Clock Gates
    ///
    Data32 = 0x80;
    Mmio32 (GTTMMADR, 0x9400) = Data32;
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (GTTMMADR + 0x9400),
      1,
      &Data32
      );

    Data32 = 0x40401000;
    Mmio32 (GTTMMADR, 0x9404) = Data32;
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (GTTMMADR + 0x9404),
      1,
      &Data32
      );

    Data32 = 0;
    Mmio32 (GTTMMADR, 0x9408) = Data32;
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (GTTMMADR + 0x9408),
      1,
      &Data32
      );

    Data32 = 0x02000001;
    Mmio32 (GTTMMADR, 0x940c) = Data32;
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (GTTMMADR + 0x940c),
      1,
      &Data32
      );

    Data32 = 0x08000000;
    Mmio32 (GTTMMADR, 0xA008) = Data32;

    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (GTTMMADR + 0xA008),
      1,
      &Data32
      );
    ///
    /// RC6 Settings
    ///
    for (i = 0; i < sizeof (gSaGtRC6Registers) / sizeof (BOOT_SCRIPT_REGISTER_SETTING); ++i) {
      RegOffset                     = gSaGtRC6Registers[i].Offset;
      Data32And                     = gSaGtRC6Registers[i].AndMask;
      Data32Or                      = gSaGtRC6Registers[i].OrMask;

      Mmio32AndThenOr (GTTMMADR, RegOffset, Data32And, Data32Or);

    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (GTTMMADR + RegOffset),
      &Data32Or,  /// Data to be ORed
      &Data32And  /// Data to be ANDed
      );
    }

    ///
    /// RP Control
    ///
    Data32                    = 0xB92;
    Mmio32 (GTTMMADR, 0xA024) = Data32;

    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (GTTMMADR + 0xA024),
      1,
      &Data32
      );

    ///
    /// HW RC6 Control Settings
    ///
    Data32 = 0;

    if (DxePlatformSaPolicy->IgdConfig->RenderStandby) {
      Data32 = 0x88040000;
    }

    Mmio32 (GTTMMADR, 0xA090) = Data32;

    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (GTTMMADR + 0xA090),
      1,
      &Data32
      );

    ///
    /// Video frequency request
    ///
    Data32  = 0x08000000;
    Mmio32 (GTTMMADR, 0xA00C) = Data32;
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (GTTMMADR + 0xA00C),
      1,
      &Data32
      );

    ///
    /// RC6 Settings
    ///
    ///
    /// Wait for Mailbox ready
    ///
    Data32Mask  = BIT31;
    Result      = 0;

    PollGtReady (GTTMMADR, 0x138124, Data32Mask, Result);

    SCRIPT_MEM_POLL (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (GTTMMADR + 0x138124),
      &Data32Mask,
      &Result,
      50,
      200000
      );

    ///
    /// Mailbox Data  - RC6 VIDS
    ///
    Data32 = 0x0;
    Mmio32 (GTTMMADR, 0x138128) = Data32;

    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (GTTMMADR + 0x138128),
      1,
      &Data32
      );

    ///
    /// Mailbox Command
    ///
    Data32                      = 0x80000004;
    Mmio32 (GTTMMADR, 0x138124) = Data32;

    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (GTTMMADR + 0x138124),
      1,
      &Data32
      );

    ///
    /// Wait for Mailbox ready
    ///
    Data32Mask  = BIT31;
    Result      = 0;

    PollGtReady (GTTMMADR, 0x138124, Data32Mask, Result);

    SCRIPT_MEM_POLL (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (GTTMMADR + 0x138124),
      &Data32Mask,
      &Result,
      50,
      200000
      );

    ///
    /// Enable PM Interrupts
    ///
    Data32                      = 0x3000076;
    Mmio32 (GTTMMADR, 0x4402C)  = Data32;

    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (GTTMMADR + 0x4402C),
      1,
      &Data32
      );

    ///
    /// RC6 setting
    ///
    if (mDxePlatformSaPolicy->IgdConfig->RenderStandby) {
      ///
      /// Software RC state - RC6
      ///
      Data32                    = 0x40000;
      Mmio32 (GTTMMADR, 0xA094) = Data32;
      SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint8,
        (UINTN) (GTTMMADR + 0xA094),
        1,
        &Data32
        );
    }

      ///
      /// PM Lock Settings
      ///
      for (i = 0; i < sizeof (gSaGtPmLockBits) / sizeof (BOOT_SCRIPT_REGISTER_SETTING); ++i) {
        RegOffset = gSaGtPmLockBits[i].Offset;
        Data32And = gSaGtPmLockBits[i].AndMask;
        Data32Or  = gSaGtPmLockBits[i].OrMask;

        Mmio32AndThenOr (GTTMMADR, RegOffset, Data32And, Data32Or);

        SCRIPT_MEM_READ_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint32,
          (UINTN) (GTTMMADR + RegOffset),
          &Data32Or,  /// Data to be ORed
          &Data32And  /// Data to be ANDed
          );
      }
  return EFI_SUCCESS;
}

EFI_STATUS
PavpInit (
  IN EFI_HANDLE                      ImageHandle,
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
  )
/**
  Initialize PAVP feature of SystemAgent.

  @param[in] ImageHandle         - Handle for the image of this driver
  @param[in] DxePlatformSaPolicy - SA DxePlatformPolicy protocol

  @retval EFI_SUCCESS - PAVP initialization complete
**/
{

  UINT32  DwordData;
  UINT32  PcmBase;
  CPU_STEPPING      CpuSteppingId;
  CPU_FAMILY        CpuFamilyId;

  CpuFamilyId   = GetCpuFamily();
  CpuSteppingId = GetCpuStepping();

    McD0PciCfg32And (R_SA_PAVPC, ~(B_SA_PAVPC_HVYMODSEL_MASK | B_SA_PAVPC_PCMBASE_MASK | B_SA_PAVPC_PAVPE_MASK | B_SA_PAVPC_PCME_MASK));
    McD0PciCfg16Or (R_SA_PAVPC, B_SA_PAVPC_PCME_MASK | B_SA_PAVPC_PAVPE_MASK);
    PcmBase = ((UINT32) RShiftU64 ((McD0PciCfg32 (R_SA_TOLUD)), 20)) - PAVP_PCM_SIZE_1_MB;
    McD0PciCfg32Or (R_SA_PAVPC, (UINT32) LShiftU64 (PcmBase, 20));

    if ((CpuFamilyId == EnumCpuHsw) ||
        (CpuFamilyId == EnumCpuHswUlt) ||
        (CpuFamilyId == EnumCpuCrw)
    ){
      McD0PciCfg32Or (R_SA_PAVPC, (BIT4));
    }

  DwordData = McD0PciCfg32 (R_SA_PAVPC);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (MmPciAddress (0,
            0,
            0,
            0,
            R_SA_PAVPC)),
    1,
    &DwordData
    );

  return EFI_SUCCESS;
}

EFI_STATUS
PostPmInitExitPmAuth (
  VOID
  )
/**
    Do Post GT PM Init Steps after VBIOS Initialization.

  @retval EFI_SUCCESS          Succeed.
**/
{
  UINT32                    RegOffset;
  UINT32                    Data32;
  EFI_STATUS                Status;
  UINT32                    Data32Mask;
  UINT32                    Result;
  UINT16                    Data16;
  CHAR16                    *DriverVersion;
  UINTN                     Index;
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios = NULL;

  ///
  /// Get the platform setup policy.
  ///
  Status = gBS->LocateProtocol (&gDxePlatformSaPolicyGuid, NULL, (VOID **) &mDxePlatformSaPolicy);
  ASSERT_EFI_ERROR (Status);

  ///
  /// only 32bit read/write is legal for device 0:2:0
  ///
  GTTMMADR  = (UINT64) (McD2PciCfg32 (R_SA_IGD_GTTMMADR));
  GTTMMADR  = LShiftU64 ((UINT64) McD2PciCfg32 (R_SA_IGD_GTTMMADR + 4), 32) | (GTTMMADR);

  ///
  /// Save the current GTMMADR value into S3 resume script before other S3 resume items in this function.
  /// GTTMMADR may have been modified by the PCI enumeration code at this point,
  /// but not saved into the S3 resume script yet.
  ///
  ///
  /// only 32bit read/write is legal for device 0:2:0
  ///
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (MmPciAddress (0,
            0,
            2,
            0,
            R_SA_IGD_GTTMMADR)),
    1,
    &GTTMMADR
    );

  ///
  /// only 32bit read/write is legal for device 0:2:0
  ///
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (MmPciAddress (0,
            0,
            2,
            0,
            R_SA_IGD_GTTMMADR + 4)),
    1,
    ((UINT32 *)&GTTMMADR) + 1
    );

  GTTMMADR = GTTMMADR &~(BIT2 | BIT1);

  ///
  /// Enable Bus Master, I/O and Memory access on 0:2:0
  ///
  McD2PciCfg16Or (R_SA_IGD_CMD, (BIT2 | BIT1));
  Data16 = McD2PciCfg16(R_SA_IGD_CMD);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    (UINTN) (MmPciAddress (0, 0, 2, 0, R_SA_IGD_CMD)),
    1,
    &Data16
    );

      ///
      /// Deassert Force Wake
      RegOffset                     = 0xA188;
      Data32                        = 0x00010000;
      Mmio32 (GTTMMADR, RegOffset)  = Data32;

      SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (GTTMMADR + RegOffset),
        1,
        &Data32
        );

      RegOffset                     = 0x130044;
      Data32Mask                    = BIT0;
      Result                        = 0;
      PollGtReady (GTTMMADR, RegOffset, Data32Mask, Result);

      SCRIPT_MEM_POLL (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (GTTMMADR + RegOffset),
        &Data32Mask,
        &Result,
        50,
        200000
        );

      RegOffset                     = 0xA188;
      Data32                        = 0x00000001;
      Mmio32 (GTTMMADR, RegOffset)  = Data32;
      SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (GTTMMADR + RegOffset),
        1,
        &Data32
        );


  Status = gBS->LocateProtocol (
                  &gEfiLegacyBiosProtocolGuid,
                  NULL,
                  (VOID **) &LegacyBios
                  );

  if (!LegacyBios) {
    Status = gBS->LocateProtocol (&gGopComponentName2ProtocolGuid, NULL, (VOID **)&GopComponentName2Protocol);
    if (!EFI_ERROR (Status)) {
      Status = GopComponentName2Protocol->GetDriverVersion (
                                    GopComponentName2Protocol,
                                    "en-US",
                                    &DriverVersion
                                    );
      if (!EFI_ERROR (Status)) {
        for (Index = 0; (DriverVersion[Index] != '\0'); Index++) {
        }
        Index = (Index+1)*2;
        CopyMem(mDxePlatformSaPolicy->IgdConfig->GopVersion, DriverVersion, Index);
      }
    }
  }

  ///
  /// Return final status
  ///
  return EFI_SUCCESS;
}

EFI_STATUS
GraphicsInit (
  IN EFI_HANDLE                      ImageHandle,
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
  )
/**
Initialize GT Post Routines.

  @param[in] ImageHandle         -     Handle for the image of this driver
  @param[in] DxePlatformSaPolicy -     SA DxePlatformPolicy protocol

  @retval EFI_SUCCESS   - GT POST initialization complete
  @retval EFI_NOT_FOUND - Dxe System Table not found.
**/
{
  EFI_PHYSICAL_ADDRESS  MemBaseAddress;
  UINT32                LoGTBaseAddress;
  UINT32                HiGTBaseAddress;
  UINT32                Data32And;
  UINT32                Data32Or;
  EFI_STATUS            Status;
  UINTN                 DwordData;
  UINT32                Data32Mask;
  UINT32                Result;
  UINT16                Data16;
  CPU_FAMILY            CpuFamilyId;
  UINT8                 CpuSteppingId;
  UINT32                Data32;
  UINT16                IsUlx;

  CpuFamilyId   = GetCpuFamily();
  CpuSteppingId = GetCpuStepping();

  GTTMMADR    = 0;
  Status      = EFI_SUCCESS;
  MCHBAR_BASE = McD0PciCfg64 (0x48) &~BIT0;

  ///
  /// Read the RP1 Graphics Frequency
  ///
  rP1GraphicsFreq = (UINT8) ((Mmio32 (MCHBAR_BASE, 0x5998) >> 8) & 0xFF);

  ///
  /// If device 0:2:0 (Internal Graphics Device, or GT) is enabled, then Program GTTMMADR,
  ///
  if (McD2PciCfg16 (R_SA_IGD_VID) != 0xFFFF) {
    gDS     = NULL;
    Status  = EfiGetSystemConfigurationTable (&gEfiDxeServicesTableGuid, (VOID **) &gDS);
    ASSERT_EFI_ERROR (Status);
    ASSERT(gDS != NULL);
    if (gDS == NULL) {
      return EFI_NOT_FOUND;
    }

    ///
    /// Means Allocate 4MB for GTTMADDR
    ///
    MemBaseAddress = 0x0ffffffffffffffff;

    Status = gDS->AllocateMemorySpace (
                    EfiGcdAllocateAnySearchBottomUp,
                    EfiGcdMemoryTypeMemoryMappedIo,
                    GTT_MEM_ALIGN,
                    GTTMMADR_SIZE_4MB,
                    &MemBaseAddress,
                    ImageHandle,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status);

    ///
    /// Program GT PM Settings if GTTMMADR allocation is Successful
    ///
    GTTMMADR                          = (UINTN) MemBaseAddress;

    LoGTBaseAddress                   = (UINT32) (MemBaseAddress & 0xFFFFFFFF);
    HiGTBaseAddress                   = (UINT32) RShiftU64 ((MemBaseAddress & 0xFFFFFFFF00000000), 32);
    McD2PciCfg32 (R_SA_IGD_GTTMMADR)     = LoGTBaseAddress | BIT2;
    McD2PciCfg32 (R_SA_IGD_GTTMMADR + 4) = HiGTBaseAddress;

    DwordData                         = McD2PciCfg32 (R_SA_IGD_GTTMMADR);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (MmPciAddress (0,
              0,
              2,
              0,
              R_SA_IGD_GTTMMADR)),
      1,
      &DwordData
      );

    DwordData = McD2PciCfg32 (R_SA_IGD_GTTMMADR + 4);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (MmPciAddress (0,
              0,
              2,
              0,
              R_SA_IGD_GTTMMADR + 4)),
      1,
      &DwordData
      );

    ///
    /// Enable Bus Master, I/O and Memory access on 0:2:0
    ///
    McD2PciCfg16Or (R_SA_IGD_CMD, (BIT2 | BIT1));
    Data16 = McD2PciCfg16 (R_SA_IGD_CMD);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint16,
      (UINTN) (MmPciAddress (0, 0, 2, 0, R_SA_IGD_CMD)),
      1,
      &Data16
      );

    ///
    /// PAVP Initialization
    ///
      DEBUG ((EFI_D_INFO, "Initializing PAVP\n"));
      PavpInit (ImageHandle, DxePlatformSaPolicy);
    ///
    /// PmInit Initialization
    ///
    DEBUG ((EFI_D_INFO, "Initializing GT PowerManagement\n"));
    PmInit (ImageHandle, DxePlatformSaPolicy);

//[-start-120629-IB06460411-modify]//
//[-start-130401-IB05160428-modify]//
    //if ( DxePlatformSaPolicy->PlatformBoardID == SAPlatformBoardIDBaskingRidge ) {
    if ( DxePlatformSaPolicy->IgdConfig->FdiLaneReversal) {
//[-end-130401-IB05160428-modify]//
// Program FDI Lane Reversal.
      Data32And = 0xFFFFFFFF;
      Data32Or  = BIT16;
      Mmio32AndThenOr (GTTMMADR, 0xF000c, Data32And, Data32Or);
      SCRIPT_MEM_READ_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (GTTMMADR + 0xF000c),
        &Data32Or,  /// Data to be ORed
        &Data32And  /// Data to be ANDed
        );
    }
//[-end-120629-IB06460411-modify]//
    ///
    /// Enable PowerWell for DP and Audio: set Dev2 mmio 45400 bit 31 and poll bit30 (1 means enabled)
    ///
    Data32And = 0xFFFFFFFF;
    Data32Or  = (UINT32) (BIT31);
    Mmio32Or (GTTMMADR, 0x45400, Data32Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (GTTMMADR + 0x45400),
      &Data32Or,  /// Data to be ORed
      &Data32And  /// Data to be ANDed
      );

    Data32Mask  = BIT30;
    Result      = BIT30;
    PollGtReady (GTTMMADR, 0x45400, Data32Mask, Result);
    SCRIPT_MEM_POLL (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (GTTMMADR + 0x45400),
      &Data32Mask,
      &Result,
      50,
      200000
      );

    if (DxePlatformSaPolicy->Revision >=4) {
      IsUlx = 0;
      Data16 = McD2PciCfg16 (0x2);
      if ((Data16 == 0xA0E) || (Data16 == 0xA1E)) {
        IsUlx = 1;
      }
      ///
      /// BIOS to get supported CD frequency
      ///
      if (((MmioRead32 (GTTMMADR + 0x42014) & 0x1000000) != 0) || (IsUlx == 1) || (CpuFamilyId == EnumCpuHswUlt)) {
        ///fixed frequency
        mDxePlatformSaPolicy->IgdConfig->CdClkVar = 0;
      } else {
        ///choice of varying frequency
        mDxePlatformSaPolicy->IgdConfig->CdClkVar = 2;
      }

      if (IsUlx == 1) {
        Data32And = 0xF7FFFFFF;
        Data32Or  = 0x4000000;
      } else {
        if ((mDxePlatformSaPolicy->IgdConfig->CdClk) != 1) {
          if ((mDxePlatformSaPolicy->IgdConfig->CdClkVar != 0)) {
            ///540 or 337.5Mhz
            Data32And = 0xF7FFFFFF;
            Data32Or  = 0x4000000;
          }
        } else {
          ///450 Mhz
          Data32And = 0xF3FFFFFF;
          Data32Or  = 0;
        }
      }
      Mmio32AndThenOr (GTTMMADR, 0x130040, Data32And, Data32Or);
      SCRIPT_MEM_READ_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (GTTMMADR + 0x130040),
        &Data32Or,  /// Data to be ORed
        &Data32And  /// Data to be ANDed
        );

      if ((CpuFamilyId == EnumCpuHswUlt)||(IsUlx == 1)) {
        Data32 = 1;
        if (((mDxePlatformSaPolicy->IgdConfig->CdClk) == 1) && (IsUlx == 0)) {
          Data32 = 0;
        }

        Mmio32 (GTTMMADR, 0x138128) = Data32;
        SCRIPT_MEM_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint32,
          (UINTN) (GTTMMADR + 0x138128),
          1,
          &Data32
          );

        Data32                      = 0x0;
        Mmio32 (GTTMMADR, 0x13812c) = Data32;
        SCRIPT_MEM_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint32,
          (UINTN) (GTTMMADR + 0x13812c),
          1,
          &Data32
          );

        Data32                      = 0x80000017;
        Mmio32 (GTTMMADR, 0x138124) = Data32;
        SCRIPT_MEM_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint32,
          (UINTN) (GTTMMADR + 0x138124),
          1,
          &Data32
          );
      }
    }

    McD2PciCfg16And (R_SA_IGD_CMD, ~(BIT2 | BIT1));
    Data16 = McD2PciCfg16(R_SA_IGD_CMD);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint16,
      (UINTN) (MmPciAddress (0, 0, 2, 0, R_SA_IGD_CMD)),
      1,
      &Data16
      );

    McD2PciCfg64 (R_SA_IGD_GTTMMADR) = 0;
    DwordData = McD2PciCfg64(R_SA_IGD_GTTMMADR);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (MmPciAddress (0, 0, 2, 0, R_SA_IGD_GTTMMADR)),
      1,
      &DwordData
      );

    ///
    /// Free allocated resources
    ///
    gDS->FreeMemorySpace (MemBaseAddress, GTTMMADR_SIZE_4MB);
  }

  ///
  /// Lock PAVPC Register
  ///
  McD0PciCfg32Or (R_SA_PAVPC, B_SA_PAVPC_PAVPLCK_MASK);

  DwordData = McD0PciCfg32 (R_SA_PAVPC);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (MmPciAddress (0,
                           0,
                           0,
                           0,
                           R_SA_PAVPC)),
    1,
    &DwordData
    );

  return EFI_SUCCESS;
}

VOID
PollGtReady (
  UINT64 Base,
  UINT32 Offset,
  UINT32 Mask,
  UINT32 Result
  )
/**
  "Poll Status" for GT Readiness

  @param[in] Base            - Base address of MMIO
  @param[in] Offset          - MMIO Offset
  @param[in] Mask            - Mask
  @param[in] Result          - Value to wait for
**/
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
    gBS->Stall (1000);
    StallCount = StallCount + 1;
  }

  ASSERT ((StallCount != GT_WAIT_TIMEOUT));
}
