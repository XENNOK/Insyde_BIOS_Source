/** @file

  Header file of Store Debug Message PEI implementation.

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

#ifndef _STORE_DEBUG_MESSAGE_PEI_H_
#define _STORE_DEBUG_MESSAGE_PEI_H_

//
// Statements that include other files.
//
#include <Pi/PiPeiCis.h>

#include <Guid/DebugMessageVariable.h>
#include <Guid/StatusCodeDataTypeDebug.h>
//[-start-140326-IB08400258-add]//
#include <Guid/StatusCodeDataTypeId.h>
//[-end-140326-IB08400258-add]//

#include <Ppi/MemoryDiscovered.h>
#include <Ppi/ReportStatusCodeHandler.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>

#define MAX_DEBUG_MESSAGE_LENGTH                0x100

EFI_STATUS
EFIAPI
StoreAsciiDebugMessage (
  IN CHAR8                                 *MessageString,
  IN UINT32                                StringSize
  ); 

EFI_STATUS
EFIAPI
RscHandlerPpiNotifyCallback (
  IN CONST EFI_PEI_SERVICES         **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  );

EFI_STATUS
EFIAPI
PeiDebugMessageStatusCode (
  IN CONST EFI_PEI_SERVICES         **PeiServices,
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          CodeValue, 
  IN UINT32                         Instance  OPTIONAL,
  IN CONST EFI_GUID                 *CallerId OPTIONAL,
  IN CONST EFI_STATUS_CODE_DATA     *Data     OPTIONAL
  );

EFI_STATUS
EFIAPI
DebugMessageMemBufferInit (
  IN  EFI_PEI_SERVICES            **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
  IN  VOID                        *Ppi
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

BOOLEAN
EFIAPI
ExtractDebugInfo (
  IN CONST EFI_STATUS_CODE_DATA  *Data,
  OUT UINT32                     *ErrorLevel,
  OUT BASE_LIST                  *Marker,
  OUT CHAR8                      **Format
  );
//[-end-140327-IB08400258-add]//

#endif

