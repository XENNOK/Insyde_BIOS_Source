/** @file
  Provide hook function for OEM to get lid state from EC. 
  
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
 Get Lid state from EC.
 
 @param[out]        EcGetLidState       The status of get Lid.
 @param[out]        LidIsOpen           TRUE: Lid is open; FALSE: Lid is close.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcEcGetLidState (
  OUT EFI_STATUS   *EcGetLidState,
  OUT UINT8        *LidIsOpen
  )
{
  UINT8       Data;
  
  Data = 0;
  *EcGetLidState = EFI_SUCCESS;

  //
  // Write EC Command
  //  
  *EcGetLidState = WaitKbcIbe (SMC_CMD_STATE);
  if (!EFI_ERROR (*EcGetLidState)) {
    WriteKbc (SMC_CMD_STATE, EC_READ_ECRAM_CMD);
  } else {
    return *EcGetLidState;
  }

  //
  // Set Lid status offset 0x03 to DATA port
  //
  *EcGetLidState = WaitKbcIbe (SMC_CMD_STATE);
  if (!EFI_ERROR (*EcGetLidState)) {
    WriteKbc (SMC_DATA, EC_RAM_LID_STATUS_OFFSET);
  } else {
    return *EcGetLidState;
  }

  //
  // Get DATA from ECRAM
  //
  *EcGetLidState = WaitKbcObf (SMC_CMD_STATE);
  if (!EFI_ERROR (*EcGetLidState)) {
    *EcGetLidState = ReadKbc (SMC_DATA, &Data);
  } else {
    return *EcGetLidState;
  }

  //
  // Return Lid state
  //
  if (!EFI_ERROR(*EcGetLidState)) {
    if (Data & B_EC_LID_STATUS) {
      *LidIsOpen = TRUE;
    } else {
      *LidIsOpen = FALSE;
    }
  }

  return EFI_UNSUPPORTED;
}
