/** @file
  Define how the user confirm dialog will be printed.

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

#include <Library/DxeOemSvcKernelLib.h>
#include <KernelSetupConfig.h>

/**
  Provides a chance for OEM to customize the User Confirm Dialog. 

  @param[in]  TpmPpCommand              Physical Presence request command code
  @param[in]  TpmVersion                Version of TPM device
  @param[out] Confirmed                 Indicates the request is accepted or not
  
  @retval     EFI_SUCCESS               Operation successed  
  @retval     EFI_UNSUPPORTED           Returns unsupported by default
  @retval     Others                    Operation failed
**/
EFI_STATUS 
OemSvcTpmUserConfirmDialog (
  IN  UINT8                          TpmPpCommand,
  IN  UINT8                          TpmVersion,
  OUT BOOLEAN                        *Confirmed
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
