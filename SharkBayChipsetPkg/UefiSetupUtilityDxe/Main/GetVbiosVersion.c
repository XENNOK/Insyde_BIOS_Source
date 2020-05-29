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

#include <PlatformInfo.h>

/**
 To display VBIOS reversion:

 @param        OpCodeHandle
 @param [in]   MainHiiHandle
 @param [in]   AdvanceHiiHandle
 @param [in]   StringBuffer

 @retval EFI_SUCCESS            To display VBIOS OpRom ver. is successful.

**/
EFI_STATUS
GetVbiosVersion (
  IN   VOID                       *OpCodeHandle,
  IN     EFI_HII_HANDLE           MainHiiHandle,
  IN     EFI_HII_HANDLE           AdvanceHiiHandle,
  IN     CHAR16                   *StringBuffer
  )
{
  EFI_STATUS                   Status;
  BOOLEAN                      GetVBIOSVersionStatus;
  CHAR16                       *TempStringBuffer;
//  CHAR16                       *TempStringBuffer2;
  STRING_REF                   VbiosVersionText;
  STRING_REF                   VbiosVersionString;
  EFI_IA32_REGISTER_SET        RegisterSet;
  EFI_LEGACY_BIOS_PROTOCOL     *LegacyBios;

  LegacyBios            = NULL;
  TempStringBuffer      = NULL;
  GetVBIOSVersionStatus = FALSE;

  Status = gBS->LocateProtocol (
                  &gEfiLegacyBiosProtocolGuid,
                  NULL,
                  (VOID **)&LegacyBios
                  );
  
  if (EFI_ERROR (Status)) {
    return Status;
  }

  TempStringBuffer = AllocateZeroPool (0x100);
//[-start-130207-IB10870073-add]//
  ASSERT (TempStringBuffer != NULL);
  if (TempStringBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//
  
  ZeroMem (&RegisterSet, sizeof (EFI_IA32_REGISTER_SET));

  //
  // To know more about VBIOS function call, please refer to Intel 442269_442269_VBIOS.pdf
  //
//  RegisterSet.X.AX = VBE_OEM_EXTENSION_SERVICES;
//  RegisterSet.X.BX = GET_VBIOS_VERSION_SUBFUNCTION;  
  RegisterSet.X.AX = 0x5F01;
  LegacyBios->Int86 (LegacyBios, LEGACY_BIOS_INT_10, &RegisterSet);
  
  //
  // Reference to Intel 442269_442269_VBIOS.pdf page_57, it says 
  // AX = Return Status (function not supported if AL != 4Fh):
  //    = 004Fh, Function call successful
  //    = 014Fh, Function call failed
  //    = 024Fh, Function is not supported in current hardware configuration
  //    = 034Fh, Function call invalid in current video mode
  //
//  if (RegisterSet.X.AX == 0x004F) {
  if (RegisterSet.X.AX == 0x005F) {
    //
    // When AX returns 0x004F, it means function call successful, and then we  
    // set GetVBIOSVersion = TRUE, get VBIOS Version from CX which value returns
    // into. 
    //
    GetVBIOSVersionStatus = TRUE;
  }
  
  if (GetVBIOSVersionStatus == TRUE) {
    //
    // Reference to Intel 442269_442269_VBIOS.pdf page_76,
    // Return Registers:
    // AX = VESA VBE Return Status (see section 7.1)
    // CX = ASCII String of Build Number
    // EDX = 0
    //
    // Get 1st 2 digits of VBIOS version from CH. ie: 2102 --> Get 21.
    //
    //UnicodeValueToString (StringBuffer, PREFIX_ZERO, RegisterSet.H.CH, 2);
    
    //
    // Get 2nd 2 digits of VBIOS version from CL. ie: 2102 --> Get 02.
    //
    //UnicodeValueToString (TempStringBuffer, PREFIX_ZERO, RegisterSet.H.CL, 2);	
    //TempStringBuffer2= AllocateZeroPool (StrSize(StringBuffer)+StrSize(TempStringBuffer));
    //StrCat(TempStringBuffer2, StringBuffer);
    //StrCat(TempStringBuffer2, TempStringBuffer);
    StringBuffer[0] = (CHAR16)((RegisterSet.E.EBX >> 24) & 0xFF);
    StringBuffer[1] = (CHAR16)((RegisterSet.E.EBX >> 16) & 0xFF);
    StringBuffer[2] = (CHAR16)((RegisterSet.E.EBX >> 8) & 0xFF);
    StringBuffer[3] = (CHAR16)((RegisterSet.E.EBX) & 0xFF);

    //
    // Update VBIOS Version to SCU
    //
    VbiosVersionString=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL); 
//    gBS->FreePool (TempStringBuffer2);
  } else {
    //
    // Update Default VBIOS Version to SCU
    //
//[-start-121002-IB06460449-modify]//
//    TempStringBuffer=HiiGetString (AdvanceHiiHandle, STRING_TOKEN (STR_VBIOS_VERSION_STRING), NULL);
//    
//    VbiosVersionString=HiiSetString (MainHiiHandle, 0, TempStringBuffer, NULL);   
    NewStringToHandle (
      AdvanceHiiHandle,
      STRING_TOKEN (STR_VBIOS_VERSION_STRING),
      MainHiiHandle,
      &VbiosVersionString
      );
//[-end-121002-IB06460449-modify]//
  }

//[-start-121002-IB06460449-modify]//
//  TempStringBuffer=HiiGetString (AdvanceHiiHandle, STRING_TOKEN (STR_VBIOS_VERSION_TXT), NULL);
//  
//  VbiosVersionText=HiiSetString (MainHiiHandle, 0, TempStringBuffer, NULL);   
  NewStringToHandle (
    AdvanceHiiHandle,
    STRING_TOKEN (STR_VBIOS_VERSION_TXT),
    MainHiiHandle,
    &VbiosVersionText
    );
//[-end-121002-IB06460449-modify]//
  
  HiiCreateTextOpCode (OpCodeHandle,VbiosVersionText, 0, VbiosVersionString );

  gBS->FreePool (TempStringBuffer);
  return EFI_SUCCESS;
}
