/** @file

  Header file of POST Message SMM implementation.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _WATCH_DOG_REPORT_STATUS_CODE_SMM_H_
#define _WATCH_DOG_REPORT_STATUS_CODE_SMM_H_

//
// Statements that include other header files
//


#include <Library/SmmServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/SmmReportStatusCodeHandler.h>
#include <Library/DebugLib.h>


static
VOID
EFIAPI
RscHandlerProtocolCallback (
  IN EFI_EVENT   Event,
  IN VOID        *Context
  );

EFI_STATUS
EFIAPI
SmmWatchDogStatusCode (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          CodeValue,
  IN UINT32                         Instance    OPTIONAL,
  IN EFI_GUID                       * CallerId  OPTIONAL,
  IN EFI_STATUS_CODE_DATA           * Data      OPTIONAL  
  );

#endif
