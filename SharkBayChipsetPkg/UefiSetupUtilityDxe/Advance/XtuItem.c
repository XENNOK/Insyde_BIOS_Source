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

#include <SetupUtility.h>


EFI_STATUS
CreateXtuOption (
  IN EFI_HII_HANDLE                     HiiHandle
  )
{
  CHIPSET_CONFIGURATION                  SetupNvData;
  UINT16                                VarOffset = 0;
  VOID                                  *HobList;
  XTU_INFO_DATA                         *XTUInfoHobData;
  VOID                                  *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL                    *StartLabel;
  VOID                                  *DefaultOpCodeHandle;
  VOID                                  *DefaultOpCodeHandle2;
  VOID                                  *DefaultOpCodeHandle3;
  VOID                                  *DefaultOpCodeHandle4;

  //
  // Get XTU info hob
  //
  HobList = GetHobList ();

  XTUInfoHobData = GetNextGuidHob ( &gXTUInfoHobGuid, HobList);
//[-start-130207-IB10870073-add]//
  ASSERT (XTUInfoHobData != NULL);
  if (XTUInfoHobData == NULL) {
    return EFI_NOT_FOUND;
  }
//[-end-130207-IB10870073-add]//

//[-start-140210-IB05400512-add]//
  XTUInfoHobData = (XTU_INFO_DATA*)GET_GUID_HOB_DATA (XTUInfoHobData);
//[-end-140210-IB05400512-add]//

  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL); 
  
  DefaultOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (DefaultOpCodeHandle != NULL);  
  
  StartLabel         = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number     = RATIO_LABEL;

  //
  // Create RATIO_LABEL
  //
  VarOffset = (UINT16)((UINTN)(&SetupNvData.FlexRatio) - (UINTN)(&SetupNvData));
  HiiCreateDefaultOpCode (DefaultOpCodeHandle, EFI_HII_DEFAULT_CLASS_STANDARD, EFI_IFR_NUMERIC_SIZE_1, XTUInfoHobData->CpuNonTurboRatio.MaxRatio);

  HiiCreateNumericOpCode (
    StartOpCodeHandle,
    KEY_XTU_ITEM,
    CONFIGURATION_VARSTORE_ID,
    VarOffset,
    STRING_TOKEN(STR_NON_TURBO_FLEX_OVERRIDE),
    STRING_TOKEN(STR_NON_TURBO_FLEX_OVERRIDE_HELP),
    0,
    0,
    XTUInfoHobData->CpuNonTurboRatio.MinRatio,
    XTUInfoHobData->CpuNonTurboRatio.MaxRatio,
    1,
    DefaultOpCodeHandle
    );
  HiiUpdateForm (HiiHandle, NULL, 0x37, StartOpCodeHandle, NULL);
  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (DefaultOpCodeHandle);

  //
  // Create CORE_RATIO_LIMIT_LABEL
  //
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);
  
  DefaultOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (DefaultOpCodeHandle != NULL); 

  DefaultOpCodeHandle2 = HiiAllocateOpCodeHandle ();
  ASSERT (DefaultOpCodeHandle != NULL); 
 
  DefaultOpCodeHandle3 = HiiAllocateOpCodeHandle ();
  ASSERT (DefaultOpCodeHandle != NULL); 

  DefaultOpCodeHandle4 = HiiAllocateOpCodeHandle ();
  ASSERT (DefaultOpCodeHandle != NULL); 
  
  StartLabel         = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number     = CORE_RATIO_LIMIT_LABEL;


  VarOffset = (UINT16)((UINTN)(&SetupNvData.RatioLimit1Core) - (UINTN)(&SetupNvData));
  HiiCreateDefaultOpCode (DefaultOpCodeHandle, EFI_HII_DEFAULT_CLASS_STANDARD, EFI_IFR_NUMERIC_SIZE_1, XTUInfoHobData->TurboRatioLimitDefault.LimitCore1);

  HiiCreateNumericOpCode (
    StartOpCodeHandle,
    KEY_XTU_ITEM,
    CONFIGURATION_VARSTORE_ID,
    VarOffset,
    STRING_TOKEN(STR_1_CORE_RATIO_LIMIT_STRING),
    STRING_TOKEN(STR_1_CORE_RATIO_LIMIT_HELP),
    0,
    0,
    XTUInfoHobData->CpuNonTurboRatio.MaxRatio,
//[-start-120215-IB03780423-modify]//
    XTUInfoHobData->TurboRatioLimitMaximum.LimitCore1,
//[-end-120215-IB03780423-modify]//
    1,
    DefaultOpCodeHandle    
    );

  VarOffset = (UINT16)((UINTN)(&SetupNvData.RatioLimit2Core) - (UINTN)(&SetupNvData));
  HiiCreateDefaultOpCode (DefaultOpCodeHandle2, EFI_HII_DEFAULT_CLASS_STANDARD, EFI_IFR_NUMERIC_SIZE_1,  XTUInfoHobData->TurboRatioLimitDefault.LimitCore2);
  HiiCreateNumericOpCode (
    StartOpCodeHandle,
    KEY_XTU_ITEM,
    CONFIGURATION_VARSTORE_ID,
    VarOffset,
    STRING_TOKEN(STR_2_CORE_RATIO_LIMIT_STRING),
    STRING_TOKEN(STR_2_CORE_RATIO_LIMIT_HELP),
    0,
    0,
    XTUInfoHobData->CpuNonTurboRatio.MaxRatio,
//[-start-120215-IB03780423-modify]//
    XTUInfoHobData->TurboRatioLimitMaximum.LimitCore2,
//[-end-120215-IB03780423-modify]//
    1,
    DefaultOpCodeHandle2
    );

  VarOffset = (UINT16)((UINTN)(&SetupNvData.RatioLimit3Core) - (UINTN)(&SetupNvData));
  HiiCreateDefaultOpCode (DefaultOpCodeHandle3, EFI_HII_DEFAULT_CLASS_STANDARD, EFI_IFR_NUMERIC_SIZE_1,  XTUInfoHobData->TurboRatioLimitDefault.LimitCore3);
  HiiCreateNumericOpCode (
    StartOpCodeHandle,
    KEY_XTU_ITEM,
    CONFIGURATION_VARSTORE_ID,
    VarOffset,
    STRING_TOKEN(STR_3_CORE_RATIO_LIMIT_STRING),
    STRING_TOKEN(STR_3_CORE_RATIO_LIMIT_HELP),
    0,
    0,
    XTUInfoHobData->CpuNonTurboRatio.MaxRatio,
//[-start-120215-IB03780423-modify]//
    XTUInfoHobData->TurboRatioLimitMaximum.LimitCore3,
//[-end-120215-IB03780423-modify]//
    1,
    DefaultOpCodeHandle3
    );

  VarOffset = (UINT16)((UINTN)(&SetupNvData.RatioLimit4Core) - (UINTN)(&SetupNvData));
  HiiCreateDefaultOpCode (DefaultOpCodeHandle4, EFI_HII_DEFAULT_CLASS_STANDARD, EFI_IFR_NUMERIC_SIZE_1,  XTUInfoHobData->TurboRatioLimitDefault.LimitCore4);
  HiiCreateNumericOpCode (
    StartOpCodeHandle,
    KEY_XTU_ITEM,
    CONFIGURATION_VARSTORE_ID,
    VarOffset,
    STRING_TOKEN(STR_4_CORE_RATIO_LIMIT_STRING),
    STRING_TOKEN(STR_4_CORE_RATIO_LIMIT_HELP),
    0,
    0,
    XTUInfoHobData->CpuNonTurboRatio.MaxRatio,
//[-start-120215-IB03780423-modify]//
    XTUInfoHobData->TurboRatioLimitMaximum.LimitCore4,
//[-end-120215-IB03780423-modify]//
    1,
    DefaultOpCodeHandle4
    );
  
  HiiUpdateForm (HiiHandle, NULL, 0x37, StartOpCodeHandle, NULL);
  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (DefaultOpCodeHandle);
  HiiFreeOpCodeHandle (DefaultOpCodeHandle2);
  HiiFreeOpCodeHandle (DefaultOpCodeHandle3);
  HiiFreeOpCodeHandle (DefaultOpCodeHandle4);

  return EFI_SUCCESS;
}
