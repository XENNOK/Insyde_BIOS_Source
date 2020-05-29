/** @file
  This file is the OEM interface to the MRC core.
  This file includes the OEM MRC implementation and can be changed between projects.
  Each MRC customer must provide those OEM interfaces.

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
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
#ifndef _MRC_OEM_H_
#define _MRC_OEM_H_

#include <MrcTypes.h>
#include <MrcApi.h>
#include <MrcGlobal.h>

//
// OEM MMIO routines
//
#include "MrcOemMmio.h"

//
//////////////////////////////////////////////////////////////////////////////////////
//                               OEM debug print routines and types                 //
//////////////////////////////////////////////////////////////////////////////////////
//
#include "MrcOemDebugPrint.h"

#ifndef MRC_DEBUG_MSG
#error "MRC_DEBUG_MSG is not defined"
#endif //MRC_DEBUG_MSG
///
//////////////////////////////////////////////////////////////////////////////////////
///                           OEM platform  routines and types                      //
//////////////////////////////////////////////////////////////////////////////////////
///
/// define the oem check points the OEM can define more point and locate them in the code.
///
typedef enum {
  OemFastBootPermitted,     ///<  before fast boot.
  OemRestoreNonTraining,
  OemPrintInputParameters,  ///<  before printing input parameters
  OemSpdProcessingRun,      ///<  before spd processing code
  OemSetOverridePreSpd,     ///<  before set overrides pre spd
  OemMcCapabilityPreSpd,    ///<  before MC capability pre spd
  OemSetOverride,           ///<  before set overrides
  OemMcCapability,          ///<  before MC capability
  OemMcInitRun,             ///<  before mc init code
  OemMcMemoryMap,           ///<  before memory map
  OemMcResetRun,            ///<  before jedec reset
  OemPreTraining,           ///<  before the training.
  OemMcTrainingRun,         ///<  before training code
  OemEarlyCommandTraining,  ///<  before Early Command training
  OemJedecInitLpddr3,       ///<  before Jedec Init Lpddr3
  OemSenseAmpTraining,      ///<  before Sense Amp Training
  OemReadMprTraining,       ///<  before Read MPR Training
  OemReceiveEnable,         ///<  before Read Leveling
  OemJedecWriteLeveling,    ///<  before Jedec Write Leveling
  OemWriteLeveling,         ///<  before Functional Write Leveling
  OemWriteDqDqs,            ///<  before Write Timing Centering
  OemReadDqDqs,             ///<  before Read Timing Centering
  OemDimmRonTraining,       ///<  before DIMM Ron algorithm.
  OemDimmODTTraining,       ///<  before DIMM ODT algorithm.
  OemDimmODT1dTraining,     ///<  before DIMM ODT 1d algorithm.
  OemWriteDriveStrength,    ///<  before Write DS
  OemWriteSlewRate,         ///<  before Write SR
  OemWriteEQTraining,       ///<  before Write Equalization Training
  OemReadODTTraining,       ///<  before Read ODT algorithm.
  OemReadEQTraining,        ///<  before Read Equalization Training
  OemReadAmplifierPower,    ///<  before Read Amplifier Power
  OemOptimizeComp,          ///<  before Comp Optimization Training
  OemPowerSavingMeter,      ///<  before PowerSavingMeter step
  OemWriteDqDqs2D,          ///<  before Write Timing Centering 2D
  OemReadDqDqs2D,           ///<  before Read Timing Centering 2D
  OemCmdVoltCentering,      ///<  before Command Voltage Centering
  OemWriteVoltCentering2D,  ///<  before Write Voltage Centering 2D
  OemReadVoltCentering2D,   ///<  before Read Voltage Centering 2D
  OemLateCommandTraining,   ///<  before Late Command training
  OemRoundTripLatency,      ///<  before Round Trip Latency Traiing
  OemTurnAroundTimes,       ///<  before Turn Aorund Times.
  OemRcvEnCentering1D,      ///<  before Receive Enable Centring 
  OemSaveMCValues,          ///<  before saving memory controller values
  OemRmt,                   ///<  before RMT crosser tool.
  OemMemTest,               ///<  before Memory testing
  OemRestoreTraining,       ///<  before Restoring Training Values
  OemSelfRefreshExit,       ///<  before Self Refresh Exit
  OemNormalMode,            ///<  before Normal Mode on non-cold boots.
  OemAliasCheck,            ///<  before alias checking on cold boots.
  OemHwMemInit,

  OemPostTraining,          ///<  after the training.
  OemMrcActivate,           ///<  before MrcActivate call.
  OemMrcDone,               ///<  call to MrcOemCheckPoint when MRC was done.
  OemFrequencySet,          ///<  do operation before frequency set.
  OemFrequencySetDone,      ///<  do operation after frequency set.
  OemStartMemoryConfiguration,
  OemBeforeNormalMode,      ///<  call to MrcOemCheckPoint before normal mode is enalbed
  OemAfterNormalMode,       ///<  call to MrcOemCheckPoint after normal mode is enalbed
  OemMrcFillRmt,
  OemRetrainMarginCheck,
  ///
  ///*********************************************************************************
  ///
  OemNumOfCommands          ///<  Should always be last in the list!
} MRC_OemStatusCommand;

///
/// Define the MRC SMBUS devices type.
///
typedef enum {
  datOemSpd_0_0,      ///< use for get the device address for channel 0 dimm 0
  datOemSpd_0_1,      ///< use for get the device address for channel 0 dimm 1
  datOemSpd_1_0,      ///< use for get the device address for channel 1 dimm 0
  datOemSpd_1_1,      ///< use for get the device address for channel 1 dimm 1
  datOemVrefWrite_0,  ///< use for get the device address for dimm vref controlled potentiometer channel 0
  datOemVrefWrite_1,  ///< use for get the device address for dimm vref controlled potentiometer channel 1
  datOemVrefRead      ///< use for get the device address for cpu vref controlled potentiometer
} MRC_OemSmbusDeviceType;

///
//////////////////////////////////////////////////////////////////////////////////////
///                             OEM IO routines and types                           //
//////////////////////////////////////////////////////////////////////////////////////
///

/**
  8 bit I/O port read.
     Needs to be ported for OEM platform requirements.

  @param[in] Address - the IO read address.

  @retval return the read value.
**/
extern
U8
MrcOemInPort8 (
  IN U16 IoAddress
  );

/**
  8 bit I/O port write.
     Needs to be ported for OEM platform requirements.

  @param[in] IoAddress - The I/O port read address.
  @param[in] Data      - The value to write.

  @retval Nothing
**/
extern
void
MrcOemOutPort8 (
  IN U16 IoAddress,
  IN U8  data
  );

/**
  16 bit I/O port read.
     Needs to be ported for OEM platform requirements.

  @param[in] IoAddress - The I/O port read address.

  @retval return the read value.
**/
extern
U16
MrcOemInPort16 (
  IN U16 IoAddress
  );

/**
  16 bit I/O port write.
     Needs to be ported for OEM platform requirements.

  @param[in] IoAddress - The I/O port read address.
  @param[in] Data      - The value to write.

  @retval Nothing
**/
extern
void
MrcOemOutPort16 (
  IN U16 IoAddress,
  IN U16 data
  );

/**
  32 bit I/O port read.
     Needs to be ported for OEM platform requirements.

  @param[in] IoAddress - The I/O port read address.

  @retval The value read.
**/
extern
U32
MrcOemInPort32 (
  IN U16 IoAddress
  );

/**
  32 bit I/O port write.
     Needs to be ported for OEM platform requirements.

  @param[in] IoAddress - The I/O port read address.
  @param[in] Data      - The value to write.

  @retval Nothing
**/
extern
void
MrcOemOutPort32 (
  IN U16 IoAddress,
  IN U32 data
  );

/**
  This function return the PCI index address.
     Needs to be ported for OEM platform requirements.

  @param[in] None.

  @retval The PCI index address.
**/
extern
U16
MrcOemPciIndex (
  void
  );

/**
  This function return the PCI data address.
     Needs to be ported for OEM platform requirements.

  @param[in] None

  @retval The PCI data address.
**/
extern
U16
MrcOemPciData (
  void
  );

/**
  This function return the PCI device address.
      Needs to be ported for OEM platform requirements.

  @param[in] Bus      - PCI bus
  @param[in] Device   - PCI device
  @param[in] Function - PCI function
  @param[in] Offset   - Offset

  @retval The PCI device address.
**/
extern
U32
MrcOemGetPciDeviceAddress (
  IN U8 Bus,
  IN U8 Device,
  IN U8 Function,
  IN U8 Offset
  );

/**
  This function return the PCIE device address.
      Needs to be ported for OEM platform requirements.

  @param[in] Bus      - PCI bus
  @param[in] Device   - PCI device
  @param[in] Function - PCI function
  @param[in] Offset   - Offset

  @retval The PCIe device address.
**/
extern
U32
MrcOemGetPcieDeviceAddress (
  IN U8 Bus,
  IN U8 Device,
  IN U8 Function,
  IN U8 Offset
  );

/**
  Returns the current time, as determined by reading the Real Time Clock (RTC) on the platform.
  Since RTC time is stored in BCD, convert each value to binary.

  @param[out] Seconds       - The current second (0-59).
  @param[out] Minutes       - The current minute (0-59).
  @param[out] Hours         - The current hour (0-23).
  @param[out] DayOfMonth    - The current day of the month (1-31).
  @param[out] Month         - The current month (1-12).
  @param[out] Year          - The current year (2000-2099).

  @retval Nothing
**/
extern
void
MrcOemGetRtcTime (
  U8 *Seconds,
  U8 *Minutes,
  U8 *Hours,
  U8 *DayOfMonth,
  U8 *Month,
  U16*Year
  );

///
//////////////////////////////////////////////////////////////////////////////////////
///                    OEM Memory routines and types                                //
//////////////////////////////////////////////////////////////////////////////////////
///

/**
  Copy the specified number of memory bytes, a byte at a time, from the
  specified source to the specified destination.
     Needs to be ported for OEM platform requirements.

  @param[in, out] Dest            - Destination pointer.
  @param[in]      Src             - Source pointer.
  @param[in]      NumBytes        - The number of bytes to copy.

  @retval Nothing
**/
extern
void
MrcOemMemoryCpy (
  IN OUT U8  *Dest,
  IN     U8  *Src,
  IN     U32 NumBytes
  );

/**
  Sets the specified number of memory bytes, a byte at a time, at the
  specified destination.
     Needs to be ported for OEM platform requirements.

  @param[in, out] Dest            - Destination pointer.
  @param[in]      Value           - The value to set.
  @param[in]      NumBytes        - The number of bytes to set.

  @retval Nothing
**/
extern
void
MrcOemMemorySet (
  IN OUT U8  *Dest,
  IN     U32 Value,
  IN     U32 NumBytes
  );

/**
  Sets the specified number of memory dwords, a dword at a time, at the
  specified destination.

  @param[in, out] Dest     - Destination pointer.
  @param[in]      Value    - The value to set.
  @param[in]      NumWords - The number of dwords to set.

  @retval Nothing.
**/
extern
void
MrcOemMemorySetWord (
  IN OUT U16    *Dest,
  IN const U16  Value,
  IN U32        NumWords
  );

/**
  Sets the specified number of memory dwords, a dword at a time, at the
  specified destination.

  @param[in, out] Dest      - Destination pointer.
  @param[in]      Value     - The value to set.
  @param[in]      NumDwords - The number of dwords to set.

  @retval Nothing.
**/
extern
void
MrcOemMemorySetDword (
  IN OUT U32    *Dest,
  IN const U32  Value,
  IN U32        NumDwords
  );

/**
  Shift the specified data value left by the specified count.
    Needs to be ported for OEM platform requirements.

  @param[in] Data            - 64 bit number to shift left.
  @param[in] Count           - Number of bits to shift (0..63)

  @retval The number of bits shifted left.
**/
extern
U64
MrcOemMemoryLeftShiftU64 (
  IN const U64 Data,
  IN const U8  Count
  );

/**
  Shift the specified data value Right by the specified count.

  @param[in] Data  - U64 number to shift
  @param[in] Count - number of bits to shift (0..63)

  @retval Returns the shifted U64 value.
**/
extern
U64
MrcOemMemoryRightShiftU64 (
  IN const U64 Data,
  IN const U8  Count
  );

/**
  This function Multiply U64 with a U32 number. Result is <= 64 bits
  Needs to be ported for OEM platform requirements.

  @param[in] Multiplicand - U64 number to be multiplied with
  @param[in] Multiplier   - U32 number to multiply

  @retval Returns the multiplication result of U64 value.
**/
extern
U64
MrcOemMemoryMultiplyU64ByU32 (
  IN const U64 Multiplicand,
  IN const U32 Multiplier
  );

/**
  Divide U64 with a U64 number. Result is <= 32 bits

  @param[in] Dividend - U64 number to be multiplied with
  @param[in] Divisor  - U32 number to multiply

  @retval Returns the quotient result of U64 value.
**/
extern
U64
MrcOemMemoryDivideU64ByU64 (
  IN const U64 Dividend,
  IN const U64 Divisor
  );

#endif // _MrcOem_h_
