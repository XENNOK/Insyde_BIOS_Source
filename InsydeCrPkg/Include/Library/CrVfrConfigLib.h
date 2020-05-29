/** @file
  Console Redirection VFR config Definition Lib

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _CR_VFR_CONFIG_LIB_H_
#define _CR_VFR_CONFIG_LIB_H_

#include <Uefi.h>
#include <CrSetupConfig.h>

#include <Protocol/CRPolicy.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/KernelConfigLib.h>
#include <Protocol/ConsoleRedirectionService.h>
#include <Protocol/HiiConfigAccess.h>


EFI_STATUS
CrSerialDevOptionInit (
  IN EFI_HII_HANDLE               HiiHandle
  );

EFI_STATUS
CRScuAdvanceCallback (
  IN  EFI_QUESTION_ID             QuestionId
  );

EFI_STATUS
CRScuHotKeyCallback (
  IN  EFI_QUESTION_ID             QuestionId,
  IN  EFI_INPUT_KEY               Key
  );

EFI_STATUS
CRBdsScuInit (
  EFI_CR_POLICY_PROTOCOL         *CRPolicy
  );

EFI_STATUS
CrConfigInit (void);

EFI_STATUS
EFIAPI
CRConfigExtractCallBack (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  );


EFI_STATUS
EFIAPI
CRConfigRouteCallBack (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN CONST EFI_STRING                       Configuration,
  OUT      EFI_STRING                       *Progress
  );

#endif
