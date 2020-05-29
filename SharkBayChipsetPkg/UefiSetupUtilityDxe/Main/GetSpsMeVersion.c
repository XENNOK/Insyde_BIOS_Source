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

#include "PlatformInfo.h"

#include <Library/SpsBiosLib.h>

EFI_STATUS
GetSpsMeVersion (
  IN VOID                              *OpCodeHandle,
  IN EFI_HII_HANDLE                    MainHiiHandle,
  IN EFI_HII_HANDLE                    AdvanceHiiHandle,
  IN CHAR16                            *StringBuffer
  )
{
  EFI_STATUS                        Status;
  STRING_REF                        SpsMeFwVersionString;
  STRING_REF                        SpsMeVersionString;
//[-start-140625-IB05080432-modify]//
  UINT16                            FwVerMinor = 0;
  UINT16                            FwVerMajor = 0;
  UINT16                            FwVerBuild = 0;
  UINT16                            FwVerPatch = 0;
  UINT16                            FwVerSku = 0;
//[-end-140625-IB05080432-modify]//
  CHAR16                            *SpsMeFwSkuString[2] = {L"Silicon Enabling", L"Node Manager"};

  Status = EFI_UNSUPPORTED;
  if (!FeaturePcdGet (PcdSpsMeSupported)) {
    return Status;
  }

  Status = SpsBiosGetFwVersion (&FwVerMinor, &FwVerMajor, &FwVerBuild, &FwVerPatch, &FwVerSku);
  if (!EFI_ERROR (Status)) {
    UnicodeSPrint (
      StringBuffer,
      0x100,
      L"%02d.%02d.%02d.%03d / %s",
      (UINTN) (UINT16) FwVerMajor,
      (UINTN) (UINT16) FwVerMinor,
      (UINTN) (UINT16) FwVerPatch,
      (UINTN) (UINT16) FwVerBuild,
      SpsMeFwSkuString[FwVerSku]
      );
  } else {
    StringBuffer = HiiGetString (AdvanceHiiHandle, STRING_TOKEN (STR_ME_VERSION), NULL);
  }

  SpsMeVersionString = HiiSetString (MainHiiHandle, 0, StringBuffer, NULL); 
  StringBuffer       = HiiGetString (AdvanceHiiHandle, STRING_TOKEN (STR_ME_FW_VERSION_STRING), NULL);

  SpsMeFwVersionString = HiiSetString (MainHiiHandle, 0, StringBuffer, NULL); 

  HiiCreateTextOpCode (OpCodeHandle, SpsMeFwVersionString, 0, SpsMeVersionString );

  return EFI_SUCCESS;
}
