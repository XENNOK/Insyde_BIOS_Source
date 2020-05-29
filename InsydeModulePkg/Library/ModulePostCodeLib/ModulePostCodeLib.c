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
#include <Library/UefiLib.h>
#include <Library/PcdLib.h>
#include <Library/PostCodeLib.h>


EFI_STATUS
EFIAPI
ModulePOST_CODELibConstructor (
  VOID
  )
{
  if (PcdGet32(PcdModuleEntryPOST_CODEValue) != 0) {
    POST_CODE (PcdGet32(PcdModuleEntryPOST_CODEValue));
  }
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
ModulePOST_CODELibDestructor (
  VOID
  )
{
  if (PcdGet32(PcdModuleExitPOST_CODEValue) != 0) {
    POST_CODE (PcdGet32(PcdModuleExitPOST_CODEValue));
  }
  return EFI_SUCCESS;
}
