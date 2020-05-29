/** @file
  MRC Common / Generic functions

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************


@copyright
  Copyright (c) 2011 - 2012 Intel Corporation. All rights reserved.
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

#ifndef _MRC_GENERAL_H_
#define _MRC_GENERAL_H_

#include <MrcTypes.h>
#include <MrcApi.h>
#include <MrcAddressDecodeConfiguration.h>
#include <MrcCommon.h>
#include <MrcCrosser.h>
#include <MrcDebugHook.h>
//
// #include "MrcMemoryClean.h"
//
#include <MrcOem.h>
#include <MrcPowerModes.h>
//
// #include "MrcRefreshConfiguration.h"
//
#include <MrcSpdProcessing.h>

#define GENERAL_PM_CONFIGURATION_2_BUS_ADDRESS      (0)
#define GENERAL_PM_CONFIGURATION_2_DEVICE_ADDRESS   (31)
#define GENERAL_PM_CONFIGURATION_2_FUNCTION_ADDRESS (0)

#define GENERAL_PM_CONFIGURATION_2                  ((0xA2) & (~0x03))

#define GENERAL_PM_CONFIGURATION_2_DISB_OFFSET      (0x7)
#define GENERAL_PM_CONFIGURATION_2_DISB_WIDTH       (0x1)
#define GENERAL_PM_CONFIGURATION_2_DISB_MASK        (0x80)
#define GENERAL_PM_CONFIGURATION_2_DISB_DEFAULT     (0x0)

#define GENERAL_PM_CONFIGURATION_2_MEM_SR_OFFSET    (0x5)
#define GENERAL_PM_CONFIGURATION_2_MEM_SR_WIDTH     (0x1)
#define GENERAL_PM_CONFIGURATION_2_MEM_SR_MASK      (0x20)
#define GENERAL_PM_CONFIGURATION_2_MEM_SR_DEFAULT   (0x0)

#define MEMORY_TEST_CACHELINE                       (100) ///< max number can be 100 from one rank
///
/// Define the total memory size of a channel.
///
typedef enum {
  tcs16GB,  ///< 16 GB per channel
  tcs8GB,   ///< 8 GB
  tcs2GB,   ///< 2 GB
  tcs512MB  ///< 512 MB
} MrcTotalChannelSize;

extern MrcUpmPwrRetrainLimits  InitialLimits[MRC_NUMBER_UPM_PWR_RETRAIN_MARGINS];

/**
  Thisfunction performs Software Memory testing

  @param[in] MrcData - Include all MRC global data.

  @retval Always returns mrcSuccess.
**/
extern
MrcStatus
MrcHwMemTest (
  IN     MrcParameters *const MrcData
  );

/**
  This function changes the MC to normal mode, enables the ECC if needed, lock configuration and set PU_MRC_Done.
  If the ECC is enabled, this function should be called after memory is cleaned.

  @param[in, out] MrcData - Include all MRC global data.

  @retval Always returns mrcSuccess.
**/
extern
MrcStatus
MrcMcActivate (
  IN OUT MrcParameters *const MrcData
  );

/**
  This function enables Normal Mode and configures the Power Down Modes
  for the boot flows other than Cold Boot.

  @param[in] MrcData - The MRC general data.

  @retval Always returns mrcSuccess.
**/
extern
MrcStatus
MrcNormalMode (
  IN MrcParameters *const MrcData
  );

/**
  This function is the last funtion that call from the MRC core.
  The function set DISB and set the MRC_Done.

  @param[in, out] MrcData - include all the MRC general data.

  @retval Always returns mrcSuccess.
**/
extern
MrcStatus
MrcDone (
  IN OUT MrcParameters *const MrcData
  );

/**
  Print the MRC version to the MRC output device.

  @param[in] Debug   - Pointer to the MRC Debug structure.
  @param[in] Version - The MRC version.

  @retval Nothing.
**/
extern
void
MrcVersionPrint (
  IN const MrcParameters *MrcData,
  IN const MrcVersion    *Version
  );

/**
  This function return the MRC version.

  @param[out] Version - Location to store the MRC version.

  @retval Nothing.
**/
extern
void
MrcVersionGet (
     OUT MrcVersion *const Version
  );

/**
  This function set the MRC vertion to MCDECS_SPARE register.
  The function need to be call by the wrapper after MrcStartMemoryConfiguration function where the MC CLK enable.
  The function write:
  Major number to bits 16-23
  Minor number to bits 8-15
  Build number to bits 0 - 7

  @param[in] MrcData - Include all MRC global data.

  @retval Nothing.
**/
extern
MrcStatus
MrcSetMrcVersion (
  IN     MrcParameters *const MrcData
  );

/**
  This function locks the memory controller and memory map registers.

  @param[in] MrcData - Include all MRC global data.

  @retval Nothing.
**/
extern
void
McRegistersLock (
  IN     MrcParameters *const MrcData
  );

/**
  This function returns the recommended MRC boot mode.

  @param[in] void - No arguments

  @retval bmWarm if we are in self refresh and the DISB bit is set, otherwise returns bmCold.
**/
extern
MrcBootMode
MrcGetBootMode (
  void
  );

/**
  This function sets the DISB bit in General PM Configuration 2 B:D:F 0,31,0 offset 0xA2.

  @param[in] void - No arguments

  @retval Nothing.
**/
extern
void
MrcSetDISB (
  void
  );

/**
  This function resets the DISB bit in General PM Configuration 2 B:D:F 0,31,0 offset 0xA2.

  @param[in] void - No arguments

  @retval Nothing.
**/
extern
void
MrcResetDISB (
  void
  );

/**
  This function reads the CAPID0 register and sets the memory controller's capability.

  @param[in, out] MrcData - All the MRC global data.

  @retval Returns mrcSuccess if the memory controller's capability has been determined, otherwise returns mrcFail.
**/
extern
MrcStatus
MrcMcCapability (
  IN OUT MrcParameters *const MrcData
  );

/**
  This function reads the CAPID0 register and sets the memory controller's capability.

  @param[in, out] MrcData - All the MRC global data.

  @retval Returns mrcSuccess if the memory controller's capability has been determined, otherwise returns mrcFail.
**/
MrcStatus
MrcMcCapabilityPreSpd (
  IN OUT MrcParameters *const MrcData
  );

/**
  This function sets the appropriate timing overrides in the output structure
  prior to Spd processing.

  @param[in, out] MrcData - All the MRC global data.

  @retval Returns mrcSuccess if the timing overrides have been conpleted.
**/
extern
MrcStatus
MrcSetOverridesPreSpd (
  IN OUT MrcParameters *const MrcData
  );

/**
  This function reads the input data structure and sets the appropriate timing overrides in the output structure.

  @param[in, out] MrcData - All the MRC global data.

  @retval Returns mrcSuccess if the timing overrides have been conpleted.
**/
extern
MrcStatus
MrcSetOverrides (
  IN OUT MrcParameters *const MrcData
  );

/**
  This function set the WM0-5 values. Those values are be using by the graphics driver.
  need to be call after PU_MRC_DONE bit is set to 1.

  @param[in] MrcData - include all the MRC data.

  @retval Nothing.

  **/
void
MrcWmRegSet (
  IN     MrcParameters *const MrcData
  );


/**
@brief
  Program DDRPL_CR_DDR_TX_DELAY if Memory Trace is enabled

  @param[in] MrcData - The MRC general data.

  @retval None
**/
void
MrcSetDdrplTxDelay (
  IN MrcParameters *const MrcData
  );

#endif
