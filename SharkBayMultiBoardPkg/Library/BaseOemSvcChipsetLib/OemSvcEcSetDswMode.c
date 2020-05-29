/** @file
  Provide hook function for OEM to set DSW mode to EC. 
  
;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/BaseOemSvcChipsetLib.h>
#include <Library/CommonEcLib.h>
#include <OemEc.h>

/**
 Set DSW mode to EC. 
 
 @param[out]        SetDswModeStatus    The status of set DSW mode.
 @param[in]         DswMode             DSW mode.
                                          0x0 Disalbe DSW mode
                                          0x1 Enable DSW in S5 (DC)
                                          0x3 Enable DSW in S4-S5 (DC)
                                          0x5 Enable DSW in S3-S4-S5 (DC)

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcEcSetDswMode (
  OUT EFI_STATUS   *SetDswModeStatus,
  IN  UINT8        DswMode
  )
{
  if (DswMode != DSW_DISABlE && DswMode != DSW_IN_S5 && DswMode != DSW_IN_S4_S5 && DswMode != DSW_IN_S3_S4_S5) {
    return EFI_INVALID_PARAMETER;
  }
  
  *SetDswModeStatus = WaitKbcIbe (SMC_CMD_STATE);
  if (*SetDswModeStatus == EFI_SUCCESS) {
    *SetDswModeStatus = WriteKbc (SMC_CMD_STATE, SET_DSW_MODE);
  }

  if (*SetDswModeStatus == EFI_SUCCESS) {
    *SetDswModeStatus = WaitKbcIbe (SMC_CMD_STATE); 
    if (*SetDswModeStatus == EFI_SUCCESS) {
      *SetDswModeStatus = WriteKbc (SMC_DATA, DswMode);
    }
  }

  *SetDswModeStatus = WaitKbcIbe (SMC_CMD_STATE);

  return EFI_UNSUPPORTED;
}
