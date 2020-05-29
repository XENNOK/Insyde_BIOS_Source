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
  /*++
    Todo:
    Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
//[-end-130613-IB05400415-modify]//
