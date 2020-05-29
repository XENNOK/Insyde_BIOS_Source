/** @file
    This function offers an interface to get GPIO table.

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

#include <Library/PeiOemSvcChipsetLib.h>

//[-start-130613-IB05400415-modify]//
/**
 This function offers an interface to get GPIO Table.

 @param[out]        **GpioTable                   Pointer to correct GPIO table for current board.

 @retval            EFI_UNSUPPORTED               Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED             Alter the Configuration Parameter or hook code.

*/
EFI_STATUS
OemSvcGetGpioTable (
  OUT  GPIO_SETTINGS_TABLE   **GpioTable
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/
  return EFI_UNSUPPORTED;
}  
//[-end-130613-IB05400415-modify]//