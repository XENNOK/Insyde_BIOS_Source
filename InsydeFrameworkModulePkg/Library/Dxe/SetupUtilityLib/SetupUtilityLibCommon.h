//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   SetupUtilityLibCommon.h
//;
//; Abstract:
//;
//;   Header file for Setup Utility Library common use
//;

#ifndef _SETUP_UTILITY_LIB_COMMON_H_
#define _SETUP_UTILITY_LIB_COMMON_H_

#include "SetupUtilityLib.h"
#include "Kernel.h"
#include "SetupUtilityLibStrDefs.h"
#include "BvdtLib.h"
#include "Boot.h"
#include EFI_PROTOCOL_DEFINITION (IdeControllerInit)

#include EFI_PROTOCOL_DEFINITION (EfiSetupUtility)
#include EFI_PROTOCOL_DEFINITION (OemFormBrowser)


typedef struct _STRING_PTR {
  UINT16                                    EfiBootDevFlag;
  UINT16                                    BootOrderIndex;
  CHAR16                                    *pString;
  BBS_TABLE                                 *BbsEntry;
  EFI_DEVICE_PATH_PROTOCOL                  *DevicePath;
} STRING_PTR;
//
// The following functioons are used in library internal
//
EFI_STATUS
GetSetupUtilityBrowserData (
  OUT SETUP_UTILITY_BROWSER_DATA   **SuBrowser
  );

EFI_STATUS
UpdatePasswordState (
  EFI_HII_PROTOCOL                      *Hii,
  EFI_HII_HANDLE                        HiiHandle
  );

VOID *
SetupUtilityLibGetVariableAndSize (
  IN  CHAR16              *Name,
  IN  EFI_GUID            *VendorGuid,
  OUT UINTN               *VariableSize
  );

CHAR16 *
SetupUtilityLibGetTokenStringByLanguage (
  IN EFI_HII_HANDLE                             HiiHandle,
  IN STRING_REF                                 Token,
  IN CHAR16                                     *LanguageString
  );

CHAR16 *
SetupUtilityLibGetTokenString (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  STRING_REF                        Token
  );

EFI_STATUS
SetupUtilityLibGetLangDatabase (
  OUT UINTN                             *LangNumber,
  OUT UINT16                            **LanguageString
  );

EFI_STATUS
SetupUtilityLibUpdateDeviceString (
  IN     EFI_HII_HANDLE                         HiiHandle,
  IN OUT SYSTEM_CONFIGURATION                   *SetupVariable
);

EFI_STATUS
SetupUtilityLibAsciiToUnicode (
  IN    CHAR8     *AsciiString,
  IN    CHAR16    *UnicodeString,
  IN    UINTN     Length
  );

EFI_STATUS
SetupUtilityLibUpdateAtaString(
  IN      EFI_IDENTIFY_DATA                 *IdentifyDriveInfo,
  IN      UINTN                             BufSize,
  IN OUT  CHAR16                            **BootString
  );

EFI_STATUS
BootOptionStrAppendDeviceName (
  IN     UINT16                         EfiBootDeviceNum,
  IN OUT STRING_PTR                     *UpdateEfiBootString
  );
  
EFI_STATUS
GetBbsName (
  IN     EFI_HII_HANDLE                     HiiHandle,
  IN     SYSTEM_CONFIGURATION               *SetupVariable,
  OUT    STRING_PTR                         **BootTypeOrderString,
  OUT    STRING_PTR                         **AdvBootOrderString,
  OUT    STRING_PTR                         **LegacyAdvString,
  OUT    STRING_PTR                         **EfiBootOrderString
  );  

#endif