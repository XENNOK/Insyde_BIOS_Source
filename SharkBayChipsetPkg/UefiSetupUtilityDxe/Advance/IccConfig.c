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

#include <SetupUtility.h>
#include <IccOverClocking.h>
#include <Protocol/ICCPlatformPolicy.h>

typedef struct {
  STRING_REF                      FreqToken;
  STRING_REF                      SccModeToken;
  STRING_REF                      SccValueToken;
} ICC_SETTING_TOKENS;

//[-start-120315-IB06460374-add]//
typedef enum {
  IccProfile0 = 0,
  IccProfile1,
  IccProfile2,
  IccProfile3,
  IccProfile4,
  IccProfile5,
  IccProfile6,
  IccProfile7
} ICC_PROFILE;
//[-end-120315-IB06460374-add]//

typedef enum {
  ICC_SCC_MODE_UP       = 0,
  ICC_SCC_MODE_CENTER,
  ICC_SCC_MODE_DOWN
} ICC_SCC_MODE;

typedef enum {
  ICC_APPLY_IMMEDIATE     = 0,
  ICC_APPLY_PERMANENT,  
  MAX_ICC_ITEM
} ICC_SCC_APPLY;

static UINT16                   IccClockGenForms[] = {
                                    ICC_CLOCK_GEN1_LABEL,
                                    ICC_CLOCK_GEN2_LABEL,
                                    ICC_CLOCK_GEN3_LABEL,
                                    ICC_CLOCK_GEN4_LABEL,
                                    ICC_CLOCK_GEN5_LABEL,
                                    ICC_CLOCK_GEN6_LABEL
                                };

static UINT16                   IccFormKeyList[] = {
                                    KEY_ICC_CLOCK_1,
                                    KEY_ICC_CLOCK_2,
                                    KEY_ICC_CLOCK_3,
                                    KEY_ICC_CLOCK_4,
                                    KEY_ICC_CLOCK_5,
                                    KEY_ICC_CLOCK_6
                                };

static CHAR16                   *ClockUsageNames[] = {
                                    L"BCLK, ",
                                    L"DMI, ",
                                    L"PEG, ",
                                    L"PCIe, ",
                                    L"PCI33, ",
                                    L"Reserved, ",
                                    L"SATA, ",
                                    L"USB3, ",
                                    L"GFX, ",
                                    L"GFX Bending, ",
                                    L"27MHz, ",
                                    NULL
                                };

static CHAR16                   *IccLibStatusString[] = {
                                    L"SUCCESS",                                 // 0x00
                                    L"INVALID_PARAMS",                          // 0x01
                                    L"INVALID_CLOCK_NUMBER",                    // 0x02
                                    L"CLOCK_NOT_FOUND",                         // 0x03
                                    L"INVALID_FREQ_VALUE",                      // 0x04
                                    L"FREQ_TOO_LOW",                            // 0x05
                                    L"FREQ_TOO_HIGH",                           // 0x06
                                    L"FREQ_MUST_HAVE_ZERO_SSC",                 // 0x07
                                    L"SSC_CHANGE_NOT_ALLOWED",                  // 0x08
                                    L"INVALID_SSC_MODE",                        // 0x09
                                    L"SSC_MODE_NOT_SUPPORTED",                  // 0x0a
                                    L"SSC_OUT_OF_RANGE",                        // 0x0b
                                    L"SSC_TOO_HIGH",                            // 0x0c
                                    L"SSC_TOO_LOW",                             // 0x0d
                                    L"SSC_CHANGE_NOT_ALLOWED_SSC_DISABLED",     // 0x0e
                                    L"DYNAMIC_CHANGE_NOT_ALLOWED",              // 0x0f
                                    L"INVALID_DMI_PEG_RATIO",                   // 0x10
                                    L"REGISTER_IS_LOCKED",                      // 0x11
                                    L"MEI_INITIALIZATION_FAILED",               // 0x12
                                    L"MEI_CONNECTION_FAILED",                   // 0x13
                                    L"UNEXPECTED_FW_ERROR",                     // 0x14
                                    L"UNSUPPORTED_HW",                          // 0x15
                                    L"CLOCK_DISABLED_FAILED_PROGRAMMING",       // 0x16
                                    L"FREQ_AND_SSC_NOT_MATCH",                  // 0x17
                                    L"WAITING_FOR_WARM_RESET",                  // 0x18
                                    L"NOT_ALLOWED_IN_USER_MODE",                // 0x19
                                    L"TOO_MANY_CONNECTIONS",                    // 0x1a
                                    L"INVALID_COOKIE",                          // 0x1b
                                    L"DMI_PEG_RATIO_CHANGE_NOT_ALLOWED",        // 0x1c
                                    L"NO_USAGE_DEFINED_FOR_THE_CLOCK",          // 0x1d
                                    L"DATA_NOT_AVAILABLE",                      // 0x1e                                    
                                    NULL
                                };

ICC_CLOCK_RANGES                mClockRangeDefinition;
ICC_OVERCLOCKING_PROTOCOL       *mIccProtocol;
ICC_SETTING_TOKENS              *mIccSettingTokens;

ICC_LIB_STATUS
UpdateIccLibInfo (
  IN  EFI_HII_HANDLE              HiiHandle,
  IN  UINT8                       *NumberOfClockCount
  );

ICC_LIB_STATUS
InitClockGenForms (
  IN  EFI_HII_HANDLE              HiiHandle,
  IN  UINT8                       NumberOfClockCount  
  );

UINT32
IccDivisorToFrequency(
  IN UINT32 Divisor
  );


/**
 Init Extended ICC Setup Form

 @param [in]   HiiHandle

 @retval EFI_SUCCESS

**/
EFI_STATUS
IccOverClockingInit (
  IN  EFI_HII_HANDLE              HiiHandle
)
{
  EFI_STATUS                      Status;
  ICC_LIB_STATUS                  IccStatus;
  UINT8                           NumberOfClockCount;  
  DXE_ME_POLICY_PROTOCOL          *MEPlatformPolicy;
  CHAR16                          *StringBuffer;

  ((CHIPSET_CONFIGURATION *)gSUBrowser->SCBuffer)->IccOperable = FALSE;

  Status = gBS->LocateProtocol ( &gDxePlatformMePolicyGuid, NULL, (VOID **)&MEPlatformPolicy );
  ASSERT_EFI_ERROR ( Status );

  if (MEPlatformPolicy->MeConfig.EndOfPostDone) {
    //
    // ICC overclocking items will be skipped if EndOfPostDone was set,
    // because them do not support after sent end of post.
    //
    return EFI_ABORTED;
  }  
  
  StringBuffer = AllocateZeroPool (0x100);
  UnicodeSPrint (
    StringBuffer,
    0x100,
    L"  "
    );

  HiiSetString (HiiHandle, STRING_TOKEN(STR_ICC_LIB_DISABLE_STRING), StringBuffer, NULL);

  Status = gBS->LocateProtocol ( &gIccOverClockingProtocolGuid, NULL, (VOID **)&mIccProtocol );

  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  IccStatus = UpdateIccLibInfo (HiiHandle, &NumberOfClockCount);

  mIccSettingTokens = AllocateZeroPool (sizeof(ICC_SETTING_TOKENS) * NumberOfClockCount);
  if ( IccStatus != ICC_LIB_STATUS_SUCCESS ) {
    return EFI_ABORTED;
  }
  
  IccStatus = InitClockGenForms (HiiHandle, NumberOfClockCount);

  if ( IccStatus != ICC_LIB_STATUS_SUCCESS ) {
    return EFI_ABORTED;
  }

  ((CHIPSET_CONFIGURATION *)gSUBrowser->SCBuffer)->IccOperable = TRUE;
  
  return EFI_SUCCESS;
}

/**
 Get ICC Lib version from IccLib, and update to SCU

 @param [in]   HiiHandle
 @param [in]   NumberOfClockCount

**/
ICC_LIB_STATUS
UpdateIccLibInfo (
  IN  EFI_HII_HANDLE              HiiHandle,
  IN  UINT8                       *NumberOfClockCount
)
{
  ICC_LIB_STATUS                  IccStatus;
  ICC_LIB_VERSION                 IccLibVersion;
  CHAR16                          *StringBuffer;
  
  StringBuffer = NULL;

  mIccProtocol->GetInfo (&IccLibVersion, NumberOfClockCount, &IccStatus);
  
  if (IccStatus == ICC_LIB_STATUS_SUCCESS) {
    StringBuffer = AllocateZeroPool (0x100);

    UnicodeSPrint (
      StringBuffer,
      0x100,
      L"%d.%d.%d.%d",
      IccLibVersion.Major,
      IccLibVersion.Minor,
      IccLibVersion.Hotfix,
      IccLibVersion.Build
      );

    HiiSetString (HiiHandle, STRING_TOKEN(STR_ICC_LIB_VERSION), StringBuffer, NULL);
    
    gBS->FreePool (StringBuffer);
  }
  return IccStatus;
}

/**
 Init ICC Clock Gen form 1-6

 @param [in]   HiiHandle
 @param [in]   NumberOfClockCount

**/
ICC_LIB_STATUS
InitClockGenForms (
  IN  EFI_HII_HANDLE              HiiHandle,
  IN  UINT8                       NumberOfClockCount  
)
{
  ICC_LIB_STATUS                  IccStatus;
  UINTN                           Index;
  UINT8                           ClockIndex;
  ICC_CLOCK_RANGES                CurrentClockRangeDef;
  ICC_CLOCK_SETTINGS              CurrentClockSettings;
  UINT32                          FreqKHz;
  UINT32                          ClockUsage;
  BOOLEAN                         AllowChangeFreq;
  BOOLEAN                         AllowChangeSccMode;
  BOOLEAN                         AllowChangeSccValue;
  STRING_REF                      TempToken = 0;
  CHAR16                          *StringBuffer;
  CHAR16                          *StringBuffer2;
  CHAR16                          *gStringBuffer;
  CHIPSET_CONFIGURATION            SetupNvData;
  UINT16                          QuestionId;
  VOID                            *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL              *StartLabel;
  VOID                            *DefaultOpCodeHandle;
  VOID                            *DefaultOpCodeHandle2;
  VOID                            *OptionsOpCodeHandle;
  VOID                            *OptionsOpCodeHandle2; 


  StringBuffer = AllocateZeroPool (0x100);
  for (ClockIndex = 0; ClockIndex < NumberOfClockCount; ClockIndex++) {
    StartOpCodeHandle = HiiAllocateOpCodeHandle ();
    ASSERT (StartOpCodeHandle != NULL); 

    DefaultOpCodeHandle = HiiAllocateOpCodeHandle ();
    ASSERT (DefaultOpCodeHandle != NULL);
  
    DefaultOpCodeHandle2 = HiiAllocateOpCodeHandle ();
    ASSERT (DefaultOpCodeHandle2 != NULL);

    OptionsOpCodeHandle= HiiAllocateOpCodeHandle ();
    ASSERT (OptionsOpCodeHandle != NULL);  

    OptionsOpCodeHandle2= HiiAllocateOpCodeHandle ();
    ASSERT (OptionsOpCodeHandle != NULL); 
  
    StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
    StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
    StartLabel->Number       = IccClockGenForms[ClockIndex];
    
    mIccProtocol->GetClockRanges (ClockIndex, &CurrentClockRangeDef, &IccStatus);
    if (IccStatus != ICC_LIB_STATUS_SUCCESS) {
      continue;
    }
    
    mIccProtocol->GetCurrentClockSettings (ClockIndex, &CurrentClockSettings, &IccStatus);
    if (IccStatus != ICC_LIB_STATUS_SUCCESS) {
      continue;
    }

    //
    // Check if this clock settings are changeable or not.
    //
    AllowChangeFreq = (CurrentClockRangeDef.FrequencyMax != CurrentClockRangeDef.FrequencyMin);
    AllowChangeSccMode = FALSE;
    AllowChangeSccValue = FALSE;
    if (CurrentClockRangeDef.SscChangeAllowed) {
     //
     // SCC Mode options greater than 1, then we can change to another one.
     //
      if ((CurrentClockRangeDef.SscUpAllowed
            + CurrentClockRangeDef.SscCenterAllowed
            + CurrentClockRangeDef.SscDownAllowed) > 1) {
        AllowChangeSccMode = TRUE;
      }
      if (CurrentClockRangeDef.SscPercentMax > 0) {
        AllowChangeSccValue = TRUE;
      }
    }
    //
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // <Workaround!, clock4 cannot operate, demoBIOS the same ==> skip this clock>
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //
    if ( (CurrentClockSettings.Frequency > CurrentClockRangeDef.FrequencyMax)
      || (CurrentClockSettings.Frequency < CurrentClockRangeDef.FrequencyMin) ) {
      AllowChangeFreq = AllowChangeSccMode = AllowChangeSccValue = FALSE;
    }

      
    //
    //==========================================
    // Check clock usages and add text to forms
    // "Clock Usage : xxx, yyy, zzz"
    //==========================================
    //
//[-start-130717-IB05400432-modify]//
    StringBuffer2 = AllocateZeroPool (0x200);
//[-end-130717-IB05400432-modify]//
//[-start-130207-IB10870073-add]//
    ASSERT (StringBuffer2 != NULL);
    if (StringBuffer2 == NULL) {
      return ICC_LIB_STATUS_UNSPECIFIED_ERROR;
    }
//[-end-130207-IB10870073-add]//
    gStringBuffer = HiiGetString( HiiHandle, STRING_TOKEN ( STR_ICC_CLOCK_USAGE_STRING), NULL );
    ASSERT (gStringBuffer != NULL);
    //
    // process ClockUsage String
    //
    ClockUsage = CurrentClockRangeDef.UsageMask;
    if (ClockUsage != 0) {
      for (Index = 0; ClockUsageNames[Index] != NULL; Index++) {
        if (ClockUsage & BIT0) {
          StrCat (StringBuffer2, gStringBuffer);
          StrCat (StringBuffer2, ClockUsageNames[Index]);
        }
        ClockUsage >>= 1;
      }
      //
      // if this clock is used, cut the latest two char ", "
      //
      StringBuffer2 [StrLen (StringBuffer2) - 2] = StringBuffer2 [StrLen (StringBuffer2)];
    } else {
      //
      // if this clock is not used, show "None."
      //
      StrCat (StringBuffer2, gStringBuffer);
      StrCat (StringBuffer2, L"No Used.");
    }

    TempToken=HiiSetString (HiiHandle, 0, StringBuffer2, NULL); 
    HiiCreateTextOpCode (
      StartOpCodeHandle,
      TempToken,
      STRING_TOKEN(STR_BLANK_STRING),
      STRING_TOKEN(STR_BLANK_STRING)
      );
    gBS->FreePool (gStringBuffer);
    gBS->FreePool (StringBuffer2);
    //============
    // Blank Line
    //============
    HiiCreateSubTitleOpCode(StartOpCodeHandle, STRING_TOKEN (STR_BLANK_STRING), 0, 0, 0); 

    //
    //========================================
    // "Max Supproted Frequency  xxx.xx MHz"
    // "Min Supported Frequency  ooo.oo MHz"
    //========================================
    //
    FreqKHz = IccDivisorToFrequency (CurrentClockRangeDef.FrequencyMax);
    UnicodeSPrint (StringBuffer, 0x100, L"%d.%d MHz", FreqKHz/100, FreqKHz%100);
    TempToken=HiiSetString (HiiHandle, 0, StringBuffer, NULL);
    HiiCreateTextOpCode (StartOpCodeHandle,STRING_TOKEN (STR_ICC_MAX_FR_STRING), 0, TempToken);

    FreqKHz = IccDivisorToFrequency (CurrentClockRangeDef.FrequencyMin);
    UnicodeSPrint (StringBuffer, 0x100, L"%d.%d MHz", FreqKHz/100, FreqKHz%100);
    TempToken=HiiSetString (HiiHandle, 0, StringBuffer, NULL);
    HiiCreateTextOpCode (StartOpCodeHandle,STRING_TOKEN (STR_ICC_MIN_FR_STRING), 0, TempToken);

    //
    //========================================
    // "Supported SSC Mode       Up/Center/Down"
    //========================================
    //
    gBS->SetMem (StringBuffer, 0x100, 0);
    if (CurrentClockRangeDef.SscUpAllowed) {
      gStringBuffer=HiiGetString( HiiHandle, STRING_TOKEN (STR_ICC_SSC_UP_TEXT), NULL );
      StrCat (StringBuffer, gStringBuffer);
    } 
    if (CurrentClockRangeDef.SscCenterAllowed) {
      if (StrLen(StringBuffer) > 0) {
        StrCat (StringBuffer, L"/");
      }
      gStringBuffer=HiiGetString( HiiHandle, STRING_TOKEN (STR_ICC_SSC_CENTER_TEXT), NULL );
      StrCat (StringBuffer, gStringBuffer);
    }
    if (CurrentClockRangeDef.SscDownAllowed) {
      if (StrLen(StringBuffer) > 0) {
        StrCat (StringBuffer, L"/");
      }
      gStringBuffer=HiiGetString( HiiHandle, STRING_TOKEN (STR_ICC_SSC_DOWN_TEXT), NULL );
      StrCat (StringBuffer, gStringBuffer);
    }
    if (StrLen(StringBuffer) == 0){
      TempToken = STRING_TOKEN (STR_ICC_UNSUPPORTED_TEXT);
    } else {
      TempToken=HiiSetString (HiiHandle, 0, StringBuffer, NULL);
    }
    HiiCreateTextOpCode (StartOpCodeHandle,STRING_TOKEN (STR_ICC_SUPPORT_SCC_STRING), 0, TempToken);
    gBS->FreePool (gStringBuffer);
    //
    //========================================
    // "Maximum Supported SSC %  0.xx%" (less than 2.50%)
    //========================================
    //
    if (CurrentClockRangeDef.SscPercentMax == 0) {
      TempToken = STRING_TOKEN (STR_ICC_UNSUPPORTED_TEXT);
    } else {
      UnicodeSPrint (StringBuffer, 0x100, L"%d.%d %%", CurrentClockRangeDef.SscPercentMax / 100,
                                                CurrentClockRangeDef.SscPercentMax % 100);
     TempToken=HiiSetString (HiiHandle, 0, StringBuffer, NULL);
    }
    HiiCreateTextOpCode (StartOpCodeHandle,STRING_TOKEN (STR_ICC_MAX_SCC_STRING), 0, TempToken);

    //============
    // Blank Line
    //============
    HiiCreateSubTitleOpCode(StartOpCodeHandle, STRING_TOKEN (STR_BLANK_STRING), 0, 0, 0); 

    //
    //========================================
    // "Current Frequency  xxx.xx MHz"
    //========================================
    //
    FreqKHz = IccDivisorToFrequency(CurrentClockSettings.Frequency);
    UnicodeSPrint (StringBuffer, 0x100, L"%d.%d MHz", FreqKHz/100, FreqKHz%100);
    TempToken=HiiSetString (HiiHandle, 0, StringBuffer, NULL);
    mIccSettingTokens[ClockIndex].FreqToken = TempToken;
    HiiCreateTextOpCode (StartOpCodeHandle,STRING_TOKEN (STR_ICC_CURRENT_FR_STRING), 0, TempToken);

    //
    //========================================
    // "Current SSC Mode  Up/Center/Down"
    //========================================
    //
    if (CurrentClockSettings.SscMode & BIT0) {
      TempToken = STRING_TOKEN (STR_ICC_SSC_UP_TEXT);
    } else if (CurrentClockSettings.SscMode & BIT1) {
      TempToken = STRING_TOKEN (STR_ICC_SSC_CENTER_TEXT);
    } else if (CurrentClockSettings.SscMode & BIT2) {
      TempToken = STRING_TOKEN (STR_ICC_SSC_DOWN_TEXT);
    } else {
      TempToken = STRING_TOKEN (STR_ICC_NONE_TEXT);
    }
    gStringBuffer=HiiGetString( HiiHandle, TempToken, NULL );

    TempToken=HiiSetString (HiiHandle, 0, gStringBuffer, NULL);
    mIccSettingTokens[ClockIndex].SccModeToken = TempToken;
    HiiCreateTextOpCode (StartOpCodeHandle,STRING_TOKEN (STR_ICC_CURRENT_SSC_MODE_STRING), 0, TempToken);
    gBS->FreePool (gStringBuffer);
    //
    //========================================
    // "Current SSC %  x.xx%" (less than 2.50%)
    //========================================
    //
    if (CurrentClockSettings.SscPercent== 0) {
      gStringBuffer=HiiGetString( HiiHandle, STRING_TOKEN (STR_ICC_NONE_TEXT), NULL );
      TempToken=HiiSetString (HiiHandle, 0, gStringBuffer, NULL);
      gBS->FreePool (gStringBuffer);
    } else {
      UnicodeSPrint (StringBuffer, 0x100, L"%d.%d %%", CurrentClockSettings.SscPercent / 100,
                                                CurrentClockSettings.SscPercent % 100);
      TempToken=HiiSetString (HiiHandle, 0, StringBuffer, NULL);
    }
    mIccSettingTokens[ClockIndex].SccValueToken = TempToken;
    HiiCreateTextOpCode (StartOpCodeHandle,STRING_TOKEN (STR_ICC_CURRENT_SCC_STRING), 0, TempToken);


    //
    // If this clock not used, skip setting items.
    //
    if (CurrentClockRangeDef.UsageMask == 0) {
      HiiUpdateForm (
        HiiHandle,
        NULL, 
        (EFI_FORM_ID)(ICC_CLOCK_GEN1_FORM_ID + ClockIndex),
        StartOpCodeHandle,
        NULL
        );
      HiiFreeOpCodeHandle (StartOpCodeHandle);
      HiiFreeOpCodeHandle (DefaultOpCodeHandle);
      HiiFreeOpCodeHandle (DefaultOpCodeHandle2);
      HiiFreeOpCodeHandle (OptionsOpCodeHandle);
      HiiFreeOpCodeHandle (OptionsOpCodeHandle2);
      continue;
    }

    //============
    // Blank Line
    //============
     HiiCreateSubTitleOpCode(StartOpCodeHandle, STRING_TOKEN (STR_BLANK_STRING), 0, 0, 0); 

    //
    //========================================
    // "New Frequency [10KHz]        [List]"
    //========================================
    //
    if (AllowChangeFreq) {
      QuestionId = (UINT16)((UINTN)(&SetupNvData.IccNewDivisor) - (UINTN)(&SetupNvData));

      HiiCreateDefaultOpCode (DefaultOpCodeHandle, EFI_HII_DEFAULT_CLASS_STANDARD, EFI_IFR_NUMERIC_SIZE_2, (UINT64)IccDivisorToFrequency(CurrentClockSettings.Frequency));
    
      HiiCreateNumericOpCode (
        StartOpCodeHandle,
        KEY_ICC_CLOCK_2,
        CONFIGURATION_VARSTORE_ID,
        QuestionId,
        STRING_TOKEN(STR_ICC_NEW_FR_STRING),
        0,
        0,
        EFI_IFR_NUMERIC_SIZE_2,
        (UINT64)IccDivisorToFrequency(CurrentClockRangeDef.FrequencyMax),
        (UINT64)IccDivisorToFrequency(CurrentClockRangeDef.FrequencyMin),
        (UINT64)7,
        DefaultOpCodeHandle
        );
    } else {
      HiiCreateTextOpCode (StartOpCodeHandle,STRING_TOKEN (STR_ICC_NEW_FR_STRING), 0, STRING_TOKEN (STR_ICC_UNCHANGEABLE_TEXT) );
    }

    //
    //========================================
    // "New SSC Mode        [List]"
    //========================================
    //
    if (AllowChangeSccMode) {
      Index = 0;
      if (CurrentClockRangeDef.SscUpAllowed) {
        HiiCreateOneOfOptionOpCode (
          OptionsOpCodeHandle,
          STRING_TOKEN (STR_ICC_SSC_UP_TEXT),
          Index == 0 ? EFI_IFR_OPTION_DEFAULT :0,
          EFI_IFR_TYPE_NUM_SIZE_8,
          (UINT16)ICC_SCC_MODE_UP
          );
        Index++;
      }
      if (CurrentClockRangeDef.SscCenterAllowed) {
        HiiCreateOneOfOptionOpCode (
          OptionsOpCodeHandle,
          STRING_TOKEN (STR_ICC_SSC_CENTER_TEXT),
          Index == 0 ? EFI_IFR_OPTION_DEFAULT :0,
          EFI_IFR_TYPE_NUM_SIZE_8,
          (UINT16)ICC_SCC_MODE_CENTER
          );
        Index++;
      }
      if (CurrentClockRangeDef.SscDownAllowed) {
        HiiCreateOneOfOptionOpCode (
          OptionsOpCodeHandle,
          STRING_TOKEN (STR_ICC_SSC_DOWN_TEXT),
          Index == 0 ? EFI_IFR_OPTION_DEFAULT :0,
          EFI_IFR_TYPE_NUM_SIZE_8,
          (UINT16)ICC_SCC_MODE_DOWN
          );
        Index++;
      }
      QuestionId = (UINT16)((UINTN)(&SetupNvData.IccNewSccMode) - (UINTN)(&SetupNvData));
      HiiCreateOneOfOpCode (
        StartOpCodeHandle,
        0,
        CONFIGURATION_VARSTORE_ID,
        QuestionId,
        STRING_TOKEN (STR_ICC_NEW_SCC_MODE_STRING),
        0,
        0,
        EFI_IFR_TYPE_NUM_SIZE_8,
        OptionsOpCodeHandle,
        NULL
        );

    } else {
      HiiCreateTextOpCode (StartOpCodeHandle,STRING_TOKEN (STR_ICC_NEW_SCC_MODE_STRING), 0, STRING_TOKEN (STR_ICC_UNCHANGEABLE_TEXT) );
    }

    //
    //========================================
    // "New SSC [0.01%]            [List]"
    //========================================
    //
    if (AllowChangeSccValue) {
      QuestionId = (UINT16)((UINTN)(&SetupNvData.IccNewSccValue) - (UINTN)(&SetupNvData));
      HiiCreateDefaultOpCode (DefaultOpCodeHandle2, EFI_HII_DEFAULT_CLASS_STANDARD, EFI_IFR_NUMERIC_SIZE_1, 0);
      HiiCreateNumericOpCode (
        StartOpCodeHandle,
        KEY_ICC_CLOCK_2,
        CONFIGURATION_VARSTORE_ID,
        QuestionId,
        STRING_TOKEN(STR_ICC_NEW_SCC_STRING),
        0,
        0,
        0,
        (UINT16)0,
        (UINT16)CurrentClockRangeDef.SscPercentMax,
        (UINT16)1,
        DefaultOpCodeHandle2
        );
    } else {
      HiiCreateTextOpCode (StartOpCodeHandle,STRING_TOKEN (STR_ICC_NEW_SCC_STRING), 0, STRING_TOKEN (STR_ICC_UNCHANGEABLE_TEXT) );
    }

    //============
    // Blank Line
    //============
     HiiCreateSubTitleOpCode(StartOpCodeHandle, STRING_TOKEN (STR_BLANK_STRING), 0, 0, 0); 

    //
    //========================================
    // "Apply above settings :        Immediately/Permanently"
    //========================================
    //
    if (AllowChangeFreq || AllowChangeSccMode || AllowChangeSccValue) {
      HiiCreateOneOfOptionOpCode (
        OptionsOpCodeHandle2,
        STRING_TOKEN (STR_ICC_APPLY_IMMEDIATE_TEXT),
        EFI_IFR_OPTION_DEFAULT,
        EFI_IFR_TYPE_NUM_SIZE_8,
        (UINT16)ICC_APPLY_IMMEDIATE
        );
      HiiCreateOneOfOptionOpCode (
        OptionsOpCodeHandle2,
        STRING_TOKEN (STR_ICC_APPLY_PERMANENT_TEXT),
        EFI_IFR_OPTION_DEFAULT,
        EFI_IFR_TYPE_NUM_SIZE_8,
        (UINT16)ICC_APPLY_PERMANENT
        );
      QuestionId = (UINT16)((UINTN)(&SetupNvData.IccApply) - (UINTN)(&SetupNvData));
      HiiCreateOneOfOpCode (
        StartOpCodeHandle,
        KEY_ICC_APPLY,
        CONFIGURATION_VARSTORE_ID,
        QuestionId,
        STRING_TOKEN (STR_ICC_APPLY_STRING),
        0,
        EFI_IFR_FLAG_CALLBACK,
        EFI_IFR_TYPE_NUM_SIZE_8,
        OptionsOpCodeHandle2,
        NULL
        );

    }


    //
    // Finally update form.
    //
    HiiUpdateForm (
      HiiHandle,
      NULL, 
      (EFI_FORM_ID)(ICC_CLOCK_GEN1_FORM_ID + ClockIndex),
      StartOpCodeHandle,  
      NULL
      );
    HiiFreeOpCodeHandle (StartOpCodeHandle);
    HiiFreeOpCodeHandle (DefaultOpCodeHandle);
    HiiFreeOpCodeHandle (DefaultOpCodeHandle2);
    HiiFreeOpCodeHandle (OptionsOpCodeHandle);
    HiiFreeOpCodeHandle (OptionsOpCodeHandle2);
  }
  gBS->FreePool (StringBuffer);

  return ICC_LIB_STATUS_SUCCESS;
}

/**
 When change form, reset the default select options to current settings

 @param [in]   HiiHandle
 @param [in]   MyIfrNVData
 @param [in]   QuestionId
 @param [in]   RefreshCurrentValueText

**/
EFI_STATUS
ResetIccClockForm (
  IN  EFI_HII_HANDLE              HiiHandle,
  IN  CHIPSET_CONFIGURATION        *MyIfrNVData, 
  IN  EFI_QUESTION_ID             QuestionId,
  IN  BOOLEAN                     RefreshCurrentValueText
)
{
  ICC_LIB_STATUS                  IccStatus;
  UINT8                           ClockIndex;
  ICC_CLOCK_SETTINGS              CurrentSettings;
  CHAR16                          *StringBuffer;
  CHAR16                          *gStringBuffer;
  UINT32                          FreqKHz;
  STRING_REF                      TempToken = 0;
  ICC_LIB_VERSION                 IccLibVersion;  
  UINT8                           NumberOfClockCount;

  mIccProtocol->GetInfo (&IccLibVersion, &NumberOfClockCount, &IccStatus);
  for (ClockIndex = 0; ClockIndex < NumberOfClockCount; ClockIndex++) {
    if (QuestionId == IccFormKeyList[ClockIndex]) {
      break;
    }
  }
  if (ClockIndex >= NumberOfClockCount) {
    return EFI_ABORTED;
  }

  //
  // Get Current Clock Settings (six clock records)
  //  
  mIccProtocol->GetCurrentClockSettings (ClockIndex, &CurrentSettings, &IccStatus);
  if (IccStatus != ICC_LIB_STATUS_SUCCESS) {
    return EFI_ABORTED;
  }

  //
  // record current clock index, use for apply settings
  //
  MyIfrNVData->IccClockPage = (UINT8) ClockIndex;

  //
  // set selected option to current value
  //
  MyIfrNVData->IccNewDivisor = (UINT16) IccDivisorToFrequency(CurrentSettings.Frequency);
  if (CurrentSettings.SscMode & BIT0) {
    MyIfrNVData->IccNewSccMode = (UINT8) ICC_SCC_MODE_UP;
  } else if (CurrentSettings.SscMode & BIT1) {
    MyIfrNVData->IccNewSccMode = (UINT8) ICC_SCC_MODE_CENTER;
  } else {
    MyIfrNVData->IccNewSccMode = (UINT8) ICC_SCC_MODE_DOWN;
  } 
  MyIfrNVData->IccNewSccValue = (UINT8) CurrentSettings.SscPercent;
  MyIfrNVData->IccApply = ICC_APPLY_IMMEDIATE;

  //
  // Update Current Settings' Text
  //
  if (RefreshCurrentValueText) {
    
    StringBuffer = AllocateZeroPool (0x100);
    
    //
    // Frequency
    //
    FreqKHz = IccDivisorToFrequency(CurrentSettings.Frequency);
    UnicodeSPrint (StringBuffer, 0x100, L"%d.%d MHz", FreqKHz/100, FreqKHz%100);
    HiiSetString (HiiHandle, mIccSettingTokens[ClockIndex].FreqToken, StringBuffer, NULL);
    gBS->FreePool (StringBuffer);

    //
    // SCC Mode
    //
    if (CurrentSettings.SscMode & BIT0) {
      TempToken = STRING_TOKEN (STR_ICC_SSC_UP_TEXT);
    } else if (CurrentSettings.SscMode & BIT1) {
      TempToken = STRING_TOKEN (STR_ICC_SSC_CENTER_TEXT);
    } else if (CurrentSettings.SscMode & BIT2) {
      TempToken = STRING_TOKEN (STR_ICC_SSC_DOWN_TEXT);
    } else {
      TempToken = STRING_TOKEN (STR_ICC_NONE_TEXT);
    }
    gStringBuffer=HiiGetString( HiiHandle, TempToken, NULL );
    HiiSetString (HiiHandle, mIccSettingTokens[ClockIndex].SccModeToken, gStringBuffer, NULL);
    gBS->FreePool(gStringBuffer);
    //
    // SCC Value
    //
    if (CurrentSettings.SscPercent== 0) {
      gStringBuffer=HiiGetString( HiiHandle, STRING_TOKEN (STR_ICC_NONE_TEXT), NULL );
    } else {
      gStringBuffer = AllocateZeroPool (0x100);
      UnicodeSPrint (gStringBuffer, 0x100, L"%d.%d %%", CurrentSettings.SscPercent / 100,
                                                CurrentSettings.SscPercent % 100);    
    }  
    HiiSetString (HiiHandle, mIccSettingTokens[ClockIndex].SccValueToken, gStringBuffer, NULL);
    
    gBS->FreePool (gStringBuffer);
  }

  return EFI_SUCCESS;
}

/**
 Apply new settings to PCH/ICC, we can get target clock group from MyIfrNVData->IccClockPage

 @param [in]   HiiHandle
 @param [in]   MyIfrNVData

**/
EFI_STATUS
IccSettingsApply (
  IN  EFI_HII_HANDLE              HiiHandle,
  IN  CHIPSET_CONFIGURATION        *MyIfrNVData
)
{
  ICC_LIB_STATUS                  IccStatus;
  EFI_INPUT_KEY                   Key;
  CHAR16                          *StringBuffer1;
  CHAR16                          *StringBuffer2;
  CHAR16                          *StringBuffer3;
  UINT8                           IccClockIndex;
  ICC_CLOCK_SETTINGS              IccSettings;
  ICC_CLOCK_RANGES                CurrentClockRangeDef;
  UINTN                           ErrMsgIndex;

  StringBuffer2 = NULL;

  //
  // Confirm
  //
  StringBuffer1 = HiiGetString (HiiHandle, STRING_TOKEN(STR_ICC_APPLY_STRING), NULL);  
  switch (MyIfrNVData->IccApply) {

  case ICC_APPLY_IMMEDIATE:
    StringBuffer2 = HiiGetString (HiiHandle, STRING_TOKEN(STR_ICC_APPLY_IMMEDIATE_TEXT), NULL);    
    break;

  case ICC_APPLY_PERMANENT:
    StringBuffer2 = HiiGetString (HiiHandle, STRING_TOKEN(STR_ICC_APPLY_PERMANENT_TEXT), NULL);    
    break;

//[-start-130207-IB10870073-modify]//
  default:
    ASSERT (StringBuffer2 != NULL);
    if (StringBuffer2 == NULL) {
      return EFI_NOT_FOUND;
    }
//[-end-130207-IB10870073-modify]//
  }
  StringBuffer3 = AllocateZeroPool (StrSize(StringBuffer1) + StrSize(StringBuffer2));
//[-start-130207-IB10870073-add]//
  ASSERT (StringBuffer3 != NULL);
  if (StringBuffer3 == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//
  StrCat (StringBuffer3, StringBuffer1); 
  StrCat (StringBuffer3, StringBuffer2);

  gSUBrowser->H2ODialog->ConfirmDialog (
                         DlgOkCancel,   // NumberOfLines (???)
                         FALSE,         // HotKey
                         0,             // MaximumStringSize
                         NULL,          // StringBuffer
                         &Key,          // KeyValue
                         StringBuffer3
                         );
  
  gBS->FreePool (StringBuffer1);
  gBS->FreePool (StringBuffer2);
  gBS->FreePool (StringBuffer3);


  if (Key.UnicodeChar != CHAR_CARRIAGE_RETURN) {
    return EFI_ABORTED;
  }


  IccClockIndex = MyIfrNVData->IccClockPage;

  mIccProtocol->GetClockRanges (IccClockIndex, &CurrentClockRangeDef, &IccStatus);
  //
  // Get Current Settings
  //
  mIccProtocol->GetCurrentClockSettings (IccClockIndex, &IccSettings, &IccStatus);

  if (IccStatus != ICC_LIB_STATUS_SUCCESS) {
    return EFI_ABORTED;
  }

  // set New Value
  IccSettings.Frequency = (MyIfrNVData->IccNewDivisor * 10000) & (~BIT0);  // must be a even number
  
  if (CurrentClockRangeDef.SscChangeAllowed) {
    switch (MyIfrNVData->IccNewSccMode) {

      case ICC_SCC_MODE_UP:
        IccSettings.SscMode = ICC_SSC_UP;
        break;

      case ICC_SCC_MODE_CENTER:
        IccSettings.SscMode = ICC_SSC_CENTER;
        break;

      case ICC_SCC_MODE_DOWN:
        IccSettings.SscMode = ICC_SSC_DOWN;
        break;

      default:
        IccSettings.SscMode = ICC_SSC_NONE;
        break;
    }
    IccSettings.SscPercent= MyIfrNVData->IccNewSccValue;
  }

  //
  // submit to IccLib
  //
  switch (MyIfrNVData->IccApply) {

    case ICC_APPLY_IMMEDIATE:
      mIccProtocol->SetCurrentClockSettings (IccClockIndex, IccSettings, &IccStatus);
      break;

    case ICC_APPLY_PERMANENT:
      mIccProtocol->SetBootClockSettings (IccClockIndex, IccSettings, &IccStatus);
      break;
  }
  
  if (IccStatus == ICC_LIB_STATUS_SUCCESS) {
    StringBuffer1=HiiGetString( HiiHandle, STRING_TOKEN (STR_ICC_APPLY_OK_STRING), NULL );
    //
    // refresh "Current" value in the form
    //
    ResetIccClockForm (HiiHandle, MyIfrNVData, IccFormKeyList[IccClockIndex], TRUE);
  } else {
    ErrMsgIndex = IccStatus & 0xff;
    if (ErrMsgIndex > 0x1c) {
      ErrMsgIndex = 0x1c;
    }
    StringBuffer1 = AllocateZeroPool (0x100);
    StringBuffer2=HiiGetString( HiiHandle, STRING_TOKEN (STR_ICC_APPLY_FAIL_STRING), NULL );
    UnicodeSPrint (StringBuffer1, 0x100, L"%s : %s", StringBuffer2, IccLibStatusString[ErrMsgIndex]);
  }

  gSUBrowser->H2ODialog->ConfirmDialog (DlgOk, FALSE, 0, NULL, &Key, StringBuffer1);

  gBS->FreePool (StringBuffer1);
  gBS->FreePool (StringBuffer2);
  
  return EFI_SUCCESS;
}

/**
 convert Divisor to Frequency (uint : 10kHz)



**/
UINT32
IccDivisorToFrequency(
  IN UINT32 Divisor
)
{
  UINT32                FreqKHz = 0;

  FreqKHz = (UINT32)(Divisor / 10000);

  return FreqKHz;
}

//[-start-120315-IB06460374-add]//
/**
 Create the option about IccProfile.

 @param [in]   HiiHandle


**/
EFI_STATUS
IccProfileItemInit (
  IN  EFI_HII_HANDLE              HiiHandle
  )
{
  EFI_STATUS                              Status;
  UINT8                                   Index;
  EFI_SETUP_UTILITY_PROTOCOL              *SetupUtility; 
  CHIPSET_CONFIGURATION                    *SetupVariable;  
  CHIPSET_CONFIGURATION                    SetupNvData;  
  UINT16                                  OptionCount;
  UINT16                                  QuestionId;  
  UINT16                                  VarOffset;  
  DXE_PLATFORM_ICC_POLICY_PROTOCOL        *ICCPlatformPolicy;  
  STRING_REF                              StringTokenArray[] = {STRING_TOKEN(STR_ICC_PROFILE_0_TEXT), STRING_TOKEN(STR_ICC_PROFILE_1_TEXT),
                                                                STRING_TOKEN(STR_ICC_PROFILE_2_TEXT), STRING_TOKEN(STR_ICC_PROFILE_3_TEXT),
                                                                STRING_TOKEN(STR_ICC_PROFILE_4_TEXT), STRING_TOKEN(STR_ICC_PROFILE_5_TEXT),
                                                                STRING_TOKEN(STR_ICC_PROFILE_6_TEXT), STRING_TOKEN(STR_ICC_PROFILE_7_TEXT)};  
  VOID                                    *OptionsOpCodeHandle;
  VOID                                    *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL                      *StartLabel;
  
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  OptionsOpCodeHandle= HiiAllocateOpCodeHandle ();
  ASSERT ( OptionsOpCodeHandle != NULL);
  
  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = EXTENDED_ICC_FORM_ID;

  //
  //  get hob and call subfunction
  //
  Index = 0;

  Status = gBS->LocateProtocol ( &gDxePlatformICCPolicyGuid, NULL, (VOID **)&ICCPlatformPolicy );
  ASSERT_EFI_ERROR ( Status );
  
  QuestionId = (UINT16)((UINTN)(&SetupNvData.IccProfile)-(UINTN)(&SetupNvData));
  VarOffset = (UINT16)((UINTN)(&SetupNvData.IccProfile) - (UINTN)(&SetupNvData));
  
  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility);
  ASSERT_EFI_ERROR(Status);
  SetupVariable = (CHIPSET_CONFIGURATION *)(SetupUtility->SetupNvData);

  OptionCount = SetupVariable->IccSupportedProfilesNumber + 1;

  for (Index = 0; Index < OptionCount; Index ++) {
    HiiCreateOneOfOptionOpCode (
      OptionsOpCodeHandle,
      StringTokenArray[Index],
      EFI_IFR_OPTION_DEFAULT,
      EFI_IFR_NUMERIC_SIZE_1,
      (UINT16)Index
      );
  }

  HiiCreateOneOfOpCode (
    StartOpCodeHandle,
    QuestionId,
    CONFIGURATION_VARSTORE_ID,
    VarOffset,
    STRING_TOKEN (STR_ICC_PROFILE_STRING),
    STRING_TOKEN (STR_ICC_PROFILE_HELP),
    EFI_IFR_FLAG_CALLBACK,
    EFI_IFR_NUMERIC_SIZE_1,
    OptionsOpCodeHandle,
    NULL
    );
  
  HiiUpdateForm (
    HiiHandle,
    NULL, 
    (EFI_FORM_ID)EXTENDED_ICC_FORM_ID,
    StartOpCodeHandle,
    NULL
    );
  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (OptionsOpCodeHandle);


  return EFI_SUCCESS;
}

/**
 Set Icc Default Value When press hot key for "Load Optimal"

 @param [in]   HiiHandle
 @param [in]   Buffer


**/
EFI_STATUS
UpdateIccProfileDefaultValue (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  CHIPSET_CONFIGURATION              *Buffer
  )
{
  EFI_STATUS                              Status;
  DXE_PLATFORM_ICC_POLICY_PROTOCOL        *ICCPlatformPolicy;

  Status = gBS->LocateProtocol ( &gDxePlatformICCPolicyGuid, NULL, (VOID **)&ICCPlatformPolicy );
  ASSERT_EFI_ERROR ( Status );

  Buffer->IccProfile = ICCPlatformPolicy->IccDefaultProfile;
  return EFI_SUCCESS;
}
//[-end-120315-IB06460374-add]//
