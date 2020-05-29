/** @file

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

#include <Platform.h>
#include <SmmPassword.h>
#include <Protocol/HddPasswordService.h>

//[-start-121023-IB11410011-add]//
#define DEFAULT_RETRY_COUNT                10
//[-end-121023-IB11410011-add]//

/**
 Check all harddisk security status.

 @param [in]   HddInfoPtr       Harddisk info data address.
 @param [in]   NumOfHdd         Number of harddisk.

 @retval TRUE                   Found harddisk is Frezon.
 @retval FALSE                  Could not found any harddisk is Frezon.

**/
BOOLEAN
ChekHddFrozen (
  IN HDD_SECURITY_INFO_DATA        *HddInfoPtr,
  IN UINTN                         NumOfHdd
  )
{
  UINTN Index;

  for (Index = 0; Index < NumOfHdd; Index++) {
     if ((HddInfoPtr[Index].HddSecurityStatus & HDD_SECURITY_FROZEN) == HDD_SECURITY_FROZEN) { 
       return TRUE;
     }
  }
  return FALSE;
}

/**
 Check all harddisk security status.

 @param [in]   HddInfoPtr       Harddisk info data address.
 @param [in]   NumOfHdd         Number of harddisk.

 @retval TRUE                   Found harddisk is Enable.
 @retval FALSE                  Could not found any harddisk is Enable.

**/
BOOLEAN
ChekHddSecurityEnable (
  IN HDD_SECURITY_INFO_DATA        *HddInfoPtr,
  IN UINTN                         NumOfHdd
  )
{ 
  UINTN Index;

  for (Index = 0; Index < NumOfHdd; Index++) {
    if ((HddInfoPtr[Index].HddSecurityStatus & HDD_SECURITY_ENABLE) == HDD_SECURITY_ENABLE) { 
       return TRUE;
    }
  }
  return FALSE;
}

/**
 Check all harddisk security status.

 @param [in]   HddInfoPtr       Harddisk info data address.
 @param [in]   NumOfHdd         Number of harddisk.

 @retval TRUE                   Found harddisk is locked.
 @retval FALSE                  Could not found any harddisk is locked.

**/
BOOLEAN
ChekHddLock (
  IN HDD_SECURITY_INFO_DATA        *HddInfoPtr,
  IN UINTN                         NumOfHdd
  )
{ 
  UINTN Index;

  for (Index = 0; Index < NumOfHdd; Index++) { 
     if ((HddInfoPtr[Index].HddSecurityStatus & HDD_SECURITY_LOCK) == HDD_SECURITY_LOCK) { 
       return TRUE;
     }
  }
  return FALSE;
}

/**
 Check Harddisk password.

 @param [in]   ResumeCheckFlag  if ResumeCheckFlag == 0x01 the show password dialog
                                and waiting user pass current password.
                                if  != 0x01 only call unlock harddisk
                                servie routine

 @retval EFI_STATUS

**/
EFI_STATUS
CheckHarddiskPassword (
  IN UINT8                         ResumeCheckFlag
  )
{ 
  EFI_HDD_PASSWORD_SERVICE_PROTOCOL   *SmmHddPswdService;
  EFI_STATUS                          Status;
  HDD_SECURITY_INFO_DATA              *HddInfoPtr;
  UINTN                               NumOfHarddisk;
  UINTN                             Index;
  IDENTIFY_TAB                      *IdentifyData;
//[-start-120905-IB08520056-add]//
  UINTN                               RetryCount;

  RetryCount = 0;
//[-end-120905-IB08520056-add]//
  HddInfoPtr = NULL;
  IdentifyData = NULL;
  
  Status = mSmst->SmmLocateProtocol(
                     &gEfiHddPasswordServiceProtocolGuid,
                     NULL,
                     (VOID **)&SmmHddPswdService
                     );
  if (EFI_ERROR(Status)) { 
    return Status;
  }
  
  //
  // Get Hddinfo table
  //

  SmmHddPswdService->GetHddSecurityInfo (
                       SmmHddPswdService,
                       (UINTN **)&HddInfoPtr
                       );
  
  NumOfHarddisk = HddInfoPtr->TotalNumOfHddInfo;

  if (NumOfHarddisk == 0) {
    //
    // Not found any Harddisk
    //
    return EFI_SUCCESS;
  }

  if (ChekHddSecurityEnable (HddInfoPtr, NumOfHarddisk) == FALSE) {
    //
    // If harddisk is not set password then don't do anything
    //
    return EFI_SUCCESS;
  }

  Status = mSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof(IDENTIFY_TAB),
                    (VOID **)&IdentifyData
                    );
  SmmHddPswdService->HddReset (SmmHddPswdService);

  for (Index = 0; Index < NumOfHarddisk; Index++) {
//[-start-120905-IB08520056-modify]//
    for (RetryCount = 0; RetryCount < DEFAULT_RETRY_COUNT; RetryCount++) {
      Status = SmmHddPswdService->HddIdentify (
                 SmmHddPswdService,
                 &HddInfoPtr[Index],
                 (UINT16 *)IdentifyData
                 );
      if (!EFI_ERROR (Status)) {
        break;
      }
    }
//[-end-120905-IB08520056-modify]//
    //
    // Update the Hdd status after S3 resume
    //
    HddInfoPtr[Index].HddSecurityStatus = (UINT8) IdentifyData->security_status;
  }
  mSmst->SmmFreePool (IdentifyData);

  if (ChekHddLock(HddInfoPtr,NumOfHarddisk) ==  FALSE) {
    //
    // Not found any security enable harddisk
    //

    return EFI_SUCCESS;
  } else {
//[-start-121102-IB05280008-modify]//
    Status = SmmHddPswdService->UnlockAllHdd (
                                  SmmHddPswdService
                                  );
//[-end-121102-IB05280008-modify]//
    return Status;
  }
}

/**
 S3 resume password entry.

 @param [in]   ResumeCheckFlag  if ResumeCheckFlag == 0x01 the show password dialog
                                and waiting user pass current password.
                                if  != 0x01 only call unlock harddisk
                                servie routine

 @retval EFI_SUCCESS            Always return.

**/
EFI_STATUS
CheckPasswordEntry( 
  IN UINT8 ResumeCheckFlag
  ) 
{
  EFI_STATUS     Status;
  
  //
  // Check Harddisk Password
  //
  Status = CheckHarddiskPassword(ResumeCheckFlag);
  
  return Status;
}
