/** @file
  Header file for Setup Utility Library common use

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

#ifndef _SETUP_UTILITY_LIB_COMMON_H_
#define _SETUP_UTILITY_LIB_COMMON_H_

#include <Library/SetupUtilityLib.h>
#include <OemServices/Kernel.h>
#include <Library/BvdtLib.h>
#include <Library/VariableLib.h>
#include "Boot.h"
#include "BootConfig.h"

#include <Protocol/IdeControllerInit.h>
#include <Protocol/SetupUtility.h>
#include <Protocol/FormBrowser2.h>

#include <Guid/DebugMask.h>

#define PRINTABLE_LANGUAGE_NAME_STRING_ID     0x0001

extern BOOT_CONFIGURATION                        mBootConfig;

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
  EFI_HII_HANDLE                        HiiHandle
  );

CHAR16 *
SetupUtilityLibGetTokenStringByLanguage (
  IN EFI_HII_HANDLE                             HiiHandle,
  IN STRING_REF                                 Token,
  IN CHAR8                                      *LanguageString
  );

EFI_STATUS
SetupUtilityLibGetLangDatabase (
  OUT UINTN            *LangNumber,
  OUT UINT8            **LanguageString
  );

EFI_STATUS
SetupUtilityLibUpdateDeviceString (
  IN     EFI_HII_HANDLE                         HiiHandle,
  IN OUT KERNEL_CONFIGURATION                   *KernelConfig
  );

typedef struct {
  UINTN    LangNum;
  UINT8    LangString[1];
  } LANGUAGE_DATA_BASE;

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
  IN     KERNEL_CONFIGURATION               *KernelConfig,
  OUT    STRING_PTR                         **BootTypeOrderString,
  OUT    STRING_PTR                         **AdvBootOrderString,
  OUT    STRING_PTR                         **LegacyAdvString,
  OUT    STRING_PTR                         **EfiBootOrderString
  );

extern BOOT_CONFIGURATION      mBootConfig;
#endif
