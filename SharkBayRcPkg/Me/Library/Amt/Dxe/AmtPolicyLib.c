/** @file

  Implementation file for AMT Policy functionality

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

//[-start-120808-IB10820098-modify]//
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/Amt/Dxe/AmtPolicyLib.h>
#include <Library/MeKernel/Dxe/MeLib.h>
#include <Protocol/ActiveManagement.h>
#include <Protocol/AmtPlatformPolicy.h>
//[-end-120808-IB10820098-modify]//

///
/// Global variables
///
DXE_AMT_POLICY_PROTOCOL  *mDxePlatformAmtPolicy = NULL;

/**
  Check if AMT is enabled in setup options.

  @param[in] None.

  @retval EFI_SUCCESS             AMT platform policy pointer is initialized.
  @retval All other error conditions encountered when no AMT platform policy available.
**/
EFI_STATUS
AmtPolicyLibInit (
  VOID
  )
{
  EFI_STATUS  Status;

  if (mDxePlatformAmtPolicy != NULL) {
    return EFI_SUCCESS;
  }
  //
  // Get the desired platform setup policy.
  //
  Status = gBS->LocateProtocol (&gDxePlatformAmtPolicyGuid, NULL, (VOID **) &mDxePlatformAmtPolicy);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "No AMT Platform Policy Protocol available"));
    ASSERT_EFI_ERROR(Status);
  } else if (mDxePlatformAmtPolicy == NULL) {
    DEBUG ((EFI_D_ERROR, "No AMT Platform Policy Protocol available"));
    Status = EFI_UNSUPPORTED;
  }

  return Status;
}

/**
  Check if Asf is enabled in setup options.

  @param[in] None.

  @retval FALSE                   Asf is disabled.
  @retval TRUE                    Asf is enabled.
**/
BOOLEAN
AsfSupported (
  VOID
  )
{
  EFI_STATUS                      Status;

  if (mDxePlatformAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  //
  // First check if ASF support is enabled in Setup.
  //
  if (mDxePlatformAmtPolicy->AmtConfig.AsfEnabled != 1) {
    return FALSE;
  }

  return TRUE;
}

/**
  Check if Amt is enabled in setup options.

  @param[in] None.

  @retval FALSE                   Amt is disabled.
  @retval TRUE                    Amt is enabled.
**/
BOOLEAN
AmtSupported (
  VOID
  )
{
  EFI_STATUS                      Status;

  if (mDxePlatformAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  //
  // First check if AMT support is enabled in Setup.
  //
  if (mDxePlatformAmtPolicy->AmtConfig.iAmtEnabled != 1) {
    return FALSE;
  }

  return TRUE;
}

/**
  Check if AMT BIOS Extension hotkey was pressed during BIOS boot.

  @param[in] None.

  @retval FALSE                   MEBx hotkey was not pressed.
  @retval TRUE                    MEBx hotkey was pressed.
**/
BOOLEAN
AmtHotkeyPressed (
  VOID
  )
{
  BOOLEAN                         Supported;
  EFI_STATUS                      Status;

  if (mDxePlatformAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  //
  // First check if AMT Setup Prompt is enabled in Setup.
  //
  if (mDxePlatformAmtPolicy->AmtConfig.iAmtbxHotkeyPressed == 1) {
    Supported = TRUE;
  } else {
    Supported = FALSE;
  }

  return Supported;
}

/**
  Check if AMT BIOS Extension Selection Screen is enabled in setup options.

  @param[in] None.

  @retval FALSE                   AMT Selection Screen is disabled.
  @retval TRUE                    AMT Selection Screen is enabled.
**/
BOOLEAN
AmtSelectionScreen (
  VOID
  )
{
  BOOLEAN                         Supported;
  EFI_STATUS                      Status;

  if (mDxePlatformAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  //
  // First check if AMT Selection Screen is enabled in Setup.
  //
  if (mDxePlatformAmtPolicy->AmtConfig.iAmtbxSelectionScreen == 1) {
    Supported = TRUE;
  } else {
    Supported = FALSE;
  }

  return Supported;
}

/**
  Check if AMT WatchDog is enabled in setup options.

  @param[in] None.

  @retval FALSE                   AMT WatchDog is disabled.
  @retval TRUE                    AMT WatchDog is enabled.
**/
BOOLEAN
AmtWatchDog (
  VOID
  )
{
  BOOLEAN                         Supported;
  EFI_STATUS                      Status;

  if (mDxePlatformAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  Supported = FALSE;
  if (ManageabilityModeSetting () != 0) {
    //
    // First check if AMT WatchDog is enabled in Setup.
    //
    if (AsfSupported ()) {
      if (mDxePlatformAmtPolicy->AmtConfig.WatchDog == 1) {
        Supported = TRUE;
      }
    }
  }

  return Supported;
}

/**
  Return BIOS watchdog timer

  @param[in] None.

  @retval UINT16                  BIOS ASF Watchdog Timer
**/
UINT16
AmtWatchDogTimerBiosGet (
  VOID
  )
{
  EFI_STATUS                      Status;

  if (mDxePlatformAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return 0;
    }
  }
  return mDxePlatformAmtPolicy->AmtConfig.WatchDogTimerBios;
}

/**
  Return OS watchdog timer

  @param[in] None.

  @retval UINT16                  OS ASF Watchdog Timer
**/
UINT16
AmtWatchDogTimerOsGet (
  VOID
  )
{
  EFI_STATUS                      Status;

  if (mDxePlatformAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return 0;
    }
  }
  return mDxePlatformAmtPolicy->AmtConfig.WatchDogTimerOs;
}

/**
  Provide CIRA request information from OEM code.

  @param[in] None.

  @retval Check if any CIRA requirement during POST
**/
BOOLEAN
AmtCiraRequestTrigger (
  VOID
  )
{
  EFI_STATUS                      Status;

  if (mDxePlatformAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  return mDxePlatformAmtPolicy->AmtConfig.CiraRequest == 1;
}

/**
  Provide CIRA request Timeout from OEM code.

  @param[in] None.

  @retval CIRA require Timeout for MPS connection to be estabilished
**/
UINT8
AmtCiraRequestTimeout (
  VOID
  )
{
  EFI_STATUS                      Status;

  if (mDxePlatformAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  return mDxePlatformAmtPolicy->AmtConfig.CiraTimeout;
}

/**
  Provide Manageability Mode setting from MEBx BIOS Sync Data

  @param[in] None

  @retval UINT8                   Manageability Mode = MNT_AMT or MNT_ASF
**/
UINT8
ManageabilityModeSetting (
  VOID
  )
{
  EFI_STATUS                      Status;

  if (mDxePlatformAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return 0;
    }
  }
  return (UINT8) (mDxePlatformAmtPolicy->AmtConfig.ManageabilityMode);
}

/**
  Provide UnConfigure ME without password request from OEM code.

  @param[in] None.

  @retval Check if unConfigure ME without password request
**/
BOOLEAN
AmtUnConfigureMe (
  VOID
  )
{
  EFI_STATUS                      Status;

  if (mDxePlatformAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  return mDxePlatformAmtPolicy->AmtConfig.UnConfigureMe == 1;
}

/**
  Provide 'Hiding the Unconfigure ME without password confirmation prompt' request from OEM code.

  @param[in] None.

  @retval Check if 'Hide unConfigure ME without password Confirmation prompt' request
**/
BOOLEAN
AmtHideUnConfigureMeConfPrompt (
  VOID
  )
{
  EFI_STATUS                      Status;

  if (mDxePlatformAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  return mDxePlatformAmtPolicy->AmtConfig.HideUnConfigureMeConfirm == 1;
}

/**
  Provide show MEBx debug message request from OEM code.

  @param[in] None.

  @retval Check show MEBx debug message request
 **/
BOOLEAN
AmtMebxDebugMsg (
  VOID
  )
{
  EFI_STATUS                      Status;

  if (mDxePlatformAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  return mDxePlatformAmtPolicy->AmtConfig.MebxDebugMsg == 1;
}

/**
  Provide on-board device list table and do not need to report them to AMT.  AMT only need to know removable PCI device
  information.

  @param[in] None.

  @retval on-board device list table pointer other than system device.
**/
UINT32
AmtPciDeviceFilterOutTable (
  VOID
  )
{
  EFI_STATUS                      Status;

  if (mDxePlatformAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return 0;
    }
  }
  return mDxePlatformAmtPolicy->AmtConfig.PciDeviceFilterOutTable;
}

/**
  Check if USB provisioning enabled/disabled in platform policy.

  @param[in] None.

  @retval FALSE                   USB provisioning is disabled.
  @retval TRUE                    USB provisioning is enabled.
**/
BOOLEAN
USBProvisionSupport (
  VOID
  )
{
  BOOLEAN                         Supported;
  EFI_STATUS                      Status;

  if (mDxePlatformAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  Supported = FALSE;

  //
  // First check if USB Provision is enabled in Setup.
  //
  if (mDxePlatformAmtPolicy->AmtConfig.USBProvision == 1) {
    Supported = TRUE;
  }

  return Supported;
}

/**
  This will return progress event Option.
  True if the option is enabled.

  @param[in] None.

  @retval True                    progress event is enabled.
  @retval False                   progress event is disabled.
**/
BOOLEAN
FwProgressSupport (
  VOID
  )
{
  BOOLEAN                         Supported;
  EFI_STATUS                      Status;

  if (mDxePlatformAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  Supported = FALSE;

  //
  // First check if FW Progress is enabled in Setup.
  //
  if (mDxePlatformAmtPolicy->AmtConfig.FWProgress == 1) {
    Supported = TRUE;
  }

  return Supported;
}

/**
  Check if ForcMebxSyncUp is enabled in setup options.

  @param[in] None.

  @retval FALSE                   ForcMebxSyncUp is disabled.
  @retval TRUE                    ForcMebxSyncUp is enabled.
**/
BOOLEAN
AmtForcMebxSyncUp (
  VOID
  )
{
  BOOLEAN                         Supported;
  EFI_STATUS                      Status;

  if (mDxePlatformAmtPolicy == NULL) {
    Status = AmtPolicyLibInit();
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
  }
  if (mDxePlatformAmtPolicy->AmtConfig.ForcMebxSyncUp == 1) {
    Supported = TRUE;
  } else {
    Supported = FALSE;
  }

  return Supported;
}
