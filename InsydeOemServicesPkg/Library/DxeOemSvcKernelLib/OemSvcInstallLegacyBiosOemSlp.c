/** @file
  Provides an opportunity for OEM to define SLP string which is based on OEM specification.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/DxeOemSvcKernelLib.h>

/**
  This OemService provides OEM to define SLP string which based on OEM specification. 
  This service only supports the legacy OS (XP system).

  @param[out]  *SlpStringLength      The length of SLP string.
  @param[out]  *SlpStringAddress     A pointer to the address of SLP string. 

  @retval      EFI_UNSUPPORTED       Returns unsupported by default.
  @retval      EFI_SUCCESS           Get SLP string success.
  @retval      EFI_MEDIA_CHANGED     The value of IN OUT parameter is changed. 
  @retval      Others                Base on OEM design.
**/
EFI_STATUS
OemSvcInstallLegacyBiosOemSlp (
  OUT UINTN                                 *SlpLength,
  OUT UINTN                                 *SlpAddress
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
