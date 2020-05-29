/** @file
  Header file for support functions in protected mode.

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

#ifndef _RUNTIME_FUNCTIONS_H_
#define _RUNTIME_FUNCTIONS_H_
#include "FvbServicesRuntimeDxe.h"


/**
  This function allows the caller to determine if UEFI SetVirtualAddressMap() has been called.

  This function returns TRUE after all the EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE functions have
  executed as a result of the OS calling SetVirtualAddressMap(). Prior to this time FALSE
  is returned. This function is used by runtime code to decide it is legal to access services
  that go away after SetVirtualAddressMap().

  @retval  TRUE  The system has finished executing the EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE event.
  @retval  FALSE The system has not finished executing the EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE event.
**/
BOOLEAN
GoneVirtual (
  VOID
  );

/**
  According to input base address to get next FWH instance.

  @param[in] FwhInstance          Pointer to EFI_FW_VOL_INSTANCE instance.

  @retunr EFI_FW_VOL_INSTANCE *   Start address of next FWH instance.
  @retval NULL                    Cannot find next FWH intance.
**/
EFI_FW_VOL_INSTANCE *
GetNextFwhInstance (
  IN EFI_FW_VOL_INSTANCE     *FwhInstance
  );

/**
  Routine to enable Access FVB through SMI feature.

  The mainly action of this routine is follows:
  1. Install gFvbAccessThroughSmiGuid to meet dependency to load image to SMM RAM.
  2. Create event to wait the completion of SMM image initialization.
  Note: If enabling access FVB through SMI, all of FVB access in this driver MUST
  through SMI. User should take attention to disable all of FVB access in protected
  mode.

  @retval EFI_SUCCESS      Enable FVB acccess through SMI feature successful.
  @retval EFI_UNSUPPORTED  System is in SMM mode or at runtime.
**/
EFI_STATUS
EnableFvbAccessThroughSmi (
  VOID
  );

/**
  Initialize all of FVB services and provide FVB services relative interfaces.

  Any error occurred in this function will be asserted.

  @retval EFI_SUCCESS  Initialize FVbServices successful.
**/
EFI_STATUS
InitializeFVbServices (
  VOID
  );



#endif
