//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Module Name:

  AdmiSecureBoot.h

Abstract:

Revision History

--*/

#ifndef _SECURE_BOOT_MGR_H_
#define _SECURE_BOOT_MGR_H_
#include "Tiano.h"
#include "Bds.h"
#include "String.h"
#include "SecureBootFormGuid.h"
#include "SmiTable.h"

#include EFI_PROTOCOL_CONSUMER (OemFormBrowser2)

#define SECURE_BOOT_CALLBACK_DATA_SIGNATURE  EFI_SIGNATURE_32 ('S', 'B', 'C', 'B')

#define SECURE_BOOT_CALLBACK_DATA_FROM_THIS(a) \
  CR (a, \
      SECURE_BOOT_MANAGER_CALLBACK_DATA, \
      ConfigAccess, \
      SECURE_BOOT_CALLBACK_DATA_SIGNATURE \
      )

#define SECURE_BOOT_DATA_NAME    L"SecureBootData"

typedef struct {
  UINTN                           Signature;

  //
  // HII relative handles
  //
  EFI_HII_HANDLE                  HiiHandle;
  EFI_HANDLE                      DriverHandle;

  //
  // Produced protocols
  //
  EFI_HII_CONFIG_ACCESS_PROTOCOL   ConfigAccess;

  EFI_OEM_FORM_BROWSER2_PROTOCOL   *OemFormBrowser;
  SECURE_BOOT_NV_DATA              SecureBootData;
  SECURE_BOOT_NV_DATA              BackupSecureBootData;
  UINTN                            EmDisplayContext;
} SECURE_BOOT_MANAGER_CALLBACK_DATA;


EFI_STATUS
InitializeSecureBoot (
  VOID
  )
/*++

Routine Description:

  Initialize HII information for the FrontPage

Arguments:
  None

Returns:

--*/
;

VOID
CallSecureBootMgr (
  VOID
  );

EFI_STATUS
EFIAPI
SecureBootCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
/*++

Routine Description:
  This function processes the results of changes in configuration.

Arguments:
  This          - Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  Action        - Specifies the type of action taken by the browser.
  QuestionId    - A unique value which is sent to the original exporting driver
                  so that it can identify the type of data to expect.
  Type          - The type of value for the question.
  Value         - A pointer to the data being sent to the original exporting driver.
  ActionRequest - On return, points to the action requested by the callback function.

Returns:
  EFI_SUCCESS          - The callback successfully handled the action.
  EFI_OUT_OF_RESOURCES - Not enough storage is available to hold the variable and its data.
  EFI_DEVICE_ERROR     - The variable could not be saved.
  EFI_UNSUPPORTED      - The specified Action is not supported by the callback.

--*/
;

EFI_STATUS
EFIAPI
SecureBootExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
/*++

Routine Description:
  This function allows a caller to extract the current configuration for one
  or more named elements from the target driver.

Arguments:
  This       - Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  Request    - A null-terminated Unicode string in <ConfigRequest> format.
  Progress   - On return, points to a character in the Request string.
               Points to the string's null terminator if request was successful.
               Points to the most recent '&' before the first failing name/value
               pair (or the beginning of the string if the failure is in the
               first name/value pair) if the request was not successful.
  Results    - A null-terminated Unicode string in <ConfigAltResp> format which
               has all values filled in for the names in the Request string.
               String to be allocated by the called function.

Returns:
  EFI_SUCCESS           - The Results is filled with the requested values.
  EFI_OUT_OF_RESOURCES  - Not enough memory to store the results.
  EFI_INVALID_PARAMETER - Request is NULL, illegal syntax, or unknown name.
  EFI_NOT_FOUND         - Routing data doesn't match any storage in this driver.

--*/
;

EFI_STATUS
EFIAPI
SecureBootRouteConfig (
  IN  EFI_HII_CONFIG_ACCESS_PROTOCOL         *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
/*++

Routine Description:
  This function processes the results of changes in configuration.

Arguments:
  This          - Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  Configuration - A null-terminated Unicode string in <ConfigResp> format.
  Progress      - A pointer to a string filled in with the offset of the most
                  recent '&' before the first failing name/value pair (or the
                  beginning of the string if the failure is in the first
                  name/value pair) or the terminating NULL if all was successful.

Returns:
  EFI_SUCCESS           - The Results is processed successfully.
  EFI_INVALID_PARAMETER - Configuration is NULL.
  EFI_NOT_FOUND         - Routing data doesn't match any storage in this driver.

--*/
;

UINT8
SmmSecureBootCall (
  IN     UINT8            *InPutBuff,       // rcx
  IN     UINTN            DataSize,         // rdx
  IN     UINT8            SubFunNum,        // r8
  IN     UINT16           SmiPort           // r9
  );
/*++

Routine Description:
  This function according to sub-function number and SMI port triggers SMI to
  invoke relative secure boot relative function

Arguments:

  InPutBuff     - Points to input buffer.
  DataSize      - The size of input buffer.
  SubFunNum     - Secure Boot callback sub-function number.
  SmiPort       - SMI port number

Returns:

  0             - The callback successfully handled the action.
  other         - Some errors occured

--*/


extern UINT8                              SecureBootMgrVfrBin[];
extern EFI_GUID                           mSecureBootGuid;
extern SECURE_BOOT_MANAGER_CALLBACK_DATA  mSecureBootPrivate;
extern EFI_LIST_ENTRY                     mHashLinkList;
#endif
