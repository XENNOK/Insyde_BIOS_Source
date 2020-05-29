/** @file

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

#include <SetupUtility.h>
#include <Protocol/CpuIo2.h>
#include <Protocol/PchPlatformPolicy.h>
//[-start-120730-IB03600494-add]//
#include <Protocol/PchReset.h>
//[-end-120730-IB03600494-add]//
//[-start-130611-IB11120016-add]//
#include <Library/DxeIpmiSetupUtilityLib.h>
//[-end-130611-IB11120016-add]//
//[-start-130403-IB02950501-add]//
#include <CpuRegs.h>
#include <Library/CpuPlatformLib.h>
//[-end-130403-IB02950501-add]//
//[-start-131129-IB09740048-add]//
#include <Library/CrVfrConfigLib.h>
//[-end-131129-IB09740048-add]//
//[-start-140320-IB06040445-modify]//
#include <Library/MultiConfigBaseLib.h>
#include <Library/VariableSupportLib.h>
//[-end-140320-IB06040445-modify]//

UINT16                          gSCUSystemHealth;
EFI_EVENT                       gSCUTimerEvent;
BOOLEAN                         mIsSaveWithoutExit;
BOOLEAN                         mPopLoadDefaultDialog = TRUE;

STATIC
EFI_STATUS
FullReset (
  IN  BOOLEAN        GlobalReset
)
{
//[-start-120730-IB03600494-modify]//
  EFI_STATUS                 Status;
  EFI_CPU_IO2_PROTOCOL       *CpuIo;
  UINT8                      ResetFlag;
  UINT32                     GpioBaseAddr;
  UINT32                     Data32;
  CHIPSET_CONFIGURATION       *NVData;
  UINTN                      VariableSize = 0;
  EFI_GUID                   SetupVariableGuidId = SYSTEM_CONFIGURATION_GUID;
  PCH_RESET_PROTOCOL         *PchResetProtocol;

  Status = EFI_SUCCESS;

  //
  // Using PCH Reset, first.
  //
  if (GlobalReset) {
    Status = gBS->LocateProtocol (
                    &gPchResetProtocolGuid,
                    NULL,
                    (VOID **)&PchResetProtocol
                    );
    if (!EFI_ERROR (Status)) {
      Status = PchResetProtocol->Reset (PchResetProtocol, GlobalResetWithEc);
    }
  }
//[-start-120730-IB06150235-remove]//
//  Status = HeciSendCbmResetRequest (CBM_RR_REQ_ORIGIN_BIOS_POST, CBM_HRR_GLOBAL_RESET);
//[-end-120730-IB06150235-remove]//
//[-start-120730-IB06150235-remove]//
//  if (EFI_ERROR(Status)) {
//[-end-120730-IB06150235-remove]//

  GpioBaseAddr = PchLpcPciCfg32 (R_PCH_LPC_GPIO_BASE) & (B_PCH_LPC_GPIO_BASE_BAR);

  //
  // When PCH Reset fail, do full reset here.
  //
  if (EFI_ERROR (Status)) {
    Status = gBS->LocateProtocol ( &gEfiCpuIo2ProtocolGuid, NULL, (VOID **)&CpuIo );
    if ( !EFI_ERROR ( Status ) ) {
      if (GlobalReset) {
        //
        // CF9h Global Reset:
        //
        NVData = GetVariableAndSize (
               L"Setup",
               &SetupVariableGuidId,
               &VariableSize
               );
//[-start-130207-IB10870073-add]//
        ASSERT (NVData != NULL);
        if (NVData == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
//[-end-130207-IB10870073-add]//
        if((NVData->BoardCapability == 0 ) || (NVData->DeepSlpSx == 0)) {
          //
          // 1. Set GPIOBASE + 00h[30] = 1b (for non-Deep S4/S5 enabled platforms)
          //
          Status = CpuIo->Io.Read ( CpuIo, EfiCpuIoWidthUint32, GpioBaseAddr, 1, &Data32 );
          Data32 |= BIT30;
          Status = CpuIo->Io.Write ( CpuIo, EfiCpuIoWidthUint32, GpioBaseAddr, 1, &Data32 );
          //
          // 2. Set GPIOBASE + 0Ch[30] = 0b (for non-Deep S4/S5 enabled platforms)
          //
          Status = CpuIo->Io.Read ( CpuIo, EfiCpuIoWidthUint32, GpioBaseAddr + R_PCH_GPIO_LVL, 1, &Data32 );
          Data32 &= (~BIT30);
          Status = CpuIo->Io.Write ( CpuIo, EfiCpuIoWidthUint32, GpioBaseAddr + R_PCH_GPIO_LVL, 1, &Data32 );
          //
          // 3. Set GPIOBASE + 60h[30] = 1b (for non-Deep S4/S5 enabled platforms)
          //
          Status = CpuIo->Io.Read ( CpuIo, EfiCpuIoWidthUint32, GpioBaseAddr + R_PCH_GP_RST_SEL, 1, &Data32 );
          Data32 |= BIT30;
          Status = CpuIo->Io.Write ( CpuIo, EfiCpuIoWidthUint32, GpioBaseAddr + R_PCH_GP_RST_SEL, 1, &Data32 );
        }
        //
        // Set CF9GR bit, D31:F0:ACh[20],
        //
//[-start-120807-IB04770241-modify]//
        PchLpcPciCfg32Or ( R_PCH_LPC_PMIR, B_PCH_LPC_PMIR_CF9GR );
//[-end-120807-IB04770241-modify]//
      }
      //
      //issue a Global Reset through a CF9h write of either 06h or 0Eh commands.
      //
      ResetFlag = V_RST_CNT_FULLRESET;
      Status = CpuIo->Io.Write ( CpuIo, EfiCpuIoWidthUint8, RST_CNT, 1, &ResetFlag );
    }
//[-start-120730-IB06150235-remove]//
//  }
//[-end-120730-IB06150235-remove]//
  }

  //
  // When run into here, means Global Reset fail.
  // Using ME to do Global Reset
  //
  if (GlobalReset) {
    //
    // Global Reset MEI Message
    // BIOS makes sure GPIO30 is left in native mode (default mode) before sending
    // a Global Reset MEI message.
    //
    Data32 = IoRead32((UINT16)(GpioBaseAddr ));
    if ((Data32 & BIT30) != 0) {
      Data32 &= (~BIT30);
      IoWrite32((UINT16)(GpioBaseAddr ), Data32);
    }
    if (FeaturePcdGet (PcdMeSupported)) {
      Status = HeciSendCbmResetRequest (CBM_RR_REQ_ORIGIN_BIOS_POST, CBM_HRR_GLOBAL_RESET);
    }
    CpuDeadLoop();
  }
//[-end-120730-IB03600494-modify]//

  //
  // Should NOT reach here.
  //

  return Status;
}

EFI_STATUS
HotKeyCallBackByAction (
  IN CONST  EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN  EFI_BROWSER_ACTION                    Action,
  IN  EFI_QUESTION_ID                       QuestionId,
  IN  UINT8                                 Type,
  IN  EFI_IFR_TYPE_VALUE                    *Value,
  OUT EFI_BROWSER_ACTION_REQUEST            *ActionRequest
  )
{
  EFI_STATUS                     Status;

  Status = EFI_UNSUPPORTED;

  switch (Action) {

  case EFI_BROWSER_ACTION_CHANGING:
    return EFI_SUCCESS;

  case EFI_BROWSER_ACTION_DEFAULT_MANUFACTURING:
    if (QuestionId == KEY_SCAN_F9) {
      mPopLoadDefaultDialog = FALSE;
      Status = HotKeyCallBack (
                 This,
                 EFI_BROWSER_ACTION_CHANGED,
                 QuestionId,
                 Type,
                 Value,
                 ActionRequest
                 );
      mPopLoadDefaultDialog = TRUE;
      BrowserRefreshFormSet ();
    }
    //
    // avoid GetQuestionDefault execute ExtractConfig, so always return success.
    //
    return EFI_SUCCESS;

  default:
    break;
  }

  return Status;
}

/**
 Proccess HotKey function

 @param [in]   This
 @param [in]   HiiHandle
 @param [in]   Action
 @param [in]   QuestionId
 @param [in]   Type
 @param [in]   Value
 @param [out]  ActionRequest


**/
EFI_STATUS
EFIAPI
HotKeyCallBack (
  IN CONST  EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN  EFI_BROWSER_ACTION                    Action,
  IN  EFI_QUESTION_ID                       QuestionId,
  IN  UINT8                                 Type,
  IN  EFI_IFR_TYPE_VALUE                    *Value,
  OUT EFI_BROWSER_ACTION_REQUEST            *ActionRequest

)
{

  CHIPSET_CONFIGURATION                      *MyIfrNVData;
  CHAR16                                    *StringPtr;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  EFI_INPUT_KEY                             Key;
  EFI_STATUS                                Status;
  UINT16                                    *BootOrderList;
  UINTN                                     BufferSize = 0;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL        *Interface;
  UINT8                                     CmosVmxSmxFlag;
  UINT8                                     TempCmosVmxSmxFlag;
  UINT8                                     CmosPlatformSetting;
//[-start-121201-IB03780468-remove]//
//  VOID                                      *HobList;
//  XTU_INFO_DATA                             *XTUInfoHobData;
//[-end-121201-IB03780468-remove]//
  UINT16                                    PmBase;
  UINT16                                    Pm1EnData;
  CHIPSET_CONFIGURATION                      *NVData;
  UINTN                                     VariableSize = 0;
  EFI_GUID                                  SetupVariableGuidId = SYSTEM_CONFIGURATION_GUID;
  EFI_HII_HANDLE                            HiiHandle;

  if (Action != EFI_BROWSER_ACTION_CHANGED) {
    return HotKeyCallBackByAction (This, Action, QuestionId, Type, Value, ActionRequest);
  }

  BootOrderList       = NULL;
  SUCInfo             = gSUBrowser->SUCInfo;
  MyIfrNVData = (CHIPSET_CONFIGURATION *) gSUBrowser->SCBuffer;
  CmosPlatformSetting = 0;
  Status              = EFI_SUCCESS;
  HiiHandle           = mDriverHiiHandle;
  *ActionRequest      = EFI_BROWSER_ACTION_REQUEST_NONE;

  switch (QuestionId) {

  case KEY_SCAN_ESC :
    //
    // Discard setup and exit
    //
    StringPtr=HiiGetString (HiiHandle, STRING_TOKEN (STR_EXIT_DISCARDING_CHANGES_STRING), NULL);
    gSUBrowser->H2ODialog->ConfirmDialog (
                             0,
                             FALSE,
                             0,
                             NULL,
                             &Key,
                             StringPtr
                             );
    //[-start-131129-IB09740048-add]//
    CRScuHotKeyCallback (QuestionId, Key);
    //[-end-131129-IB09740048-add]//
    if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      if ( mFullResetFlag == 2 ) {
        Status = FullReset (TRUE);
        ASSERT_EFI_ERROR (Status);
      }
      CheckLanguage ();
      *ActionRequest = EFI_BROWSER_ACTION_REQUEST_EXIT;
    }

    gBS->FreePool (StringPtr);
    break;

  case KEY_SCAN_F9 :
    //
    // Load Optimal
    //
    // If user Accesslevel = USER_PASSWORD_VIEW_ONLY(0x02) or USER_PASSWORD_LIMITED(0x03)
    // and use user password into SCU,the user don't use load optimal function.
    if ((MyIfrNVData->SetUserPass == TRUE) &&
        ((MyIfrNVData->UserAccessLevel == 2) ||
        (MyIfrNVData->UserAccessLevel == 3))) {
      return EFI_ABORTED;
    }

    EventTimerControl(0);

    //
    // Load optimal setup.
    //
    Key.UnicodeChar = CHAR_CARRIAGE_RETURN;
    if (mPopLoadDefaultDialog) {
      StringPtr = HiiGetString (HiiHandle, STRING_TOKEN (STR_LOAD_OPTIMAL_DEFAULTS_STRING), NULL);
    gSUBrowser->H2ODialog->ConfirmDialog (
                               0,
                               FALSE,
                               0,
                               NULL,
                               &Key,
                               StringPtr
                               );
      gBS->FreePool (StringPtr);
      //[-start-131129-IB09740048-add]//
      CRScuHotKeyCallback (QuestionId, Key);
      //[-end-131129-IB09740048-add]//
      if (Key.UnicodeChar != CHAR_CARRIAGE_RETURN) {
        EventTimerControl(TIMEOUT_OF_EVENT);
        return EFI_UNSUPPORTED;
      }
    }

    if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      Status = gRT->SetVariable (
                      L"PlatformLang",
                      &gEfiGlobalVariableGuid,
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                      AsciiStrSize ((CHAR8 *) PcdGetPtr (PcdUefiVariableDefaultPlatformLang)),
                      (VOID *) PcdGetPtr (PcdUefiVariableDefaultPlatformLang)
                      );
      if (!EFI_ERROR (Status)) {
        GetLangIndex ((CHAR8 *) PcdGetPtr (PcdUefiVariableDefaultPlatformLang), &MyIfrNVData->Language);
      }

      if (FeaturePcdGet (PcdH2OFormBrowserSupported)) {
        BrowserRefreshFormSet ();
      } else {
        Status = gBS->LocateProtocol (
                        &gEfiSetupUtilityBrowserProtocolGuid,
                        NULL,
                        (VOID **)&Interface
                        );
        if (EFI_ERROR (Status)) {
          return Status;
        }
        Interface->Firstin = TRUE;
      }
      //
      //Remove this condition, because no loneger use Changelanguage Flag to determine chagelanguage
      //
      UpdatePasswordState (gSUBrowser->SUCInfo->MapTable[SecurityHiiHandle].HiiHandle);

      BootOrderList = GetVariableAndSize (
                        L"BootOrder",
                        &gEfiGlobalVariableGuid,
                        &BufferSize
                        );

      if (BufferSize != 0) {
         gBS->FreePool (SUCInfo->BootOrder);
         SUCInfo->BootOrder = BootOrderList;
         SUCInfo->AdvBootDeviceNum  = (UINT16) (BufferSize / sizeof(UINT16));
       }

      MyIfrNVData->BootMenuType         = NORMAL_MENU;
      MyIfrNVData->BootNormalPriority   = EFI_FIRST;
      MyIfrNVData->LegacyNormalMenuType = NORMAL_MENU;
      //
      // reset the flag of Internal Graphics Display selection
      //
      MyIfrNVData->IGDBootType = SCU_IGD_BOOT_TYPE_VBIOS_DEFAULT;
      //
      //  reset the flag of showing from.
      //

//[-start-140320-IB06040445-modify]//
      if (FeaturePcdGet (PcdMultiConfigSupported)) {
        UINT16                ConfigCount;
        UINT16                Index;
        VOID                  *NamePtr;
        UINTN                 MultiConfigDataSize;
        VOID                  *MultiConfigData;

        Index = 0;
        MultiConfigDataSize = PcdGet32 (PcdSetupConfigSize);;
        ConfigCount = GetConfigCount();

        for ( ; Index < ConfigCount; Index ++) {
          //
          // Get Active (Attribute: ACTIVE & LOAD_DEFAULT) Config Data from Multi Config Region
          //
          Status = GetFullSetupSetting (
                    SETUP_FOR_LOAD_DEFAULT,
                    &Index,
                    MultiConfigDataSize,
                    &MultiConfigData,
                    NULL,
                    NULL
                     );
          if (!EFI_ERROR (Status)) {
            NamePtr = (VOID *) GET_VARIABLE_NAME_PTR (MultiConfigData);
            if (CompareMem (L"Setup", NamePtr, NameSizeOfVariable (MultiConfigData)) == 0) {
              CopyMem (MyIfrNVData, GetVariableDataPtr (MultiConfigData), MultiConfigDataSize);
              break;
            }
          }
        }

        if (EFI_ERROR (Status)) {
          DefaultSetup (MyIfrNVData);
        }
      } else {
        DefaultSetup (MyIfrNVData);
      }
//[-end-140320-IB06040445-modify]//

    SetupRuntimeDetermination ( MyIfrNVData );

  if ( SUCInfo->LegacyBootDeviceNum != 0 ) {
    MyIfrNVData->NoLegacyBootDev = 1;
  }

  if ( SUCInfo->EfiBootDeviceNum != 0 ) {
    MyIfrNVData->NoEfiBootDev = 1;
  }

//[-start-121201-IB03780468-remove]//
//    //
//    // For XTU, some options ware created dynamically, their default value need process additionally
//    //
//
//    // Get XTU info hob
//  if (FeaturePcdGet(PcdXtuSupported)){
//    HobList = GetHobList ();
//
//    XTUInfoHobData=GetNextGuidHob ( &gXTUInfoHobGuid, HobList);
//
//    // Non-Turbo Ratio
//    MyIfrNVData->FlexRatio                = XTUInfoHobData->CpuNonTurboRatio.MaxRatio;
//    // Ratio Limit Core 1~4
//    MyIfrNVData->RatioLimit1Core          = XTUInfoHobData->TurboRatioLimitDefault.LimitCore1;
//    MyIfrNVData->RatioLimit2Core          = XTUInfoHobData->TurboRatioLimitDefault.LimitCore2;
//    MyIfrNVData->RatioLimit3Core          = XTUInfoHobData->TurboRatioLimitDefault.LimitCore3;
//    MyIfrNVData->RatioLimit4Core          = XTUInfoHobData->TurboRatioLimitDefault.LimitCore4;
//    // Turbo Power Limit 1&2
//    MyIfrNVData->LongDurationPowerLimit   = XTUInfoHobData->TruboTdpLimitDefault.PowerLimit1Default;
//    MyIfrNVData->ShortDurationPowerLimit  = XTUInfoHobData->TruboTdpLimitDefault.PowerLimit2Default;
//  }
//[-end-121201-IB03780468-remove]//

      UpdateHDCConfigure (
        gSUBrowser->SUCInfo->MapTable[AdvanceHiiHandle].HiiHandle,
        MyIfrNVData
        );
//[-start-120315-IB06460374-add]//
      if (FeaturePcdGet (PcdMeSupported)) {
        UpdateIccProfileDefaultValue (
          gSUBrowser->SUCInfo->MapTable[AdvanceHiiHandle].HiiHandle,
          MyIfrNVData
        );
      }
//[-end-120315-IB06460374-add]//
      UpdateHddPasswordLabel (
        gSUBrowser->SUCInfo->MapTable[AdvanceHiiHandle].HiiHandle,
        gSUBrowser->SUCInfo->HarddiskPassword,
        gSUBrowser->SUCInfo->NumOfHarddiskPswd
        );
      UpdateStringToken ((KERNEL_CONFIGURATION *) MyIfrNVData);

      NVData = GetVariableAndSize (
                 L"Setup",
                 &SetupVariableGuidId,
                 &VariableSize
                 );
//[-start-130207-IB10870073-add]//
      ASSERT (NVData != NULL);
      if (NVData == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
//[-end-130207-IB10870073-add]//
//[-start-120116-IB03600466-remove]//
//      if (
//#ifdef TXT_SUPPORT
//        ( MyIfrNVData->TXT               != NVData->TXT               ) ||
//#endif
//#ifdef RAPID_START_FLAG
//        ( MyIfrNVData->EnableRapidStart  != NVData->EnableRapidStart  ) ||
//#endif
//#ifdef XTU_SUPPORT
//        ( MyIfrNVData->CpuBurnInEnable   != NVData->CpuBurnInEnable   ) ||
//        ( MyIfrNVData->MemoryVoltage     != NVData->MemoryVoltage     ) ||
//        ( MyIfrNVData->XmpProfileSetting != NVData->XmpProfileSetting ) ||
//        ( MyIfrNVData->MemoryTcl         != NVData->MemoryTcl         ) ||
//        ( MyIfrNVData->MemoryTrcd        != NVData->MemoryTrcd        ) ||
//        ( MyIfrNVData->MemoryTrp         != NVData->MemoryTrp         ) ||
//        ( MyIfrNVData->MemoryTras        != NVData->MemoryTras        ) ||
//        ( MyIfrNVData->MemoryTwr         != NVData->MemoryTwr         ) ||
//        ( MyIfrNVData->MemoryTrfc        != NVData->MemoryTrfc        ) ||
//        ( MyIfrNVData->MemoryTrrd        != NVData->MemoryTrrd        ) ||
//        ( MyIfrNVData->MemoryTwtr        != NVData->MemoryTwtr        ) ||
//        ( MyIfrNVData->MemoryTrtp        != NVData->MemoryTrtp        ) ||
//        ( MyIfrNVData->MemoryTrc         != NVData->MemoryTrc         ) ||
//        ( MyIfrNVData->MemoryTfaw        != NVData->MemoryTfaw        ) ||
//#endif
//        ( MyIfrNVData->PrimaryDisplay    != NVData->PrimaryDisplay    ) ||
//        ( MyIfrNVData->VTSupport         != NVData->VTSupport         ) ||
//        ( MyIfrNVData->DvmtPreAllocated  != NVData->DvmtPreAllocated  ) ||
//        ( MyIfrNVData->GttSize           != NVData->GttSize           ) ||
//        ( MyIfrNVData->IGDControl        != NVData->IGDControl        ) ||
//        ( MyIfrNVData->ApertureSize      != NVData->ApertureSize      ) ||
////[-start-120316-IB06460376-remove]//
////        ( MyIfrNVData->BootPState        != NVData->BootPState        ) ||
////[-end-120316-IB06460376-remove]//
//        ( MyIfrNVData->PegGenx0          != NVData->PegGenx0          ) ||
//        ( MyIfrNVData->PegGenx1          != NVData->PegGenx1          ) ||
//        ( MyIfrNVData->PegGenx2          != NVData->PegGenx2          ) ||
//        ( MyIfrNVData->PegGenx3          != NVData->PegGenx3          ) ||
//        ( MyIfrNVData->CoreActive        != NVData->CoreActive        )
//        ) {
//        if ( !mFullResetFlag ) {
//          mFullResetFlag = 1;
//        }
//      }
//[-end-120116-IB03600466-remove]//
      if (
        ( MyIfrNVData->DvmtPreAllocated != NVData->DvmtPreAllocated ) ||
        ( MyIfrNVData->ApertureSize     != NVData->ApertureSize     )
        ) {
        CmosPlatformSetting = ReadExtCmos8 ( R_XCMOS_INDEX, R_XCMOS_DATA, PlatformSettingFlag );
        CmosPlatformSetting = CmosPlatformSetting | B_SETTING_MEM_REFRESH_FLAG;
        WriteExtCmos8 ( R_XCMOS_INDEX, R_XCMOS_DATA, PlatformSettingFlag, CmosPlatformSetting );
      }
      FreePool (NVData);
    }
    EventTimerControl(TIMEOUT_OF_EVENT);
//[-start-130611-IB11120016-add]//
    IpmiConfigRestore (HiiHandle, MyIfrNVData, TRUE);
//[-end-130611-IB11120016-add]//
    break;

  case KEY_SCAN_F10 :
    //
    // If user Accesslevel = USER_PASSWORD_VIEW_ONLY(0x02) or USER_PASSWORD_LIMITED(0x03)
    // and use user password into SCU,the user don't use load optimal function.
    //
    if ((MyIfrNVData->SetUserPass == TRUE) &&
        (MyIfrNVData->UserAccessLevel == 2)) {
      return EFI_ABORTED;
    }
    EventTimerControl (0);
    //
    // Save setup and exit.
    //
    StringPtr=HiiGetString (HiiHandle, STRING_TOKEN (STR_EXIT_SAVING_CHANGES_STRING), NULL);

    gSUBrowser->H2ODialog->ConfirmDialog (
                             0,
                             FALSE,
                             0,
                             NULL,
                             &Key,
                             StringPtr
                             );
    //[-start-131129-IB09740048-add]//
    CRScuHotKeyCallback (QuestionId, Key);
	//[-end-131129-IB09740048-add]//
    if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      GenericRouteConfig ((EFI_HII_CONFIG_ACCESS_PROTOCOL *) This, NULL, NULL);
      //
      // set the request action to save all value and then exit
      //
      gBS->RaiseTPL (TPL_NOTIFY);
      gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
    }

    gBS->FreePool (StringPtr);
    EventTimerControl (TIMEOUT_OF_EVENT);
    break;

    case KEY_TXT:
      if (FeaturePcdGet(PcdTXTSupported)){
        TempCmosVmxSmxFlag = ReadExtCmos8 ( R_XCMOS_INDEX, R_XCMOS_DATA, VmxSmxFlag );
        CmosVmxSmxFlag     = TempCmosVmxSmxFlag;
        if ( MyIfrNVData->TXT ) {
          CmosVmxSmxFlag = CmosVmxSmxFlag | B_SMX_SETUP_FLAG;
        } else {
          CmosVmxSmxFlag = CmosVmxSmxFlag & ~B_SMX_SETUP_FLAG;
        }

        if ( CmosVmxSmxFlag != TempCmosVmxSmxFlag ) {
          WriteExtCmos8 ( R_XCMOS_INDEX, R_XCMOS_DATA, VmxSmxFlag, CmosVmxSmxFlag );
          if (mFullResetFlag == 0) {
            mFullResetFlag = 1;
          }
        }
      }
    break;
    case KEY_RAPID_START_ENABLE :
      if (FeaturePcdGet(PcdRapidStartSupported)){
        NVData = GetVariableAndSize (
                    L"Setup",
                    &SetupVariableGuidId,
                    &VariableSize
                    );
//[-start-130207-IB10870073-add]//
        ASSERT (NVData != NULL);
        if (NVData == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
//[-end-130207-IB10870073-add]//
        //
        // enable RapidStart ,need set After G3 state as S5
        //
        if (MyIfrNVData->EnableRapidStart){
           MyIfrNVData->StateAfterG3 = 1 ;  //S5
        } else {
           MyIfrNVData->StateAfterG3 = NVData->StateAfterG3;
        }
      }
//[-start-120116-IB03600466-remove]//
//    if (MyIfrNVData->EnableRapidStart != NVData->EnableRapidStart){
//      if ( !mFullResetFlag ) {
//        mFullResetFlag = 1;
//      }
//    }
//[-end-120116-IB03600466-remove]//
    break;
//[-start-120604-IB06150223-modify]//
    case KEY_ISCT_ENABLE :
      //
      // Spec #471987 section 4.1 BIOS Requirements :
      //  "Ensure the PCI Express Wake Disable (PCIEXP_WAKE_DIS) bit 14 in PM1_EN PCH register is set to 0b to allow wakes by the WLAN card"
      //  "Disable RTC wake from S4(OS Hibernate.)"
      //
      if( MyIfrNVData->ISCTEnable ) {
//[-start-120704-IB03600491-remove]//
//        MyIfrNVData->FacpRTCS4Wakeup = 0 ;  //disable
//[-end-120704-IB03600491-remove]//
        if (MyIfrNVData->WakeOnPME !=1){
          MyIfrNVData->WakeOnPME = 1;       //enable wake on PME
          goto LABEL_WAKE_ON_PME;
        }
      }
      break;
//[-end-120604-IB06150223-modify]//
//[-start-120920-IB05330379-add]//
    case KEY_AZALIA_CHANGE :
      if (MyIfrNVData->Azalia){
//[-start-121115-IB06150259-modify]//
        MyIfrNVData->EnableSerialIoAudioDsp = 0;
//[-end-121115-IB06150259-modify]//
      }
      break;

    case KEY_DSP_CHANGE :
//[-start-121115-IB06150259-modify]//
      if (MyIfrNVData->EnableSerialIoAudioDsp){
//[-end-121115-IB06150259-modify]//
        MyIfrNVData->Azalia = 0;
      }
      break;
//[-end-120920-IB05330379-add]//

    case KEY_XHCI_PREBOOT_SUPPORT :
    //
    //  - If BIOS supports xHCI pre-boot driver then use Smart Auto mode as default
    //  - If BIOS does not support xHCI pre-boot driver then use AUTO mode as default
    //
      if (MyIfrNVData->XhciPreBootSupport) {
        MyIfrNVData->XHCIMode = PCH_XHCI_MODE_SMARTAUTO ;
      } else {
        MyIfrNVData->XHCIMode = PCH_XHCI_MODE_AUTO ;
      }
    break;

//[-start-140716-IB12740059-remove]//
//    case KEY_TPM_CLEAR:
//      if (FeaturePcdGet(PcdH2OTpmSupported)){
//        if ( MyIfrNVData->TpmDeviceOk && MyIfrNVData->TpmClear == 1) {
//          MyIfrNVData->TpmOperation = 2;
//        }
//      }
//    break;
//[-end-140716-IB12740059-remove]//
//[-start-130220-IB02950497-add]//
    case KEY_XHCI_MANUAL_OPTION :
      //
      // If change XHCI switch the Manual mode switch will resume to default value.
      //
      MyIfrNVData->USBRouteSwitch = 1;
      MyIfrNVData->USB30SuperSpeed = 1;

    break;
//[-end-130220-IB02950497-add]//
//[-start-130403-IB02950501-add]//
    case KEY_XHCI_PB_OPTION :
    case KEY_XHCI_OPTION :
    case KEY_USBRS_OPTION :
    case KEY_EHCI1_OPTION :
    case KEY_EHCI2_OPTION :

      if (!(GetCpuFamily() == EnumCpuHswUlt)) {
        if ((MyIfrNVData->XHCIMode == 1) || (MyIfrNVData->USBRouteSwitch == 2) || ((MyIfrNVData->Ehci1Enable == 0) && (MyIfrNVData->Ehci2Enable == 0))) {
          MyIfrNVData->UsbPerPortCtl = 0;
        }
      } else {
        if ((MyIfrNVData->XHCIMode == 1) || (MyIfrNVData->USBRouteSwitch == 2) || (MyIfrNVData->Ehci1Enable == 0)) {
          MyIfrNVData->UsbPerPortCtl = 0;
        }
      }
    break;
//[-end-130403-IB02950501-add]//
//[-start-120116-IB03600466-remove]//
//#ifdef XTU_SUPPORT
//    case KEY_XTU_ITEM:
//    case KEY_XTU_ITEM_01:
//    case KEY_XTU_ITEM_02:
//    case KEY_XTU_ITEM_03:
//    case KEY_XTU_ITEM_04:
//    case KEY_XTU_ITEM_05:
//    case KEY_XTU_ITEM_06:
//    case KEY_XTU_ITEM_07:
//    case KEY_XTU_ITEM_08:
//    case KEY_XTU_ITEM_09:
//    case KEY_XTU_ITEM_0A:
//    case KEY_XTU_ITEM_0B:
//    case KEY_XTU_ITEM_0C:
//    case KEY_XTU_ITEM_0D:
//    case KEY_XTU_ITEM_0E:
//        if (mFullResetFlag == 0) {
//          mFullResetFlag = 1;
//        }
//      break;
//#endif
//[-end-120116-IB03600466-remove]//
    case KEY_VT_CONFIG:
      TempCmosVmxSmxFlag = ReadExtCmos8 ( R_XCMOS_INDEX, R_XCMOS_DATA, VmxSmxFlag );
      CmosVmxSmxFlag     = TempCmosVmxSmxFlag;
      if ( MyIfrNVData->VTSupport ) {
        CmosVmxSmxFlag = CmosVmxSmxFlag | B_VMX_SETUP_FLAG;
      } else {
        CmosVmxSmxFlag = CmosVmxSmxFlag & ~B_VMX_SETUP_FLAG;
      }

//[-start-120116-IB03600466-remove]//
//      if ( !mFullResetFlag ) {
//        mFullResetFlag = 1;
//      }
//[-end-120116-IB03600466-remove]//
      break;

    case KEY_DVMT_PREALLOCATE:
      CmosPlatformSetting = ReadExtCmos8 ( R_XCMOS_INDEX, R_XCMOS_DATA, PlatformSettingFlag );
      CmosPlatformSetting = CmosPlatformSetting | B_SETTING_MEM_REFRESH_FLAG;
      WriteExtCmos8 ( R_XCMOS_INDEX, R_XCMOS_DATA, PlatformSettingFlag, CmosPlatformSetting );

//[-start-120116-IB03600466-remove]//
//      if ( !mFullResetFlag ) {
//        mFullResetFlag = 1;
//      }
//[-end-120116-IB03600466-remove]//
      break;
//[-start-120116-IB03600466-remove]//
//    case KEY_GTT:
//        if ( !mFullResetFlag ) {
//          mFullResetFlag = 1;
//        }
//      break;
//    case KEY_IGD_CONTROL:
//        if ( !mFullResetFlag ) {
//          mFullResetFlag = 1;
//        }
//      break;
//#ifdef DDR3LV_SUPPORT
//    case KEY_DDR3LV_OPTION:
//        if ( !mFullResetFlag ) {
//          mFullResetFlag = 1;
//        }
//      break;
//#endif
//[-end-120116-IB03600466-remove]//
    case KEY_APERTURE_SIZE:
      CmosPlatformSetting = ReadExtCmos8 ( R_XCMOS_INDEX, R_XCMOS_DATA, PlatformSettingFlag );
      CmosPlatformSetting = CmosPlatformSetting | B_SETTING_MEM_REFRESH_FLAG;
      WriteExtCmos8 ( R_XCMOS_INDEX, R_XCMOS_DATA, PlatformSettingFlag, CmosPlatformSetting );
//[-start-120116-IB03600466-remove]//
//      if ( !mFullResetFlag ) {
//        mFullResetFlag = 1;
//      }
//[-end-120116-IB03600466-remove]//
      break;
//[-start-120116-IB03600466-remove]//
//    case KEY_PRIMARY_DISPLAY:
//        if ( !mFullResetFlag ) {
//          mFullResetFlag = 1;
//        }
//      break;
//[-end-120116-IB03600466-remove]//

    LABEL_WAKE_ON_PME:
    case KEY_WAKE_ON_PME:
      PmBase = PchLpcPciCfg16 (R_PCH_LPC_ACPI_BASE) & 0xFFFE;
      Pm1EnData = IoRead16 (PmBase + R_PCH_ACPI_PM1_EN);

      if ( MyIfrNVData->WakeOnPME ) {
        Pm1EnData &= ~BIT14; // B_PCH_ACPI_PM1_EN_PCIEXPWAK_DIS;
      } else {
        Pm1EnData |= BIT14;
      }
      IoWrite16 (PmBase + R_PCH_ACPI_PM1_EN, Pm1EnData);
    break;

//[-start-120116-IB03600466-remove]//
//    case KEY_FULL_RESET:
//    case KEY_PEG_FULL_RESET_0:
//    case KEY_PEG_FULL_RESET_1:
//    case KEY_PEG_FULL_RESET_2:
//    case KEY_PEG_FULL_RESET_3:
//    case KEY_COREACTIVE_FULL_RESET:
//
//        if ( !mFullResetFlag ) {
//          mFullResetFlag = 1;
//        }
//      break;
//[-end-120116-IB03600466-remove]//

//[-start-110829-IB07370065-add]//
  case KEY_BOOT_MODE_TYPE:
    //
    // For IGD display selection:
    // if user switch mode, suppress the display selection in this boot.
    // due to the display selection is decided by LegacyBiosProtocol.
    //
    MyIfrNVData->IGDBootType = SCU_IGD_BOOT_TYPE_VBIOS_DEFAULT;
    MyIfrNVData->IGDBootTypeSecondary = SCU_IGD_BOOT_TYPE_DISABLE;
//[-start-130401-IB05400394-modify]//
//[-start-121207-IB04770253-modify]//
    ClearFormDataFromLabel (HiiHandle, IGD_FORM_ID, IGD_DISPLAY_SELECTION_START_LABEL, IGD_DISPLAY_SELECTION_END_LABEL);
//[-end-121207-IB04770253-modify]//
//[-end-130401-IB05400394-modify]//
    //
    // For Plug-in display selection:
    //
//[-start-130401-IB05400394-modify]//
    ClearFormDataFromLabel (HiiHandle, VIDEO_CONFIG_FORM_ID, PLUG_IN_DISPLAY_SELECTION_START_LABEL, 0);
//[-end-130401-IB05400394-modify]//
    //
    // For dual vga controller supported:
    // It only supported the Uefi mode.
    //
//[-start-111101-IB07370070-modify]//
    if (Value->u8 != EFI_BOOT_TYPE && MyIfrNVData->UefiDualVgaControllers == DUAL_VGA_CONTROLLER_ENABLE) {
//[-end-111101-IB07370070-modify]//
      MyIfrNVData->UefiDualVgaControllers = DUAL_VGA_CONTROLLER_DISABLE;
//[-start-121207-IB04770253-modify]//
      MyIfrNVData->PrimaryDisplay = DISPLAY_MODE_AUTO;
//[-end-121207-IB04770253-modify]//
//[-start-130401-IB05400394-modify]//
      ClearFormDataFromLabel (HiiHandle, VIDEO_CONFIG_FORM_ID, DUAL_VGA_SUPPORT_START_LABEL, 0);
//[-end-130401-IB05400394-modify]//
    }
    break;
//[-end-110829-IB07370065-add]//

  case KEY_SAVE_WITHOUT_EXIT:
    StringPtr=HiiGetString (HiiHandle, STRING_TOKEN (STR_SAVE_CHANGE_WITHOUT_EXIT_STRING), NULL);
    gSUBrowser->H2ODialog->ConfirmDialog (
                             0,
                             FALSE,
                             0,
                             NULL,
                             &Key,
                             StringPtr
                             );
    //[-start-131129-IB09740048-add]//
    CRScuHotKeyCallback (QuestionId, Key);
    //[-end-131129-IB09740048-add]//
    if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      mIsSaveWithoutExit = TRUE;
      if (mFullResetFlag == 1) {
        mFullResetFlag = 2;
      }
      GenericRouteConfig ((EFI_HII_CONFIG_ACCESS_PROTOCOL *) This, NULL, NULL);
    } else {
      Status = EFI_UNSUPPORTED;
    }
    gBS->FreePool (StringPtr);
    break;

  case KEY_DISCARD_CHANGE:
    //
    // discard setup change.
    //
    StringPtr=HiiGetString (HiiHandle, STRING_TOKEN (STR_DISCARD_CHANGES_STRING), NULL);
    gSUBrowser->H2ODialog->ConfirmDialog (
                             0,
                             FALSE,
                             0,
                             NULL,
                             &Key,
                             StringPtr
                             );
    //[-start-131129-IB09740048-add]//
    CRScuHotKeyCallback (QuestionId, Key);
    //[-end-131129-IB09740048-add]//
    if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      if (FeaturePcdGet (PcdH2OFormBrowserSupported)) {
        BrowserRefreshFormSet ();
      } else {
        Status = gBS->LocateProtocol (
                       &gEfiSetupUtilityBrowserProtocolGuid,
                       NULL,
                       (VOID **)&Interface
                       );
        if (EFI_ERROR (Status)) {
          return Status;
        }
        Interface->Firstin = TRUE;
      }
      if (mFullResetFlag == 1) {
        mFullResetFlag = 0;
      }
      Status = DiscardChange (This);
      UpdatePasswordState (gSUBrowser->SUCInfo->MapTable[SecurityHiiHandle].HiiHandle);
      SUCInfo->DoRefresh = TRUE;
    }

    gBS->FreePool (StringPtr);
    break;
//[-start-121129-IB06150263-add]//
  case KEY_LAN_SELECT:
    if (MyIfrNVData->Lan) {
      MmioAnd8 (FixedPcdGet32 (PcdRcbaBaseAddress) + R_PCH_RCRB_BUC, (UINT8) (~B_PCH_RCRB_BUC_LAN_DIS));
      MmioRead8 (FixedPcdGet32 (PcdRcbaBaseAddress) + R_PCH_RCRB_BUC);
    }
    break;
//[-end-121129-IB06150263-add]//
  default :
    break;
  }
//[-start-120116-IB03600466-add]//
      NVData = GetVariableAndSize (
                 L"Setup",
                 &SetupVariableGuidId,
                 &VariableSize
                 );
  if (NVData != NULL) {
     if (
//[-start-121003-IB05300339-add]//
        ( MyIfrNVData->Pfat              != NVData->Pfat              ) ||
//[-end-121003-IB05300339-add]//
        ( MyIfrNVData->TXT               != NVData->TXT               ) ||
//[-start-120730-IB06150235-modify]//
//[-start-120820-IB10820110-modify]//
        ( MyIfrNVData->EnableRapidStart  != NVData->EnableRapidStart  ) ||
//[-end-120820-IB10820110-modify]//
//[-end-120730-IB06150235-modify]//
//[-start-130206-IB03780481-add]//
        ( MyIfrNVData->TBTSecurityLevel  != NVData->TBTSecurityLevel  ) ||
//[-end-130206-IB03780481-add]//
//[-start-121127-IB03780468-modify]//
        ( MyIfrNVData->ExtremeEdition            != NVData->ExtremeEdition            ) ||
        ( MyIfrNVData->RatioLimit1Core           != NVData->RatioLimit1Core           ) ||
        ( MyIfrNVData->RatioLimit2Core           != NVData->RatioLimit2Core           ) ||
        ( MyIfrNVData->RatioLimit3Core           != NVData->RatioLimit3Core           ) ||
        ( MyIfrNVData->RatioLimit4Core           != NVData->RatioLimit4Core           ) ||
        ( MyIfrNVData->Overclocking              != NVData->Overclocking              ) ||
        ( MyIfrNVData->FlexRatioOverride         != NVData->FlexRatioOverride         ) ||
        ( MyIfrNVData->FlexRatio                 != NVData->FlexRatio                 ) ||
        ( MyIfrNVData->MemoryTcwl                != NVData->MemoryTcwl                ) ||
        ( MyIfrNVData->MemoryTcl                 != NVData->MemoryTcl                 ) ||
        ( MyIfrNVData->MemoryTrcd                != NVData->MemoryTrcd                ) ||
        ( MyIfrNVData->MemoryTrp                 != NVData->MemoryTrp                 ) ||
        ( MyIfrNVData->MemoryTras                != NVData->MemoryTras                ) ||
        ( MyIfrNVData->MemoryTwr                 != NVData->MemoryTwr                 ) ||
        ( MyIfrNVData->MemoryTrfc                != NVData->MemoryTrfc                ) ||
        ( MyIfrNVData->MemoryTrrd                != NVData->MemoryTrrd                ) ||
        ( MyIfrNVData->MemoryTwtr                != NVData->MemoryTwtr                ) ||
        ( MyIfrNVData->MemoryTrtp                != NVData->MemoryTrtp                ) ||
        ( MyIfrNVData->MemoryTrc                 != NVData->MemoryTrc                 ) ||
        ( MyIfrNVData->MemoryTfaw                != NVData->MemoryTfaw                ) ||
        ( MyIfrNVData->MemoryVoltage             != NVData->MemoryVoltage             ) ||
        ( MyIfrNVData->XmpProfileSetting         != NVData->XmpProfileSetting         ) ||
        ( MyIfrNVData->MemoryTrpab               != NVData->MemoryTrpab               ) ||
        ( MyIfrNVData->MemoryTrefi               != NVData->MemoryTrefi               ) ||
        ( MyIfrNVData->GtOverclockFreq           != NVData->GtOverclockFreq           ) ||
        ( MyIfrNVData->CoreMaxOcRatio            != NVData->CoreMaxOcRatio            ) ||
        ( MyIfrNVData->CoreVoltageMode           != NVData->CoreVoltageMode           ) ||
        ( MyIfrNVData->CoreExtraTurboVoltage     != NVData->CoreExtraTurboVoltage     ) ||
        ( MyIfrNVData->CoreVoltageOverride       != NVData->CoreVoltageOverride       ) ||
        ( MyIfrNVData->CoreVoltageOffset         != NVData->CoreVoltageOffset         ) ||
        ( MyIfrNVData->CoreVoltageOffsetPrefix   != NVData->CoreVoltageOffsetPrefix   ) ||
        ( MyIfrNVData->ClrMaxOcRatio             != NVData->ClrMaxOcRatio             ) ||
        ( MyIfrNVData->ClrVoltageMode            != NVData->ClrVoltageMode            ) ||
        ( MyIfrNVData->ClrExtraTurboVoltage      != NVData->ClrExtraTurboVoltage      ) ||
        ( MyIfrNVData->ClrVoltageOverride        != NVData->ClrVoltageOverride        ) ||
        ( MyIfrNVData->ClrVoltageOffset          != NVData->ClrVoltageOffset          ) ||
        ( MyIfrNVData->ClrVoltageOffsetPrefix    != NVData->ClrVoltageOffsetPrefix    ) ||
        ( MyIfrNVData->SvidSupport               != NVData->SvidSupport               ) ||
        ( MyIfrNVData->SvidVoltageOverride       != NVData->SvidVoltageOverride       ) ||
        ( MyIfrNVData->FivrFaults                != NVData->FivrFaults                ) ||
        ( MyIfrNVData->FivrEfficiencyManagement  != NVData->FivrEfficiencyManagement  ) ||
        ( MyIfrNVData->GtVoltageMode             != NVData->GtVoltageMode             ) ||
        ( MyIfrNVData->GtExtraTurboVoltage       != NVData->GtExtraTurboVoltage       ) ||
        ( MyIfrNVData->GtVoltageOverride         != NVData->GtVoltageOverride         ) ||
        ( MyIfrNVData->GtVoltageOffset           != NVData->GtVoltageOffset           ) ||
        ( MyIfrNVData->GtVoltageOffsetPrefix     != NVData->GtVoltageOffsetPrefix     ) ||
        ( MyIfrNVData->UncoreVoltageOffset       != NVData->UncoreVoltageOffset       ) ||
        ( MyIfrNVData->UncoreVoltageOffsetPrefix != NVData->UncoreVoltageOffsetPrefix ) ||
        ( MyIfrNVData->IoaVoltageOffset          != NVData->IoaVoltageOffset          ) ||
        ( MyIfrNVData->IoaVoltageOffsetPrefix    != NVData->IoaVoltageOffsetPrefix    ) ||
        ( MyIfrNVData->IodVoltageOffset          != NVData->IodVoltageOffset          ) ||
        ( MyIfrNVData->IodVoltageOffsetPrefix    != NVData->IodVoltageOffsetPrefix    ) ||
        ( MyIfrNVData->MemoryRefClk              != NVData->MemoryRefClk              ) ||
        ( MyIfrNVData->MemoryRatio               != NVData->MemoryRatio               ) ||
//[-end-121127-IB03780468-modify]//
        ( MyIfrNVData->PrimaryDisplay    != NVData->PrimaryDisplay    ) ||
        ( MyIfrNVData->VTSupport         != NVData->VTSupport         ) ||
        ( MyIfrNVData->DvmtPreAllocated  != NVData->DvmtPreAllocated  ) ||
        ( MyIfrNVData->GttSize           != NVData->GttSize           ) ||
        ( MyIfrNVData->IGDControl        != NVData->IGDControl        ) ||
        ( MyIfrNVData->ApertureSize      != NVData->ApertureSize      ) ||
        ( MyIfrNVData->PegGenx0          != NVData->PegGenx0          ) ||
        ( MyIfrNVData->PegGenx1          != NVData->PegGenx1          ) ||
        ( MyIfrNVData->PegGenx2          != NVData->PegGenx2          ) ||
//[-start-120712-IB06150233-remove]//
//        ( MyIfrNVData->PegGenx3          != NVData->PegGenx3          ) ||
//[-end-120712-IB06150233-remove]//
        ( MyIfrNVData->DDR3Voltage       != NVData->DDR3Voltage       ) ||
        ( MyIfrNVData->CoreActive        != NVData->CoreActive        )
        ) {
        if ( !mFullResetFlag ) {
          mFullResetFlag = 1;
        }
      } else {
         mFullResetFlag = 0;
      }
    gBS->FreePool (NVData);
  }
//[-end-120116-IB03600466-add]//
  return Status;
}

/**
 This function allows a caller to extract the current configuration for one
 or more named elements from the target driver.

 @param [in]   This             Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
 @param [in]   Request          A null-terminated Unicode string in <ConfigRequest> format.
 @param [out]  Progress         On return, points to a character in the Request string.
                                Points to the string's null terminator if request was successful.
                                Points to the most recent '&' before the first failing name/value
                                pair (or the beginning of the string if the failure is in the
                                first name/value pair) if the request was not successful.
 @param [out]  Results          A null-terminated Unicode string in <ConfigAltResp> format which
                                has all values filled in for the names in the Request string.
                                String to be allocated by the called function.

 @retval EFI_SUCCESS            The Results is filled with the requested values.
 @retval EFI_OUT_OF_RESOURCES   Not enough memory to store the results.
 @retval EFI_INVALID_PARAMETER  Request is NULL, illegal syntax, or unknown name.
 @retval EFI_NOT_FOUND          Routing data doesn't match any storage in this driver.

**/
EFI_STATUS
EFIAPI
GenericExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  EFI_STATUS                    Status;
  UINTN                         BufferSize;
  EFI_STRING                    ConfigRequestHdr;
  EFI_STRING                    ConfigRequest;
  BOOLEAN                       AllocatedRequest;
  UINTN                         Size;
  CHAR16                        *StrPointer;
  EFI_CALLBACK_INFO             *CallbackInfo;
  EFI_HANDLE                    DriverHandle;
  EFI_GUID                      VarStoreGuid = SYSTEM_CONFIGURATION_GUID;

//[-start-131202-IB09740048-add]//
  Status = CRConfigExtractCallBack (This, Request, Progress, Results);
  if (!EFI_ERROR (Status)) {
    return Status;
  }
//[-end-131202-IB09740048-add]//


  if (This == NULL || Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress        = Request;
  CallbackInfo     = EFI_CALLBACK_INFO_FROM_THIS (This);
  BufferSize       = GetVarStoreSize (CallbackInfo->HiiHandle, &CallbackInfo->FormsetGuid, &VarStoreGuid, "SystemConfig");
  ConfigRequestHdr = NULL;
  ConfigRequest    = NULL;
  AllocatedRequest = FALSE;
  Size             = 0;

  if (Request == NULL) {
    //
    // Request is set to NULL, construct full request string.
    //
    //
    // Allocate and fill a buffer large enough to hold the <ConfigHdr> template
    // followed by "&OFFSET=0&WIDTH=WWWWWWWWWWWWWWWW" followed by a Null-terminator
    //
    Status = gSUBrowser->HiiDatabase->GetPackageListHandle (gSUBrowser->HiiDatabase, CallbackInfo->HiiHandle, &DriverHandle);
    if (EFI_ERROR (Status)) {
      return EFI_NOT_FOUND;
    }
    ConfigRequestHdr = HiiConstructConfigHdr (&mFormSetGuid, mVariableName, DriverHandle);
    Size = (StrLen (ConfigRequestHdr) + 32 + 1) * sizeof (CHAR16);
    ConfigRequest = AllocateZeroPool (Size);
    ASSERT (ConfigRequest != NULL);
    AllocatedRequest = TRUE;
    UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, (UINT64) BufferSize);
    FreePool (ConfigRequestHdr);
    ConfigRequestHdr = NULL;
  } else {
    //
    // Check routing data in <ConfigHdr>.
    // Note: if only one Storage is used, then this checking could be skipped.
    //
    if (!HiiIsConfigHdrMatch (Request, &mFormSetGuid, mVariableName)) {
      return EFI_NOT_FOUND;
    }
    //
    // Set Request to the unified request string.
    //
    ConfigRequest = Request;
    //
    // Check whether Request includes Request Element.
    //
    if (StrStr (Request, L"OFFSET") == NULL) {
      //
      // Check Request Element does exist in Reques String
      //
      StrPointer = StrStr (Request, L"PATH");
      if (StrPointer == NULL) {
        return EFI_INVALID_PARAMETER;
      }
      if (StrStr (StrPointer, L"&") == NULL) {
        Size = (StrLen (Request) + 32 + 1) * sizeof (CHAR16);
        ConfigRequest = AllocateZeroPool (Size);
        ASSERT (ConfigRequest != NULL);
        AllocatedRequest = TRUE;
        UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", Request, (UINT64) BufferSize);
      }
    }
  }

  if (StrStr (ConfigRequest, L"OFFSET") == NULL) {
    //
    // If requesting Name/Value storage, return not found.
    //
    return EFI_NOT_FOUND;
  }

  //
  // Convert buffer data to <ConfigResp> by helper function BlockToConfig()
  //
  Status = gSUBrowser->HiiConfigRouting->BlockToConfig (
                                           gSUBrowser->HiiConfigRouting,
                                           ConfigRequest,
                                           (UINT8 *) gSUBrowser->SCBuffer,
                                           BufferSize,
                                           Results,
                                           Progress
                                           );
  //
  // Free the allocated config request string.
  //
  if (AllocatedRequest) {
    gBS->FreePool (ConfigRequest);
    ConfigRequest = NULL;
  }
  //
  // Set Progress string to the original request string.
  //
  if (Request == NULL) {
    *Progress = NULL;
  } else if (StrStr (Request, L"OFFSET") == NULL) {
    *Progress = Request + StrLen (Request);
  }

  return Status;
}

/**
  Set changed sensitive setup data to sensitive setup variable.

  @param [in] SetupNvData        Points to CHIPSET_CONFIGURATION instance.

  @retval EFI_SUCCESS            Set sensitive setup data successfully.
  @retval EFI_INVALID_PARAMETER  SetupNvData is NULL.
  @retval EFI_NOT_FOUND          Set sensitive setup variable failed.
**/
EFI_STATUS
SetSetupSensitiveVariable (
  IN OUT CHIPSET_CONFIGURATION        *SetupNvData
  )
{
  EFI_STATUS            Status;
  UINTN                 TokenNum;
  CHAR16                *ConfigName;
  UINT32                OffsetAndSize;
  UINTN                 Offset;
  UINTN                 DataSize;
  UINT8                 *Data;

  if (SetupNvData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status   = EFI_SUCCESS;
  TokenNum = LibPcdGetNextToken(&gSetupConfigNameGuid, 0);
  while (TokenNum != 0) {
    ConfigName = (CHAR16 *) LibPcdGetExPtr(&gSetupConfigNameGuid, TokenNum);
    OffsetAndSize = LibPcdGetEx32 (&gSetupConfigOffsetSizeGuid, TokenNum);
    Offset        = (UINTN) ((OffsetAndSize >> 16) & 0xFFFF);
    DataSize      = (UINTN) (OffsetAndSize & 0xFFFF);
    if (ConfigName != NULL && DataSize != 0) {
      Data = CommonGetVariableData (
                 ConfigName,
                 &mFormSetGuid
                 );
      if (Data == NULL || CompareMem ((UINT8 *) SetupNvData + Offset, Data, DataSize) != 0) {
        Status = SetVariableToSensitiveVariable (
                   ConfigName,
                   &mFormSetGuid,
                   EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                   DataSize,
                   (UINT8 *) SetupNvData + Offset
                   );
      }
    }
    TokenNum = LibPcdGetNextToken(&gSetupConfigNameGuid, TokenNum);
  }
  return Status;
}


/**
 This function processes the results of changes in configuration.

 @param [in]   This             Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
 @param [in]   Configuration    A null-terminated Unicode string in <ConfigResp> format.
 @param [out]  Progress         A pointer to a string filled in with the offset of the most
                                recent '&' before the first failing name/value pair (or the
                                beginning of the string if the failure is in the first
                                name/value pair) or the terminating NULL if all was successful.

 @retval EFI_SUCCESS            The Results is processed successfully.
 @retval EFI_INVALID_PARAMETER  Configuration is NULL.
 @retval EFI_NOT_FOUND          Routing data doesn't match any storage in this driver.

**/
EFI_STATUS
EFIAPI
GenericRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{
//[-start-130710-IB05160465-modify]//
  EFI_STATUS                                Status;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  VOID                                      *SetupNvData;
  UINTN                                     BufferSize;


//[-start-131202-IB09740048-add]//
  CRConfigRouteCallBack (This, Configuration, Progress);
//[-end-131202-IB09740048-add]//

  BufferSize = PcdGet32 (PcdSetupConfigSize);
  SetupVariableConfig (
    NULL,
    NULL,
    BufferSize,
    (UINT8 *) gSUBrowser->SCBuffer,
    TRUE
    );

  SUCInfo      = gSUBrowser->SUCInfo;

  BufferSize = PcdGet32 (PcdSetupConfigSize);
  SetupNvData = AllocateZeroPool (BufferSize);
  CopyMem (SetupNvData, gSUBrowser->SCBuffer, BufferSize);

//[-start-131029-IB08520089-remove]//
//  CopyMem (((CHIPSET_CONFIGURATION *)SetupNvData)->SupervisorPassword, &SUCInfo->SupervisorPassword, 2);
//  CopyMem (((CHIPSET_CONFIGURATION *)SetupNvData)->UserPassword, &SUCInfo->UserPassword, 2);
//[-end-131029-IB08520089-remove]//
  Status = gRT->SetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  SUCInfo->AdvBootDeviceNum * sizeof (UINT16),
                  (UINT16 *) SUCInfo->BootOrder
                  );


  Status = gRT->SetVariable (
                  L"BackupPlatformLang",
//[-start-120326-IB02960435-modify]//
                  &gEfiGenericVariableGuid,
//[-end-120326-IB02960435-modify]//
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  0,
                  NULL
                  );
  //
  // If Setup variable is changed, Win 8 fast boot should not be active at next boot.
  //
  gRT->SetVariable (
         L"TargetHddDevPath",
         &gEfiGenericVariableGuid,
         EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
         0,
         NULL
         );

//[-start-120703-IB07240121-add]//
  Status = gRT->SetVariable (
                  L"Timeout",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  sizeof (UINT16),
                  (UINT16 *) &((CHIPSET_CONFIGURATION *)SetupNvData)->Timeout
                  );
//[-end-120703-IB07240121-add]//

  SetSecurityStatus ();

  BufferSize = PcdGet32 (PcdSetupConfigSize);
  Status = SaveSetupConfig (
             L"Setup",
             &mFormSetGuid,
             EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
             BufferSize,
             SetupNvData
             );
  SetSetupSensitiveVariable (SetupNvData);
  PlugInVgaDisplaySelectionSave ();
//[-start-130611-IB11120016-add]//
  IpmiSetToBmc (SetupNvData);
//[-end-130611-IB11120016-add]//

  if (mFullResetFlag != 0) {
    if (!mIsSaveWithoutExit) {
      Status = FullReset (TRUE);
      ASSERT_EFI_ERROR (Status);
    }

    mIsSaveWithoutExit = FALSE;
  }

  FreePool (SetupNvData);
//[-end-130710-IB05160465-modify]//

  return  Status;
}
