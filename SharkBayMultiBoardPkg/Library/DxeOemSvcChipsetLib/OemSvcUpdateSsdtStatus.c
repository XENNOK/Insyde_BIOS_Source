/** @file
  This function provides an interface to update SSDT install status.
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

#include <Library/DxeOemSvcChipsetLib.h>
#include <Library/PcdLib.h>
#include <OemBoardId.h>


//[-start-130613-IB05400415-modify]//
/**
 This function provides an interface to update SSDT install status.

 @param[in]         OemTableId          OEM table id of this table.
 @param[in]         SetupVariable       Pointer to chipset configuration structure.
 @param[out]        NewStatus           Pointer to updated status.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
*/
EFI_STATUS
OemSvcUpdateSsdtStatus (
  IN  UINT64                            OemTableId,
  IN  CHIPSET_CONFIGURATION             *SetupVariable,
  OUT EFI_STATUS                        *NewStatus
  )
{
  EFI_STATUS  Status;


  Status = EFI_UNSUPPORTED;
  switch (OemTableId) {

  case SIGNATURE_64 ('U', 'l', 't', '0', 'R', 't', 'd', '3'):
    if (SetupVariable->RuntimeDevice3 && 
        (PcdGet8 (PcdCurrentBoardId) == V_EC_BOARD_ID_WHITE_TIP_MOUNTAIN ||
         PcdGet8 (PcdCurrentBoardId) == V_EC_BOARD_ID_SAWTOOTH_PEAK)) {
      *NewStatus = EFI_SUCCESS;
      Status = EFI_MEDIA_CHANGED;
    }
    break;

  case SIGNATURE_64 ('H', 'S', 'W', '-', 'F', 'F', 'R', 'D'):
    if (SetupVariable->RuntimeDevice3 &&
        PcdGet8 (PcdCurrentBoardId) == V_EC_BOARD_ID_HARRIS_BEACH) {
      *NewStatus = EFI_SUCCESS;
      Status = EFI_MEDIA_CHANGED;
    }
    break;

  case SIGNATURE_64 ('B', 'R', '0', '_', 'R', 't', 'd', '3'):
    if (SetupVariable->RuntimeDevice3 && 
        PcdGet8 (PcdCurrentBoardId) == V_EC_BOARD_ID_BASKING_RIDGE) {
      *NewStatus = EFI_SUCCESS;
      Status = EFI_MEDIA_CHANGED;
    }
    break;
    
  }

  return Status;
}
//[-end-130613-IB05400415-modify]//
