/** @file
 This function offers an interface to Hook IHISI Sub function AH=20h,function "FetsDoBeforeFlashing"
;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/SmmOemSvcChipsetLib.h>

/**
 This function offers an interface to Hook IHISI Sub function AH=20h,function "FetsDoBeforeFlashing"
 Hook function before flashing EC part.
 
 @param[in, out]    FlashingDataBuffer  Double pointer to data buffer.
 @param[in, out]    SizeToFlash         Data size by bytes want to flash.
 @param[in, out]    DestBlockNo         Dsstination block number.
 
 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcIhisiS20HookFetsDoBeforeFlashing (
  IN OUT UINT8      **FlashingDataBuffer,
  IN OUT UINTN      *SizeToFlash,
  IN OUT UINT8      *DestBlockNo
  )
{
/*++
  Todo:
  Add project specific code in here.
--*/

  return EFI_UNSUPPORTED;
}
