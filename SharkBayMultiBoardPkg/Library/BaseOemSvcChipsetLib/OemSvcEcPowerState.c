/** @file
  Provide hook function for OEM to implement EC power state detection. 
  
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
 Get power state from EC.  If power state cannot be determined,
 battery powered is assumed.
 
 @param[in, out]    PowerStateIsAc      A boolean pointer. TRUE means AC power; FALSE means Battery power.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcEcPowerState (
  IN  BOOLEAN                       *PowerStateIsAc
  )
{
  EFI_STATUS  Status;
  UINT8       PortDataOut;
    
  //
  // Query EC for system status
  //
  PortDataOut = 0;
  
  Status = WaitKbcIbe(SMC_CMD_STATE);
  if(Status == EFI_SUCCESS){
     Status = WriteKbc(SMC_CMD_STATE,QUERY_SYS_STATUS);
  } 
  
  //
  // Read status data
  //
  if (!EFI_ERROR (Status)) {
     Status = WaitKbcObf(SMC_CMD_STATE);
     ReadKbc(SMC_DATA,&PortDataOut);
  }

  //
  // Return power state
  // If we had an issue, we default to battery mode to save a small amount of power.
  //
  if (!EFI_ERROR (Status) && PortDataOut & B_SYS_STATUS_AC) {
    *PowerStateIsAc = TRUE;
  } else {
    *PowerStateIsAc = FALSE;
  }
  return EFI_UNSUPPORTED;
}
