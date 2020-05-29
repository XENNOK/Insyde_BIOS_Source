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
  PcieComplex.c

  This file will perform SA PCIE Root Complex initialization.

**/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/SaPcieLib.h>
#include "PcieComplex.h"
#include "SaBuildFlags.h"
#include "PciExpressInit.h"
//#include "EdkIIGluePcdPciExpressLib.h"

///
/// Global variables
///
UINT8                                  HwStrap;
extern UINT16                          mSaIotrapSmiAddress;
extern BOOLEAN                         mInitPcieAspmAfterOprom;
extern DXE_PLATFORM_SA_POLICY_PROTOCOL *mDxePlatformSaPolicy;

#ifdef PEG_FLAG
PEG_PORT_DEVICE PegDeviceTable[] = {
  ///
  ///          Bus,           Device,         Function, Slot, Bus2, Device2, Function2
  ///
  { SA_PEG_BUS_NUM, SA_PEG10_DEV_NUM, SA_PEG10_FUN_NUM,    1,    2,       0,         0 },
  { SA_PEG_BUS_NUM, SA_PEG11_DEV_NUM, SA_PEG11_FUN_NUM,    2,    3,       0,         0 },
  { SA_PEG_BUS_NUM, SA_PEG12_DEV_NUM, SA_PEG12_FUN_NUM,    3,    4,       0,         0 }
};
#endif // PEG_FLAG

///
/// Functions
///
VOID
EFIAPI
SaLateInitSmiCallback (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
/**

    This function gets registered as a callback to perform all SA late initialization

    @param[in] Event     - A pointer to the Event that triggered the callback.
    @param[in] Context   - A pointer to private data registered with the callback function.

**/
{
  if (mSaIotrapSmiAddress != 0) {
    DEBUG ((EFI_D_INFO, "[SA] Issue IOTRAP SMI %X\n", mSaIotrapSmiAddress));
    IoWrite8 (mSaIotrapSmiAddress, 0);
  }
  if (Event != NULL) {
    gBS->CloseEvent(Event);
  }
  return;
}

EFI_STATUS
Cid1EgressPort0Init (
  IN  UINT64      EgressPortBar
  )
/**

    Perform Egress Port 0 Initialization.

    @param[in] EgressPortBar   - EPBAR Address

    @retval EFI_SUCCESS        - Egress Port 0 initialization successed.

**/
{
  UINT32  Data32And;
  UINT32  Data32Or;
  UINT8   Data8And;
  UINT8   Data8Or;
  UINT8   BitMask;
  UINT8   BitValue;

  ///
  /// Egress Port Configuration
  ///
  /// Egress Port Virtual Channel 0 Configuration
  ///   System BIOS must insure that only TC0 is mapped to VC0.
  ///   a. Set the Egress Port Register EPBAR offset 014h[7:1]=0000000b
  ///
  Data32And = BIT0;
  Data32Or  = 0;
  Mmio32And (EgressPortBar, 0x14, Data32And);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (EgressPortBar + 0x14),
    &Data32Or,
    &Data32And
    );

  ///
  /// System BIOS must program the extended VC count field.
  ///   b. Set the Egress Port Register EPBAR offset 004h[2:0]=001b
  ///
  Data8And  = (UINT8)~(0x07);
  Data8Or   = BIT0;
  Mmio8AndThenOr (EgressPortBar, 0x4, Data8And, Data8Or);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint8,
    (UINTN) (EgressPortBar + 0x4),
    &Data8Or,
    &Data8And
    );

  ///
  /// Egress Port VC1 Configuration
  ///   a. Assign Virtual Channel ID 1 to VC1: by programming PXEPBAR Offset 020[26:24] = '001b'
  ///   b. Select and map TC1 to VC1: by programming PXPEPBAR Offset 020h[7:1] = '0000001b'
  ///
  Data32And = (UINT32)~(0x070000FE);
  Data32Or  = ((0x01 << 24) + BIT1);
  Mmio32AndThenOr (EgressPortBar, 0x20, Data32And, Data32Or);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (EgressPortBar + 0x20),
    &Data32Or,
    &Data32And
    );

  ///
  /// c. Enable VC1 (no hardware behind this bit, s/w compatibility flag only) BIT31
  ///    Program EXPEPBAR Offset 020h[31]=1
  ///
  Data32And = 0xFFFFFFFF;
  Data32Or  = BIT31;
  Mmio32Or (EgressPortBar, 0x20, Data32Or);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (EgressPortBar + 0x20),
    &Data32Or,
    &Data32And
    );

  ///
  /// d. Poll the VC1 Negotiation Pending bit until it reads 0:
  ///    Read the Egress Port Register EPBAR Offset 026h until [1]==0
  ///
  BitMask   = (UINT8) BIT1;
  BitValue  = 0;
  while ((Mmio8 (EgressPortBar, 0x26) & BitMask) != BitValue) {
  };
  SCRIPT_MEM_POLL (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint8,
    (UINTN) (EgressPortBar + 0x26),
    &BitMask,
    &BitValue,
    50,
    200000
    );

  return EFI_SUCCESS;
}

#ifdef PEG_FLAG
EFI_STATUS
Cid1PegPortInit (
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
  )
/**

    Conditionally perform PEG Port Initialization.
    bugbug: organize this code in a way that can utilize the
    PchS3ItemTypeInitPcieRootPortDownstream EFI_PCH_S3_DISPATCH_ITEM_TYPE

    @param[in] DxePlatformSaPolicy - SA DxePlatformPolicy protocol

    @retval EFI_SUCCESS            - PEG Port initialization successed.

**/
{
  UINT32              Data32;
  UINT32              Data32Or;
  UINT32              Data32And;
  UINT16              Data16Or;
  UINT16              Data16And;
  UINT8               Data8;
  UINT32              PegBaseAddress;
  UINT8               Bus;
  UINT8               Dev;
  UINT8               Func;
  UINT8               Slot;
  UINT8               Bus2;
  UINT8               Dev2;
  UINT8               Func2;
  UINT8               PegComplete;
  CPU_FAMILY          CpuFamilyId;
  CPU_STEPPING        CpuSteppingId;

  CpuFamilyId   = GetCpuFamily();
  CpuSteppingId = GetCpuStepping();

  ///
  /// Read HwStrap Register - PEG1CFGSEL D1.R 504h [17:16]
  ///
  HwStrap = (UINT8) ((McD1PciCfg32 (R_SA_PEG_FUSESCMN_OFFSET) & (BIT17 + BIT16)) >> 16);

  ///
  /// HSW - Scan/initialize PEG devices based on HW strapping.
  ///
  for (PegComplete = 0; PegComplete < ((sizeof (PegDeviceTable)) / (sizeof (PEG_PORT_DEVICE))); PegComplete++) {
    ///
    /// Get Peg Device BDF, Slot# and EndPoint(Temporary)
    ///
    Bus             = PegDeviceTable[PegComplete].Bus;
    Dev             = PegDeviceTable[PegComplete].Device;
    Func            = PegDeviceTable[PegComplete].Function;
    Slot            = PegDeviceTable[PegComplete].Slot;
    Bus2            = PegDeviceTable[PegComplete].Bus2;
    Dev2            = PegDeviceTable[PegComplete].Device2;
    Func2           = PegDeviceTable[PegComplete].Function2;

    PegBaseAddress  = (UINT32) MmPciAddress (0, Bus, Dev, Func, 0);
    ///
    /// Check if the PEG is Enabled. Since Graphics initialization has already
    /// occurred, simply check for PEG presence.
    /// bugbug: need to make sure this dependency is captured in the integration guide
    ///
    if (McDevFunPciCfg16 (Bus, Dev, Func, PCI_VID) != 0xFFFF) {
      ///
      /// PEG port enable and visible
      ///
      ///
      /// Initialize Slot Implemented for PCI Express Port
      ///
      Data16And = 0xFFFF;
      Data16Or  = BIT8;

      McDevFunPciCfg16Or (Bus, Dev, Func, R_SA_PEG_CAP_OFFSET, Data16Or);
      SCRIPT_MEM_READ_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint16,
        (UINTN) (PegBaseAddress + R_SA_PEG_CAP_OFFSET),
        &Data16Or,
        &Data16And
        );

      ///
      /// Initialize "Physical Slot Number" for PCI Express Port
      ///
      Data32 = McDevFunPciCfg32 (Bus, Dev, Func, R_SA_PEG_SLOTCAP_OFFSET);
      Data32 &= 0x0007FFFF;
      ///
      /// Set [31:19] - Slot # based on Peg Port
      ///
      Data32 |= (Slot << 19);

      ///
      /// Initialize Slot Power Limit for PCI Express Port and Power Limit Scale.
      /// Note: this register is a write once.
      ///
      /// Set [14:7] - 75 Watts (Default)
      ///
      Data32 &= 0xFFFE007F;
      Data32 |= (75 << 7);
      ///
      /// [16:15] - 1.0 Watt Scale
      ///
      Data32 |= (0 << 15);

      McDevFunPciCfg32 (Bus, Dev, Func, R_SA_PEG_SLOTCAP_OFFSET) = Data32;
      SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (PegBaseAddress + R_SA_PEG_SLOTCAP_OFFSET),
        1,
        &Data32
        );

      ///
      /// Additional Programming Steps
      ///
      /// Set PEG D.F.R 006h [15:0] = 0FFFFh
      ///
      Data32And = ~(0xFFFF0000);
      Data32Or  = 0xFFFF0000;

      McDevFunPciCfg32AndThenOr (Bus, Dev, Func, 0x4, Data32And, Data32Or);
      SCRIPT_MEM_READ_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (PegBaseAddress + 0x4),
        &Data32Or,
        &Data32And
        );

      ///
      /// Set PEG D.F.R 01Eh [15:0] = 0FFFFh
      ///
      Data32And = ~(0xFFFF0000);
      Data32Or  = 0xFFFF0000;

      McDevFunPciCfg32AndThenOr (Bus, Dev, Func, 0x1C, Data32And, Data32Or);
      SCRIPT_MEM_READ_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (PegBaseAddress + 0x1C),
        &Data32Or,
        &Data32And
        );

      ///
      /// Set PEG D.F.R 0AAh [15:0] = 0FFFFh
      ///
      Data32And = 0xFFFF;
      Data32Or  = 0xFFFF;

      McDevFunPciCfg16Or (Bus, Dev, Func, 0xAA, Data32Or);
      SCRIPT_MEM_READ_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint16,
        (UINTN) (PegBaseAddress + 0xAA),
        &Data32Or,
        &Data32And
        );

      ///
      /// Set PEG D.F.R 1C4h [31:0] = 0FFFFFFFFh
      ///
      Data32 = 0xFFFFFFFF;

      McDevFunPciCfg32Or (Bus, Dev, Func, R_SA_PEG_PEGUESTS_OFFSET, Data32);
      SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (PegBaseAddress + R_SA_PEG_PEGUESTS_OFFSET),
        1,
        &Data32
        );

      ///
      /// Set PEG D.F.R 1D0h [31:0] = 0FFFFFFFFh
      ///
      Data32 = 0xFFFFFFFF;

      McDevFunPciCfg32Or (Bus, Dev, Func, R_SA_PEG_PEGCESTS_OFFSET, Data32);
      SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (PegBaseAddress + R_SA_PEG_PEGCESTS_OFFSET),
        1,
        &Data32
        );

      ///
      /// Set PEG D.F.R 1F0h [31:0] = 0FFFFFFFFh
      ///
      Data32 = 0xFFFFFFFF;

      McDevFunPciCfg32Or (Bus, Dev, Func, 0x1F0, Data32);
      SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (PegBaseAddress + 0x1F0),
        1,
        &Data32
        );
      ///
      /// If HSW CPU steppingId >= B0 or CRW, set BIT19 of DCAP2 register of the PEG port,to enable OBFF support using WAKE# signaling
      ///
      if (((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId >= EnumHswB0)) ||
           (CpuFamilyId == EnumCpuCrw)) {
        Data32 = (UINT32) BIT19;
        McDevFunPciCfg32Or (Bus, Dev, Func, R_SA_PEG_DCAP2_OFFSET, Data32);
        SCRIPT_MEM_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint32,
          (UINTN) (PegBaseAddress + R_SA_PEG_DCAP2_OFFSET),
          1,
          &Data32
        );
      }
      ///
      /// Complete Common Port and Endpoint Configuration
      ///
      ///
      /// Virtual Channel Configuration of PCI Express Port
      /// Set the VC0RCTL register D1:F0 Offset 114h [7:1] = 7Fh
      ///
      Data32And = 0xFFFFFF01;
      Data32Or  = BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1;

      McDevFunPciCfg32AndThenOr (Bus, Dev, Func, R_SA_PEG_VC0RCTL0_OFFSET, Data32And, Data32Or);
      SCRIPT_MEM_READ_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        (UINTN) (PegBaseAddress + R_SA_PEG_VC0RCTL0_OFFSET),
        &Data32Or,
        &Data32And
        );
      ///
      /// 6.8 Additional Programming Steps before Enabling ASPM for PEG device
      ///
      AdditionalPEGProgramStepsBeforeASPM (Bus, Dev, Func, DxePlatformSaPolicy);

      ///
      /// 6.10 Interrupt Routing for PCI Express*
      /// It is recommened to re-route the legacy interrupts (INTA -> INTB,C,D)
      /// to avoid overcrowded INTA. ACPI PRT needs update.
      /// The ACPI _PRT() methods for PEG controllers must match the legacy interrupt routing.
      ///
      if ((Dev == 1) && (Func == 1)) {
        Data32And = (UINT32)~(BIT25 | BIT24);
        Data32Or  = BIT24 | BIT25;
        McDevFunPciCfg32AndThenOr (Bus, Dev, Func, R_SA_PEG_CFG4_OFFSET, Data32And, Data32Or);
        SCRIPT_MEM_READ_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint32,
          (UINTN) (PegBaseAddress + R_SA_PEG_CFG4_OFFSET),
          &Data32Or,
          &Data32And
          );
      }

      if ((Dev == 1) && (Func == 2)) {
        Data32And = (UINT32)~(BIT25 | BIT24);
        Data32Or  = BIT25;
        McDevFunPciCfg32AndThenOr (Bus, Dev, Func, R_SA_PEG_CFG4_OFFSET, Data32And, Data32Or);
        SCRIPT_MEM_READ_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint32,
          (UINTN) (PegBaseAddress + R_SA_PEG_CFG4_OFFSET),
          &Data32Or,
          &Data32And
          );
      }
    }

    ///
    /// Lock offset 3Dh for Interrupt Pin
    ///
    Data8 = McDevFunPciCfg8 (Bus, Dev, Func, PCI_INTPIN);
    McDevFunPciCfg8 (Bus, Dev, Func, PCI_INTPIN) = Data8;
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint8,
      (UINTN) (PegBaseAddress + PCI_INTPIN),
      1,
      &Data8
      );

    ///
    /// Lock DCAP register
    ///
    Data32 = McDevFunPciCfg32 (Bus, Dev, Func, R_SA_PEG_DCAP_OFFSET);
    McDevFunPciCfg32 (Bus, Dev, Func, R_SA_PEG_DCAP_OFFSET) = Data32;
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (PegBaseAddress + R_SA_PEG_DCAP_OFFSET),
      1,
      &Data32
      );

    if ((CpuFamilyId == EnumCpuHsw) || (CpuFamilyId == EnumCpuCrw)
        ){
      if ((Bus == 0) && (Dev == 1) && (Func == 0)) {
        Data32  = McDevFunPciCfg32 (Bus, Dev, Func, 0xCD0);
        Data32 |= BIT11;
        McDevFunPciCfg32 (Bus, Dev, Func, 0xCD0) = Data32;
        SCRIPT_MEM_WRITE (
          EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
          EfiBootScriptWidthUint32,
          (UINTN) (PegBaseAddress + 0xCD0),
          1,
          &Data32
          );
       }
    }

    if ((HwStrap == SA_PEG_x16_x0_x0) && (PegComplete == 0)) {
      break;
    }
    if ((HwStrap == SA_PEG_x8_x8_x0) && (PegComplete == 1)) {
      break;
    }
    if ((HwStrap == SA_PEG_x8_x4_x4) && (PegComplete == 2)) {
      break;
    }
  }

  return EFI_SUCCESS;
}
#endif // PEG_FLAG

EFI_STATUS
Cid1Cid2DmiPortInit (
  IN  UINT64                         DmiBar,
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
  )
/**

    DMI Port Initialization for both CID1 (Port 1 in MCH) and CID2 (Port 0 in PCH).

    @param[in] DmiBar              - DMIBAR Address
    @param[in] DxePlatformSaPolicy - SA DxePlatformPolicy protocol

    @retval EFI_SUCCESS            - DMI Port initialization successed.

**/
{
  UINT32     Data32Or;
#ifdef DMI_FLAG
  UINT8      Data8And;
  UINT8      Data8Or;
  CPU_FAMILY CpuFamilyId;

  CpuFamilyId = GetCpuFamily();
#endif // DMI_FLAG

#ifdef DMI_FLAG
  if ((CpuFamilyId == EnumCpuHsw) || (CpuFamilyId == EnumCpuCrw)) {
    ///
    /// Additional Programming Steps
    ///
    AdditionalDMIProgramStepsBeforeASPM (DmiBar, DxePlatformSaPolicy);
  }
#endif // DMI_FLAG

  ///
  /// Set DMIBAR Offset 1C4h [31:0] = 0FFFFFFFFh
  ///
  Data32Or                = 0xFFFFFFFF;
  Mmio32 (DmiBar, 0x1C4)  = 0xFFFFFFFF;
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (DmiBar + 0x1C4),
    1,
    &Data32Or
    );

  ///
  /// Set DMIBAR Offset 1D0h [31:0] = 0FFFFFFFFh
  ///
  Data32Or                = 0xFFFFFFFF;
  Mmio32 (DmiBar, 0x1D0)  = 0xFFFFFFFF;
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (DmiBar + 0x1D0),
    1,
    &Data32Or
    );

#ifdef DMI_FLAG
  if ((CpuFamilyId == EnumCpuHsw) || (CpuFamilyId == EnumCpuCrw)) {
    ///
    /// Enable `Active State PM'. DMILCTL register at DMIBAR 088h [1:0] = '11b'.
    /// Based on the policy:
    ///
    if (DxePlatformSaPolicy->PcieConfig->DmiAspm == PcieAspmAutoConfig ||
        DxePlatformSaPolicy->PcieConfig->DmiAspm == PcieAspmL0sL1
        ) {
      ///
      /// Enable ASPM = L0s and L1 Entry
      ///
      Data8And  = 0xFC;
      Data8Or   = 0x03;
      Mmio8Or (DmiBar, 0x88, Data8Or);
      SCRIPT_MEM_READ_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint8,
        (UINTN) (DmiBar + 0x088),
        &Data8Or,
        &Data8And
        );
    } else if (DxePlatformSaPolicy->PcieConfig->DmiAspm == PcieAspmL0s) {
      ///
      /// Enable ASPM = L0s
      ///
      Data8And  = 0xFC;
      Data8Or   = 0x01;
      Mmio8Or (DmiBar, 0x88, Data8Or);
      SCRIPT_MEM_READ_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint8,
        (UINTN) (DmiBar + 0x088),
        &Data8Or,
        &Data8And
        );
    } else if (DxePlatformSaPolicy->PcieConfig->DmiAspm == PcieAspmL1) {
      ///
      /// Enable ASPM = L1
      ///
      Data8And  = 0xFC;
      Data8Or   = 0x02;
      Mmio8Or (DmiBar, 0x88, Data8Or);
      SCRIPT_MEM_READ_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint8,
        (UINTN) (DmiBar + 0x088),
        &Data8Or,
        &Data8And
        );
    }

    if (DxePlatformSaPolicy->PcieConfig->DmiExtSync == ENABLED) {
      ///
      /// Enable Extended Synch
      ///
      Data8And  = 0xFF;
      Data8Or   = 0x10;
      Mmio8Or (DmiBar, 0x88, Data8Or);
      SCRIPT_MEM_READ_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint8,
        (UINTN) (DmiBar + 0x088),
        &Data8Or,
        &Data8And
        );
    }

    if (DxePlatformSaPolicy->PcieConfig->DmiIot == ENABLED) {
      ///
      /// if DMI Iot is enabled, set DMIBAR offset 0xD34 = 0x44
      ///
      Data8Or               = 0x44;
      Mmio8 (DmiBar, 0xD34) = 0x44;
      SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint8,
        (UINTN) (DmiBar + 0xD34),
        1,
        (VOID *) (UINTN) (DmiBar + 0xD34)
        );
    }
  }
#endif // DMI_FLAG

  return EFI_SUCCESS;
}

EFI_STATUS
Cid1TopologyInit (
  IN UINT64        EgressPortBar,
  IN  UINT64       DmiBar,
  IN  UINT32       PchRootComplexBar
  )
/**

    Perform Root Complex Topology Initialization for CID1.

    @param[in] EgressPortBar     - EPBAR Address
    @param[in] DmiBar            - DMIBAR Address
    @param[in] PchRootComplexBar - PCH RCBA Address

    @retval EFI_SUCCESS          - Root Complex topology initialization for CID1 successed.

**/
{
  UINT32  Data32;
  UINT32  Data32And;
  UINT32  Data32Or;
  UINT32  DwordReg;
  UINT64  McD1Base;
  UINT64  McD1F1Base;
  UINT64  McD1F2Base;

  McD1Base    = MmPciAddress (0, 0, 1, 0, 0);
  McD1F1Base  = MmPciAddress (0, 0, 1, 1, 0);
  McD1F2Base  = MmPciAddress (0, 0, 1, 2, 0);

  ///
  /// Set the CID1 Egress Port 0 Topology
  ///
  ///
  /// Step 1, Set the SA Component ID = 1.
  ///
  Data32And = 0xFF00FFFF;
  Data32Or  = BIT16;
  Mmio32AndThenOr (EgressPortBar, 0x44, Data32And, Data32Or);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (EgressPortBar + 0x44),
    &Data32Or,
    &Data32And
    );

  ///
  /// Step 2, Set link 1 Target Component ID and valid (Bit 0 = 1b).
  /// Set the Link 1 TCID = 1 (Bits 23:16 = 01h).
  ///
  Data32And = 0xFF00FFFF;
  Data32Or  = (BIT16 | BIT0);
  Mmio32AndThenOr (EgressPortBar, 0x50, Data32And, Data32Or);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (EgressPortBar + 0x50),
    &Data32Or,
    &Data32And
    );

  ///
  /// Step 3, Set Link 1 to Reference the DMI RCRB (Bits 31:0 = DMI Base).
  ///
  Data32 = (UINT32) DmiBar;
  MmioWrite32 ((UINTN) EgressPortBar + 0x58, Data32);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (EgressPortBar + 0x58),
    1,
    &Data32
    );
  Data32 = (UINT32) 0x00;
  MmioWrite32 ((UINTN) EgressPortBar + 0x58 + 4, Data32);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (EgressPortBar + 0x58 + 4),
    1,
    &Data32
    );

#ifdef PEG_FLAG
  ///
  /// CID1 Egress port Root Topology and PciExpress Port (PEG Ports) Topology
  /// Programming only if PEG devices are enabled.
  ///
  /// Step 1 PCI Express Enabled Check
  /// Check and Configure CID1 root and Device 1 function 0
  ///
  if (McD1PciCfg16 (PCI_VID) != 0xFFFF) {
    ///
    /// Step 4, Set Link 2 to Reference the Device 1 function 0.
    ///
    Data32 = (UINT32) 0x8000;
    MmioWrite32 ((UINTN) EgressPortBar + 0x68, Data32);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (EgressPortBar + 0x68),
      1,
      &Data32
      );
    Data32 = (UINT32) 0x00;
    MmioWrite32 ((UINTN) EgressPortBar + 0x68 + 4, Data32);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (EgressPortBar + 0x68 + 4),
      1,
      &Data32
      );

    ///
    /// Step 5, Set link 2 Target Component ID and valid bit(Offset 60h, Bit 0 = 1b)
    /// Set the Link 2 TCID = 1 (Bits 23:16 = 01h).
    ///
    Data32And = 0xFF00FFFF;
    Data32Or  = (BIT16 | BIT0);
    Mmio32AndThenOr (EgressPortBar, 0x60, Data32And, Data32Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (EgressPortBar + 0x60),
      &Data32Or,
      &Data32And
      );

    ///
    /// Set the CID1 PCI Express* Port Root Topology
    ///
    /// Step 2 Set the Link 1 CID = 1 144h(23:16 = 01h).
    ///
    Data32And = 0xFF00FFFF;
    Data32Or  = BIT16;
    MmioAndThenOr32 ((UINTN) McD1Base + 0x144, Data32And, Data32Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (McD1Base + 0x144),
      &Data32Or,
      &Data32And
      );

    ///
    /// Step 3. Set Link 1 to Reference the SA EP RCRB (Bits 31:0 = EPBAR).
    ///
    Data32 = (UINT32) EgressPortBar;
    MmioWrite32 ((UINTN) McD1Base + 0x158, Data32);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (McD1Base + 0x158),
      1,
      &Data32
      );
    Data32 = (UINT32) 0x00;
    MmioWrite32 ((UINTN) McD1Base + 0x158 + 4, Data32);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (McD1Base + 0x158 + 4),
      1,
      &Data32
      );

    ///
    /// Step 4 Set link 1 Target Component ID and valid bit(Offset 150h, Bit 0 = 1b)
    /// Set the Link 1 TCID = 1 (Bits 23:16 = 01h).
    ///
    Data32And = 0xFF00FFFF;
    Data32Or  = (BIT16 | BIT0);
    MmioAndThenOr32 ((UINTN) McD1Base + 0x150, Data32And, Data32Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (McD1Base + 0x150),
      &Data32Or,
      &Data32And
      );

    ///
    /// Step 5 Program Read-Only Write-Once Registers
    /// D1.F0.R 0C4h [31:0]
    ///
    Data32And             = 0xFFFFFFFF;
    Data32Or              = (UINT32) 0x00;
    Data32                = McD1PciCfg32 (0xC4);
    McD1PciCfg32 (0xC4)   = Data32;
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (McD1Base + 0xC4),
      &Data32Or,
      &Data32And
      );
  }
  ///
  /// Check and Configure CID1 root and Device 1 function 1
  ///
  if (McD1F1PciCfg16 (PCI_VID) != 0xFFFF) {
    ///
    /// Step 6, Set Link 3 to Reference the Device 1 function 1.
    ///
    Data32 = (UINT32) 0x9000;
    MmioWrite32 ((UINTN) EgressPortBar + 0x78, Data32);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (EgressPortBar + 0x78),
      1,
      &Data32
      );
    Data32 = (UINT32) 0x00;
    MmioWrite32 ((UINTN) EgressPortBar + 0x78 + 4, Data32);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (EgressPortBar + 0x78 + 4),
      1,
      &Data32
      );

    ///
    /// Step 7. Set the Link 3 Target Component ID and Valid Bit 70h [0]
    ///
    Data32And = 0xFF00FFFF;
    Data32Or  = (BIT16 | BIT0);
    Mmio32AndThenOr (EgressPortBar, 0x70, Data32And, Data32Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (EgressPortBar + 0x70),
      &Data32Or,
      &Data32And
      );

    ///
    /// Step 2 Set the Link 1 CID = 1 144h(23:16 = 01h).
    ///
    Data32And = 0xFF00FFFF;
    Data32Or  = BIT16;
    MmioAndThenOr32 ((UINTN) McD1F1Base + 0x144, Data32And, Data32Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (McD1F1Base + 0x144),
      &Data32Or,
      &Data32And
      );

    ///
    /// Step 3 Set Link 1 to Reference the IMC EP RCRB (Bits 31:0 = EPBAR).
    ///
    Data32 = (UINT32) EgressPortBar;
    MmioWrite32 ((UINTN) McD1F1Base + 0x158, Data32);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (McD1F1Base + 0x158),
      1,
      &Data32
      );
    Data32 = (UINT32) 0x00;
    MmioWrite32 ((UINTN) McD1F1Base + 0x158 + 4, Data32);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (McD1F1Base + 0x158 + 4),
      1,
      &Data32
      );

    ///
    /// Step 4 Set link 1 Target Component ID and valid bit(Offset 150h, Bit 0 = 1b)
    /// Set the Link 1 TCID = 1 (Bits 23:16 = 01h).
    ///
    Data32And = 0xFF00FFFF;
    Data32Or  = (BIT16 | BIT0);
    MmioAndThenOr32 ((UINTN) McD1F1Base + 0x150, Data32And, Data32Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (McD1F1Base + 0x150),
      &Data32Or,
      &Data32And
      );

    ///
    /// Step 5 Program Read-Only Write-Once Registers
    ///
    Data32And               = 0xFFFFFFFF;
    Data32Or                = (UINT32) 0x00;
    Data32                  = McD1F1PciCfg32 (0xC4);
    McD1F1PciCfg32 (0xC4)   = Data32;
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (McD1F1Base + 0xC4),
      &Data32Or,
      &Data32And
      );
  }
  ///
  /// Check and Configure CID1 root and Device 1 function 2
  ///
  if (McD1F2PciCfg16 (PCI_VID) != 0xFFFF) {
    ///
    /// Step 8, Set Link 4 to Reference the Device 1 function 2.
    ///
    Data32 = (UINT32) 0xA000;
    MmioWrite32 ((UINTN) EgressPortBar + 0x88, Data32);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (EgressPortBar + 0x88),
      1,
      &Data32
      );
    Data32 = (UINT32) 0x00;
    MmioWrite32 ((UINTN) EgressPortBar + 0x88 + 4, Data32);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (EgressPortBar + 0x88 + 4),
      1,
      &Data32
      );

    ///
    /// Step 9, Set the Link 4 Target Component ID and Valid Bit 80h [0]
    ///
    Data32And = 0xFF00FFFF;
    Data32Or  = (BIT16 | BIT0);
    Mmio32AndThenOr (EgressPortBar, 0x80, Data32And, Data32Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (EgressPortBar + 0x80),
      &Data32Or,
      &Data32And
      );

    ///
    /// Step 2. Set the Link 1 CID = 1 144h(23:16 = 01h).
    ///
    Data32And = 0xFF00FFFF;
    Data32Or  = BIT16;
    MmioAndThenOr32 ((UINTN) McD1F2Base + 0x144, Data32And, Data32Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (McD1F2Base + 0x144),
      &Data32Or,
      &Data32And
      );

    ///
    /// Step 3. Set Link 1 to Reference the IMC EP RCRB (Bits 31:0 = EPBAR).
    ///
    Data32 = (UINT32) EgressPortBar;
    MmioWrite32 ((UINTN) McD1F2Base + 0x158, Data32);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (McD1F2Base + 0x158),
      1,
      &Data32
      );
    Data32 = (UINT32) 0x00;
    MmioWrite32 ((UINTN) McD1F2Base + 0x158 + 4, Data32);
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (McD1F2Base + 0x158 + 4),
      1,
      &Data32
      );

    ///
    /// Step 4. Set link 1 Target Component ID and valid bit(Offset 150h, Bit 0 = 1b)
    /// Set the Link 1 TCID = 1 (Bits 23:16 = 01h).
    ///
    Data32And = 0xFF00FFFF;
    Data32Or  = (BIT16 | BIT0);
    MmioAndThenOr32 ((UINTN) McD1F2Base + 0x150, Data32And, Data32Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (McD1F2Base + 0x150),
      &Data32Or,
      &Data32And
      );

    ///
    /// Step 5. Program Read-Only Write-Once Registers
    ///
    Data32And               = 0xFFFFFFFF;
    Data32Or                = (UINT32) 0x00;
    Data32                  = McD1F2PciCfg32 (0xC4);
    McD1F2PciCfg32 (0xC4)   = Data32;
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (McD1F2Base + 0xC4),
      &Data32Or,
      &Data32And
      );
  }
#endif // PEG_FLAG

  ///
  /// Set the CID1 DMI Port Root Topology
  ///
  /// Step 1 Set the CID = 1 ( Bits 23:16 = 01h).
  ///
  Data32And = 0xFF00FFFF;
  Data32Or  = BIT16;
  Mmio32AndThenOr (DmiBar, 0x44, Data32And, Data32Or);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (DmiBar + 0x44),
    &Data32Or,
    &Data32And
    );

  ///
  /// Step 2.  Set Link 1 Target Port Number = 0 (Bits 31:24 = 00h).
  /// Step 3.  Set Link 1 TCID = 2 (Bits 23:16 = 02h).
  /// Step 4.  Set Link 1 as valid (Bit 0 = 1b).
  ///
  Data32And = 0x0000FFFF;
  Data32Or  = (BIT17 + BIT0);
  Mmio32AndThenOr (DmiBar, 0x50, Data32And, Data32Or);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (DmiBar + 0x50),
    &Data32Or,
    &Data32And
    );

  ///
  /// Step 5, Set Link 1 to Reference the PCH RCRB
  ///
  Data32                  = PchRootComplexBar;
  Mmio32 (DmiBar, 0x58)   = PchRootComplexBar;
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (DmiBar + 0x58),
    1,
    &Data32
    );
  Data32                      = (UINT32) 0x00;
  Mmio32 (DmiBar, 0x58 + 4)   = Data32;
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (DmiBar + 0x58 + 4),
    1,
    &Data32
    );

  ///
  /// Step 6, Set Link 2 to Reference the IMC EP (Bits 31:0 = EP).
  ///
  Data32 = (UINT32) EgressPortBar;
  MmioWrite32 ((UINTN) DmiBar + 0x68, Data32);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (DmiBar + 0x68),
    1,
    &Data32
    );
  Data32 = (UINT32) 0x00;
  MmioWrite32 ((UINTN) DmiBar + 0x68 + 4, Data32);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (DmiBar + 0x68 + 4),
    1,
    &Data32
    );

  ///
  /// Step 7, Set link 2 Target Component ID and valid Bit(Bit 0 = 1b)
  /// Set the Link 2 TCID = 1 (Bits 23:16 = 01h).
  ///
  Data32And = 0xFF00FFFF;
  Data32Or  = (BIT16 | BIT0);
  Mmio32AndThenOr (DmiBar, 0x60, Data32And, Data32Or);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (DmiBar + 0x60),
    &Data32Or,
    &Data32And
    );

//[-start-120815-IB06150236-modify]//
  ///
  /// Step 8. Program RO and Write-Once Registers
  /// DMIBAR Offset 004h [31:0]
  /// DMIBAR Offset 084h [31:0]
  /// DMIBAR Offset 334h [31:0]
  /// DMIBAR Offset 338h [31:0]
  /// DMIBAR Offset 344h [31:0]
  /// DMIBAR Offset 348h [31:0]
  ///
  DwordReg                = Mmio32 (DmiBar, 0x4);
  Mmio32 (DmiBar, 0x4)    = DwordReg;

  DwordReg                = Mmio32 (DmiBar, 0x84);
  Mmio32 (DmiBar, 0x84)   = DwordReg;

//[-start-121224-IB06150266-remove]//
//  DwordReg                = Mmio32 (DmiBar, 0x334);
//  Mmio32 (DmiBar, 0x334)   = DwordReg;
//
//  DwordReg                = Mmio32 (DmiBar, 0x338);
//  Mmio32 (DmiBar, 0x338)   = DwordReg;
//
//  DwordReg                = Mmio32 (DmiBar, 0x344);
//  Mmio32 (DmiBar, 0x344)   = DwordReg;
//
//  DwordReg                = Mmio32 (DmiBar, 0x348);
//  Mmio32 (DmiBar, 0x348)   = DwordReg;
//[-end-121224-IB06150266-remove]//
//[-end-120815-IB06150236-modify]//

  return EFI_SUCCESS;
}

EFI_STATUS
Cid2TopologyInit (
  IN UINT32   PchRootComplexBar,
  IN UINT64   DmiBar
  )
/**

  Perform Root Complex Topology Initialization for CID2.
  Note: This sequence follows PCH BIOS specification Ver 0.5 section 8.3
        Root Complex Topology Programming

  @param[in] PchRootComplexBar - PCH RCBA Address
  @param[in] DmiBar            - DMIBAR Address

  @retval EFI_SUCCESS          - Root Complex topology initialization for CID2 successed.

**/
{
  UINT32  Data32;
  UINT32  Data32And;
  UINT32  Data32Or;

  ///
  /// PCH BIOS specification Ver 0.5 section 8.3 Note 1,2
  /// program a value into this Component ID field to determine the Component ID of
  /// ICH8, and this value must be different from the Component ID value of the MCH.
  /// Set the CID = 2 (Offset 104h, Bits 23:16 = 02h).
  ///
  Data32And = 0xFF00FFFF;
  Data32Or  = BIT17;
  MmioAndThenOr32 (PchRootComplexBar + 0x104, Data32And, Data32Or);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (PchRootComplexBar + 0x104),
    &Data32Or,
    &Data32And
    );

  ///
  /// PCH BIOS specification Ver 0.5 section 8.3 Note 3,4
  /// Note 3: This Target Port # field must be filled in by System BIOS with the Port
  /// # of the RCRB DMI link in the MCH.
  /// Note 4: This Target CID field must be filled in by System BIOS with the
  /// Component ID of the MCH.
  /// Set the Link 1 Target Port Number = 1 (Offset 110h, bits 31:24 = 01h).
  /// Set the Link 1 Target Component ID = 1 (Offset 110h, bits 23:16 = 01h).
  ///
  Data32And = 0x0000FFFF;
  Data32Or  = (BIT24 | BIT16);
  MmioAndThenOr32 (PchRootComplexBar + 0x110, Data32And, Data32Or);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (PchRootComplexBar + 0x110),
    &Data32Or,
    &Data32And
    );

  ///
  /// PCH BIOS specification Ver 0.5 section 8.3 Note 5
  /// Fill the Base Address field with the same base address of the RCRB DMI link in
  /// the MCH, This register field is located at RCBA+ 0118h[63:0],
  /// and will be locked once written until the next reset.
  ///
  Data32 = (UINT32) DmiBar;
  MmioWrite32 (PchRootComplexBar + 0x118, Data32);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (PchRootComplexBar + 0x118),
    1,
    &Data32
    );

  Data32 = *((UINT32 *) (&DmiBar) + 1);
  MmioWrite32 (PchRootComplexBar + 0x118 + 4, Data32);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint8,
    (UINTN) (PchRootComplexBar + 0x118 + 4),
    1,
    &Data32
    );

  return EFI_SUCCESS;
}

EFI_STATUS
PegInitBeforeExitPmAuth (
  VOID
  )
/**

  This function performs Peg initialization before ExitPmAuth.

  @retval EFI_SUCCESS   - Always.

**/
{
#if SA_PCIE_ASPM_IN_SMM == 1
  EFI_EVENT                       ReadyToBoot;
  EFI_STATUS                      Status;
#endif
  BOOLEAN                         AspmHasBeenHandled;

  DEBUG ((EFI_D_ERROR, "[SA] Pcie before ExitPmAuth callback.\n"));
  AspmHasBeenHandled = FALSE;
  ///
  /// SMM mode ASPM handling
  /// Check if supported and enabled
  ///
#if SA_PCIE_ASPM_IN_SMM == 1
  if ((mInitPcieAspmAfterOprom == 1) && (mSaIotrapSmiAddress != 0)) {
    ///
    /// Do the Phase 1 SMI callback
    /// This will enumerate PCIe downstream devices
    ///
    SaLateInitSmiCallback (NULL, NULL);

    ///
    /// Create an ReadyToBoot call back event to do the Phase 3 SMI callback
    /// This will handle PEG ASPM programming after OROM execution
    ///
    Status = EfiCreateEventReadyToBootEx (
               TPL_NOTIFY,
               SaLateInitSmiCallback,
               NULL,
               &ReadyToBoot
               );
    ASSERT_EFI_ERROR (Status);
    AspmHasBeenHandled = TRUE;
  }
#endif

  ///
  /// DXE mode ASPM handling
  /// Check if SMM mode already taken care all things
  /// TRUE to skip DXE mode task. Otherwise do DXE mode ASPM initialization
  ///
#if SA_PCIE_ASPM_IN_DXE == 1
  if (AspmHasBeenHandled == FALSE) {
    ///
    /// Initialize ASPM before OpROM, S3 Save script supported
    /// First initialize all policy settings
    /// Initialize module global variables - Stepping ID and Platform Policy
    ///
    SaPcieInitPolicy (mDxePlatformSaPolicy);
    ///
    /// Do initialization
    ///
    SaPcieEnumCallback ();
    SaPcieConfigAfterOpRom ();
  }
#endif

  return EFI_SUCCESS;
}

EFI_STATUS
SaSecurityInit (
  VOID
  )
/**

  This function performs SA Security locking in ExitPmAuth callback

  @retval EFI_SUCCESS     - Security lock has done
  @retval EFI_UNSUPPORTED - Security lock not done successfully

**/
{
  BOOLEAN                         SecurityHasBeenHandled;
  SecurityHasBeenHandled = FALSE;
#if SA_PCIE_ASPM_IN_SMM == 1
  if ((mInitPcieAspmAfterOprom == 1) && (mSaIotrapSmiAddress != 0)) {
    ///
    /// Generate the Phase 2 of SA SMI to do security lock
    ///
    SaLateInitSmiCallback (NULL, NULL);

    SecurityHasBeenHandled = TRUE;
  }
#endif

  ///
  /// Check if SMM mode already taken care this task
  ///
#if SA_PCIE_ASPM_IN_DXE == 1
  if (SecurityHasBeenHandled == FALSE) {
    SaSaveRestorePlatform (TRUE);
    SaSecurityLock ();
    SecurityHasBeenHandled = TRUE;
  }
#endif
  ///
  /// Security locking is important so fail to do this will be an ERROR.
  ///
  if (SecurityHasBeenHandled == TRUE) {
    return EFI_SUCCESS;
  } else {
    return EFI_UNSUPPORTED;
  }
}
