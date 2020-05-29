/** @file
  Provide gIsaAcpiDeviceList and gIsaDeviceFunction.

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

#include "DxeOemServicesThunk.h"
#include <Library/DxeOemSvcKernelLib.h>

/**
  Provide gIsaAcpiDeviceList and gIsaDeviceFunction.

  @param  SIOTable              All supported ISA device base on platform design.
  @param  IsaDeviceFunction     Function call for ISA device use.

  @retval EFI_SUCCESS           Always returns success.
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcGetSioResourceTableThunk (
  IN OEM_SERVICES_PROTOCOL  *Services,
  IN UINTN                  NumOfArgs,
  ...
  )
{
  return EFI_UNSUPPORTED;
}

