/** @file

  Implementation file for Me Policy functionality

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

//[-start-120808-IB10820098-modify]/
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <MePolicyLib.h>
#include <MeAccess.h>
//[-end-120808-IB10820098-modify]//

//
// Global variables
//
DXE_ME_POLICY_PROTOCOL            *mDxePlatformMePolicy = NULL;

/**
  Check if Me is enabled.

  @param[in] None.

  @retval EFI_SUCCESS             ME platform policy pointer is initialized.
  @retval All other error conditions encountered when no ME platform policy available.
**/
EFI_STATUS
MePolicyLibInit (
  VOID
  )
{
  EFI_STATUS                      Status;

  if (mDxePlatformMePolicy != NULL) {
    return EFI_SUCCESS;
  }
  //
  // Get the desired platform setup policy.
  //
  Status = gBS->LocateProtocol (&gDxePlatformMePolicyGuid, NULL, (VOID **) &mDxePlatformMePolicy);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "No ME Platform Policy Protocol available"));
    ASSERT_EFI_ERROR (Status);
  } else if (mDxePlatformMePolicy == NULL) {
    DEBUG ((EFI_D_ERROR, "No ME Platform Policy Protocol available"));
    Status = EFI_UNSUPPORTED;
  }
  return Status;
}

/**
  Check if End of Post Message is enabled in setup options.

  @param[in] None.

  @retval FALSE                   EndOfPost is disabled.
  @retval TRUE                    EndOfPost is enabled.
**/
BOOLEAN
MeEndOfPostEnabled (
  VOID
  )
{
  BOOLEAN                         Supported;
  EFI_STATUS                      Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  if (mDxePlatformMePolicy->MeConfig.EndOfPostEnabled != 1) {
    Supported = FALSE;
  } else {
    Supported = TRUE;
  }

  return Supported;
}

/**
  Checks if EndOfPost event already happened

  @param[in] None

  @retval TRUE                    if end of post happened
  @retval FALSE                   if not yet
**/
BOOLEAN
IsAfterEndOfPost (
  VOID
  )
{
  EFI_STATUS                      Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }

  if (mDxePlatformMePolicy->MeConfig.EndOfPostDone == TRUE) {
    return TRUE;
  }

  return FALSE;
}

/**
  Show Me Error message. This is used to support display error message on the screen for localization
  description

  @param[in] MsgId                Me error message ID for displaying on screen message

  @retval None
**/
VOID
MeReportError (
  IN ME_ERROR_MSG_ID              MsgId
  )
{
  EFI_STATUS                      Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit();
    if (EFI_ERROR(Status)) {
      return;
    }
  }
  mDxePlatformMePolicy->MeReportError (MsgId);

  return ;
}

/**
  Check if MeFwDowngrade is enabled in setup options.

  @param[in] None.

  @retval FALSE                   MeFwDowngrade is disabled.
  @retval TRUE                    MeFwDowngrade is enabled.
**/
BOOLEAN
MeFwDowngradeSupported (
  VOID
  )
{
  EFI_STATUS                      Status;

  if (mDxePlatformMePolicy == NULL) {
    Status = MePolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  if (mDxePlatformMePolicy->MeConfig.MeFwDownGrade == 1) {
    return TRUE;
  }

  return FALSE;
}
