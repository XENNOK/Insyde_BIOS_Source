/** @file

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

#include <Uefi.h>
#include <BiosId.h>

#pragma pack(16)
CHAR8 BiosId[] = BIOS_ID;
#pragma pack()

/**
  Initialization routine for BiosIdLib

  @param  None

  @retval EFI_SUCCESS           BiosIdLib successfully initialized

**/
EFI_STATUS
EFIAPI
BiosIdInit (
  VOID
  )
{
  return EFI_SUCCESS;
}
