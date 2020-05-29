/** @file
 Library Instance for KernelConfigLib Library Class.

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/VariableLib.h>
#include <KernelSetupConfig.h>


/**
 Retrieve kernel setup configuration data

 @param[out] KernelConfig       Pointer to the structure of KERNEL_CONFIGURATION,
                                this pointer must be allocated with sizeof(KERNEL_CONFIGURATION)
                                before being called

 @retval EFI_SUCCESS            The kernel configuration is successfully retrieved
 @retval EFI_INVALID_PARAMETER  NULL pointer for input KernelConfig paramater
 @return others                 Failed to retrieve kernel configuration
**/
EFI_STATUS
EFIAPI
GetKernelConfiguration (
  OUT KERNEL_CONFIGURATION      *KernelConfig
  )
{
  EFI_STATUS Status;
  VOID       *Buffer;
  UINTN      BufferSize;

  if (KernelConfig == NULL) {
    ASSERT_EFI_ERROR (KernelConfig != NULL);
    return EFI_INVALID_PARAMETER;
  }

  Buffer = NULL;
  Status = CommonGetVariableDataAndSize (
             SETUP_VARIABLE_NAME,
             &gSystemConfigurationGuid,
             &BufferSize,
             (VOID **) &Buffer
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CopyMem (KernelConfig, Buffer, sizeof(KERNEL_CONFIGURATION));
  FreePool (Buffer);
  return EFI_SUCCESS;
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
  EFI_STATUS      Status;
  UINTN           Size;

  Size = 0;
  Status = CommonGetVariable (
             SETUP_VARIABLE_NAME,
             &gSystemConfigurationGuid,
             &Size,
             NULL
             );

  return Size;
}

