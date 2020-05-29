/** @file
 Media Player library header file.

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


#ifndef _MEDIA_PLAYER_LIB_H_
#define _MEDIA_PLAYER_LIB_H_

#include <PiDxe.h>

/**
  Play media file during BIOS POST.
  
  @param[in]  None

  @retval EFI_SUCCESS                  Succeed to play media file.
  @retval EFI_UNSUPPORTED              Unsupport to play media file.
**/
EFI_STATUS
PlayMediaFileDuringPost (
  VOID
  );

#endif 
 
  
