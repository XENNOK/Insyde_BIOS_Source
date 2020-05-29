/** @file
 SMM Chipset Services Library.

 This file contains only one function that is SmmCsSvcResetSystem().
 The function SmmCsSvcResetSystem() use chipset services to reset system.
  
***************************************************************************
* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************

*/

#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <PchRegs.h>

/**
 Resets the system.

 @param[in]         ResetType           The type of reset to perform.
                                
 @retval            EFI_UNSUPPORTED     ResetType is invalid.
 @retval            others              Error occurs
*/
EFI_STATUS
EFIAPI
ResetSystem (
  IN EFI_RESET_TYPE   ResetType
  )
{
  UINT8       InitialData;
  UINT8       OutputData;

  switch (ResetType) {
  case EfiResetWarm:
    InitialData  = V_PCH_RST_CNT_SOFTSTARTSTATE;
    OutputData   = V_PCH_RST_CNT_SOFTRESET;
    break;

  case EfiResetCold:
    InitialData  = V_PCH_RST_CNT_HARDSTARTSTATE;
    OutputData   = V_PCH_RST_CNT_HARDRESET;
    break;

  default:
    return EFI_UNSUPPORTED;
  }

  IoWrite8 ((UINT16)R_PCH_RST_CNT, (UINT8)InitialData);
  IoWrite8 ((UINT16)R_PCH_RST_CNT, (UINT8)OutputData);

  //
  // Given we should have reset getting here would be bad
  //
  CpuDeadLoop ();
  return EFI_DEVICE_ERROR;
}
