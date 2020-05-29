/** @file

  Implementation file for SPS Policy functionality in SMM Phase.
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

#include <Uefi.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

#include <Protocol/SpsPolicy.h>

///
/// Global variables
///
SPS_POLICY_PROTOCOL                    mPrivateSpsMePolicy;
SPS_POLICY_PROTOCOL                    *mSpsMePolicy = NULL;

/**
  Get SPS ME Policy.

  @param[out] SpsPolicy           Point of SPS Policy.

  @retval EFI_SUCCESS             SPS Policy Found.
**/
EFI_STATUS
GetSpsPolicy (
  OUT VOID                             **SpsPolicy
  )
{
  EFI_STATUS       Status;

  if (SpsPolicy == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (mSpsMePolicy != NULL) {
    *SpsPolicy = mSpsMePolicy;
    return EFI_SUCCESS;
  }

  Status = gBS->LocateProtocol (&gSpsPolicyProtocolGuid, NULL, (VOID **) &mSpsMePolicy);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SMM SPS POLICY LIB] ERROR: Locate SPS Policy Protocol => (%r)\n", Status));
    return Status;
  }

  CopyMem (&mPrivateSpsMePolicy, mSpsMePolicy, sizeof (SPS_POLICY_PROTOCOL));
  mSpsMePolicy = &mPrivateSpsMePolicy;

  *SpsPolicy = mSpsMePolicy;

  return EFI_SUCCESS;
}

/**
  SPS Policy Lib callback functions.

  @param  Event         Event whose notification function is being invoked.
  @param  Context       Pointer to the notification function's context, which is
                        always zero in current implementation.

**/
VOID
EFIAPI
SpsPolicyLibCallbackFunction (
  IN EFI_EVENT                         Event,
  IN VOID                              *Context
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;

  Status = GetSpsPolicy (&SpsMePolicy);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SMM SPS POLICY LIB] ERROR: CallbackFunction() Locate SPS Policy Protocol => (%r)\n", Status));
  }

}

/**
  The constructor function caches the pointer of SPS ME Policy.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.

  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
SpsPolicyLibConstructor (
  IN EFI_HANDLE                        ImageHandle,
  IN EFI_SYSTEM_TABLE                  *SystemTable
  )
{
  SPS_POLICY                 *SpsMePolicy;
  EFI_STATUS                 Status;
  EFI_EVENT                  SpsPolicyProtocolEvent;

  SpsPolicyProtocolEvent = NULL;

  Status = GetSpsPolicy (&SpsMePolicy);
  if (EFI_ERROR (Status)) {
    mSpsMePolicy = NULL;
    Status = SystemTable->BootServices->CreateEventEx (
                            EVT_NOTIFY_SIGNAL,
                            TPL_NOTIFY,
                            SpsPolicyLibCallbackFunction,
                            NULL,
                            &gSpsPolicyProtocolGuid,
                            &SpsPolicyProtocolEvent
                            );
  }

  return EFI_SUCCESS;
}

