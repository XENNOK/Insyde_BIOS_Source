/** @file

  Header file for SPS ME functions
  Note: Only for SPS.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SPS_ME_LIB_H_
#define _SPS_ME_LIB_H_

///
/// SPS LIB
///
#include <Library/SpsHeciMsgLib.h>
#include <Library/SpsMePolicyLib.h>

/**
  Get SPS ME HECI#1 BAR.

  @param[out] Heci1Bar            SPS ME HECI#1 BAR.

  @retval EFI_SUCCESS             Get SPS ME HECI#1 BAR successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsGetHeci1Bar (
  OUT UINT64                           *Heci1Bar
  );

/**
  Get SPS ME HECI#2 BAR.

  @param[out] Heci2Bar            SPS ME HECI#2 BAR.

  @retval EFI_SUCCESS             Get SPS ME HECI#2 BAR successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsGetHeci2Bar (
  OUT UINT64                           *Heci2Bar
  );

/**
  Get SPS ME mode.

  @param[out] MeMode              SPS ME mode.

  @retval EFI_SUCCESS             Get SPS ME mode successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsGetMeMode (
  OUT UINT32                           *MeMode
  );

/**
  Get SPS ME status

  @param[out] MeStatus            SPS ME Status.

  @retval EFI_SUCCESS             Get SPS ME Status successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsGetMeStatus (
  OUT UINT32                           *MeStatus
  );

/**
  Clear SPS ME wake status.

  @param None.

  @retval EFI_SUCCESS             Allways return success.
**/
EFI_STATUS
SpsMeClearWakeStatus (
  VOID
  );

/**
  Check SPS ME firmware is init done.

  @param[out] IsInitComplete      SPS ME init done.
  @param[out] ErrorCode           Error code.

  @retval EFI_SUCCESS             Get version successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsCheckMeFwInit (
  OUT BOOLEAN                          *IsInitComplete,
  OUT UINT8                            *ErrorCode
  );

typedef enum {
  SpsPeiBeforeMemoryInit = 1,
  SpsPeiAfterMemoryInit,
  SpsPeiDisableAll,
  SpsDxeEntry,
  SpsDxeReadyToBoot,
} SPS_ME_DEVICE_CONFIG_PHASE;

/**
  Configure ME Devices when needed

  @param[in] Phase                Phase of POST

  @retval EFI_SUCCESS             Always return EFI_SUCCESS

**/
EFI_STATUS
SpsMeDeviceConfigure (
  IN UINTN                             Phase
  );

typedef enum {
  Disable = 1,
  Lock,
  GlobalReset,
} SPS_ME_CF9_CTRL;

/**
  Config CF9 setting.

  @param[in] Action               Lock, Disable or do global reset.

  @retval None.
**/
VOID
SpsMeCF9Configure (
  IN UINTN                             Action
  );

/**
  This procedure will configure the ME Host General Status register,
  indicating that DRAM Initialization is complete and ME FW may
  begin using the allocated ME UMA space.

  @param[in] InitStat             H_GS[27:24] Status

  @retval EFI_SUCCESS
**/
EFI_STATUS
SpsMeConfigDidReg (
  IN UINT8                             InitStat
  );

/**
  Get mPhy survivaility table version before DID.

  @param[out] mPhyVersion         mPhy version.

  @retval EFI_SUCCESS             Get version successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsMeGetmPhySurvivabilityTablePreDID (
  OUT UINT16                           *mPhyVersion
  );

/**
  Send mPhy survivaility table to SPS ME firmware.

  @param[in] ChipsetInitTableLen  Length of table.
  @param[in] ChipsetInitTable     Point to table.

  @retval EFI_SUCCESS             Send message successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsMemPhySurvivabilityProgramming (
  IN UINT32                            ChipsetInitTableLen,
  IN UINT8                             *ChipsetInitTable
  );

/**
  Check SPS ME is already received END_OF_POST message.

  @param None.

  @retval TRUE                    SPS ME is already received END_OF_POST message.
  @retval TRUE                    SPS ME is not receive END_OF_POST message.
**/
BOOLEAN
SpsIsAlreadyReceivedEndOfPost (
  VOID
  );

///
/// SPS Node Manager
///
/**
  Get SPS NM booting mode setting.

  @param[out] BiosBootingMode     Booting mode setting.

  @retval EFI_SUCCESS             Function completed successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsNmGetBootingMode (
  OUT UINT8                            *BiosBootingMode
  );

/**
  Get SPS NM cores disable setting.

  @param[out] NumberOfCoreDisable Cores disable setting.

  @retval EFI_SUCCESS             Function completed successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsNmGetCoresDisableNumber (
  OUT UINT8                            *NumberOfCoreDisable
  );

/**
  Get SPS NM power limiting setting.

  @param[out] IsPowerLimiting     Power limiting setting.

  @retval EFI_SUCCESS             Function completed successfully.
  @retval All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
SpsNmGetPowerLimiting (
  OUT UINT32                           *IsPowerLimiting
  );

#endif
