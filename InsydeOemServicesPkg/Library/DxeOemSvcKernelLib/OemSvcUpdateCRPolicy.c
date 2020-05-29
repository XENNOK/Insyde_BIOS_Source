/** @file
  Update the Setting of CRPolicy.

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

#include <Library/DxeOemSvcKernelLib.h>
#include <Protocol/CRPolicy.h>


/**
  Update CRPolicy according to OEM setting.

  @param[in]   *VOID                   Point to CRPolicy Structure. 

  @retval      EFI_MEDIA_CHANGED       CRPolicy was updated.
  @retval      EFI_UNSUPPORTED           CRPolicy was not updated.
**/
EFI_STATUS
OemSvcUpdateCRPolicy (
  IN OUT VOID    *CRPolicy
  )
{
 /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
