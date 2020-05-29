/** @file
  Firmware authentication routines
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

EFI_STATUS
VerifyFirmware (
  IN UINT8                      *FirmwareData,
  IN UINTN                      FirmwareFileSize
  )
{
  return EFI_SUCCESS;
}  
