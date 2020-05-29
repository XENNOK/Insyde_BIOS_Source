/** @file
  CPU/Chipset/Platform Initial depends on project characteristic.

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

#include <Library/PeiOemSvcKernelLib.h>

/**
  CPU/Chipset/Platform Initial depends on project characteristic.

  @param[in]  *Buffer               A pointer to SYSTEM_CONFIGURATION.
  @param[in]  SetupVariableExist    Setup variable be found in variable storage or not.

  @retval     EFI_UNSUPPORTED       Returns unsupported by default.
  @retval     EFI_SUCCESS           The service is customized in the project.
  @retval     EFI_MEDIA_CHANGED     The value of IN OUT parameter is changed. 
  @retval     Others                Depends on customization.
**/
EFI_STATUS
OemSvcInitPlatformStage2 (
  IN  VOID                                 *Buffer,
  IN  BOOLEAN                              SetupVariableExist
  )
{

  /*++
    Tips for programmer at Project layer:
  
//  SYSTEM_CONFIGURATION                  *SystemConfiguration;
//  SystemConfiguration = (SYSTEM_CONFIGURATION *)Buffer;

    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}

