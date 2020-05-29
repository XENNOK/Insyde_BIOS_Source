/** @file
  Display warning message if the spidevice is not in spi device table.

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
  Provide OEM to display Spi not support message.

  @retval EFI_SUCCESS           Always returns success.
**/
EFI_STATUS
OemSvcDisplaySpiNotSupportThunk (
  IN OEM_SERVICES_PROTOCOL                 *OemServices,
  IN  UINTN                                NumOfArgs,
  ...
  )
{
  return EFI_UNSUPPORTED;
}
