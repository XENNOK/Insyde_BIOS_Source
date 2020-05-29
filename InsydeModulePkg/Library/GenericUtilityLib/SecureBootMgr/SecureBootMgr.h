/** @file
  GenericUtilityLib

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SECURE_BOOT_MGR_H_
#define _SECURE_BOOT_MGR_H_

#include "InternalUtilityLib.h"
#include "SecureBootFormGuid.h"
#include "String.h"
#include <Protocol/FormBrowserEx.h>


#define SECURE_BOOT_CALLBACK_DATA_SIGNATURE  SIGNATURE_32 ('S', 'B', 'C', 'B')

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

  H2O_DIALOG_PROTOCOL              *H2ODialog;
  SECURE_BOOT_NV_DATA              SecureBootData;
  SECURE_BOOT_NV_DATA              BackupSecureBootData;
  UINTN                            EmDisplayContext;
} SECURE_BOOT_MANAGER_CALLBACK_DATA;


EFI_STATUS
EFIAPI
SecureBootCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  );

EFI_STATUS
EFIAPI
SecureBootExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  );

EFI_STATUS
EFIAPI
SecureBootRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  );

UINT8
EFIAPI
SmmSecureBootCall (
  IN     UINT8            *InPutBuff,       // rcx
  IN     UINTN            DataSize,         // rdx
  IN     UINT8            SubFunNum,        // r8
  IN     UINT16           SmiPort           // r9
  );


extern UINT8                              SecureBootMgrVfrBin[];
extern EFI_GUID                           mSecureBootGuid;
extern SECURE_BOOT_MANAGER_CALLBACK_DATA  mSecureBootPrivate;
extern LIST_ENTRY                         mHashLinkList;
#endif
