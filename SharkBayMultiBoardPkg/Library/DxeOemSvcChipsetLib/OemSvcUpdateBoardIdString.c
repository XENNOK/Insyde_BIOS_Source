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
#include <Library/PrintLib.h>
#include <OemBoardId.h>


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

  switch (BoardId) {
  case V_EC_BOARD_ID_FLATHEAD_CREEK_CRB:
    UnicodeSPrint (StringBuffer, StringBufferSize, L"Flathead Creek CRB");
    break;

  case V_EC_BOARD_ID_HADDOCK_CREEK:
    UnicodeSPrint (StringBuffer, StringBufferSize, L"Haddock Creek");
    break;

  case V_EC_BOARD_ID_GRAYS_REEF:
    UnicodeSPrint (StringBuffer, StringBufferSize, L"Grays Reef");
    break;

  case V_EC_BOARD_ID_BASKING_RIDGE:
    UnicodeSPrint (StringBuffer, StringBufferSize, L"Basking Ridge");
    break;

  case V_EC_BOARD_ID_WHITE_TIP_MOUNTAIN:
    UnicodeSPrint (StringBuffer, StringBufferSize, L"White Tip Mountain");
    break;

  case V_EC_BOARD_ID_SAWTOOTH_PEAK:
    UnicodeSPrint (StringBuffer, StringBufferSize, L"Sawtooth Peak");
    break;

  case V_EC_BOARD_ID_HARRIS_BEACH:
    UnicodeSPrint (StringBuffer, StringBufferSize, L"Harris Beach");
    break;

  case V_EC_BOARD_ID_WHITE_TIP_MOUNTAIN2:
    UnicodeSPrint (StringBuffer, StringBufferSize, L"White Tip Mountain 2");
    break;

  case V_CRB_BOARD_ID_AGATE_BEACH_CRB:
    UnicodeSPrint (StringBuffer, StringBufferSize, L"Agate Beach (Denlow Server)");
    break;

  default:
    break;
  }

  return EFI_MEDIA_CHANGED;

}  
//[-end-130613-IB05400415-modify]//