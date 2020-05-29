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
#include <OemBoardId.h>

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

  EFI_STATUS   Status;

  if (FeaturePcdGet (PcdDenlowServerSupported)) {
    *GpioTable = (GPIO_SETTINGS_TABLE *)PcdGetPtr (PcdPeiGpioTable2);
    return EFI_MEDIA_CHANGED;
  }

  Status = EFI_MEDIA_CHANGED;
  
  switch (PcdGet8 (PcdCurrentBoardId)) {
  case V_EC_BOARD_ID_FLATHEAD_CREEK_CRB:
  case V_EC_BOARD_ID_HADDOCK_CREEK:
    *GpioTable = (GPIO_SETTINGS_TABLE *)PcdGetPtr (PcdPeiGpioTable1);
    break;
    
  case V_EC_BOARD_ID_WHITE_TIP_MOUNTAIN:
  case V_EC_BOARD_ID_WHITE_TIP_MOUNTAIN2:
    *GpioTable = (GPIO_SETTINGS_TABLE *)PcdGetPtr (PcdPeiGpioTable3);
    break;

  case V_EC_BOARD_ID_SAWTOOTH_PEAK:
    *GpioTable = (GPIO_SETTINGS_TABLE *)PcdGetPtr (PcdPeiGpioTable4);
    break;

  case V_EC_BOARD_ID_HARRIS_BEACH:
    *GpioTable = (GPIO_SETTINGS_TABLE *)PcdGetPtr (PcdPeiGpioTable5);
    break;

  default:
    Status = EFI_UNSUPPORTED;
    break;

  }
  return Status;
}  
//[-end-130613-IB05400415-modify]//