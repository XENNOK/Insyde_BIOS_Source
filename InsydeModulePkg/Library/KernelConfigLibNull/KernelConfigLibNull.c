/** @file
 NULL Library Instance for KernelConfigLib Library Class.

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <KernelSetupConfig.h>
#include <Library/DebugLib.h>

/**
 Retrieve kernel setup configuration data

 @param[out] KernelConfig       Pointer to the structure of KERNEL_CONFIGURATION,
                                this pointer must be allocated with sizeof(KERNEL_CONFIGURATION)
                                before being called

 @retval EFI_UNSUPPORTED        Not supported for NULL library instance implementation
**/
EFI_STATUS
EFIAPI
GetKernelConfiguration (
  OUT KERNEL_CONFIGURATION      *KernelConfig
  )
{
  ASSERT (FALSE);
  return EFI_UNSUPPORTED;
}

/**
 Retrieve the size of Setup variable

 @return                        Size of the setup variable data
**/
UINTN
EFIAPI
GetSetupVariableSize (
  VOID
  )
{
  ASSERT (FALSE);
  return 0;
}


