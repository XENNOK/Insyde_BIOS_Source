/** @file

  Copies the memory related timing and configuration information into the
  Compatible BIOS data (BDAT) table.
  
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;* Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
;*
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/

#include <MrcTypes.h>
#include <MrcApi.h>
#include <MrcGlobal.h>
#include <MrcOem.h>
#include <MrcSpdProcessing.h>
#include <McAddress.h>

#ifdef BDAT_SUPPORT
#define TBD                       0
#define CRC_SEED                  0
#define CRC_XOR_MASK              0x1021
#define MAX_UINT8_VALUE           (((1UL) << CHAR_BITS) - 1)
#define HOST_BRIDGE_BUS           0
#define HOST_BRIDGE_DEVICE        0
#define HOST_BRIDGE_FUNCTION      0
#define HOST_BRIDGE_OFFSET_DEVID  0
#define HOST_BRIDGE_OFFSET_REVID  8
#define CopyMem                   MrcOemMemoryCpy
#define GetCrc16                  GetDimmCrc

typedef U8  UINT8;
typedef U16 UINT16;

/**
  @brief
  Finds the window value for the given DQ value and if it is less than the
    current value, then save the end point values.

    @param[in, out] Rank1         - Pointer to the first rank training value (left or low).
    @param[in, out] Rank2         - Pointer to the second rank training value (right or high).
    @param[in, out] CurrentWindow - The current window value.
    @param[in] Value1        - The first training value (left or low).
    @param[in] Value2        - The second training value (right or high).

    @retval Nothing.
**/
void
ConvertDq2Rank (
  IN OUT UINT8        *Rank1,
  IN OUT UINT8        *Rank2,
  IN OUT UINT8        *CurrentWindow,
  IN     const UINT8  Value1,
  IN     const UINT8  Value2
  )
{
  UINT8 Window;  // The calculated window value.

  Window = MAX (Value1, Value2) - MIN (Value1, Value2);
  if (Window < *CurrentWindow) {
    *CurrentWindow  = Window;
    *Rank1          = Value1;
    *Rank2          = Value2;
  } // if
  return;
}

/**
@brief
  Initialize the memory rank margin area of the RMT_STRUCTURE.

  @param[in] MrcData - The MRC "global data".
  @param[in, out] RmtRankMargin - Pointer to the start of the rank margin information in the RMT table.
  @param[in] RmtDq         - Pointer to the start of the dq margin information in the RMT table.

  @retval Nothing.
**/
void
MrcFillRmtRankMargin (
  IN     const MrcParameters  *MrcData,
  IN OUT RmtRankMargin        *RmtRankMargin,
  IN     RmtDqMargin          *RmtDq
  )
{
  RmtDqMargin *RmtDqMargin;         // Pointer to the current DQ margin in the RMT structure.
  UINT8       Dq;                   // DQ number in the rank.
  UINT8       DqEnd;
  UINT8       SmallestWindowRxDq;   // The smallest of the Rx DQ windows.
  UINT8       SmallestWindowTxDq;   // The smallest of the Tx DQ windows.
  UINT8       SmallestWindowRxVref; // The smallest of the Rx Vref windows.
  UINT8       SmallestWindowTxVref; // The smallest of the Tx Vref windows.

  SmallestWindowRxDq   = (UINT8) MAX_UINT8_VALUE;
  SmallestWindowTxDq   = (UINT8) MAX_UINT8_VALUE;
  SmallestWindowRxVref = (UINT8) MAX_UINT8_VALUE;
  SmallestWindowTxVref = (UINT8) MAX_UINT8_VALUE;
  DqEnd                = (MrcData->SysOut.Outputs.EccSupport) ? MAX_DQ : (MAX_DQ - MAX_BITS);
  for (Dq = 0; Dq < DqEnd; Dq++) {
    RmtDqMargin = &RmtDq[Dq];
    ConvertDq2Rank (
      &RmtRankMargin->RxDqLeft,
      &RmtRankMargin->RxDqRight,
      &SmallestWindowRxDq,
      RmtDqMargin->RxDqLeft,
      RmtDqMargin->RxDqRight
      );
    ConvertDq2Rank (
      &RmtRankMargin->TxDqLeft,
      &RmtRankMargin->TxDqRight,
      &SmallestWindowTxDq,
      RmtDqMargin->TxDqLeft,
      RmtDqMargin->TxDqRight
      );
    ConvertDq2Rank (
      &RmtRankMargin->RxVrefLow,
      &RmtRankMargin->RxVrefHigh,
      &SmallestWindowRxVref,
      RmtDqMargin->RxVrefLow,
      RmtDqMargin->RxVrefHigh
      );
    ConvertDq2Rank (
      &RmtRankMargin->TxVrefLow,
      &RmtRankMargin->TxVrefHigh,
      &SmallestWindowTxVref,
      RmtDqMargin->TxVrefLow,
      RmtDqMargin->TxVrefHigh
      );
  } // Dq loop
  return;
}

/**
@brief
  Initialize the memory DQ area of the RMT_STRUCTURE.

  @param[in, out] RmtDq   - Pointer to the start of the DQ information in the RMT table.
  @param[in] Channel - Specific Channel
  @param[in] Dimm    - Specific Dimm
  @param[in] Rank    - Specific Rank
  @param[in] MrcData - The MRC "global data".

  @retval Nothing.
**/
void
MrcFillRmtDqMargin (
  IN OUT RmtDqMargin          *RmtDq,
  IN     const UINT8          Controller,
  IN     const UINT8          Channel,
  IN     const UINT8          Dimm,
  IN     const UINT8          Rank,
  IN     const MrcParameters  *MrcData
  )
{
  const MrcOutput       *Outputs;
  const MrcChannelOut   *ChannelOut;
  const MrcDqTimeMargin *RxDqMargin;      // Pointer to the output portion of the MRC global data area.
  const MrcDqTimeMargin *TxDqMargin;      // Pointer to the output portion of the MRC global data area.
  const MrcDqVrefMargin *RxVrefDqMargin;  // Pointer to the output portion of the MRC global data area.
  const MrcDqVrefMargin *TxVrefDqMargin;  // Pointer to the output portion of the MRC global data area.
  RmtDqMargin           *RmtDqMargin;     // Pointer to the current DQ margin in the RMT structure.
  UINT8                 RankInChannel;
  UINT8                 Sdram;
  UINT8                 Dq;               // DQ number in the rank.
  UINT8                 DqEnd;
  UINT8                 Bit;

  Outputs       = &MrcData->SysOut.Outputs;
  ChannelOut    = &Outputs->Controller[Controller].Channel[Channel];
  RankInChannel = (Dimm * MAX_RANK_IN_DIMM) + Rank;
  DqEnd         = (Outputs->EccSupport) ? MAX_DQ : (MAX_DQ - MAX_BITS);
  for (Dq = 0; Dq < DqEnd; Dq++) {
    Sdram                   = Dq / CHAR_BITS;
    Bit                     = Dq % CHAR_BITS;
    RxDqMargin              = &ChannelOut->RxDqPb[RankInChannel][Sdram][Bit];
    TxDqMargin              = &ChannelOut->TxDqPb[RankInChannel][Sdram][Bit];
    RxVrefDqMargin          = &ChannelOut->RxDqVrefPb[RankInChannel][Sdram][Bit];
    TxVrefDqMargin          = &ChannelOut->TxDqVrefPb[RankInChannel][Sdram][Bit];
    RmtDqMargin             = &RmtDq[Dq];
    RmtDqMargin->RxDqLeft   = RxDqMargin->Left;
    RmtDqMargin->RxDqRight  = RxDqMargin->Right;
    RmtDqMargin->TxDqLeft   = TxDqMargin->Left;
    RmtDqMargin->TxDqRight  = TxDqMargin->Right;
    RmtDqMargin->RxVrefLow  = RxVrefDqMargin->Low;
    RmtDqMargin->RxVrefHigh = RxVrefDqMargin->High;
    RmtDqMargin->TxVrefLow  = TxVrefDqMargin->Low;
    RmtDqMargin->TxVrefHigh = TxVrefDqMargin->High;
  } // Dq loop
  return;
}

/**
@brief
  Initialize the memory rank training area of the RMT_STRUCTURE.

  @param[in, out] RmtRankTraining - Pointer to the start of the rank training information in the RMT table.
  @param[in] Channel         - Specific Channel
  @param[in] Dimm            - Specific Dimm
  @param[in] Rank            - Specific Rank
  @param[in] MrcData         - The MRC "global data".

  @retval Nothing.
**/
void
MrcFillRmtRankTraining (
  IN OUT RmtRankTraining      *RmtRankTraining,
  IN     const UINT8          Controller,
  IN     const UINT8          Channel,
  IN     const UINT8          Dimm,
  IN     const UINT8          Rank,
  IN     const MrcParameters  *MrcData
  )
{
  const MrcOutput     *Outputs;     // Pointer to the output portion of the MRC global data area.
  const MrcChannelOut *ChannelOut;  // Pointer to the channel portion of the MRC global data area.
  UINT8               Index;
  UINT8               RankInChannel;
  UINT8               Sdram;
  UINT8               Strobe;

  Outputs       = &MrcData->SysOut.Outputs;
  ChannelOut    = &Outputs->Controller[Controller].Channel[Channel];
  RankInChannel = (Dimm * MAX_RANK_IN_DIMM) + Rank;
  for (Strobe = 0; Strobe < MAX_STROBE; Strobe++) {
    Sdram = Strobe / 2;
    RmtRankTraining->RecEnDelay[Strobe] = ChannelOut->RcvEn[RankInChannel][Sdram];
    RmtRankTraining->WlDelay[Strobe]    = ChannelOut->WlDelay[RankInChannel][Sdram];
    RmtRankTraining->RxDqDelay[Strobe]  = (Strobe % 2)
                                        ? ChannelOut->RxDqsN[RankInChannel][Sdram]
                                        : ChannelOut->RxDqsP[RankInChannel][Sdram];
    RmtRankTraining->TxDqDelay[Strobe]  = ((U8) (ChannelOut->TxDq[RankInChannel][Sdram] >> 6)) & 7;
  } // Strobe loop
  RmtRankTraining->ClkDelay = ChannelOut->ClkPiCode[RankInChannel];
  RmtRankTraining->CtlDelay = ChannelOut->CtlPiCode[RankInChannel];
  for (Index = 0; Index < (sizeof (RmtRankTraining->CmdDelay) / sizeof (RmtRankTraining->CmdDelay[0])); Index++) {
    RmtRankTraining->CmdDelay[Index] = TBD;  // Need to implement code.
  } // Index loop
  RmtRankTraining->IoLatency = ChannelOut->IoLatency[RankInChannel];
  RmtRankTraining->Roundtrip = ChannelOut->RTLatency[RankInChannel];
  return;
}

/**
@brief
  Initialize the memory rank area of the RMT_STRUCTURE.

  @param[in, out] RmtRank  - Pointer to the start of the rank information in the RMT table.
  @param[in] MrcData  - The MRC "global data".
  @param[in] Channel  - Current channel number.
  @param[in] Dimm     - Current dimm number.

  @retval Nothing.
**/
void
MrcFillRmtRankStructure (
  IN OUT RmtRankList         *RmtRank,
  IN     const MrcParameters *MrcData,
  IN     UINT8               Controller,
  IN     UINT8               Channel,
  IN     UINT8               Dimm
)
{
  const MrcOutput     *Outputs;          // Pointer to the output portion of the MRC global data area.
  const MrcChannelOut *ChannelOut;       // Pointer to the DIMM output portion of the MRC global data area.
  const MrcDimmOut    *DimmOut;          // Pointer to the DIMM output portion of the MRC global data area.
  RmtRankList         *RmtRankLists;     // Pointer to the current rank list in the RMT structure.
  RmtRankTraining     *RmtRankTraining;  // Pointer to the current rank training in the RMT structure.
  UINT8               Rank;              // Rank count for sequencing.
  UINT8               RankInChannel;     // Rank number in a channel.

  Outputs    = &MrcData->SysOut.Outputs;
  ChannelOut = &Outputs->Controller[Controller].Channel[Channel];
  DimmOut    = &ChannelOut->Dimm[Dimm];
  for (Rank = 0; (Rank < MAX_RANK_IN_DIMM) && (Rank < DimmOut->RankInDIMM); Rank++) {
    RmtRankLists                    = &RmtRank[Rank];
    RmtRankTraining                 = &RmtRankLists->RankTraining;
    RankInChannel                   = (Dimm * MAX_RANK_IN_DIMM) + Rank;
    RmtRankLists->RankEnabled       = TRUE;
    RmtRankLists->RankMarginEnabled = TRUE;
    RmtRankLists->DqMarginEnabled   = TRUE;
    MrcFillRmtDqMargin (&RmtRankLists->DqMargin[0], Controller, Channel, Dimm, Rank, MrcData);
    MrcFillRmtRankMargin (MrcData, &RmtRankLists->RankMargin, &RmtRankLists->DqMargin[0]);
    MrcFillRmtRankTraining (&RmtRankLists->RankTraining, Controller, Channel, Dimm, Rank, MrcData);
    CopyMem (
      (UINT8 *) &RmtRankLists->RankMRS.ModeRegister[0],
      (UINT8 *) &DimmOut->Rank[Rank].MR[0],
      sizeof (RmtRankMrs)
      );
    RmtRankLists->RankMargin.CmdLeft      = ChannelOut->Command[RankInChannel].Left;
    RmtRankLists->RankMargin.CmdRight     = ChannelOut->Command[RankInChannel].Right;
    RmtRankLists->RankMargin.CmdVrefLow   = ChannelOut->Command[RankInChannel].Low;
    RmtRankLists->RankMargin.CmdVrefHigh  = ChannelOut->Command[RankInChannel].High;
    RmtRankLists->RankMargin.RecvenLeft   = ChannelOut->ReceiveEnable[RankInChannel].Left;
    RmtRankLists->RankMargin.RecvenRight  = ChannelOut->ReceiveEnable[RankInChannel].Right;
    RmtRankLists->RankMargin.WrLevelLeft  = ChannelOut->WriteLevel[RankInChannel].Left;
    RmtRankLists->RankMargin.WrLevelRight = ChannelOut->WriteLevel[RankInChannel].Right;
  }  // Rank loop
  return;
}

/**
@brief
  Initialize the memory dimm area of the RMT_STRUCTURE.

  @param[in, out] RmtDimm - Pointer to the start of the dimm information in the RMT table.
  @param[in] MrcData - The MRC "global data".
  @param[in] Channel - Current channel number.

  @retval Nothing.
**/
void
MrcFillRmtDimmStructure (
  IN OUT RmtDimmList         *RmtDimm,
  IN     const MrcParameters *MrcData,
  IN     UINT8               Controller,
  IN     UINT8               Channel
  )
{
  const MrcInput      *Inputs;              // Pointer to the input portion of the MRC global data area.
  const MrcDimmIn     *DimmIn;              // Pointer to the DIMM input portion of the MRC global data area.
  const MrcSpd        *SpdIn;               // Pointer to the SPD input portion of the MRC global data area.
  const MrcOutput     *Outputs;             // Pointer to the output portion of the MRC global data area.
  const MrcChannelOut *ChannelOut;          // Pointer to the channel output portion of the MRC global data area.
  const MrcDimmOut    *DimmOut;             // Pointer to the DIMM output portion of the MRC global data area.
  RmtDimmList         *RmtDimmList;         // Pointer to the current DIMM in the RMT structure.
  RmtSpd              *RmtSpdList;          // Pointer to the current SPD in the RMT structure.
  UINT8               Dimm;                 // Dimm count for sequencing.

  Inputs     = &MrcData->SysIn.Inputs;
  Outputs    = &MrcData->SysOut.Outputs;
  ChannelOut = &Outputs->Controller[Controller].Channel[Channel];
  for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
    DimmOut = &ChannelOut->Dimm[Dimm];
    if (DimmOut->Status == DIMM_PRESENT) {
      RmtDimmList               = &RmtDimm[Dimm];
      RmtSpdList                = &RmtDimmList->SpdBytes;
      DimmIn                    = &Inputs->Controller[Controller].Channel[Channel].Dimm[Dimm];
      SpdIn                     = &DimmIn->Spd;
      RmtDimmList->DimmEnabled  = TRUE;
      CopyMem ((UINT8 *) &RmtSpdList->SpdData[0], (UINT8 *) SpdIn, 128);
#if (defined SUPPORT_XMP && SUPPORT_XMP == SUPPORT)
      CopyMem (&RmtSpdList->SpdData[128], ((UINT8 *) SpdIn) + 128, 128);
#endif // (defined SUPPORT_XMP && SUPPORT_XMP == SUPPORT)
      CopyMem ((UINT8 *) &RmtSpdList->SpdValid, (UINT8 *) &DimmIn->SpdValid, sizeof (RmtDimmList->SpdBytes.SpdValid));

      //
      // Initialize the memory rank area of the RMT_STRUCTURE.
      //
      MrcFillRmtRankStructure (&RmtDimmList->RankList[0], MrcData, Controller, Channel, Dimm);
    } // end if
  } // Dimm loop
  return;
}

/**
@brief
  Initialize the memory channel area of the RMT_STRUCTURE.

  @param[in, out] RmtChannel - Pointer to the start of the channel information in the RMT table.
  @param[in] MrcData     - The MRC "global data".

  @retval Nothing.
**/
void
MrcFillRmtChannelStructure (
  IN OUT RmtChannelList      *RmtChannel,
  IN     const MrcParameters *MrcData,
  IN     const UINT8         Controller
  )
{
  const MrcOutput *Outputs;             // Pointer to the output portion of the MRC global data area.
  RmtChannelList  *RmtChannelList;      // Pointer to the current channel in the RMT structure.
  UINT8           Channel;              // Channel count for sequencing.

  Outputs = &MrcData->SysOut.Outputs;
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (Outputs->Controller[Controller].Channel[Channel].Status == CHANNEL_PRESENT) {
      RmtChannelList                  = &RmtChannel[Channel];
      RmtChannelList->ChannelEnabled  = TRUE;
      RmtChannelList->NumDimmSlot     = MAX_DIMMS_IN_CHANNEL;

      //
      // Initialize the memory DIMM area of the RMT_STRUCTURE.
      //
      MrcFillRmtDimmStructure (&RmtChannelList->DimmList[0], MrcData, Controller, Channel);
    } // end if
  } // Channel loop
  return;
}

/**
@brief
  Initialize the memory controller area of the RMT_STRUCTURE.

  @param[in, out] RmtController - Pointer to the start of the controller information in the RMT table.
  @param[in] MrcData        - The MRC "global data".

  @retval Nothing.
**/
void
MrcFillRmtControllerStructure (
  IN OUT RmtControllerList   *RmtController,
  IN     const MrcParameters *MrcData
  )
{
  const MrcInput    *Inputs;
  const MrcOutput   *Outputs;
  RmtControllerList *RmtControllerList;
  UINT8             Controller;
  MrcVddSelect      VddVoltage;

  Inputs     = &MrcData->SysIn.Inputs;
  Outputs    = &MrcData->SysOut.Outputs;
  VddVoltage = Outputs->VddVoltage[Inputs->MemoryProfile];
  for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
    if (Outputs->Controller[Controller].Status == CONTROLLER_PRESENT) {
      RmtControllerList                       = &RmtController[Controller];
      RmtControllerList->ControllerEnabled    = TRUE;
      RmtControllerList->ControllerDeviceId   = Outputs->Controller[Controller].DeviceId;
      RmtControllerList->ControllerRevisionId = Outputs->Controller[Controller].RevisionId;
      RmtControllerList->MemoryFrequency      = (UINT16) (Outputs->Frequency / 10);
      RmtControllerList->MemoryVoltage        = (UINT16) VddVoltage;
      //
      // Step unit = piStep * (tCK / 2048)
      //
      RmtControllerList->PiStep               = (UINT8) PI_STEP;
      RmtControllerList->RecvenStep           = (UINT8) PI_STEP;
      RmtControllerList->WrLevelStep          = (UINT8) PI_STEP;
      if (VddVoltage > 0) {
        //
        // Step unit = __VrefStep * Vdd / 100
        //
        RmtControllerList->RxVrefStep  = (UINT16) RX_VREF (VddVoltage);
        RmtControllerList->TxVrefStep  = (UINT16) TX_VREF (VddVoltage);
        RmtControllerList->CaVrefStep  = (UINT16) CA_VREF (VddVoltage);
      } else {
        RmtControllerList->RxVrefStep  = 0;
        RmtControllerList->TxVrefStep  = 0;
        RmtControllerList->CaVrefStep  = 0;
      }
      //
      // Initialize the memory channel area of the RMT_STRUCTURE.
      //
      MrcFillRmtChannelStructure (&RmtControllerList->ChannelList[0], MrcData, Controller);
    } // Controller loop
  }

  return;
}

/**
@brief
  Fill the compatible data structure RMT with the information provided by
  the memory initialization code.

  @param[in, out] MrcData - Constant pointer to the Mrc data structure which conatins the Rmt structure to fill.

  @retval Nothing.
**/
MrcStatus
MrcFillRmtStructure (
  IN OUT MrcParameters *const MrcData
  )
{
  const UINT8       RmtHeaderSign[] = {'B', 'D', 'A', 'T', 'H', 'E', 'A', 'D'};
  const MrcVersion  *Version;             // Pointer to the output portion of the MRC global data area.
  RmtData           *Rmt;
  RmtHeader         *RmtHeader;           // Pointer to the header data area in the RMT structure.
  RmtSystem         *RmtSystem;           // Pointer to the system data area in the RMT structure.
  const MrcInput    *Inputs;
  MrcOutput         *Outputs;

  Inputs  = &MrcData->SysIn.Inputs;
  Outputs = &MrcData->SysOut.Outputs;
  if (Inputs->RmtBdatEnable) {
    //
    // Initialize the header area of the RMT_STRUCTURE.
    //
    Rmt       = &MrcData->Rmt;
    RmtHeader = &Rmt->RmtHeader;
    CopyMem (&RmtHeader->BiosDataSignature[0], (UINT8 *) RmtHeaderSign, sizeof (RmtHeader->BiosDataSignature));
    RmtHeader->BiosDataStructSize           = sizeof (RmtData);
    RmtHeader->Version.S[PRIMARY_OFFSET]    = RMT_PRIMARY_VERSION;
    RmtHeader->Version.S[SECONDARY_OFFSET]  = RMT_SECONDARY_VERSION;
    RmtHeader->OemOffset                    = OEM_OFFSET;
    RmtHeader->Reserved1 = (Inputs->BaseTime.Hours << 16) | (Inputs->BaseTime.Minutes << 8) | Inputs->BaseTime.Seconds;
    RmtHeader->Reserved2 = (Inputs->BaseTime.Year << 16) | (Inputs->BaseTime.Month << 8) | Inputs->BaseTime.DayOfMonth;

    //
    // Initialize the system area of the RMT_STRUCTURE.
    //
    Version                               = &Outputs->Version;
    RmtSystem                             = &Rmt->RmtSystem;
    RmtSystem->RefCodeRevision.c.Major    = Version->Major;
    RmtSystem->RefCodeRevision.c.Minor    = Version->Minor;
    RmtSystem->RefCodeRevision.c.Revision = Version->Rev;
    RmtSystem->RefCodeRevision.c.Build    = Version->Build;
    RmtSystem->MaxController              = MAX_CONTROLLERS;
    RmtSystem->MaxChannel                 = MAX_CHANNEL;
    RmtSystem->MaxDimm                    = MAX_DIMMS_IN_CHANNEL;
    RmtSystem->MaxRankDimm                = MAX_RANK_IN_DIMM;
    RmtSystem->MaxStrobe                  = MAX_STROBE;
    RmtSystem->MaxDq                      = MAX_DQ;
    RmtSystem->MarginLoopCount            = Outputs->DQPatLC;
    //
    // Initialize the memory controller area of the RMT_STRUCTURE.
    //
    MrcFillRmtControllerStructure (&RmtSystem->ControllerList[0], MrcData);

    //
    // Initialize the CRC of the RMT_STRUCTURE.
    // Ensure that the CRC calculation is the last field initialized.
    //
    GetCrc16 ((const UINT8 *const) Rmt, sizeof (RmtData), &Rmt->RmtHeader.Crc16);
    MrcOemMmioWrite (NCDECS_CR_SCRATCHPAD_NCU_2_REG, (U32) Rmt, Inputs->MchBarBaseAddress);
  } // end if
  return mrcSuccess;
}
#endif // BDAT_SUPPORT
