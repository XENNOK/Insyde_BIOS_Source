/** @file
  This module includes the power modes definitions.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@Copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved.
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
  license agreement.
**/
#ifndef _MrcPowerModes_h_
#define _MrcPowerModes_h_
#pragma pack(push, 1)

#include <McAddress.h>
#include <MrcTypes.h>
#include <MrcCommon.h>
#include <MrcGlobal.h>

//
// Lookup table definitions
//
#define GEN_DDR3    1
#define GEN_DDR4    2
#define VDD_120     1
#define VDD_135     2
#define VDD_150     3
#define VDD_OTHER   4
#define ECC_F       0
#define ECC_T       1
#define TYPE_SODIMM 3
#define TYPE_UDIMM  2
#define WIDTH_8     1
#define WIDTH_16    2
#define WIDTH_32    3
#define RANKS_1     1
#define RANKS_2     2
#define DPC_1       1
#define DPC_2       2
#define FREQ_800    1
#define FREQ_1000   2
#define FREQ_1067   3
#define FREQ_1200   4
#define FREQ_1333   5
#define FREQ_1400   6
#define FREQ_1600   7
#define FREQ_1800   8
#define FREQ_1867   9
#define FREQ_2000   10
#define FREQ_2133   11
#define FREQ_2200   12
#define FREQ_2400   13
#define FREQ_2600   14
#define FREQ_2667   15
#define DENSITY_1   2
#define DENSITY_2   3
#define DENSITY_4   4

typedef enum {
  tsmNoThermalSensing = 0,  ///< No thermal sensing in MC
  tsmThermalSensor,         ///< Thermal Sensor (on DIMM) - when set thermal sense is active
  tsmBwEstimation,          ///< BW estimation - when set, PM_SUM_PC_CxRy of this DIMM accumulates command power estimation
  tsmBoth                   ///< Both (1) and (2)
} ThermalSensorModes;

///
/// Power Down mode
///
typedef enum {
  pdmNoPowerDown      = 0,
  pdmAPD              = 1,
  pdmPPD              = 2,
  pdmPPDDLLOFF        = 6,
  pdmAuto             = 0xFF,
} MrcPowerDownMode;

typedef union {
  struct {
    U32 Vddq        :  4;
    U32 Ecc         :  1;
    U32 DimmType    :  4;
    U32 DeviceWidth :  3;
    U32 NumOfRanks  :  3;
    U32 Dpc         :  2;
    U32 Frequency   :  4;
    U32 DramDensity :  3;
    U32 Spare       :  8;
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PowerWeightInputs;

typedef struct {
  PowerWeightInputs PwInput;
  U8                ScaleFactor;
  U8                RdCr;
  U8                WrCr;
  U8                ActCr;
  U8                CkeL;
  U8                CkeH;
  U8                ChPwrFloor;
} Ddr3PowerWeightEntry;

typedef struct {
  PowerWeightInputs PwInput;
  U8                ScaleFactor;
  U8                RdCr;
  U8                WrCr;
  U8                ActCr;
  U8                CkeL;
  U8                CkeH;
  U8                OneChPwrFloor;
  U8                TwoChPwrFloor;
} Lpddr3PowerWeightEntry;

#define PDWN_IDLE_COUNTER   (0x80)

#define MCDECS_CBIT_DEFAULT (0x00000000)

/**
@brief
  This function configure the MC power register post training after normal mode before PCU start working.

  @param[in, out] MrcData - Include all MRC global data.

  @retval Nothing.
**/
extern
void
MrcPowerModesPostTraining (
  IN OUT MrcParameters *const MrcData
  );

/**
@brief
  This function configures the power down control register.

  @param[in]  - MrcData - The MRC global data.

  @retval - Nothing
**/
extern
void
MrcPowerDownConfig (
  IN MrcParameters *const MrcData
  );

/**
@brief
  This functions applies power weight values from lookup table to every DIMM in the system.

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus - mrcSuccess or reason for failure.
**/
extern
MrcStatus
MrcPowerWeight (
  MrcParameters * const MrcData
);

#pragma pack(pop)
#endif // _MrcPowerModes_h_
