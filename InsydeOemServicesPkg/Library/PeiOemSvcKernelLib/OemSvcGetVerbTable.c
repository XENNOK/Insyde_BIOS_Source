/** @file
  This OemService is part of setting Verb Table. The function is created for setting verb table 
  to support Multi-Sku and return the table to common code to program.

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

#include <Library/PeiOemSvcKernelLib.h>

/**
  This OemService is part of setting Verb Table. The function is created for setting verb table 
  to support Multi-Sku and return the table to common code to program.

  @param[out]  *VerbTableHeaderDataAddress    A pointer to VerbTable data/header

  @retval      EFI_UNSUPPORTED                Returns unsupported by default.
  @retval      EFI_SUCCESS                    The service is customized in the project.
  @retval      EFI_MEDIA_CHANGED              The value of IN OUT parameter is changed. 
  @retval      Others                         Depends on customization.
**/
EFI_STATUS
OemSvcGetVerbTable (
  OUT COMMON_CHIPSET_AZALIA_VERB_TABLE      **VerbTableHeaderDataAddress
  )
{
  /*++
    Todo:
      Get all of arguments by Macro VA_ARG.
  __*/

  return EFI_UNSUPPORTED;
}
