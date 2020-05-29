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

#ifndef _SETUP_FUNCS_H_
#define _SETUP_FUNCS_H_

#include <SetupUtility.h>
#include <SaAccess.h>
#include <Library/BaseLib.h>

#include <Protocol/MemInfo.h>
#include <Protocol/IdeControllerInit.h>
//[-start-130110-IB10820219-add]//
#include <PortNumberMap.h>
//[-end-130110-IB10820219-add]//

#define CONFIG_TDP_NUM_LEVELS_MASK        ( BIT34 | BIT33 )

typedef struct {
  UINTN    LangNum;
  UINT8    LangString[1];
  } LANGUAGE_DATA_BASE;
//[-start-120509-IB02960446-remove]//
//typedef enum {
//  DlgYesNo,
//  DlgYesNoCancel,
//  DlgOk,
//  DlgOkCancel
//} UI_DIALOG_OPERATION;
//[-end-120509-IB02960446-remove]//

EFI_STATUS
LoadCustomOption (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL        *This
  );

EFI_STATUS
SaveCustomOption (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL        *This
  );

EFI_STATUS
DiscardChange (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL        *This
  );


VOID *
GetVariableAndSize (
  IN  CHAR16              *Name,
  IN  EFI_GUID            *VendorGuid,
  OUT UINTN               *VariableSize
  );

EFI_STATUS
CheckLanguage (
  VOID
  );

EFI_STATUS
UpdateAtaString(
  IN      EFI_ATAPI_IDENTIFY_DATA     *IdentifyDriveInfo,
  IN OUT  CHAR16                      **NewString
  );

EFI_STATUS
AsciiToUnicode (
  IN    CHAR8     *AsciiString,
  IN    CHAR16    *UnicodeString
  );
EFI_STATUS
EventTimerControl (
  IN UINT64                     Timeout
  );

EFI_STATUS
SearchMatchedPortNum (
  IN     UINT32                              Bus,
  IN     UINT32                              Device,
  IN     UINT32                              Function,
  IN     UINT8                               PrimarySecondary,
  IN     UINT8                               SlaveMaster,
  IN OUT UINTN                               *PortNum
  );

EFI_STATUS
CheckSataPort (
  IN UINTN                                  PortNum
  );


CHAR16 *
GetTokenStringByLanguage (
  IN EFI_HII_HANDLE                             HiiHandle,
  IN STRING_REF                                 Token,
  IN CHAR8                                      *LanguageString
  );
EFI_STATUS
SaveSetupConfig (
  IN     CHAR16             *VariableName,
  IN     EFI_GUID           *VendorGuid,
  IN     UINT32             Attributes,
  IN     UINTN              DataSize,
  IN     VOID               *Buffer
  );

EFI_STATUS
GetLangDatabase (
  OUT UINTN            *LangNumber,
  OUT UINT8            **LanguageString
  );
EFI_STATUS
AddNewString (
  IN   EFI_HII_HANDLE           InputHiiHandle,
  IN   EFI_HII_HANDLE           OutputHiiHandle,
  IN   STRING_REF               InputToken,
  OUT  STRING_REF               *OutputToken
  );

EFI_STATUS
DefaultSetup (
  OUT CHIPSET_CONFIGURATION          *SetupNvData
  );

//[-start-120109-IB03090372-add]//
VOID
ClearSetupVariableInvalid (
  VOID
  );
//[-end-120109-IB03090372-add]//
EFI_STATUS
SetupRuntimeDetermination (
  IN OUT  CHIPSET_CONFIGURATION        *SetupNvData
  );

EFI_STATUS
PEGDetect (
  IN  CHIPSET_CONFIGURATION                  *SetupNvData
  );

UINT8
MeFwTypeDetect (
  );
#endif
