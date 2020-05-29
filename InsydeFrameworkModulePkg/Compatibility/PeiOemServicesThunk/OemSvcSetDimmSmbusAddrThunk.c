/** @file
  Provide OEM to  set Dimm Smbus address which is defined by the platform specification.

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

#include "PeiOemServicesThunk.h"
#include <Library/PeiOemSvcKernelLib.h>

/**
  This OemService provides OEM to set Dimm Smbus address which is defined by the platform specification. 

  @param  *DimmSmbusAddrArray   Point to the Dimm Smbus address array.

  @retval EFI_SUCCESS           Get address success.
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcSetDimmSmbusAddrThunk (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  OEM_SERVICES_PPI                  *This,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  return EFI_UNSUPPORTED;
}
