/** @file

  StartWatchDog header file

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

#ifndef _EFI_START_WATCH_DOG_H_
#define _EFI_START_WATCH_DOG_H_

#include <Uefi.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/DebugLib.h>
#include <Ppi/MasterBootMode.h>
#include <Library/Amt/Pei/PeiAmtLib.h>
#include <PeiMeLib.h>
#include <Ppi/Heci.h>

///
/// Function Prototypes
///
/**
  Perform the platform spefific initializations.

  @param  FileHandle              Handle of the file being invoked. 
  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_SUCCESS             if the interface could be successfully installed.
**/
EFI_STATUS
EFIAPI
PeiInitStartWatchDog (
  IN        EFI_PEI_FILE_HANDLE         FileHandle,
  IN CONST EFI_PEI_SERVICES             **PeiServices
  );

#endif /// _EFI_START_WATCH_DOG_H_
