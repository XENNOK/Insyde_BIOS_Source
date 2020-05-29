/** @file
  This module configures the memory controller timing parameters.

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@Copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved.

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

#ifndef _MRC_TIMING_CONFIGURATION_H_
#define _MRC_TIMING_CONFIGURATION_H_

#include <MrcTypes.h>
#include <MrcApi.h>
#include <McAddress.h>
#include <MrcCommon.h>
#include <MrcDdr3.h>
#include <MrcGlobal.h>
#include <MrcOem.h>
#if SUPPORT_SODIMM == SUPPORT
#include <MrcSpdProcessing.h>
#endif //SUPPORT_SODIMM == SUPPORT

///
/// tCCD values.
///
#define TCCD_ALL_FREQ (4) ///< tCCD is = 4 DCLK for all frequencies up to 1600.
///
/// tWPRE values.
///
#define TWPRE_ALL_FREQ  (1) ///< tWPRE is = 1 DCLK for all frequencies up to 1600.
///
/// tRPRE values.
///
#define TRPRE_ALL_FREQ  (1) ///< tRPRE is = 1 DCLK for all frequencies up to 1600.
///
/// tCKE values.
///
#define TCKE_800  (3)
#define TCKE_1067 (3)
#define TCKE_1333 (4)
#define TCKE_1600 (4)
#define TCKE_1867 (5)
#define TCKE_2133 (6)
#define TCKE_2400 (6)
#define TCKE_2667 (7)

///
/// tCKE values for LPDDR: max(7.5ns, 3nCK)
///
#define tCKE_LPDDR_1067 (4)
#define tCKE_LPDDR_1333 (5)
#define tCKE_LPDDR_1600 (6)

///
/// tXP values for LPDDR: max(7.5ns, 3nCK)
///
#define tXP_LPDDR_1333 (5)
#define tXP_LPDDR_1600 (6)

///
/// tXPDLL values.
///
#define TXPDLL_800  (10)
#define TXPDLL_1067 (13)
#define TXPDLL_1333 (16)
#define TXPDLL_1600 (20)
#define TXPDLL_1867 (23)
#define TXPDLL_2133 (26)
#define TXPDLL_2400 (29)
#define TXPDLL_2667 (32)

///
/// tAONPD values.
///
#define TAONPD_800  (4)
#define TAONPD_1067 (5)
#define TAONPD_1333 (6)
#define TAONPD_1600 (7) ///< SNB had 8
#define TAONPD_1867 (8)
#define TAONPD_2133 (10)
#define TAONPD_2400 (11)
#define TAONPD_2667 (12)

#define MC_tXP_1600_1N (5)
#define MC_tXP_1867_2N (6)
#define MC_tXP_1867_3N (7)
#define MC_tXP_2133_1N (7)
#define MC_tXP_MAX     (8) ///< The maximum value that the MC supports.

///
/// tODTon / tODToff values, in [ps]
///
#define tODT_ON_MIN   1750
#define tODT_ON_MAX   3500
#define tODT_OFF_MIN  1750
#define tODT_OFF_MAX  3500

///
/// tDQSCK values, in [ps]
///
#define tDQSCK_MIN    2500
#define tDQSCK_MAX    5500
///
/// Specified in PI-Ticks.  64 == 1 QClk
///
#define tDQSCK_DRIFT  64

/**
@brief
  This function configures the memory controller timings.

  @param[in] MrcData - Include all MRC global data.

  @retval Nothing.
**/
extern
void
MrcTimingConfiguration (
  IN MrcParameters *const MrcData
  );

/**
@brief
  This function returns the tXP value for the specified frequency.

  @param[in] DdrType   - DDR type
  @param[in] Frequency - The memory frequency.
  @param[in] NMode     - Command mode to lookup.

  @retval The tXP value for the specified frequency.
**/
extern
U32
tXPValue (
  IN MrcDdrType         DdrType,
  IN const MrcFrequency Frequency,
  IN U8                 NMode
  );

/**
@brief
  This function sets up the Read ODTD values based on RawCard types and adjusts the tDRRD2RD, tDDRD2RD, tDRRD2WR and tDDRD2WR

  @param[in, out] MrcData - Include all MRC global data.

  @retval Nothing.
**/
extern
void
RdOdtStretch (
  IN OUT MrcParameters *const MrcData
  );

#endif // _MrcTimingConfiguration_h_
