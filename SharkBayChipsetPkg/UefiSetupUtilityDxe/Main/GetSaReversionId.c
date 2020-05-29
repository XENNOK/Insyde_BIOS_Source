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

//[-start-120525-IB10540007-add]//
SA_INFO_DISPLAY_TABLE        mDisplaySaFunction[]       = {SA_INFO_DISPLAY_TABLE_LIST};
//[-end-120525-IB10540007-add]//

//[-start-120313-IB05300297-modify]//
STEPPING_STRING_DEFINITION          SaSteppingRevString[] = {
                                      {HSW_STEP_A0, L"A0/A1"},
                                      {HSW_STEP_B0, L"B0"},
                                      {HSW_STEP_C0, L"C0"},
                                      {0xFF, L""}
                                    };
//[-end-120313-IB05300297-modify]//

//[-start-121026-IB10370026-remove]//
////[-start-120525-IB10540007-add]//
//EFI_STATUS
//GetSaReversionId (
//  IN    VOID                      *OpCodeHandle,
//  IN    EFI_HII_HANDLE            MainHiiHandle,
//  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
//  IN    CHAR16                    *StringBuffer
//  )
//{
//  EFI_STATUS                    Status = EFI_SUCCESS;
//  UINTN                         Index;
//  
//  for (Index = 0; mDisplaySaFunction[Index].DisplaySaFunction != NULL; Index++) {
//    if (mDisplaySaFunction[Index].Option == DISPLAY_ENABLE) {
//      ZeroMem(StringBuffer, 0x100);
//      Status = mDisplaySaFunction[Index].DisplaySaFunction (OpCodeHandle, MainHiiHandle, AdvanceHiiHandle, StringBuffer);
//    }
//  }
//  return Status;
//}
////[-end-120525-IB10540007-add]//
//[-end-121026-IB10370026-remove]//

//[-start-120525-IB10540007-modify]//
//[-start-121026-IB10370026-remove]//
//EFI_STATUS
//GetSaReversionIdFunc (
//  IN    VOID                      *OpCodeHandle,
//  IN    EFI_HII_HANDLE            MainHiiHandle,
//  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
//  IN    CHAR16                    *StringBuffer
//  )
////[-end-120525-IB10540007-modify]//
//{
//  EFI_STATUS          Status;
//  UINT8               Data8;
//  UINTN               Index;
//  BOOLEAN             UpdateSaRevStatus;
//  STRING_REF          SaReversionText;
//  STRING_REF          SaReversionIdString;
////[-start-120313-IB05300297-remove]//
////  EFI_CPUID_REGISTER  Cpuid01;
////[-end-120313-IB05300297-remove]//
//
//  Status             = EFI_SUCCESS;
//  UpdateSaRevStatus  = FALSE;
//
//  Data8 = McD0PciCfg8(PCI_RID);
////[-start-120313-IB05300297-remove]//
////  AsmCpuid (EFI_CPUID_VERSION_INFO, &Cpuid01.RegEax, &Cpuid01.RegEbx, &Cpuid01.RegEcx, &Cpuid01.RegEdx);
////[-end-120313-IB05300297-remove]//
////[-start-120313-IB05300297-modify]//
////  if ((Cpuid01.RegEax & 0xffff0) == CPUID_FULL_FAMILY_MODEL_HASWELL ) {
//  for (Index = 0; SaSteppingRevString[Index].ReversionValue != 0xFF; Index++) {
//    if (Data8 == SaSteppingRevString[Index].ReversionValue) {
//      UnicodeSPrint (StringBuffer, 0x100, L"%02x (%s Stepping)", (UINTN)Data8, SaSteppingRevString[Index].SteppingString);
//      UpdateSaRevStatus = TRUE;
//      break;
//    }
//  }
////  }
////[-end-120313-IB05300297-modify]//
//  
//  if (UpdateSaRevStatus == FALSE) {
////[-start-121002-IB06460449-modify]//
////    StringBuffer=HiiGetString (AdvanceHiiHandle, STRING_TOKEN (STR_SYS_AGNT_REV_ID_STRING), NULL);
//    NewStringToHandle (
//      AdvanceHiiHandle,
//      STRING_TOKEN (STR_SYS_AGNT_REV_ID_STRING),
//      MainHiiHandle,
//      &SaReversionIdString
//      );
//  } else {
//    SaReversionIdString=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL); 
//  }
//  
////  StringBuffer=HiiGetString (AdvanceHiiHandle, STRING_TOKEN (STR_SYS_AGNT_REV_ID_TEXT), NULL);
////  
////  SaReversionText=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);   
//  NewStringToHandle (
//    AdvanceHiiHandle,
//    STRING_TOKEN (STR_SYS_AGNT_REV_ID_TEXT),
//    MainHiiHandle,
//    &SaReversionText
//    );
////[-end-121002-IB06460449-modify]//
//  HiiCreateTextOpCode (OpCodeHandle,SaReversionText, 0, SaReversionIdString );
//
//  return Status;
//}
//[-end-121026-IB10370026-remove]//


//[-start-120524-IB10540007-add]//
EFI_STATUS
GetIgdFreqFunc (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer
  )
{
  EFI_STATUS     Status = EFI_UNSUPPORTED;
  STRING_REF     IgdFreqText;
  STRING_REF     IgdFreqString;
  UINTN          IgdFreq;
  
  UINTN   PciD0F0RegBase;
  UINTN   MchBarBase;
  UINTN   MchBar;

  ///
  /// Get the MCH space base address.
  /// Read MMIO register MCHBAR+0x5998 to get Integrated GFX
  ///
  PciD0F0RegBase  = MmPciAddress (0, SA_BUS_NUMBER, SA_DEVICE_NUMBER, SA_FUNCTION_NUMBER, 0);
  MchBarBase      = MmioRead32 (PciD0F0RegBase + 0x48) & 0xffff0000;
  MchBar          = MmioRead32 (MchBarBase + 0x5998);

  IgdFreq = (MchBar & 0x00ff0000) >> 16;

  UnicodeSPrint ( StringBuffer, 0x100, L"%d MHz", IgdFreq*MAXIMUN_EFFICIENCY_RATIO );

  IgdFreqString=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);
  
//[-start-121002-IB06460449-modify]//
//  StringBuffer=HiiGetString (AdvanceHiiHandle, STRING_TOKEN (STR_IGD_FREQ_TEXT), NULL); 
//  
//  IgdFreqText=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL); 
  NewStringToHandle (
    AdvanceHiiHandle,
    STRING_TOKEN (STR_IGD_FREQ_TEXT),
    MainHiiHandle,
    &IgdFreqText
    );
//[-end-121002-IB06460449-modify]//
  
  HiiCreateTextOpCode (OpCodeHandle, IgdFreqText, 0, IgdFreqString);
  
  return Status;
}
//[-end-120524-IB10540007-add]//