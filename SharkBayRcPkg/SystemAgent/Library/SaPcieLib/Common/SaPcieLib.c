/** @file
  SA PCIE Library

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
  Copyright (c) 2012 - 2013 Intel Corporation. All rights reserved
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

//#if !defined(EDK_RELEASE_VERSION) || (EDK_RELEASE_VERSION < 0x00020000)
//#include "EdkIIGlueBase.h"
#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <SaRegs.h>
#include <SaAccess.h>
#include <PchAccess.h>
#include <Protocol/SaPlatformPolicy.h>
#include <CpuRegs.h>
#include <Library/CpuPlatformLib.h>
#include <Library/S3BootScriptLib.h>
#include <Library/SaPcieLib.h>
#include "SaPcieLib.h"
//#endif

#ifdef MmPciAddress
#undef MmPciAddress
#endif

#define MmPciAddress(Segment, Bus, Device, Function, Register) \
    ( \
      EDKII_GLUE_PciExpressBaseAddress + (UINTN) (Bus << 20) + (UINTN) (Device << 15) + (UINTN) \
        (Function << 12) + (UINTN) (Register) \
    )

#ifdef PcdGet64
#undef PcdGet64
#endif

#define PcdGet64(TokenName)    EDKII_GLUE_##TokenName


#define LTR_VALUE_MASK (BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7 + BIT8 + BIT9)
#define LTR_SCALE_MASK (BIT10 + BIT11 + BIT12)

///
/// LTR related macros
///
#define LTR_LATENCY_VALUE(x)           (x & LTR_VALUE_MASK)
#define LTR_MULTIPLIER_INDEX(x)        ((UINT32)(x & LTR_SCALE_MASK) >> 10)


typedef struct {
  UINT64  BaseAddr;
  UINT32  Offset;
  UINT32  AndMask;
  UINT32  OrMask;
} BOOT_SCRIPT_REGISTER_SETTING;

///
/// SA PCI Registers to save for S3 resume
///
UINTN mSaChipsetPciRegistersTable[] = {
  R_SA_DEVEN,
  R_SA_REMAPBASE + 4,
  R_SA_REMAPBASE,
  R_SA_REMAPLIMIT + 4,
  R_SA_REMAPLIMIT
};
//
// Address values for mSaChipsetPciRegistersSaveTable will be initialized at Runtime inside function
// SaPcieInitPolicy(). The Address uses the Register Offset from table mSaChipsetPciRegistersTable
//
BOOT_SCRIPT_PCI_REGISTER_SAVE mSaChipsetPciRegistersSaveTable[] = {
  0,  S3BootScriptWidthUint32,  0,  // R_SA_DEVEN
  0,  S3BootScriptWidthUint32,  0,  // R_SA_REMAPBASE + 4
  0,  S3BootScriptWidthUint32,  0,  // R_SA_REMAPBASE
  0,  S3BootScriptWidthUint32,  0,  // R_SA_REMAPLIMIT + 4
  0,  S3BootScriptWidthUint32,  0,  // R_SA_REMAPLIMIT
};

///
/// SA IGFX PCI Registers to save for S3 resume
///
UINTN mSaIgfxPciRegistersTable[] = {
  R_SA_IGD_SWSCI_OFFSET,
  R_SA_IGD_ASLS_OFFSET,
  R_SA_IGD_GTTMMADR + 4,
  R_SA_IGD_GTTMMADR,
  R_SA_IGD_GMADR + 4,
  R_SA_IGD_GMADR,
  R_SA_IGD_IOBAR,
  R_SA_IGD_CMD
};
//
// Address values for mSaIgfxPciRegistersSaveTable will be initialized at Runtime inside function
// SaPcieInitPolicy(). The Address uses the Register Offset from table mSaIgfxPciRegistersTable
//
BOOT_SCRIPT_PCI_REGISTER_SAVE mSaIgfxPciRegistersSaveTable[] = {
  0,  S3BootScriptWidthUint32,  0,  // R_SA_IGD_SWSCI_OFFSET
  0,  S3BootScriptWidthUint32,  0,  // R_SA_IGD_ASLS_OFFSET
  0,  S3BootScriptWidthUint32,  0,  // R_SA_IGD_GTTMMADR + 4
  0,  S3BootScriptWidthUint32,  0,  // R_SA_IGD_GTTMMADR
  0,  S3BootScriptWidthUint32,  0,  // R_SA_IGD_GMADR + 4
  0,  S3BootScriptWidthUint32,  0,  // R_SA_IGD_GMADR
  0,  S3BootScriptWidthUint32,  0,  // R_SA_IGD_IOBAR
  0,  S3BootScriptWidthUint32,  0,  // R_SA_IGD_CMD
};

/*
  4th Gen Intel(R) Core Processor (Haswell) System Agent BIOS Spec 0.7.2

  13.1.1 SMRAM Locking
   Lock down SMRAM Space by setting the B0.D0.F0 register offset 088h[4] = '1b'
  
  13.2 System Agent configuration Locking.
  For reliable operation and security, it is strongly recommended that BIOS should lock critical system agent configuration.
  When the lock bits are set to 1 then all related registers become read only to the CPU.

    For reliable operation and security, it is strongly recommended that BIOS should set the following bits:
    .Lock GGC from writes by setting the B0.D0.F0 register offset 050[0] = '1b'. (Duplicate of MRC)
    .Lock DPR by setting the B0.D0.F0 register offset 05Ch[0] = '1b'.
    .Lock ME memory range configuration by setting the B0.D0.F0 register offset 078h[10] = '1b'.  (Duplicate of MRC)
    .Lock remap base and limit by setting the B0.D0.F0 register offset 090h[0] = '1b' and B0.D0.F0 register offset 098h[0] = '1b' (Duplicate of MRC)
    .Lock TOM by setting the B0.D0.F0 register offset 0A0h[0] = '1b'.  (Duplicate of MRC)
    .Lock TOUUD by setting the B0.D0.F0 register offset 0A8h[0] = '1b'. (Duplicate of MRC)
    .Lock BDSM by setting the B0.D0.F0 register offset 0B0h[0] = '1b'. (Duplicate of MRC)
    .Lock BGSM by setting the B0.D0.F0 register offset 0B4h[0] = '1b'. (Duplicate of MRC)
    .Lock TSEG Memory Base by setting the B0.D0.F0 register offset 0B8h[0] = '1b'.
    .Lock TOLUD by setting the B0.D0.F0 register offset 0BCh[0] = '1b'.  (Duplicate of MRC)
    .Lock Memory controller configuration by setting the MCHBAR Offset 50FCh [7:0] = '8Fh'. (Duplicate of MRC)
    .Lock DDR power/thermal management settings by setting MCHBAR offset 5880h [5] = '1b'
    .Lock primary channel arbiter weights by setting the MCHBAR Offset 7000h [31] = '1b',
       MCHBAR Offset 77FCh [0] = '1b', MCHBAR Offset 7FFCh [0] = '1b' and MCHBAR Offset 6800h [31] = '1b'.
    .Lock UMA GFX by setting the MCHBAR Offset 6020h [0] = '1b'.
    .Lock PAVP settings by setting MCHBAR Offset 0x5500h [0] = '1b'.
    .Lock VTDTRK by setting the MCHBAR Offset 63FCh [0] = '1b'.
    .Read and write back MCHBAR Offset 6008h [31:0]
    .Read and write back MCHBAR Offset 6030h [31:0]
    .Read and write back MCHBAR Offset 6034h [31:0]
    .Lock processor/chipset BAR registers by setting MSR 0x2E7h [0] = '1b' at the end of POST.
*/
//
// BaseAddr values for mSaSecurityRegisters that uses PciExpressBaseAddress will be initialized at
// Runtime inside function SaPcieInitPolicy().
//
BOOT_SCRIPT_REGISTER_SETTING mSaSecurityRegisters[] = {
  0,  0x088,  0xFFFFFFFF,  BIT4,
  0,  0x050,  0xFFFFFFFF,  BIT0,
  0,  0x05C,  0xFFFFFFFF,  BIT0,
  0,  0x078,  0xFFFFFFFF,  BIT10,
  0,  0x090,  0xFFFFFFFF,  BIT0,
  0,  0x098,  0xFFFFFFFF,  BIT0,
  0,  0x0A0,  0xFFFFFFFF,  BIT0,
  0,  0x0A8,  0xFFFFFFFF,  BIT0,
  0,  0x0B0,  0xFFFFFFFF,  BIT0,
  0,  0x0B4,  0xFFFFFFFF,  BIT0,
  0,  0x0B8,  0xFFFFFFFF,  BIT0,
  0,  0x0BC,  0xFFFFFFFF,  BIT0,
  MCH_BASE_ADDRESS,  0x50FC,  0xFFFFFFFF,  0x8F,
  MCH_BASE_ADDRESS,  0x5880,  0xFFFFFFFF,  BIT5,
  MCH_BASE_ADDRESS,  0x7000,  0xFFFFFFFF,  BIT31,
  MCH_BASE_ADDRESS,  0x77FC,  0xFFFFFFFF,  BIT0,
  MCH_BASE_ADDRESS,  0x7FFC,  0xFFFFFFFF,  BIT0,
  MCH_BASE_ADDRESS,  0x6800,  0xFFFFFFFF,  BIT31,
  MCH_BASE_ADDRESS,  0x6020,  0xFFFFFFFF,  BIT0,
  MCH_BASE_ADDRESS,  0x5500,  0xFFFFFFFF,  BIT0,
  MCH_BASE_ADDRESS,  0x63FC,  0xFFFFFFFF,  BIT0,
  MCH_BASE_ADDRESS,  0x6008,  0xFFFFFFFF,  0,
  MCH_BASE_ADDRESS,  0x6030,  0xFFFFFFFF,  0,
  MCH_BASE_ADDRESS,  0x6034,  0xFFFFFFFF,  0
};

#ifdef PEG_FLAG
UINTN                           mDeviceCapMmBaseSave [MAX_SUPPORTED_ROOT_BRIDGE_NUMBER][MAX_SUPPORTED_DEVICE_NUMBER];
UINT32                          mDeviceExtCapLtrOffsetSave [MAX_SUPPORTED_ROOT_BRIDGE_NUMBER][MAX_SUPPORTED_DEVICE_NUMBER];
UINT32                          mDeviceExtCapVcOffsetSave [MAX_SUPPORTED_ROOT_BRIDGE_NUMBER][MAX_SUPPORTED_DEVICE_NUMBER];
UINT32                          mDeviceBusNumberSave [MAX_SUPPORTED_ROOT_BRIDGE_NUMBER][MAX_SUPPORTED_DEVICE_NUMBER];
UINT8                           mDeviceAspmSave [MAX_SUPPORTED_ROOT_BRIDGE_NUMBER][MAX_SUPPORTED_DEVICE_NUMBER];
UINT16                          mDeviceLtrObffSave [MAX_SUPPORTED_ROOT_BRIDGE_NUMBER][MAX_SUPPORTED_DEVICE_NUMBER];
UINT16                          mDeviceMaxSnoopLatencySave [MAX_SUPPORTED_ROOT_BRIDGE_NUMBER][MAX_SUPPORTED_DEVICE_NUMBER];
UINT16                          mDeviceMaxNoSnoopLatencySave [MAX_SUPPORTED_ROOT_BRIDGE_NUMBER][MAX_SUPPORTED_DEVICE_NUMBER];
UINT8                           mDeviceTcxVc0MappingSave [MAX_SUPPORTED_ROOT_BRIDGE_NUMBER][MAX_SUPPORTED_DEVICE_NUMBER];
UINT8                           mCommonClock [MAX_SUPPORTED_ROOT_BRIDGE_NUMBER] = {0, 0, 0};
UINT16                          mDeviceControlRegisterSave [MAX_SUPPORTED_ROOT_BRIDGE_NUMBER][MAX_SUPPORTED_DEVICE_NUMBER];
UINT8                           mNumberOfDeviceFound [MAX_SUPPORTED_ROOT_BRIDGE_NUMBER] = {0, 0, 0};
#endif  // PEG_FLAG

UINT8                           mPAMSave [MAX_PAM_REG_COUNT];
UINTN                           mSaChipsetPciRegistersSave [sizeof (mSaChipsetPciRegistersSaveTable) / sizeof (BOOT_SCRIPT_PCI_REGISTER_SAVE)];
UINTN                           mSaIgfxPciRegistersSave [sizeof (mSaIgfxPciRegistersSaveTable) / sizeof (BOOT_SCRIPT_PCI_REGISTER_SAVE)];
UINT8                           mSteppingId;
UINT8                           mBridgeId;
UINT8                           mBridgeSteppingId;

///
/// Store required policy setting in global variables.
///
UINT8                           mDxePlatformSaPolicyRevision;
SA_PCIE_ASPM_CONFIG             mPegAspmPerPort[MAX_SUPPORTED_ROOT_BRIDGE_NUMBER];
UINT8                           mPegAspmL0sPerPort[MAX_SUPPORTED_ROOT_BRIDGE_NUMBER];
BOOLEAN                         mCridEnable;
BOOLEAN                         mDevice4Enable;
PCIE_ASPM_DEV_INFO              *mPcieAspmDevsOverride;
PCIE_LTR_DEV_INFO               *mPcieLtrDevsOverride;
UINT16                          mSnoopLatencyOvrValue;
UINT16                          mNonSnoopLatencyOvrValue;
SA_PCIE_PWR_OPT                 mPegPwrOpt[SA_PEG_MAX_FUN];

//#ifdef EFI_DEBUG
UINT8                           mMaxBusNumberSupported;
//#endif
EFI_STATUS                      mEnumStatus;

#ifdef PEG_FLAG
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

  @retval 0            - CAPID not found
  @retval Other        - CAPID found, Offset of desired CAPID
**/
UINT32
LibPcieFindCapId (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function,
  IN UINT8   CapId
  )
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

/**
  Search and return the offset of desired Pci Express Capability ID
  CAPID list:
    0x0001 = Advanced Error Rreporting Capability
    0x0002 = Virtual Channel Capability
    0x0003 = Device Serial Number Capability
    0x0004 = Power Budgeting Capability

  @param[in] PegBaseAddress - DeviceMmBase which points to PCIe device register space.
  @param[in] CapId          - Extended CAPID to search for

  @retval 0       - CAPID not found
  @retval Other   - CAPID found, Offset of desired CAPID
**/
UINT32
LibPcieFindExtendedCapId (
  IN UINT32  PegBaseAddress,
  IN UINT16  CapId
  )
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
    CapHeaderId = MmioRead16 (PegBaseAddress + CapHeaderOffset);
    if (CapHeaderId == CapId) {
      return CapHeaderOffset;
    }

    CapHeaderOffset = (MmioRead16 (PegBaseAddress + CapHeaderOffset + 2) >> 4);
  }

  return 0;
}

/**
  Enumerate all end point devices connected to root bridge ports and record their MMIO base address

  @exception EFI_UNSUPPORTED      PCIe capability structure not found
  @retval    EFI_SUCCESS          All done successfully
**/
EFI_STATUS
EnumerateAllPcieDevices (
  VOID
  )
{
  UINT32      CapOffset1;
  UINT32      CapOffset2;
  UINTN       RootPortMmBase;
  UINT8       SubBusNum;
  UINT8       BusNum;
  UINT8       PegBus;
  UINT8       PegDev;
  UINT8       PegFunc;
  UINT8       Bus;
  UINT8       Dev;
  UINT8       Func;
  UINT8       RootBridgeCompleted;
  UINT8       RootBridgeDeviceNumber [MAX_SUPPORTED_ROOT_BRIDGE_NUMBER] = {1, 1, 1};
  UINT8       RootBridgeFunctionNumber [MAX_SUPPORTED_ROOT_BRIDGE_NUMBER] = {0, 1, 2};

//#ifdef EFI_DEBUG
  mMaxBusNumberSupported = 0xFF >> ((MmPci32 (0, SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_PCIEXBAR) & B_SA_PCIEXBAR_LENGTH_MASK) >> N_SA_PCIEXBAR_LENGTH_OFFSET);
//#endif
  PegBus = 0;
  for (RootBridgeCompleted = 0; RootBridgeCompleted < MAX_SUPPORTED_ROOT_BRIDGE_NUMBER; RootBridgeCompleted++) {
    PegDev  = RootBridgeDeviceNumber [RootBridgeCompleted];
    PegFunc = RootBridgeFunctionNumber [RootBridgeCompleted];
    if (MmPci16 (0, PegBus, PegDev, PegFunc, PCI_VID) == 0xFFFF) {
      continue;
    }
    ///
    /// Save Bridge Bus number assignment
    ///
    mDeviceBusNumberSave [RootBridgeCompleted][0] = MmPci32 (0, PegBus, PegDev, PegFunc, PCI_PBUS);
    SaScriptMemWrite (
      S3BootScriptWidthUint32,
      (UINTN) MmPciAddress (0, PegBus, PegDev, PegFunc, PCI_PBUS),
      1,
      &mDeviceBusNumberSave [RootBridgeCompleted][0]
      );

    ///
    /// Get the pointer to the Port PCI Express Capability Structure.
    ///
    CapOffset1  = LibPcieFindCapId (PegBus, PegDev, PegFunc, 0x10);
    if (CapOffset1 == 0) {
      DEBUG ((EFI_D_ERROR, "ERROR: Cannot find Root Bridge PCIE capability structure.\n"));
      return EFI_UNSUPPORTED;
    }

    ///
    /// Save root bridge device MM base
    ///
    RootPortMmBase = MmPciAddress (0, PegBus, PegDev, PegFunc, 0);
    mDeviceCapMmBaseSave [RootBridgeCompleted][0] = RootPortMmBase + CapOffset1;
    DEBUG ((EFI_D_INFO, "Root bridge [B%X|D%X|F%X]:", PegBus, PegDev, PegFunc));
    DEBUG ((EFI_D_INFO, " mDeviceCapMmBaseSave [%d][0] = 0x%X\n", RootBridgeCompleted, mDeviceCapMmBaseSave [RootBridgeCompleted][0]));
    mNumberOfDeviceFound [RootBridgeCompleted] ++;

    ///
    /// Get Secondary bus number and Subordinate bus number for end point device enumeration
    ///
    BusNum = MmPci8 (0, PegBus, PegDev, PegFunc, PCI_SBUS);
    SubBusNum = MmPci8 (0, PegBus, PegDev, PegFunc, PCI_SUBUS);
    for (Bus = SubBusNum; Bus >= BusNum; Bus--) {
      for (Dev = 0; Dev < 32; Dev++) {
        for (Func =0; Func <=7; Func++) {
          ///
          /// Read Vendor ID to check if device exists
          /// if no device exists, then check next device
          ///
          if (MmPci16 (0, Bus, Dev, 0, PCI_VID) == 0xFFFF) {
            ///
            /// No other functions present if Func0 is not preset
            ///
            break;
          }
          if (MmPci16 (0, Bus, Dev, Func, PCI_VID) == 0xFFFF) {
            continue;
          }

          DEBUG ((EFI_D_INFO, "End point [B%X|D%X|F%X]:", Bus, Dev, Func));
          CapOffset2 = LibPcieFindCapId (Bus, Dev, Func, 0x10);
          if (CapOffset2 != 0) {
            ///
            /// Save end point device MM base
            ///
            if (mNumberOfDeviceFound [RootBridgeCompleted] >= MAX_SUPPORTED_DEVICE_NUMBER) {
              DEBUG ((EFI_D_ERROR, "ERROR: DeviceMmBaseSave array size not big enough.\n"));
              return EFI_BUFFER_TOO_SMALL;
            } else {
              ///
              /// Save Downstream Bus number assignment
              ///
              mDeviceBusNumberSave [RootBridgeCompleted][mNumberOfDeviceFound [RootBridgeCompleted]] = MmPci32 (0, Bus, Dev, Func, PCI_PBUS);
              SaScriptMemWrite (
                S3BootScriptWidthUint32,
                (UINTN) MmPciAddress (0, Bus, Dev, Func, PCI_PBUS),
                1,
                &mDeviceBusNumberSave [RootBridgeCompleted][mNumberOfDeviceFound [RootBridgeCompleted]]
                );
              mDeviceCapMmBaseSave [RootBridgeCompleted][mNumberOfDeviceFound [RootBridgeCompleted]] = MmPciAddress (0, Bus, Dev, Func, 0) + CapOffset2;
              DEBUG ((EFI_D_INFO, " mDeviceCapMmBaseSave [%d][%d]", RootBridgeCompleted, mNumberOfDeviceFound [RootBridgeCompleted]));
              DEBUG ((EFI_D_INFO, " = 0x%X\n", mDeviceCapMmBaseSave [RootBridgeCompleted][mNumberOfDeviceFound [RootBridgeCompleted]]));
              mNumberOfDeviceFound [RootBridgeCompleted] ++;
            }
          }
        }
      }
    }
    DEBUG ((EFI_D_INFO, "mNumberOfDeviceFound [%d] = %d\n", RootBridgeCompleted, mNumberOfDeviceFound [RootBridgeCompleted]));
  }
  return EFI_SUCCESS;
}

/**
  Initialize VC0 traffic class

  @param[in] RootBridgeCompleted - The first index (point to root bridge) of the device MMIO Base array to select bridge which currently under working or completed.
  @param[in] EndpointCompleted   - The second index (point to end point devices) of the device MMIO Base array to select device which currently under working or completed.

  @exception EFI_UNSUPPORTED - VC capability ID not found
  @retval EFI_SUCCESS     - VC mapping correctly initialized
**/
EFI_STATUS
PcieInitTcxVc0 (
  IN UINT8   RootBridgeCompleted,
  IN UINT8   EndpointCompleted
  )
{
  UINT32  Offset;
  UINT8   Data8And;
  UINT8   Data8Or;
  UINT32  PegBaseAddress;
  UINT8   i;
  UINT8   CapId[] = {'2','8'};

  Offset         = 0;
  i              = 0;
  PegBaseAddress = (UINT32) (mDeviceCapMmBaseSave [RootBridgeCompleted][EndpointCompleted] & (UINTN) ~0xFF);
  ///
  /// Set TCx-VC0 value - map all TCs to VC0
  ///
  ///
  /// Fix for ClientSW s5039821: PTC Gen3 BIOS Test : MFVC Capability test failure
  ///
  while ((Offset == 0x0) && (i < sizeof(CapId))) {
    mDeviceExtCapVcOffsetSave [RootBridgeCompleted][EndpointCompleted] |= LibPcieFindExtendedCapId (PegBaseAddress, CapId[i]);
    Offset = mDeviceExtCapVcOffsetSave [RootBridgeCompleted][EndpointCompleted];
    i++;
  }

  DEBUG ((EFI_D_INFO, "PTC SaPcieLib.c - Offset: 0x%x\n", Offset));
  if (Offset == 0) {
    DEBUG ((EFI_D_INFO, "VC ExtCap structure not found on device [0x%08X]\n", PegBaseAddress));
    return EFI_UNSUPPORTED;
  }

  Data8And        = 0;
  Data8Or         = BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7;
  MmioAndThenOr8 (PegBaseAddress + Offset + 0x014, Data8And, Data8Or);
  SaScriptMemReadWrite (
    S3BootScriptWidthUint8,
    (UINTN) (PegBaseAddress + Offset + 0x014),
    &Data8Or,
    &Data8And
    );
  mDeviceTcxVc0MappingSave [RootBridgeCompleted][EndpointCompleted] = MmioRead8 (PegBaseAddress + Offset + 0x014);
  return EFI_SUCCESS;
}

/**
  Sets Common Clock, TCx-VC0 mapping, and Max Payload for PCIe
**/
VOID
SaPcieConfigBeforeOpRom (
  VOID
  )
{
  UINT8       RootBridgeCompleted;
  UINT8       EndpointCompleted;
  UINT16      EndpointMaxPayload;
  UINTN       RootPortPcieCapMmBase;
  UINTN       EndPointPcieCapMmBase;
  UINT16      Data16And;
  UINT16      Data16Or;
  UINT16      MaxPayload [MAX_SUPPORTED_ROOT_BRIDGE_NUMBER];

  for (RootBridgeCompleted = 0; RootBridgeCompleted < MAX_SUPPORTED_ROOT_BRIDGE_NUMBER; RootBridgeCompleted++) {
    RootPortPcieCapMmBase = mDeviceCapMmBaseSave [RootBridgeCompleted][0];
    EndPointPcieCapMmBase = mDeviceCapMmBaseSave [RootBridgeCompleted][1];

    if (RootPortPcieCapMmBase == 0) {
      continue;
    }
    ///
    /// Get the PCIE root Port Max Payload Size support.
    ///
    MaxPayload [RootBridgeCompleted] = MmioRead16 (RootPortPcieCapMmBase + 4) & (BIT2 | BIT1 | BIT0);
    ///
    /// Check the Port Slot Clock Configuration Bit.
    ///
    if ((MmioRead16 (RootPortPcieCapMmBase + 0x012) & BIT12) == 0) {
      ///
      /// Indicate CommonClock is not supported on this port
      ///
      mCommonClock [RootBridgeCompleted] = 0xFF;
      DEBUG ((EFI_D_INFO, "CommonClock not supported by root bridge [B%X|D%X|F%X|R%X]\n",
                             (RootPortPcieCapMmBase >> 20)   & mMaxBusNumberSupported,
                             (RootPortPcieCapMmBase >> 15)   & 0x1F,
                             (RootPortPcieCapMmBase >> 12)   & 0x07,
                             (RootPortPcieCapMmBase + 0x012) & 0xFFF));
    }
    for (EndpointCompleted = 1; EndpointCompleted < mNumberOfDeviceFound [RootBridgeCompleted]; EndpointCompleted++) {
      ///
      /// Set TCx-VC0 mapping on Endpoint
      ///
      PcieInitTcxVc0 (RootBridgeCompleted, EndpointCompleted);

      EndPointPcieCapMmBase = mDeviceCapMmBaseSave [RootBridgeCompleted][EndpointCompleted];
      ///
      /// Get the end point function Max Payload Size support
      ///
      EndpointMaxPayload = MmioRead16 (EndPointPcieCapMmBase + 4) & (BIT2 | BIT1 | BIT0);
      DEBUG ((EFI_D_INFO, "CAP_MPS of [%X] = %X\n", EndPointPcieCapMmBase + 4, EndpointMaxPayload));
      ///
      /// Obtain the minimum Max Payload Size between the PCIE root Port and the end point functions
      ///
      if (MaxPayload [RootBridgeCompleted] > EndpointMaxPayload) {
        MaxPayload [RootBridgeCompleted] = EndpointMaxPayload;
      }
      ///
      /// Check the Endpoint Slot Clock Configuration Bit.
      ///
      if ((MmioRead16 (EndPointPcieCapMmBase + 0x012) & BIT12) != 0) {
        ///
        /// Common clock is supported, set common clock bit on root port
        /// and the endpoint
        ///
        if (mCommonClock [RootBridgeCompleted] == 0) {
          MmioOr8 (RootPortPcieCapMmBase + 0x010, BIT6);
          Data16Or  = BIT6;
          Data16And = (UINT16)~BIT6;
          SaScriptMemReadWrite (
            S3BootScriptWidthUint8,
            (UINTN) (RootPortPcieCapMmBase + 0x010),
            &Data16Or,  ///< Data to be ORed
            &Data16And  ///< Data to be ANDed
            );
          mCommonClock [RootBridgeCompleted] = 1;
          DEBUG ((EFI_D_INFO, "Set CommonClock on Root [B%X|D%X|F%X|R%X]\n",
                                 (RootPortPcieCapMmBase >> 20)   & mMaxBusNumberSupported,
                                 (RootPortPcieCapMmBase >> 15)   & 0x1F,
                                 (RootPortPcieCapMmBase >> 12)   & 0x07,
                                 (RootPortPcieCapMmBase + 0x010) & 0xFFF));
        }
        MmioOr8 (EndPointPcieCapMmBase + 0x010, BIT6);
        Data16Or  = BIT6;
        Data16And = (UINT16)~BIT6;
        SaScriptMemReadWrite (
          S3BootScriptWidthUint8,
          (UINTN) (EndPointPcieCapMmBase + 0x010),
          &Data16Or,  ///< Data to be ORed
          &Data16And  ///< Data to be ANDed
          );
        DEBUG ((EFI_D_INFO, "Set CommonClock on Device [B%X|D%X|F%X|R%X]\n",
                               (EndPointPcieCapMmBase >> 20)   & mMaxBusNumberSupported,
                               (EndPointPcieCapMmBase >> 15)   & 0x1F,
                               (EndPointPcieCapMmBase >> 12)   & 0x07,
                               (EndPointPcieCapMmBase + 0x010) & 0xFFF));
      }
    }
    ///
    /// If common clock supported on root port and endpoint, retrain link
    ///
    if (mCommonClock [RootBridgeCompleted] == 1) {
      DEBUG ((EFI_D_INFO, "CommonClock supported. Retrain link\n"));
      ///
      /// Retrain the Link per PCI Express Specification.
      ///
      MmioOr8 (RootPortPcieCapMmBase + 0x010, BIT5);
      Data16Or = BIT5;
      Data16And = (UINT8) ~BIT5;
      SaScriptMemReadWrite (
        S3BootScriptWidthUint8,
        (UINTN) (RootPortPcieCapMmBase + 0x010),
        &Data16Or,
        &Data16And
        );
      ///
      /// Wait until Re-Training has completed.
      ///
      while ((MmioRead16 (RootPortPcieCapMmBase + 0x012) & BIT11) != 0) {}
      ///
      /// Poll until BIT11 clear. 200 ms as polling timeout
      ///
      Data16Or  = BIT11;
      Data16And = 0;
      SaScriptMemPoll (
        S3BootScriptWidthUint16,
        (UINTN) (RootPortPcieCapMmBase + 0x012),
        &Data16Or,
        &Data16And,
        1,
        200000
        );
    }

    ///
    /// Set Max Payload to all root and end point devices
    ///
    MaxPayload [RootBridgeCompleted] <<= 5;
    EndpointCompleted = 0;
    while (EndpointCompleted < mNumberOfDeviceFound [RootBridgeCompleted]) {
      EndPointPcieCapMmBase = mDeviceCapMmBaseSave [RootBridgeCompleted][EndpointCompleted];
      DEBUG ((EFI_D_INFO, "Set MPS of [%X] to %X\n", EndPointPcieCapMmBase + 0x8, MaxPayload [RootBridgeCompleted]));
      MmioAndThenOr16 (EndPointPcieCapMmBase + 0x8, (UINT16)~(BIT7 | BIT6 | BIT5), MaxPayload [RootBridgeCompleted]);
      ///
      /// S3 Save for Device Control Register on all bridges and devices is done after the OpRom has run in EnableExtendedTag.
      ///
      EndpointCompleted ++;
    }
  }
}

/**
  Calculate ASPM Auto setting value

  @param[in] RootBridgeIndex   - Root Bridge Index to select mDeviceCapMmBaseSave array elements for ASPM capability calculation.

  @retval AspmVal - ASPM settings for RP and all EP
**/
UINT16
PcieCalcAspmSettings (
  IN  UINT8   RootBridgeIndex
  )
{
  UINT16              RootPortAspm;
  UINT16              EndPointAspm;
  UINT32              PcieAspmDev;
  UINT16              EndPointVendorId;
  UINT16              EndPointDeviceId;
  UINT8               EndPointRevId;
  UINT16              AspmVal;
  UINT32              PortLxLat;
  UINT32              EndPointLxLat;
  UINT32              LxLat;
  UINT8               EndpointCompleted;
  UINTN               RootPortPcieCapMmBase;
  UINT32              RootPortLinkCap;
  UINTN               EndPointPcieCapMmBase;
  UINT32              EndPointLinkCap;
  UINT32              EndPointDevCap;

  RootPortPcieCapMmBase = mDeviceCapMmBaseSave [RootBridgeIndex][0];
  RootPortLinkCap       = MmioRead32 (RootPortPcieCapMmBase + 0x0C);

  ///
  /// Obtain initial ASPM settings from respective port capability registers.
  ///
  RootPortAspm = (RootPortLinkCap >> 10) & 3;
  DEBUG ((EFI_D_INFO, "Root [%X] Aspm capability = %X\n", RootPortPcieCapMmBase + 0xC, RootPortAspm));
  AspmVal = RootPortAspm;
  EndpointCompleted = 1;

  while (EndpointCompleted < mNumberOfDeviceFound [RootBridgeIndex]) {
    EndPointPcieCapMmBase = mDeviceCapMmBaseSave [RootBridgeIndex][EndpointCompleted];
    EndPointDevCap        = MmioRead32 (EndPointPcieCapMmBase + 0x04);
    EndPointLinkCap       = MmioRead32 (EndPointPcieCapMmBase + 0x0C);

    EndPointAspm = (EndPointLinkCap >> 10) & 3;
    DEBUG ((EFI_D_INFO, "End point [%X] Aspm capability = %X\n", EndPointPcieCapMmBase + 0xC, EndPointAspm));
    if (mPcieAspmDevsOverride != NULL) {
      ///
      /// Mask APMC with values from lookup table.
      /// RevID of 0xFF applies to all steppings.
      ///
      EndPointVendorId  = MmioRead16 ((EndPointPcieCapMmBase & (UINTN) ~0xFF) + 0x000);
      EndPointDeviceId  = MmioRead16 ((EndPointPcieCapMmBase & (UINTN) ~0xFF) + 0x002);
      EndPointRevId     = MmioRead8  ((EndPointPcieCapMmBase & (UINTN) ~0xFF) + 0x008);
      DEBUG ((EFI_D_INFO, "End point [%X] VendorID:DeviceID = %04X:%04X RevID=%02X\n",
                EndPointPcieCapMmBase, EndPointVendorId, EndPointDeviceId, EndPointRevId));
      PcieAspmDev       = 0;
      while ((mPcieAspmDevsOverride[PcieAspmDev].VendorId != SA_PCIE_DEV_END_OF_TABLE)
          && (PcieAspmDev < MAX_PCIE_ASPM_OVERRIDE)) {
        if ((mPcieAspmDevsOverride[PcieAspmDev].VendorId == EndPointVendorId) &&
           (mPcieAspmDevsOverride[PcieAspmDev].DeviceId == EndPointDeviceId) &&
           ((mPcieAspmDevsOverride[PcieAspmDev].RevId == 0xFF) ||
            (mPcieAspmDevsOverride[PcieAspmDev].RevId == EndPointRevId))) {
          RootPortAspm &= mPcieAspmDevsOverride[PcieAspmDev].RootApmcMask;
          EndPointAspm &= mPcieAspmDevsOverride[PcieAspmDev].EndpointApmcMask;
          break;
        }
        PcieAspmDev++;
      }
    }

    if (AspmVal > EndPointAspm) {
      AspmVal = EndPointAspm;
      ///
      /// In case L0s and L1 both can't be enabled, return the result now.
      ///
      if((AspmVal & 0x03) == 0) {
        return AspmVal;
      }
    }
    ///
    /// Check if L1 should be enabled based on port and endpoint L1 exit latency.
    ///
    if (AspmVal & BIT1) {
      if (!(EndPointLinkCap & BIT22) && !(AspmVal & BIT0)) {
        DEBUG ((EFI_D_INFO, "\nDevice [%X] ASPM Optionality Compliance bit not set!", EndPointPcieCapMmBase));
        DEBUG ((EFI_D_INFO, "ASPM L1-Only setting is not supported!\n"));
        AspmVal = 0;
        return AspmVal;
      }
      PortLxLat = RootPortLinkCap & (BIT17 + BIT16 + BIT15);
      EndPointLxLat = EndPointLinkCap & (BIT17 + BIT16 + BIT15);
      DEBUG ((EFI_D_INFO, "Root [%X] L1 Exit Latency = %X\n", RootPortPcieCapMmBase + 0xC, PortLxLat >> 15));
      DEBUG ((EFI_D_INFO, "End  [%X] L1 Exit Latency = %X\n", EndPointPcieCapMmBase + 0xC, EndPointLxLat >> 15));
      LxLat = PortLxLat;
      if (PortLxLat < EndPointLxLat) {
        LxLat = EndPointLxLat;
      }
      ///
      /// check if the value is bigger than endpoint L1 acceptable exit latency, if it is
      /// larger than accepted value, then we should disable L1
      ///
      LxLat >>= 6;
      DEBUG ((EFI_D_INFO, "End [%X] L1 Acceptable Latency = %X\n", EndPointPcieCapMmBase + 0x4,
        (EndPointDevCap & (BIT11 + BIT10 + BIT9)) >> 9));
      if (LxLat > (EndPointDevCap & (BIT11 + BIT10 + BIT9))) {
        AspmVal &= ~BIT1;
        if((AspmVal & 0x03) == 0) {
          return AspmVal;
        }
      }
    }
    ///
    /// Check if L0s should be enabled based on port and endpoint L0s exit latency.
    ///
    if (AspmVal & BIT0) {
      PortLxLat     = RootPortLinkCap & (BIT14 + BIT13 + BIT12);
      EndPointLxLat = EndPointLinkCap & (BIT14 + BIT13 + BIT12);
      DEBUG ((EFI_D_INFO, "Root [%X] L0s Exit Latency = %X\n", RootPortPcieCapMmBase + 0xC, PortLxLat >> 12));
      DEBUG ((EFI_D_INFO, "End  [%X] L0s Exit Latency = %X\n", EndPointPcieCapMmBase + 0xC, EndPointLxLat >> 12));
      LxLat = PortLxLat;
      if (PortLxLat < EndPointLxLat) {
        LxLat = EndPointLxLat;
      }
      ///
      /// check if the value is bigger than endpoint L0s acceptable exit latency, if it is
      /// larger than accepted value, then we should disable L0s
      ///
      LxLat >>= 6;
      DEBUG ((EFI_D_INFO, "End [%X] L0s Acceptable Latency = %X\n", EndPointPcieCapMmBase + 0x4,
                                                            (EndPointDevCap & (BIT8 + BIT7 + BIT6)) >> 6));
      if (LxLat > (EndPointDevCap & (BIT8 + BIT7 + BIT6))) {
        AspmVal &= ~BIT0;
        if((AspmVal & 0x03) ==0) {
          return AspmVal;
        }
      }
    }
    EndpointCompleted ++;
  }
  return AspmVal;
}

/**
  This function compares the actual latency in LatencyValue1
  with actual latency in LatencyValue2 programs the minimum
  back to LatencyValue1, in the required format.

  @param[in] LatencyValue1  - Current latency value
  @param[in] LatencyValue2  - Latency value from the Table
**/
VOID
DetermineLatencyValue (
  IN UINT16   *LatencyValue1,
  IN UINT16   *LatencyValue2
  )
{
  UINT8      Scale1;
  UINT8      Scale2;
  UINT64     ActualLatency1 = 0;
  UINT64     ActualLatency2 = 0;
  UINT32     Multiplier[6] = {
    1,
    32,
    1024,
    32768,
    1048576,
    33554432
  };

  Scale1 = LTR_MULTIPLIER_INDEX(*LatencyValue1);
  Scale2 = LTR_MULTIPLIER_INDEX(*LatencyValue2);
  if ((Scale1 <= 5) && (Scale2 <= 5)) {
    ActualLatency1 = LTR_LATENCY_VALUE(*LatencyValue1) * Multiplier[Scale1];
    ActualLatency2 = LTR_LATENCY_VALUE(*LatencyValue2) * Multiplier[Scale2];
  }

  ///
  /// Store the lower latency value and corresponding scale bits back to LatencyValue1
  /// and set the Force bit
  ///
  if ((ActualLatency1 == 0) || ActualLatency1 > ActualLatency2) {
    *LatencyValue1 = *LatencyValue2;
  }
}

/**
  This function will scan the LTR override table and update the default values for snoop and non-snoop latencies.

  @param[in] PegBaseAddress   - DeviceMmBase which points to PCIe device register space.
**/
VOID
ScanLtrOverrideTable (
  IN  UINT32  PegBaseAddress
  )
{
  UINT8               EndPointRevId;
  UINT16              EndPointVendorId;
  UINT16              EndPointDeviceId;
  UINT32              PcieLtrDev;

  EndPointVendorId  = MmioRead16 (PegBaseAddress + 0x000);
  EndPointDeviceId  = MmioRead16 (PegBaseAddress + 0x002);
  EndPointRevId     = MmioRead8  (PegBaseAddress + 0x008);
  if (mPcieLtrDevsOverride != NULL) {
    PcieLtrDev = 0;
    while ((mPcieLtrDevsOverride[PcieLtrDev].VendorId != SA_PCIE_DEV_END_OF_TABLE)
        && (PcieLtrDev < MAX_PCIE_LTR_OVERRIDE)) {
      if ((mPcieLtrDevsOverride[PcieLtrDev].VendorId == EndPointVendorId) &&
         (mPcieLtrDevsOverride[PcieLtrDev].DeviceId == EndPointDeviceId) &&
         ((mPcieLtrDevsOverride[PcieLtrDev].RevId == 0xFF) ||
         (mPcieLtrDevsOverride[PcieLtrDev].RevId == EndPointRevId))
          ) {
        if (mPcieLtrDevsOverride[PcieLtrDev].SnoopLatency & BIT15) {
          DetermineLatencyValue(&mSnoopLatencyOvrValue, &mPcieLtrDevsOverride[PcieLtrDev].SnoopLatency);
        }
        if (mPcieLtrDevsOverride[PcieLtrDev].NonSnoopLatency & BIT15) {
          DetermineLatencyValue(&mNonSnoopLatencyOvrValue, &mPcieLtrDevsOverride[PcieLtrDev].NonSnoopLatency);
        }
        break;
      }
      PcieLtrDev++;
    }
  }
}

/**
  Configure LTR/OBFF on end point device

  @param[in] RootBridgeIndex   - Root Bridge index number to select mDeviceCapMmBaseSave array element for end point device MmBase.
  @param[in] EndpointCompleted - End point device index number to select mDeviceCapMmBaseSave array element for end point device MmBase.
**/
VOID
PcieSetEndpointLtrObff (
  IN UINT8      RootBridgeIndex,
  IN UINT8      EndpointCompleted
  )
{

  UINT16  Data16Or;
  UINT16  Data16And;
  UINT32  DeviceCapabilities2;
  UINT32  PegBaseAddress;
  UINTN   EndpointCapMmBase;
  UINT32  ExtendedCapOffset;
  UINT16  DefaultMaxLatency;
  UINT8   PegFunc;

  PegFunc = (UINT8) ((mDeviceCapMmBaseSave [RootBridgeIndex][0] >> 12) & 0x07);
  if (PegFunc >= SA_PEG_MAX_FUN) {
    DEBUG ((EFI_D_ERROR, "PegFunc out of bound! Exit from PcieSetEndpointLtrObff()!\n"));
    ASSERT (PegFunc < SA_PEG_MAX_FUN);
    return;
  }

  EndpointCapMmBase = mDeviceCapMmBaseSave [RootBridgeIndex][EndpointCompleted];
  ///
  /// Scan the LTR override table
  ///
  PegBaseAddress = (UINT32) (EndpointCapMmBase & (UINTN) ~0xFF);
  ScanLtrOverrideTable (PegBaseAddress);
  DeviceCapabilities2 = MmioRead32 (EndpointCapMmBase + 0x24);
  Data16And           = (UINT16)~(BIT10 + BIT13 + BIT14);
  Data16Or            = 0;
  ///
  /// If mSnoopLatencyOvrValue and mNonSnoopLatencyOvrValue has a value of 0, then this endpoint is not
  /// part of the override table If it supports LTR messaging then enable the capability
  ///
  if ((mSnoopLatencyOvrValue == 0) &&
      (mNonSnoopLatencyOvrValue == 0)) {
    if (DeviceCapabilities2 & BIT11) {
      // If PegPortLtrEnable set to true
      //
      if (mPegPwrOpt[PegFunc].LtrEnable == 1) {
        Data16Or |= BIT10;
      }
    }
  }
  ///
  /// Check if endpoint device is capable of OBFF using WAKE# signaling
  ///
  if ((DeviceCapabilities2 & BIT19) != 0) {
    if (mPegPwrOpt[PegFunc].ObffEnable == 1) {
      Data16Or |= BIT14 | BIT13; ///< 11b - Enable OBFF using WAKE# signaling
    }
  }
  DEBUG ((EFI_D_INFO, "Configure LTR/OBFF setting to EndPoint: [0x%x] to value 0x%x\n",
          EndpointCapMmBase + 0x28, Data16Or));
  MmioAndThenOr16 (EndpointCapMmBase + 0x28, Data16And, Data16Or);
  SaScriptMemReadWrite (
    S3BootScriptWidthUint16,
    (UINTN) (EndpointCapMmBase + 0x28),
    &Data16Or,
    &Data16And
    );
  mDeviceLtrObffSave [RootBridgeIndex][EndpointCompleted] = MmioRead16 (EndpointCapMmBase + 0x28);
  ///
  ///
  /// Program LTR Max Latencies
  ///
  if (DeviceCapabilities2 & BIT11) {
    if (mPegPwrOpt[PegFunc].LtrEnable == 1) {
      ///
      /// Get the pointer to the Endpoint PCI Express Extended Capability Structure.
      ///
      mDeviceExtCapLtrOffsetSave [RootBridgeIndex][EndpointCompleted] = LibPcieFindExtendedCapId (PegBaseAddress, 0x18);
      ExtendedCapOffset = mDeviceExtCapLtrOffsetSave [RootBridgeIndex][EndpointCompleted];
      Data16And         = (UINT16) (~0x1FFF);
      Data16Or          = mSnoopLatencyOvrValue;
      DefaultMaxLatency = mPegPwrOpt[PegFunc].LtrMaxSnoopLatency;
      if (ExtendedCapOffset != 0) {
        DetermineLatencyValue(&Data16Or, &DefaultMaxLatency);
        MmioAndThenOr16 (PegBaseAddress + ExtendedCapOffset + 4, Data16And, Data16Or);
        SaScriptMemReadWrite (
          S3BootScriptWidthUint16,
          (UINTN) (PegBaseAddress + ExtendedCapOffset + 4),
          &Data16Or,  ///< Data to be ORed
          &Data16And  ///< Data to be ANDed
          );
        mDeviceMaxSnoopLatencySave [RootBridgeIndex][EndpointCompleted] = MmioRead16 (PegBaseAddress + ExtendedCapOffset + 0x4);
        Data16Or           = mNonSnoopLatencyOvrValue;
        DefaultMaxLatency  = mPegPwrOpt[PegFunc].LtrMaxNoSnoopLatency;
        DetermineLatencyValue(&Data16Or, &DefaultMaxLatency);
        MmioAndThenOr16 (PegBaseAddress + ExtendedCapOffset + 6, Data16And, Data16Or);
        SaScriptMemReadWrite (
          S3BootScriptWidthUint16,
          (UINTN) (PegBaseAddress + ExtendedCapOffset + 6),
          &Data16Or,  ///< Data to be ORed
          &Data16And  ///< Data to be ANDed
          );
        mDeviceMaxNoSnoopLatencySave [RootBridgeIndex][EndpointCompleted] = MmioRead16 (PegBaseAddress + ExtendedCapOffset + 0x6);
      } else {
        DEBUG ((EFI_D_INFO, "LTR ExtCap structure not found on device [0x%08X]\n", PegBaseAddress));
      }
    }
  }
}

/**
  This function programs the LTR Override values
**/
VOID
PcieILtrOverride (
  VOID
  )
{
  UINT64  MchBar;
  UINT32  Data32And;
  UINT32  Data32Or;

  MchBar    = McD0PciCfg64 (R_SA_MCHBAR) &~BIT0;
  Data32And = 0x0;

  ///
  /// Configure Snoop Latency value
  ///
  if (mSnoopLatencyOvrValue != 0) {
    mSnoopLatencyOvrValue |= BIT14;
  }
  Data32Or = ((UINT32) (mSnoopLatencyOvrValue << 16));


  ///
  /// Configure Non-Snoop Latency value
  ///
  if (mNonSnoopLatencyOvrValue != 0) {
    mNonSnoopLatencyOvrValue |= BIT14;
  }
  Data32Or |= mNonSnoopLatencyOvrValue;

  ///
  /// Program ILTR_OVRD with latency values
  ///
  Mmio32AndThenOr (MchBar, R_SA_MCHBAR_ILTR_OVRD_OFFSET, Data32And, Data32Or);
  SaScriptMemReadWrite (
    S3BootScriptWidthUint32,
    (UINTN) (MchBar + R_SA_MCHBAR_ILTR_OVRD_OFFSET),
    &Data32Or,  /// Data to be ORed
    &Data32And  /// Data to be ANDed
    );
}

/**
  This function enumerate devices to apply Peg Aspm settings.

  @param[in] RootBridgeIndex   - Root Bridge Index to select mDeviceCapMmBaseSave array elements for ASPM capability calculation.
  @param[in] AutoFlag          - ASPM AUTO calculation mode
  @param[in] RootPortAspm      - ASPM setting for Root port
  @param[in] EndPointAspm      - ASPM setting for End point
**/
VOID
PcieEnumerateDeviceToApplyASPM (
  IN UINT8      RootBridgeIndex,
  IN BOOLEAN    AutoFlag,
  IN UINT16     RootPortAspm,
  IN UINT16     EndPointAspm
  )
{
  UINT16  LinkAspmCap;
  UINT16  LinkAspmVal;
  UINT8   EndpointCompleted;
  UINT32  PegBaseAddress;
  UINT16  Data16Or;
  UINT16  Data16And;
  UINT32  DeviceCapabilities2;
  UINT8   PegFunc;

  PegFunc = (UINT8) ((mDeviceCapMmBaseSave [RootBridgeIndex][0] >> 12) & 0x07);
  if (PegFunc >= SA_PEG_MAX_FUN) {
    DEBUG ((EFI_D_ERROR, "PegFunc out of bound! Exit from PcieEnumerateDeviceToApplyASPM()!\n"));
    ASSERT (PegFunc < SA_PEG_MAX_FUN);
    return;
  }

  if(AutoFlag) {
    LinkAspmVal = PcieCalcAspmSettings(RootBridgeIndex);
    RootPortAspm = LinkAspmVal & 0xFF;
    EndPointAspm = LinkAspmVal & 0xFF;
    DEBUG ((EFI_D_INFO, "PcieCalcAspmSettings return ASPM value as: %x, apply on both RP and EP\n", RootPortAspm));
  }

  ///
  /// Apply to root port first
  ///
  MmioAndThenOr16 (mDeviceCapMmBaseSave [RootBridgeIndex][0] + 0x010, (UINT16)~3, RootPortAspm);
  DEBUG ((EFI_D_INFO, "Apply Aspm settings to Root [%X] to Aspm value: 0x%x\n", mDeviceCapMmBaseSave [RootBridgeIndex][0] + 0x010, RootPortAspm));
  ///
  /// Save register setting for S3 resume.
  ///
  mDeviceAspmSave [RootBridgeIndex][0] = MmioRead8 (mDeviceCapMmBaseSave [RootBridgeIndex][0] + 0x010);
  SaScriptMemWrite (
    S3BootScriptWidthUint8,
    (UINTN) (mDeviceCapMmBaseSave [RootBridgeIndex][0] + 0x010),
    1,
    &mDeviceAspmSave [RootBridgeIndex][0]
    );
  ///
  /// Apply Power Optimizer settings to PEG RootPoints.
  /// Read DCAP2 to see if LTR/OBFF is supported, program DCTL2 according to policies.
  ///
  PegBaseAddress = (UINT32) (mDeviceCapMmBaseSave [RootBridgeIndex][0] & (UINTN) ~0xFF);
  DeviceCapabilities2 = MmioRead32 (PegBaseAddress + R_SA_PEG_DCAP2_OFFSET);
  Data16And           = (UINT16)~(BIT10 + BIT13 + BIT14);
  Data16Or            = 0;

  if (DeviceCapabilities2 & BIT11) {
    if (mPegPwrOpt[PegFunc].LtrEnable == 1) {
    Data16Or |= BIT10;
    }
  }
  ///
  /// Check if root port support WAKE# signaling
  ///
  if ((DeviceCapabilities2 & BIT19) != 0) {
    if (mPegPwrOpt[PegFunc].ObffEnable == 1) {
    Data16Or |= BIT14 | BIT13; ///< 11b - Enable OBFF using WAKE#
    }
  }

  DEBUG ((EFI_D_INFO, "Configure LTR/OBFF setting to PEG root [0x%x] to value 0x%x\n",
          PegBaseAddress + R_SA_PEG_DCTL2_OFFSET, Data16Or));
  MmioAndThenOr16 (PegBaseAddress + R_SA_PEG_DCTL2_OFFSET, Data16And, Data16Or);
  SaScriptMemReadWrite (
    S3BootScriptWidthUint16,
    (UINTN) (PegBaseAddress + R_SA_PEG_DCTL2_OFFSET),
    &Data16Or,
    &Data16And
    );
  mDeviceLtrObffSave [RootBridgeIndex][0] = MmioRead16 (PegBaseAddress + R_SA_PEG_DCTL2_OFFSET);

  ///
  /// Apply to end point devices
  ///
  EndpointCompleted = 1;
  while (EndpointCompleted < mNumberOfDeviceFound [RootBridgeIndex]) {
    ///
    /// Read the Link Capability register's ASPM setting and apply ASPM setting to end point device
    ///
    LinkAspmCap = (MmioRead16 (mDeviceCapMmBaseSave [RootBridgeIndex][EndpointCompleted] + 0x00C) >> 10) & 3;
    MmioAndThenOr16 (mDeviceCapMmBaseSave [RootBridgeIndex][EndpointCompleted] + 0x010, (UINT16)~3, EndPointAspm & LinkAspmCap);
    DEBUG ((EFI_D_INFO, "Apply Aspm settings to device [%X] to Aspm value: 0x%x and in fact set to 0x%x\n", mDeviceCapMmBaseSave [RootBridgeIndex][EndpointCompleted] + 0x10, EndPointAspm, EndPointAspm & LinkAspmCap));
    ///
    /// Save register setting for S3 resume.
    ///
    mDeviceAspmSave [RootBridgeIndex][EndpointCompleted] = MmioRead8 (mDeviceCapMmBaseSave [RootBridgeIndex][EndpointCompleted] + 0x010);
    SaScriptMemWrite (
      S3BootScriptWidthUint8,
      (UINTN) (mDeviceCapMmBaseSave [RootBridgeIndex][EndpointCompleted] + 0x010),
      1,
      &mDeviceAspmSave [RootBridgeIndex][EndpointCompleted]
      );
    ///
    /// Power optimization on end point device
    ///
    PcieSetEndpointLtrObff (RootBridgeIndex, EndpointCompleted);
    EndpointCompleted ++;
  }
}

/**
  This function apply additional settings before ASPM enabling

  @param[in] RootBridgeIndex   - Root Bridge Index to select mDeviceCapMmBaseSave array elements for ASPM capability calculation.
**/
VOID
PcieAdditionalSettingBeforeASPM (
  IN UINT8      RootBridgeIndex
  )
{
  UINT32 Data32Or;
  UINT32 Data32And;

  if ((mPegAspmPerPort[RootBridgeIndex] != PcieAspmDisabled) && (mCommonClock [RootBridgeIndex] == 1)) {
    ///
    /// Enable support for L0s and L1 by programming the `Active State Link
    /// PM Support' field of the LCAP register at D.F.R 0ACh [11:10] = `11b'.
    ///
    MmioOr32 ((mDeviceCapMmBaseSave[RootBridgeIndex][0] & (UINTN) ~0xFF) + 0x0AC, BIT11 | BIT10);
    Data32Or  = (BIT11 | BIT10);
    Data32And = (UINT32)~(BIT11 | BIT10);
    SaScriptMemReadWrite (
      S3BootScriptWidthUint32,
      (UINTN) ((mDeviceCapMmBaseSave [RootBridgeIndex][0] & (UINTN) ~0xFF) + 0x0AC),
      &Data32Or,  ///< Data to be ORed
      &Data32And  ///< Data to be ANDed
      );

    ///
    /// Set D.F.R 0200h [27:26] to `00b'.
    ///
    MmioAnd32 ((mDeviceCapMmBaseSave[RootBridgeIndex][0] & (UINTN) ~0xFF) + 0x200, (UINT32) ~(BIT27 | BIT26));
    Data32Or  = 0;
    Data32And = (UINT32)~(BIT27 | BIT26);
    SaScriptMemReadWrite (
      S3BootScriptWidthUint32,
      (UINTN) ((mDeviceCapMmBaseSave [RootBridgeIndex][0] & (UINTN) ~0xFF) + 0x200),
      &Data32Or,  ///< Data to be ORed
      &Data32And  ///< Data to be ANDed
      );

    ///
    /// Set D.F.R 0258 [2] to '1b' for PCI Express 2.0 compliance
    /// Note: Other fields within this register must not be changed
    /// while writing to D1.F0.R 0258h [2]
    ///
    MmioOr16 ((mDeviceCapMmBaseSave [RootBridgeIndex][0] & (UINTN) ~0xFF) + R_SA_PEG_CFG4_OFFSET, BIT2);
    Data32Or  = BIT2;
    Data32And = (UINT32)~(BIT2);
    SaScriptMemReadWrite (
      S3BootScriptWidthUint32,
      (UINTN) ((mDeviceCapMmBaseSave [RootBridgeIndex][0] & (UINTN) ~0xFF) + R_SA_PEG_CFG4_OFFSET),
      &Data32Or,  ///< Data to be ORed
      &Data32And  ///< Data to be ANDed
      );
  }
}

/**
  This function does all SA ASPM initialization
**/
VOID
SaAspm (
  VOID
  )
{
  BOOLEAN                         AutoFlag;
  UINT16                          RootPortAspm;
  UINT16                          EndPointAspm;
  UINT8                           PegComplete;
  SA_PCIE_ASPM_CONFIG             PegAspmSetup;
  UINT8                           PegAspmL0sSetup;

  ///
  /// Scan PEG devices and program ASPM
  ///
  for (PegComplete = 0; PegComplete < MAX_SUPPORTED_ROOT_BRIDGE_NUMBER; PegComplete++) {
    if (mDeviceCapMmBaseSave [PegComplete][0] == 0) {
      continue;
    }
    PegAspmSetup      = mPegAspmPerPort[PegComplete];
    PegAspmL0sSetup   = mPegAspmL0sPerPort[PegComplete];
    RootPortAspm      = 0;
    EndPointAspm      = 0;
    AutoFlag          = FALSE;

    PcieAdditionalSettingBeforeASPM (PegComplete);
    DEBUG ((EFI_D_INFO, "PegAspmSetup[%x]=%x\n", PegComplete, PegAspmSetup));
    DEBUG ((EFI_D_INFO, "PegAspmL0sSetup[%x]=%x\n", PegComplete, PegAspmL0sSetup));
    if ((PegAspmSetup != PcieAspmDisabled) && (mCommonClock [PegComplete] == 1)) {
      ///
      /// Select L1/L0 programming based on setup questions
      ///
      switch(PegAspmSetup) {
        case PcieAspmL1:///<2
        case PcieAspmL0sL1:///<3
          RootPortAspm = L1_SET;
          EndPointAspm = L1_SET;

          ///
          /// If L1's only then break, else also program L0's
          ///
          if (PegAspmSetup == PcieAspmL1 ) {
            break;
            }
        case PcieAspmL0s:///<1
          RootPortAspm |= (PegAspmL0sSetup & L0_SET );
          EndPointAspm |= ((PegAspmL0sSetup >> 1 ) & L0_SET );
          break;
        case PcieAspmAutoConfig: ///<4
        case PcieAspmMax: ///<5
          AutoFlag = TRUE;
          break;
        case PcieAspmDisabled:
        default:
          break;
      }
    }
    DEBUG ((EFI_D_INFO, "RootPortAspm=%x\n", RootPortAspm ));
    DEBUG ((EFI_D_INFO, "EndPointAspm=%x\n", EndPointAspm ));
    PcieEnumerateDeviceToApplyASPM(PegComplete, AutoFlag, RootPortAspm, EndPointAspm);
  }
  ///
  /// Program override register with final latency values.
  ///
  PcieILtrOverride ();
  return;
}

/**
  This function checks PEG end point device for extended tag capability and enables them if they are.
**/
VOID
EnableExtendedTag (
  VOID
  )
{
  UINT8       RootBridgeCompleted;
  UINT8       EndpointCompleted;
  UINTN       RootPortPcieCapMmBase;
  UINTN       EndPointPcieCapMmBase;
  UINTN       PcieCapMmBase;

  ///
  /// Scan PEG devices
  ///
  for (RootBridgeCompleted = 0; RootBridgeCompleted < MAX_SUPPORTED_ROOT_BRIDGE_NUMBER; RootBridgeCompleted++) {
    RootPortPcieCapMmBase = mDeviceCapMmBaseSave [RootBridgeCompleted][0];
    if (RootPortPcieCapMmBase == 0) {
      continue;
    }
    for (EndpointCompleted = 0; EndpointCompleted < mNumberOfDeviceFound [RootBridgeCompleted]; EndpointCompleted++) {
      if (EndpointCompleted != 0) {
        EndPointPcieCapMmBase = mDeviceCapMmBaseSave [RootBridgeCompleted][EndpointCompleted];
        DEBUG ((EFI_D_INFO, "Extended tag scan End Point [B%X|D%X|F%X|R%X]\n",
                               (EndPointPcieCapMmBase >> 20)   & mMaxBusNumberSupported,
                               (EndPointPcieCapMmBase >> 15)   & 0x1F,
                               (EndPointPcieCapMmBase >> 12)   & 0x07,
                               (EndPointPcieCapMmBase + 0x010) & 0xFFF));
        ///
        /// If extended tag is supported, enable it.
        ///
        if (((MmioRead32 (EndPointPcieCapMmBase + 0x4)) & BIT5) != 0) {
          MmioOr16 (EndPointPcieCapMmBase + 0x8, BIT8);
          DEBUG ((EFI_D_INFO, "Extended tag enabled [B%X|D%X|F%X|R%X]\n",
                                 (EndPointPcieCapMmBase >> 20)   & mMaxBusNumberSupported,
                                 (EndPointPcieCapMmBase >> 15)   & 0x1F,
                                 (EndPointPcieCapMmBase >> 12)   & 0x07,
                                 (EndPointPcieCapMmBase + 0x010) & 0xFFF));
        }
      }
      ///
      /// Save Device Control Register value on all bridges and devices for S3 resume
      ///
      PcieCapMmBase = mDeviceCapMmBaseSave [RootBridgeCompleted][EndpointCompleted];
      mDeviceControlRegisterSave [RootBridgeCompleted][EndpointCompleted] = MmioRead16 (PcieCapMmBase + 0x8);
      SaScriptMemWrite (
        S3BootScriptWidthUint16,
        (UINTN) (PcieCapMmBase + 0x8),
        1,
        &mDeviceControlRegisterSave [RootBridgeCompleted][EndpointCompleted]
        );
    }
  }
  return;
}
#endif  // PEG_FLAG

/**
  This function saves/restores Chipset registers

  @param[in] IsSaving                  - TRUE for saving and FALSE for restoring
  @param[in] PciRegistersSaveTable[]   - The register table that has to be saved/restored
  @param[in] PciRegistersSaveTableSize - Size of above table
  @param[in] PciRegistersSaveBuffer    - A saving/restoring buffer for those register settings.
**/
VOID
SaSaveRestoreChipset (
  IN     BOOLEAN                       IsSaving,
  IN     BOOT_SCRIPT_PCI_REGISTER_SAVE PciRegistersSaveTable[],
  IN     UINTN                         PciRegistersSaveTableSize,
  IN OUT UINTN                         *PciRegistersSaveBuffer
)
{
  UINT8    Index;

  if (IsSaving == TRUE) {
    DEBUG ((EFI_D_INFO, "SA Save PCI register settings\n"));
    ///
    /// Save SA PCI Registers for S3 resume
    ///
    for (Index = 0; Index < PciRegistersSaveTableSize; Index++) {
      PciRegistersSaveBuffer[Index] = MmioRead32 (PciRegistersSaveTable[Index].Address);
      SaScriptMemWrite (
        S3BootScriptWidthUint32,
        (UINTN) PciRegistersSaveTable[Index].Address,
        1,
        &PciRegistersSaveBuffer[Index]
        );
      DEBUG ((EFI_D_INFO, "SA Register = %X, SaPciRegSave = %08X\n", PciRegistersSaveTable[Index].Address, PciRegistersSaveBuffer[Index]));
    }
  } else {
    DEBUG ((EFI_D_INFO, "SA Restore PCI register settings\n"));
    ///
    /// Restore SA PCI Registers for S3 resume
    ///
    for (Index = 0; Index < PciRegistersSaveTableSize; Index++) {
      MmioWrite32 (PciRegistersSaveTable[Index].Address, (UINT32) PciRegistersSaveBuffer[Index]);
      DEBUG ((EFI_D_INFO, "SA Register = %X, SaPciRegSave = %08X\n", PciRegistersSaveTable[Index].Address, PciRegistersSaveBuffer[Index]));
    }
  }
}

/**
  This function saves/restores platform relative registers

  @param[in] IsSaving                  - TRUE for saving and FALSE for restoring
**/
VOID
SaSaveRestorePlatform (
  IN BOOLEAN        IsSaving
)
{
  UINT8    Index;
  UINT8    Data8;

  ///
  /// Save (or restore) IGFX registers
  /// When saving, it has to be done after all BAR/Command registers have been assigned.
  ///
  if (MmPci16 (0, SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, PCI_VID) != 0xFFFF) {
    SaSaveRestoreChipset (IsSaving, mSaIgfxPciRegistersSaveTable, sizeof (mSaIgfxPciRegistersSaveTable) / sizeof (BOOT_SCRIPT_PCI_REGISTER_SAVE), mSaIgfxPciRegistersSave);
  }

  if (IsSaving == TRUE) {
    DEBUG ((EFI_D_INFO, "SA Save platform register settings\n"));
    ///
    /// Save PAM register
    ///
    for (Index = 0; Index < MAX_PAM_REG_COUNT; Index++) {
      mPAMSave[Index] = MmPci8 (0, SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, PAM_REG_BASE + Index);
      SaScriptMemWrite (
        S3BootScriptWidthUint8,
        (UINTN) MmPciAddress (0, SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, PAM_REG_BASE + Index),
        1,
        &mPAMSave[Index]
        );
    }
  } else {
    DEBUG ((EFI_D_INFO, "SA Restore platform register settings\n"));
    ///
    /// Restore PAM register
    ///
    for (Index = 0; Index < MAX_PAM_REG_COUNT; Index++) {
      MmPci8 (0, SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, PAM_REG_BASE + Index) = mPAMSave [Index];
    }
  }
  ///
  /// CRID configuration
  ///
  if (mCridEnable == TRUE) {
    Data8 = CRID_DATA;
  } else {
    Data8 = CRID_LOCK;
  }
  MmPci8 (0, SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, PCI_RID) = Data8;
  SaScriptMemWrite (
    S3BootScriptWidthUint8,
    (UINTN) MmPciAddress (0, SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, 0x08),
    1,
    &Data8
    );

}

/**
  This function does SA security lock
**/
VOID
SaSecurityLock (
  VOID
)
{
  UINT8           Index;
  UINT64          BaseAddress;
  UINT32          RegOffset;
  UINT32          Data32And;
  UINT32          Data32Or;
  CPU_STEPPING    CpuSteppingId;
  CPU_FAMILY      CpuFamilyId;

  CpuSteppingId = GetCpuStepping();
  CpuFamilyId   = GetCpuFamily();

  ///
  /// 17.2 System Agent Security Lock configuration
  ///
    DEBUG ((EFI_D_INFO, "SaSecurityLock\n"));
    for (Index = 0; Index < (sizeof (mSaSecurityRegisters) / sizeof (BOOT_SCRIPT_REGISTER_SETTING)); Index++) {
      BaseAddress = mSaSecurityRegisters[Index].BaseAddr;
      RegOffset   = mSaSecurityRegisters[Index].Offset;
      Data32And   = mSaSecurityRegisters[Index].AndMask;
      Data32Or    = mSaSecurityRegisters[Index].OrMask;

//[-start-130618-IB05400417-add]//
      //
      // If headless system, we will not lock SMM and let console redirection
      // can access AB segment.
      //
      if (RegOffset == 0x88 && PcdGetBool (PcdHeadlessSystem)) {
        continue;
      }
//[-end-130618-IB05400417-add]//

      if (mDevice4Enable == TRUE) {
        if (RegOffset ==  0x50FC){
          Data32Or = 0x87; ///< unlock bit3 if Device (0,4,0) is enabbled.
        }
      }
      MmioAndThenOr32 (BaseAddress + RegOffset, Data32And, Data32Or);
      SaScriptMemReadWrite (
        S3BootScriptWidthUint32,
        (UINTN) (BaseAddress + RegOffset),
        &Data32Or,  ///< Data to be ORed
        &Data32And  ///< Data to be ANDed
        );
    }

    ///
    /// Lock processor/chipset BAR registers
    ///

    if (!(((CpuFamilyId == EnumCpuHsw)    && (CpuSteppingId <= EnumHswB0)   ) ||
          ((CpuFamilyId == EnumCpuHswUlt) && (CpuSteppingId <= EnumHswUltB0)) ||
          ((CpuFamilyId == EnumCpuCrw)    && (CpuSteppingId <= EnumCrwB0)   ) )) {
      AsmMsrOr64 (0x2e7, 1);
    }
}

#ifdef PEG_FLAG
/**
  This function handles SA S3 resume
**/
VOID
SaS3Resume (
  VOID
)
{
  UINT8                           PegComplete;
  UINT8                           EndpointCompleted;
  UINT32                          PegBaseAddress;

  DEBUG ((EFI_D_INFO, "SA S3 resume\n"));
  if (mEnumStatus == EFI_SUCCESS) {
    ///
    /// Restore Bus number assignment first
    ///
    for (PegComplete = 0; PegComplete < MAX_SUPPORTED_ROOT_BRIDGE_NUMBER; PegComplete++) {
      if (mDeviceCapMmBaseSave [PegComplete][0] == 0) {
        continue;
      }
      EndpointCompleted = 0;
      while (EndpointCompleted < mNumberOfDeviceFound [PegComplete]) {
        PegBaseAddress = (mDeviceCapMmBaseSave [PegComplete][EndpointCompleted] & (UINTN) ~0xFF);
        MmioWrite32 (PegBaseAddress + PCI_PBUS, mDeviceBusNumberSave [PegComplete][EndpointCompleted]);
        DEBUG ((EFI_D_INFO, " Restore Bus number [%X] = %08X\n", PegBaseAddress + PCI_PBUS, MmioRead32 (PegBaseAddress + PCI_PBUS)));
        EndpointCompleted ++;
      }
    }
    for (PegComplete = 0; PegComplete < MAX_SUPPORTED_ROOT_BRIDGE_NUMBER; PegComplete++) {
      if (mDeviceCapMmBaseSave [PegComplete][0] == 0) {
        continue;
      }
      PcieAdditionalSettingBeforeASPM (PegComplete);
      EndpointCompleted = 0;
      while (EndpointCompleted < mNumberOfDeviceFound [PegComplete]) {
        PegBaseAddress = (mDeviceCapMmBaseSave [PegComplete][EndpointCompleted] & (UINTN) ~0xFF);
        ///
        /// Restore Max Pay Load and Extended Tag
        ///
        MmioWrite16 (mDeviceCapMmBaseSave [PegComplete][EndpointCompleted] + 0x8, mDeviceControlRegisterSave [PegComplete][EndpointCompleted]);
        DEBUG ((EFI_D_INFO, "[B%X|D%X|F%X|R%X] DCTL=%X\n",
                               (mDeviceCapMmBaseSave [PegComplete][EndpointCompleted] >> 20)  & mMaxBusNumberSupported,
                               (mDeviceCapMmBaseSave [PegComplete][EndpointCompleted] >> 15)  & 0x1F,
                               (mDeviceCapMmBaseSave [PegComplete][EndpointCompleted] >> 12)  & 0x07,
                               (mDeviceCapMmBaseSave [PegComplete][EndpointCompleted] + 0x08) & 0xFFF,
                               mDeviceControlRegisterSave [PegComplete][EndpointCompleted]));
        ///
        /// Restore ASPM and Common Clock
        ///
        MmioWrite8 ((mDeviceCapMmBaseSave [PegComplete][EndpointCompleted] + 0x010), mDeviceAspmSave [PegComplete][EndpointCompleted]);
        DEBUG ((EFI_D_INFO, "[B%X|D%X|F%X|R%X] ASPM/CommonClock=%X\n",
                               (mDeviceCapMmBaseSave [PegComplete][EndpointCompleted] >> 20)  & mMaxBusNumberSupported,
                               (mDeviceCapMmBaseSave [PegComplete][EndpointCompleted] >> 15)  & 0x1F,
                               (mDeviceCapMmBaseSave [PegComplete][EndpointCompleted] >> 12)  & 0x07,
                               (mDeviceCapMmBaseSave [PegComplete][EndpointCompleted] + 0x10) & 0xFFF,
                               mDeviceAspmSave [PegComplete][EndpointCompleted]));
        ///
        /// Restore PEG power optimization.
        ///
        MmioAndThenOr16 (mDeviceCapMmBaseSave [PegComplete][EndpointCompleted] + 0x28, (UINT16)~(BIT10 + BIT13 + BIT14), mDeviceLtrObffSave [PegComplete][EndpointCompleted]);
        DEBUG ((EFI_D_INFO, "[B%X|D%X|F%X|R%X] LTR/OBFF=%X\n",
                               (mDeviceCapMmBaseSave [PegComplete][EndpointCompleted] >> 20)  & mMaxBusNumberSupported,
                               (mDeviceCapMmBaseSave [PegComplete][EndpointCompleted] >> 15)  & 0x1F,
                               (mDeviceCapMmBaseSave [PegComplete][EndpointCompleted] >> 12)  & 0x07,
                               (mDeviceCapMmBaseSave [PegComplete][EndpointCompleted] + 0x28) & 0xFFF,
                               mDeviceLtrObffSave [PegComplete][EndpointCompleted]));

        if (mDeviceExtCapLtrOffsetSave [PegComplete][EndpointCompleted] != 0) {
          MmioAndThenOr16 (PegBaseAddress + mDeviceExtCapLtrOffsetSave [PegComplete][EndpointCompleted] + 0x4, (UINT16) (~0x1FFF), mDeviceMaxSnoopLatencySave [PegComplete][EndpointCompleted]);
          DEBUG ((EFI_D_INFO, "[B%X|D%X|F%X|R%X] Max snoop latency=%X\n",
                                 ((PegBaseAddress + mDeviceExtCapLtrOffsetSave [PegComplete][EndpointCompleted]) >> 20)  & mMaxBusNumberSupported,
                                 ((PegBaseAddress + mDeviceExtCapLtrOffsetSave [PegComplete][EndpointCompleted]) >> 15)  & 0x1F,
                                 ((PegBaseAddress + mDeviceExtCapLtrOffsetSave [PegComplete][EndpointCompleted]) >> 12)  & 0x07,
                                 (PegBaseAddress + mDeviceExtCapLtrOffsetSave [PegComplete][EndpointCompleted] + 0x4) & 0xFFF,
                                 mDeviceMaxSnoopLatencySave [PegComplete][EndpointCompleted]));

          MmioAndThenOr16 ((PegBaseAddress + mDeviceExtCapLtrOffsetSave [PegComplete][EndpointCompleted] + 0x6), (UINT16) (~0x1FFF), mDeviceMaxNoSnoopLatencySave [PegComplete][EndpointCompleted]);
          DEBUG ((EFI_D_INFO, "[B%X|D%X|F%X|R%X] Max No-snoop latency=%X\n",
                                 ((PegBaseAddress + mDeviceExtCapLtrOffsetSave [PegComplete][EndpointCompleted]) >> 20)  & mMaxBusNumberSupported,
                                 ((PegBaseAddress + mDeviceExtCapLtrOffsetSave [PegComplete][EndpointCompleted]) >> 15)  & 0x1F,
                                 ((PegBaseAddress + mDeviceExtCapLtrOffsetSave [PegComplete][EndpointCompleted]) >> 12)  & 0x07,
                                 (PegBaseAddress + mDeviceExtCapLtrOffsetSave [PegComplete][EndpointCompleted] + 0x6) & 0xFFF,
                                 mDeviceMaxNoSnoopLatencySave [PegComplete][EndpointCompleted]));
        }
        if (mDeviceExtCapVcOffsetSave [PegComplete][EndpointCompleted]) {
          MmioAndThenOr8 ((mDeviceExtCapVcOffsetSave [PegComplete][EndpointCompleted] + 0x14), 0, mDeviceTcxVc0MappingSave [PegComplete][EndpointCompleted]);
          DEBUG ((EFI_D_INFO, "[B%X|D%X|F%X|R%X] TCx/VC0 mapping=%X\n",
                                 ((PegBaseAddress + mDeviceExtCapLtrOffsetSave [PegComplete][EndpointCompleted]) >> 20)  & mMaxBusNumberSupported,
                                 ((PegBaseAddress + mDeviceExtCapLtrOffsetSave [PegComplete][EndpointCompleted]) >> 15)  & 0x1F,
                                 ((PegBaseAddress + mDeviceExtCapLtrOffsetSave [PegComplete][EndpointCompleted]) >> 12)  & 0x07,
                                 (PegBaseAddress + mDeviceExtCapLtrOffsetSave [PegComplete][EndpointCompleted] + 0x14) & 0xFFF,
                                 mDeviceTcxVc0MappingSave [PegComplete][EndpointCompleted]));
        }
        EndpointCompleted ++;
      }
      ///
      /// If common clock supported on root port and endpoint, retrain link
      ///
      if (mCommonClock [PegComplete] == 1) {
        DEBUG ((EFI_D_INFO, "Retrain Link for Common Clock\n"));
        ///
        /// Retrain the Link per PCI Express Specification.
        ///
        MmioOr8 (mDeviceCapMmBaseSave [PegComplete][0] + 0x010, BIT5);

        ///
        /// Wait until Re-Training has completed.
        ///
        while ((MmioRead16 (mDeviceCapMmBaseSave [PegComplete][0] + 0x012) & BIT11) != 0) {}
      }
    }
  }
  ///
  /// Re-do this during S3 resume
  ///
  PcieILtrOverride ();
}
#endif  // PEG_FLAG

/**
  Wrapper function for all SA S3 resume tasks which can be a callback function.
**/
VOID
SaS3ResumeCallback (
  VOID
)
{
#ifdef PEG_FLAG
    SaS3Resume ();
#endif  // PEG_FLAG
    SaSaveRestoreChipset (FALSE, mSaChipsetPciRegistersSaveTable, sizeof (mSaChipsetPciRegistersSaveTable) / sizeof (BOOT_SCRIPT_PCI_REGISTER_SAVE), mSaChipsetPciRegistersSave);
    SaSaveRestorePlatform (FALSE);
    SaSecurityLock ();
}

/**
  Wrapper function for all SA ASPM tasks and extended tag which can be a callback function.
**/
VOID
SaPcieConfigAfterOpRom (
  VOID
)
{

#ifdef PEG_FLAG
    DEBUG ((EFI_D_INFO, "SA ASPM\n"));
    if (mEnumStatus == EFI_SUCCESS) {
      SaAspm ();
      ///
      /// Device Control Register on all endpoint devices is saved after the OpRom has run in EnableExtendedTag (DXE phase will still use ExitPmAuth)
      /// This is to prevent duplication of saving this register in different phases.
      ///
      EnableExtendedTag ();
    }
#endif  // PEG_FLAG
}

/**
  Wrapper function for all SA enumeration tasks which can be a callback function.
**/
VOID
SaPcieEnumCallback (
  VOID
)
{
    DEBUG ((EFI_D_INFO, "SA PCIe enumeration\n"));
#ifdef PEG_FLAG
    ///
    /// Expected to execute in ExitPmAuth point (before OROM)
    ///
    mEnumStatus = EnumerateAllPcieDevices ();
    if (mEnumStatus == EFI_SUCCESS) {
      SaPcieConfigBeforeOpRom ();
    }
#endif  // PEG_FLAG
    ///
    /// Save Chipset registers
    ///
    SaSaveRestoreChipset (TRUE, mSaChipsetPciRegistersSaveTable, sizeof (mSaChipsetPciRegistersSaveTable) / sizeof (BOOT_SCRIPT_PCI_REGISTER_SAVE), mSaChipsetPciRegistersSave);
}

/**
  This function will initialize all required platform policy into global veriables so no need to locate policy protocol during runtime.
**/
VOID
SaPcieInitPolicy (
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
)
{
  UINT8  RootPortCount;
  UINT8  Index;

  ///
  /// Initialize module global variables - Stepping ID and Platform Policy
  ///
  for (Index = 0; Index < (sizeof (mSaChipsetPciRegistersSaveTable) / sizeof (BOOT_SCRIPT_PCI_REGISTER_SAVE)); Index++) {
    mSaChipsetPciRegistersSaveTable[Index].Address = MmPciAddress (0, SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, mSaChipsetPciRegistersTable[Index]);
  }

  for (Index = 0; Index < (sizeof (mSaIgfxPciRegistersSaveTable) / sizeof (BOOT_SCRIPT_PCI_REGISTER_SAVE)); Index++) {
    mSaIgfxPciRegistersSaveTable[Index].Address = MmPciAddress (0, SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, mSaIgfxPciRegistersTable[Index]);
  }

  for (Index = 0; Index < (sizeof (mSaSecurityRegisters) / sizeof (BOOT_SCRIPT_REGISTER_SETTING)); Index++) {
    if (mSaSecurityRegisters[Index].BaseAddr != MCH_BASE_ADDRESS) {
      mSaSecurityRegisters[Index].BaseAddr = (UINTN) PcdGet64(PciExpressBaseAddress);
    }
  }

  for (RootPortCount = 0; RootPortCount < MAX_SUPPORTED_ROOT_BRIDGE_NUMBER; RootPortCount++) {
    mPegAspmPerPort[RootPortCount] = DxePlatformSaPolicy->PcieConfig->PegAspm[RootPortCount];
    mPegAspmL0sPerPort[RootPortCount] = DxePlatformSaPolicy->PcieConfig->PegAspmL0s[RootPortCount];
    if (DxePlatformSaPolicy->Revision >= DXE_SA_PLATFORM_POLICY_PROTOCOL_REVISION_7) {
      mPegPwrOpt[RootPortCount] = DxePlatformSaPolicy->PcieConfig->PegPwrOpt[RootPortCount];
    } else {
      //
      // This Platform policy protocol field may not have been initialized.
      // Initialize a default for RC to use
      //
      mPegPwrOpt[RootPortCount].LtrEnable            = 1;
      mPegPwrOpt[RootPortCount].LtrMaxSnoopLatency   = V_SA_LTR_MAX_SNOOP_LATENCY_VALUE;
      mPegPwrOpt[RootPortCount].LtrMaxNoSnoopLatency = V_SA_LTR_MAX_NON_SNOOP_LATENCY_VALUE;
      mPegPwrOpt[RootPortCount].ObffEnable           = 1;
    }
  }

  if (DxePlatformSaPolicy->Revision >= DXE_SA_PLATFORM_POLICY_PROTOCOL_REVISION_2) {
    mPcieAspmDevsOverride = DxePlatformSaPolicy->PcieConfig->PcieAspmDevsOverride;
    mPcieLtrDevsOverride = DxePlatformSaPolicy->PcieConfig->PcieLtrDevsOverride;
  }
  mCridEnable = DxePlatformSaPolicy->MiscConfig->CridEnable;
  mDevice4Enable = DxePlatformSaPolicy->MiscConfig->Device4Enable;
  mSteppingId = GetCpuStepping ();

  mBridgeId = (UINT8) (McD0PciCfg16 (R_SA_MC_DEVICE_ID) & 0xF0);
  mBridgeSteppingId = mBridgeId + mSteppingId;

  ///
  /// Initialize Snoop and Non-Snoop Latencies
  ///
  mSnoopLatencyOvrValue        = 0;
  mNonSnoopLatencyOvrValue     = 0;

}
