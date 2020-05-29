/** @file
  Provide hook function for OEM to retrieve EC version. 
  
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
 Get major/minor version from EC. 
 
 @param[out]        ReadEcVersionStatus The status of read EC version.
 @param[out]        MajorNum            EC major number.
 @param[out]        MinorNum            EC minor number.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcEcVersion (
  OUT EFI_STATUS   *ReadEcVersionStatus,
  OUT UINT8        *MajorNum,
  OUT UINT8        *MinorNum
  )
{
  *ReadEcVersionStatus = WaitKbcIbe (SMC_CMD_STATE);
  if (*ReadEcVersionStatus == EFI_SUCCESS) {
    *ReadEcVersionStatus = WriteKbc (SMC_CMD_STATE, EC_READ_VERSION_CMD);
  }

  if (*ReadEcVersionStatus == EFI_SUCCESS) {
    *ReadEcVersionStatus = WaitKbcObf (SMC_CMD_STATE);
    ReadKbc (SMC_DATA, MajorNum);
    *ReadEcVersionStatus = WaitKbcObf (SMC_CMD_STATE);
    ReadKbc (SMC_DATA, MinorNum);
  }

  return EFI_UNSUPPORTED;
}
