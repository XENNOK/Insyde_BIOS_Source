/** @file

  This driver trains the PEG interface.

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

#include "PcieTraining.h"

#ifdef PEG_FLAG

#if ME_SUPPORT_FLAG
#include <Ppi/PchMeUma.h>
#endif /// ME_SUPPORT_FLAG

typedef struct _MARGIN_DATA {
  UINT8 Preset;
  INT32 TimingMargin[SA_PEG_MAX_LANE];
} MARGIN_DATA;

UINT8
SelectBestPresetForLane (
  IN  PORT_INFO               *PortInfo,
  IN  UINT8                   Lane,
  IN  SA_PLATFORM_POLICY_PPI  *SaPlatformPolicyPpi,
  IN  SA_DATA_HOB             *SaDataHob,
  IN  UINT8                   TrainingPreset
  )
{
  UINT8   BestPreset;

  ///
  /// Init to EV default
  ///
  BestPreset = 7;

  ///
  /// EnableMargin    FoundUsablePreset    SaDataHob
  ///            Y                    Y        !NULL  -> Use training results; update SaDataHob
  ///            Y                    Y         NULL  -> Use training results
  ///            Y                    N        !NULL  -> Use Policy value; update SaDataHob
  ///            X                    N         NULL  -> Use Policy value
  ///            N                    X        !NULL  -> Restore from SaDataHob
  ///
  if (Lane < SA_PEG_MAX_LANE) {
    DEBUG ((EFI_D_INFO, "Preset for Lane %2d: ", Lane));
    if ((PortInfo->EnableMargin) && (PortInfo->FoundUsablePreset)) {
      ///
      /// Use the best preset found during training
      ///
      BestPreset = TrainingPreset;
      DEBUG ((EFI_D_INFO, "Search Result: P%d", BestPreset));
      if (SaDataHob != NULL) {
        DEBUG ((EFI_D_INFO, ". Saving value for next boot."));
        SaDataHob->PegData.BestPreset[Lane] = BestPreset;
      }
    } else if ((PortInfo->EnableMargin) && (!PortInfo->FoundUsablePreset) && (SaDataHob != NULL)) {
      ///
      /// Use the policy value; update SaDataHob
      ///
      BestPreset = (UINT8) SaPlatformPolicyPpi->PcieConfig->Gen3EndPointPreset[Lane];
      DEBUG ((EFI_D_INFO, "Applying Policy value: P%d. Saving value for next boot.", BestPreset));
      SaDataHob->PegData.BestPreset[Lane] = BestPreset;
    } else if ((!PortInfo->FoundUsablePreset) && (SaDataHob == NULL)) {
      ///
      /// Use the policy value
      ///
      BestPreset = (UINT8) SaPlatformPolicyPpi->PcieConfig->Gen3EndPointPreset[Lane];
      DEBUG ((EFI_D_INFO, "Applying Policy value: P%d", BestPreset));
    } else if ((!PortInfo->EnableMargin) && (SaDataHob != NULL)) {
      ///
      /// Use the preset found on a previous boot
      ///
      BestPreset = (UINT8) SaDataHob->PegData.BestPreset[Lane];
      DEBUG ((EFI_D_INFO, "Restoring previous value: P%d", BestPreset));
    }
    DEBUG ((EFI_D_INFO, "\n"));
  } else {
    ///
    /// Error: Non-existent lane
    ///
    DEBUG ((EFI_D_ERROR, "Illegal Lane: %d", Lane));
  }

  return BestPreset;
}

VOID
PegGen3PresetSearch (
  IN CONST EFI_PEI_SERVICES       **PeiServices,
  IN SA_PLATFORM_POLICY_PPI *SaPlatformPolicyPpi,
  IN EFI_PEI_STALL_PPI          *StallPpi,
  SA_DATA_HOB               *SaDataHob
  )
{
  UINT8                 Index;
  INT32                 MarginScore;
  UINT8                 PegBus;
  UINT8                 PegDev;
  UINT8                 PegFunc;
  UINT16                LinkStatus;
  BOOLEAN               LoadedSavedPreset;
  BOOLEAN               EndpointDeviceChanged;
  UINT8                 TempIndex;
  BOOLEAN               SkipBundle0;
  CPU_FAMILY            CpuFamilyId;
  CPU_STEPPING          CpuSteppingId;
  MARGIN_DATA           MarginData[MAX_PRESETS];
  PORT_INFO             PortInfoList[SA_PEG_MAX_FUN];
  INT32                 LaneScores[SA_PEG_MAX_LANE];
  INT32                 BestScores[SA_PEG_MAX_LANE];
  UINT8                 BestPresets[SA_PEG_MAX_LANE];
  EFI_STATUS            Status;
  UINT32                MonitorPort;
  UINT8                 PortInfoListLength;
  UINT8                 PortIndex;
  UINT8                 Lane;
  BOOLEAN               AnyGen3CapableLinks;
  BOOLEAN               AnyMarginingNeeded;
  UINT8                 PreCursor;
  UINT8                 Cursor;
  UINT8                 PostCursor;
  UINT8                 FullSwing;
  UINT32                NominalRecoveryCount;
  UINT8                 FirstSkippedLane;
  UINT8                 LastSkippedLane;
  UINT32                Data32;
  BOOLEAN               SlotResetNeeded;
#ifdef ME_SUPPORT_FLAG
  PCH_ME_UMA_PPI        *PchMeUmaPpi;
  UINT8                 DetectedReplacedCpu;
#endif // ME_SUPPORT_FLAG

  AnyMarginingNeeded = FALSE;
  CpuFamilyId        = GetCpuFamily();
  CpuSteppingId      = GetCpuStepping();

  ///
  /// Setup default presets to search
  ///
  MarginData[0].Preset = 7;
  MarginData[1].Preset = 3;
  MarginData[2].Preset = 5;

  ///
  /// Initialize Arrays
  ///
  for (Lane = 0; Lane < SA_PEG_MAX_LANE; Lane++) {
    LaneScores[Lane]  = -1;
    BestScores[Lane]  = -1;
    BestPresets[Lane] = MarginData[0].Preset;
  }

  DEBUG ((EFI_D_INFO, "PEG Gen3 Preset Search\n"));

  if (!SaPolicyEnablesGen3 (SaPlatformPolicyPpi)) {
    DEBUG ((EFI_D_INFO, " Gen3 is disabled by policy\n"));
    return;
  }
  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId < EnumHswB0)) {
    DEBUG ((EFI_D_WARN, " Gen3 preset search is not supported on this stepping\n"));
    return;
  }
  Data32 = (McD1PciCfg32 (R_SA_PEG_PEGTST_OFFSET) & BIT20) >> 20;
  if (Data32 != 0) {
    DEBUG ((EFI_D_WARN, " Gen3 preset search does not support lane reversal\n"));
    return;
  }

#ifdef ME_SUPPORT_FLAG
  ///
  /// If ME is supported and the CPU has been replaced, redo the Preset Search.
  /// Note that calling CpuReplacementCheck() can induce an ME-required warm reset.
  ///
  DetectedReplacedCpu = 0;
  Status              = EFI_SUCCESS;

  if ((SaDataHob != NULL) && (SaDataHob->PegDataValid)) {
    Status = (*PeiServices)->LocatePpi (PeiServices, &gPchMeUmaPpiGuid, 0, NULL, (VOID **)&PchMeUmaPpi);
    ASSERT_EFI_ERROR (Status);
    DEBUG ((EFI_D_INFO, "Calling CpuReplacementCheck\n"));
    Status = PchMeUmaPpi->CpuReplacementCheck (PeiServices, NULL, &DetectedReplacedCpu);
    ASSERT_EFI_ERROR (Status);
    DEBUG ((EFI_D_INFO, " ME reported CPU Replacement value: %x\n", DetectedReplacedCpu));
  }
#endif // ME_SUPPORT_FLAG

  ///
  /// Get the furcation setup and port information
  ///
  GetPortInfo (&(PortInfoList[0]), &PortInfoListLength, &SkipBundle0);

  ///
  /// Make sure we at Gen3 before starting, if not attempt reset and see if that helps
  ///
  SlotResetNeeded = FALSE;
  for (PortIndex = 0; PortIndex < PortInfoListLength; PortIndex++) {
    PegBus  = (PortInfoList[PortIndex]).PegPort.Bus;
    PegDev  = (PortInfoList[PortIndex]).PegPort.Device;
    PegFunc = (PortInfoList[PortIndex]).PegPort.Function;
    LinkStatus = MmPci16 (0, PegBus, PegDev, PegFunc, R_SA_PEG_LSTS_OFFSET);
    if (((LinkStatus & 0x0F) != 3) && ((PortInfoList[PortIndex]).LinkIsGen3Capable)) {
      SlotResetNeeded = TRUE;
      break;
    }
  }
  if (SlotResetNeeded) {
    ///
    /// Bypass phase2 and assert slot reset
    ///
    McD1PciCfg32Or (R_SA_PEG_EQCFG_OFFSET, BIT15);
    Status = TogglePegSlotReset (PeiServices, StallPpi, SaPlatformPolicyPpi);
    if (!EFI_ERROR (Status)) {
      for (PortIndex = 0; PortIndex < PortInfoListLength; PortIndex++) {
        PegBus  = (PortInfoList[PortIndex]).PegPort.Bus;
        PegDev  = (PortInfoList[PortIndex]).PegPort.Device;
        PegFunc = (PortInfoList[PortIndex]).PegPort.Function;
        ///
        /// Wait for Equalization Done
        ///
        while (((MmPci32 (0, PegBus, PegDev, PegFunc, R_SA_PEG_LSTS2_OFFSET) >> 1) & 0x1) != 0x1);
        ///
        /// Wait for flow control credits exchange
        ///
        WaitForVc0Negotiation (PeiServices, StallPpi, PegBus, PegDev, PegFunc);
      }
    }
  }

  ///
  /// Determine which PEG ports require testing
  ///
  AnyGen3CapableLinks = FALSE;
  for (PortIndex = 0; PortIndex < PortInfoListLength; PortIndex++) {
    PegBus  = (PortInfoList[PortIndex]).PegPort.Bus;
    PegDev  = (PortInfoList[PortIndex]).PegPort.Device;
    PegFunc = (PortInfoList[PortIndex]).PegPort.Function;

    ///
    /// If we already have Best Preset value from previous boot, use it and skip PresetSearch when end point device no change
    ///
    LoadedSavedPreset     = FALSE;
    EndpointDeviceChanged = TRUE;
    if (SaDataHob != NULL) {
      if (SaDataHob->PegDataValid) {
        LoadedSavedPreset = TRUE;
        if (SaDataHob->PegData.EndPointVendorIdDeviceId[PegFunc] == (PortInfoList[PortIndex]).EndPointVendorIdDeviceId) {
          EndpointDeviceChanged = FALSE;
        }
      }
      if (EndpointDeviceChanged) {
        ///
        /// Save new device ID vendor ID
        ///
        SaDataHob->PegData.EndPointVendorIdDeviceId[PegFunc] = (PortInfoList[PortIndex]).EndPointVendorIdDeviceId;
      }
    }
    DEBUG ((EFI_D_INFO, " PEG%x%x (%x:%x:%x) - LoadedSavedPreset = %d. EndpointDeviceChanged = %d.\n",
      PegDev, PegFunc, PegBus, PegDev, PegFunc, LoadedSavedPreset, EndpointDeviceChanged));

    ReportPcieLinkStatus (PegBus, PegDev, PegFunc);

    if ( (!LoadedSavedPreset)         ||
         (EndpointDeviceChanged)      ||
#ifdef ME_SUPPORT_FLAG
         (DetectedReplacedCpu   != 0) ||
#endif // ME_SUPPORT_FLAG
         ((SaPlatformPolicyPpi->Revision >= SA_PLATFORM_POLICY_PPI_REVISION_2) &&
          (SaPlatformPolicyPpi->PcieConfig->PegGen3ForcePresetSearch == 1    ))) {
      (PortInfoList[PortIndex]).EnableMargin = TRUE;
    } else {
      (PortInfoList[PortIndex]).EnableMargin = FALSE;
    }

    if (!(PortInfoList[PortIndex]).LinkIsGen3Capable) {
      (PortInfoList[PortIndex]).EnableMargin = FALSE;
    } else {
      AnyGen3CapableLinks = TRUE;
    }
  } ///< PegFunc Loop end

  if (!AnyGen3CapableLinks) {
    DEBUG ((EFI_D_INFO, "Skipping Preset Search - No Gen3 capable links\n"));
    return;
  }

  ///
  /// Determine if any ports need to be trained.
  /// If any ports are trained, the corresponding endpoint should also be reset with PERST#.
  ///
  for (PortIndex = 0; PortIndex < PortInfoListLength; PortIndex++) {
    if ((PortInfoList[PortIndex]).EnableMargin == TRUE) {
      AnyMarginingNeeded = TRUE;
      break;
    }
  }

  if (AnyMarginingNeeded) {
    MonitorPort = OpenMonitor (PeiServices, SaPlatformPolicyPpi, StallPpi);

    McD1PciCfg32Or (R_SA_PEG_REUT_PH_CTR_OFFSET, B_SA_PEG_REUT_PH_CTR_AUTOCOMP_MASK);

    ///
    /// Presets Loop start
    ///
    for (Index = 0; Index < MAX_PRESETS; Index++) {
      ///
      /// Clear out old values
      ///
      for (TempIndex = 0; TempIndex < SA_PEG_MAX_LANE; TempIndex++) {
        MarginData[Index].TimingMargin[TempIndex]      = 0;
      } ///< End of for each Lane


      for (PortIndex = 0; PortIndex < PortInfoListLength; PortIndex++) {
        (PortInfoList[PortIndex]).SkipMargin = FALSE;
        PegBus  = (PortInfoList[PortIndex]).PegPort.Bus;
        PegDev  = (PortInfoList[PortIndex]).PegPort.Device;
        PegFunc = (PortInfoList[PortIndex]).PegPort.Function;

        if (!(PortInfoList[PortIndex]).LinkIsGen3Capable) {
          DEBUG ((EFI_D_INFO, "Skipping PEG%d%d - Not Gen3 capable\n", PegDev, PegFunc));
          continue;
        }

        ///
        /// Find first lane of the port for coefficient programming
        ///
        switch (PegFunc) {
          default:
          case 0:
            Lane             = 0;
            FirstSkippedLane = 0;
            LastSkippedLane  = 1;
            break;
          case 1:
            Lane             = 8;
            FirstSkippedLane = 8;
            LastSkippedLane  = 9;
            break;
          case 2:
            Lane             = 12;
            FirstSkippedLane = 12;
            LastSkippedLane  = 13;
            break;
        }

        ///
        /// Get FullSwing
        ///
        GetLinkPartnerFullSwing (Lane, &FullSwing);

        ///
        /// Get Coefficients
        ///
        GetCoefficientsFromPreset (MarginData[Index].Preset, FullSwing, &PreCursor, &Cursor, &PostCursor);

        ///
        /// Set Lane's Coefficients
        ///
        if (SkipBundle0) {
          for (TempIndex = FirstSkippedLane; TempIndex <= LastSkippedLane; TempIndex++) {
            SetPartnerTxCoefficients (TempIndex, &PreCursor, &Cursor, &PostCursor);
            ProgramPreset (1, MarginData[Index].Preset, PegFunc, TempIndex);
          }
        }
        for (TempIndex = 0; TempIndex < (PortInfoList[PortIndex]).LaneListLength; TempIndex++) {
          SetPartnerTxCoefficients ((PortInfoList[PortIndex]).LaneList[TempIndex], &PreCursor, &Cursor, &PostCursor);
          ProgramPreset (1, MarginData[Index].Preset, PegFunc, (PortInfoList[PortIndex]).LaneList[TempIndex]);
        }

        ///
        /// Set DOEQ bit
        ///
        MmPci32Or (0, PegBus, PegDev, PegFunc, R_SA_PEG_LCTL3_OFFSET, BIT0);
        RetrainLink (PeiServices, StallPpi, &((PortInfoList[PortIndex]).PegPort));

        ///
        /// Get the current link status
        ///
        LinkStatus = MmPci16 (0, PegBus, PegDev, PegFunc, R_SA_PEG_LSTS_OFFSET);
        ///
        /// No need to margin if couldn't get to Gen3 with this preset
        ///
        if ((LinkStatus & 0x0F) != 3) {
          RecoverLinkFailure (PeiServices, SaPlatformPolicyPpi, SaDataHob, StallPpi,
                              &((PortInfoList[PortIndex]).PegPort), 3,
                              GetNegotiatedWidth (&((PortInfoList[PortIndex]).PegPort)));
          LinkStatus = MmPci16 (0, PegBus, PegDev, PegFunc, R_SA_PEG_LSTS_OFFSET);
          if ((LinkStatus & 0x0F) != 3) {
            (PortInfoList[PortIndex]).SkipMargin = TRUE;
            continue;
          }
        }

        NominalRecoveryCount = SaPcieGetErrorCount (MonitorPort, PegFunc);
        StallPpi->Stall (PeiServices, StallPpi, SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearchDwellTime * STALL_ONE_MICRO_SECOND);
        NominalRecoveryCount = SaPcieGetErrorCount (MonitorPort, PegFunc) - NominalRecoveryCount;
        if (NominalRecoveryCount > 0) {
          (PortInfoList[PortIndex]).SkipMargin = TRUE;
          continue;
        }
        (PortInfoList[PortIndex]).FoundUsablePreset = TRUE;
      } ///< End of for each Port

      Status = RunMarginTest (
                 PeiServices,
                 SaPlatformPolicyPpi,
                 SaDataHob,
                 StallPpi,
                 MonitorPort,
                 &(PortInfoList[0]),
                 PortInfoListLength,
                 LaneLevelRxJitter,
                 MarginData[Index].TimingMargin
                 );

      if (EFI_ERROR (Status)) {
        for (TempIndex = 0; TempIndex < SA_PEG_MAX_LANE; TempIndex++) {
          MarginData[Index].TimingMargin[TempIndex] = 0;
        }
      }
    } ///< Presets Loop end

    McD1PciCfg32And (R_SA_PEG_REUT_PH_CTR_OFFSET, ~B_SA_PEG_REUT_PH_CTR_AUTOCOMP_MASK);
    CloseMonitor (SaPlatformPolicyPpi, MonitorPort);

    ///
    /// Find the preset with the maximum margin (largest of all margin values)
    ///
    for (Index = 0; Index < sizeof (MarginData) / sizeof (MarginData[0]); Index++) {
      for (Lane = 0; Lane < SA_PEG_MAX_LANE; Lane++) {
        LaneScores[Lane] = -1;
      }
      for (Lane = 0; Lane < SA_PEG_MAX_LANE; Lane++) {
        if (MarginData[Index].TimingMargin[Lane] != -1) {
          MarginScore = MarginData[Index].TimingMargin[Lane];
          LaneScores[Lane] = MarginScore;
        }
        if (LaneScores[Lane] > BestScores[Lane]) {
          BestScores[Lane]  = LaneScores[Lane];
          BestPresets[Lane] = MarginData[Index].Preset;
        }
      } ///< End of for each Lane
    } ///< End of for each Preset

    ///
    /// If Lanes 0-1 were skipped, copy the values from Lane 2
    ///
    if (SkipBundle0) {
      for (PortIndex = 0; PortIndex < PortInfoListLength; PortIndex++) {
        PegFunc = (PortInfoList[PortIndex]).PegPort.Function;
        switch (PegFunc) {
          default:
          case 0:
            Lane             = 2;
            FirstSkippedLane = 0;
            LastSkippedLane  = 1;
            break;
          case 1:
            Lane             = 10;
            FirstSkippedLane = 8;
            LastSkippedLane  = 9;
            break;
          case 2:
            Lane             = 14;
            FirstSkippedLane = 12;
            LastSkippedLane  = 13;
            break;
        }
        if (BestScores[Lane] != -1) {
          DEBUG ((EFI_D_INFO, "Using Lane %2d's Best Preset for Lanes %2d-%2d.\n", Lane, FirstSkippedLane, LastSkippedLane));
          for (TempIndex = FirstSkippedLane; TempIndex <= LastSkippedLane; TempIndex++) {
            BestScores[TempIndex]  = BestScores[Lane];
            BestPresets[TempIndex] = BestPresets[Lane];
          }
        }
      } ///< End of for each port
    } ///< End of SkipBundle0
  } ///< End of AnyMarginingNeeded

  for (PortIndex = 0; PortIndex < PortInfoListLength; PortIndex++) {
    PegBus  = (PortInfoList[PortIndex]).PegPort.Bus;
    PegDev  = (PortInfoList[PortIndex]).PegPort.Device;
    PegFunc = (PortInfoList[PortIndex]).PegPort.Function;
    if (!(PortInfoList[PortIndex]).LinkIsGen3Capable) {
      continue;
    }
    if (SkipBundle0) {
      switch (PegFunc) {
        default:
        case 0:
          FirstSkippedLane = 0;
          LastSkippedLane  = 1;
          break;
        case 1:
          FirstSkippedLane = 8;
          LastSkippedLane  = 9;
          break;
        case 2:
          FirstSkippedLane = 12;
          LastSkippedLane  = 13;
          break;
      }
      for (TempIndex = FirstSkippedLane; TempIndex <= LastSkippedLane; TempIndex++) {
        BestPresets[TempIndex] = SelectBestPresetForLane (
                                   &(PortInfoList[PortIndex]),
                                   TempIndex,
                                   SaPlatformPolicyPpi,
                                   SaDataHob,
                                   BestPresets[TempIndex]
                                   );
      }
    }
    for (TempIndex = 0; TempIndex < (PortInfoList[PortIndex]).LaneListLength; TempIndex++) {
      BestPresets[(PortInfoList[PortIndex]).LaneList[TempIndex]] = SelectBestPresetForLane (
                                                                     &(PortInfoList[PortIndex]),
                                                                     (PortInfoList[PortIndex]).LaneList[TempIndex],
                                                                     SaPlatformPolicyPpi,
                                                                     SaDataHob,
                                                                     BestPresets[(PortInfoList[PortIndex]).LaneList[TempIndex]]
                                                                     );
    }
  }

  ///
  /// Program the presets.  If any link was margined, also reset the
  /// endpoints in order to return the endpoints to a known-good state.
  ///
  if (AnyMarginingNeeded) {
    for (TempIndex = 0; TempIndex < SA_PEG_MAX_FUN; TempIndex++) {
      MmPci16Or (0, SA_PEG_BUS_NUM, SA_PEG_DEV_NUM, TempIndex, R_SA_PEG_LCTL_OFFSET, BIT4);
    }
    AssertPegSlotReset (SaPlatformPolicyPpi);
  }
  for (PortIndex = 0; PortIndex < PortInfoListLength; PortIndex++) {
    PegBus  = (PortInfoList[PortIndex]).PegPort.Bus;
    PegDev  = (PortInfoList[PortIndex]).PegPort.Device;
    PegFunc = (PortInfoList[PortIndex]).PegPort.Function;
    if (!(PortInfoList[PortIndex]).LinkIsGen3Capable) {
      DEBUG ((EFI_D_INFO, "PEG%d%d - Not Gen3 capable, skip coefficient programming.\n", PegDev, PegFunc));
      continue;
    }
    ///
    /// Find first lane of the port for coefficient programming
    ///
    switch (PegFunc) {
      default:
      case 0:
        Lane             = 0;
        FirstSkippedLane = 0;
        LastSkippedLane  = 1;
        break;
      case 1:
        Lane             = 8;
        FirstSkippedLane = 8;
        LastSkippedLane  = 9;
        break;
      case 2:
        Lane             = 12;
        FirstSkippedLane = 12;
        LastSkippedLane  = 13;
        break;
    }
    ///
    /// Get FullSwing
    ///
    GetLinkPartnerFullSwing (Lane, &FullSwing);

    if (SkipBundle0) {
      for (TempIndex = FirstSkippedLane; TempIndex <= LastSkippedLane; TempIndex++) {
        GetCoefficientsFromPreset (BestPresets[TempIndex], FullSwing, &PreCursor, &Cursor, &PostCursor);
        SetPartnerTxCoefficients (TempIndex, &PreCursor, &Cursor, &PostCursor);
        ProgramPreset (1, BestPresets[TempIndex], PegFunc, TempIndex);
      }
    }
    for (TempIndex = 0; TempIndex < (PortInfoList[PortIndex]).LaneListLength; TempIndex++) {
      ///
      /// Get Coefficients
      ///
      GetCoefficientsFromPreset (BestPresets[((PortInfoList[PortIndex]).LaneList[TempIndex])], FullSwing, &PreCursor, &Cursor, &PostCursor);

      ///
      /// Set Lane's Coefficients
      ///
      SetPartnerTxCoefficients ((PortInfoList[PortIndex]).LaneList[TempIndex], &PreCursor, &Cursor, &PostCursor);

      ///
      /// Set Phase 1 Presets
      ///
      ProgramPreset (1, BestPresets[((PortInfoList[PortIndex]).LaneList[TempIndex])], PegFunc, (PortInfoList[PortIndex]).LaneList[TempIndex]);
    }

    MmPci32Or (0, PegBus, PegDev, PegFunc, R_SA_PEG_LCTL3_OFFSET, BIT0);   ///< DOEQ
    MmPci16Or (0, PegBus, PegDev, PegFunc, R_SA_PEG_LCTL_OFFSET,  BIT5);   ///< Retrain link
  }

  if (AnyMarginingNeeded) {
    StallPpi->Stall (PeiServices, StallPpi, 100 * STALL_ONE_MICRO_SECOND);
    for (TempIndex = 0; TempIndex < SA_PEG_MAX_FUN; TempIndex++) {
      MmPci16And (0, SA_PEG_BUS_NUM, SA_PEG_DEV_NUM, TempIndex, R_SA_PEG_LCTL_OFFSET, (UINT16) ~(BIT4));
    }
    DeassertPegSlotReset (SaPlatformPolicyPpi);
  }

  for (PortIndex = 0; PortIndex < PortInfoListLength; PortIndex++) {
    PegBus  = (PortInfoList[PortIndex]).PegPort.Bus;
    PegDev  = (PortInfoList[PortIndex]).PegPort.Device;
    PegFunc = (PortInfoList[PortIndex]).PegPort.Function;
    WaitForL0 (PeiServices, StallPpi, &(PortInfoList[PortIndex].PegPort), FALSE);
    ReportPcieLinkStatus (PegBus, PegDev, PegFunc);
  }

  DEBUG ((EFI_D_INFO, "PEG Gen3 Preset Search done\n\n"));

  return;
}

BOOLEAN
SaPolicyEnablesGen3 (
  IN SA_PLATFORM_POLICY_PPI *SaPlatformPolicyPpi
  )
{
  UINTN   PegPortGenx;
  BOOLEAN Gen3Enabled;
  UINT8   Index;

  Gen3Enabled = FALSE;

  ///
  /// Check if Gen3 is enabled on PEG10/11/12
  ///
  for (Index = 0; Index < SA_PEG_MAX_FUN; Index++) {
    ///
    /// PegPortGenx: 0 = Auto, 1 = Gen1, 2 = Gen2, 3 = Gen3
    ///
    PegPortGenx = SaPlatformPolicyPpi->PcieConfig->PegGenx[Index];

    ///
    /// Check if the root port is present and the speed is not limited to Gen1/Gen2
    ///
    if ((PegPortGenx == PEG_AUTO) || (PegPortGenx == PEG_GEN3)) {
      Gen3Enabled = TRUE;
      break;
    }
  }
  return Gen3Enabled;
}

#endif // PEG_FLAG
