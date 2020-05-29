/** @file
  This driver provides IHISI interface in SMM mode

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "Fets.h"


/**
  AH=20h, Flash EC through SMI (FETS) Write.

  @retval EFI_SUCCESS    Flash EC successful.
  @return Other          Flash EC failed.
**/
EFI_STATUS
FetsWrite (
  VOID
  )
{
  EFI_STATUS        Status;
  UINT8             *FlashingDataBuffer;
  UINTN             SizeToFlash;
  UINT8             DestBlockNo;
  UINT8             ActionAfterFlashing;

  Status              = EFI_SUCCESS;
  FlashingDataBuffer  = NULL;
  SizeToFlash         = 0;
  DestBlockNo         = 0;
  ActionAfterFlashing = 0;

  //
  // Get flashing data from address stored in ESI
  //
  FlashingDataBuffer = (UINT8 *) (UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI);

  //
  // Get the size to flash from EDI
  //
  SizeToFlash = (UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDI);

  //
  // Get destined block number from CH
  //
  DestBlockNo = (UINT8) (IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RCX) >> 8);

  //
  // Get action after flashing from CL
  //
  ActionAfterFlashing = (UINT8) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RCX);

  if ((SizeToFlash == 0) || (FlashingDataBuffer == NULL)) {
    //
    // BUGBUG : Temporary use FBTS_READ_FAIL to report error. (Specification has no FETS error code definitions yet.)
    //
    IhisiLibErrorCodeHandler (FbtsReadFail);

    return EFI_LOAD_ERROR;
  }

  FetsDoBeforeFlashing (&FlashingDataBuffer, &SizeToFlash, &DestBlockNo);

  Status = FetsEcFlash (FlashingDataBuffer, SizeToFlash, DestBlockNo);

  if (EFI_ERROR (Status)) {
    //
    // BUGBUG : Temporary use FBTS_WRITE_FAIL to report error. ( Specification has no FETS error code definitions yet. )
    //
    IhisiLibErrorCodeHandler (FbtsWriteFail);
  } else {
    IhisiLibErrorCodeHandler (IhisiSuccess);
  }

  FetsDoAfterFlashing (ActionAfterFlashing);

  return Status;
}

/**
  AH=21h, Get EC part information.

  @retval EFI_SUCCESS     Get EC part information successful.
**/
EFI_STATUS
GetPartInfo (
  VOID
  )
{
//[-start-130828-IB12360019-remove]//
//  UINT8        EcBinSize;
//[-end-130828-IB12360019-remove]//
//[-start-130125-IB10820224-modify]//
  UINT32       IhisiStatus;
  UINT32       EcPartSize;
  EFI_STATUS   Status;

//[-start-130828-IB12360019-remove]//
//  EcBinSize = 0;
//[-end-130828-IB12360019-remove]//
  IhisiStatus = IhisiSuccess;
  EcPartSize = EC_PART_SIZE;
//[-start-130828-IB12360019-remove]//
//  //
//  // Get EC binary size from CH
//  //
//  EcBinSize = ( UINT8 ) (IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RCX) >> 8);
//[-end-130828-IB12360019-remove]//

//[-start-130524-IB05160451-modify]//
  Status = OemSvcIhisiS21HookFetsGetPartInfo (&IhisiStatus, &EcPartSize);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "SmmOemSvcChipsetLib OemSvcIhisiS21FetsGetPartInfoHook, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
  if (Status == EFI_SUCCESS) {
    return EFI_SUCCESS;
  }
  //
  // Return CL => Size of EC rom part
  //
  IhisiLibSetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RCX, EcPartSize);

  //
  // Return AL for funtion status
  //
  IhisiLibErrorCodeHandler (IhisiStatus);

//[-end-130125-IB10820224-modify]//
  return EFI_SUCCESS;
}