/** @file
    This function offers an interface to get board information, ex. board id and fab id.

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
 This function offers an interface to get board id and fab id using hardware specified way.

 @param[out]        *BoardId                      Board Id get from current board.
 @param[out]        *FabId                        Fab Id get from current board.

 @retval            EFI_UNSUPPORTED               Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED             Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS                   The function performs the same operation as caller.
                                                  The caller will skip the specified behavior and assuming
                                                  that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcGetBoardInfo (
  OUT  UINT8   *BoardId,
  OUT  UINT8   *FabId
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/
  return EFI_UNSUPPORTED;
}  
//[-end-130613-IB05400415-modify]//