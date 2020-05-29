/** @file

  Header file for Me Policy functionality

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

#ifndef _ME_POLICY_LIB_H_
#define _ME_POLICY_LIB_H_
//[-start-120808-IB10820098-modify]//
#include <Protocol/MePlatformPolicy.h>
//[-end-120808-IB10820098-modify]//

/**
  Check if Me is enabled.

  @param[in] None.

  @retval EFI_SUCCESS             ME platform policy pointer is initialized.
  @retval All other error conditions encountered when no ME platform policy available.
**/
EFI_STATUS
MePolicyLibInit (
  VOID
  );

/**
  Check if End of Post Message is enabled in setup options.

  @param[in] None.

  @retval FALSE                   EndOfPost is disabled.
  @retval TRUE                    EndOfPost is enabled.
**/
BOOLEAN
MeEndOfPostEnabled (
  VOID
  );


/**
  Show Me Error message. This is used to support display error message on the screen for localization
  description

  @param[in] MsgId                Me error message ID for displaying on screen message

  @retval None
**/
VOID
MeReportError (
  IN ME_ERROR_MSG_ID              MsgId
  );

/**
  Check if MeFwDowngrade is enabled in setup options.

  @param[in] None.

  @retval FALSE                   MeFwDowngrade is disabled.
  @retval TRUE                    MeFwDowngrade is enabled.
**/
BOOLEAN
MeFwDowngradeSupported (
  VOID
  );

/**
  Dump DXE ME Platform Policy

  @param[in] None.

  @retval None
**/
VOID
DxeMePolicyDebugDump (
  VOID
  );
#endif
