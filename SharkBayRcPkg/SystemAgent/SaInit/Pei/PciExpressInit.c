/** @file

  PEI Function to initialize SA PciExpress.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 1999 - 2014 Intel Corporation. All rights reserved
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
#include "PcieTraining.h"
#include <Ppi/Stall.h>
#include <Library/PchPlatformLib.h>

#ifdef PEG_FLAG

#define SA_PEG_VMARGIN_UP   0
#define SA_PEG_VMARGIN_DOWN 1

#define SA_PEG_LINK_DISABLE_MAXWAIT 100*10

VOID
ReportPcieLinkStatus (
  IN UINT8             PegBus,
  IN UINT8             PegDev,
  IN UINT8             PegFunc
)
/**
 This function reports a PEG controller's link status

  @param[in] PegBus       - Peg Bus
  @param[in] PegDev       - Peg Device
  @param[in] PegFunc      - Peg Function

  @retval None
**/
{
  UINT32 Deven;
  UINT16 LinkStatus;
  UINT8  LinkWidth;
  UINT8  LinkSpeed;
  UINT16 Vc0Pending;

  Deven  = MmPci32 (0, SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_DEVEN);
  Deven = (Deven >> 1) & 0x7;
  DEBUG ((EFI_D_INFO, "PEG%x%x (%x:%x:%x) - ", PegDev, PegFunc, PegBus, PegDev, PegFunc));
  if (((Deven >> (SA_PEG_MAX_FUN - 1 - PegFunc)) & 0x1) == 0x1) {
    LinkStatus = MmPci16 (0, PegBus, PegDev, PegFunc, R_SA_PEG_LSTS_OFFSET);
    LinkWidth  = (LinkStatus >> 4) & 0x3F;
    LinkSpeed  = LinkStatus & 0xF;
    Vc0Pending = ((MmPci16 (0, PegBus, PegDev, PegFunc, R_SA_PEG_VC0RSTS_OFFSET)) >> 1) & 0x1;
    DEBUG ((EFI_D_INFO, "Trained to x%d at Gen%d.", LinkWidth, LinkSpeed));
    DEBUG ((EFI_D_INFO, " VC0 Negotiation Pending = %d.", Vc0Pending));
    DEBUG ((EFI_D_INFO, "\n"));
  } else {
    DEBUG ((EFI_D_INFO, "Disabled.\n"));
  }
}

VOID
WaitForVc0Negotiation (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_STALL_PPI           *StallPpi,
  IN UINT8             PegBus,
  IN UINT8             PegDev,
  IN UINT8             PegFunc
)
/**
  This function prints the time required for VC0 Negotiation Pending to be cleared. Quits after 100 msec.

  @param[in] PeiServices  - Pointer to the PEI services table
  @param[in] StallPpi     - Pointer to PEI_STALL_PPI
  @param[in] PegBus       - Peg Bus
  @param[in] PegDev       - Peg Device
  @param[in] PegFunc      - Peg Function

  @retval None
**/
{
  UINT32 Deven;
  UINT32 MsecWait;
  UINT16 Vc0Pending;

  Deven  = MmPci32 (0, SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_DEVEN);
  Deven = (Deven >> 1) & 0x7;
  if (((Deven >> (SA_PEG_MAX_FUN - 1 - PegFunc)) & 0x1) == 0x1) {
    MsecWait = 0;
    Vc0Pending = ((MmPci16 (0, PegBus, PegDev, PegFunc, R_SA_PEG_VC0RSTS_OFFSET)) >> 1) & 0x1;

    while (Vc0Pending && (MsecWait < 100)) {
      MsecWait++;
      StallPpi->Stall (PeiServices, StallPpi, STALL_ONE_MILLI_SECOND);
      Vc0Pending = ((MmPci16 (0, PegBus, PegDev, PegFunc, R_SA_PEG_VC0RSTS_OFFSET)) >> 1) & 0x1;
    }
    DEBUG ((EFI_D_INFO, "PEG%x%x (%x:%x:%x) - VC0 Negotiation Pending = %x after %d msec.\n",
      PegDev, PegFunc, PegBus, PegDev, PegFunc, Vc0Pending, MsecWait));
  }
}

VOID
GracefulLinkStatusStall (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_STALL_PPI           *StallPpi,
  IN SA_PLATFORM_POLICY_PPI             *SaPlatformPolicyPpi,
  IN SA_DATA_HOB                        *SaDataHob,
  IN UINT8                              HwStrap
  )
/**
  If we have a record of links that did not train from last boot, then do not stall for them
  For links that did train last boot, stall until they train or 100ms pass

  @param[in] PeiServices         - Pointer to the PEI services table
  @param[in] StallPpi            - Pointer to PEI_STALL_PPI
  @param[in] SaPlatformPolicyPpi - Pointer to SA_PLATFORM_POLICY_PPI
  @param[in] SaDataHob           - Pointer to SA_DATA_HOB
  @param[in] HwStrap             - HwStrap configuration from FUSESCMN

  @retval None
**/
{
  UINT8          LinkStatusGood;
  UINT8          PegLinkFailMask;
  UINT8          i;
  UINT8          PegDev;
  UINT8          PegFunc;
  UINT8          CurrentPegFuncBit;
  UINT8          SkipFuncMask;
  EFI_STATUS     Status;
  EFI_BOOT_MODE  BootMode;

  PegLinkFailMask = 0;
  ///
  /// If this is S3 Resume or Warm Boot, check if PEG delay can be skipped when no PEG devices populated.
  ///
  Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
  ASSERT_EFI_ERROR (Status);
  if ((BootMode == BOOT_ON_S3_RESUME) || (GetPchPmStatus (WarmBoot) == TRUE)) {
    if ((SaDataHob != NULL) && (SaDataHob->PegDataValid)) {
      if (SaDataHob->PegData.PegLinkFailMask != 0) {
        PegLinkFailMask = SaDataHob->PegData.PegLinkFailMask;
        DEBUG ((EFI_D_INFO, "Previous Link Training Fail Mask 0x%2.2X\n", SaDataHob->PegData.PegLinkFailMask));
      } else {
        PegLinkFailMask = 0xFF;
        for (i = 0; i < SA_PEG_MAX_FUN; i++) {
          PegLinkFailMask &= (UINT8) ~(0x1 << i);
        }
      }
    }
  }
  DEBUG ((EFI_D_INFO, "New Link Training Fail Mask 0x%2.2X\n", PegLinkFailMask));

  switch (HwStrap) {
    case SA_PEG_x16_x0_x0:
      SkipFuncMask = BIT2 | BIT1;
      break;

    case SA_PEG_x8_x8_x0:
      SkipFuncMask = BIT2;
      break;

    default:
    case SA_PEG_x8_x4_x4:
      SkipFuncMask = 0;
      break;
  }

  LinkStatusGood = 0;
  PegDev = 1;
//[-start-110930-IB05300224-modify]//  
  for (i = 0; i < SaPlatformPolicyPpi->PcieConfig->PcieResetDelay * 10; i++) {
//[-end-110930-IB05300224-modify]//    
    ///
    /// Poll endpoints
    /// Skip endpoints that failed last boot
    /// Skip endpoints that trained good this boot
    /// Loop in 1-ms increments until all endpoints are:
    ///  reported bad from last boot, or
    ///  have trained good on this boot
    ///
    for (PegFunc = 0, CurrentPegFuncBit = 1; PegFunc < SA_PEG_MAX_FUN; PegFunc++, CurrentPegFuncBit <<= 1) {
      if (CurrentPegFuncBit == (CurrentPegFuncBit & SkipFuncMask)) {
        if (i == 0) {
          DEBUG ((EFI_D_INFO, " PEG%x%x (%x:%x:%x) - skipping due to furcation\n", PegDev, PegFunc, SA_PEG_BUS_NUM, PegDev, PegFunc));
        }
        continue;
      }
      ///
      /// If configuration change, then break out of loop.
      /// Test if current PEG is neither in fail mask nor previously tested good.
      ///
      if (CurrentPegFuncBit != (CurrentPegFuncBit & (UINT8) (PegLinkFailMask | LinkStatusGood))) {
        DEBUG ((EFI_D_INFO, " PEG%x%x (%x:%x:%x) - checking\n", PegDev, PegFunc, SA_PEG_BUS_NUM, PegDev, PegFunc));
        ///
        /// VC negotiation is complete
        ///
        if ((BIT1 & MmPci16(0, SA_PEG_BUS_NUM, PegDev, PegFunc, R_SA_PEG_VC0RSTS_OFFSET)) != BIT1) {
          DEBUG ((EFI_D_INFO, "  VC negotiation is complete\n"));
          ///
          /// Record as a good link
          ///
          LinkStatusGood |= CurrentPegFuncBit;
        }
      }
    }
    ///
    /// If all links accounted for, then exit.
    ///
    if (((UINT8) (LinkStatusGood | PegLinkFailMask | SkipFuncMask)) == 0xFF) {
      break;
    }
    StallPpi->Stall (PeiServices, StallPpi, STALL_ONE_MILLI_SECOND);
  } ///< End of stall loop

  DEBUG ((EFI_D_INFO, "Total Stall: %d msec\n", i));
  if (SaDataHob != NULL) {
    SaDataHob->PegData.PegLinkFailMask = (UINT8) ~LinkStatusGood;
    DEBUG ((EFI_D_INFO, "Returned PegLinkFailMask 0x%2.2X\n", SaDataHob->PegData.PegLinkFailMask));
  }

}

UINT32
PcieFindCapId (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   PegFunc,
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
  @param[in] PegFunc   -   Pci Function Number
  @param[in] CapId     -   CAPID to search for

  @retval 0       - CAPID not found
  @retval Other   - CAPID found, Offset of desired CAPID
**/
{
  UINT8 CapHeader;

  ///
  /// Always start at Offset 0x34
  ///
  CapHeader = MmPci8 (0, Bus, Device, PegFunc, PCI_CAPBILITY_POINTER_OFFSET);
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
    if (MmPci8 (0, Bus, Device, PegFunc, CapHeader) == CapId) {
      return CapHeader;
    }

    CapHeader = MmPci8 (0, Bus, Device, PegFunc, CapHeader + 1);
  }

  return 0;
}


VOID
SetLoadBus (
  IN UINT32 DmiBar,
  IN UINTN  Dev,
  IN UINTN  Lane,
  IN UINT32 LoadSel,
  IN UINT32 LoadData,
  IN UINT8  CpuSteppingId
  )
/**
  Set Load Bus

  @param[in] DmiBar        - DMIBAR address
  @param[in] Dev           - Device Number
  @param[in] Lane          - Number of Lane
  @param[in] LoadSel       - Load selection value
  @param[in] LoadData      - Load Data
  @param[in] CpuSteppingId - CPUID.1.EAX[3:0], CPU stepping ID

  @retval None
**/
{
  UINT32  lbcvalue;
  UINT32  lbcdata;
  UINT32  lbclnsel;
  UINT32  lbcldsel;
  UINT32  lbcaddr;

  lbcaddr   = R_SA_PEG_LOADBUSCTL0_OFFSET + BUNDLE_STEP * (Lane >> 1);
  lbcvalue  = 0x70000000;

  lbclnsel  = ((Lane & 1) == 0) ? 0x80000 : 0x100000;
  lbcldsel  = (LoadSel & 0x3f) << 21;
  lbcdata   = (LoadData << 1) & 0x7FFF;

  lbcvalue  = lbcvalue | lbclnsel | lbcldsel | lbcdata;

  if (Dev == 0) {
    Mmio32 (DmiBar, lbcaddr) = lbcvalue;
  } else {
    MmPci32 (0, SA_PEG_BUS_NUM, Dev, SA_PEG10_FUN_NUM, lbcaddr) = lbcvalue;
  }

  return;
}

UINT32
GetMonBus (
  IN UINT32 DmiBar,
  IN UINTN  Dev,
  IN UINTN  Lane,
  IN UINT32 LoadSel,
  IN UINT8  CpuSteppingId
  )
/**
  Get monitor bus from the lane selected

  @param[in] DmiBar        - DMIBAR address
  @param[in] Dev           - Device number
  @param[in] Lane          - Number of Lane
  @param[in] LoadSel       - Load selection value
  @param[in] LoadData      - Load selecttion data
  @param[in] CpuSteppingId - CPUID.1.EAX[3:0], CPU stepping ID

  @retval UINT32 - Load bus address
**/
{
  UINT32  monvalue;
  UINT32  prevalue;
  UINT32  lbclnsel;
  UINT32  lbcldsel;
  UINT32  lbcaddr;
  UINT32  Result;

  lbcaddr   = R_SA_PEG_LOADBUSCTL0_OFFSET + BUNDLE_STEP * (Lane >> 1);
  monvalue  = 0x068008000;

  if (Dev == 0) {
    prevalue = Mmio32 (DmiBar, lbcaddr);
  } else {
    prevalue = MmPci32 (0, SA_PEG_BUS_NUM, Dev, SA_PEG10_FUN_NUM, lbcaddr);
  }
  monvalue  = monvalue | (prevalue & 0x7FFE);

  lbclnsel  = ((Lane & 1) == 0) ? 0x80000 : 0x100000;
  lbcldsel  = (LoadSel & 0x3f) << 21;
  monvalue  = monvalue | lbclnsel | lbcldsel;

  if (Dev == 0) {
    Mmio32 (DmiBar, lbcaddr)  = monvalue;
    Result                    = Mmio32 (DmiBar, lbcaddr);
  } else {
    MmPci32 (0, SA_PEG_BUS_NUM, Dev, SA_PEG10_FUN_NUM, lbcaddr) = monvalue;
    Result = MmPci32 (0, SA_PEG_BUS_NUM, Dev, SA_PEG10_FUN_NUM, lbcaddr);
  }

  return (Result >> 1) & 0x3FFF;
}

VOID
ProgramPreset (
  IN UINT8  Direction,
  IN UINT8  PresetValue,
  IN UINT8  PegFunc,
  IN UINT8  Lane
  )
/**
  Program PEG Gen3 preset value

  @param[in] Direction     - 0 = Root Port, 1 = End Point
  @param[in] PresetValue   - Preset value to program
  @param[in] PegFunc       - Peg function number to be configured
  @param[in] Lane          - Lane to be configured

  @retval None
**/
{
  UINT32  Data32Or;
  UINT32  Data32And;
  UINT8   OriginalLane;

  OriginalLane = Lane;
  switch (PegFunc) {
    case 1:
      if (Lane < 8) {
        DEBUG ((EFI_D_WARN, "Invalid input to ProgramPreset() function!  PegFunc=%d, Lane=%d\n", PegFunc, Lane));
        return;
      } else {
        Lane -= 8;
      }
      break;
    case 2:
      if (Lane < 12) {
        DEBUG ((EFI_D_WARN, "Invalid input to ProgramPreset() function!  PegFunc=%d, Lane=%d\n", PegFunc, Lane));
        return;
      } else {
        Lane -= 12;
      }
      break;
    default:
      break;
  }
  ///
  /// RP preset goes to bits [3:0]  for even lane and [19:16] for odd lane
  /// EP preset goes to bits [11:8] for even lane and [27:24] for odd lane
  ///
  if (Direction != 0) {
    if ((Lane % 2) == 0) {
      Data32And = 0xFFFFF0FF;
      Data32Or  = PresetValue << 8;
    } else {
      Data32And = 0xF0FFFFFF;
      Data32Or  = PresetValue << 24;
    }
  } else {
    if (OriginalLane >= 8) {
      if ((Lane % 2) == 0) {
        Data32And = 0xFFF0FFFF;
        Data32Or  = PresetValue << 16;
      } else {
            Data32And = 0xFFFFFFF0;
            Data32Or  = PresetValue;
      }
    } else {
      if ((Lane % 2) == 0) {
        Data32And = 0xFFFFFFF0;
        Data32Or  = PresetValue;
      } else {
        Data32And = 0xFFF0FFFF;
        Data32Or  = PresetValue << 16;
      }
    }
  }

  MmPci32AndThenOr (0, SA_PEG_BUS_NUM, SA_PEG_DEV_NUM, PegFunc, R_SA_PEG_EQCTL0_1_OFFSET + (Lane / 2) * 4, Data32And, Data32Or);

  return;
}

VOID
ProgramPresetPerLane (
  IN UINT8  *RootPortPreset,
  IN UINT8  *EndPointPreset,
  IN UINT8  *EndPointHint,
  IN UINTN  HwStrap
  )
/**
  Program PEG Gen3 preset values per lane

  @param[in] RootPortPreset  - Array of root port preset values to program
  @param[in] EndPointPreset  - Array of end point preset values to program
  @param[in] EndPointHint    - Array of end point hint value to program
  @param[in] HwStrap         - HwStrap configuration from FUSESCMN

  @retval None
**/
{
  UINTN i;

  for (i = 0; i < SA_PEG_MAX_LANE; i++) {
    if (RootPortPreset[i] > 9) {
      RootPortPreset[i] = 8;
    }
    if (EndPointPreset[i] > 9) {
      EndPointPreset[i] = 7;
    }
    if (EndPointHint[i] > 6) {
      EndPointHint[i] = 2;
    }
  }

  ///
  /// RP preset goes to bits [3:0]   and [19:16]
  /// EP preset goes to bits [11:8]  and [27:24]
  /// EP hint   goes to bits [14:12] and [30:28]
  ///
  switch (HwStrap) {
    case SA_PEG_x16_x0_x0:
      ///
      /// PEG10 x16
      ///

      for (i = 0; i < 4; ++i) {
        McD1PciCfg32AndThenOr (R_SA_PEG_EQCTL0_1_OFFSET + i * 4, 0x80F080F0,
          RootPortPreset[2 * i] | (EndPointPreset[2 * i] << 8) | (EndPointHint[2 * i] << 12) | (RootPortPreset[2 * i + 1] << 16) | (EndPointPreset[2 * i + 1] << 24) | (EndPointHint[2 * i + 1] << 28));
      }
      for (i = 4; i < 8; ++i) {
        McD1PciCfg32AndThenOr (R_SA_PEG_EQCTL0_1_OFFSET + i * 4, 0x80F080F0,
          RootPortPreset[2 * i + 1] | (EndPointPreset[2 * i] << 8) | (EndPointHint[2 * i] << 12) | (RootPortPreset[2 * i] << 16) | (EndPointPreset[2 * i + 1] << 24) | (EndPointHint[2 * i + 1] << 28));
      }
      break;

    case SA_PEG_x8_x8_x0:
      ///
      /// PEG10 x8 / PEG11 x8
      ///
      for (i = 0; i < 4; ++i) {
        McD1PciCfg32AndThenOr (R_SA_PEG_EQCTL0_1_OFFSET + i * 4, 0x80F080F0,
          RootPortPreset[2 * i] | (EndPointPreset[2 * i] << 8) | (EndPointHint[2 * i] << 12) | (RootPortPreset[2 * i + 1] << 16) | (EndPointPreset[2 * i + 1] << 24) | (EndPointHint[2 * i + 1] << 28));
        McD1F1PciCfg32AndThenOr (R_SA_PEG_EQCTL0_1_OFFSET + i * 4, 0x80F080F0,
          RootPortPreset[8 + 2 * i + 1] | (EndPointPreset[8 + 2 * i] << 8) | (EndPointHint[8 + 2 * i] << 12) | (RootPortPreset[8 + 2 * i] << 16) | (EndPointPreset[8 + 2 * i + 1] << 24) | (EndPointHint[8 + 2 * i + 1] << 28));
      }
      break;

    case SA_PEG_x8_x4_x4:
      ///
      /// PEG10 x8 / PEG11 x4 / PEG12 x4
      ///
      for (i = 0; i < 4; ++i) {
        McD1PciCfg32AndThenOr (R_SA_PEG_EQCTL0_1_OFFSET + i * 4, 0x80F080F0,
          RootPortPreset[2 * i] | (EndPointPreset[2 * i] << 8) | (EndPointHint[2 * i] << 12) | (RootPortPreset[2 * i + 1] << 16) | (EndPointPreset[2 * i + 1] << 24) | (EndPointHint[2 * i + 1] << 28));
      }
      for (i = 0; i < 2; ++i) {
        McD1F1PciCfg32AndThenOr (R_SA_PEG_EQCTL0_1_OFFSET + i * 4, 0x80F080F0,
          RootPortPreset[8 + 2 * i + 1] | (EndPointPreset[8 + 2 * i] << 8) | (EndPointHint[8 + 2 * i] << 12) | (RootPortPreset[8 + 2 * i] << 16) | (EndPointPreset[8 + 2 * i + 1] << 24) | (EndPointHint[8 + 2 * i + 1] << 28));
        McD1F2PciCfg32AndThenOr (R_SA_PEG_EQCTL0_1_OFFSET + i * 4, 0x80F080F0,
          RootPortPreset[12 + 2 * i + 1] | (EndPointPreset[12 + 2 * i] << 8) | (EndPointHint[12 + 2 * i] << 12) | (RootPortPreset[12 + 2 * i] << 16) | (EndPointPreset[12 + 2 * i + 1] << 24) | (EndPointHint[12 + 2 * i + 1] << 28));
      }
      break;

    default:
      break;
  }

  return;
}

VOID
PegGen3Equalization (
  IN SA_PLATFORM_POLICY_PPI  *SaPlatformPolicyPpi,
  IN UINT8                   CpuSteppingId,
  IN UINTN                   HwStrap
  )
/**
  Perform PEG Gen3 Equalization steps

  @param[in] SaPlatformPolicyPpi - pointer to SA_PLATFORM_POLICY_PPI
  @param[in] CpuSteppingId       - CPUID.1.EAX[3:0], CPU stepping ID
  @param[in] HwStrap             - HwStrap configuration from FUSESCMN

  @retval None
**/
{
  UINT8               *RootPortPreset;
  UINT8               *EndPointPreset;
  UINT8               *EndPointHint;

  ///
  /// Apply static presets for root port and endpoint
  ///
  RootPortPreset = SaPlatformPolicyPpi->PcieConfig->Gen3RootPortPreset;
  EndPointPreset = SaPlatformPolicyPpi->PcieConfig->Gen3EndPointPreset;
  EndPointHint   = SaPlatformPolicyPpi->PcieConfig->Gen3EndPointHint;

  ProgramPresetPerLane (RootPortPreset, EndPointPreset, EndPointHint, HwStrap);

  return;
}

#endif // PEG_FLAG

#if defined(DMI_FLAG) || defined(PEG_FLAG)
VOID
PegDmiRecipe (
  IN SA_PLATFORM_POLICY_PPI *SaPlatformPolicyPpi,
  IN UINT32                 MchBar,
  IN UINT32                 DmiBar,
  IN UINTN                  Dev,
  IN UINT8                  SwingControl
  )
/**
  Perform PEG/DMI PCIe Recipe steps

  @param[in] SaPlatformPolicyPpi - Pointer to SA_PLATFORM_POLICY_PPI
  @param[in] MchBar              - MCHBAR or zero if called for PEG
  @param[in] DmiBar              - DMIBAR or zero if called for PEG
  @param[in] Dev                 - PEG device number: 1 for PEG10, 0 if called for DMI.
  @param[in] SwingControl        - 1 = Half, 2 = Full

  @retval None
**/
{
  UINTN             i;
  UINTN             BundlesCount;
  UINTN             LanesCount;
  UINT32            Data32;
  UINT32            Data32And;
  UINT32            Data32Or;
  UINT32            VcuAddress;
  UINT8             VcuReadOp;
  UINT8             VcuWriteOp;
  CPU_FAMILY        CpuFamilyId;
  CPU_STEPPING      CpuSteppingId;
  BOOLEAN           IsSingleCall;

  CpuFamilyId   = GetCpuFamily();
  CpuSteppingId = GetCpuStepping();

  ///
  /// With both DMI_FLAG and PEG_FLAG defined, this function is called twice: once for DMI and once for PEG.
  /// Some registers are shared between DMI and PEG, so they are only programmed during one of the calls.
  /// If only one flag is defined, this function will only be called once, so the shared registers need to be
  /// programmed during the first/only call.
  ///
  IsSingleCall = TRUE;
#if defined(DMI_FLAG) && defined(PEG_FLAG)
  IsSingleCall = FALSE;
#endif

  if (Dev == SA_PEG_DEV_NUM) {
    ///
    /// PEG10
    ///
    LanesCount = SA_PEG_MAX_LANE;
  } else {
    ///
    /// DMI
    ///
    LanesCount = SA_DMI_MAX_LANE;
  }

  BundlesCount = LanesCount >> 1;

  ///
  /// g23rxvref = 0xC (DMI & PEG)
  ///
  Data32And = (UINT32) ~(BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
  Data32Or  = 0xC;
  for (i = 0; i < LanesCount; i++) {
    SaMmio32AndThenOr (DmiBar, Dev, R_SA_PEG_AFELN0CFG0_OFFSET + i * LANE_STEP, Data32And, Data32Or);
  }
  ///
  /// CDRPDDATMODE = 0x1 (DMI & PEG)
  ///
  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
    for (i = 0; i < LanesCount; i++) {
      SaMmio32AndThenOr (DmiBar, Dev, R_SA_PEG_AFELN0CFG1_OFFSET + i * LANE_STEP, 0xFFFFFFFF, BIT11);
    }
  }
  ///
  /// irefctl = 0x2 (PEG only)
  ///
  if (DmiBar == 0) {
    ///
    /// Switch on
    /// SwingControl 1 == Half
    ///              2 == Full (Default)
    ///
    switch (SwingControl) {
      case SA_SWING_HALF:
        Data32Or = 0x0C;
        break;

      default:
      case SA_SWING_FULL:
        Data32Or = 0x02;
        break;
    }

    for (i = 0; i < BundlesCount; i++) {
      SaMmio32AndThenOr (DmiBar, Dev, R_SA_PEG_AFEBND0CFG1_OFFSET + i * BUNDLE_STEP, (UINT32) ~(0x1F), Data32Or);
    }
  }
  ///
  /// txvrefsel = 0x3 (PEG only)
  ///
  if (DmiBar == 0) {
    ///
    /// Switch on
    /// SwingControl 1 == Half
    ///              2 == Full (Default)
    ///
    switch (SwingControl) {
      case SA_SWING_HALF:
        Data32Or = 0x0E;
        break;

      default:
      case SA_SWING_FULL:
        Data32Or = 0x03;
        break;
    }

    for (i = 0; i < BundlesCount; i++) {
      SaMmio32AndThenOr (DmiBar, Dev, R_SA_PEG_AFEBND0CFG1_OFFSET + i * BUNDLE_STEP, (UINT32) ~(0x1F << 5), Data32Or << 5);
    }
  }
  ///
  /// igacq = 0x0 (HSW A0: DMI & PEG)
  ///
  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
    for (i = 0; i < BundlesCount; i++) {
      SaMmio32AndThenOr (DmiBar, Dev, R_SA_PEG_AFEBND0CFG1_OFFSET + i * BUNDLE_STEP, (UINT32) ~(7 << 22), 0);
    }
  }
  ///
  /// dfegainacq = 0x1 (HSW A0: DMI & PEG)
  ///
  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
    for (i = 0; i < BundlesCount; i++) {
      SaMmio32AndThenOr (DmiBar, Dev, R_SA_PEG_AFEBND0CFG1_OFFSET + i * BUNDLE_STEP, (UINT32) ~(3 << 29), 1 << 29);
    }
  }
  ///
  /// PGTRK = 0x9 (DMI & PEG)
  ///
  Data32And = (UINT32) ~(BIT10 | BIT9 | BIT8 | BIT7 | BIT6 | BIT5);
  Data32Or  = 0x9 << 5;
  for (i = 0; i < BundlesCount; i++) {
    SaMmio32AndThenOr (DmiBar, Dev, R_SA_PEG_AFEBND0CFG3_OFFSET + i * BUNDLE_STEP, Data32And, Data32Or);
  }
  ///
  /// igtrk = 0x0 (HSW A0: DMI & PEG)
  ///
  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
    for (i = 0; i < BundlesCount; i++) {
      SaMmio32AndThenOr (DmiBar, Dev, R_SA_PEG_AFEBND0CFG3_OFFSET + i * BUNDLE_STEP, (UINT32) ~(7 << 17), 0);
    }
  }
  ///
  /// RXRTBIN = 0x5 (PEG only)
  ///
  if (DmiBar == 0) {
    Data32And = (UINT32) ~(BIT24 | BIT23 | BIT22 | BIT21);
    Data32Or  = 0x5 << 21;
    for (i = 0; i < BundlesCount; i++) {
      SaMmio32AndThenOr (DmiBar, Dev, R_SA_PEG_AFEBND0CFG3_OFFSET + i * BUNDLE_STEP, Data32And, Data32Or);
    }
  }
  ///
  /// G3RXCTLEPEAK = 0x8 (PEG only)
  ///
  if (DmiBar == 0) {
    Data32And = (UINT32) ~(BIT9 | BIT8 | BIT7 | BIT6);
    for (i = 0; i < BundlesCount; i++) {
      if (SaPlatformPolicyPpi->Revision >= SA_PLATFORM_POLICY_PPI_REVISION_3) {
        Data32Or = SaPlatformPolicyPpi->PcieConfig->Gen3RxCtleP[i] << 6;
      } else {
        Data32Or  = 8 << 6;
      }
      SaMmio32AndThenOr (DmiBar, Dev, R_SA_PEG_AFEBND0CFG4_OFFSET + i * BUNDLE_STEP, Data32And, Data32Or);
    }
  }
  ///
  /// g2rxctlepeak = 0x0 (DMI & PEG)
  ///
  for (i = 0; i < BundlesCount; i++) {
    SaMmio32AndThenOr (DmiBar, Dev, R_SA_PEG_AFEBND0CFG4_OFFSET + i * BUNDLE_STEP, (UINT32) ~(0xF << 10), 0);
  }
  ///
  /// AFEBNDSPARE[uneqmm] = 0x4 (HSW == A0: DMI & PEG)
  ///
  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
    for (i = 0; i < BundlesCount; i++) {
      SaMmio32AndThenOr (DmiBar, Dev, R_SA_PEG_AFEBND0CFG4_OFFSET + i * BUNDLE_STEP, (UINT32) ~(7 << 29), 4 << 29);
    }
  }
  ///
  /// AGCACQLEN = 0x2 (HSW == A0: DMI & PEG)
  ///
  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
    for (i = 0; i < BundlesCount; i++) {
      SaMmio32AndThenOr (DmiBar, Dev, R_SA_PEG_AFEBND0CFG5_OFFSET + i * BUNDLE_STEP, (UINT32) ~(3 << 7), 2 << 7);
    }
  }
  ///
  /// G2DFEC1CTL = 0x0 (HSW == A0: DMI & PEG)
  ///
  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
    for (i = 0; i < BundlesCount; i++) {
      SaMmio32AndThenOr (DmiBar, Dev, R_SA_PEG_AFEBND0CFG5_OFFSET + i * BUNDLE_STEP, (UINT32) ~(3 << 12), 0);
    }
  }
  ///
  /// RXSQEXCTL = 0x0 (DMI & PEG)
  ///
  Data32And = (UINT32) ~(BIT20 | BIT19 | BIT18);
  Data32Or  = 0;
  for (i = 0; i < BundlesCount; i++) {
    SaMmio32AndThenOr (DmiBar, Dev, R_SA_PEG_AFEBND0CFG5_OFFSET + i * BUNDLE_STEP, Data32And, Data32Or);
  }
  ///
  /// dfemfc = 0x3 (HSW == A0: DMI & PEG)
  ///
  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
    for (i = 0; i < BundlesCount; i++) {
      SaMmio32AndThenOr (DmiBar, Dev, R_SA_PEG_AFEBND0CFG5_OFFSET + i * BUNDLE_STEP, (UINT32) ~(3 << 21), 3 << 21);
    }
  }

  ///
  /// ICOMPGAIN = 0x6 (HSW == A0: DMI & PEG)
  ///
  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
    VcuAddress = R_SA_VCU_AFECMNCFG0_ADDRESS_REV1;
    VcuReadOp  = V_SA_VCU_OPCODE_READ_MMIO_REV1;
    VcuWriteOp = V_SA_VCU_OPCODE_WRITE_MMIO_REV1;
  } else {
    VcuAddress = R_SA_VCU_AFECMNCFG0_ADDRESS_REV2;
    VcuReadOp  = V_SA_VCU_OPCODE_READ_MMIO_REV2;
    VcuWriteOp = V_SA_VCU_OPCODE_WRITE_MMIO_REV2;
  }

  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
    if ((DmiBar != 0) || (IsSingleCall)) {
      Data32  = SendVcuApiSequence (MchBar, VcuAddress, VcuReadOp, 0);
      Data32 &= (UINT32) ~(BIT12 | BIT11 | BIT10 | BIT9);
      Data32 |= 0x6 << 9;
      SendVcuApiSequence (MchBar, VcuAddress, VcuWriteOp, Data32);
    }
  }

  ///
  /// PEGVCMSEL = 0x3 (PEG only)
  ///
  if (DmiBar == 0) {
    Data32  = SendVcuApiSequence (MchBar, VcuAddress, VcuReadOp, 0);
    Data32 &= (UINT32) ~(BIT29 | BIT28 | BIT27 | BIT26 | BIT25);
    ///
    /// Switch on
    /// SwingControl 1 == Half
    ///              2 == Full (Default)
    ///
    switch (SwingControl) {
      case SA_SWING_HALF:
        Data32 |= (UINT32) (0x0E << 25);
        break;

      default:
      case SA_SWING_FULL:
        Data32 |= (UINT32) (0x03 << 25);
        break;
    }
    SendVcuApiSequence (MchBar, VcuAddress, VcuWriteOp, Data32);
  }

  ///
  /// PH3EQFFEKNOBS = 0x8 (DMI & PEG)
  ///
  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
    VcuAddress = R_SA_VCU_AFECMNCFG2_ADDRESS_REV1;
  } else {
    VcuAddress = R_SA_VCU_AFECMNCFG2_ADDRESS_REV2;
  }

  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
    if ((DmiBar != 0) || (IsSingleCall)) {
      Data32  = SendVcuApiSequence (MchBar, VcuAddress, VcuReadOp, 0);
      Data32 &= (UINT32) ~(BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25);
      Data32 |= 0x8 << 25;
      SendVcuApiSequence (MchBar, VcuAddress, VcuWriteOp, Data32);
    }
  }

  ///
  /// FIXUNCORRDATA = 0x0 (DMI & PEG)
  ///
  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
    VcuAddress = R_SA_VCU_AFECMNCFG3_ADDRESS_REV1;
  } else {
    VcuAddress = R_SA_VCU_AFECMNCFG3_ADDRESS_REV2;
  }

  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
    if ((DmiBar != 0) || (IsSingleCall)) {
      Data32  = SendVcuApiSequence (MchBar, VcuAddress, VcuReadOp, 0);
      Data32 &= (UINT32) ~(BIT20);
      Data32 |= 0;
      SendVcuApiSequence (MchBar, VcuAddress, VcuWriteOp, Data32);
    }
  }
  ///
  /// BLEGCTL = 0x0 (DMI & PEG)
  ///
  if ((DmiBar != 0) || (IsSingleCall)) {
    Data32  = SendVcuApiSequence (MchBar, VcuAddress, VcuReadOp, 0);
    Data32 &= (UINT32) ~(BIT28 | BIT27 | BIT26 | BIT25 | BIT24 | BIT23);
    Data32 |= 0;
    SendVcuApiSequence (MchBar, VcuAddress, VcuWriteOp, Data32);
  }
  if (((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) &&
      ((DmiBar      != 0)          || (IsSingleCall)              )) {
    VcuAddress = R_SA_VCU_AFECMNCFG7_ADDRESS_REV1;
    VcuReadOp  = V_SA_VCU_OPCODE_READ_CSR_REV1;
    VcuWriteOp = V_SA_VCU_OPCODE_WRITE_CSR_REV1;

    Data32 = SendVcuApiSequence (MchBar, VcuAddress, VcuReadOp, 0);
    ///
    /// VREFRXDET = 0x19 (PEG only)
    ///
    if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
      Data32 &= (UINT32) ~(BIT18 | BIT17 | BIT16 | BIT15 | BIT14);
      Data32 |= 0x19 << 14;
    }
    ///
    /// DFEFIX = 0x4 (PEG only)
    ///
    if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
      Data32 &= (UINT32) ~(BIT30 | BIT29 | BIT28);
      Data32 |= 0x4 << 28;
    }
    SendVcuApiSequence (MchBar, VcuAddress, VcuWriteOp, Data32);
  }


  ///
  /// RXDETECT_SAMPLE_TIME = 0x4 (PEG only)
  ///
  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
    if (DmiBar == 0) {
      Data32And = (UINT32) ~(BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 | BIT7);
      Data32Or  = 0x4 << 7;
      SaMmio32AndThenOr (DmiBar, Dev, R_SA_PEG_AFE_PWRON_OFFSET, Data32And, Data32Or);
    }
  }

  ///
  /// RXL0S_ENTRY_EXIT_TIMER = 0x00 (HSW == A0: DMI & PEG)
  ///                        = 0x13 (HSW >= B0: DMI & PEG)
  ///
  Data32And = (UINT32) ~(BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
    Data32Or = 0x00;
  } else {
    Data32Or = 0x13;
  }
  SaMmio32AndThenOr (DmiBar, Dev, R_SA_PEG_AFE_PM_TMR_OFFSET, Data32And, Data32Or);

  ///
  /// Set 0xC38[6] = 0x1 (HSW == A0), 0x0 (HSW >= B0) (DMI & PEG)
  ///
  Data32And = (UINT32) ~(BIT6);
  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
    Data32Or = 0x1 << 6;
  } else {
    Data32Or = 0x0 << 6;
  }
  if (DmiBar == 0) {
    McD1PciCfg16AndThenOr   (R_SA_PEG_CMNSPARE_OFFSET, Data32And, Data32Or);
    McD1F1PciCfg16AndThenOr (R_SA_PEG_CMNSPARE_OFFSET, Data32And, Data32Or);
    McD1F2PciCfg16AndThenOr (R_SA_PEG_CMNSPARE_OFFSET, Data32And, Data32Or);
  } else {
    Mmio32AndThenOr (DmiBar, R_SA_DMIBAR_CMNSPARE_OFFSET, Data32And, Data32Or);
  }

  ///
  /// Set 0x260[1:0] = '10b' (DMI & PEG)
  ///
  Data32And = (UINT32) ~(BIT1 | BIT0);
  Data32Or  = 0x2;
  if (DmiBar == 0) {
    McD1PciCfg16AndThenOr   (R_SA_PEG_CFG6_OFFSET, Data32And, Data32Or);
    McD1F1PciCfg16AndThenOr (R_SA_PEG_CFG6_OFFSET, Data32And, Data32Or);
    McD1F2PciCfg16AndThenOr (R_SA_PEG_CFG6_OFFSET, Data32And, Data32Or);
  } else {
    Mmio32AndThenOr (DmiBar, R_SA_DMIBAR_CFG6_OFFSET, Data32And, Data32Or);
  }

  ///
  /// setdfelsbsel = 0
  ///
  for (i = 0; i < BundlesCount; i++) {
    SaMmio32AndThenOr (DmiBar, Dev, R_SA_PEG_AFEBND0CFG0_OFFSET + i * BUNDLE_STEP, (UINT32) ~(3 << 26), 0);
  }
  ///
  /// OFFCORGAIN = 0x3 (HSW >= A0)
  ///
  for (i = 0; i < BundlesCount; i++) {
    SaMmio32AndThenOr (DmiBar, Dev, R_SA_PEG_AFEBND0CFG1_OFFSET + i * BUNDLE_STEP, (UINT32) ~(3 << 10), 3 << 10);
  }

  ///
  /// fixtxrtermoffset = -3; (PEG only)
  ///
  if (DmiBar == 0) {
    for (i = 0; i < BundlesCount; i++) {
      SaMmio32AndThenOr (DmiBar, Dev, R_SA_PEG_AFEBND0CFG3_OFFSET + i * BUNDLE_STEP, (UINT32) ~(0x1F << 25), ((0x03 << 1) | 0x01) << 25);
    }
  }

  ///
  /// Set BND0SPARE[29:27] = '101b' (PEG)
  ///
  Data32And = (UINT32) ~(BIT29 | BIT28 | BIT27);
  Data32Or  = (UINT32) 0x28000000;
  for (i = 0; i < BundlesCount; i++) {
    MmPci32AndThenOr (
      0,
      SA_PEG_BUS_NUM,
      SA_PEG10_DEV_NUM,
      SA_PEG10_FUN_NUM,
      R_SA_PEG_BND0SPARE_OFFSET + (i * BUNDLE_STEP),
      Data32And,
      Data32Or
      );
  }
  return;
}
#endif // DMI_FLAG || PEG_FLAG

#ifdef PEG_FLAG
VOID
BubbleSort (
  IN OUT DATA_SAMPLE Array[]
  )
/**
  Bubble sort from DATA_SAMPLE

  @param[in, out] - Array[]: array of DATA_SAMPLE

  @retval None
**/
{
  UINTN       n;
  UINTN       i;
  UINTN       j;
  DATA_SAMPLE Temp;

  n = (UINTN) Array[0].Count;

  for (i = 1; i <= n; i++) {
    for (j = n; j > i; j--) {
      if (Array[j - 1].Data > Array[j].Data) {
        Temp          = Array[j - 1];
        Array[j - 1]  = Array[j];
        Array[j]      = Temp;
      }
    }
  }

  return;
}

UINT32
GetMiddleValue (
  IN OUT DATA_SAMPLE Array[]
  )
/**
  Get Middle Value from DATA_SAMPLE

  @param[in, out] - Array[]: array of DATA_SAMPLE

  @retval UINT32 : Middle Value of DATA_SAMPLE
**/
{
  UINT32  n;

  n = Array[0].Count;

  return Array[n / 2 + 1].Data;
}

#ifdef EFI_DEBUG
VOID
DumpSamplerValues (
  IN UINT32 DmiBar,
  IN UINT8  CpuSteppingId,
  IN UINTN  Dev,
  IN UINTN  LanesCount
  )
/**
  Dump Sampler Values

  @param[in] DmiBar        - DMIBAR address
  @param[in] CpuSteppingId - CPUID.1.EAX[3:0], CPU stepping ID
  @param[in] Dev           - Device number
  @param[in] LanesCount    - Value of Lanes

  @retval None
**/
{
  UINTN   Lane;
  UINT32  Sampler;
  UINT8   SampleData;

  DEBUG ((EFI_D_INFO, "Lane DS0  DS1  ESP0 ESP1 ESM0 ESM1\n"));
  for (Lane = 0; Lane < LanesCount; Lane++) {
    DEBUG ((EFI_D_INFO, "%2d:  ", Lane));
    for (Sampler = 0; Sampler < 6; Sampler++) {
      SampleData = GetMonBus (DmiBar, Dev, Lane, 0x31 + Sampler, CpuSteppingId) & 0x3F;
      DEBUG ((EFI_D_INFO, "%02X   ", SampleData));
    }

    DEBUG ((EFI_D_INFO, "\n"));
  }

  return;
}
#endif // EFI_DEBUG

VOID
SamplerCalibratePegPort (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_STALL_PPI           *StallPpi,
  IN       UINT8                       CpuSteppingId,
  IN       UINTN                       Dev,
  IN       UINTN                       LanesCount,
  IN       UINTN                       HwStrap
  )
/**
 This function performs the PEG Sampler Calibration for HSW on a given PEG controller
 It also calls the Step 2 of PEG Recipe routine.

  @param[in] PeiServices         - Pointer to the PEI services table
  @param[in] StallPpi            - pointer to PEI_STALL_PPI
  @param[in] CpuSteppingId       - CPUID.1.EAX[3:0], CPU stepping ID
  @param[in] Dev                 - Device number
  @param[in] LanesCount          - Value of Lanes
  @param[in] HwStrap             - HwStrap configuration from FUSESCMN

  @retval None
**/
{
  UINTN       Iteration;
  UINTN       PegSamplerIterations;
  UINTN       Lane;
  UINTN       i;
  BOOLEAN     Peg11Present;
  BOOLEAN     Peg12Present;
  BOOLEAN     EarlyExit;
  BOOLEAN     CodeFound;
  UINT8       SampleData;
  UINT32      Sampler;
  UINT16      CodesFound;
  UINT32      MiddleCode;
  UINTN       OcDelay;

  ///
  /// Array of sampling data - 16 lanes max, 6 samplers per lane, up to MAX_CODES per sampler
  /// Size is 3 * 16 * 6 * 11 = 3168 bytes
  ///
  DATA_SAMPLE SampleArray[16][6][MAX_CODES + 1];

  PegSamplerIterations  = SA_PEG_SAMPLER_ITERATIONS;
  Peg11Present          = FALSE;
  Peg12Present          = FALSE;

  if (LanesCount == 16) {
    OcDelay = 10;
  } else {
    OcDelay = 20;
  }

  ZeroMem (&SampleArray, sizeof (SampleArray));

  if (HwStrap == SA_PEG_x8_x8_x0) {
    Peg11Present = TRUE;
  }


  if (HwStrap == SA_PEG_x8_x4_x4) {
    Peg11Present  = TRUE;
    Peg12Present  = TRUE;
  }
  ///
  /// Dump the Sampler values before calibration
  ///
  DEBUG ((EFI_D_INFO, "--- Sampler values before calibration ---\n"));
#ifdef EFI_DEBUG
  DumpSamplerValues (0, CpuSteppingId, Dev, LanesCount);
#endif // EFI_DEBUG
  ///
  /// Disable the link
  ///
  MmPci16Or   (0, SA_PEG_BUS_NUM, Dev, 0, R_SA_PEG_LCTL_OFFSET, BIT4);
  if (Peg11Present) {
    MmPci16Or (0, SA_PEG_BUS_NUM, Dev, 1, R_SA_PEG_LCTL_OFFSET, BIT4);
  }
  if (Peg12Present) {
    MmPci16Or (0, SA_PEG_BUS_NUM, Dev, 2, R_SA_PEG_LCTL_OFFSET, BIT4);
  }

  ///
  /// Delay 10 ms after link disable
  ///
  StallPpi->Stall (PeiServices, StallPpi, 10 * STALL_ONE_MILLI_SECOND);

  ///
  /// Override L0s and L1 - set bits 11, 13 & 15 of AFEOVR
  ///
  MmPci32Or   (0, SA_PEG_BUS_NUM, Dev, 0, R_SA_PEG_AFEOVR_OFFSET, 0xA800);
  if (Peg11Present) {
    MmPci32Or (0, SA_PEG_BUS_NUM, Dev, 1, R_SA_PEG_AFEOVR_OFFSET, 0xA800);
  }
  if (Peg12Present) {
    MmPci32Or (0, SA_PEG_BUS_NUM, Dev, 2, R_SA_PEG_AFEOVR_OFFSET, 0xA800);
  }

  for (Iteration = 0; Iteration < PegSamplerIterations; Iteration++) {
    PostCode ((UINT8) (Iteration / 100));
    ///
    /// First trigger OC on each lane
    ///
    for (Lane = 0; Lane < LanesCount; Lane++) {
      SetLoadBus (0, Dev, Lane, 0x39, 0x1, CpuSteppingId);
      SetLoadBus (0, Dev, Lane, 0x3A, 0xC, CpuSteppingId);
    }
    ///
    /// Delay to give the OC time to complete
    ///
    StallPpi->Stall (PeiServices, StallPpi, OcDelay * STALL_ONE_MICRO_SECOND);

    for (Lane = 0; Lane < LanesCount; Lane++) {
      for (Sampler = 0; Sampler < 6; Sampler++) {
        SampleData = GetMonBus (0, Dev, Lane, 0x31 + Sampler, CpuSteppingId) & 0x3F;

        ///
        /// SampleArray[Lane][Sampler][0].Count holds the number of distinct codes found so far
        ///
        CodeFound = FALSE;
        for (i = 1; i <= SampleArray[Lane][Sampler][0].Count; ++i) {
          if (SampleArray[Lane][Sampler][i].Data == SampleData) {
            CodeFound = TRUE;
            SampleArray[Lane][Sampler][i].Count++;
            break;
          }
        }

        if (!CodeFound) {
          if (i > MAX_CODES) {
            DEBUG ((EFI_D_ERROR, "ERROR: PEG dev=%d, lane=%d, sampler=%d, iteration=%d, found more than %d distinct codes!!!\n", Dev, Lane, Sampler, Iteration, MAX_CODES));
            if (LanesCount == 16) {
              PostCode ((UINT8) (ERROR_BY_16 >> 8));
            } else {
              PostCode ((UINT8) (ERROR_NOT_BY_16 >> 8));
            }

            CpuDeadLoop ();
          }
          ///
          /// Increment the number of distinct codes found for this sampler
          ///
          SampleArray[Lane][Sampler][0].Count++;

          ///
          /// Save the new code
          ///
          SampleArray[Lane][Sampler][i].Data  = SampleData;
          SampleArray[Lane][Sampler][i].Count = 1;
        }
      }
    }
    ///
    /// Exit early if all the sampled lanes have more than 5 codes,
    /// and we covered at least 20% of iterations.
    /// Ignore the inactive lanes, these will only show one code.
    ///
    if (Iteration > PegSamplerIterations / 5) {
      EarlyExit = TRUE;
      for (Lane = 0; Lane < LanesCount; Lane++) {
        for (Sampler = 0; Sampler < 6; Sampler++) {
          CodesFound = SampleArray[Lane][Sampler][0].Count;
          if ((CodesFound > 1) && (CodesFound < 5)) {
            EarlyExit = FALSE;
            break;
          }
        }

        if (!EarlyExit) {
          break;
        }
      }

      if (EarlyExit) {
        break;
      }
    }
  }
  ///
  /// Iteration
  ///
  for (Lane = 0; Lane < LanesCount; Lane++) {
    for (Sampler = 0; Sampler < 6; Sampler++) {
      ///
      /// Sort the codes for this Lane / Sampler
      ///
      BubbleSort (SampleArray[Lane][Sampler]);
      ///
      /// Find the middle value
      ///
      MiddleCode = GetMiddleValue (SampleArray[Lane][Sampler]);
      ///
      /// Set the middle calibration code
      ///
      SetLoadBus (0, Dev, Lane, 0x31 + Sampler, MiddleCode, CpuSteppingId);
    }
  }
  ///
  /// Restore AFEOVR
  ///
  MmPci32   (0, SA_PEG_BUS_NUM, Dev, 0, R_SA_PEG_AFEOVR_OFFSET) = 0;
  if (Peg11Present) {
    MmPci32 (0, SA_PEG_BUS_NUM, Dev, 1, R_SA_PEG_AFEOVR_OFFSET) = 0;
  }
  if (Peg12Present) {
    MmPci32 (0, SA_PEG_BUS_NUM, Dev, 2, R_SA_PEG_AFEOVR_OFFSET) = 0;
  }
  ///
  /// Delay 1 ms before link enable
  ///
  StallPpi->Stall (PeiServices, StallPpi, STALL_ONE_MILLI_SECOND);

  ///
  /// Enable the link
  ///
  MmPci16And   (0, SA_PEG_BUS_NUM, Dev, 0, R_SA_PEG_LCTL_OFFSET, ~BIT4);
  if (Peg11Present) {
    MmPci16And (0, SA_PEG_BUS_NUM, Dev, 1, R_SA_PEG_LCTL_OFFSET, ~BIT4);
  }
  if (Peg12Present) {
    MmPci16And (0, SA_PEG_BUS_NUM, Dev, 2, R_SA_PEG_LCTL_OFFSET, ~BIT4);
  }
  ///
  /// Dump the Sampler values after calibration
  ///
  DEBUG ((EFI_D_INFO, "--- Sampler values after calibration ---\n"));
#ifdef EFI_DEBUG
  DumpSamplerValues (0, CpuSteppingId, Dev, LanesCount);
#endif // EFI_DEBUG
  return;
}

VOID
PegSamplerCalibration (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SA_PLATFORM_POLICY_PPI      *SaPlatformPolicyPpi,
  IN       EFI_PEI_STALL_PPI           *StallPpi,
  IN       UINT8                       CpuSteppingId,
  IN       UINT8                       HwStrap
  )
/**
 This function performs the PEG Sampler Calibration

  @param[in] PeiServices         - Pointer to the PEI services table
  @param[in] SaPlatformPolicyPpi - pointer to SA_PLATFORM_POLICY_PPI
  @param[in] StallPpi            - pointer to PEI_STALL_PPI
  @param[in] CpuSteppingId       - CPUID.1.EAX[3:0], CPU stepping ID
  @param[in] HwStrap             - HwStrap configuration from FUSESCMN

  @retval None
**/
{
  UINT32  Data32;

  ///
  /// Calibrate PEG10/PEG11/PEG12 - 16 lanes total
  ///
  if (MmPci16 (0, 0, 1, 0, PCI_VID) != 0xFFFF) {
    Data32 = MmPci32 (0, 0, 1, 0, R_SA_PEG_PEGSTS_OFFSET);
    if (((Data32 & 0xFFFF) != 0) && (((Data32 >> 16) & 0x0F) >= 7)) {
      SamplerCalibratePegPort (PeiServices, StallPpi, CpuSteppingId, SA_PEG10_DEV_NUM, 16, HwStrap);
    }
  }

  return;
}

VOID
PegGen2AutoSpeedDisable (
  IN CONST EFI_PEI_SERVICES             **PeiServices,
  IN SA_PLATFORM_POLICY_PPI             *SaPlatformPolicyPpi,
  IN EFI_PEI_STALL_PPI                  *StallPpi,
  IN SA_DATA_HOB                        *SaDataHob,
  IN PEG_PORT                           *PegPortTable,
  IN UINTN                              PegPortTableSize,
  IN UINT8                              HwStrap
  )
/**
 This function performs the PEG GEN2 Auto Speed Disable

  @param[in] PeiServices         - Pointer to the PEI services table
  @param[in] SaPlatformPolicyPpi - Pointer to SA_PLATFORM_POLICY_PPI
  @param[in] StallPpi            - Pointer to PEI_STALL_PPI
  @param[in] SaDataHob           - Pointer to SA_DATA_HOB
  @param[in] PegPortTable        - Pointer to PEG Port Table
  @param[in] PegPortTableSize    - Size of PEG Port Table
  @param[in] HwStrap             - Furcation HW Strap Value

  @retval None
**/
{
  UINTN          PortIndex;
  EFI_STATUS     Status;
  EFI_BOOT_MODE  BootMode;
  UINT8          DisableAutoSpeedUp;
  UINT8          PegBus;
  UINT8          PegDev;
  UINT8          PegFunc;

  DisableAutoSpeedUp = 0;
  Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
  ASSERT_EFI_ERROR (Status);

  for (PortIndex = 0; PortIndex < PegPortTableSize; PortIndex++) {
    PegBus  = PegPortTable[PortIndex].Bus;
    PegDev  = PegPortTable[PortIndex].Device;
    PegFunc = PegPortTable[PortIndex].Function;

    ///
    /// If VC0 is still pending
    /// And presence of an endpoint was detected, enable GEN2 auto speed disable
    ///
    if ((MmPci16 (0, PegBus, PegDev, PegFunc, PCI_VID                        ) != 0xFFFF) &&
       ((MmPci16 (0, PegBus, PegDev, PegFunc, R_SA_PEG_VC0RSTS_OFFSET) & BIT1) != 0)      &&
       ((MmPci16 (0, PegBus, PegDev, PegFunc, R_SA_PEG_SLOTSTS_OFFSET) & BIT6) != 0)) {
      DisableAutoSpeedUp |= 1 << PegFunc;
      MmPci16Or  (0, PegBus, PegDev, PegFunc, R_SA_PEG_LCTL_OFFSET, BIT4);
      MmPci32Or  (0, PegBus, PegDev, PegFunc, R_SA_PEG_CFG5_OFFSET, BIT9);
      MmPci16And (0, PegBus, PegDev, PegFunc, R_SA_PEG_LCTL_OFFSET, (UINT16)~(BIT4));
    }
  }
  ///
  /// If needed, reinitialize link after disable
  ///
  if (DisableAutoSpeedUp != 0) {
    if ((BootMode == BOOT_ON_S3_RESUME) || (GetPchPmStatus (WarmBoot) == TRUE)) {
      if ((SaDataHob != NULL) && (SaDataHob->PegDataValid)) {
        if (SaDataHob->PegData.PegLinkFailMask != 0) {
          ///
          /// Even on warm boot/S3 resume, we still want to force checking of VC0 status
          ///
          SaDataHob->PegData.PegLinkFailMask = 0;
        }
      }
    }
    GracefulLinkStatusStall (PeiServices, StallPpi, SaPlatformPolicyPpi, SaDataHob, HwStrap);
    for (PortIndex = 0; PortIndex < PegPortTableSize; PortIndex++) {
      PegBus  = PegPortTable[PortIndex].Bus;
      PegDev  = PegPortTable[PortIndex].Device;
      PegFunc = PegPortTable[PortIndex].Function;
      if (((DisableAutoSpeedUp >> PegFunc) & 1) == 1) {
        ///
        /// Retrain to allow link to reach GEN2
        ///
        MmPci16Or (0, PegBus, PegDev, PegFunc, R_SA_PEG_LCTL_OFFSET, BIT5);
      }
    }
    ///
    /// Ensure all links are now ready
    ///
    if ((BootMode == BOOT_ON_S3_RESUME) || (GetPchPmStatus (WarmBoot) == TRUE)) {
      if ((SaDataHob != NULL) && (SaDataHob->PegDataValid)) {
        if (SaDataHob->PegData.PegLinkFailMask != 0) {
          ///
          /// Even on warm boot/S3 resume, we still want to force checking of VC0 status
          ///
          SaDataHob->PegData.PegLinkFailMask = 0;
        }
      }
    }
    GracefulLinkStatusStall (PeiServices, StallPpi, SaPlatformPolicyPpi, SaDataHob, HwStrap);
    DEBUG ((EFI_D_INFO, "PEG Link Status after auto speed disable:\n"));
    for (PortIndex = 0; PortIndex < PegPortTableSize; PortIndex++) {
      PegBus  = PegPortTable[PortIndex].Bus;
      PegDev  = PegPortTable[PortIndex].Device;
      PegFunc = PegPortTable[PortIndex].Function;
      ReportPcieLinkStatus (PegBus, PegDev, PegFunc);
    }
  }
}

VOID
PciExpressInit (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SA_PLATFORM_POLICY_PPI      *SaPlatformPolicyPpi
  )
/**
  Initialize the SA PciExpress in PEI

  @param[in] PeiServices          - Pointer to the PEI services table
  @param[in] SaPlatformPolicyPpi  - SaPlatformPolicyPpi to access the GtConfig related information
**/
{
  UINT8               PegBus;
  UINT8               PegDev;
  UINT8               PegFunc;
  UINT8               PegIndex;
  UINT8               HwStrap;
  UINT64              MchBar;
  UINT64              DmiBar;
  BOOLEAN             DisableFun0;
  BOOLEAN             DisableFun1;
  BOOLEAN             DisableFun2;
  BOOLEAN             DisableLinkFunc0;
  EFI_STATUS          Status;
  CPU_FAMILY          CpuFamilyId;
  CPU_STEPPING        CpuSteppingId;
  UINTN               PegComplete;
  UINT16              Peg10Speed;
  UINT16              Peg11Speed;
  UINT16              Peg12Speed;
  UINT8               SwingControl;
  UINT8               Gen3Capable;
  UINT8               i;
  EFI_PEI_STALL_PPI   *StallPpi;
  UINT32              Data32;
  UINT32              CapOffset;
  BOOLEAN             AnyGen3Endpoint;
  UINT8               FullSwing;
  UINT8               PreCursor;
  UINT8               Cursor;
  UINT8               PostCursor;

  PEG_PORT            PegPortTable[] = {
    ///
    ///  Bus, Device, Function, Index, PresenceDetect, MaxLinkWidth, EndpointMaxLinkSpeed
    ///
    { SA_PEG_BUS_NUM, SA_PEG10_DEV_NUM, SA_PEG10_FUN_NUM, 0, FALSE, 16, 0},
    { SA_PEG_BUS_NUM, SA_PEG11_DEV_NUM, SA_PEG11_FUN_NUM, 1, FALSE,  8, 0},
    { SA_PEG_BUS_NUM, SA_PEG12_DEV_NUM, SA_PEG12_FUN_NUM, 2, FALSE,  4, 0}
  };
  SA_DATA_HOB         *SaDataHob;
  BOOLEAN             S3Flow;
  UINT8               LinkStatusGood;
  BOOLEAN             FunctionExists;
  UINT8               UnusedLanes;
  UINT8               CtrlMaxLinkWidth;
  UINT8               EpMaxLinkWidth;
  UINT16              LoopCount;
  UINT8               MaxBndlPwrdnCount;
  UINT8               BndlPwrdnCount;
  UINT8               PwrDnUnusedBundlesSetupData;

  HwStrap                     = SA_PEG_x16_x0_x0;
  DisableFun0                 = FALSE;
  DisableFun1                 = FALSE;
  DisableFun2                 = FALSE;
  DisableLinkFunc0            = FALSE;
  MchBar                      = McD0PciCfg64 (R_SA_MCHBAR) &~BIT0;
  DmiBar                      = McD0PciCfg64 (R_SA_DMIBAR) &~BIT0;
  PegDev                      = 0x1;

  Peg10Speed                  = 0;
  Peg11Speed                  = 0;
  Peg12Speed                  = 0;
  SwingControl                = SaPlatformPolicyPpi->PcieConfig->PegSwingControl;
  SaDataHob                   = NULL;
  S3Flow                      = FALSE;
  Gen3Capable                 = TRUE;
  AnyGen3Endpoint             = FALSE;
  FullSwing                   = 0;
  CtrlMaxLinkWidth            = 0;
  EpMaxLinkWidth              = 0;
  UnusedLanes                 = 0;
  LoopCount                   = 0;
  MaxBndlPwrdnCount           = 0;
  BndlPwrdnCount              = 0;
  PwrDnUnusedBundlesSetupData = 0xFF;

  ///
  /// Check to see if PEG exists and leave initialization function if non-existant
  ///
  if (McD1PciCfg16 (PCI_VID) == 0xFFFF) {
    DEBUG ((EFI_D_INFO, "PEG controller not detected\n"));
    return;
  }

  ///
  /// Read the HW straps - Bus 0, Device 1, Fun 0, Reg 0x504 BIT17,16
  /// Fun 1 & 2 disabled = Pcie 1x16, Fun 2 disabled = Pcie 2x8
  /// Fun 1 & 2 enabled = Pcie 1x8 + (2+4)
  ///
  HwStrap = (UINT8) ((MmPci32 (0, SA_MC_BUS, PegDev, 0, R_SA_PEG_FUSESCMN_OFFSET) >> 16) & 0x03);
  DEBUG ((EFI_D_INFO, "PEG HW Strap value %x\n", HwStrap));

  Status = (*PeiServices)->LocatePpi (PeiServices, &gEfiPeiStallPpiGuid, 0, NULL, (VOID **)&StallPpi);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Update PEG LCAP.MLW based on PEG port Split configuration.
  /// This is Write-Once field, so keep the values in the structure,
  /// and write later together with LCAP.MLS.
  ///
  switch (HwStrap) {
    case SA_PEG_x8_x4_x4:
      ///
      /// 0x0: Device 1 functions 1 and 2 enabled
      /// PEG10: x16->x8, PEG11: x8->x4, PEG12: x4
      ///
      PegPortTable[0].MaxLinkWidth  = 8;
      PegPortTable[1].MaxLinkWidth  = 4;
      break;


    case SA_PEG_x8_x8_x0:
      ///
      /// 0x2: Device 1 function 1 enabled; function 2 disabled
      /// PEG10: x16->x8, PEG11: x8, PEG12: N/A
      ///
      PegPortTable[0].MaxLinkWidth = 8;
      break;

    default:
    case SA_PEG_x16_x0_x0:
      break;
  }

  CpuFamilyId   = GetCpuFamily();
  CpuSteppingId = GetCpuStepping();

  ///
  /// Read PEG Gen3 Fuse to see if it should override programming
  ///
  if (MmPci32 (0, SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_MC_CAPID0_B) & BIT20) {
    Gen3Capable = FALSE;
    DEBUG ((EFI_D_INFO, "PEG Gen3 Fused off\n"));
  }

  ///
  /// Gen3 Preset Search: 0 = Disabled, 1 = Enabled (default)
  ///
  if (SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearch == 2) {
    SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearch = 1;
  }

  ///
  /// Restore SA Data HOB's PEG data
  ///
  if (SaPlatformPolicyPpi->Revision >= SA_PLATFORM_POLICY_PPI_REVISION_2) {
    SaDataHob = (SA_DATA_HOB *) GetFirstGuidHob (&gSaDataHobGuid);

    if (SaDataHob != NULL) {
      if (SaPlatformPolicyPpi->PcieConfig->PegDataPtr != NULL) {
        DEBUG ((EFI_D_INFO, "\nRestore SA PEG DATA from previous boot: Size=%X\n", sizeof (SA_PEG_DATA)));
        CopyMem (&(SaDataHob->PegData), SaPlatformPolicyPpi->PcieConfig->PegDataPtr, sizeof (SA_PEG_DATA));
        SaDataHob->PegDataValid = TRUE;
        if ((SaDataHob->PegData.PegGen3PresetSearch != SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearch) && (SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearch != 1)) {
          ///
          /// Zero out previous boot GEN3 Preset data so old data won't be re-used when PegGen3PresetSearch re-enabled later
          ///
          DEBUG ((EFI_D_INFO, "\nPegGen3PresetSearch is disabled, Clear old Preset data\n"));
          for (PegIndex = 0; PegIndex < SA_PEG_MAX_FUN; PegIndex++) {
            SaDataHob->PegData.EndPointVendorIdDeviceId[PegIndex] = 0;
            for (i = 0; i < SA_PEG_MAX_LANE; i++) {
              SaDataHob->PegData.BestPreset[i] = 0;
            }
          }
        }
      }
      SaDataHob->PegData.PegGen3PresetSearch = SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearch;
    }
  }

  ///
  /// Perform PEG Pre-Detection steps
  ///
  for (PegComplete = 0; PegComplete < ((sizeof (PegPortTable)) / (sizeof (PEG_PORT))); PegComplete++) {
    PegBus  = PegPortTable[PegComplete].Bus;
    PegDev  = PegPortTable[PegComplete].Device;
    PegFunc = PegPortTable[PegComplete].Function;
    PegPreDetectionSteps (PegBus, PegDev, PegFunc, SaPlatformPolicyPpi);
  }

  ///
  /// Perform PEG Recipe steps
  ///
  DEBUG ((EFI_D_INFO, "PEG Recipe...\n"));
  PegDmiRecipe (SaPlatformPolicyPpi, (UINT32) MchBar, 0, SA_PEG10_DEV_NUM, SwingControl);

  ///
  /// Perform PEG Gen3 Equalization steps and load preset values
  ///
  if (Gen3Capable == TRUE) {
    if (SaPlatformPolicyPpi->PcieConfig->PegGen3Equalization != 0) {
      DEBUG ((EFI_D_INFO, "PEG Gen3 Equalization...\n"));
      PegGen3Equalization (SaPlatformPolicyPpi, CpuSteppingId, HwStrap);
    }
  }

  ///
  /// PEG Sampler Calibration: 0 = Disabled (default), 1 = Enabled
  ///
  if (SaPlatformPolicyPpi->PcieConfig->PegSamplerCalibrate == 2) {
    SaPlatformPolicyPpi->PcieConfig->PegSamplerCalibrate = 0;
  }

  if (SaPlatformPolicyPpi->PcieConfig->PegSamplerCalibrate == 1) {
    ///
    /// Back up the current PEG speed in LCTL2.TLS[3:0]
    ///
    Peg10Speed = McD1PciCfg16   (R_SA_PEG_LCTL2_OFFSET) & 0x0F;
    Peg11Speed = McD1F1PciCfg16 (R_SA_PEG_LCTL2_OFFSET) & 0x0F;
    Peg12Speed = McD1F2PciCfg16 (R_SA_PEG_LCTL2_OFFSET) & 0x0F;
    ///
    /// Set the PEG speed in LCTL2.TLS[3:0] to Gen1 before clearing DEFER_OC,
    /// in order to run Sampler Calibration at Gen1.
    ///
    McD1PciCfg16AndThenOr   (R_SA_PEG_LCTL2_OFFSET, 0xFFF0, 1);
    McD1F1PciCfg16AndThenOr (R_SA_PEG_LCTL2_OFFSET, 0xFFF0, 1);
    McD1F2PciCfg16AndThenOr (R_SA_PEG_LCTL2_OFFSET, 0xFFF0, 1);
  }

  for (PegComplete = 0; PegComplete < ((sizeof (PegPortTable)) / (sizeof (PEG_PORT))); PegComplete++) {
    ///
    /// Program the PEG speed according to Setup options: Auto/Gen1/2/3
    /// We have to do it before endpoint enumeration, so that uncompliant card
    /// can train at a lower speed.
    ///
    ConfigurePegGenX (
      PeiServices,
      StallPpi,
      SaPlatformPolicyPpi,
      PegPortTable,
      PegComplete,
      CpuSteppingId,
      Gen3Capable
      );
  }

  ///
  /// RxCEM Loopback (LPBK) Mode
  ///
  if ((SaPlatformPolicyPpi->Revision >= SA_PLATFORM_POLICY_PPI_REVISION_3) && (SaPlatformPolicyPpi->PcieConfig->RxCEMLoopback == 1)) {
    McD1PciCfg32AndThenOr (R_SA_PEG_PEGTST_OFFSET, ~(BIT19|BIT18|BIT17|BIT16), (SaPlatformPolicyPpi->PcieConfig->RxCEMLoopbackLane & 0xF) << 16);
    for (i = 0; i < SA_PEG_MAX_LANE; i++) {
      if (i == SaPlatformPolicyPpi->PcieConfig->RxCEMLoopbackLane) {
        McD1PciCfg32And (R_SA_PEG_AFELN0CFG0_OFFSET + LANE_STEP * i, ~BIT9);
      } else {
        McD1PciCfg32Or (R_SA_PEG_AFELN0CFG0_OFFSET + LANE_STEP * i, BIT9);
      }
    }
  }

  ///
  /// Enable 3-OC retry for PEG(0/1/2). HSW/CRW earlier than C0.
  ///
  if (((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId < EnumHswC0)) ||
      ((CpuFamilyId == EnumCpuCrw) && (CpuSteppingId < EnumCrwC0))) {
    McD1PciCfg32Or    (R_SA_PEG_AFE_PWRON_OFFSET, BIT5);
    McD1F1PciCfg32Or  (R_SA_PEG_AFE_PWRON_OFFSET, BIT5);
    McD1F2PciCfg32Or  (R_SA_PEG_AFE_PWRON_OFFSET, BIT5);
  }

  ///
  /// Bypass phase2
  ///
  Data32  = McD1PciCfg32 (R_SA_PEG_EQCFG_OFFSET);
  Data32 |= BIT15 | BIT1;
  McD1PciCfg32 (R_SA_PEG_EQCFG_OFFSET) = Data32;

  ///
  /// Clear DEFER_OC in offset 0xC24[16] on all PEG controllers to start the PEG training
  ///
  McD1PciCfg32And   (R_SA_PEG_AFE_PWRON_OFFSET, ~BIT16);
  McD1F1PciCfg32And (R_SA_PEG_AFE_PWRON_OFFSET, ~BIT16);
  McD1F2PciCfg32And (R_SA_PEG_AFE_PWRON_OFFSET, ~BIT16);

  ///
  /// Delay for 100ms to meet the timing requirements of the PCI Express Base
  /// Specification, Revision 1.0A, Section 6.6 ("...software must wait at least
  /// 100 ms from the end of reset of one or more device before it is permitted
  /// to issue Configuration Requests to those devices").
  ///
  GracefulLinkStatusStall (PeiServices, StallPpi, SaPlatformPolicyPpi, SaDataHob, HwStrap);

  PegGen2AutoSpeedDisable (PeiServices,
                           SaPlatformPolicyPpi,
                           StallPpi,
                           SaDataHob,
                           &(PegPortTable[0]),
                           ((sizeof (PegPortTable)) / (sizeof (PEG_PORT))),
                           HwStrap);

  ///
  /// Read the presence detect bit for each PEG port - must be done before sampler calibration
  ///
  for (PegComplete = 0; PegComplete < ((sizeof (PegPortTable)) / (sizeof (PEG_PORT))); PegComplete++) {
    PegBus  = PegPortTable[PegComplete].Bus;
    PegDev  = PegPortTable[PegComplete].Device;
    PegFunc = PegPortTable[PegComplete].Function;
    if (( MmPci16 (0, PegBus, PegDev, PegFunc, PCI_VID                        ) != 0xFFFF) &&
        ((MmPci16 (0, PegBus, PegDev, PegFunc, R_SA_PEG_SLOTSTS_OFFSET) & BIT6) !=      0)) {
      PegPortTable[PegComplete].PresenceDetect = TRUE;

      ///
      /// Read the endpoint's Max Link Speed
      ///
      MmPci32AndThenOr (0, PegBus, PegDev, PegFunc, PCI_PBUS, 0xFF0000FF, 0x00010100);
      MmPci16 (0, 1, 0, 0, PCI_VID) = 0;
      CapOffset = PcieFindCapId (1, 0, 0, EFI_PCI_CAPABILITY_ID_PCIEXP);
      if (CapOffset != 0) {
        Data32 = MmPci32 (0, 1, 0, 0, CapOffset + 0xC);
        PegPortTable[PegComplete].EndpointMaxLinkSpeed = Data32 & 0xF;
        if (PegPortTable[PegComplete].EndpointMaxLinkSpeed >= 0x3) {
          AnyGen3Endpoint = TRUE;
        }
      }
      MmPci32And (0, PegBus, PegDev, PegFunc, PCI_PBUS, 0xFF0000FF);
    }
  }
  DEBUG ((EFI_D_INFO, "Presence detect table...\n"));
  for (PegComplete = 0; PegComplete < ((sizeof (PegPortTable)) / (sizeof (PEG_PORT))); PegComplete++) {
    DEBUG ((EFI_D_INFO, " PEG%d%d PresenceDetect: %x. EndpointMaxLinkSpeed: %x.\n",
            PegPortTable[PegComplete].Device,
            PegPortTable[PegComplete].Function,
            PegPortTable[PegComplete].PresenceDetect,
            PegPortTable[PegComplete].EndpointMaxLinkSpeed));
  }

  ///
  /// If any Gen3 device, setup equalization values and retrain link
  ///
  if (AnyGen3Endpoint &&
      (((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId >= EnumHswB0)) ||
       ((CpuFamilyId == EnumCpuCrw) && (CpuSteppingId >= EnumCrwB0)))) {
    ///
    /// Program presets based upon endpoint fullswing value
    ///
    for (i = 0; i < SA_PEG_MAX_LANE; i++) {
      switch (i) {
        case  0:
          GetLinkPartnerFullSwing (i, &FullSwing);
          break;
        case  8:
          if (PegPortTable[1].PresenceDetect) {
            GetLinkPartnerFullSwing (i, &FullSwing);
          }
          break;
        case 12:
          if (PegPortTable[2].PresenceDetect) {
            GetLinkPartnerFullSwing (i, &FullSwing);
          }
          break;
        default:
          break;
      }
      GetCoefficientsFromPreset (SaPlatformPolicyPpi->PcieConfig->Gen3EndPointPreset[i], FullSwing, &PreCursor, &Cursor, &PostCursor);
      SetPartnerTxCoefficients (i, &PreCursor, &Cursor, &PostCursor);
    }

    ///
    /// Redo EQ
    ///
    for (PegComplete = 0; PegComplete < ((sizeof (PegPortTable)) / (sizeof (PEG_PORT))); PegComplete++) {
      if (PegPortTable[PegComplete].PresenceDetect) {
        PegBus  = PegPortTable[PegComplete].Bus;
        PegDev  = PegPortTable[PegComplete].Device;
        PegFunc = PegPortTable[PegComplete].Function;
        MmPci32Or (0, PegBus, PegDev, PegFunc, R_SA_PEG_LCTL3_OFFSET, BIT0);  ///< DOEQ
        MmPci16Or (0, PegBus, PegDev, PegFunc, R_SA_PEG_LCTL_OFFSET, BIT5);   ///< Retrain link
      }
    }
    for (PegComplete = 0; PegComplete < ((sizeof (PegPortTable)) / (sizeof (PEG_PORT))); PegComplete++) {
      if (PegPortTable[PegComplete].PresenceDetect) {
        WaitForL0 (PeiServices, StallPpi, &(PegPortTable[PegComplete]), FALSE);
      }
    }
  }

  ///
  /// Sampler Calibration
  ///
  if (SaPlatformPolicyPpi->PcieConfig->PegSamplerCalibrate == 1) {
    DEBUG ((EFI_D_INFO, "PEG SamplerCalibration...\n"));
    PegSamplerCalibration (PeiServices, SaPlatformPolicyPpi, StallPpi, CpuSteppingId, HwStrap);

    ///
    /// Restore the PEG speed
    ///
    McD1PciCfg16AndThenOr   (R_SA_PEG_LCTL2_OFFSET, 0xFFF0, Peg10Speed);
    McD1F1PciCfg16AndThenOr (R_SA_PEG_LCTL2_OFFSET, 0xFFF0, Peg11Speed);
    McD1F2PciCfg16AndThenOr (R_SA_PEG_LCTL2_OFFSET, 0xFFF0, Peg12Speed);

    ///
    /// Delay 100ms to let endpoint train properly
    ///
//[-start-110930-IB05300224-modify]//    
    StallPpi->Stall (PeiServices, StallPpi, SaPlatformPolicyPpi->PcieConfig->PcieResetDelay * 10 * STALL_ONE_MILLI_SECOND);
//[-end-110930-IB05300224-modify]//    
  }
  ///
  /// Gen3 Preset Search: 0 = Disabled, 1 = Enabled (default)
  ///
  if (SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearch == 2) {
    SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearch = 1;
  }

  if (SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearch == 1) {
    PegGen3PresetSearch (PeiServices, SaPlatformPolicyPpi, StallPpi, SaDataHob);
  }

  ///
  /// After last equalization, set PH3 bypass
  ///
  McD1PciCfg32Or (R_SA_PEG_EQCFG_OFFSET, BIT15 | BIT14);

  ///
  /// Scan PEG Ports for device population
  ///
  DEBUG ((EFI_D_INFO, "PEG Ports Scanning starts.\n"));
  for (PegComplete = 0; PegComplete < ((sizeof (PegPortTable)) / (sizeof (PEG_PORT))); PegComplete++) {

    PegBus    = PegPortTable[PegComplete].Bus;
    PegDev    = PegPortTable[PegComplete].Device;
    PegFunc   = PegPortTable[PegComplete].Function;
    PegIndex  = PegPortTable[PegComplete].Index;

    ///
    /// Check for a card presence in the PEG slot, or if the PEG port exists.
    ///
    if ((MmPci16 (0, PegBus, PegDev, PegFunc, PCI_VID) == 0xFFFF) ||
        (PegPortTable[PegComplete].PresenceDetect == FALSE)) {
      if (SaPlatformPolicyPpi->PcieConfig->AlwaysEnablePeg == 0) {
        goto PegDisable;
      }
    } else {
      if (SaPlatformPolicyPpi->PcieConfig->AlwaysEnablePeg == 0) {
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
        /// Negotiation Done?
        ///
        if ((MmPci16 (0, PegBus, PegDev, PegFunc, R_SA_PEG_VC0RSTS_OFFSET) & BIT1) != 0) {
//[-start-130626-IB05400422-add]//
          //
          // Must clear 0x18, 0x19, 0x1A to release bus number 1
          // Be careful when updating RC.
          //
          MmPci32And (0, PegBus, PegDev, PegFunc, PCI_PBUS, 0xFF0000FF);
//[-end-130626-IB05400422-add]//
          goto PegDisable;
        }

        ///
        /// Restore bus numbers on the PEG bridge.
        ///
        MmPci32And (0, PegBus, PegDev, PegFunc, PCI_PBUS, 0xFF0000FF);
      }

      if (SaPlatformPolicyPpi->Revision >= SA_PLATFORM_POLICY_PPI_REVISION_8) {
        PwrDnUnusedBundlesSetupData = SaPlatformPolicyPpi->PcieConfig->PowerDownUnusedBundles[PegIndex];
      } else {
        PwrDnUnusedBundlesSetupData = 0xFF; ///< Forced to AUTO mode for calculating unused bundles to powerdown
      }

      if (PwrDnUnusedBundlesSetupData == 0xff) { ///< AUTO mode for calculating unused bundles to powerdown
        ///
        /// Read the controller's Max Link Width
        ///
        CtrlMaxLinkWidth = (UINT8) ((MmPci32 (0, PegBus, PegDev, PegFunc, R_SA_PEG_LCAP_OFFSET) >> 4) & 0x3F);

        ///
        /// Read the endpoint's Max Link Width
        ///

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
        /// Check if the device actually got mapped into config space,
        /// if the device wasn't able to be mapped into config space then
        /// it's possible that it's a test card or some other device that
        /// does not support config space.  In that case our only option is
        /// to assume that the link trains to its max width and use that to
        /// determine which bundles to power down
        ///
        if (MmPci32 (0, 1, 0, 0, PCI_VID) == 0xFFFFFFFF) {
          EpMaxLinkWidth = (UINT8) ((MmPci16 (0, PegBus, PegDev, PegFunc, R_SA_PEG_LSTS_OFFSET) & 0x3f0) >> 4);
          DEBUG ((EFI_D_INFO,
                  "PEG%d%d - Endpoint not responding to PCI config space access, assuming negotiated width (X%d) is max width\n",
                  PegDev,
                  PegFunc,
                  EpMaxLinkWidth
                ));
        } else {
          CapOffset = PcieFindCapId (1, 0, 0, EFI_PCI_CAPABILITY_ID_PCIEXP);
          if (CapOffset != 0) {
            EpMaxLinkWidth = (UINT8) ((MmPci32 (0, 1, 0, 0, CapOffset + 0xC) >> 4) & 0x3F);
          }
        }

        ///
        /// Restore bus numbers on the PEG bridge.
        ///
        MmPci32And (0, PegBus, PegDev, PegFunc, PCI_PBUS, 0xFF0000FF);

        if (CtrlMaxLinkWidth > EpMaxLinkWidth) {
          UnusedLanes = CtrlMaxLinkWidth - EpMaxLinkWidth;
        } else {
          UnusedLanes = 0;
        }

        BndlPwrdnCount  = (UnusedLanes / 2); 

        DEBUG ((EFI_D_INFO, "CtrlMLW[%d]. EpMLW[%d]. UnusedLanes[%d]. BndlPwrdnCount[%d].\n", CtrlMaxLinkWidth, EpMaxLinkWidth, UnusedLanes, BndlPwrdnCount));
        } else if (PwrDnUnusedBundlesSetupData != 0) { ///< User selection mode: 1...8 unused bundles
          BndlPwrdnCount = PwrDnUnusedBundlesSetupData;
          DEBUG ((EFI_D_INFO, "BndlPwrdnCount[%d].\n", BndlPwrdnCount));
        }

      ///
      /// PowerOff unused lanes for PEGs
      ///
      if (((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId >= EnumHswC0)) ||
          ((CpuFamilyId == EnumCpuCrw) && (CpuSteppingId >= EnumCrwC0))) {
          if (PwrDnUnusedBundlesSetupData != 0) {
            MaxBndlPwrdnCount = GetMaxBundles((EFI_PEI_SERVICES **)PeiServices, PegFunc, HwStrap);
            if (BndlPwrdnCount > MaxBndlPwrdnCount) {
              BndlPwrdnCount = MaxBndlPwrdnCount;
              DEBUG ((EFI_D_INFO+EFI_D_ERROR, "BndlPwrdnCount violation! Overriding BndlPwrdnCount! BndlPwrdnCount[%d].\n", BndlPwrdnCount));
		    }
            PowerDownUnusedBundles((EFI_PEI_SERVICES **)PeiServices, PegFunc, HwStrap, BndlPwrdnCount);
		  }
	  }
	  
      ///
      /// Additional Programming Steps for PEGs
      ///
      DEBUG ((EFI_D_INFO, "Run AdditionalPegProgramSteps on PEG%x%x!\n", PegDev, PegFunc));
      AdditionalPegProgramSteps (SaPlatformPolicyPpi, PegBus, PegDev, PegFunc);
    }

    if ((HwStrap == SA_PEG_x16_x0_x0) && (PegIndex == 0)) {
      DisableFun1 = TRUE;
      DisableFun2 = TRUE;
      break;
    }
    if ((HwStrap == SA_PEG_x8_x8_x0) && (PegIndex == 1)) {
      DisableFun2 = TRUE;
      break;
    }
    if ((HwStrap == SA_PEG_x8_x4_x4) && (PegIndex == 2)) {
      break;
    }

    continue;

PegDisable:
    ///
    /// SA_PEG_x16_x0_x0 Mode: in this mode, PEG11 and PEG12 need to be Disabled by BIOS in this driver.
    /// Only PEG10 needs to be checked (whether has a VGA device on it) and disabled if not.
    ///
    /// SA_PEG_x8_x8_x0 Mode: in this mode, PEG12 needs to be disabled, PEG10 and PEG11
    /// need to be checked and disabled if no device installed.
    ///
    ///
    /// SA_PEG_x8_x4_x4 Mode: in this mode, all PEG10, PEG11 and PEG12 devices
    /// need to be checked and disabled if no device installed.
    ///
    if (HwStrap == SA_PEG_x16_x0_x0) {
      DisableFun0 = TRUE;
      DisableFun1 = TRUE;
      DisableFun2 = TRUE;
      break;
    } else if (HwStrap == SA_PEG_x8_x8_x0) {
      DisableFun2 = TRUE;
      if (PegIndex == 0) {
        DisableFun0 = TRUE;
      } else {
        DisableFun1 = TRUE;
        break;
      }
    } else if (HwStrap == SA_PEG_x8_x4_x4) {
      if (PegIndex == 0) {
        DisableFun0 = TRUE;
      } else if (PegIndex == 1) {
        DisableFun1 = TRUE;
      } else {
        DisableFun2 = TRUE;
        break;
      }
    }
  } ///< End of the for Loop

  if (!DisableFun1 || !DisableFun2) {
    ///
    /// PEG10 must be enabled if PEG11 and/or PEG12 are enabled
    ///
    if (DisableFun0) {
      DisableLinkFunc0 = TRUE;
    }

    DisableFun0 = FALSE;
  }

  if (MmPci16 (0, SA_PEG_BUS_NUM, SA_PEG10_DEV_NUM, SA_PEG10_FUN_NUM, R_SA_PEG_VID_OFFSET) != 0xFFFF) {
    FunctionExists = TRUE;
  } else {
    FunctionExists = FALSE;
    DEBUG ((EFI_D_WARN, "PEG10 Disabled.\n"));
  }
  if ((DisableFun0 || DisableLinkFunc0) && FunctionExists) {
    ///
    /// Set D1.F0.R 224h [8] = 1
    ///
    MmPci32Or (0, SA_PEG_BUS_NUM, SA_PEG10_DEV_NUM, SA_PEG10_FUN_NUM, R_SA_PEG_LTSSMC_OFFSET, BIT8);

    ///
    /// DisableLink. Set D1.F0.R 0B0h [4] (LD (Link Disable) bit in Link Control Register
    /// Set D1.F0.R D10h [0].
    ///
    MmPci8Or (0, SA_PEG_BUS_NUM, SA_PEG10_DEV_NUM, SA_PEG10_FUN_NUM, R_SA_PEG_LCTL_OFFSET, BIT4);

    ///
    /// Poll until D1.F0.R 464h [5:0] = 2
    ///
    LoopCount = 0;
    while(   ((MmPci32 (0, SA_PEG_BUS_NUM, SA_PEG10_DEV_NUM, SA_PEG10_FUN_NUM, R_SA_PEG_REUT_PH1_PIS_OFFSET) & 0x3F) != 2)
          && (LoopCount < SA_PEG_LINK_DISABLE_MAXWAIT)) {
      StallPpi->Stall (PeiServices, StallPpi, STALL_ONE_MICRO_SECOND*100); //100usec
      LoopCount++;
    }

    ///
    /// Program AFEOVR.RXSQDETOVR
    /// PCIe link disable for Switchable GFx
    /// Additional Power savings: Set 0:1:0 0xC20 BIT4 = 0 & BIT5 = 1
    ///
    MmPci8AndThenOr (0, SA_PEG_BUS_NUM, SA_PEG10_DEV_NUM, SA_PEG10_FUN_NUM, R_SA_PEG_AFEOVR_OFFSET, ~(BIT5 | BIT4), BIT5);

    if (((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId >= EnumHswC0)) ||
        ((CpuFamilyId == EnumCpuCrw) && (CpuSteppingId >= EnumCrwC0))) {
        MaxBndlPwrdnCount = GetMaxBundles((EFI_PEI_SERVICES **)PeiServices, 0, HwStrap);
        PowerDownUnusedBundles((EFI_PEI_SERVICES **)PeiServices, 0, HwStrap, MaxBndlPwrdnCount);
	}

    if (DisableFun0) {
      ///
      /// Set D1.F0.R D20h [30] to power off PEG lanes when no device is attached (prvtexdetq=1).
      /// Clear D0.F0.R 054h (DEVEN) enable bit.
      ///
      MmPci32Or (0, SA_PEG_BUS_NUM, SA_PEG10_DEV_NUM, SA_PEG10_FUN_NUM, R_SA_PEG_PEGCOMLCGCTRL_OFFSET, BIT30);
      MmPci8And (0, SA_MC_BUS, 0, 0, R_SA_DEVEN, ~B_SA_DEVEN_D1F0EN_MASK);
      DEBUG ((EFI_D_WARN, "PEG10 Disabled.\n"));
    }
  }

  if (MmPci16 (0, SA_PEG_BUS_NUM, SA_PEG11_DEV_NUM, SA_PEG11_FUN_NUM, R_SA_PEG_VID_OFFSET) != 0xFFFF) {
    FunctionExists = TRUE;
  } else {
    FunctionExists = FALSE;
    DEBUG ((EFI_D_WARN, "PEG11 Disabled.\n"));
  }
  if (DisableFun1 && FunctionExists) {
    ///
    /// Set D1.F1.R 224h [8] = 1
    ///
    MmPci32Or (0, SA_PEG_BUS_NUM, SA_PEG11_DEV_NUM, SA_PEG11_FUN_NUM, R_SA_PEG_LTSSMC_OFFSET, BIT8);

    ///
    /// DisableLink. Set D1.F1.R 0B0h [4] (LD (Link Disable) bit in Link Control Register.
    /// Set D1.F1.R D10h [0].
    /// Set D1.F1.R D20h [30] to power off PEG lanes when no device is attached (prvtexdetq=1).
    /// Clear B0,D0,F0 054h (DEVEN) enable bit.
    ///
    MmPci8Or (0, SA_PEG_BUS_NUM, SA_PEG11_DEV_NUM, SA_PEG11_FUN_NUM, R_SA_PEG_LCTL_OFFSET, BIT4);

    ///
    /// Poll until D1.F0.R 464h [13:8] = 2
    ///
    LoopCount = 0;
    while(   (((MmPci32 (0, SA_PEG_BUS_NUM, SA_PEG10_DEV_NUM, SA_PEG10_FUN_NUM, R_SA_PEG_REUT_PH1_PIS_OFFSET) >> 8) & 0x3F) != 2)
          && (LoopCount < SA_PEG_LINK_DISABLE_MAXWAIT)) {
      StallPpi->Stall (PeiServices, StallPpi, STALL_ONE_MICRO_SECOND*100); //100usec
      LoopCount++;
    }

    ///
    /// Program AFEOVR.RXSQDETOVR
    /// PCIe link disable for Switchable GFx
    /// Additional Power savings: Set 0:1:1 0xC20 BIT4 = 0 & BIT5 = 1
    ///
    MmPci8AndThenOr (0, SA_PEG_BUS_NUM, SA_PEG11_DEV_NUM, SA_PEG11_FUN_NUM, R_SA_PEG_AFEOVR_OFFSET, ~(BIT5 | BIT4), BIT5);

    if (((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId >= EnumHswC0)) ||
        ((CpuFamilyId == EnumCpuCrw) && (CpuSteppingId >= EnumCrwC0))) {
        MaxBndlPwrdnCount = GetMaxBundles((EFI_PEI_SERVICES **)PeiServices, 1, HwStrap);
        PowerDownUnusedBundles((EFI_PEI_SERVICES **)PeiServices, 1, HwStrap, MaxBndlPwrdnCount);
	}

    MmPci32Or (0, SA_PEG_BUS_NUM, SA_PEG11_DEV_NUM, SA_PEG11_FUN_NUM, R_SA_PEG_PEGCOMLCGCTRL_OFFSET, BIT30);
    MmPci8And (0, SA_MC_BUS, 0, 0, R_SA_DEVEN, ~B_SA_DEVEN_D1F1EN_MASK);
    DEBUG ((EFI_D_WARN, "PEG11 Disabled.\n"));
  }

  if (MmPci16 (0, SA_PEG_BUS_NUM, SA_PEG12_DEV_NUM, SA_PEG12_FUN_NUM, R_SA_PEG_VID_OFFSET) != 0xFFFF) {
    FunctionExists = TRUE;
  } else {
    FunctionExists = FALSE;
    DEBUG ((EFI_D_WARN, "PEG12 Disabled.\n"));
  }
  if (DisableFun2 && FunctionExists) {
    ///
    /// Set D1.F2.R 224h [8] = 1
    ///
    MmPci32Or (0, SA_PEG_BUS_NUM, SA_PEG12_DEV_NUM, SA_PEG12_FUN_NUM, R_SA_PEG_LTSSMC_OFFSET, BIT8);

    ///
    /// DisableLink. Set D1.F2.R 0B0h [4] (LD (Link Disable) bit in Link Control Register.
    /// Set D1.F2.R D10h [0].
    /// Set D1.F2.R D20h [30] to power off PEG lanes when no device is attached (prvtexdetq=1).
    /// Clear B0,D0,F0 054h (DEVEN) enable bit.
    ///
    MmPci8Or (0, SA_PEG_BUS_NUM, SA_PEG12_DEV_NUM, SA_PEG12_FUN_NUM, R_SA_PEG_LCTL_OFFSET, BIT4);

    ///
    /// Poll until D1.F0.R 464h [21:16] = 2
    ///
    LoopCount = 0;
    while(   (((MmPci32 (0, SA_PEG_BUS_NUM, SA_PEG10_DEV_NUM, SA_PEG10_FUN_NUM, R_SA_PEG_REUT_PH1_PIS_OFFSET) >> 16) & 0x3F) != 2)
          && (LoopCount < SA_PEG_LINK_DISABLE_MAXWAIT)) {
      StallPpi->Stall (PeiServices, StallPpi, STALL_ONE_MICRO_SECOND*100); //100usec
      LoopCount++;
    }

    ///
    /// Program AFEOVR.RXSQDETOVR
    /// PCIe link disable for Switchable GFx
    /// Additional Power savings: Set 0:1:2 0xC20 BIT4 = 0 & BIT5 = 1
    ///
    MmPci8AndThenOr (0, SA_PEG_BUS_NUM, SA_PEG12_DEV_NUM, SA_PEG12_FUN_NUM, R_SA_PEG_AFEOVR_OFFSET, ~(BIT5 | BIT4), BIT5);

    if (((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId >= EnumHswC0)) ||
        ((CpuFamilyId == EnumCpuCrw) && (CpuSteppingId >= EnumCrwC0))) {
        MaxBndlPwrdnCount = GetMaxBundles((EFI_PEI_SERVICES **)PeiServices, 2, HwStrap);
        PowerDownUnusedBundles((EFI_PEI_SERVICES **)PeiServices, 2, HwStrap, MaxBndlPwrdnCount);
	}

    MmPci32Or (0, SA_PEG_BUS_NUM, SA_PEG12_DEV_NUM, SA_PEG12_FUN_NUM, R_SA_PEG_PEGCOMLCGCTRL_OFFSET, BIT30);
    MmPci8And (0, SA_MC_BUS, 0, 0, R_SA_DEVEN, ~B_SA_DEVEN_D1F2EN_MASK);
    DEBUG ((EFI_D_WARN, "PEG12 Disabled.\n"));
  }

  for (PegComplete = 0; PegComplete < ((sizeof (PegPortTable)) / (sizeof (PEG_PORT))); PegComplete++) {
    PegBus  = PegPortTable[PegComplete].Bus;
    PegDev  = PegPortTable[PegComplete].Device;
    PegFunc = PegPortTable[PegComplete].Function;
    WaitForVc0Negotiation(PeiServices, StallPpi, PegBus, PegDev, PegFunc);
    ReportPcieLinkStatus(PegBus, PegDev, PegFunc);
  }

  ///
  /// Re-check Link again and see if PegLinkFailMask in SaDataHob needed update
  ///
  if (SaDataHob != NULL) {
    LinkStatusGood = 0;
    if ((BIT1 & MmPci16(0, 0, SA_PEG10_DEV_NUM, SA_PEG10_FUN_NUM, R_SA_PEG_VC0RSTS_OFFSET)) != BIT1) {
      LinkStatusGood |= BIT0;
    }
    if ((BIT1 & MmPci16(0, 0, SA_PEG10_DEV_NUM, SA_PEG11_FUN_NUM, R_SA_PEG_VC0RSTS_OFFSET)) != BIT1) {
      LinkStatusGood |= BIT1;
    }
    if ((BIT1 & MmPci16(0, 0, SA_PEG10_DEV_NUM, SA_PEG12_FUN_NUM, R_SA_PEG_VC0RSTS_OFFSET)) != BIT1) {
      LinkStatusGood |= BIT2;
    }
    if (SaDataHob->PegData.PegLinkFailMask != (UINT8) (~LinkStatusGood)) {
      DEBUG ((EFI_D_INFO, "Original PegLinkFailMask=%X, Final PegLinkFailMask=%X\n", SaDataHob->PegData.PegLinkFailMask, (UINT8) (~LinkStatusGood)));
      SaDataHob->PegData.PegLinkFailMask = (UINT8) (~LinkStatusGood);
    }
  }

  if ((SaPlatformPolicyPpi->Revision >= SA_PLATFORM_POLICY_PPI_REVISION_11) &&
      (SaPlatformPolicyPpi->PcieConfig->PegComplianceTestingMode == 1)) {
    for (PegComplete = 0; PegComplete < ((sizeof (PegPortTable)) / (sizeof (PEG_PORT))); PegComplete++) {
      PegBus  = PegPortTable[PegComplete].Bus;
      PegDev  = PegPortTable[PegComplete].Device;
      PegFunc = PegPortTable[PegComplete].Function;
      MmPci32Or (0, PegBus, PegDev, PegFunc, R_SA_PEG_CFG5_OFFSET, BIT0);
    }
  }

  ///
  /// Maximize the dedicated credits for the PEG controllers
  ///
  MaximizeSharedCredits();
  RebalancePegPerformanceCredits (DisableFun0, DisableFun1, DisableFun2);
  return;
}

VOID
ConfigurePegGenX (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_STALL_PPI           *StallPpi,
  IN       SA_PLATFORM_POLICY_PPI      *SaPlatformPolicyPpi,
  IN       PEG_PORT                    *PegPortTable,
  IN       UINTN                       TableIndex,
  IN       UINT8                       CpuSteppingId,
  IN       UINT8                       Gen3Capable
  )
/**
  Configure PEG GenX mode

  @param[in] PeiServices         - Pointer to the PEI services table
  @param[in] StallPpi            - Pointer to PEI_STALL_PPI
  @param[in] SaPlatformPolicyPpi - Pointer to SA_PLATFORM_POLICY_PPI
  @param[in] PegPortTable        - Pointer to PEG_PORT array
  @param[in] TableIndex          - Index in PEG_PORT array
  @param[in] CpuSteppingId       - CPU stepping
  @param[in] Gen3Capable         - Selected PEG_PORT is Gen3 capable

  @retval None
**/
{
  UINT8   PegPortGenx;
  UINT8   PegBus;
  UINT8   PegDev;
  UINT8   PegFunc;
  UINT8   PegIndex;
  UINT8   MaxLinkWidth;
  UINT16  LinkSpeed;

  PegBus        = PegPortTable[TableIndex].Bus;
  PegDev        = PegPortTable[TableIndex].Device;
  PegFunc       = PegPortTable[TableIndex].Function;
  PegIndex      = PegPortTable[TableIndex].Index;
  MaxLinkWidth  = PegPortTable[TableIndex].MaxLinkWidth;

  ///
  /// Check if this port exists
  ///
  if (MmPci16 (0, PegBus, PegDev, PegFunc, PCI_VID) == 0xFFFF) {
    return;
  }

  ///
  /// PegPortGenx: 0 = Auto, 1 = Gen1, 2 = Gen2, 3 = Gen3
  ///
  PegPortGenx = SaPlatformPolicyPpi->PcieConfig->PegGenx[PegIndex];

  if (PegPortGenx == PEG_AUTO) {
    DEBUG ((EFI_D_ERROR, "Auto\n"));
    LinkSpeed = (UINT16)(MmPci32(0, PegBus, PegDev, PegFunc, R_SA_PEG_LCAP_OFFSET) & 0x0F);
  } else {
    LinkSpeed = SaPlatformPolicyPpi->PcieConfig->PegGenx[PegIndex];
    DEBUG ((EFI_D_ERROR, "Speed From Setup %x\n", LinkSpeed));
  }
  ///
  /// If Gen3 is fused off, limit is Gen2
  ///
  if (Gen3Capable == FALSE) {
    if (LinkSpeed > 2) {
      LinkSpeed = 2;
    }
  }
  ///
  /// Set the requested speed in Max Link Speed in LCAP[3:0] and Target Link Speed in LCTL2[3:0].
  /// Update LCAP.MLW in the same write as it's a Write-Once field
  ///
  DEBUG ((EFI_D_INFO, "PEG%x%x (%x:%x:%x) - Max Link Speed = Gen%d\n", PegDev, PegFunc, PegBus, PegDev, PegFunc, LinkSpeed));
  MmPci32AndThenOr (0, PegBus, PegDev, PegFunc, R_SA_PEG_LCAP_OFFSET, 0xFFFFFC00, ((UINT32) MaxLinkWidth << 4) | LinkSpeed);
  MmPci16AndThenOr (0, PegBus, PegDev, PegFunc, R_SA_PEG_LCTL2_OFFSET, ~(0x0F), LinkSpeed);

  return;
}

VOID
AdditionalPegProgramSteps (
  IN SA_PLATFORM_POLICY_PPI  *SaPlatformPolicyPpi,
  IN  UINT8                  PegBus,
  IN  UINT8                  PegDev,
  IN  UINT8                  PegFunc
  )
/**
  Additional PEG Programming Steps at PEI

  @param[in] SaPlatformPolicyPpi - pointer to SA_PLATFORM_POLICY_PPI
  @param[in] PegBus      - Pci Bus Number
  @param[in] PegDev      - Pci Device Number
  @param[in] PegFunc     - Pci Func Number

  @retval None
**/
{
  UINT32              Data32And;
  UINT32              Data32Or;


  ///
  /// Set L0SLAT[15:0] to 0x2020
  ///
  Data32And = (UINT32) ~(0xFFFF);
  Data32Or  = 0x00002020;
  MmPci32AndThenOr (0, PegBus, PegDev, PegFunc, R_SA_PEG_L0SLAT_OFFSET, Data32And, Data32Or);

  ///
  /// Disable PEG Debug Align Message - set 258[29] = '1b'
  ///
  Data32And = (UINT32) ~BIT29;
  Data32Or  = BIT29;
  MmPci32AndThenOr (0, PegBus, PegDev, PegFunc, R_SA_PEG_CFG4_OFFSET, Data32And, Data32Or);

  ///
  /// Retrain the link only if VC0 negotiation is complete at this point.
  /// This is to support CLB card together with "Aways Enable PEG" option
  ///
  if ((MmPci16 (0, PegBus, PegDev, PegFunc, R_SA_PEG_VC0RSTS_OFFSET) & BIT1) == 0) {
    ///
    /// Set LCTL.RL (0xb0 bit 5) to initiate link retrain
    ///
    MmPci8Or (0, PegBus, PegDev, PegFunc, R_SA_PEG_LCTL_OFFSET, BIT5);
    ///
    /// Wait for Link training complete
    ///
    while (MmPci16 (0, PegBus, PegDev, PegFunc, R_SA_PEG_LSTS_OFFSET) & BIT11) {
    };
  }

  return;
}

VOID
MaximizeSharedCredits (
  )
/**
  Maximize the dedicated credits for the PEG controllers
**/
{

  UINT64 MchBar;
  UINT32 Crdtctl0;
  UINT32 Crdtctl1;
  UINT32 Crdtctl2;
  UINT32 Crdtctl3;
  UINT8  Data8;
  UINT8  Iotrk;
  UINT8  Rrtrk;
  BOOL   CommitUpdates;
  UINT8  i;

  Iotrk = 40;
  Rrtrk = 71;
  CommitUpdates = FALSE;

  MchBar = McD0PciCfg64 (R_SA_MCHBAR) & ~BIT0;
  Crdtctl0 = Mmio32 (MchBar, R_SA_MCHBAR_CRDTCTL0_OFFSET);
  Crdtctl1 = Mmio32 (MchBar, R_SA_MCHBAR_CRDTCTL1_OFFSET);
  Crdtctl2 = Mmio32 (MchBar, R_SA_MCHBAR_CRDTCTL2_OFFSET);
  Crdtctl3 = Mmio32 (MchBar, R_SA_MCHBAR_CRDTCTL3_OFFSET);

  Data8 = 0;
  for (i = 0; i < 24; i += 3) {
    Data8 += (Crdtctl0 >> i) & 0x7;
    Data8 += (Crdtctl1 >> i) & 0x7;
  }
  if (Data8 > Iotrk) {
    DEBUG ((EFI_D_ERROR, "ERROR: Attempted to reserve > %d IOTRK (Attempt = %d)! Skipping programming.\n", Iotrk, Data8));
    CommitUpdates = FALSE;
  } else {
    Iotrk -= Data8;
    DEBUG ((EFI_D_INFO, "IOTRK: Reserved = %d. Shared = %d. Total = %d.\n", Data8, Iotrk, Data8 + Iotrk));
  }

  Data8 = 0;
  for (i = 0; i < 24; i += 3) {
    Data8 += (Crdtctl2 >> i) & 0x7;
  }
  Data8 += (Crdtctl2 >> 24) & 0x3F;
  if (Data8 > Rrtrk) {
    DEBUG ((EFI_D_ERROR, "ERROR: Attempted to reserve > %d RRTRK (Attempt = %d)! Skipping programming.\n", Rrtrk, Data8));
    CommitUpdates = FALSE;
  } else {
    Rrtrk -= Data8;
    DEBUG ((EFI_D_INFO, "RRTRK: Reserved = %d. Shared = %d. Total = %d.\n", Data8, Rrtrk, Data8 + Rrtrk));
  }

  if (CommitUpdates) {
    Crdtctl3 = ((Rrtrk & 0x7F) << 6) | (Iotrk & 0x3F);
    Mmio32AndThenOr (MchBar, R_SA_MCHBAR_CRDTCTL3_OFFSET, (UINT32) ~(0x00001FFF), Crdtctl3);
  }

  return;
}

VOID
RebalancePegPerformanceCredits (
  IN  BOOLEAN   DisablePeg10,
  IN  BOOLEAN   DisablePeg11,
  IN  BOOLEAN   DisablePeg12
  )
/**
  Rebalance Credits when PEG controllers so that no starvation occurs

  @param[in] DisablePeg10     - Peg10 disable/enable status
  @param[in] DisablePeg11     - Peg11 disable/enable status
  @param[in] DisablePeg12     - Peg12 disable/enable status

  @retval None
**/
{
  UINT64 MchBar;
  UINT32 Crdtctl4;
  UINT32 Crdtctl6;
  UINT32 Crdtctl8;
  UINT16 PegLinkWidth10;
  UINT16 PegLinkWidth11;
  UINT16 PegLinkWidth12;

  MchBar = McD0PciCfg64 (R_SA_MCHBAR) & ~BIT0;
  Crdtctl4 = Mmio32 (MchBar, R_SA_MCHBAR_CRDTCTL4_OFFSET);
  Crdtctl6 = Mmio32 (MchBar, R_SA_MCHBAR_CRDTCTL6_OFFSET);
  Crdtctl8 = Mmio32 (MchBar, R_SA_MCHBAR_CRDTCTL8_OFFSET);

  DEBUG ((EFI_D_INFO, "Crdtctl4 Crdtctl6 Crdtctl8 Before = %x %x %x\n", Crdtctl4, Crdtctl6, Crdtctl8));

  PegLinkWidth10 = 0;
  PegLinkWidth11 = 0;
  PegLinkWidth12 = 0;

  if (!DisablePeg10) {
    PegLinkWidth10 = (MmPci16 (0, SA_PEG_BUS_NUM, SA_PEG10_DEV_NUM, SA_PEG10_FUN_NUM, R_SA_PEG_LSTS_OFFSET) & 0x3F0) >> 4;
  }
  if (!DisablePeg11) {
    PegLinkWidth11 = (MmPci16 (0, SA_PEG_BUS_NUM, SA_PEG11_DEV_NUM, SA_PEG11_FUN_NUM, R_SA_PEG_LSTS_OFFSET) & 0x3F0) >> 4;
  }
  if (!DisablePeg12) {
    PegLinkWidth12 = (MmPci16 (0, SA_PEG_BUS_NUM, SA_PEG12_DEV_NUM, SA_PEG12_FUN_NUM, R_SA_PEG_LSTS_OFFSET) & 0x3F0) >> 4;
  }

  DEBUG ((EFI_D_INFO, "PEG10: LinkDisabled = %x. Width = %x\n", DisablePeg10, PegLinkWidth10));
  DEBUG ((EFI_D_INFO, "PEG11: LinkDisabled = %x. Width = %x\n", DisablePeg11, PegLinkWidth11));
  DEBUG ((EFI_D_INFO, "PEG12: LinkDisabled = %x. Width = %x\n", DisablePeg12, PegLinkWidth12));

  ///
  /// PEG10 = x8 and PEG11 = x8
  ///
  if (!DisablePeg10 && !DisablePeg11 && (PegLinkWidth10 == 8) && (PegLinkWidth11 == 8)) {
    Crdtctl4 &= ~0x3E0;
    Crdtctl4 |= (Crdtctl4 & 0x7C00) >> 5;
    Crdtctl6 &= ~0x3E0;
    Crdtctl6 |= (Crdtctl6 & 0x7C00) >> 5;
    Crdtctl8 &= ~0xFC0;
    Crdtctl8 |= (Crdtctl8 & 0x3F000) >> 6;
  }

  ///
  /// PEG12 = x4
  ///
  if (!DisablePeg12 && (PegLinkWidth12 == 4)) {
    ///
    /// PEG10 = x4
    ///
    if (!DisablePeg10 && (PegLinkWidth10 == 4)) {
      Crdtctl4 &= ~0x3E0;
      Crdtctl4 |= (Crdtctl4 & 0xF8000) >> 10;
      Crdtctl6 &= ~0x3E0;
      Crdtctl6 |= (Crdtctl6 & 0xF8000) >> 10;
      Crdtctl8 &= ~0xFC0;
      Crdtctl8 |= (Crdtctl8 & 0xFC0000) >> 12;
    }
    ///
    /// PEG11 = x4
    ///
    if (!DisablePeg11 && (PegLinkWidth11 == 4)) {
      Crdtctl4 &= ~0x7C00;
      Crdtctl4 |= (Crdtctl4 & 0xF8000) >> 5;
      Crdtctl6 &= ~0x7C00;
      Crdtctl6 |= (Crdtctl6 & 0xF8000) >> 5;
      Crdtctl8 &= ~0x3F000;
      Crdtctl8 |= (Crdtctl8 & 0xFC0000) >> 6;
    }
  }

  DEBUG ((EFI_D_INFO, "Crdtctl4 Crdtctl6 Crdtctl8 After = %x %x %x\n", Crdtctl4, Crdtctl6, Crdtctl8));

  Mmio32AndThenOr (MchBar, R_SA_MCHBAR_CRDTCTL4_OFFSET, (UINT32) ~0x01FFFFFF, Crdtctl4);
  Mmio32AndThenOr (MchBar, R_SA_MCHBAR_CRDTCTL6_OFFSET, (UINT32) ~0x01FFFFFF, Crdtctl6);
  Mmio32AndThenOr (MchBar, R_SA_MCHBAR_CRDTCTL8_OFFSET, (UINT32) ~0x3FFFFFFF, Crdtctl8);

  return;
}

VOID
PegPreDetectionSteps (
  IN  UINT8                   PegBus,
  IN  UINT8                   PegDev,
  IN  UINT8                   PegFunc,
  IN SA_PLATFORM_POLICY_PPI   *SaPlatformPolicyPpi
  )
/**
  Additional PEG Programming Steps before PEG detection at PEI

  @param[in] PegBus              - Pci Bus Number
  @param[in] PegDev              - Pci Device Number
  @param[in] PegFunc             - Pci Func Number
  @param[in] SaPlatformPolicyPpi - Pointer to SA_PLATFORM_POLICY_PPI
**/
{
  UINT32        Data32;
  UINT32        Data32And;
  UINT32        Data32Or;
  UINT32        i;
  CPU_STEPPING  CpuSteppingId;
  CPU_FAMILY    CpuFamilyId;

  CpuSteppingId = GetCpuStepping();
  CpuFamilyId   = GetCpuFamily();


  MmPci32Or (0, PegBus, PegDev, PegFunc, R_SA_PEG_LTSSMC_OFFSET, (UINT32) (BIT4 | BIT3 | BIT2 | BIT1 | BIT0));

  ///
  /// Set PPCIE_CR_REUT_OVR_CTL_0_1_0_MMR.GRCLKGTDIS [28] to 1 (for PCIE Margin Test, Default is kept 0)
  ///
  if ((PegDev == 1) && (PegFunc == 0)) {
    Data32And = (UINT32) ~BIT28;
    Data32Or  = 0;
    MmPci32AndThenOr (0, PegBus, PegDev, PegFunc, R_SA_PEG_REUT_OVR_CTL_OFFSET, Data32And, Data32Or);
  }

  if ((PegDev == 1) && (PegFunc == 0)) {
    ///
    /// DCBLNC = 0
    ///
    Data32And = (UINT32) ~(BIT3 | BIT2);
    Data32Or  = 0;
    for (i = 0; i < SA_PEG_MAX_BUNDLE; i++) {
      MmPci32AndThenOr (0, PegBus, PegDev, PegFunc, R_SA_PEG_G3CTL0_OFFSET + i * BUNDLE_STEP, Data32And, Data32Or);
    }
  }

  ///
  /// DEBUP3[4] = 1
  ///
  if ((CpuFamilyId == EnumCpuHsw) || (CpuFamilyId == EnumCpuCrw)) {
    Data32And = (UINT32) ~(BIT4);
    Data32Or  = BIT4;
    MmPci32AndThenOr (0, PegBus, PegDev, PegFunc, R_SA_PEG_DEBUP3_OFFSET, Data32And, Data32Or);
  }

  ///
  /// FCLKGTTLLL[2] = 1
  ///
  if (((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId < EnumHswC0)) ||
      ((CpuFamilyId == EnumCpuCrw) && (CpuSteppingId < EnumCrwC0))) {
    Data32And = (UINT32) ~(BIT2);
    Data32Or  = BIT2;
    MmPci32AndThenOr (0, PegBus, PegDev, PegFunc, R_SA_PEG_FCLKGTTLLL_OFFSET, Data32And, Data32Or);
  }

  ///
  /// Program Read-Only Write-Once Registers
  ///   R 308h [31:0]
  ///   R 314h [31:0]
  ///   R 32Ch [31:0]
  ///   R 330h [31:0]
  ///
  Data32 = MmPci32 (0, PegBus, PegDev, PegFunc, R_SA_PEG_VC0PRCA_OFFSET);
  MmPci32          (0, PegBus, PegDev, PegFunc, R_SA_PEG_VC0PRCA_OFFSET)  = Data32;
  Data32 = MmPci32 (0, PegBus, PegDev, PegFunc, R_SA_PEG_VC0NPRCA_OFFSET);
  MmPci32          (0, PegBus, PegDev, PegFunc, R_SA_PEG_VC0NPRCA_OFFSET) = Data32;
  Data32 = MmPci32 (0, PegBus, PegDev, PegFunc, R_SA_PEG_VC1PRCA_OFFSET);
  MmPci32          (0, PegBus, PegDev, PegFunc, R_SA_PEG_VC1PRCA_OFFSET)  = Data32;
  Data32 = MmPci32 (0, PegBus, PegDev, PegFunc, R_SA_PEG_VC1NPRCA_OFFSET);
  MmPci32          (0, PegBus, PegDev, PegFunc, R_SA_PEG_VC1NPRCA_OFFSET) = Data32;

  return;
}
#endif // PEG_FLAG

#if defined(DMI_FLAG) || defined(PEG_FLAG)
UINT32
SendVcuApiSequence (
  IN    UINT32  MchBar,
  IN    UINT32  Address,
  IN    UINT16  OpCode,
  IN    UINT32  WriteData
  )
/**
  Send one sequence to VCU MailBox

  @param[in] MchBar              - MCHBAR value
  @param[in] Address             - Target address
  @param[in] OpCode              - OpCode number
  @param[in] WriteData           - Data value (only used if OpCode is a write)
**/
{
  BOOL              IsWrite;
  BOOL              IsCsr;
  UINT32            DataOpCode;
  UINT32            SequenceId;
  UINT32            VcuData;
  CPU_FAMILY        CpuFamilyId;
  CPU_STEPPING      CpuSteppingId;

  CpuFamilyId   = GetCpuFamily();
  CpuSteppingId = GetCpuStepping();

  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
    if ((OpCode == V_SA_VCU_OPCODE_WRITE_CSR_REV1) || (OpCode == V_SA_VCU_OPCODE_WRITE_MMIO_REV1)) {
      IsWrite = TRUE;
    } else {
      IsWrite = FALSE;
    }

    if ((OpCode == V_SA_VCU_OPCODE_READ_CSR_REV1) || (OpCode == V_SA_VCU_OPCODE_WRITE_CSR_REV1)) {
      IsCsr = TRUE;
    } else {
      IsCsr = FALSE;
    }
  } else {
    if ((OpCode == V_SA_VCU_OPCODE_WRITE_CSR_REV2) || (OpCode == V_SA_VCU_OPCODE_WRITE_MMIO_REV2)) {
      IsWrite = TRUE;
    } else {
      IsWrite = FALSE;
    }

    if ((OpCode == V_SA_VCU_OPCODE_READ_CSR_REV2) || (OpCode == V_SA_VCU_OPCODE_WRITE_CSR_REV2)) {
      IsCsr = TRUE;
    } else {
      IsCsr = FALSE;
    }
  }

  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
    if (IsWrite) {
      DataOpCode = V_SA_VCU_OPCODE_WRITE_DATA_REV1;
      if (IsCsr) {
        SequenceId = V_SA_VCU_SEQID_WRITE_CSR_REV1;
      } else {
        SequenceId = V_SA_VCU_SEQID_WRITE_MMIO_REV1;
      }
    } else {
      DataOpCode = V_SA_VCU_OPCODE_READ_DATA_REV1;
      if (IsCsr) {
        SequenceId = V_SA_VCU_SEQID_READ_CSR_REV1;
      } else {
        SequenceId = V_SA_VCU_SEQID_READ_MMIO_REV1;
      }
    }
  } else {
    if (IsWrite) {
      DataOpCode = V_SA_VCU_OPCODE_WRITE_DATA_REV2;
      if (IsCsr) {
        SequenceId = V_SA_VCU_SEQID_WRITE_CSR_REV2;
      } else {
        SequenceId = V_SA_VCU_SEQID_WRITE_MMIO_REV2;
      }
    } else {
      DataOpCode = V_SA_VCU_OPCODE_READ_DATA_REV2;
      if (IsCsr) {
        SequenceId = V_SA_VCU_SEQID_READ_CSR_REV2;
      } else {
        SequenceId = V_SA_VCU_SEQID_READ_MMIO_REV2;
      }
    }
  }
  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
    SendVcuApiCmd (MchBar, V_SA_VCU_OPCODE_OPEN_SEQ_REV1, SequenceId);
  } else {
    SendVcuApiCmd (MchBar, V_SA_VCU_OPCODE_OPEN_SEQ_REV2, SequenceId);
  }
  SendVcuApiCmd (MchBar, OpCode, Address);
  SendVcuApiCmd (MchBar, DataOpCode, WriteData);
  VcuData = Mmio32 (MchBar, R_SA_MCHBAR_VCU_MAILBOX_DATA_OFFSET);
  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
    SendVcuApiCmd (MchBar, V_SA_VCU_OPCODE_CLOSE_SEQ_REV1, SequenceId);
  } else {
    SendVcuApiCmd (MchBar, V_SA_VCU_OPCODE_CLOSE_SEQ_REV2, SequenceId);
  }

  return VcuData;
}

VOID
SendVcuApiCmd (
  IN    UINT32  MchBar,
  IN    UINT32  Interface,
  IN    UINT32  Data
  )
/**
  Send one command to VCU MailBox

  @param[in] MchBar              - MCHBAR value
  @param[in] Interface           - Interface number
  @param[in] Data                - Data value
**/
{
  UINT32  ResponseCounter;
  UINT16  ResponseCode;
  UINT32  BusyCounter;
  UINT32  RunBusy;
  BOOL    BusyStatus;

  ResponseCode = V_SA_VCU_RESPONSE_SUCCESS;
  for (ResponseCounter = 0; ResponseCounter < V_SA_VCU_RESPONSE_RETRY_LIMIT; ResponseCounter++) {
    Mmio32 (MchBar, R_SA_MCHBAR_VCU_MAILBOX_DATA_OFFSET) = Data;
    Mmio32 (MchBar, R_SA_MCHBAR_VCU_MAILBOX_INTERFACE_OFFSET) = (Interface | B_SA_MCHBAR_VCU_STATUS_RUN_BUSY);
    BusyStatus = FALSE;
    for (BusyCounter = 0; BusyCounter < V_SA_VCU_STATUS_BUSY_LIMIT; BusyCounter++) {
      RunBusy     = Mmio32 (MchBar, R_SA_MCHBAR_VCU_MAILBOX_INTERFACE_OFFSET);
      BusyStatus  = (RunBusy & B_SA_MCHBAR_VCU_STATUS_RUN_BUSY) ? TRUE : FALSE;
      if (BusyStatus == FALSE) {
        break;
      }
    }

    if (BusyStatus) {
      DEBUG ((EFI_D_INFO, "VCU Busy Timeout after %d tries: MCHBAR=%8.8X. Interface=%8.8X. Data=%4.4X.\n", BusyCounter, MchBar, Interface, Data));
    }
    ResponseCode = Mmio16 (MchBar, R_SA_MCHBAR_VCU_MAILBOX_INTERFACE_OFFSET);
    if (ResponseCode == V_SA_VCU_RESPONSE_SUCCESS) {
      break;
    }
  }

  if (ResponseCode != V_SA_VCU_RESPONSE_SUCCESS) {
    DEBUG ((EFI_D_ERROR, "ERROR: VCU Response Error after %d tries: MCHBAR=%8.8X. Interface=%4.4X. Data=%8.8X. ResponseCode=%4.4X\n", ResponseCounter, MchBar, Interface, Data, ResponseCode));
  }

  return;
}
#endif // DMI_FLAG || PEG_FLAG

#ifdef PEG_FLAG

UINT8
GetMaxBundles (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN UINT8                     PegFunc,
  IN UINT8                     HwStrap
  )
/**
  GetMaxBundles: Get the maximum bundle numbers for the corresponding PEG

  @param[in] PeiServices          - Pointer to the PEI services table
  @param[in] PegFunc              - Points to PEG0/PEG1/PEG2/...
  @param[in] HwStrap              - Points to PEG configuration information [x16_x0_x0/x8_x8_x0/x8_x4_x4/...]

  @retval                         - MaxBndlPwrdnCount [Maximun number of bundles for this HW configuration]
**/
{
  UINT8  MaxBndlPwrdnCount;

  MaxBndlPwrdnCount  = 0;

  DEBUG ((EFI_D_INFO, "In GetMaxBundles procedure\n"));

  if (PegFunc == 0) { // PEG10
    if (HwStrap == SA_PEG_x16_x0_x0) {
      MaxBndlPwrdnCount = 8;
    } else {
      MaxBndlPwrdnCount = 4;
    }
  } else if (PegFunc == 1) { // PEG11
    if (HwStrap == SA_PEG_x8_x8_x0) {
      MaxBndlPwrdnCount = 4;
    } else if (HwStrap == SA_PEG_x8_x4_x4) {
      MaxBndlPwrdnCount = 2;
    }
  } else if (PegFunc == 2) { // PEG12
    if (HwStrap == SA_PEG_x8_x4_x4) {
      MaxBndlPwrdnCount = 2;
    }
  }

  DEBUG ((EFI_D_INFO, "MaxBndlPwrdnCount = %d\n", MaxBndlPwrdnCount));
  return MaxBndlPwrdnCount;
}

VOID
PowerDownUnusedBundles (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN UINT8                     PegFunc,
  IN UINT8                     HwStrap,
  IN UINT8                     BndlPwrdnCount
  )
/**
  PowerDownUnusedBundles: Program the PEG BundleSpare registers for power on sequence [PowerOff unused bundles for PEGs]

  @param[in] PeiServices          - Pointer to the PEI services table
  @param[in] PegFunc              - Points to PEG0/PEG1/PEG2/...
  @param[in] HwStrap              - Points to PEG configuration information [x16_x0_x0/x8_x8_x0/x8_x4_x4/...]
  @param[in] BndlPwrdnCount       - Points to how many bundles are unused and should be powered down
**/
{
  BOOLEAN  PegLaneReversal;
  UINT8    BndlPwrdnFirst;

  UINT8    i;
  UINT8    j;

  PegLaneReversal = FALSE;
  BndlPwrdnFirst  = 0;

  DEBUG ((EFI_D_INFO, "In PowerDownUnusedBundles sequence\n"));

  if (BndlPwrdnCount == 0) {
    ///
    /// If all lanes are used. Do nothing
    ///
    DEBUG ((EFI_D_INFO, "All lanes are used. Do nothing.\n"));
    return;
  }

  if ((MmPci32 (0, SA_PEG_BUS_NUM, SA_PEG10_DEV_NUM, SA_PEG10_FUN_NUM, R_SA_PEG_PEGTST_OFFSET) & BIT20) != 0) {
    DEBUG ((EFI_D_INFO, "PegLaneReversal is true\n"));
    PegLaneReversal = TRUE;
  }

  if (PegFunc == 0) { // PEG10
    if (HwStrap == SA_PEG_x16_x0_x0) {
      if (!PegLaneReversal) {
        BndlPwrdnFirst = 8 - BndlPwrdnCount;
      } else {
        BndlPwrdnFirst = 0;
      }
    } else {
      if (!PegLaneReversal) {
        BndlPwrdnFirst = 4 - BndlPwrdnCount;
      } else {
        BndlPwrdnFirst = 4;
      }
    }
  } else if (PegFunc == 1) { // PEG11
    if (HwStrap == SA_PEG_x8_x8_x0) {
      if (!PegLaneReversal) {
        BndlPwrdnFirst = 8 - BndlPwrdnCount;
      } else {
        BndlPwrdnFirst = 0;
      }
    } else if (HwStrap == SA_PEG_x8_x4_x4) {
      if (!PegLaneReversal) {
        BndlPwrdnFirst = 6 - BndlPwrdnCount;
      } else {
        BndlPwrdnFirst = 2;
      }
    }
  } else if (PegFunc == 2) { // PEG12
    if (HwStrap == SA_PEG_x8_x4_x4) {
      if (!PegLaneReversal) {
        BndlPwrdnFirst = 8 - BndlPwrdnCount;
      } else {
        BndlPwrdnFirst = 0;
      }
    }
  }

  ///
  /// Power down unused lanes per request
  ///
  DEBUG ((EFI_D_INFO, "BNDL_PWRDN PEG%d%d[%d:%d]\n", 0, PegFunc, BndlPwrdnFirst, (BndlPwrdnFirst+BndlPwrdnCount-1)));
  for (i = BndlPwrdnFirst, j=1; j <= BndlPwrdnCount; i++, j++) {
    MmPci32Or (0, SA_PEG_BUS_NUM, SA_PEG10_DEV_NUM, SA_PEG10_FUN_NUM, R_SA_PEG_BND0SPARE_OFFSET + (i * BUNDLE_STEP), BIT31);
  }

  return;
}
#endif // PEG_FLAG
