/** @file
  Provide OEM to customize the Hardware Signature.

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
  To determine Hardware Signature by oem.

  @param[in,out]  HardwareSignature  Value of Hardware Signature.

  @retval      EFI_SUCCESS           The hardware signature has already been updated by this 
                                     function. The Kernel should not update the hardware 
                                     signature.
  @retval      EFI_MEDIA_CHANGED     Hardware Signature was updated by this function. Then 
                                     Kernel will update the FACS with the returned Hardware 
                                     Signature.
  @retval      EFI_UNSUPPORTED       The hardware signature was not updated by this function.
**/
EFI_STATUS
OemSvcUpdateAcpiFacsHardwareSignature (
  IN OUT  UINT32                  *HardwareSignature
)
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}

