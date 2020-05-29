/** @file

  Header file of Store DEBUG() Message Smm implementation.

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

#ifndef _STORE_DEBUG_MESSAGE_SMM_H_
#define _STORE_DEBUG_MESSAGE_SMM_H_

//
// Statements that include other header files
//
#include <Protocol/VariableWrite.h>
#include <Protocol/SmmReportStatusCodeHandler.h>

#include <Library/SmmServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>

#include <Guid/StatusCodeDataTypeDebug.h>
#include <Guid/EventGroup.h>
#include <Guid/DebugMessageVariable.h>
//[-start-140326-IB08400258-add]//
#include <Guid/StatusCodeDataTypeId.h>
//[-end-140326-IB08400258-add]//

#define MAX_DEBUG_MESSAGE_LENGTH                0x100

EFI_STATUS
EFIAPI
DebugMessageMemBufferInit (
  VOID
  );

EFI_STATUS
EFIAPI
RscHandlerProtocolCallback (
  IN CONST EFI_GUID                       *Protocol,
  IN VOID                                 *Interface,
  IN EFI_HANDLE                           Handle
  );

EFI_STATUS
EFIAPI
DebugMessageStatusCode (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          CodeValue,
  IN UINT32                         Instance    OPTIONAL,
  IN EFI_GUID                       * CallerId  OPTIONAL,
  IN EFI_STATUS_CODE_DATA           * Data      OPTIONAL  
  ); 

EFI_STATUS
EFIAPI
StoreAsciiDebugMessage (
  IN CHAR8                                 *MessageString,
  IN UINTN                                 StringSize
  ); 

//[-start-140327-IB08400258-add]//
BOOLEAN
EFIAPI
ExtractAssertInfo (
  IN EFI_STATUS_CODE_TYPE        CodeType,
  IN EFI_STATUS_CODE_VALUE       Value,
  IN CONST EFI_STATUS_CODE_DATA  *Data,
  OUT CHAR8                      **Filename,
  OUT CHAR8                      **Description,
  OUT UINT32                     *LineNumber
  );

EFI_STATUS
EFIAPI
StoreAsciiDebugMessage (
  IN CHAR8                                 *MessageString,
  IN UINTN                                 StringSize
  ); 

//[-end-140327-IB08400258-add]//
#endif
