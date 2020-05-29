/** @file
  Update Bbs table.

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

#include <Library/DxeOemSvcKernelLib.h>


/**
  Update Bbs table.

  @param[in, out]  *EfiToLegacy16BootTable   Pointer to Legacy16BootTable
  @param[in, out]  *BbsTable                 Pointer to BBS table

  @retval          EFI_UNSUPPORTED           Returns unsupported by default.
  @retval          EFI_SUCCESS               The service is customized in the project.
  @retval          EFI_MEDIA_CHANGED         The value of IN OUT parameter is changed. 
  @retval          Others                    Base on OEM design.
**/
EFI_STATUS
OemSvcUpdateBbsTable (
  IN OUT EFI_TO_COMPATIBILITY16_BOOT_TABLE    *EfiToLegacy16BootTable,
  IN OUT BBS_TABLE                            *BbsTable
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
