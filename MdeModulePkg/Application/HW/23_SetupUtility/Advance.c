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

#include <Advance.h>
//[-start-131129-IB09740048-add]//
#include <Library/CrVfrConfigLib.h>
//[-end-131129-IB09740048-add]//
//[-start-130225-IB10920016-add]//
#ifdef HARDWARE_MONITOR_SUPPORT
#include <HardwareMonitor/HardwareMonitor.h>
#include <Protocol/IsaNonPnpDevice.h>
#endif
//[-end-130225-IB10920016-add]//

//[-start-130205-IB03780481-add]//
#include <OemThunderbolt.h>
#include <Library/TbtLib.h>
#include <IndustryStandard/Pci.h>
#include <Protocol/PciRootBridgeIo.h>
//[-end-130205-IB03780481-add]//
//[-start-130611-IB11120016-add]//
#include <Library/DxeIpmiSetupUtilityLib.h>
//[-end-130611-IB11120016-add]//

IDE_CONFIG                               *mIdeConfig;
EFI_CALLBACK_INFO                         *mAdvCallBackInfo;

/**
 This is the callback function for the Advance Menu.

 @param [in]   This
 @param [in]   Action
 @param [in]   QuestionId
 @param [in]   Type
 @param [in]   Value
 @param [out]  ActionRequest


**/
EFI_STATUS
EFIAPI
AdvanceCallbackRoutine (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_STATUS                            Status;
  BOOLEAN                               PState;
  CHAR16                                *StringPtr;
  CHIPSET_CONFIGURATION                  *MyIfrNVData;
  EFI_HII_HANDLE                        HiiHandle;
  EFI_CALLBACK_INFO                     *CallbackInfo;
  EFI_INPUT_KEY                         Key;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  UINT8                                 DeviceKind;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL    *Interface;
  UINTN                                 BufferSize;
//[-start-130205-IB03780481-add]//
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL       *PciRootBridgeIo;
  UINT64                                PciAddress;
  UINT8                                 PciData8;
  UINT32                                Tbt2PcieData;
//[-end-130205-IB03780481-add]//
//[-start-130617-IB04560405-add]// 
    EVENT_LOG_VIEWER_PROTOCOL             *EventLogViewer; 
//[-end-130617-IB04560405-add]//
//[-start-131002-IB08520088-add]//
  EFI_GUID                              PasswordConfigGuid = PASSWORD_CONFIGURATION_GUID;
  UINTN                                 PasswordConfigBufferSize;
//[-end-131002-IB08520088-add]//
  EFI_GUID                              VarStoreGuid = SYSTEM_CONFIGURATION_GUID;

  if (!FeaturePcdGet (PcdH2OFormBrowserSupported) && Action == EFI_BROWSER_ACTION_CHANGING) {
    //
    // For compatible to old form browser which only use EFI_BROWSER_ACTION_CHANGING action,
    // change action to EFI_BROWSER_ACTION_CHANGED to make it workable.
    //
    Action = EFI_BROWSER_ACTION_CHANGED;
  }

  if (Action != EFI_BROWSER_ACTION_CHANGED) {
    return AdvanceCallbackRoutineByAction (This, Action, QuestionId, Type, Value, ActionRequest);
  }

  *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  CallbackInfo = EFI_CALLBACK_INFO_FROM_THIS (This);
//[-start-131002-IB08520088-add]//
  PasswordConfigBufferSize = sizeof (PASSWORD_CONFIGURATION);
//[-end-131002-IB08520088-add]//
  Interface = NULL;
//[-start-130715-IB05160466-modify]//
  BufferSize = GetVarStoreSize (CallbackInfo->HiiHandle, &CallbackInfo->FormsetGuid, &VarStoreGuid, "SystemConfig");
  if (!FeaturePcdGet (PcdH2OFormBrowserSupported)) {
    if (QuestionId == GET_SETUP_CONFIG || QuestionId == SET_SETUP_CONFIG) {
      Status = SetupVariableConfig (
                 NULL,
                 NULL,
                 BufferSize,
                 (UINT8 *) gSUBrowser->SCBuffer,
                 (BOOLEAN)(QuestionId == GET_SETUP_CONFIG)
                 );
  //[-start-131002-IB08520088-add]//
      Status = SetupVariableConfig (
                 &PasswordConfigGuid,
                 L"PasswordConfig",
                 PasswordConfigBufferSize,
                 (UINT8 *) &mPasswordConfig,
                 (BOOLEAN) (QuestionId == GET_SETUP_CONFIG)
                 );
  //[-end-131002-IB08520088-add]//
      return Status;
    }
    Status = gBS->LocateProtocol (
                   &gEfiSetupUtilityBrowserProtocolGuid,
                   NULL,
                   (VOID **)&Interface
                   );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  Status = SetupVariableConfig (
             NULL,
             NULL,
             BufferSize,
             (UINT8 *) gSUBrowser->SCBuffer,
             TRUE
             );
  Status = SetupVariableConfig (
             &PasswordConfigGuid,
             L"PasswordConfig",
             PasswordConfigBufferSize,
             (UINT8 *) &mPasswordConfig,
             TRUE
             );
//[-end-130715-IB05160466-modify]//
  MyIfrNVData = (CHIPSET_CONFIGURATION *)gSUBrowser->SCBuffer;
  Status    = EFI_SUCCESS;
  StringPtr = NULL;
  SUCInfo   = gSUBrowser->SUCInfo;

  HiiHandle = CallbackInfo->HiiHandle;

  switch (QuestionId) {

  // my homework
  case KEY_TEST_CALLBACK_SET:
    if (MyIfrNVData->SuppressBtn == TRUE || MyIfrNVData->GrayOutBtn == TRUE) {
      MyIfrNVData->SuppressBtn = FALSE;
      MyIfrNVData->GrayOutBtn = FALSE;
    }
    break;

  case KEY_AHCI_OPROM_CONFIG:
    if (MyIfrNVData->SataCnfigure == AHCI_MODE || MyIfrNVData->SataCnfigure == RAID_MODE) {
      MyIfrNVData->AhciOptionRomSupport = TRUE;
    }else {
      MyIfrNVData->AhciOptionRomSupport = FALSE;
    }
    break;
  case KEY_SERIAL_PORTA:
  case KEY_SERIAL_PORTA_BASE_IO:
  case KEY_SERIAL_PORTA_INTERRUPT:
  case KEY_SERIAL_PORTB:
  case KEY_SERIAL_PORTB_BASE_IO:
  case KEY_SERIAL_PORTB_INTERRUPT:
    //
    //  Check IRQ conflict between serial port and pci solt.
    //
    if (CheckSioAndPciSoltConflict (MyIfrNVData, (UINT8 *)&(MyIfrNVData->PciSlot3), &DeviceKind)) {
        StringPtr=HiiGetString (HiiHandle, STRING_TOKEN(ISA_AND_PCISOLT_CONFLICT_STRING), NULL);
      gSUBrowser->H2ODialog->ConfirmDialog (2, FALSE, 0, NULL, &Key, StringPtr);
      gBS->FreePool (StringPtr);
      switch (DeviceKind) {

      case 0:
        MyIfrNVData->ComPortA = 0;
        break;

      case 1:
        MyIfrNVData->ComPortB = 0;
        break;
      }
    }
    if (CheckSioConflict (MyIfrNVData)) {
      StringPtr=HiiGetString (HiiHandle, STRING_TOKEN(CONFLICT_STRING), NULL);
      gSUBrowser->H2ODialog->ConfirmDialog (
                               2,
                               FALSE,
                               0,
                               NULL,
                               &Key,
                               StringPtr
                               );
      gBS->FreePool (StringPtr);
    }
    break;
  case KEY_PCI_SLOT3_IRQ_SET:
  case KEY_PCI_SLOT4_IRQ_SET:
    if (CheckPciSioConflict (MyIfrNVData, (UINT8 *)&(MyIfrNVData->PciSlot3), &DeviceKind)) {
      switch (DeviceKind) {

      case 1:
        StringPtr=HiiGetString (HiiHandle, STRING_TOKEN(COMPORTA_CONFLICT_STRING), NULL);
        break;

      case 2:
        StringPtr=HiiGetString (HiiHandle, STRING_TOKEN(COMPORTB_CONFLICT_STRING), NULL);
        break;

      }
      gSUBrowser->H2ODialog->ConfirmDialog (2, FALSE, 0, NULL, &Key, StringPtr);
      gBS->FreePool (StringPtr);
    }
    break;

  case KEY_CHIPSET_EXTENDED_CONFIG:
    if (MyIfrNVData->ExtendedConfig == 0) {
      MyIfrNVData->SdRamFrequency    = 0;
      MyIfrNVData->SdRamTimeCtrl     = 0;
      MyIfrNVData->SdRamRasActToPre  = 6;
      MyIfrNVData->SdRamCasLatency   = 25;
      MyIfrNVData->SdRamRasCasDelay  = 3;
      MyIfrNVData->SdRamRasPrecharge = 3;
    }
    break;

  case KEY_CHIPSET_SDRAM_TIME_CTRL:
    switch (MyIfrNVData->SdRamTimeCtrl) {

    case 0:         // AUTO
      MyIfrNVData->SdRamRasActToPre = 6;
      MyIfrNVData->SdRamCasLatency = 25;
      MyIfrNVData->SdRamRasCasDelay = 3;
      MyIfrNVData->SdRamRasPrecharge = 3;
      break;

    case 1:         // MANUAL_AGRESSIVE
      MyIfrNVData->SdRamRasActToPre  = 5;
      MyIfrNVData->SdRamCasLatency   = 20;
      MyIfrNVData->SdRamRasCasDelay  = 2;
      MyIfrNVData->SdRamRasPrecharge = 2;
      break;
    }
    SUCInfo->DoRefresh = TRUE; 
    break;

    case KEY_SET_HDD_PSW_NUM_0:
    case KEY_SET_HDD_PSW_NUM_1:
    case KEY_SET_HDD_PSW_NUM_2:
    case KEY_SET_HDD_PSW_NUM_3:
    case KEY_SET_HDD_PSW_NUM_4:
    case KEY_SET_HDD_PSW_NUM_5:
    case KEY_SET_HDD_PSW_NUM_6:
    case KEY_SET_HDD_PSW_NUM_7:
    case KEY_SET_HDD_PSW_NUM_8:
    case KEY_SET_HDD_PSW_NUM_9:
    case KEY_SET_HDD_PSW_NUM_A:
    case KEY_SET_HDD_PSW_NUM_B:
    case KEY_SET_HDD_PSW_NUM_C:
    case KEY_SET_HDD_PSW_NUM_D:
    case KEY_SET_HDD_PSW_NUM_E:
    case KEY_SET_HDD_PSW_NUM_F:
    case KEY_CHECK_HDD_PSW_NUM_0:
    case KEY_CHECK_HDD_PSW_NUM_1:
    case KEY_CHECK_HDD_PSW_NUM_2:
    case KEY_CHECK_HDD_PSW_NUM_3:
    case KEY_CHECK_HDD_PSW_NUM_4:
    case KEY_CHECK_HDD_PSW_NUM_5:
    case KEY_CHECK_HDD_PSW_NUM_6:
    case KEY_CHECK_HDD_PSW_NUM_7:
    case KEY_CHECK_HDD_PSW_NUM_8:
    case KEY_CHECK_HDD_PSW_NUM_9:
    case KEY_CHECK_HDD_PSW_NUM_A:
    case KEY_CHECK_HDD_PSW_NUM_B:
    case KEY_CHECK_HDD_PSW_NUM_C:
    case KEY_CHECK_HDD_PSW_NUM_D:
    case KEY_CHECK_HDD_PSW_NUM_E:
    case KEY_CHECK_HDD_PSW_NUM_F:
    case KEY_SET_HDD_PSW_CBM_NUM_0:
    case KEY_SET_HDD_PSW_CBM_NUM_1:
    case KEY_SET_HDD_PSW_CBM_NUM_2:
    case KEY_SET_HDD_PSW_CBM_NUM_3:
    case KEY_SET_HDD_PSW_CBM_NUM_4:
    case KEY_SET_HDD_PSW_CBM_NUM_5:
    case KEY_SET_HDD_PSW_CBM_NUM_6:
    case KEY_SET_HDD_PSW_CBM_NUM_7:
    case KEY_SET_HDD_PSW_CBM_NUM_8:
    case KEY_SET_HDD_PSW_CBM_NUM_9:
    case KEY_SET_HDD_PSW_CBM_NUM_A:
    case KEY_SET_HDD_PSW_CBM_NUM_B:
    case KEY_SET_HDD_PSW_CBM_NUM_C:
    case KEY_SET_HDD_PSW_CBM_NUM_D:
    case KEY_SET_HDD_PSW_CBM_NUM_E:
    case KEY_SET_HDD_PSW_CBM_NUM_F:
    case KEY_CHECK_HDD_PSW_CBM_NUM_0:
    case KEY_CHECK_HDD_PSW_CBM_NUM_1:
    case KEY_CHECK_HDD_PSW_CBM_NUM_2:
    case KEY_CHECK_HDD_PSW_CBM_NUM_3:
    case KEY_CHECK_HDD_PSW_CBM_NUM_4:
    case KEY_CHECK_HDD_PSW_CBM_NUM_5:
    case KEY_CHECK_HDD_PSW_CBM_NUM_6:
    case KEY_CHECK_HDD_PSW_CBM_NUM_7:
    case KEY_CHECK_HDD_PSW_CBM_NUM_8:
    case KEY_CHECK_HDD_PSW_CBM_NUM_9:
    case KEY_CHECK_HDD_PSW_CBM_NUM_A:
    case KEY_CHECK_HDD_PSW_CBM_NUM_B:
    case KEY_CHECK_HDD_PSW_CBM_NUM_C:
    case KEY_CHECK_HDD_PSW_CBM_NUM_D:
    case KEY_CHECK_HDD_PSW_CBM_NUM_E:
    case KEY_CHECK_HDD_PSW_CBM_NUM_F:
      Status = HddPasswordCallback(
                 This,
                 HiiHandle,
                 Type,
                 Value,
                 ActionRequest,
                 &PState,
                 (QuestionId & 0x0f),
                 SUCInfo->PswdConsoleService,
                 SUCInfo->HarddiskPassword,
                 USER_PSW,
                 SUCInfo->NumOfHarddiskPswd
                 );
      break;
    case KEY_HDD_MASTER_PSW_KEY_NUM_0:
    case KEY_HDD_MASTER_PSW_KEY_NUM_1:
    case KEY_HDD_MASTER_PSW_KEY_NUM_2:
    case KEY_HDD_MASTER_PSW_KEY_NUM_3:
    case KEY_HDD_MASTER_PSW_KEY_NUM_4:
    case KEY_HDD_MASTER_PSW_KEY_NUM_5:
    case KEY_HDD_MASTER_PSW_KEY_NUM_6:
    case KEY_HDD_MASTER_PSW_KEY_NUM_7:
    case KEY_HDD_MASTER_PSW_KEY_NUM_8:
    case KEY_HDD_MASTER_PSW_KEY_NUM_9:
    case KEY_HDD_MASTER_PSW_KEY_NUM_A:
    case KEY_HDD_MASTER_PSW_KEY_NUM_B:
    case KEY_HDD_MASTER_PSW_KEY_NUM_C:
    case KEY_HDD_MASTER_PSW_KEY_NUM_D:
    case KEY_HDD_MASTER_PSW_KEY_NUM_E:
    case KEY_HDD_MASTER_PSW_KEY_NUM_F:
    case KEY_HDD_MASTER_PSW_KEY_CBM_NUM_0:
    case KEY_HDD_MASTER_PSW_KEY_CBM_NUM_1:
    case KEY_HDD_MASTER_PSW_KEY_CBM_NUM_2:
    case KEY_HDD_MASTER_PSW_KEY_CBM_NUM_3:
    case KEY_HDD_MASTER_PSW_KEY_CBM_NUM_4:
    case KEY_HDD_MASTER_PSW_KEY_CBM_NUM_5:
    case KEY_HDD_MASTER_PSW_KEY_CBM_NUM_6:
    case KEY_HDD_MASTER_PSW_KEY_CBM_NUM_7:
    case KEY_HDD_MASTER_PSW_KEY_CBM_NUM_8:
    case KEY_HDD_MASTER_PSW_KEY_CBM_NUM_9:
    case KEY_HDD_MASTER_PSW_KEY_CBM_NUM_A:
    case KEY_HDD_MASTER_PSW_KEY_CBM_NUM_B:
    case KEY_HDD_MASTER_PSW_KEY_CBM_NUM_C:
    case KEY_HDD_MASTER_PSW_KEY_CBM_NUM_D:
    case KEY_HDD_MASTER_PSW_KEY_CBM_NUM_E:
    case KEY_HDD_MASTER_PSW_KEY_CBM_NUM_F:
      Status = HddPasswordCallback(
                 This,
                 HiiHandle,
                 Type,
                 Value,
                 ActionRequest,
                 &PState,
                 (QuestionId & 0x0f),
                 SUCInfo->PswdConsoleService,
                 SUCInfo->HarddiskPassword,
                 MASTER_PSW,
                 SUCInfo->NumOfHarddiskPswd
                 );
      break;
  case KEY_SATA_CNFIGURE_MODE :
    UpdateHDCConfigure (
      HiiHandle,
      MyIfrNVData
      );

    break;

  case KEY_ICC_CLOCK_1 :
  case KEY_ICC_CLOCK_2 :
  case KEY_ICC_CLOCK_3 :
  case KEY_ICC_CLOCK_4 :
  case KEY_ICC_CLOCK_5 :
  case KEY_ICC_CLOCK_6 :
    if (FeaturePcdGet (PcdMeSupported)) {
      ResetIccClockForm (HiiHandle, MyIfrNVData, QuestionId, FALSE);
    }
    break;
  case KEY_ICC_APPLY :
    if (FeaturePcdGet (PcdMeSupported)) {
      IccSettingsApply (HiiHandle, MyIfrNVData);
    }
    break;
//[-start-121129-IB03780468-remove]//
//  case KEY_XTU_TURN_ON :
//    //
//    // When XTU from Disable to Enable, init specific options to default value
//    //
//    if (FeaturePcdGet(PcdXtuSupported)) {
//
//      //
//      // Get XTU info hob
//      //
//      HobList = GetHobList ();
//
//      XTUInfoHobData= GetNextGuidHob (&gXTUInfoHobGuid, HobList);
//
//      // Non-Turbo Ratio
//      MyIfrNVData->FlexRatio                = XTUInfoHobData->CpuNonTurboRatio.MaxRatio;
//      // Ratio Limit Core 1~4
//      MyIfrNVData->RatioLimit1Core          = XTUInfoHobData->TurboRatioLimitDefault.LimitCore1;
//      MyIfrNVData->RatioLimit2Core          = XTUInfoHobData->TurboRatioLimitDefault.LimitCore2;
//      MyIfrNVData->RatioLimit3Core          = XTUInfoHobData->TurboRatioLimitDefault.LimitCore3;
//      MyIfrNVData->RatioLimit4Core          = XTUInfoHobData->TurboRatioLimitDefault.LimitCore4;
//      // Turbo Power Limit 1&2
//      MyIfrNVData->LongDurationPowerLimit   = XTUInfoHobData->TruboTdpLimitDefault.PowerLimit1Default;
//      MyIfrNVData->ShortDurationPowerLimit  = XTUInfoHobData->TruboTdpLimitDefault.PowerLimit2Default;
//    }
//    break;
//[-end-121129-IB03780468-remove]//

  case KEY_DPTF:
//[-start-120403-IB05300308-modify]//
    if (MyIfrNVData->EnableDptf) {
      MyIfrNVData->Device4Enable = 1;
      MyIfrNVData->ThermalDevice = 1;
    } else {
      MyIfrNVData->Device4Enable = 0;
      MyIfrNVData->ThermalDevice = 0;
    }
//[-end-120403-IB05300308-modify]//
    break;

  case KEY_IFR_UPDATE:
    if (FeaturePcdGet (PcdMeSupported)) {
      Status = HeciSetIfrUpdate (MyIfrNVData->IFRUpdate);
    }
    break;

//[-start-130131-IB03780481-add]//
  case KEY_TBT_SECURITY_OPTION:
    if (FeaturePcdGet (PcdThunderBoltSupported)) {
      Status = gBS->LocateProtocol (&gEfiPciRootBridgeIoProtocolGuid, NULL, (VOID **)&PciRootBridgeIo);
      ASSERT_EFI_ERROR (Status);

      //
      // Detect the bus number of TBT host bridge.
      //
//[-start-130709-IB05400426-modify]//
    PciAddress = EFI_PCI_ADDRESS (THUNDERBOLT_PCIE_ROOT_PORT_BUS_NUM, THUNDERBOLT_PCIE_ROOT_PORT_DEV_NUM, (MyIfrNVData->TbtDevice - 1), PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
//[-end-130709-IB05400426-modify]//
      PciData8 = 0;
      Status = PciRootBridgeIo->Pci.Read (
                                  PciRootBridgeIo,
                                  EfiPciWidthUint8,
                                  PciAddress,
                                  1,
                                  &PciData8
                                  );
      if (!EFI_ERROR (Status) && PciData8 != 0) {
        //
        // Drive GPIO_3__FORCE_PWR
        //
        TbtSetGpio3 (TRUE);

        //
        // Update Security Level.
        //
        TbtSetPCIe2TBTCommand (SET_SECURITY_LEVEL, MyIfrNVData->TBTSecurityLevel, PciData8, &Tbt2PcieData);
        TbtSetGpio3 (FALSE);
      }
    }
    break;
    
  case KEY_TBT_SX_OPTION:
    if (FeaturePcdGet (PcdThunderBoltSupported)) {
      if (MyIfrNVData->TbtWakeFromDevice) {
        MyIfrNVData->WakeOnPME = 1;
      }
    }
    break;
//[-end-130131-IB03780481-add]//
    
//[-start-110719-IB07370064-add]//
  case KEY_DUAL_VGA_SUPPORT:
    if (MyIfrNVData->UefiDualVgaControllers == DUAL_VGA_CONTROLLER_ENABLE) {
      //
      // Set PrimaryDisplay Mode IGD, and enable the IGD.
      // Always enable PEG detection.
      //
      MyIfrNVData->PrimaryDisplay = 0;
      MyIfrNVData->IGDControl = 1;
      MyIfrNVData->PEGWorkAround = 1;
    }

    if (MyIfrNVData->UefiDualVgaControllers == DUAL_VGA_CONTROLLER_DISABLE) {
      //
      // Set PrimaryDisplay Mode AUTO, and set IGD enable in AUTO mode.
      // disable PEG detection.
      //
      MyIfrNVData->PrimaryDisplay = 3;
      MyIfrNVData->IGDControl = 2;
      MyIfrNVData->PEGWorkAround = 0;
    }
  break;
//[-end-110719-IB07370064-add]//
//[-start-110824-IB07370065-add]//
    case KEY_PLUG_IN_DISPLAY_SELECTION1:
    case KEY_PLUG_IN_DISPLAY_SELECTION2:
      PlugInVideoDisplaySelectionOption (QuestionId, Value);
      break;

    case KEY_IGD_PRIMARY_DISPLAY_SELECTION:
      if (Value->u8 == SCU_IGD_BOOT_TYPE_VBIOS_DEFAULT) {
        MyIfrNVData->IGDBootTypeSecondary = SCU_IGD_BOOT_TYPE_DISABLE;
      }
//      MyIfrNVData->IGDBootType = Value->u8;
//      IgdVideoDisplaySelectionOption(HiiHandle, MyIfrNVData, QuestionId, MyIfrNVData->IGDBootType);
    break;
//[-end-110824-IB07370065-add]//

//[-start-130611-IB11120016-add]//
  case KEY_SET_BMC_IPV4_IP_ADDRESS:
  case KEY_SET_BMC_IPV4_SUBNET_MASK:
  case KEY_SET_BMC_IPV4_GATEWAY_ADDRESS:
  case KEY_IPMI_SDR_LIST_SUPPORT:
  case KEY_SET_BMC_IPV6_IP_ADDRESS:
  case KEY_SET_BMC_IPV6_GATEWAY_ADDRESS:
  case KEY_SET_BMC_LAN_CHANNEL_NUM:
    IpmiConfigCallback (HiiHandle, QuestionId, MyIfrNVData, gSUBrowser);
    break;

  case KEY_EXECUTE_IPMI_UTILITY:
    ExecuteIpmiUtility ();
    break;
//[-end-130611-IB11120016-add]//

//[-start-131129-IB09740048-add]//
  case CONSOLE_REDIRECTION_FORM_ID_0 :
  case CONSOLE_REDIRECTION_FORM_ID_1 :
  case CONSOLE_REDIRECTION_FORM_ID_2 :
  case CONSOLE_REDIRECTION_FORM_ID_3 :
  case CONSOLE_REDIRECTION_FORM_ID_4 :
  case CONSOLE_REDIRECTION_FORM_ID_5 :
  case CONSOLE_REDIRECTION_FORM_ID_6 :
  case CONSOLE_REDIRECTION_FORM_ID_7 :
  case KEY_CONSOLE_REDIRECTION_1 : 
  case KEY_CONSOLE_REDIRECTION_2 :
  case KEY_CONSOLE_REDIRECTION_3 :    
  case KEY_CONSOLE_REDIRECTION_4 :
  case KEY_CONSOLE_REDIRECTION_5 :
  case KEY_CONSOLE_REDIRECTION_6 :
  case KEY_CONSOLE_REDIRECTION_7 :
  case KEY_CONSOLE_REDIRECTION_8 :
  case KEY_CONSOLE_REDIRECTION_9 :
  case KEY_CONSOLE_REDIRECTION_10 :  
  case KEY_CONSOLE_REDIRECTION_11 :
  case KEY_CONSOLE_REDIRECTION_12 : 
  case KEY_CONSOLE_REDIRECTION_13 : 
  case KEY_CONSOLE_REDIRECTION_14 :
  case KEY_CONSOLE_REDIRECTION_15 :
  case KEY_CONSOLE_REDIRECTION_16 :

    CRScuAdvanceCallback(QuestionId);
    
    break;
//[-end-131129-IB09740048-add]//

//[-start-130617-IB04560405-add]//
//[-start-140318-IB08400253-modify]//
  case KEY_EVENT_LOG_VIEWER:
    if (FeaturePcdGet(PcdH2OEventLogSupport)) {
      Status = gBS->LocateProtocol(
                      &gH2OEventLogViewerProtocolGuid,
                      NULL,
                      (VOID **)&EventLogViewer
                      );
      if (!EFI_ERROR (Status)) {
        EventLogViewer->ExecuteEventLogViewer (EventLogViewer);
        if (FeaturePcdGet (PcdH2OFormBrowserSupported)) {
          BrowserRefreshFormSet ();
        } else {
          Interface->Firstin = TRUE;
        }
      }
      break;
    }
//[-end-140318-IB08400253-modify]//
//[-end-130617-IB04560405-add]//

  default:

//[-start-130225-IB10920016-add]//
#ifdef HARDWARE_MONITOR_SUPPORT
    UpdateHardwareMonitorVoltage (HiiHandle, QuestionId);
    UpdateHardwareMonitorTemperature (HiiHandle, QuestionId);
    UpdateHardwareMonitorFanSpeed (HiiHandle, QuestionId);
#endif    
//[-end-130225-IB10920016-add]//
  
    Status = HotKeyCallBack (
               This,
               Action,
               QuestionId,
               Type,
               Value,
               ActionRequest
               );
    break;
  }
//[-start-130715-IB05160466-remove]//
////[-start-130709-IB05160465-modify]//
//  BufferSize = GetSetupConfigSize ();
////[-end-130709-IB05160465-modify]//
//[-end-130715-IB05160466-remove]//
  SetupVariableConfig (
    NULL,
    NULL,
    BufferSize,
    (UINT8 *) gSUBrowser->SCBuffer,
    FALSE
    );
//[-start-131002-IB08520088-add]//
  SetupVariableConfig (
    &PasswordConfigGuid,
    L"PasswordConfig",
    PasswordConfigBufferSize,
    (UINT8 *) &mPasswordConfig,
    FALSE
    );
//[-end-131002-IB08520088-add]//
  return Status;
}

EFI_STATUS
AdvanceCallbackRoutineByAction (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_STATUS                            Status;
  EFI_CALLBACK_INFO                     *CallbackInfo;
  UINTN                                 BufferSize;
  EFI_GUID                              PasswordConfigGuid = PASSWORD_CONFIGURATION_GUID;
  UINTN                                 PasswordConfigBufferSize;
  EFI_GUID                              VarStoreGuid = SYSTEM_CONFIGURATION_GUID;

  if ((This == NULL) ||
      ((Value == NULL) &&
       (Action != EFI_BROWSER_ACTION_FORM_OPEN) &&
       (Action != EFI_BROWSER_ACTION_FORM_CLOSE))||
      (ActionRequest == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  CallbackInfo   = EFI_CALLBACK_INFO_FROM_THIS (This);
  BufferSize     = GetVarStoreSize (CallbackInfo->HiiHandle, &CallbackInfo->FormsetGuid, &VarStoreGuid, "SystemConfig");
  PasswordConfigBufferSize = sizeof (PASSWORD_CONFIGURATION);
  Status         = EFI_UNSUPPORTED;

  switch (Action) {

  case EFI_BROWSER_ACTION_FORM_OPEN:
    if (QuestionId == 0) {
      Status = SetupVariableConfig (
                 NULL,
                 NULL,
                 BufferSize,
                 (UINT8 *) gSUBrowser->SCBuffer,
                 FALSE
                 );
      Status = SetupVariableConfig (
                 &PasswordConfigGuid,
                 L"PasswordConfig",
                 PasswordConfigBufferSize,
                 (UINT8 *) &mPasswordConfig,
                 FALSE
                 );

    }
    break;

  case EFI_BROWSER_ACTION_FORM_CLOSE:
    if (QuestionId == 0) {
      Status = SetupVariableConfig (
                 NULL,
                 NULL,
                 BufferSize,
                 (UINT8 *) gSUBrowser->SCBuffer,
                 TRUE
                 );
      Status = SetupVariableConfig (
                 &PasswordConfigGuid,
                 L"PasswordConfig",
                 PasswordConfigBufferSize,
                 (UINT8 *) &mPasswordConfig,
                 TRUE
                 );
    }
    break;

  case EFI_BROWSER_ACTION_CHANGING:
    Status = EFI_SUCCESS;
    break;

  case EFI_BROWSER_ACTION_DEFAULT_MANUFACTURING:
    if (QuestionId == KEY_SCAN_F9) {
      Status = HotKeyCallBack (
                This,
                Action,
                QuestionId,
                Type,
                Value,
                ActionRequest
                );
      Status = SetupVariableConfig (
                  NULL,
                  NULL,
                  BufferSize,
                  (UINT8 *) gSUBrowser->SCBuffer,
                  FALSE
                  );
      Status = SetupVariableConfig (
                 &PasswordConfigGuid,
                 L"PasswordConfig",
                 PasswordConfigBufferSize,
                 (UINT8 *) &mPasswordConfig,
                 FALSE
                 );
    }
    //
    // avoid GetQuestionDefault execute ExtractConfig
    //
    return EFI_SUCCESS;

  default:
    break;
  }

  return Status;
}

EFI_STATUS
InstallAdvanceCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle,
  IN EFI_HII_HANDLE                         HiiHandle
  )
{
  EFI_STATUS                                Status;
  EFI_GUID                                  FormsetGuid = FORMSET_ID_GUID_ADVANCE;
  EFI_GUID                                  OldFormsetGuid = SYSTEM_CONFIGURATION_GUID;

  mAdvCallBackInfo = AllocatePool (sizeof(EFI_CALLBACK_INFO));
  if (mAdvCallBackInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  mAdvCallBackInfo->Signature                    = EFI_CALLBACK_INFO_SIGNATURE;
  mAdvCallBackInfo->DriverCallback.ExtractConfig = gSUBrowser->ExtractConfig;
  mAdvCallBackInfo->DriverCallback.RouteConfig   = gSUBrowser->RouteConfig;
  mAdvCallBackInfo->DriverCallback.Callback      = AdvanceCallbackRoutine;
  mAdvCallBackInfo->HiiHandle                    = HiiHandle;
  if (FeaturePcdGet (PcdH2OFormBrowserSupported)) {
    CopyGuid (&mAdvCallBackInfo->FormsetGuid, &FormsetGuid);
  } else {
    CopyGuid (&mAdvCallBackInfo->FormsetGuid, &OldFormsetGuid);
  }

  //
  // Install protocol interface
  //
  Status = gBS->InstallProtocolInterface (
                  &DriverHandle,
                  &gEfiHiiConfigAccessProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mAdvCallBackInfo->DriverCallback
                  );
  ASSERT_EFI_ERROR (Status);
  Status = InitAdvanceMenu (HiiHandle);
  return Status;
}

EFI_STATUS
EFIAPI
UninstallAdvanceCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle
  )
{
  EFI_STATUS     Status;

  if (mAdvCallBackInfo == NULL) {
    return EFI_SUCCESS;
  }
  Status = gBS->UninstallProtocolInterface (
                  DriverHandle,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &mAdvCallBackInfo->DriverCallback
                  );
  ASSERT_EFI_ERROR (Status);
  gBS->FreePool (mAdvCallBackInfo);
  mAdvCallBackInfo = NULL;
  return Status;
}

//[-start-130611-IB11120016-add]//
/**
 Collect IPMI Config related string and call OemIpmiConfigUpdate to update IPMI config menu.

 @param[in]         HiiHandle           EFI_HII_HANDLE.

*/
VOID
IpmiItemInit (
  IN  EFI_HII_HANDLE                    HiiHandle
  )
{
  IPMI_STR_ID                           StrIdBuffer;

  StrIdBuffer.CurrentBmcIpv6IpAddress      = STR_CURRENT_BMC_IPV6_IP_ADDRESS;
  StrIdBuffer.CurrentBmcIpv6GatewayAddress = STR_CURRENT_BMC_IPV6_GATEWAY_ADDRESS;
  StrIdBuffer.CurrentBmcIpv4IpAddress      = STR_CURRENT_BMC_IPV4_IP_ADDRESS;
  StrIdBuffer.CurrentBmcIpv4SubnetMask     = STR_CURRENT_BMC_IPV4_SUBNET_MASK;
  StrIdBuffer.CurrentBmcIpv4GatewayAddress = STR_CURRENT_BMC_IPV4_GATEWAY_ADDRESS;
  StrIdBuffer.BmcLanChannelHelp            = STR_BMC_LAN_CHANNEL_HELP;
  StrIdBuffer.CurrentBmcFirmwareVersion    = STR_CURRENT_BMC_FIRMWARE_VERSION;
  StrIdBuffer.CurrentBmcMacAddress         = STR_CURRENT_BMC_MAC_ADDRESS;
  StrIdBuffer.CurrentBmcStatus             = STR_CURRENT_BMC_STATUS;
  StrIdBuffer.CurrentIpmiInterfaceType     = STR_CURRENT_IPMI_INTERFACE_TYPE;
  StrIdBuffer.CurrentIpmiOsBaseAddress     = STR_CURRENT_IPMI_OS_BASE_ADDRESS;
  StrIdBuffer.CurrentIpmiPostBaseAddress   = STR_CURRENT_IPMI_POST_BASE_ADDRESS;
  StrIdBuffer.CurrentIpmiSmmBaseAddress    = STR_CURRENT_IPMI_SMM_BASE_ADDRESS;
  StrIdBuffer.CurrentIpmiSpecVersion       = STR_CURRENT_IPMI_SPEC_VERSION;
  StrIdBuffer.IpmiConfigFormTitle          = STR_IPMI_CONFIG_FORM_TITLE;

  IpmiConfigUpdate (HiiHandle, gSUBrowser->SCBuffer, &StrIdBuffer);
}
//[-end-130611-IB11120016-add]//

EFI_STATUS
InitAdvanceMenu (
  IN EFI_HII_HANDLE                         HiiHandle
)
{
  EFI_STATUS                                Status;

  //
  // Check and update IDE configuration.
  //
  gSUBrowser->SUCInfo->PrevSataCnfigure = ((CHIPSET_CONFIGURATION *)gSUBrowser->SCBuffer)->SataCnfigure;

  mIdeConfig = AllocateZeroPool (sizeof(IDE_CONFIG)*8);
  InitIdeConfig (mIdeConfig);
  gSUBrowser->IdeConfig = mIdeConfig;
  Status = CheckIde (
             HiiHandle,
             (CHIPSET_CONFIGURATION *)gSUBrowser->SCBuffer,
             FALSE
             );
  UpdateGbePeiEPortSelect (HiiHandle);
//[-start-121201-IB03780468-modify]//
  //
  // XTU CPU Non Turbo Ratio
  //
  Status = CreateXtuOption (HiiHandle);
//[-end-121201-IB03780468-modify]//


//[-start-120511-IB06460397-add]//
  if (FeaturePcdGet (PcdMeSupported)) {
    IccOverClockingInit (HiiHandle);
  }
//[-end-120511-IB06460397-add]//

//[-start-120816-IB05330371-add]//
  UpdateACPIDebugInfo(HiiHandle);
//[-end-120816-IB05330371-add]//

//[-start-120315-IB06460374-add]//
  if (FeaturePcdGet (PcdMeSupported)) {
    IccProfileItemInit (HiiHandle);
  }
//[-end-120315-IB06460374-add]//
//[-start-110825-IB07370065-add]//
  PlugInVideoDisplaySelectionLabel (HiiHandle);
  IgdInitialVideoDisplaySelectionLabel (HiiHandle);
  InitialDualVgaControllersLabel (HiiHandle);
//[-end-110825-IB07370065-add]//
//[-start-130225-IB10920016-add]//
#ifdef HARDWARE_MONITOR_SUPPORT
  InitHardwareMonitorMenu (HiiHandle, &gEfiIsaTemperatureDeviceProtocolGuid, TEMPERATURE_LABEL);
  InitHardwareMonitorMenu (HiiHandle, &gEfiIsaVoltagleDeviceProtocolGuid, VOLTAGE_LABEL);
  InitHardwareMonitorMenu (HiiHandle, &gEfiIsaFanSpeedDeviceProtocolGuid, FAN_SPEED_LABEL);  
#endif
//[-end-130225-IB10920016-add]//
//[-start-130611-IB11120016-add]//
  IpmiItemInit (HiiHandle);
//[-end-130611-IB11120016-add]//

  //[-start-131129-IB09740048-add]//
  CrSerialDevOptionInit (HiiHandle);
  //[-end-131129-IB09740048-add]//
  return Status;
}


/**
 Update Gbe PeiE Port Selection

 @param [in]   HiiHandle


**/
EFI_STATUS
UpdateGbePeiEPortSelect (
  IN  EFI_HII_HANDLE                      HiiHandle
  )
{
  EFI_STATUS                        Status;
  CHAR16                            NewString[10];
  UINTN                             Index;
  EFI_SETUP_UTILITY_PROTOCOL        *SetupUtility;
  CHIPSET_CONFIGURATION              *SetupVariable;
  INT64                             GbePeiEPort;

  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility);
  ASSERT_EFI_ERROR(Status);
  SetupVariable = (CHIPSET_CONFIGURATION *)(SetupUtility->SetupNvData);

  GbePeiEPort = SetupVariable->GbePeiEPortSelect + 1;
  Index = UnicodeValueToString (NewString, 0, GbePeiEPort, 0); 
  HiiSetString (HiiHandle, STRING_TOKEN(STR_PCIE_PORT_ASSIGNED_TO_LAN2), NewString, NULL);

  return Status;
}

//[-start-120816-IB05330371-add]//
EFI_STATUS
UpdateACPIDebugInfo (
  IN  EFI_HII_HANDLE                      HiiHandle
  )
{
  EFI_STATUS                        Status;
  CHAR16                            *StringBuffer;
  EFI_SETUP_UTILITY_PROTOCOL        *SetupUtility;
  CHIPSET_CONFIGURATION              *SetupVariable;
  STRING_REF                        ACPIDebugAddr;
  STRING_REF                        ACPIDebugAddrString;
  VOID                              *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL                *StartLabel;
  
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT ( StartOpCodeHandle != NULL );

  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = APCI_DEBUG_ADDRESS_LABEL;
  
  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility);
  ASSERT_EFI_ERROR(Status);
  SetupVariable = (CHIPSET_CONFIGURATION *)( SetupUtility->SetupNvData );
  StringBuffer = AllocateZeroPool (0x100);

  UnicodeSPrint ( StringBuffer, 0x100, L"0x%8x", SetupVariable->ACPIDebugAddr );
  ACPIDebugAddrString = HiiSetString ( HiiHandle, 0, StringBuffer, NULL );

  StringBuffer = HiiGetString ( HiiHandle, STRING_TOKEN (STR_APCI_DEBUG_ADDRESS_STRING), NULL );
  ACPIDebugAddr = HiiSetString ( HiiHandle, 0, StringBuffer, NULL );

  HiiCreateTextOpCode ( StartOpCodeHandle, ACPIDebugAddr, 0, ACPIDebugAddrString );

  HiiUpdateForm (
    HiiHandle,
    NULL,
    0x28,
    StartOpCodeHandle,
    NULL
    );

  gBS->FreePool ( StringBuffer );
  HiiFreeOpCodeHandle ( StartOpCodeHandle );
  return Status;
}
//[-end-120816-IB05330371-add]//
