/** @file
  Header file for SPI Access Library Initialization

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

#ifndef _SPI_ACCESS_INIT_LIB_H_
#define _SPI_ACCESS_INIT_LIB_H_

#include <Uefi.h>

/**
  Detect whether the system is at EFI runtime or not

  @param  None

  @retval TRUE                  System is at EFI runtime
  @retval FALSE                 System is not at EFI runtime

**/
BOOLEAN
EFIAPI
AtRuntime (
  VOID
  );

/**
  Initialization routine for SpiAccessLib

  @param  None

  @retval EFI_SUCCESS           SpiAccessLib successfully initialized
  @return Others                SpiAccessLib initialization failed

**/
EFI_STATUS
EFIAPI
SpiAccessInit (
  VOID
  );

/**
  This routine uses to free the allocated resource by SpiAccessInit ().

  @retval EFI_SUCCESS    Free allocated resource successful.
  @return Others         Free allocated resource failed.
**/
EFI_STATUS
EFIAPI
SpiAccessDestroy (
  VOID
  );

#endif
