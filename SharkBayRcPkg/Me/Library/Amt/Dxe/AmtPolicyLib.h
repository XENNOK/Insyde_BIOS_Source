/** @file

  Header file for AMT Policy functionality

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

#ifndef _AMT_POLICY_LIB_H_
#define _AMT_POLICY_LIB_H_

//[-start-120808-IB10820098-modify]//
#include <Guid/MeBiosExtensionSetup.h>
#include <Protocol/AmtPlatformPolicy.h>
//[-end-120808-IB10820098-modify]//

/**
  Check if AMT is enabled in setup options.

  @param[in] None.

  @retval EFI_SUCCESS             AMT platform policy is initialized.
  @retval All other error conditions encountered when no AMT platform policy available.
**/
EFI_STATUS
AmtPolicyLibInit (
  VOID
  );

/**
  Check if Asf is enabled in setup options.

  @param[in] None.

  @retval FALSE                   Asf is disabled.
  @retval TRUE                    Asf is enabled.
**/
BOOLEAN
AsfSupported (
  VOID
  );

/**
  Check if Amt is enabled in setup options.

  @param[in] None.

  @retval FALSE                   Amt is disabled.
  @retval TRUE                    Amt is enabled.
**/
BOOLEAN
AmtSupported (
  VOID
  );

/**
  Check if AMT BIOS Extension hotkey was pressed during BIOS boot.

  @param[in] None.

  @retval FALSE                   MEBx hotkey was not pressed.
  @retval TRUE                    MEBx hotkey was pressed.
**/
BOOLEAN
AmtHotkeyPressed (
  VOID
  );

/**
  Check if AMT BIOS Extension Selection Screen is enabled in setup options.

  @param[in] None.

  @retval FALSE                   AMT Selection Screen is disabled.
  @retval TRUE                    AMT Selection Screen is enabled.
**/
BOOLEAN
AmtSelectionScreen (
  VOID
  );

/**
  Check if AMT WatchDog is enabled in setup options.

  @param[in] None.

  @retval FALSE                   AMT WatchDog is disabled.
  @retval TRUE                    AMT WatchDog is enabled.
**/
BOOLEAN
AmtWatchDog (
  VOID
  );

/**
  Return BIOS watchdog timer

  @param[in] None.

  @retval UINT16                  BIOS ASF Watchdog Timer
**/
UINT16
AmtWatchDogTimerBiosGet (
  VOID
  );

/**
  Return OS watchdog timer

  @param[in] None.

  @retval UINT16                  OS ASF Watchdog Timer
**/
UINT16
AmtWatchDogTimerOsGet (
  VOID
  );

/**
  Provide CIRA request information from OEM code.

  @param[in] None.

  @retval Check if any CIRA requirement during POST
**/
BOOLEAN
AmtCiraRequestTrigger (
  VOID
  );

/**
  Provide CIRA request Timeout from OEM code.

  @param[in] None.

  @retval CIRA require Timeout for MPS connection to be estabilished
**/
UINT8
AmtCiraRequestTimeout (
  VOID
  );

/**
  Provide Manageability Mode setting from MEBx BIOS Sync Data

  @param[in] None

  @retval UINT8                   Manageability Mode = MNT_AMT or MNT_ASF
**/
UINT8
ManageabilityModeSetting (
  VOID
  );

/**
  Provide UnConfigure ME without password request from OEM code.

  @param[in] None.

  @retval Check if unConfigure ME without password request
**/
BOOLEAN
AmtUnConfigureMe (
  VOID
  );

/**
  Provide 'Hiding the Unconfigure ME without password confirmation prompt' request from OEM code.

  @param[in] None.

  @retval Check if 'Hide unConfigure ME without password Confirmation prompt' request
**/
BOOLEAN
AmtHideUnConfigureMeConfPrompt (
  VOID
  );

/**
  Provide show MEBx debug message request from OEM code.

  @param[in] None.

  @retval Check show MEBx debug message request
 **/
BOOLEAN
AmtMebxDebugMsg (
  VOID
  );

/**
  Provide on-board device list table and do not need to report them to AMT.  AMT only need to know removable PCI device
  information.

  @param[in] None.

  @retval on-board device list table pointer other than system device.
**/
UINT32
AmtPciDeviceFilterOutTable (
  VOID
  );

/**
  Check if USB provisioning enabled/disabled in platform policy.

  @param[in] None.

  @retval FALSE                   USB provisioning is disabled.
  @retval TRUE                    USB provisioning is enabled.
**/
BOOLEAN
USBProvisionSupport (
  VOID
  );

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
  );

/**
  Check if ForcMebxSyncUp is enabled in setup options.

  @param[in] None.

  @retval FALSE                   ForcMebxSyncUp is disabled.
  @retval TRUE                    ForcMebxSyncUp is enabled.
**/
BOOLEAN
AmtForcMebxSyncUp (
  VOID
  );

/**
  Dump DXE Amt Platform Policy

  @param[in] None.

  @retval None
**/
VOID
DxeAmtPolicyDebugDump (
  VOID
  );

/**
  Dump ME_BIOS_EXTENSION_SETUP variable

  @param[in] MeBiosExtensionSetup   Pointer to ME_BIOS_EXTENSION_SETUP variable

  @retval None
**/
VOID
DxeMebxSetupVariableDebugDump (
  IN ME_BIOS_EXTENSION_SETUP  *MeBiosExtensionSetup OPTIONAL
  );

#endif
