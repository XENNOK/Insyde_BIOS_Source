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

#ifndef _MRC_RMT_DATA_H_
#define _MRC_RMT_DATA_H_

#include <MrcTypes.h>
#include <MrcApi.h>

#define VDD_1_350             1350                      ///< VDD in millivolts
#define VDD_1_500             1500                      ///< VDD in millivolts
#define PI_STEP_BASE          2048                      ///< Magic number from spec
#define PI_STEP_INTERVAL      128                       ///< tCK is split into this amount of intervals
#define PI_STEP               ((PI_STEP_BASE) / (PI_STEP_INTERVAL))
#define VREF_STEP_BASE        100                       ///< Magic number from spec
#define TX_VREF_STEP          7800                      ///< TX Vref step in microvolts
#define TX_VREF(VDD)          (((TX_VREF_STEP) * (VREF_STEP_BASE)) / (VDD)) ///< VDD passed in is in millivolts
#define RX_VREF_STEP          8000                      ///< TX Vref step in microvolts
#define RX_VREF(VDD)          (((RX_VREF_STEP) * (VREF_STEP_BASE)) / (VDD)) ///< VDD passed in is in millivolts
#define CA_VREF_STEP          8000                      ///< TX Vref step in microvolts
#define CA_VREF(VDD)          (((CA_VREF_STEP) * (VREF_STEP_BASE)) / (VDD)) ///< VDD passed in is in millivolts
#define PRIMARY_OFFSET        0                         ///< Offset into the BDAT header version.
#define SECONDARY_OFFSET      1                         ///< Offset into the BDAT header version.
#define RMT_PRIMARY_VERSION   3                         ///< The BDAT structure that is currently supported.
#define RMT_SECONDARY_VERSION 0                         ///< The BDAT structure that is currently supported.
#define OEM_OFFSET            0                         ///< The current offset to the OEM data in the BDAT structure.
#define MAX_SPD_RMT           256                       ///< The maximum amount of data, in bytes, in an SPD structure.

#pragma pack(push, 1)

typedef struct {
  U8                          BiosDataSignature[CHAR_BITS * sizeof (U8)]; ///< "BDATHEAD"
  U32                         BiosDataStructSize;       ///< sizeof BDAT_STRUCTURE
  U16                         Crc16;                    ///< 16-bit CRC of BDAT_STRUCTURE (calculated with 0 in this field)
  U16                         Reserved;
  union {
    U32                       L;
    U16                       S[2];
  }                           Version;                  ///< Version, primary then secondary.
  U32                         OemOffset;                ///< Optional offset to OEM-defined structure
  U32                         Reserved1;
  U32                         Reserved2;
} RmtHeader;

typedef struct {
  U8                          RxDqLeft;                 ///< Units = piStep
  U8                          RxDqRight;
  U8                          TxDqLeft;
  U8                          TxDqRight;
  U8                          RxVrefLow;                ///< Units = rxVrefStep
  U8                          RxVrefHigh;
  U8                          TxVrefLow;                ///< Units = txVrefStep
  U8                          TxVrefHigh;
} RmtDqMargin;

typedef struct {
  U8                          RxDqLeft;                 ///< Units = piStep
  U8                          RxDqRight;
  U8                          TxDqLeft;
  U8                          TxDqRight;
  U8                          CmdLeft;
  U8                          CmdRight;
  U8                          RecvenLeft;               ///< Units = recvenStep
  U8                          RecvenRight;
  U8                          WrLevelLeft;              ///< Units = wrLevelStep
  U8                          WrLevelRight;
  U8                          RxVrefLow;                ///< Units = rxVrefStep
  U8                          RxVrefHigh;
  U8                          TxVrefLow;                ///< Units = txVrefStep
  U8                          TxVrefHigh;
  U8                          CmdVrefLow;               ///< Units = caVrefStep
  U8                          CmdVrefHigh;
} RmtRankMargin;

typedef struct {
  U16                         RecEnDelay[MAX_STROBE];
  U16                         WlDelay[MAX_STROBE];
  U8                          RxDqDelay[MAX_STROBE];
  U8                          TxDqDelay[MAX_STROBE];
  U8                          ClkDelay;
  U8                          CtlDelay;
  U8                          CmdDelay[3];
  U8                          IoLatency;
  U8                          Roundtrip;
} RmtRankTraining;

typedef union {
  U16                         ModeRegister[MAX_MR_IN_DIMM]; ///< Dimm mode registers MR0 - MR3.
#if 0
  struct {
    struct {
      U16                     BurstLength     : 2;      ///<   A1:A0 - Burst length
      U16                     CasLatency0     : 1;      ///<      A2 - CAS latency bit 0
      U16                     ReadBurstType   : 1;      ///<      A3 - Read burst type
      U16                     CasLatency      : 3;      ///<   A6:A4 - CAS latency bits 3:1
      U16                     Mode            : 1;      ///<      A7 - Test/Normal mode
      U16                     DllReset        : 1;      ///<      A8 - DLL reset
      U16                     WriteRecovery   : 3;      ///<  A11:A9 - CAS latency bits 3:1
      U16                     DllPd           : 1;      ///<     A12 - DLL control for precharge power down
      U16                                     : 3;      ///< A15:A13 - Reserved
    } Mr0;
    struct {
      U16                     DllEnable       : 1;      ///<      A0 - DLL enable
      U16                     DriverImpCtrl0  : 1;      ///<      A1 - Output driver impedance control bit 0
      U16                     RttNom0         : 1;      ///<      A2 - Odt Rtt values bit 0
      U16                     AdditiveLatency : 2;      ///<   A4:A3 - Additive latency
      U16                     DriverImpCtrl   : 1;      ///<      A5 - Output driver impedance control bit 1
      U16                     RttNom1         : 1;      ///<      A6 - Odt Rtt values bit 1
      U16                     WriteLeveling   : 1;      ///<      A7 - Write leveling enable
      U16                                     : 1;      ///<      A8 - Reserved
      U16                     RttNom2         : 1;      ///<      A9 - Odt Rtt values bit 2
      U16                                     : 1;      ///<     A10 - Reserved
      U16                     TdqsEnable      : 1;      ///<     A11 - Termination data strobe
      U16                     Qoff            : 1;      ///<     A12 - Output disable
      U16                                     : 3;      ///< A15:A13 - Reserved
    } Mr1;
    struct {
      U16                     Pasr            : 3;      ///<   A2:A0 - Partial array self refresh
      U16                     CasWrLatency    : 3;      ///<   A5:A3 - CAS write latency
      U16                     AutoSelfRefresh : 1;      ///<      A6 - Automatic self refresh
      U16                     SelfRefreshTemp : 1;      ///<      A7 - Self refresh temperature range
      U16                                     : 1;      ///<      A8 - Reserved
      U16                     RttWrite        : 2;      ///<  A10:A9 - Dynamic ODT
      U16                                     : 5;      ///< A15:A11 - Reserved
    } Mr2;
    struct {
      U16                     MprAddress      : 2;      ///<   A1:A0 - Multi-purpose register address
      U16                     MprControl      : 1;      ///<      A2 - Multi-purpose register control
      U16                                     : 13;     ///<  A15:A3 - Reserved
    } Mr3;
  } b;
#endif
} RmtRankMrs;

typedef struct {
  U8                          RankEnabled;              ///< 0 = Rank disabled
  U8                          RankMarginEnabled;        ///< 0 = Rank margin disabled
  U8                          DqMarginEnabled;          ///< 0 = Dq margin disabled
  RmtRankMargin               RankMargin;               ///< Rank margin data
  RmtDqMargin                 DqMargin[MAX_DQ];         ///< Array of Dq margin data per rank
  RmtRankTraining             RankTraining;             ///< Rank training settings
  RmtRankMrs                  RankMRS;                  ///< Rank MRS settings
} RmtRankList;

typedef struct {
  U8                          SpdValid[MAX_SPD_RMT / (CHAR_BITS * sizeof (U8))]; ///< Each valid bit maps to SPD byte
  U8                          SpdData[MAX_SPD_RMT];     ///< Array of raw SPD data bytes
} RmtSpd;

typedef struct {
  U8                          DimmEnabled;              ///< 0 = DIMM disabled
  RmtRankList                 RankList[MAX_RANK_IN_DIMM]; ///< Array of ranks per DIMM
  RmtSpd                      SpdBytes;                 ///< SPD data per DIMM
} RmtDimmList;

typedef struct {
  U8                          ChannelEnabled;           ///< 0 = Channel disabled
  U8                          NumDimmSlot;              ///< Number of slots per channel on the board
  RmtDimmList                 DimmList[MAX_DIMMS_IN_CHANNEL]; ///< Array of DIMMs per channel
} RmtChannelList;

typedef struct {
  U8                          ControllerEnabled;        ///< 0 = MC disabled
  U16                         ControllerDeviceId;       ///< MC device Id
  U8                          ControllerRevisionId;     ///< MC revision Id    
  U16                         MemoryFrequency;          ///< Memory frequency in units of MHz / 10
                                                        ///< e.g. ddrFreq = 13333 for tCK = 1.5 ns
  U16                         MemoryVoltage;            ///< Memory Vdd in units of mV
                                                        ///< e.g. ddrVoltage = 1350 for Vdd = 1.35 V
  U8                          PiStep;                   ///< Step unit = piStep * tCK / 2048
                                                        ///< e.g. piStep = 16 for step = 11.7 ps (1/128 tCK)
  U16                         RxVrefStep;               ///< Step unit = rxVrefStep * Vdd / 100
                                                        ///< e.g. rxVrefStep = 520 for step = 7.02 mV
  U16                         TxVrefStep;               ///< Step unit = txVrefStep * Vdd / 100
  U16                         CaVrefStep;               ///< Step unit = caVrefStep * Vdd / 100
  U8                          RecvenStep;               ///< Step unit = recvenStep * tCK / 2048
  U8                          WrLevelStep;              ///< Step unit = wrLevelStep * tCK / 2048
  RmtChannelList              ChannelList[MAX_CHANNEL]; ///< Array of channels per memory controller
} RmtControllerList;

typedef struct {
  union {
    U32                       l;                        ///< MRC version: Major.Minor.Revision.Build
    struct {
      U8                      Build;                    ///< MRC version: Build
      U8                      Revision;                 ///< MRC version: Revision
      U8                      Minor;                    ///< MRC version: Minor
      U8                      Major;                    ///< MRC version: Major
    } c;
  }                           RefCodeRevision;          ///< Major.Minor.Revision.Build
  U8                          MaxController;            ///< Max controllers per system, e.g. 1
  U8                          MaxChannel;               ///< Max channels per memory controller, e.g. 2
  U8                          MaxDimm;                  ///< Max DIMM per channel, e.g. 2
  U8                          MaxRankDimm;              ///< Max ranks per DIMM, e.g. 2
  U8                          MaxStrobe;                ///< Number of Dqs used by the rank, e.g. 18
  U8                          MaxDq;                    ///< Number of Dq bits used by the rank, e.g. 72
  U32                         MarginLoopCount;          ///< Units of cache line
  RmtControllerList           ControllerList[MAX_CONTROLLERS]; ///< Array of memory controllers per system
} RmtSystem;

typedef struct RmtStruct {
  RmtHeader                   RmtHeader;
  RmtSystem                   RmtSystem;
} RmtData;

#pragma pack (pop)

#endif //_MrcRmtData_h_
