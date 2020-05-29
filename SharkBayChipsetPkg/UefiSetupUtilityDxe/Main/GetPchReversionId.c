/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PlatformInfo.h>

//[-start-130411-IB05160434-modify]//
//[-start-120713-IB05300317-modify]//
STEPPING_STRING_DEFINITION          PchSteppingRevString[] = {
                                      {V_PCH_LPT_LPC_RID_0, L"A0"},
                                      {V_PCH_LPT_LPC_RID_1, L"A1"},
                                      {V_PCH_LPT_LPC_RID_2, L"B0"},
                                      {V_PCH_LPT_LPC_RID_3, L"C0"},
                                      {V_PCH_LPT_LPC_RID_4, L"C1"},
                                      {V_PCH_LPT_LPC_RID_5, L"C2"},
                                      {0xFF, L""}
                                      };
//[-end-120713-IB05300317-modify]//
//[-end-130411-IB05160434-modify]//

//[-start-130222-IB10930020-modify]//
//[-start-120628-IB06460410-modify]//
//[-start-120619-IB06150226-add]//
SKU_STRING_DEFINITION               PchSkuString[] = {
                                      { V_PCH_LPTH_LPC_DEVICE_ID_MB_SUPER_SKU , L"LPT-H Super SKU"       },
                                      { V_PCH_LPTH_LPC_DEVICE_ID_MB_0         , L"LPT-H HM86"            },
                                      { V_PCH_LPTH_LPC_DEVICE_ID_MB_1         , L"LPT-H HM87"            },
                                      { V_PCH_LPTH_LPC_DEVICE_ID_MB_2         , L"LPT-H QM87"            },
                                      { V_PCH_LPTLP_LPC_DEVICE_ID_UNFUSE      , L"LPT-LP Unfuse"         },
                                      { V_PCH_LPTLP_LPC_DEVICE_ID_MB_SUPER_SKU, L"LPT-LP Super SKU"      },
                                      { V_PCH_LPTLP_LPC_DEVICE_ID_MB_0        , L"LPT-LP TBD SKU 0"      },
                                      { V_PCH_LPTLP_LPC_DEVICE_ID_MB_1        , L"LPT-LP Premium SKU"    },
                                      { V_PCH_LPTLP_LPC_DEVICE_ID_MB_2        , L"LPT-LP TBD SKU 2"      },
                                      { V_PCH_LPTLP_LPC_DEVICE_ID_MB_3        , L"LPT-LP Mainstream SKU" },
                                      { V_PCH_LPTLP_LPC_DEVICE_ID_MB_4        , L"LPT-LP TBD SKU 4"      },
                                      { V_PCH_LPTLP_LPC_DEVICE_ID_MB_5        , L"LPT-LP Value SKU"      },
                                      { V_PCH_LPTH_LPC_DEVICE_ID_DT_SUPER_SKU , L"LPT-H Desktop Super SKU"},
                                      { V_PCH_LPTH_LPC_DEVICE_ID_DT_0         , L"LPT-H Z87"             },
                                      { V_PCH_LPTH_LPC_DEVICE_ID_DT_1         , L"LPT-H Z85"             },
                                      { V_PCH_LPTH_LPC_DEVICE_ID_DT_2         , L"LPT-H H87"             },
                                      { V_PCH_LPTH_LPC_DEVICE_ID_DT_3         , L"LPT-H Q85"             },
                                      { V_PCH_LPTH_LPC_DEVICE_ID_DT_4         , L"LPT-H Q87"             },
                                      { V_PCH_LPTH_LPC_DEVICE_ID_DT_5         , L"LPT-H B85"             },
                                      { V_PCH_LPTH_LPC_DEVICE_ID_DT_6         , L"LPT-H H81"             },
                                      { V_PCH_LPTH_LPC_DEVICE_ID_SVR_0        , L"LPT-H C222"            },
                                      { V_PCH_LPTH_LPC_DEVICE_ID_SVR_1        , L"LPT-H C224"            },
                                      { V_PCH_LPTH_LPC_DEVICE_ID_SVR_2        , L"LPT-H C226"            },
                                      {0xFF, L""}
                                      };
//[-end-120619-IB06150226-add]//
//[-end-120628-IB06460410-modify]//
//[-end-130222-IB10930020-modify]//

EFI_STATUS
GetPchReversionId (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer
  )
{
  EFI_STATUS          Status;
  UINT8               Data8;
  UINTN               Index;
//[-start-120619-IB06150226-add]//
  UINT16              DevId;
  UINTN               DidIndex;
//[-end-120619-IB06150226-add]//
  BOOLEAN             UpdatePchRevStatus;
  STRING_REF          PchReversionText;
  STRING_REF          PchReversionIdString;

  Status              = EFI_SUCCESS;
  UpdatePchRevStatus  = FALSE;

  Data8 = MmioRead8 ( MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC, R_PCH_LPC_RID));
//[-start-120619-IB06150226-add]//
  DevId = MmioRead16 ( MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC, R_PCH_LPC_DEVICE_ID));
  for (DidIndex = 0; PchSkuString[DidIndex].ReversionValue != 0xFF; DidIndex++) {
    if (DevId == PchSkuString[DidIndex].ReversionValue) {
      break;
    }
  }
//[-end-120619-IB06150226-add]//
  
  for (Index = 0; PchSteppingRevString[Index].ReversionValue != 0xFF; Index++) {
    if (Data8 == PchSteppingRevString[Index].ReversionValue) {
//[-start-120619-IB06150226-modify]//
      UnicodeSPrint (StringBuffer, 0x100, L"%02x (%s Stepping) / %s", (UINTN)Data8, PchSteppingRevString[Index].SteppingString, PchSkuString[DidIndex].SteppingString);
//[-end-120619-IB06150226-modify]//
      UpdatePchRevStatus = TRUE;
      break;
    }
  }

  if (UpdatePchRevStatus == FALSE) {
//[-start-121002-IB06460449-modify]//
//    StringBuffer=HiiGetString (AdvanceHiiHandle, STRING_TOKEN (STR_PCH_REV_ID_STRING), NULL);
    NewStringToHandle (
      AdvanceHiiHandle,
      STRING_TOKEN (STR_PCH_REV_ID_STRING),
      MainHiiHandle,
      &PchReversionIdString
      );
  } else {
    PchReversionIdString=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL); 
  }
  
//  StringBuffer=HiiGetString (AdvanceHiiHandle, STRING_TOKEN (STR_PCH_REV_ID_TEXT), NULL);
//  PchReversionText=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);
  NewStringToHandle (
    AdvanceHiiHandle,
    STRING_TOKEN (STR_PCH_REV_ID_TEXT),
    MainHiiHandle,
    &PchReversionText
  );
//[-end-121002-IB06460449-modify]//
  
  HiiCreateTextOpCode (OpCodeHandle,PchReversionText, 0, PchReversionIdString );

  return Status;
}

