/** @file

  Margining for PEG Training

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

///
/// Jitter Calculations
///
JITTER_SETTING Jitter_Gen3[/* 083.333 */] = {{1, 64, 1, 0}, {1, 33, 2, 0}, {3, 64, 3, 0}, {4, 80, 4, 0}, {5, 97, 5, 0}, {6, 128, 6, 0}, {7, 129, 7, 0}, {8, 161, 8, 0}, {9, 192, 9, 0}, {10, 193, 10, 0}, {11, 225, 11, 0}, {12, 128, 12, 1}, {13, 129, 13, 1}, {14, 145, 14, 1}, {15, 160, 15, 1}, {16, 161, 16, 1}, {17, 177, 17, 1}, {18, 192, 18, 1}, {19, 193, 19, 1}, {20, 209, 20, 1}, {21, 224, 21, 1}, {22, 225, 22, 1}, {23, 241, 23, 1}, {24, 246, 24, 1}};

EFI_STATUS
SetJitterTolerance (
  IN UINT8 *LaneList,
  IN UINT8 LaneListLength,
  IN UINT8 ReportedMargin
  )
{
  JITTER_SETTING Settings = {0, 0, 0, 0};
  UINT8         i;
  BOOLEAN       Found;

  if (ReportedMargin > 0) {
    Found = FALSE;
    for (i = 0; i < (JITTER_LENGTH - 1); ++i) {
      Settings = Jitter_Gen3[i];
      if (Settings.ReportedMargin == ReportedMargin) {
        Found = TRUE;
        break;
      }
    }

    if (Found) {
      return SetRawJitterTolerance (
                                      LaneList,
                                      LaneListLength,
                                      Settings.Step,
                                      Settings.Depth,
                                      Settings.DoubleMargin,
                                      TRUE
                                    );
    } else {
      return EFI_INVALID_PARAMETER;
    }
  } else {
    return SetRawJitterTolerance (
                                   LaneList,
                                   LaneListLength,
                                   Settings.Step,
                                   Settings.Depth,
                                   Settings.DoubleMargin,
                                   FALSE
                                 );
  }
}

EFI_STATUS
SetRawJitterTolerance (
  IN UINT8   *LaneList,
  IN UINT8   LaneListLength,
  IN UINT8   Step,
  IN UINT8   Depth,
  IN UINT8   DoubleMargin,
  IN BOOLEAN EnableJitter
  )
{
  UINT8 BundleList[8];
  UINT8 BundleListLength;
  UINT8 i;
  UINT8 Lane;
  UINT8 Bundle;

  BundleListLength = 0;

  GetBundleList (LaneList, LaneListLength, &BundleList[0], &BundleListLength);

  for (i = 0; i < LaneListLength; ++i) {
    Lane = LaneList[i];

    McD1PciCfg32And (R_SA_PEG_AFELN0CFG0_OFFSET + (Lane * LANE_STEP), 0xFFFFFBFF);
  }

  for (i = 0; i < BundleListLength; ++i) {
    Bundle = BundleList[i];

    McD1PciCfg32AndThenOr (
      R_SA_PEG_AFEBND0CFG0_OFFSET + (Bundle * BUNDLE_STEP),
      (UINT32) ~(BIT28 | BIT17 | BIT16 | BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10),
      (UINT32)  ((DoubleMargin << 28) | (Step << 10))
    );

    McD1PciCfg32AndThenOr (
      R_SA_PEG_AFEBND0CFG3_OFFSET + (Bundle * BUNDLE_STEP),
      (UINT32) ~(BIT16 | BIT15 | BIT14 | BIT13 | BIT12 | BIT11),
      (UINT32)  (Depth << 11)
    );
  }

  if (EnableJitter) {
    for (i = 0; i < LaneListLength; ++i) {
      Lane = LaneList[i];
      McD1PciCfg32Or (R_SA_PEG_AFELN0CFG0_OFFSET + (Lane * LANE_STEP), BIT10);
    }
  }

  return EFI_SUCCESS;
}

VOID
EnableTxJitterInjection (
  IN UINT8   Lane,
  IN BOOLEAN EnableTxJitter
  )
{
  UINT8 Bundle;

  Bundle = Lane / 2;

  if (EnableTxJitter) {
    McD1PciCfg32AndThenOr (
      R_SA_PEG_AFEBND0CFG0_OFFSET + (Bundle * BUNDLE_STEP),
      ~(BIT1 | BIT2),
      0x6
    );
  } else {
    McD1PciCfg32And (
      R_SA_PEG_AFEBND0CFG0_OFFSET + (Bundle * BUNDLE_STEP),
      ~(BIT1 | BIT2)
    );
  }
}


VOID
ConfigureTxJitterMux (
  IN UINT8  Lane,
  IN UINT32 MchBar
  )
{
  CPU_FAMILY        CpuFamilyId;
  CPU_STEPPING      CpuSteppingId;
  UINT32            SequenceId;

  CpuFamilyId   = GetCpuFamily ();
  CpuSteppingId = GetCpuStepping ();
  SequenceId    = V_SA_VCU_SEQID_SET_TXJITTER_MUX;

  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
    SendVcuApiCmd (MchBar, V_SA_VCU_OPCODE_OPEN_SEQ_REV1, SequenceId);
  } else {
    SendVcuApiCmd (MchBar, V_SA_VCU_OPCODE_OPEN_SEQ_REV2, SequenceId);
  }

  SendVcuApiCmd (MchBar, V_SA_VCU_OPCODE_SET_TXJITTER_MUX, Lane);

  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
    SendVcuApiCmd (MchBar, V_SA_VCU_OPCODE_CLOSE_SEQ_REV1, 0);
  } else {
    SendVcuApiCmd (MchBar, V_SA_VCU_OPCODE_CLOSE_SEQ_REV2, 0);
  }

  return;
}

EFI_STATUS
GetBundleList (
  IN  UINT8 *LaneList,
  IN  UINT8 LaneListLength,
  OUT UINT8 *BundleList,
  OUT UINT8 *BundleListLength
  )
{
  UINT8   Bundle;
  BOOLEAN HasBundle;
  UINT8   i;
  UINT8   j;

  (*BundleListLength) = 0;

  for (i = 0; i < LaneListLength; ++i) {
    Bundle = LaneList[i] / 2;

    HasBundle = FALSE;
    for (j = 0; j < (*BundleListLength); ++j) {
      if (BundleList[j] == Bundle) {
          HasBundle = TRUE;
          break;
      }
    }

    if (!HasBundle) {
      BundleList[*BundleListLength] = Bundle;
      ++(*BundleListLength);
    }
  }

  return EFI_SUCCESS;
}

#endif // PEG_FLAG
