/** @file
  This module include MRC_RefreshConfiguration external data

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
  license agreement
**/

#ifndef _MRC_REFRESH_CONFIGURATION_H_
#define _MRC_REFRESH_CONFIGURATION_H_

#include <MrcTypes.h>
#include <MrcApi.h>
#include <McAddress.h>
#include <MrcCommon.h>
#include <MrcDdr3.h>
#include <MrcOem.h>
#include <MrcGlobal.h>

///
/// tDLLK values
///
#define tDLLK_VALUE         512

///
/// tXSOffset values
///
#define Offset_10nsec_800   4
#define Offset_10nsec_1067  6
#define Offset_10nsec_1333  7
#define Offset_10nsec_1600  8
#define Offset_10nsec_1867  10
#define Offset_10nsec_2133  11
#define Offset_10nsec_2400  12
#define Offset_10nsec_2667  14

///
/// tMOD values. max(12nCK,15ns)
///
#define tMOD_800            12
#define tMOD_1067           12
#define tMOD_1333           12
#define tMOD_1600           12
#define tMOD_1867           14
#define tMOD_2133           16
#define tMOD_2400           18
#define tMOD_2667           20

///
/// tZQOPER values.
///
#define tZQOPER_1600        256
#define tZQOPER_1867        299
#define tZQOPER_2133        342
#define tZQOPER_2400        384
#define tZQOPER_2667        427

//
// ZQCL and ZQCS values for LPDDR3, in [ps]
//
#define tZQCL_MIN           360000
#define tZQCS_MIN           90000

///
/// tZQCS values.
///
#define tZQCS_1600          64
#define tZQCS_1867          75
#define tZQCS_2133          86
#define tZQCS_2400          96
#define tZQCS_2667          107

//
// ZQCS period values, in (tREFI * 128) units
//
#define ZQCS_PERIOD_DDR3    128   // tREFI * 128 = 7.8 us * 128 = 1ms
#define ZQCS_PERIOD_LPDDR3  256   // tREFI * 128 = 3.9 us * 128 = 0.5ms

/**
@brief
  This function returns the tZQOPER value.
  tZQOPER = Defines the period required for ZQCL after SR exit.

  @param[in] MrcData         - Include all MRC global data.
  @param[in] Frequency       - The memory frequency.

  @retval The tZQOPER value.
**/
extern
U32
tZQOPERGet (
  IN MrcParameters *const MrcData,
  IN const MrcFrequency Frequency
  );

/**
@brief
  This function returns the tMOD value.
  tMOD = max(12nCK, 15ns) nCK change by the frequency.

  @param[in] Frequency       - The memory frequency.

  @retval The tMOD value.
**/
extern
U32
tMODGet (
  IN const MrcFrequency Frequency
  );

/**
@brief
  This function returns the tZQCS value.

  @param[in] MrcData         - Include all MRC global data.
  @param[in] Frequency       - The memory frequency.

  @retval The tZQCS value.
**/
extern
U32
tZQCSGet (
  IN MrcParameters *const MrcData,
  IN const MrcFrequency Frequency
  );

/**
@brief
  This function configures the TC_MR2_SHADDOW register and its fields.

  @param[in] MrcData         - Include all MRC global data.
  @param[in] Channel         - Channel to work on.
  @param[in] Dimm            - Dimm to work on.
  @param[in] Mr2Value        - The value of MR2 to setup.

  @retval Nothing.
**/
extern
void
SetTcMr2ShadowReg (
  IN MrcParameters *const MrcData,
  IN const U32            Channel,
  IN const U32            Dimm,
  IN U32                  Mr2Value
  );

/**
@brief
  This function executes the refresh configuration process.

  @param[in] MrcData         - Include all MRC global data.

  @retval Nothing.
**/
extern
void
MrcRefreshConfiguration (
  IN MrcParameters *const MrcData
  );

#endif
