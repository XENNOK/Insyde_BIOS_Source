/** @file
    This function offers an interface to update board id string.

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
 This function offers an interface to update board id string.

 @param[in]         BoardId                       Current board id.
 @param[out]        *StringBuffer                 String buffer will store board id string.
 @param[in]         StringBufferSize              String buffer size.
 @param [in]        SrcHiiHandle                  New Hii Handle.
 @param [in]        SrcStringId                   New String Token.
 @param [in]        DstHiiHandle                  Hii Handle of the package to be updated.

 @retval            EFI_UNSUPPORTED               Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED             Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS                   The function performs the same operation as caller.
                                                  The caller will skip the specified behavior and assuming
                                                  that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdateBoardIdString (
  IN   UINT8                    BoardId,
  OUT  CHAR16                   *StringBuffer,
  IN   UINTN                    StringBufferSize,
  IN   EFI_HII_HANDLE           SrcHiiHandle,
  IN   EFI_STRING_ID            SrcStringId,
  IN   EFI_HII_HANDLE           DstHiiHandle
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/
  return EFI_UNSUPPORTED;
}  
//[-end-130613-IB05400415-modify]//