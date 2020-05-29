/** @file
  I2c Dxe Driver

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "I2cDxe.h"
#include "I2cEnum.h"

/**
  The user Entry Point for I2C module. The user code starts with this function.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
InitializeI2c(
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS              Status;
  //
  // Install driver model protocol(s).
  //
  Status = InitializeI2cEnum ( ImageHandle, SystemTable );
  if (!EFI_ERROR (Status)) {
    Status = InitializeI2cHost ( ImageHandle, SystemTable );
    if (!EFI_ERROR (Status)) {
      Status = InitializeI2cBus ( ImageHandle, SystemTable );
    }
  }
  if (EFI_ERROR (Status)) {
    I2cUnload (ImageHandle);
  }
  return Status;
}

/**
  This is the unload handle for I2C module.

  Disconnect the driver specified by ImageHandle from all the devices in the handle database.
  Uninstall all the protocols installed in the driver entry point.

  @param[in] ImageHandle           The drivers' driver image.

  @retval    EFI_SUCCESS           The image is unloaded.
  @retval    Others                Failed to unload the image.

**/
EFI_STATUS
EFIAPI
I2cUnload (
  IN EFI_HANDLE             ImageHandle
  )
{
  I2cEnumUnload ( ImageHandle );
  I2cHostUnload ( ImageHandle );
  I2cBusUnload ( ImageHandle );
  return EFI_SUCCESS;
}
