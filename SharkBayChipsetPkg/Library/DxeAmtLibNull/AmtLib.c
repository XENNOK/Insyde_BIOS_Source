/** @file

  A emptry template implementation of AMT Library.

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

#include <Library/Amt/Dxe/AmtLib.h>

/**
  Check if Asf is enabled in setup options.

  @param[in] None.
  @retval EFI_SUCCESS             mActiveManagement is not NULL
  @retval                         Error Status code returned by
                                  LocateProtocol.
**/
EFI_STATUS
AmtLibInit (
  VOID
  )
{
  return EFI_UNSUPPORTED;
}

/**
  This will return IDE Redirection Boot Option.
  True if the option is enabled.

  @param[in] None.

  @retval True                    IDE-R is enabled.
  @retval False                   IDE-R is disabled.
**/
BOOLEAN
ActiveManagementEnableIdeR (
  VOID
  )
{
  return FALSE;
}

/**
  This will return Enforce Secure Boot over IDER Boot Option.
  True if the option is enabled.

  @param[in] None.

  @retval True                    Enforce Secure Boot is enabled.
  @retval False                   Enforce Secure Boot is disabled.
**/
BOOLEAN
ActiveManagementEnforceSecureBoot (
  VOID
  )
{
  return FALSE;
}

/**
  This will return BIOS Pause Boot Option.
  True if the option is enabled.

  @param[in] None.

  @retval True                    BIOS Pause is enabled.
  @retval False                   BIOS Pause is disabled.
**/
BOOLEAN
ActiveManagementPauseBoot (
  VOID
  )
{
  return FALSE;
}

/**
  This will return BIOS Setup Boot Option.
  True if the option is enabled.

  @param[in] None.

  @retval True                    BIOS Setup is enabled.
  @retval False                   BIOS Setup is disabled.
**/
BOOLEAN
ActiveManagementEnterSetup (
  VOID
  )
{
  return FALSE;
}

/**
  This will return Serial-over-Lan Boot Option.
  True if the option is enabled.

  @param[in] None.

  @retval True                    Console Lock is enabled.
  @retval False                   Console Lock is disabled.
**/
BOOLEAN
ActiveManagementConsoleLocked (
  VOID
  )
{
  return FALSE;
}

/**
  This will return KVM Boot Option.
  True if the option is enabled.

  @param[in] None.

  @retval True                    KVM is enabled.
  @retval False                   KVM is disabled.
**/
BOOLEAN
ActiveManagementEnableKvm (
  VOID
  )
{
  return FALSE;
}

/**
  This will return Serial-over-Lan Boot Option.
  True if the option is enabled.

  @param[in] None.

  @retval True                    Serial-over-Lan is enabled.
  @retval False                   Serial-over-Lan is disabled.
**/
BOOLEAN
ActiveManagementEnableSol (
  VOID
  )
{
  return FALSE;
}

/**
  This will return IDE Redirection boot device index to boot

  @param[in] None.

  @retval IdeBootDevice           Return the boot device number to boot
                                  Bits 0-1: If IDER boot is selected in Perimeter 1 then Bits 1,2 define the drive on the IDER controller to be used as the boot driver.
                                  Bit 1  Bit0
                                   0    0    Primary Master Drive
                                   0    1    Primary Slave Drive
                                   1    0    Secondary Master Drive
                                   1    1    Secondary Slave Drive
                                  Bits 2-7: Reserved set to 0
**/
UINT8
ActiveManagementIderBootDeviceGet (
  VOID
  )
{
  return 0;
}

/**
  Stop ASF Watch Dog Timer

  @param[in] None.

  @retval None
**/
VOID
AsfStopWatchDog (
  VOID
  )
{
  return ;
}

/**
  Start ASF Watch Dog Timer

  @param[in] WatchDogType         Which kind of WatchDog, ASF OS WatchDog Timer setting or ASF BIOS WatchDog Timer setting

  @retval None
**/
VOID
AsfStartWatchDog (
  IN  UINT8                       WatchDogType
  )
{
  return ;
}

/**
  This will return progress event Option.
  True if the option is enabled.

  @param[in] None.

  @retval True                    progress event is enabled.
  @retval False                   progress event is disabled.
**/
BOOLEAN
ActiveManagementFwProgress (
  VOID
  )
{
  return FALSE;
}

/**
  Sent initialize KVM message

  @param[in] None.

  @retval True                    KVM Initialization is successful
  @retval False                   KVM is not enabled
**/
BOOLEAN
BdsKvmInitialization (
  VOID
  )
{
  return FALSE;
}
