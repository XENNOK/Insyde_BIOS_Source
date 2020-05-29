/** @file
 DXE IPMI BDS library implement code - IPMI Boot Option.

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


#include <Library/DxeIpmiBdsLib.h>




/**
 This function should be called after IpmiMiscDxe process boot options so that
 it can return correct BootNext value.

 @param[in]         BootNext            Pointer to boot device number for Boot#### that will boot at this time.

 @retval EFI_SUCCESS                    Update boot device number success.
 @retval EFI_UNSUPPORTED                Boot device number is not set.
*/
EFI_STATUS
IpmiSetBootNext (
  IN  UINT16         **BootNext
  )
{

  UINT16    NewBootNext;

  NewBootNext = PcdGet16 (PcdIpmiBootNext);

  if (NewBootNext == 0xFFFF) {
    return EFI_UNSUPPORTED;
  }

  if (*BootNext == NULL) {
    *BootNext = AllocatePool (sizeof (UINT16));
  }

  if (*BootNext == NULL) {
    return EFI_UNSUPPORTED;
  }

  **BootNext = NewBootNext;

  return EFI_SUCCESS;

}

