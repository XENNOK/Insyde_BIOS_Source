/** @file
  Header file for SeamlessRecoveryLib Library Class

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SEAMLESS_RECOVERY_LIB_H_
#define _SEAMLESS_RECOVERY_LIB_H_

#define FIRMWARE_FAILURE_FLAG_NAME    L"FirmwareFailureRecoveryFlag"
#define FIRMWARE_FAILURE_FLAG_DATA    0xFF
/**
  Check whether it is in firmware failure recovery mode

  @param  None

  @retval TRUE      The system is in firmware failure recovery mode
          FALSE     The system is not in firmware failure recovery mode

**/
BOOLEAN
IsFirmwareFailureRecovery (
  VOID
  );

/**
  Set firmware updating in process signature

  @param  None

  @returns None

**/
VOID
SetFirmwareUpdatingFlag (
  BOOLEAN FirmwareUpdatingFlag
  );

#endif
