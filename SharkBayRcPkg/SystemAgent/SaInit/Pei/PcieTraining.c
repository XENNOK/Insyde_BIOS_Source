/** @file
  This driver trains the PEG interface.

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

#include "PcieTraining.h"

#ifdef PEG_FLAG

UINT16
GetErrorTarget (
  IN  SA_PLATFORM_POLICY_PPI *SaPlatformPolicyPpi
  )
{
  UINT16  ErrorTarget;

  if ((SaPlatformPolicyPpi->Revision >= SA_PLATFORM_POLICY_PPI_REVISION_3) &&
      (SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearchErrorTarget >= 1)) {
    ErrorTarget = SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearchErrorTarget;
  } else {
    ErrorTarget = 4;
  }

  return ErrorTarget;
}

VOID
GetPortInfo (
  OUT PORT_INFO *PortInfoList,
  OUT UINT8     *PortInfoListLength,
  OUT BOOLEAN   *SkipBundle0
  )
{
  UINT32            HwStrap;
  UINT8             PegBus;
  UINT8             PegDev;
  UINT8             PcieController;
  UINT8             Index;
  UINT8             Lane;
  UINT8             LaneIndex;
  UINT8             FurcationSetup[SA_PEG_MAX_FUN];
  UINT8             PcieControllerList[SA_PEG_MAX_FUN];
  UINT8             NumberToCheck;
  UINT8             StartLane;
  UINT8             Width;
  UINT32            Lcap;
  UINT32            CapOffset;
  CPU_FAMILY        CpuFamilyId;
  CPU_STEPPING      CpuSteppingId;

  PegBus        = SA_MC_BUS;
  PegDev        = 1;
  HwStrap       = (McD1PciCfg32(R_SA_PEG_FUSESCMN_OFFSET) >> 16) & 0x3;
  CpuFamilyId   = GetCpuFamily();
  CpuSteppingId = GetCpuStepping();

  switch(HwStrap) {
    case SA_PEG_x8_x4_x4:
      FurcationSetup[0] = 8;
      FurcationSetup[1] = 4;
      FurcationSetup[2] = 4;
      NumberToCheck     = 3;
      break;
    case SA_PEG_x8_x8_x0:
      FurcationSetup[0] = 8;
      FurcationSetup[1] = 8;
      NumberToCheck     = 2;
      break;
    default:
    case SA_PEG_x16_x0_x0:
      FurcationSetup[0] = 16;
      NumberToCheck     = 1;
      break;
  }

  ///
  /// Figure out which PcieControllers are enabled
  ///
  (*PortInfoListLength) = 0;
  for (PcieController = 0; PcieController < NumberToCheck; PcieController++) {
    ///
    /// Sanity check to make sure width > 0
    ///
    if (FurcationSetup[PcieController] == 0) {
      continue;
    }

    ///
    /// Check to make sure the Root Port Exists
    ///
    if (MmPci16 (0, PegBus, PegDev, PcieController, PCI_VID) == 0xFFFF) {
      continue;
    }

    ///
    /// Add the PcieController to the list of enabled controllers
    ///
    PcieControllerList[(*PortInfoListLength)] = PcieController;
    (*PortInfoListLength)++;
  }

  ///
  /// If needed, skip Bundle 0's preset search and use Bundle 1's preset instead.
  ///
  (*SkipBundle0) = FALSE;
  if (((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId < EnumHswC0)) ||
      ((CpuFamilyId == EnumCpuCrw) && (CpuSteppingId < EnumCrwC0))) {
    (*SkipBundle0) = TRUE;
  }
  if ((*SkipBundle0)) {
    DEBUG ((DEBUG_INFO, "Skipping each controller's Lane 0-1 preset searches; using their Lane 2 preset instead.\n"));
  }

  StartLane = 0;
  for (Index = 0; Index < (*PortInfoListLength); Index++) {
    PcieController = PcieControllerList[Index];

    ///
    /// Get information for the current port
    ///
    (PortInfoList[Index]).EnableMargin                 = TRUE;
    (PortInfoList[Index]).FoundUsablePreset            = FALSE;
    (PortInfoList[Index]).PegPort.Bus                  = PegBus;
    (PortInfoList[Index]).PegPort.Device               = PegDev;
    (PortInfoList[Index]).PegPort.Function             = PcieController;
    (PortInfoList[Index]).PegPort.Index                = PcieController;
    (PortInfoList[Index]).PegPort.EndpointMaxLinkSpeed = 0;
    ReportPcieLinkStatus (PegBus, PegDev, PcieController);
    Width                                              = GetNegotiatedWidth(&((PortInfoList[Index]).PegPort));
    (PortInfoList[Index]).LaneListLength               = Width;
    for (Lane = 0, LaneIndex = 0; Lane < Width; Lane++) {
      if ((*SkipBundle0)) {
        if ((Lane == 0) || (Lane == 1)) {
          (PortInfoList[Index]).LaneListLength--;
          continue;
        }
      }
      if (LaneIndex < SA_PEG_MAX_LANE) {
        (PortInfoList[Index]).LaneList[LaneIndex] = (Lane + StartLane);
      }
      LaneIndex++;
    }

    ///
    /// Check that both root port and endpoint support Gen3
    ///
    Lcap = MmPci32 (0, PegBus, PegDev, PcieController, R_SA_PEG_LCAP_OFFSET);
    if ((Lcap & 0x0F) != 3) {
      DEBUG ((DEBUG_INFO, " PEG%x%x (%x:%x:%x) - Root Port is not Gen3-capable. Max Link Speed = %d.\n",
        PegDev, PcieController, PegBus, PegDev, PcieController, Lcap & 0x0F));
      (PortInfoList[Index]).LinkIsGen3Capable = FALSE;
    } else {

      DEBUG ((DEBUG_INFO, " PEG%x%x (%x:%x:%x) - Root Port is Gen3-capable.\n",
        PegDev, PcieController, PegBus, PegDev, PcieController));

      ///
      /// Set PEG PortBus = 1 to Read Endpoint.
      ///
      MmPci32AndThenOr (0, PegBus, PegDev, PcieController, PCI_PBUS, 0xFF0000FF, 0x00010100);

      ///
      /// A config write is required in order for the device to re-capture the Bus number,
      /// according to PCI Express Base Specification, 2.2.6.2
      /// Write to a read-only register VendorID to not cause any side effects.
      ///
      MmPci16 (0, 1, 0, 0, PCI_VID) = 0;

      ///
      /// Save end point vendor id and device id
      ///
      (PortInfoList[Index]).EndPointVendorIdDeviceId = MmPci32 (0, 1, 0, 0, 0);

      ///
      /// Negotiation Done?
      ///
      if ((MmPci16 (0, PegBus, PegDev, PcieController, R_SA_PEG_VC0RSTS_OFFSET) & BIT1) != 0) {
        (PortInfoList[Index]).LinkIsGen3Capable = FALSE;
        DEBUG ((DEBUG_INFO, " PEG%x%x (%x:%x:%x) - VC0 negotiation is pending! Skipping endpoint.\n",
          PegDev, PcieController, PegBus, PegDev, PcieController, Lcap & 0x0F));
        ReportPcieLinkStatus (PegBus, PegDev, PcieController);
      } else {
        ///
        /// Get the pointer to the Port PCI Express Capability Structure.
        ///
        CapOffset = PcieFindCapId (1, 0, 0, EFI_PCI_CAPABILITY_ID_PCIEXP);
        if (CapOffset == 0) {
          (PortInfoList[Index]).LinkIsGen3Capable = FALSE;
          DEBUG ((DEBUG_INFO, " PEG%x%x (%x:%x:%x) - Endpoint is not Gen3-capable. No PCIe Capability found.\n",
            PegDev, PcieController, PegBus, PegDev, PcieController, Lcap & 0x0F));
        } else {
          Lcap = MmPci32 (0, 1, 0, 0, CapOffset + 0x0C);
          (PortInfoList[Index]).PegPort.EndpointMaxLinkSpeed = Lcap & 0x0F;
          if ((Lcap & 0x0F) < 3) {
            (PortInfoList[Index]).LinkIsGen3Capable = FALSE;
            DEBUG ((DEBUG_INFO, " PEG%x%x (%x:%x:%x) - Endpoint is not Gen3-capable. Max Link Speed = %d.\n",
              PegDev, PcieController, PegBus, PegDev, PcieController, Lcap & 0x0F));
          } else {
            (PortInfoList[Index]).LinkIsGen3Capable = TRUE;
            DEBUG ((DEBUG_INFO, " PEG%x%x (%x:%x:%x) - Endpoint is Gen3-capable\n",
              PegDev, PcieController, PegBus, PegDev, PcieController, Lcap & 0x0F));
          }
        }
      }
      ///
      /// Restore bus numbers on the PEG bridge.
      ///
      MmPci32And (0, PegBus, PegDev, PcieController, PCI_PBUS, 0xFF0000FF);
    }

    StartLane += FurcationSetup[PcieController];
  } ///< End of for each port

  return;
}

EFI_STATUS
RunMarginTest (
  IN CONST EFI_PEI_SERVICES   **PeiServices,
  IN  SA_PLATFORM_POLICY_PPI  *SaPlatformPolicyPpi,
  IN  SA_DATA_HOB             *SaDataHob,
  IN       EFI_PEI_STALL_PPI  *StallPpi,
  IN  UINT32                  MonitorPort,
  IN  PORT_INFO               *PortInfoList,
  IN  UINT8                   PortInfoListLength,
  IN  MARGIN_TEST_TYPE        MarginTest,
  OUT INT32                   *Margins
  )
{
  EFI_STATUS Status;
  UINT8      *LaneList;
  UINT8      Lane;
  UINT8      PortListIndex;
  UINT8      OriginalSpeed;
  UINT8      OriginalWidth;
  UINT8      LaneListLength;
  PEG_PORT   *PegPort;

  Status = EFI_SUCCESS;

  ///
  /// Initialize Margins to -1.  Since -1 is an invalid value, we know that lane wasn't tested if its margin == -1
  ///
  for (Lane = 0; Lane < SA_PEG_MAX_LANE; Lane++) {
    Margins[Lane] = -1;
  }

  for (PortListIndex = 0; PortListIndex < PortInfoListLength; PortListIndex++) {
    ///
    /// Test all lanes associated with this the current port
    ///
    LaneList       = &((PortInfoList[PortListIndex]).LaneList[0]);
    LaneListLength =   (PortInfoList[PortListIndex]).LaneListLength;
    PegPort        = &((PortInfoList[PortListIndex]).PegPort);
    if ((PortInfoList[PortListIndex]).EnableMargin == FALSE ||
        (PortInfoList[PortListIndex]).SkipMargin   == TRUE) {
      continue;
    }

    if ((PortInfoList[PortListIndex]).LinkIsGen3Capable) {
      OriginalSpeed = 3;
    } else {
      OriginalSpeed = GetLinkSpeed (PegPort);
    }
    OriginalWidth = GetNegotiatedWidth (PegPort);

    switch (MarginTest) {
      case LaneLevelRxJitter:
        Status = LaneLevelJitterTest (
                 PeiServices,
                 SaPlatformPolicyPpi,
                 SaDataHob,
                 StallPpi,
                 MonitorPort,
                 LaneList,
                 LaneListLength,
                 PegPort,
                 OriginalSpeed,
                 OriginalWidth,
                 FALSE,
                 Margins
                 );
        break;
      default:
        DEBUG ((DEBUG_WARN, "Invalid Margin Test Requested.\n"));
        break;
    }
  }

  return Status;
}

EFI_STATUS
LaneLevelJitterTest (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN  SA_PLATFORM_POLICY_PPI *SaPlatformPolicyPpi,
  IN  SA_DATA_HOB            *SaDataHob,
  IN       EFI_PEI_STALL_PPI *StallPpi,
  IN  UINT32                 MonitorPort,
  IN  UINT8                  *LaneList,
  IN  UINT8                  LaneListLength,
  IN  PEG_PORT               *PegPort,
  IN  UINT8                  OriginalLinkSpeed,
  IN  UINT8                  OriginalLinkWidth,
  IN  BOOLEAN                TxJitterTest,
  OUT INT32                  *Margins
  )
{
  EFI_STATUS Status;
  UINT8     LaneListIndex;
  INT8      Jitter;
  UINT32    Errors;
  UINT32    PreviousErrors;
  UINT8     Lane;
  UINT32    RecoveryCount;
  BOOLEAN   AbortMargin;
  UINT16    ErrorTarget;
  UINT8     ConvergenceCounter;
  INT32     LastMargin;
  INT32     MarginDifference;
  INT32     StartJitter;
  INT8      MarginDirection;
  UINT8     RepeatCount;

  ErrorTarget = GetErrorTarget (SaPlatformPolicyPpi);

  for (LaneListIndex = 0; LaneListIndex < LaneListLength; LaneListIndex++) {
    Lane            = LaneList[LaneListIndex];
    Errors          = 0;
    AbortMargin     = FALSE;
    Margins[Lane]   = 0;
    MarginDirection = 1;

    if (TxJitterTest) {
      ConfigureTxJitterMux (Lane, SaPlatformPolicyPpi->PlatformData->MchBar);
      EnableTxJitterInjection (Lane, TRUE);
    }

    ///
    /// Determine value to start at
    ///
    if (LaneListIndex == 0) {
      StartJitter = 0;
    } else {
      StartJitter = (Margins[LaneList[LaneListIndex - 1]] / 100) - JITTER_MARGIN_INITIAL_OFFSET;
      if (StartJitter < 0) {
        StartJitter = 0;
      }
    }

    ConvergenceCounter = 0;
    LastMargin         = -1;
    RepeatCount        = 0;
    while (ConvergenceCounter < MARGIN_CONVERGANCE_MIN_MATCHES && RepeatCount < MARGIN_CONVERGANCE_MAX_REPEATS) {
      RepeatCount++;
      ///
      /// Determine whether to go up or down from starting point
      ///
      AbortMargin   = FALSE;
      RecoveryCount = SaPcieGetErrorCount (MonitorPort, PegPort->Function);
      Status        = SetJitterTolerance (&Lane, 1, (UINT8) StartJitter);
      ASSERT_EFI_ERROR (Status);

      Errors = SaPciePointTest (PeiServices, SaPlatformPolicyPpi, StallPpi, MonitorPort, PegPort, RecoveryCount);
      if (Errors >= ErrorTarget) {
        if (StartJitter == 0) {
          Margins[Lane] = 0;
          AbortMargin   = TRUE;
        } else {
          MarginDirection = -1;
        }
      } else {
        MarginDirection = 1;
      }
      for (Jitter =  (INT8) (StartJitter + MarginDirection);
           Jitter <  JITTER_LENGTH &&
           Jitter >= 0             &&
           (!AbortMargin);
           Jitter = (INT8) (Jitter + MarginDirection)) {
        ///
        /// Check for a link downgrade
        ///
        AbortMargin = LinkIsDowngraded (PegPort, OriginalLinkSpeed, OriginalLinkWidth);
        if (AbortMargin) {
          if (MarginDirection < 0) {
            LastMargin         = -1;
            ConvergenceCounter = 0;
            StartJitter        = 0;
            MarginDirection    = 1;
          } else {
            if (Jitter == 0) {
              Margins[Lane] = 0;
            } else {
              Margins[Lane] = (Jitter - 1) * 100;
            }
          }
          break;
        }

        ///
        /// Get initial recovery count
        ///
        RecoveryCount = SaPcieGetErrorCount (MonitorPort, PegPort->Function);
        Status        = SetJitterTolerance (&Lane, 1, Jitter);
        ASSERT_EFI_ERROR (Status);

        PreviousErrors = Errors;
        Errors         = SaPciePointTest (PeiServices, SaPlatformPolicyPpi, StallPpi, MonitorPort, PegPort, RecoveryCount);
        if (MarginDirection < 0) {
          if (Errors < ErrorTarget) { ///< Downward direction has started passing
            Margins[Lane] = InterpolateMargin (ErrorTarget, PreviousErrors, Errors, (INT32) Jitter);
            break;
          }
        } else {
          if (Errors >= ErrorTarget) { ///< Upward direction has started failing
            Margins[Lane] = InterpolateMargin (ErrorTarget, Errors, PreviousErrors, (INT32) Jitter);
            break;
          }
        }
      } ///< End of for loop

      ///
      /// Check if we never reached the error target
      ///
      if (MarginDirection < 0) {
        if ((Errors >= ErrorTarget) && (!AbortMargin)) {
          Margins[Lane] = 0;
        }
      } else {
        if ((Errors < ErrorTarget) && (!AbortMargin)) {
          Margins[Lane] = JITTER_LENGTH * 100;
        }
      }

      ///
      /// Compute the next margin point to start at
      ///
      StartJitter = (Margins[Lane] / 100) - JITTER_MARGIN_INITIAL_OFFSET;
      if (StartJitter < 0) {
        StartJitter = 0;
      }

      ///
      /// Check for convergance
      ///
      if (LastMargin == -1) {
        LastMargin = Margins[Lane];
      } else {
        MarginDifference = CalculateMarginDifference (LastMargin, Margins[Lane]);
        if (MarginDifference <= MARGIN_CONVERGANCE_ALLOWED_DELTA) {
          ConvergenceCounter++;
        } else {
          ConvergenceCounter = 0;
        }
        LastMargin = Margins[Lane];
      }

      if (LinkIsDowngraded (PegPort, OriginalLinkSpeed, OriginalLinkWidth)) {
        Status = SetJitterTolerance (&Lane, 1, 0);
        ASSERT_EFI_ERROR (Status);
      }

      ///
      /// If the link degraded in any way, bring it back to functional state
      ///
      Status = EnsureLinkIsHealthy (PeiServices, SaPlatformPolicyPpi, SaDataHob, StallPpi, PegPort, OriginalLinkSpeed, OriginalLinkWidth);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    } ///< End of repeat while loop

    ///
    /// Remove Jitter in preparation for testing the next lane
    ///
    Status = SetJitterTolerance (&Lane, 1, 0);
    ASSERT_EFI_ERROR (Status);
    if (TxJitterTest) {
      EnableTxJitterInjection (Lane, FALSE);
    }
  } ///< End of for each lane loop

  return EFI_SUCCESS;
}

UINT32
SaPciePointTest (
  IN CONST EFI_PEI_SERVICES **PeiServices,
  IN SA_PLATFORM_POLICY_PPI *SaPlatformPolicyPpi,
  IN EFI_PEI_STALL_PPI      *StallPpi,
  IN UINT32                 MonitorPort,
  IN PEG_PORT               *PegPort,
  IN UINT32                 InitialRecoveryCount
  )
{
  UINT32  Data32;

  StallPpi->Stall (PeiServices, StallPpi, SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearchDwellTime);
  Data32 = SaPcieGetErrorCount (MonitorPort, PegPort->Function) - InitialRecoveryCount;

  return Data32;
}

INT32
CalculateMarginDifference (
  IN INT32                   Margin1,
  IN INT32                   Margin2
  )
{
  if (Margin1 < Margin2) {
    return Margin2 - Margin1;
  } else {
    return Margin1 - Margin2;
  }
}

INT32
InterpolateMargin (
  IN UINT32 ErrorTarget,
  IN UINT32 CurrentErrorCount,
  IN UINT32 PreviousErrorCount,
  IN INT32  FailingPoint
  )
{
  UINT32 LnErrorTarget;
  UINT32 LnCurrentErrorCount;
  UINT32 LnPreviousErrorCount;
  INT32  Margin;

  if (ErrorTarget > 40000) {
    ErrorTarget = 40000;
  }
  if (CurrentErrorCount > 40000) {
    CurrentErrorCount = 40000;
  }
  if (PreviousErrorCount > 40000) {
    PreviousErrorCount = 40000;
  }

  LnErrorTarget        = NaturalLog (ErrorTarget * 100);
  LnCurrentErrorCount  = NaturalLog (CurrentErrorCount * 100);
  LnPreviousErrorCount = NaturalLog (PreviousErrorCount * 100);

  if (FailingPoint >= 0) {
    if ((LnCurrentErrorCount - LnPreviousErrorCount) == 0) {
      Margin = (FailingPoint - 1) * 100;
    } else {
      Margin = ((LnErrorTarget - LnPreviousErrorCount) * 100) /
               (LnCurrentErrorCount - LnPreviousErrorCount)   +
               ((FailingPoint - 1) * 100);
    }
    if (Margin < 0) {
      Margin = 0;
    }
  } else {
    if ((LnCurrentErrorCount - LnPreviousErrorCount) == 0) {
      Margin = (FailingPoint + 1) * 100;
    } else {
      Margin = ((FailingPoint + 1) * 100)                     -
               ((LnErrorTarget - LnPreviousErrorCount) * 100) /
               (LnCurrentErrorCount - LnPreviousErrorCount);
    }
    if (Margin > 0) {
      Margin = 0;
    }
  }

  return Margin;
}


UINT32
NaturalLog (
  IN UINT32 Input
  )
  /*++

  Routine Description:

    This function calculates the Natural Log of the Input parameter using integers

  Arguments:

    Input           - 100 times a number to get the Natural log from.
                    - Max Input Number is 40,000 (without 100x)

  Returns:

    Output          - 100 times the actual result. Accurate within +/- 2

  --*/
{
  UINT32  Output;

  ///
  ///Special case - treat 0 recoveries as 1 recovery for interpolation purposes
  ///
  if (Input == 0) {
    return 0;
  }

  Output = 0;
  while (Input > 271) {
    Input   = (Input * 1000) / 2718;
    Output += 100;
  }

  Output += ((-16 * Input * Input + 11578 * Input - 978860) / 10000);

  return Output;
}

#endif // PEG_FLAG
