/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _ADVANCE_CALLBACK_H
#define _ADVANCE_CALLBACK_H

#include <SetupUtility.h>
//[-start-110825-IB07370065-add]//
#include <DisplaySelection.h>
//[-end-110825-IB07370065-add]//
//[-start-130617-IB04560405-add]//
#include <Protocol/EventLogViewer.h>
//[-end-130617-IB04560405-add]//

#define SYSTEM_MEMORY_512M    0x200
#define SYSTEM_MEMORY_1G      0x400
#define MAXPCISLOT            2

extern  IDE_CONFIG                      *mIdeConfig;
EFI_STATUS
EFIAPI
AdvanceCallbackRoutine (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  );

EFI_STATUS
AdvanceCallbackRoutineByAction (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  );

EFI_STATUS
EFIAPI
InstallAdvanceCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle,
  IN EFI_HII_HANDLE                         HiiHandle
  );

EFI_STATUS
EFIAPI
UninstallAdvanceCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle
  );

EFI_STATUS
InitAdvanceMenu (
  IN EFI_HII_HANDLE                         HiiHandle
  );
EFI_STATUS
UpdateHDCConfigure (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  CHIPSET_CONFIGURATION              *Buffer
  );
  
EFI_STATUS
GetIdeDevNameString (
  IN OUT CHAR16 *                        *DevNameString
  );
EFI_STATUS  
InitIdeConfig (
  IDE_CONFIG                             *IdeConfig
  );

EFI_STATUS
UpdateSecurityStatus(
  IN  EFI_HII_HANDLE                    HiiHandle,
  IDENTIFY_TAB                          *IdentifyData,
  IN  UINT16                            HddPortNumber,
  IN  UINT8                             Device,
  IN  STRING_REF                        TokenToUpdate,
  IN  UINT16                            CallBackFormId,
  IN  CHAR8                             *LanguageString
  );
EFI_STATUS
UpdateGbePeiEPortSelect (
  IN  EFI_HII_HANDLE                      HiiHandle
  );

EFI_STATUS
IccOverClockingInit (
  IN  EFI_HII_HANDLE              HiiHandle
  );

EFI_STATUS
ResetIccClockForm (
  IN  EFI_HII_HANDLE              HiiHandle,
  IN  CHIPSET_CONFIGURATION        *MyIfrNVData,
  IN  EFI_QUESTION_ID             QuestionId,
  IN  BOOLEAN                     RefreshCurrentValueText
  );

EFI_STATUS
IccSettingsApply (
  IN  EFI_HII_HANDLE              HiiHandle,
  IN  CHIPSET_CONFIGURATION        *MyIfrNVData
  );

//[-start-120315-IB06460374-add]//
EFI_STATUS
IccProfileItemInit (
  IN  EFI_HII_HANDLE              HiiHandle
  );

EFI_STATUS
UpdateIccProfileDefaultValue (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  CHIPSET_CONFIGURATION              *Buffer
  );
//[-end-120315-IB06460374-add]//

BOOLEAN
CheckSioConflict (
  IN  CHIPSET_CONFIGURATION                    *MyIfrNVData
  );
  
BOOLEAN
CheckSioAndPciSoltConflict (
  IN  CHIPSET_CONFIGURATION                    *MyIfrNVData,
  IN  UINT8                                   *PciIrqData,
  OUT UINT8                                   *DeviceKind
  );

BOOLEAN
CheckPciSioConflict (
  IN  CHIPSET_CONFIGURATION                    *MyIfrNVData,
  IN  UINT8                                   *PciIrqData,
  OUT UINT8                                   *DeviceKind
  );

EFI_STATUS
CreateXtuOption (
  IN EFI_HII_HANDLE           HiiHandle
  );

//[-start-120816-IB05330371-add]//
EFI_STATUS
UpdateACPIDebugInfo (
  IN  EFI_HII_HANDLE                      HiiHandle
  );
//[-end-120816-IB05330371-add]//
#endif
