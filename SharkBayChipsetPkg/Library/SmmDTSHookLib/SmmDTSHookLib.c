/** @file

 Digital Thermal Sensor (DTS) SMM Library.
 This SMM Library configures and supports the DigitalThermalSensor features
 for the platform.

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

#include <Uefi/UefiBaseType.h>
#include <Library/SmmKscLib.h>
#include <KscLib.h>

/**
  Prepare data and protocol for Dts Hooe Lib

  @param[in]       None
 
  @retval EFI_SUCCESS    Initialize complete
*/
EFI_STATUS
InitializeDtsHookLib (
  VOID
  )
{
  //
  // Nothing to do on CRB.
  //
  return EFI_SUCCESS;
}

/**
  Platform may need to register some data to private data structure before generate 
  software SMI or SCI.

  @param[in]       None
 
  @retval None
*/
VOID
PlatformHookBeforeGenerateSCI (
  VOID
  )
{
  //
  // Nothing to do on CRB.
  //
}

/**
  When system temperature out of specification, do platform specific programming to prevent 
  system damage.

  @param[in]  None
 
  @retval None
*/
VOID
PlatformEventOutOfSpec (
  VOID
  )
{
  EFI_STATUS  Status;

  //
  // Handle critical event by shutting down via EC
  //
  Status = InitializeKscLib ();
  if (Status == EFI_SUCCESS) {
    SendKscCommand (KSC_C_SYSTEM_POWER_OFF);
  }
}
