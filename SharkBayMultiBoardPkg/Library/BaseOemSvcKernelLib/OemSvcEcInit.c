/** @file
  Provide hook function for OEM to initialize EC. 

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/BaseOemSvcKernelLib.h>
#include <Library/CommonEcLib.h>
#include <OemEc.h>

VOID
KbdWrCmd (
  IN   UINT8           Cmd
)
{
  WaitKbcIbe(SMC_CMD_STATE);

  WriteKbc (SMC_CMD_STATE, Cmd);

  WaitKbcIbe(SMC_CMD_STATE);
}

/**
  EC initialize.

  @retval EFI_UNSUPPORTED    Returns unsupported by default.
  @retval EFI_SUCCESS        The service is customized in the project.
  @retval EFI_MEDIA_CHANGED  The value of IN OUT parameter is changed. 
**/
EFI_STATUS
OemSvcEcInit (
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/
  KbdWrCmd(0x46);                          // Enable the power to the on board LAN chip
  KbdWrCmd(0xAB);                          // The KBC exit ACPI mode and return to APM mode
  KbdWrCmd(0x05);

  return EFI_UNSUPPORTED;
}
